#ifndef __TCPSERVER_HPP__
#define __TCPSERVER_HPP__

#include <unistd.h>
#include <functional>
#include <string>
#include <sys/types.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <memory>
#include <wait.h>

#include <signal.h>
#include "log.hpp"
#include "inetAddr.hpp"
#include "Socket.hpp"


using namespace ns_socket;

class TcpServer;
class ThreadData
{
public:
    ThreadData(int fd, InetAddr client, TcpServer *s) : sockfd(fd), clientAddr(client), server(s)
    {

    }

public:
    socketPtr sockfd;
    InetAddr clientAddr;
    TcpServer *server;
};

class TcpServer
{
public:
    TcpServer(uint16_t port) : _local("0",port), _isRuning(false),_listensock(std::make_unique<TcpSocket>())
    {
        _listensock->buildListenSocket(_local);
    }

    ~TcpServer()
    {
        if (_listensock > DEFAULTSOCKFD)
        {
            close(_listensock);
        }
    }

    void* HandlerSock(void *args)
    {
        pthread_detach(pthread_self());
        ThreadData* td = static_cast<ThreadData*>(args);
        td->server->service(td->sockfd,)
    }

    void loop()
    {
        _isRuning = true;
        
        while (_isRuning)
        {
            // 4.建立连接
            InetAddr peerAddr;
            socketPtr normalSock = _listensock->listenSocket(peerAddr);
            if(normalSock == nullptr)
            {
                LOG(ERROR"创建sockfd失败")
                continue;
            }
            pthread_t t;
            ThreadData *td = new ThreadData(normalSock,peerAddr,this);
            pthread_create(&t,nullptr,HandlerSock,td);//将线程分离

        }
        close(_listensock);
    }

    void service(std::unique_ptr<Socket> sockfd, InetAddr client)
    {
        std::string clientaddr = "[" + client.getIp() + ":" + std::to_string(client.getPort()) + "]#";
        LOG(INFO, "get a link %s:%d,fd:%d", client.getIp(), client.getPort(), sockfd);

        while (true)
        {
            char inbuffer[1024];
            // 在网络当中，如果n == 0，表示客户端退出并且关闭链接了
            // 当字节流较长的时候，可能导致不能一次性读完
            ssize_t r = read(sockfd, inbuffer, sizeof(inbuffer) - 1);

            if (r > 0)
            {
                inbuffer[r] = '\0';
                std::cout << clientaddr << inbuffer << std::endl;
                std::string resultString = _func(inbuffer);

                ssize_t s = send(sockfd, resultString.c_str(), resultString.size(), 0);
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
    InetAddr _local;
    std::unique_ptr<Socket> _listensock;
    bool _isRuning;
};

#endif
