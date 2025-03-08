#include <iostream>
#include <string>
#include <cstdlib>
#include <unistd.h>
#include <sys/types.h>
#include <cstring>
#include <vector>
#include <sys/wait.h>   
#include "task.hpp"

using namespace std;
enum
{
    argvsError = 1,
    pipeError
};

class channel
{
public:
    //为通道初始化
    channel(int wfd,pid_t procid,string procname)
    :_wfd(wfd),_procid(procid),_procname(procname)
    {}
    ~channel()
    {}
    void print()
    {
        cout<<"_wfd is "<<_wfd;
        cout<<", _procid is "<<_procid;
        cout<<", _procname is "<<_procname<<endl;
    }
    
    string getProcName()&
    {
        return _procname;
    }
    int getWfd()
    {
        return _wfd;
    }
    pid_t getId()
    {
        return _procid;
    }
    //关闭写端fd
    void kill()
    {
        close(_wfd);
    }
private:
    int _wfd;
    pid_t _procid;
    string _procname;
};

class processPool
{
public:
    processPool(int processNum)
    :_processNum(processNum)
    {}

    ~processPool()
    {}
    
    //创建子进程
    int createProcess(work_t work)
    {
        vector<int>wfds;
        for(int num = 0;num<_processNum;num++)
        {   
            int pipefd[2] = {0};
            int n = pipe(pipefd);
            if(n < 0) return pipeError;

            pid_t id = fork();
            if(id == 0)
            {   
                //为防止子进程创建拷贝其他子进程的wfd，直接关闭每个子进程的wffd
                if(!wfds.empty())
                {
                    for(auto &wfd:wfds)
                    {
                        close(wfd);
                        cout<<"close wfd"<<wfd<<endl;
                    }
                    
                }
                //关闭子进程的wfd
                close(pipefd[1]);
                //TO DO
                dup2(pipefd[0],0);
                work(pipefd[0]);
                exit(0);
            }
            //组织子进程
            string pipename = "channel"+to_string(num);
            _channels.push_back(channel(pipefd[1],id,pipename));
            wfds.push_back(pipefd[1]);
            //关闭父进程读端
            close(pipefd[0]);
        }
        return 0;
    }
    //打印通道信息
    void debug()
    {
        for(auto &channel:_channels)
        {
            channel.print();
        }
    }
    //获取其中一个通道的下标
    int getChannel()
    {
        static int next = 0;
        int c = next++;
        c = c%_processNum;
        return c;
    }
    //发送任务编号至通道
    void sentTask(int index,uint32_t code)
    {
        cout<<"send Task:"<<code<<" to "<<_channels[index].getProcName()<<" process id is:"<<_channels[index].getId()<<endl;
        write(_channels[index].getWfd(),&code,sizeof(code));
    }
    //结束所有子进程
    void killAll()
    {
        for(auto &channel:_channels)
        {
            channel.kill();
            cout<<"process:"<<channel.getId()<<"close sucess"<<endl;
        }
    }
    //等待回收资源
    void wait()
    {
        for(auto &channel:_channels)
        {
            pid_t pid = channel.getId();
            pid_t rid = waitpid(pid,nullptr,0);
            if(pid == rid)
            {
                cout<<"wait:"<<pid<<" sucess...."<<endl;
            }
        }
    }
private:
    vector<channel> _channels;
    int _processNum;
};

void Usage(const string&proc)
{
    cout<<proc<<"格式错误"<<endl;
}


void controlTask(processPool* processpool,int cnt)
{
    while(cnt--)
    {
        //1.choose channel
        int c = processpool->getChannel();

        //2.choose task
        uint32_t code = nextTask();

        //3.sent task
        processpool->sentTask(c,code);
        sleep(1);
    }
}

//./mybin 5 
int main(int argc,char*argv[])
{
    if(argc != 2)
    {   
        Usage(argv[0]);
        return argvsError;
    }
    int processNum = stoi(argv[1]);
    if(processNum <=0 )return 1;

    //1.创建子进程
    processPool *processpool = new processPool(processNum);
    processpool->createProcess(worker);
    
    //2.控制子进程
    controlTask(processpool,10);
    cout<<"Tasks Done"<<endl;

    //3.回收子进程
    processpool->killAll();
    processpool->wait();
    return 0;
}