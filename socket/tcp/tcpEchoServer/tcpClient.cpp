#include <iostream>
#include <cstring>
#include <string>
#include <cstdlib>
#include <memory>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include "status.h"
#include "log.hpp"

void Usage(std::string proc)
{
    std::cout << "Usage:\n\t" << proc << "local_ip local_port" << std::endl;
}

// ./tcpClient ip port
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

    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0)
    {
        LOG(FATAL, "client socket create fail:%s,errno:%d", strerror(errno), errno);
        exit(SOCKET_ERROR);
    }

    // 配置连接服务端信息
    struct sockaddr_in server;
    server.sin_addr.s_addr = inet_addr(ip.c_str());
    server.sin_family = AF_INET;
    server.sin_port = htons(port);
    socklen_t len = sizeof(server);

    int n = connect(sockfd, (const struct sockaddr *)&server, len);
    if (n < 0)
    {
        std::cerr << "connect error, " << strerror(errno) << std::endl;

        exit(CONNECT_ERROR);
    }

    while (true)
    {
        std::cout << "please Enter#";
        std::string inString;
        std::getline(std::cin, inString);

        ssize_t s = send(sockfd, inString.c_str(), inString.size(), 0);
        if (s > 0)
        {
            char buffer[1024];
            ssize_t r = recv(sockfd, buffer, sizeof(buffer) - 1, 0);
            if (r > 0)
            {
                buffer[r] = '\0';
                std::cout << buffer << std::endl;
            }
            else if (r < 0)
            {
                break;
            }
        }
        else
        {
            break;
        }
    }

    close(sockfd);
    return 0;
}
