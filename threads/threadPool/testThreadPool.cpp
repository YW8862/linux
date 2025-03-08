#include <iostream>
#include <string>
#include <unistd.h>
#include <memory>
#include <ctime>
#include "threadPool.hpp"
#include "thread.hpp"
#include "log.hpp"
#include "task.hpp"

using namespace std;

int main()
{
    LOGTOFILE();
    ThreadPool<task>::getInstance();
    ThreadPool<task>::getInstance()->stop();
    ThreadPool<task>::getInstance()->wait();
    return 0;
}









// void print()
// {
//     cout<<"hello world"<<endl;
// }
// int main()
// {
//     srand(time(nullptr)^getpid()^pthread_self());
//     //LOGTOSCREEN();
//     LOGTOFILE();
//     unique_ptr<ThreadPool<task>> tp = make_unique<ThreadPool<task>>(3);
//     tp->initQueue();
//     tp->start();
    
//     int taskNum = 10;
//     while(taskNum--)
//     {
//         int a = rand()%10 + 1;
//         usleep(1234);
//         int b = rand()%20 + 1;
//         task t(a,b);

//         LOG(INFO,"main thread create a task: %s",t.questionToString().c_str());
//         tp->enQueue(t);
//         sleep(1);
//     }

//     sleep(2);
//     tp->stop();
//     tp->wait();



//     // unique_ptr<ThreadPool<func_t>> tp = make_unique<ThreadPool<func_t>>(3);
//     // tp->initQueue();
//     // tp->start();
//     // sleep(2);
//     // tp->stop();
//     // tp->wait();
//     //logMessage(__FILE__,__LINE__,3,"hello world%s,%d,%c"," hahah",13,'a');
//     // LOG(3,"hello world%s,%d,%c"," hahah",13,'a');
//     // LOG(3,"hello world");
//     return 0;
// } 