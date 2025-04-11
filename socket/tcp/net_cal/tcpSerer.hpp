/*
 * @Author: YW8862 2165628227@qq.com
 * @Date: 2025-04-09 13:59:43
 * @LastEditors: YW8862 2165628227@qq.com
 * @LastEditTime: 2025-04-11 21:36:19
 * @FilePath: /yw/linux/socket/tcp/net_cal/tcpSerer.hpp
 * @Description: 这是默认设置,请设置`customMade`, 打开koroFileHeader查看配置 进行设置: https://github.com/OBKoro1/koro1FileHeader/wiki/%E9%85%8D%E7%BD%AE
 */

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
using io_service_t = std::function<void (socket_sptr sockfd, InetAddr client)>;

class ThreadData
{
public:
    ThreadData(socket_sptr fd, InetAddr addr, TcpServer *s):sockfd(fd), clientaddr(addr), self(s)
    {}
public:
    socket_sptr sockfd;
    InetAddr clientaddr;
    TcpServer *self;
};
class TcpServer
{
public:
    TcpServer(int port, io_service_t service) 
        : _localaddr("0", port),
          _listensock(std::make_unique<TcpSocket>()),
          _service(service),
          _isrunning(false)
    {
        _listensock->BuildListenSocket(_localaddr);
    }

    static void *HandlerSock(void *args)
    {
        pthread_detach(pthread_self());
        ThreadData *td = static_cast<ThreadData *>(args);
        td->self->_service(td->sockfd, td->clientaddr);
        ::close(td->sockfd->SockFd()); // 文件描述符泄漏
        delete td;
        return nullptr;
    }
    
    void Loop()
    {
        _isrunning = true;
        // 4. 不能直接接受数据，先获取连接
        while (_isrunning)
        {
            InetAddr peeraddr;
            socket_sptr normalsock = _listensock->Accepter(&peeraddr);
            if(normalsock == nullptr) continue;

            pthread_t t;
            ThreadData *td = new ThreadData(normalsock, peeraddr, this);
            pthread_create(&t, nullptr, HandlerSock, td); //将线程分离
        }
        _isrunning = false;
    }
    ~TcpServer()
    {
    }

private:
    InetAddr _localaddr;
    std::unique_ptr<Socket> _listensock;
    bool _isrunning;

    io_service_t _service;
};

#endif