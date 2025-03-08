#ifndef __RINGQUEUE_HPP__
#define __RINGQUEUE_HPP__
#include <iostream>
#include <string>
#include <vector>
#include <semaphore.h>
using namespace std;
  
template <typename T>

class ringQueue
{
public:
    ringQueue(int cap) : _cap(cap), _ringQueue(cap), _productorIndex(0), _consumerIndex(0)
    {
        sem_init(&_roomSem, 0, cap);
        sem_init(&_dataSem, 0, 0);
        //初始化消费锁和生产锁
        pthread_mutex_init(&_productMutex,nullptr);
        pthread_mutex_init(&_consumeMutex,nullptr);
    }

    ~ringQueue()
    {
        sem_destroy(&_roomSem);
        sem_destroy(&_dataSem);
    }

    void enQueue(const T &data)
    {
        // 生产，P操作
        P(_roomSem);
        lock(_productMutex);
        _ringQueue[_productorIndex++] = move(data);
        _productorIndex %= _cap;
        unlock(_productMutex);
        V(_dataSem);
    }

    void pop(T &data)
    {
        P(_dataSem);
        lock(_consumeMutex);
        data = _ringQueue[_consumerIndex++];
        _consumerIndex %= _cap;
        unlock(_consumeMutex);
        V(_roomSem);
    }

private:
    void P(sem_t &sem)
    {
        sem_wait(&sem);
    }

    void V(sem_t &sem)
    {
        sem_post(&sem);
    }

    void lock(pthread_mutex_t &mutex)
    {
        pthread_mutex_lock(&mutex);
    }

    void unlock(pthread_mutex_t &mutex)
    {
        pthread_mutex_unlock(&mutex);
    }
private:
    vector<T> _ringQueue;
    int _cap;

    int _productorIndex;
    int _consumerIndex;

    // 定义信号量
    sem_t _roomSem;
    sem_t _dataSem;

    //定义生产消费线程死锁
    pthread_mutex_t _productMutex;
    pthread_mutex_t _consumeMutex;
};

#endif