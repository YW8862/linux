#ifndef __TCPSERVER_HPP__
#define __TCPSERVER_HPP__

#include <unistd.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include "log.hpp"
#include "inetAddr.hpp"

#define DEFAULTSOCKFD -1
#define BACKLOG 16

class TcpServer
{
public:
    TcpServer(uint16_t port) : _port(port), _listensock(DEFAULTSOCKFD)
    {
    }

    ~TcpServer()
    {
        if(_listensock > DEFAULTSOCKFD)
        {
            close(_listensock);
        }
    }

    void init()
    {
        // 1.创建流式套接字
        _listensock = socket(AF_INET, SOCK_STREAM, 0);
        if (_listensock < 0)
        {
            LOG(FATAL, "sockfd create fail,sockfd:%d", _listensock);
            exit(SOCKET_ERROR);
        }

        // 2.绑定
        struct sockaddr_in local;

        bzero(&local, sizeof(local));
        local.sin_family = AF_INET;
        local.sin_port = _port;
        local.sin_addr.s_addr = INADDR_ANY;

        int n = ::bind(_listensock, (const struct sockaddr *)&local, sizeof(local));
        if (n < 0)
        {
            LOG(FATAL, "bind fail");
            exit(BIND_ERROR);
        }
        else
        {
            LOG(INFO, "bind Sucess");
        }

        // 3.tcp是面向连接的，在通信之前，必须先建立连接
        //  等待客户端向服务端连接申请
        n = ::listen(_listensock, BACKLOG);
        if (n < 0)
        {
            LOG(FATAL, "listen error");
            exit(LISTEN_ERROR);
        }
        else
        {
            LOG(INFO, "listen sucess");
        }
    }

    void loop()
    {
        _isRuning = true;
        while (_isRuning)
        {
            // 4.建立连接
            struct sockaddr_in peer;
            socklen_t len = sizeof(peer);
            int sockfd = accept(_listensock, (struct sockaddr *)&peer, &len);
            if (sockfd < 0)
            {
                LOG(ERROR, "accept error");
                continue;
            }
            else
            {
                LOG(INFO, "accept sucess");
            }
            service(sockfd, InetAddr(peer));
        }
    }

    void service(int sockfd, InetAddr client)
    {
        std::string clientaddr = "[" + client.getIp() + ":" + std::to_string(client.getPort()) + "]#";
        LOG(INFO, "get a link %s:%d", client.getIp(), client.getPort());
        while (true)
        {
            char inbuffer[1024];
            //在网络当中，如果n == 0，表示客户端退出并且关闭链接了
            ssize_t r = read(sockfd, inbuffer, sizeof(inbuffer) - 1);
            
            if (r > 0)
            {
                inbuffer[r] = '\0';
                std::cout<<clientaddr<<inbuffer<<std::endl;

                std::string echoString = "[server echo message]";
                echoString += inbuffer;
                ssize_t s = write(sockfd,echoString.c_str(),echoString.size());
                if(s > 0)
                {
                    LOG(INFO,"send a message to %s",clientaddr.c_str());
                }
            }
            else if(r == 0)
            {
                LOG(INFO,"%s quit...",clientaddr.c_str());
                break;
            }
            else
            {
                LOG(ERROR,"%s recv error...",clientaddr.c_str());
                break;
            }
        }
        //使用完毕必须及时关闭
        close(sockfd);
    }

private:
    uint16_t _port;
    int _listensock;
    bool _isRuning;
};

#endif
