#ifndef __THREADPOOL_HPP__
#define __THREADPOOL_HPP__
#include <iostream>
#include <vector>
#include <queue>
#include <pthread.h>
#include "thread.hpp"
#include "log.hpp"
#include "task.hpp"
#include "tcpServer.hpp"
#include "LockGuard.hpp"

#define DEFAULT_THREAD_NUM 4
using namespace std;
using namespace ThreadModule;

template <typename T>
class ThreadPool
{
public:

    ~ThreadPool()
    {
        pthread_mutex_destroy(&_mutex);
        pthread_cond_destroy(&_cond);
    }

    static ThreadPool<T> *getInstance()
    {
        //防止在创建线程池后反复上锁
        if (_instance == nullptr)
        {
            LockGuard lockGuard(&_instanceLock);
            //只有线程池未创建，才创建此线程池
            if (_instance == nullptr)
            {
                _instance = new ThreadPool<T>();
                _instance->initQueue();
                _instance->start();
                LOG(DEBUG,"threadPool created");
            }
        }
        LOG(DEBUG,"get threadPool");
        return _instance;
    }

    bool enQueue(const T &task)
    {
        bool ret = false;
        if (_isRuning == true)
        {
            lock();
            _tasks.push(task);
            if (_waitNum > 0)
            {
                wakeUp();
            }
            unlock();
            ret = true;
            LOG(INFO, "enQueue sucess...");
        }
        return ret;
    }

    void handlerTask(string name)
    {
        LOG(INFO, "%s is running", name.c_str());
        while (true)
        {
            // 1.先上锁
            lock();
            // 2.如果队列为空，且线程池在运行中,等待派发任务
            while (_tasks.empty() && _isRuning)
            {
                _waitNum++;
                threadSleep();
                _waitNum--;
            }
            // 如果线程池已经退出且队列为空
            if (_tasks.empty() && !_isRuning)
            {
                unlock();
                break;
            }
            // 如果线程在运行且队列不为空

            // 如果线程池已经退出，且任务队列不为空
            //  3.获取任务
            T t = _tasks.front();
            _tasks.pop();
            // 4.解锁
            unlock();
            LOG(INFO, "%s handler a task", name.c_str());
            // 5.执行任务
            // t.server->service(t.sockfd,t.clientAddr);
            t();
        }
    }
    
    void stop()
    {
        lock();
        _isRuning = false;
        wakeUpAll();
        unlock();
    }

    void wait()
    {
        for (auto &thread : _threads)
        {
            thread.join();
            LOG(INFO, "%s quit...", thread.getName().c_str());
        }
    }

private:

    void initQueue()
    {
        for (int num = 0; num < _threadNum; num++)
        {
            string threadName = "thread - " + to_string(num + 1);
            Thread thread(bind(&ThreadPool::handlerTask, this, placeholders::_1), threadName);
            _threads.emplace_back(move(thread));
            LOG(INFO, "init thread %s done", threadName.c_str());
        }
    }

    void start()
    {
        _isRuning = true;
        for (auto &thread : _threads)
        {
            thread.start();
            // cout<<"thread: "<<thread.getName()<<" start sucess"<<endl;
        }
    }

    ThreadPool(int threadNum = DEFAULT_THREAD_NUM) : _threadNum(threadNum), _waitNum(0), _isRuning(false)
    {
        pthread_mutex_init(&_mutex, nullptr);
        pthread_cond_init(&_cond, nullptr);
        LOG(INFO, "ThreadPool()");
    }

    // 禁用拷贝构造和等号运算符重载
    ThreadPool(const ThreadPool<T> &) = delete;
    ThreadPool<T> operator=(const ThreadPool<T> &) = delete;

    void lock()
    {
        pthread_mutex_lock(&_mutex);
    }

    void unlock()
    {
        pthread_mutex_unlock(&_mutex);
    }

    void threadSleep()
    {
        pthread_cond_wait(&_cond, &_mutex);
    }
    //唤醒一个线程
    void wakeUp()
    {
        pthread_cond_signal(&_cond);
    }
    //唤醒所有线程
    void wakeUpAll()
    {
        pthread_cond_broadcast(&_cond);
    }

private:
    int _threadNum;
    vector<Thread> _threads;
    queue<T> _tasks;
    //对队列上锁，实现互斥
    pthread_mutex_t _mutex;
    pthread_cond_t _cond;

    int _waitNum;
    bool _isRuning;
    // 用于获取线程池指针实现互斥
    static pthread_mutex_t _instanceLock;
    // 懒汉模式创建单例，初始化为nullptr
    static ThreadPool<T> *_instance;
    
};

template <typename T>
pthread_mutex_t ThreadPool<T>::_instanceLock = PTHREAD_MUTEX_INITIALIZER;

template <typename T>
ThreadPool<T> *ThreadPool<T>::_instance = nullptr;

#endif