#ifndef __TCPSERVER_HPP__
#define __TCPSERVER_HPP__

#include <sys/types.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include "log.hpp"

#define DEFAULTSOCKFD -1
#define BACKLOG 16

class TcpServer
{
public:
    TcpServer(uint16_t port):_port(port),_sockfd(DEFAULTSOCKFD)
    {

    }

    ~TcpServer()
    {

    }

    void init()
    {
        //1.创建流式套接字
        _sockfd = socket(AF_INET,SOCK_STREAM,0);
        if(_sockfd < 0)
        {
            LOG(FATAL,"sockfd create fail,sockfd:%d",_sockfd);
            exit(SOCKET_ERROR);
        }

        //2.绑定
        struct sockaddr_in local;
        
        bzero(&local,sizeof(local));
        local.sin_family = AF_INET;
        local.sin_port = htons(_port);
        local.sin_addr.s_addr = INADDR_ANY;

        int n = ::bind(_sockfd,(const struct sockaddr *)&local,sizeof(local));
        if(n < 0)
        {
            LOG(FATAL,"bind fail");
            exit(BIND_ERROR);
        }

        //3.tcp是面向连接的，在通信之前，必须先建立连接
        // 等待客户端向服务端连接申请
        n = ::listen(_sockfd,BACKLOG);
        if(n < 0)
        {
            LOG(FATAL,"listen error");
            exit(LISTEN_ERROR);
        }
    }

    void loop()
    {
        _isRuning = true;
        while(true)
        {
            //4.建立连接
            struct sockaddr_in peer;
            socklen_t len = sizeof(peer);
            int sockfd = ::accept(_sockfd)
        }
    }

private:
    uint16_t _port;
    int _sockfd;
    bool _isRuning;

};

#endif


