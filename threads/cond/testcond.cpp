#include <pthread.h>
#include <iostream>
#include <pthread.h>
#include <string>
#include <vector>
#include <unistd.h>

using namespace std;

pthread_cond_t gcond = PTHREAD_COND_INITIALIZER;    //条件变量
pthread_mutex_t gmutex = PTHREAD_MUTEX_INITIALIZER; //互斥锁


// master核心代码
void *masterCore(void *args)
{
    string name = static_cast<const char *>(args);
    while (true)
    {  
        pthread_cond_signal(&gcond);
        sleep(1);
    }
}

void *slaverCore(void *args)
{
    string name = static_cast<const char *>(args);
    while (true)
    {
        //1.加锁
        pthread_mutex_lock(&gmutex);
        pthread_cond_wait(&gcond,&gmutex);
        cout<<name<<endl;
        //2.解锁
        pthread_mutex_unlock(&gmutex);
    }
}

void startMaster(vector<pthread_t> &tids)
{
    pthread_t tid;
    int n = pthread_create(&tid, nullptr, masterCore, (void *)"Master Thread");
    if (n == 0)
    {
        cout << "create master thread sucess" << endl;
        tids.emplace_back(tid);
    }
}

void startSlaver(vector<pthread_t> &tids, size_t threadCount = 3)
{
    for (int i = 0; i < threadCount; i++)
    {
        char *name = new char(64);
        snprintf(name, 64, "thread-%d" ,i+1);

        pthread_t tid;
        int n = pthread_create(&tid, nullptr, slaverCore, (void *)name);
        if (n == 0)
        {
            cout << "create slaver thread sucess "<<name << endl;
            tids.emplace_back(tid);
        }
    }
}

void waitThread(vector<pthread_t> &tids)
{
    for (auto &tid : tids)
    {
        pthread_join(tid, nullptr);
    }
}

int main()
{
    vector<pthread_t> tids;
    // 1.创建管理线程
    startMaster(tids);
    // 2.创建劳动线程
    startSlaver(tids,5);
    // 3.等待线程结束
    waitThread(tids);
    return 0;
}