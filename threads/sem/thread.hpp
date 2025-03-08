#ifndef __THREAD_HPP__
#define __THREAD_HPP__

#include <iostream>
#include <string>
#include <pthread.h>
#include <functional>
#include <unistd.h>
using namespace std;

// 封装thread类
namespace ThreadModule
{
    template <typename T>
    using func_t = function<void(T &, const string &)>;

    template <typename T>
    class Thread
    {
    public:
        Thread(func_t<T &> func, T &data, const string &name = "none-name")
            : _func(func), _data(data), _threadName(name), _stop(true)
        {
        }

        ~Thread()
        {
        }

        bool start()
        {
            int n = pthread_create(&_tid, nullptr, threadRoutine, this);
            if (!n)
            {
                _stop = false;
                return true;
            }
            else
            {
                return false;
            }
        }

        void detach()
        {
            if (!_stop)
            {
                pthread_detach(_tid);
            }
        }

        void join() const
        {
            if (!_stop)
            {
                pthread_join(_tid, nullptr);
                cout << "thread:" << _tid << " quit...." << endl;
            }
        }

        void excute()
        {
            _func(_data, _threadName);
        }

        void Stop()
        {
        }

        string getName()
        {
            return _threadName;
        }

    private:
        static void *threadRoutine(void *args)
        {
            Thread<T> *This = static_cast<Thread<T> *>(args);
            This->excute();
            return nullptr;
        }

    private:
        pthread_t _tid;
        string _threadName;
        T &_data; // 使所有的对象访问同一个数据
        func_t<T &> _func;
        bool _stop;
    };

    class ThreadData
    {
    public:
        ThreadData(int &tickets, string name, pthread_mutex_t &mutex) : _tickets(tickets), _name(name), _mutex(mutex), _total(0)
        {
        }
        ~ThreadData()
        {
        }

        int getTickets()
        {
            return _tickets;
        }

        int buyTickets()
        {
            cout << _name << " buy a tickets,left tickets: " << --_tickets << endl;
            _total++;
            return _tickets;
        }

        string &getName()
        {
            return _name;
        }

        int getTotal()
        {
            return _total;
        }

        void lock()
        {
            pthread_mutex_lock(&_mutex);
        }

        void unlock()
        {
            pthread_mutex_unlock(&_mutex);
        }
        pthread_mutex_t *getMutex()
        {
            return &_mutex;
        }

    private:
        int &_tickets;
        string _name;
        int _total;
        pthread_mutex_t &_mutex;
    };
}

#endif