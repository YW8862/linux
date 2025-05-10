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

#include "http.hpp"
#include "inetAddr.hpp"
#include "log.hpp"
#include "Socket.hpp"

using namespace socket_ns;

class TcpServer;

using http_service_t = std::function<std::string (std::string request)>;

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
    TcpServer(int port, http_service_t service) 
        : _localaddr("0", port),
          _listensock(std::make_unique<TcpSocket>()),
          _http_service(service),
          _isrunning(false)
    {
        _listensock->BuildListenSocket(_localaddr);
    }

    static void *HandlerSock(void *args)
    {
        pthread_detach(pthread_self());
        ThreadData *td = static_cast<ThreadData *>(args);
        
        std::string request,response;

        //有较大的概率是读到完整请求
        ssize_t n = td->sockfd->Recv(&request); 
        //解析Http请求
        //TO DO
         
        if(n>0)
        {
            //读取请求
            response = td->self->_http_service(request);
            //发送应答
            // if(td->sockfd->Send(response))
            // {
                td->sockfd->Close();        
            //}
        }

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

    http_service_t _http_service;
};