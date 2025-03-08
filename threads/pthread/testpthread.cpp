#include <iostream>
#include <unistd.h>
#include <pthread.h>
#include <sys/types.h>
#include <signal.h>
#include <vector>
using namespace std;

#define SIZE 5

void* handler(void* args)
{
    //pthread_detach(pthread_self());
    string name = static_cast<const char*>(args);
    int cnt = 5;
    while(cnt--)
    {
        cout<<"this is a new thread "<<name<<endl;
        sleep(1);
    }
    return nullptr;
}

int main()
{
    pthread_t tid;
    pthread_create(&tid,nullptr,handler,(void*)"thread1");
    pthread_detach(tid);
    cout<<"main thread wait"<<endl;
    pthread_join(tid,nullptr);
    cout<<"main thread wait sucess"<<endl;
    while(true)
    {
        sleep(1);
    }
    return 0;
}




// class Task
// {   
// public:
//     Task(int x,int y):datex(x),datey(y)
//     {}
//     int work()
//     {
//         return datex + datey;
//     }
//     ~Task()
//     {}
// public:
//     int datex;
//     int datey;
// };

// class Date:public Task
// {
// public:
//     Date(int x,int y,string threadname):Task(x,y),threadName(threadname)
//     {}

//     ~Date()
//     {}
//     string getThreadName()
//     {
//         return threadName;
//     }
// private:
//     string threadName;
// };

// void* handler(void*args)
// {
//     Date* date = (Date*)args;
//     cout<<date->getThreadName()<<" ;result is: "<<date->work()<<endl;
//     sleep(5);
//     return (void*)1;
// }

// int main()
// {
//     vector<pthread_t>threads;

//     for(int i= 0;i<SIZE;i++)
//     {
//         pthread_t tid = 0;
//         pthread_create(&tid,nullptr,handler,new Date(i,i+1,"thread-" + to_string(i+1)));
//         threads.push_back(tid);
//         sleep(1);
//     }
//     return 0;
// }



// int g_val = 100;

// void *newthread(void *args)
// {
//     int cnt = 5;
//     while (cnt--)
//     {
//         sleep(1);
//         // cout<<"new thread"<<endl;
//         printf("new thread: g_val: %d,&g_val: %p\n",g_val,&g_val);
//         g_val++;
//     }
//     pthread_exit((void*)123);

// }

// int main()
// {
//     pthread_t tid;
//     pthread_create(&tid, nullptr, newthread, nullptr);
//     // int cnt = 5;
//     // //两个线程的资源是共享的
//     // while (cnt--)
//     // {
//     //     sleep(1);
//     //     //cout << "main thread" << endl;
//     //     printf("mian thread: g_val: %d,&g_val: %p\n",g_val,&g_val);
//     // }
    
//     // //1.等待线程退出，并且获取其返回值
//     // void* ret = nullptr;
//     // pthread_join(tid,&ret);
//     // cout<<"new thread quit,ret val is:"<<(long long)ret<<endl;
//     // cnt = 5;
//     // while(cnt--)
//     // {
//     //     sleep(1);
//     //     //cout << "main thread" << endl;
//     //     printf("mian thread: g_val: %d,&g_val: %p\n",g_val,&g_val);
//     // }
//     //2.使用pthread_exit获取新线程退出信息
//     // void *ret = nullptr;
//     // int n = pthread_join(tid,&ret);
//     // cout<<"new thread exit,ret val is "<<(long long)ret<<endl;
    
//     //3.使用pthread_cancel取消线程,成功取消，推出码为-1，及PTHREAD_CANCEL
//     sleep(2);
//     pthread_cancel(tid);
//     void *ret = nullptr;
//     int n = pthread_join(tid,&ret);
//     cout<<"new thread exit,ret val is "<<(long long)ret<<endl;

//     return 0;
// }

// void* newthread(void* args)
// {
//     int a = 1;
//     char b = 'a';
//     double c = 1.1;

//     int* pint = &a;
//     char* pchar = &b;
//     double* pdouble = &c;

//     printf("new thread ---> &a: %p  &b: %p &c: %p \n",pint,pchar,pdouble);
//     while(true)
//     {
//         sleep(1);
//     }
// }

// int main()
// {
//     pthread_t tid;
//     signal(SIGCHLD,SIG_IGN);
//     pthread_create(&tid,nullptr,newthread,nullptr);
//     // pid_t id = fork();
//     // if(id == 0)
//     // {
//     //     int cnt=100;
//     //     while(cnt--)
//     //     {
//     //         sleep(1);
//     //     }
//     //     exit(1);
//     // }
//     // while(true)
//     // {
//     //     cout<<"I am main thread"<<endl;
//     //     sleep(1);
//     // }

//     int a = 1;
//     char b = 'a';
//     double c = 1.1;

//     int* pint = &a;
//     char* pchar = &b;
//     double* pdouble = &c;

//     printf("main thread ---> &a: %p  &b: %p &c: %p \n",pint,pchar,pdouble);

//     return 0;
// }
