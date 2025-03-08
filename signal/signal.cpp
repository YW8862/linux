#include <stdio.h>
#include <unistd.h>
#include <iostream>
#include <signal.h>
#include <cstring>
#include <sys/wait.h>
#include <sys/types.h>
using namespace std;

// void waitsubproc(int signo)
// {
//     while (true)
//     {
//         pid_t wid = waitpid(-1, nullptr, WNOHANG);
//         if (wid <= 0)
//         {
//             break;
//         }
//         else
//         {
//             cout << "wait sucess" << endl;
//         }
//     }
// }

int main()
{
    //signal(SIGCHLD, waitsubproc);
    signal(SIGCHLD, SIG_IGN);
    pid_t id;
    for (int i = 0; i < 20; i++)
    {
        pid_t id = fork();
        if (id == 0)
        {
            cout<<"create proc sucess"<<endl;
            int cnt = 5;
            while (cnt--)
            {
                sleep(1);
            }
            cout << "subprocess exit" << endl;
            exit(0);
        }
    }

    while (true)
    {
        sleep(1);
    }
}

// void print(sigset_t&pending)
// {
//     for(int sig = 31;sig>0;sig--)
//     {
//         if(sigismember(&pending,sig))
//         {
//             cout<<"1";
//         }
//         else
//         {
//             cout<<"0";
//         }
//     }
//     cout<<endl;
// }

// void handler(int signo)
// {
//     cout<<"signo: "<<signo<<endl;
//     sigset_t pending;
//     while(true)
//     {
//         //每隔1s打印一次未决信号集
//         sigpending(&pending);
//         print(pending);
//         sleep(1);
//     }
// }

// int main()
// {
//     struct sigaction act,oact;
//     act.sa_handler = handler;
//     //创建屏蔽信号集
//     sigset_t mask;
//     sigemptyset(&mask);
//     sigaddset(&mask,3);
//     sigaddset(&mask,4);
//     sigaddset(&mask,5);
//     act.sa_mask = mask;
//     //设置自定义响应和屏蔽信号集
//     sigaction(2,&act,&oact);

//     while(true);
//     return 0;
// }

// int main()
// {
//     pid_t id = fork();

//     //子进程阻塞所有信号，并且打印未决信号集
//     if(id == 0)
//     {
//         //创建两个信号集
//         sigset_t block,oblock;
//         //初始化清零
//         sigemptyset(&block);
//         sigemptyset(&oblock);

//         //阻塞所有信号
//         for(int i=1;i<=31;i++)
//         {
//             sigaddset(&block,i);
//         }
//         //写入阻塞信号集
//         sigprocmask(SIG_BLOCK,&block,&oblock);

//         sigset_t pending;
//         sigemptyset(&pending);
//         while(true)
//         {
//             //每隔一秒获取一次未决信号集
//             sleep(1);
//             sigpending(&pending);
//             print(pending);
//         }
//     }

//     //挂载2s，防止子进程信号阻塞还未完成提前被父进程发送的信号终止
//     sleep(2);
//     //父进程给子进程发送除9，19外的所有信号
//     for(int sig=1;sig<=31;sig++)
//     {
//         if(9!=sig && 19!=sig)
//         {
//             kill(id,sig);
//         }
//         sleep(1);
//     }
//     //等待50s，直接终止子进程
//     sleep(10);
//     kill(id,9);
//     int wstatus;
//     waitpid(id,&wstatus,0);
//     return 0;
// }

// int main()
// {
//     int a = 1;
//     a/= 0;
//     while(1)
//     {
//         sleep(1);
//     }
// }

// unsigned int cnt = 0;
// void handler(int signo)
// {
//     cout<<"signo is: "<<signo<<" ,cnt is: "<<cnt<<endl;
// }

// int main()
// {
//     int cnt = 0;
//     alarm(5);
//     while(true)
//     {
//         cout<<++cnt<<endl;
//         sleep(1);
//         if(cnt == 3)
//         {
//             int ret = alarm(0);
//             cout<<"ret is: "<<ret<<endl;
//             break;
//         }
//     }
// }

// void handler(int signo)
// {
//     cout<<"sigNum is "<<signo<<endl;
// }

// int main()
// {
//     signal(SIGINT,handler);
//     while(1)
//     {
//         cout<<"hello world"<<endl;
//         sleep(1);
//     }
//     return 0;
// }
