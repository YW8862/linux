#include <iostream>
#include <pthread.h>
#include <unistd.h>
#include <vector>
#include <string>
#include "blockQueue.hpp"
#include "thread.hpp"
#include "task.hpp"
using namespace std;
using namespace ThreadModule;

using blockQueue_t = blockQueue<task>;

int gvalue = 100;
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

void print()
{
    cout<<"hello world"<<endl;
}

void consumer(blockQueue_t &bq)
{
    int cnt = 0;
    while (true)
    {
        task t;
        bq.pop(t);
        //t.excute();
        t();
        // pthread_mutex_lock(&mutex);
        // cout << t.resultToString() << endl;
        // pthread_mutex_unlock(&mutex);
        // sleep(1);
    }
}

void productor(blockQueue_t &bq)
{
    srand(time(nullptr));
    while (true)
    {
        // 1.获取任务
        // int a = rand() % 10 + 1;
        // usleep(1234);
        // int b = rand() % 20 + 1;
        // task t(a, b);
        // 2.将获取的任务放在队列中
        bq.enQueue(print);
        //pthread_mutex_lock(&mutex);
        //cout << t.questionToString() << endl;
        //pthread_mutex_unlock(&mutex);
        sleep(1);
        // sleep(4);
    }
}

void startCommon(vector<Thread<blockQueue_t &>> &threads, blockQueue_t &bq, func_t<blockQueue_t> func, int num = 1)
{
    for (int i = 0; i < num; i++)
    {
        string name = "thread - " + to_string(i + 1);
        threads.emplace_back(func, bq, name);
        threads.back().Start();
    }
}

void startConsumer(vector<Thread<blockQueue_t &>> &threads, blockQueue_t &bq, int num = 1)
{
    startCommon(threads, bq, consumer, num);
}

void startProductor(vector<Thread<blockQueue_t &>> &threads, blockQueue_t &bq, int num = 1)
{
    startCommon(threads, bq, productor, num);
}

void waitAllThread(const vector<Thread<blockQueue_t &>> &threads)
{
    for (const auto &thread : threads)
    {
        thread.Join();
    }
}

int main()
{
    blockQueue_t bq(10);
    vector<Thread<blockQueue_t &>> threads;
    // 1.开始消费者线程
    startConsumer(threads, bq, 2);
    // 2.开始生产者线程
    startProductor(threads, bq, 3);
    // 3.等待线程退出
    waitAllThread(threads);

    return 0;
}