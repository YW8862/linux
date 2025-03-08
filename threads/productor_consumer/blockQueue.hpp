#ifndef __BLOCKQUEUE_HPP__
#define __BLOCKQUEUE_HPP__
#include <vector>
#include <string>
#include <queue>
#include <pthread.h>
using namespace std;

template <class T>
class blockQueue
{
public:
    blockQueue(int cap) : _cap(cap),_productorWaitNum(0),_consumerWaitNum(0)
    {
        pthread_mutex_init(&_mutex, nullptr);
        pthread_cond_init(&_productCond, nullptr);
        pthread_cond_init(&_consumeCond, nullptr);
    }
    
    ~blockQueue()
    {
        pthread_mutex_destroy(&_mutex);
        pthread_cond_destroy(&_productCond);
        pthread_cond_destroy(&_consumeCond);
    }

    bool isFull() const
    {
        return _blockQueue.size() >= _cap;
    }

    bool isEmpty() const
    {
        return _blockQueue.empty();
    }

    size_t getSize()
    {
        return _blockQueue.size();
    }

    void enQueue(const T in)  // 生产者接口
    {
        pthread_mutex_lock(&_mutex);
        // 如果阻塞队列满了，则让其在指定的等待队列中去等待
        // 在等待之前，会自动释放当前上的锁
        while (isFull())//为了增强程序健壮性，使用while，不得使用if
        {
            _productorWaitNum++;
            pthread_cond_wait(&_productCond, &_mutex);
            _productorWaitNum--;
        }
        // 队列未满，生产产品
        _blockQueue.push(move(in));
        //生产完商品，通知消费者
        if(_consumerWaitNum>0)
        {
            pthread_cond_signal(&_consumeCond);
        }
        pthread_mutex_unlock(&_mutex);
    }

    void pop(T& out) // 消费者接口
    {
        pthread_mutex_lock(&_mutex);
        //队列为空，等待并释放锁
        while(isEmpty())//为了增强程序健壮性，使用while，不得使用if
        {
            _consumerWaitNum++;
            pthread_cond_wait(&_consumeCond,&_mutex);
            _consumerWaitNum++;
        }
        //进行消费
        out = _blockQueue.front();
        _blockQueue.pop();
        //消费完商品，通知生产者
        if(_productorWaitNum>0)
        {
            pthread_cond_signal(&_productCond);
        }
        pthread_mutex_unlock(&_mutex);
    }

private:
    queue<T> _blockQueue; // 阻塞队列
    size_t _cap;

    pthread_mutex_t _mutex;
    pthread_cond_t _productCond;
    pthread_cond_t _consumeCond;

    int _consumerWaitNum;
    int _productorWaitNum;
};

#endif