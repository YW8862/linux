#pragma once
#include <iostream>
#include <unistd.h>
#include <vector>
using namespace std;

typedef void(*work_t)(int fd);
typedef void(*task_t)(int fd,pid_t id);

void task1(int fd,pid_t id)
{
    cout<<"process pid:"<< id <<" from fd:"<< fd <<" this is task1\n"<<endl;
}
void task2(int fd,pid_t id)
{
   cout<<"process pid:"<< id <<" from fd:"<< fd <<" this is task2\n"<<endl;
}

void task3(int fd,pid_t id)
{
    cout<<"process pid:"<< id <<" from fd:"<< fd <<" this is task3\n"<<endl;
}
void task4(int fd,pid_t id)
{
    cout<<"process pid:"<< id <<" from fd:"<< fd <<" this is task4\n"<<endl;
}

vector<task_t> tasks = {task1,task2,task3,task4};

uint32_t nextTask()
{
    static uint32_t nextCode = 0;
    uint32_t taskCode = nextCode++;
    taskCode = taskCode % tasks.size();
    return taskCode;
}

void worker(int fd)
{
    while(true)
    {   
        uint32_t conmmand_code = 0;
        ssize_t n = read(0,&conmmand_code,sizeof(conmmand_code));
        if(n == sizeof(conmmand_code))
        {
            if(conmmand_code >= tasks.size()) continue;
            tasks[conmmand_code](fd,getpid());
        }
        else if(n == 0)
        {
            break;
            cout<<"process"<<getpid()<<"quit....."<<endl;
        }
        sleep(1);
    }
}