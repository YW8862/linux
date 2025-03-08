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
#include "dict.hpp"

const static int defaultfd = -1;
using func_t = std::function<std::string(const std::string &)>;

class UdpServer
{
public:
    UdpServer(uint16_t port,func_t func) : _port(port), _func(func), _sockfd(defaultfd), _isRunning(false)
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
        // local.sin_addr.s_addr = inet_addr(_ip.c_str());
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
        // std::cout<<_port<<" "<<_sockfd<<std::endl;
        _isRunning = true;
        while (true)
        {
            // sleep(1);
            LOG(INFO, "socket is runing");
            // UDP是面向数字报，需要使用recvfrom从指定的套接字读取数据
            // 收数据，并且将数据发回
            char request[1024];
            struct sockaddr_in peer;
            bzero(&peer, sizeof(peer));
            socklen_t len = sizeof(peer);
            // 1.让server先收数据
            // std::cout<<"hahah"<<std::endl;
            // 阻塞
            ssize_t n = recvfrom(_sockfd, request, sizeof(request) - 1, 0, (struct sockaddr *)&peer, &len);
            std::cout << "receive " << n << " bytes" << std::endl;
            if (n > 0)
            {
                //将ip和port转换，ip转为点分十进制字符串形式，port转为本机序
                InetAddr inetAddr(peer);
                request[n] = '\0';
                LOG(INFO, "socket receive a word");

                LOG(INFO,"Server receive a word from %s:%d %s",inetAddr.getIp(),inetAddr.getPort(),buffer);
                
                std::string response = _func(request);
                //发送回调结果
                sendto(_sockfd, response.c_str(), response.size(), 0, (const struct sockaddr *)&peer, len);
                LOG(INFO, "send translation sucess");
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

    //设置回调指针
    func_t _func;
};




// class UdpServer
// {
// public:
//     UdpServer(uint16_t port) : _port(port), _sockfd(defaultfd), _isRunning(false)
//     {
//     }

//     ~UdpServer()
//     {
//     }

//     void initServer()
//     {
//         _sockfd = socket(AF_INET, SOCK_DGRAM, 0);
//         if (_sockfd < 0)
//         {
//             LOG(FATAL, "socket create fail:%s,errno:%d", strerror(errno), errno);
//             exit(SOCKET_ERROR);
//         }
//         LOG(INFO, "socket create sucess");

//         struct sockaddr_in local;
//         bzero(&local, sizeof(local));
//         // 配置通信域
//         local.sin_family = AF_INET;
//         // 配置端口号
//         local.sin_port = htons(_port);
//         // 1.将点分十进制的ip地址转为4字节的ip地址
//         // 2.主机序转为网络序 -> htonl
//         // local.sin_addr.s_addr = inet_addr(_ip.c_str());
//         local.sin_addr.s_addr = INADDR_ANY;

//         int n = bind(_sockfd, (const struct sockaddr *)&local, sizeof(local));
//         if (n < 0)
//         {
//             LOG(FATAL, "socket bind fail:%s,errno:%d", strerror(errno), errno);
//             exit(BIND_ERROR);
//         }
//         LOG(INFO, "socket init sucess");
//     }

//     void start()
//     {
//         // std::cout<<_port<<" "<<_sockfd<<std::endl;
//         _isRunning = true;
//         while (true)
//         {
//             // sleep(1);
//             LOG(INFO, "socket is runing");
//             // UDP是面向数字报，需要使用recvfrom从指定的套接字读取数据
//             // 收数据，并且将数据发回
//             char buffer[1024];
//             struct sockaddr_in peer;
//             bzero(&peer, sizeof(peer));
//             socklen_t len = sizeof(peer);
//             // 1.让server先收数据
//             // std::cout<<"hahah"<<std::endl;
//             // 阻塞
//             ssize_t n = recvfrom(_sockfd, buffer, sizeof(buffer) - 1, 0, (struct sockaddr *)&peer, &len);
//             std::cout << "receive " << n << " bytes" << std::endl;
//             if (n > 0)
//             {
//                 //将ip和port转换，ip转为点分十进制字符串形式，port转为本机序
//                 InetAddr inetAddr(peer);
//                 buffer[n] = '\0';
//                 LOG(INFO, "socket receive a word");
//                 // std::cout << "Server receive a word from " << inetAddr.getIp() + ":" + std::to_string(inetAddr.getPort())
//                 //           << " " << buffer << std::endl;
//                 LOG(INFO,"Server receive a word from %s:%d %s",inetAddr.getIp(),inetAddr.getPort(),buffer);
//                 std::string english = _dict.translate(buffer);
                
//                 sendto(_sockfd, english.c_str(), english.size(), 0, (const struct sockaddr *)&peer, len);
//                 LOG(INFO, "send translation sucess");
//             }
//             else
//             {
//                 LOG(ERROR, "receive fail");
//             }
//         }
//         _isRunning = false;
//     }

// private:
//     int _sockfd;
//     uint16_t _port;
//     bool _isRunning;
//     dict _dict;
//     // std::string _ip;
// };