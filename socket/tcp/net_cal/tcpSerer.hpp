#include <iostream>
#include <string>
#include <functional>
#include <sys/types.h> /* See NOTES */
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <cstring>
#include <pthread.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <memory>

#include "inetAddr.hpp"
#include "log.hpp"
#include "Socket.hpp"

using namespace socket_ns;

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