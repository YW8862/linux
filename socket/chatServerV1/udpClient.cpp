#include <iostream>
#include <cstring>
#include <string>
#include <cstdlib>
#include <memory>
#include <pthread.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include "status.h"
#include "log.hpp"


// 监听信息，如果收到信息，输出信息
void *handler(void *args)
{
    int sockfd = *(int *)args;
    struct sockaddr_in peer;
    socklen_t len = sizeof(peer);
    bzero(&peer, sizeof(peer));
    char buffer[1024];
    while (true)
    {

        // 接收响应信息
        ssize_t n = recvfrom(sockfd, buffer, sizeof(buffer) - 1, 0, (struct sockaddr *)&peer, &len);

        if (n > 0)
        {
            buffer[n] = '\0';
            LOG(INFO, "receive response");
            std::cout << buffer << std::endl;
        }
    }
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

    int sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd < 0)
    {
        LOG(FATAL, "client socket create fail:%s,errno:%d", strerror(errno), errno);
        exit(SOCKET_ERROR);
    }

    // 客户端也需要绑定ip地址和端口，但是不需要显式地绑定
    // udpclient在首次发送数据的时候，操作系统会自动给客户端进行随机绑定端口号，为了防止客户端端口号冲突
    // 所以客户端可以直接通信即可

    struct sockaddr_in server;
    bzero(&server, sizeof(server));
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = inet_addr(ip.c_str());
    server.sin_port = htons(port);

    std::string message;
    pthread_t tid;
    //创建监听线程
    pthread_create(&tid,nullptr,handler,(void*)&sockfd);

    while (true)
    {
        std::cout << "Please enter message:";
        // 获取用户输入信息
        std::getline(std::cin, message);
        // 发送至服务端
        sendto(sockfd, message.c_str(), message.size(), 0, (struct sockaddr *)&server, sizeof(server));
    }
    return 0;
}
