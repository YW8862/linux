#ifndef __TCPSERVER_HPP__
#define __TCPSERVER_HPP__

#include <unistd.h>
#include <functional>
#include <sys/types.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <wait.h>
#include <signal.h>
#include "log.hpp"
#include "inetAddr.hpp"
#include "threadPool.hpp"

#define DEFAULTSOCKFD -1
#define BACKLOG 16

using task_t = std::function<void()>;

class TcpServer;

// class ThreadData
// {
// public:
//     ThreadData(int fd, InetAddr client, TcpServer *s) : sockfd(fd), clientAddr(client), server(s)
//     {
//     }

// public:
//     int sockfd;
//     InetAddr clientAddr;
//     TcpServer *server;
// };

class TcpServer
{
public:
    TcpServer(uint16_t port) : _port(port), _listensock(DEFAULTSOCKFD)
    {
        
    }

    ~TcpServer()
    {
        if (_listensock > DEFAULTSOCKFD)
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

    // static void *handlerSock(void *args)
    // {
    //     //分离线程，避免主线程阻塞式等待
    //     pthread_detach(pthread_self());
    //     ThreadData* threadData = static_cast<ThreadData*>(args);
    //     threadData->server->service(threadData->sockfd,threadData->clientAddr);
    //     return nullptr;
    // }

    void loop()
    {
        _isRuning = true;
        //ThreadPool<ThreadData>* threadpool = ThreadPool<ThreadData>::getInstance();
        ThreadPool<task_t>* threadpool = ThreadPool<task_t>::getInstance();
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
            // service(sockfd, InetAddr(peer));

            // V1多进程版本
            // // 创建子进程用于完成通信任务
            // pid_t id = fork();
            // if (id == 0)
            // {
            //     // 子进程不关心监听套接字，直接关闭即可
            //     ::close(_listensock);
            //     if (fork() > 0)
            //         exit(0);
            //     service(sockfd, InetAddr(peer)); // 孙子进程变为孤儿进程，被init进程领养
            //     exit(0);
            // }
            // // 父进程
            // ::close(sockfd);
            // waitpid(id, nullptr, 0);

            // V2多线程版本
            // pthread_t tid;
            // ThreadData* data = new ThreadData(sockfd,InetAddr(peer),this);
            // pthread_create(&tid,nullptr,handlerSock,(void*)data);

            // V3线程池版本
            task_t task = std::bind(&TcpServer::service,this,sockfd,InetAddr(peer));
            // threadpool->enQueue(ThreadData(sockfd,InetAddr(peer),this));
            threadpool->enQueue(task);
        }
        close(_listensock);
    }

    void service(int sockfd, InetAddr client)
    {
        std::string clientaddr = "[" + client.getIp() + ":" + std::to_string(client.getPort()) + "]#";
        LOG(INFO, "get a link %s:%d,fd:%d", client.getIp(), client.getPort(), sockfd);

        while (true)
        {
            char inbuffer[1024];
            // 在网络当中，如果n == 0，表示客户端退出并且关闭链接了
            ssize_t r = read(sockfd, inbuffer, sizeof(inbuffer) - 1);

            if (r > 0)
            {
                inbuffer[r] = '\0';
                std::cout << clientaddr << inbuffer << std::endl;

                std::string echoString = "[server echo message]";
                echoString += inbuffer;
                ssize_t s = write(sockfd, echoString.c_str(), echoString.size());
                if (s > 0)
                {
                    LOG(INFO, "send a message to %s", clientaddr.c_str());
                }
            }
            else if (r == 0)
            {
                LOG(INFO, "%s quit...", clientaddr.c_str());
                break;
            }
            else
            {
                LOG(ERROR, "%s recv error...", clientaddr.c_str());
                break;
            }
        }

        // 使用完毕必须及时关闭
        close(sockfd);
    }

private:
    uint16_t _port;
    int _listensock;
    bool _isRuning;
};

#endif
