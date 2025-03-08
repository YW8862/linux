#include <iostream>
#include <cstring>
#include <string>
#include <cstdlib>
#include <memory>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <fcntl.h>
#include "status.h"
#include "log.hpp"
#include "thread.hpp"
#include "common.hpp"

using namespace ThreadModule;

void recvMessage(int sockfd,std::string name)
{
    //int fd = openDev("/dev/pts/2",O_WRONLY);
    while(true)
    {
        struct sockaddr_in peer;
        socklen_t len = sizeof(peer);
        char buffer[1024];
        ssize_t n = recvfrom(sockfd,buffer,sizeof(buffer)-1,0,(struct sockaddr *)&peer,&len);
        //std::cout<<"recv a message"<<std::endl;
        if(n > 0)
        {
            buffer[n] = '\0';
            // printf("%s|%s\n",name.c_str(),buffer);
            //write(fd,buffer,strlen(buffer));
            fprintf(stderr,"[%s]|%s\n",name.c_str(),buffer);
            std::cout<<std::endl;
        }
    }
}

void sendMessage(int sockfd,struct sockaddr_in &server,std::string name)
{
   
    std::string message;
    while (true)
    {
        std::cout << "Please enter message:";
        // 获取用户输入信息
        std::getline(std::cin, message);
        // 发送至服务端
        sendto(sockfd, message.c_str(), message.size(), 0, (struct sockaddr *)&server, sizeof(server));
    }
}

int InitClient(std::string &serverip, uint16_t port,struct sockaddr_in *server)
{
    int sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd < 0)
    {
        LOG(FATAL, "client socket create fail:%s,errno:%d", strerror(errno), errno);
        return -1;
    }

    // 客户端也需要绑定ip地址和端口，但是不需要显式地绑定
    // udpclient在首次发送数据的时候，操作系统会自动给客户端进行随机绑定端口号，为了防止客户端端口号冲突
    // 所以客户端可以直接通信即可
    memset(server,sizeof(server),0);
    server->sin_family = AF_INET;
    server->sin_addr.s_addr = inet_addr(serverip.c_str());
    server->sin_port = htons(port);

    return sockfd;
}

void Usage(std::string proc)
{
    std::cout << "Usage:\n\t" << proc << "local_ip local_port" << std::endl;
}

int main(int argc, char *argv[])
{
    if (argc != 3)
    {
        Usage(argv[0]);
        exit(USAGE_ERROR);
    }
    // 获取ip地址和端口号
    std::string ip = argv[1];
    uint16_t port = std::stoi(argv[2]);
    struct sockaddr_in server;
    int sockfd = InitClient(ip,port,&server);
    if(sockfd < 0)
    {
        LOG(FATAL,"sockfd create error");
        return -1;
    }

    func_t r = std::bind(&recvMessage,sockfd,placeholders::_1);
    func_t s = std::bind(&sendMessage,sockfd,server,placeholders::_1);

    Thread recvThread(r,"receive");
    Thread sendThread(s,"send");

    recvThread.start();
    sendThread.start();

    recvThread.join();
    sendThread.join();

    return 0;
}
