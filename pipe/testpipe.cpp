#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <stdio.h>
#include <iostream>
using namespace std;

void writer(int pipefd)
{
  const char message[128] = "hello world\n"; 
  write(pipefd,message,sizeof(message)+1);//向管道写数据
  sleep(4);
  cout<<message;
  cout<<"父进程写入"<<endl;
}

void reader(int pipefd)
{
  char message[1024];
  ssize_t size = read(pipefd,message,sizeof(message)-1);//从管道读数据
  (void)size;
  cout<<message<<endl;
  cout<<"子进程读取"<<endl;
  message[0] = '\0';
  sleep(1);
}

int main()
{
  int pipefd[2];    //存放文件描述符用于通信
  int stat = pipe(pipefd);
  if(stat == -1)    //创建失败，返回
    return -1;

  pid_t id = fork();//创建子进程

  if(id)
  {
    close(pipefd[0]);//关闭读端
    while(1)
    {
      writer(pipefd[1]);
    }
  }

  close(pipefd[1]);//关闭写端
  while(1)
  {
    reader(pipefd[0]);
  }
  return 0;

}
