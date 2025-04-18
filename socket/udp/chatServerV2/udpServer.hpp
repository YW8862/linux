#pragma once
#include <iostream>
#include <string>
#include <cstring>
#include <sys/types.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <cerrno>
#include <functional>
#include "log.hpp"
#include "status.h"
#include "inetAddr.hpp"
#include "messageRoute.hpp"

const static int defaultfd = -1;
using fun_t = std::function<void(int sockfd,const std::string message,InetAddr sender)>;

class UdpServer
{
public:
    UdpServer(uint16_t port,fun_t func) : _port(port),_func(func), _sockfd(defaultfd), _isRunning(false)
    {
    }

    ~UdpServer()
    {
    }

    void initServer()
    {
        _sockfd = socket(AF_INET, SOCK_DGRAM, 0);
        if (_sockfd < 0)
        {
            LOG(FATAL, "socket create fail:%s,errno:%d", strerror(errno), errno);
            exit(SOCKET_ERROR);
        }
        LOG(INFO, "socket create sucess");

        struct sockaddr_in local;
        bzero(&local, sizeof(local));
        // 配置通信域
        local.sin_family = AF_INET;
        // 配置端口号
        local.sin_port = htons(_port);
        // 1.将点分十进制的ip地址转为4字节的ip地址
        // 2.主机序转为网络序 -> htonl
        local.sin_addr.s_addr = INADDR_ANY;

        int n = bind(_sockfd, (const struct sockaddr *)&local, sizeof(local));
        if (n < 0)
        {
            LOG(FATAL, "socket bind fail:%s,errno:%d", strerror(errno), errno);
            exit(BIND_ERROR);
        }
        LOG(INFO, "socket init sucess");
    }

    void start()
    {
        _isRunning = true;
        while (true)
        {
            // sleep(1);
            LOG(INFO, "socket is runing");
            // UDP是面向数字报，需要使用recvfrom从指定的套接字读取数据
            // 收数据，并且将数据发回
            char message[1024];
            struct sockaddr_in peer;
            bzero(&peer, sizeof(peer));
            socklen_t len = sizeof(peer);
            // 1.让server先收数据
            ssize_t n = recvfrom(_sockfd, message, sizeof(message) - 1, 0, (struct sockaddr *)&peer, &len);
            std::cout << "receive " << n << " bytes" << std::endl;
            if (n > 0)
            {
                InetAddr inetAddr(peer);
                message[n] = '\0';
                
                
                LOG(INFO, "Server receive a word from %s:%d %s", inetAddr.getIp(), inetAddr.getPort(), message);
                // 将消息转发给除发送者以外的所有用户
                _func(_sockfd,message,inetAddr);

                LOG(INFO, "send translation success");
            }
            else
            {
                LOG(ERROR, "receive fail");
            }
        }
        _isRunning = false;
    }

private:
    int _sockfd;
    uint16_t _port;
    bool _isRunning;
    fun_t _func;
    //转发器
    MessageRoute _route;
};


