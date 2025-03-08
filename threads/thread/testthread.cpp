#include <iostream>
#include <unistd.h>
#include <pthread.h>
#include <thread>
#include <sys/types.h>
#include <signal.h>
#include <vector>
#include "thread.hpp"
#include "LockGuard.hpp"
using namespace std;
using namespace ThreadModule;

int g_tickets = 10000;
const int num = 4;

pthread_mutex_t gmutex = PTHREAD_MUTEX_INITIALIZER;

void route(ThreadData* data)
{
    while(true)
    {
        //创建对象时自动上锁，
        LockGuard guard(data->getMutex());
        if(data->getTickets() > 0)
        {
            usleep(100);
            data->buyTickets();
        }
        else
        {
            break;
        }
    }
}

// void route(ThreadData* data)
// {
//     while(true)
//     {
//         //1.若不对其进行保护，可能出现负数的情况,先加锁
//         data->lock();
//         if(data->getTickets() > 0)
//         {
//             usleep(100);
//             data->buyTickets();
//             //2.购票完成后解锁
//             data->unlock();
//         }
//         else
//         {
//             //2.购票完成后解锁
//             data->unlock();
//             break;
//         }
//     }
// }

int main()
{
    vector<Thread<ThreadData*>> threads;
    vector<ThreadData*>datas;
    //0.创建一把锁
    pthread_mutex_t mutex;
    pthread_mutex_init(&mutex,nullptr);
    // 1.创建一批线程
    for (int i = 0; i < num; i++)
    {
        string name = "thread- " + to_string(i + 1);
        ThreadData* threadData = new ThreadData(g_tickets,name,mutex);
        threads.emplace_back(route, threadData, name);
        datas.emplace_back(threadData);
    }

    // 2.执行一批线程
    for (auto &thread : threads)
    {
        thread.Start();
    }

    // 3.等待一批线程
    for (auto &thread : threads)
    {
        thread.Join();
    }

    //4.打印抢票信息
    for(auto& data:datas)
    {
        cout<<data->getName()<<" : "<<data->getTotal()<<endl;
        delete data;
    }
    //pthread_mutex_destroy(&gmutex);
    return 0;
}

// void print(int& data)
// {
//     while(true)
//     {
//         cout<<"this is new thread,data: "<<data<<endl;
//         sleep(1);
//     }
// }

// const int num = 10;

// int main()
// {
//     vector<Thread<int>>threads;

//     //1.创建一批线程
//     for(int i=0;i<num;i++)
//     {
//         threads.emplace_back(print,i+1,"new thread-" + to_string(i+1));
//     }

//     //2.执行一批线程
//     for(auto&thread:threads)
//     {
//         thread.Start();
//     }

//     //3.等待一批线程
//     for(auto&thread:threads)
//     {
//         thread.Join();
//     }
//     return 0;
// }

// int main()
// {
//     Thread<int> t1(print,10);
//     t1.start();
//     t1.Join();
//     return 0;
// }

// void handler(int num)
// {
//     cout<<"this is a new thread, num: "<<num<<endl;
// }

// int main()
// {
//     thread t1(handler,1);
//     thread t2(handler,2);
//     thread t3(handler,3);
//     thread t4(handler,4);

//     t1.join();
//     t2.join();
//     t3.join();
//     t4.join();

//     return 0;
// }