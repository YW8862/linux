#include <iostream>
#include <pthread.h>
#include <unistd.h>
#include <vector>
#include <string>
#include "thread.hpp"
#include "ringQueue.hpp"
#include "task.hpp"
using namespace std;
using namespace ThreadModule;

using ringQueue_t = ringQueue<Task>;

int gvalue = 0;
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

void consumer(ringQueue_t &rq, const string &name)
{
    int cnt = 0;
    Task task;
    while (true)
    {
        rq.pop(task);
        // rq.pop(cnt);
        pthread_mutex_lock(&mutex);
        cout << name << ": ";
        task();
        // cout << name << " consume data: " << cnt << endl;
        pthread_mutex_unlock(&mutex);
    }
}

void productor(ringQueue_t &rq, const string &name)
{
    while (true)
    {
        Task task = download;
        rq.enQueue(task);
        // rq.enQueue(gvalue);
        pthread_mutex_lock(&mutex);
        cout << name << " :product task" << endl;
        // cout << name << " product data: " << gvalue++ << endl;
        pthread_mutex_unlock(&mutex);
        sleep(1);
    }
}

// 初始化消费线程
void initComm(vector<Thread<ringQueue_t &>> &threads, ringQueue_t &rq, func_t<ringQueue_t> func, string name, int num = 1)
{
    for (int i = 0; i < num; i++)
    {
        string threadName = name + " - " + to_string(i + 1);
        Thread<ringQueue_t &> thread(func, rq, threadName);
        threads.emplace_back(move(thread));
    }
}

void initConsumer(vector<Thread<ringQueue_t &>> &threads, ringQueue_t &rq, int num = 1)
{
    initComm(threads, rq, consumer, "consumer", num);
}

// 初始化生产线程
void initProductor(vector<Thread<ringQueue_t &>> &threads, ringQueue_t &rq, int num = 1)
{
    initComm(threads, rq, productor, "productor", num);
}

void waitAllThread(const vector<Thread<ringQueue_t &>> &threads)
{
    for (const auto &thread : threads)
    {
        thread.join();
    }
}

// 开始所有线程
void startThread(vector<Thread<ringQueue_t &>> &threads)
{
    for (auto &thread : threads)
    {
        thread.start();
    }
}

int main()
{
    ringQueue_t rq(10);
    vector<Thread<ringQueue_t &>> threads;
    // 1.初始化消费者线程
    initConsumer(threads, rq, 3);
    // 2.初始化生产者线程
    initProductor(threads, rq, 3);
    // 3.开始所有线程
    startThread(threads);
    // 4.等待线程退出
    waitAllThread(threads);

    return 0;
}