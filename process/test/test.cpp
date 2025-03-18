#include <iostream>
#include <unistd.h>
#include <sys/types.h>


int main()
{
    pid_t id = fork();
   
    //id为0，子进程
    if(id == 0)
    {
        int cnt = 5;
        while(cnt--)
        {
            sleep(1);
            std::cout<<"this is child process"<<std::endl;
        }
        exit(1);
    }
   while(true)
   {
        sleep(1);
        std::cout<<"this is father process";
   }
    return 0;
}
