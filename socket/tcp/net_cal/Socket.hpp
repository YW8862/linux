/*
 * @Author: YW8862 2165628227@qq.com
 * @Date: 2025-04-09 13:59:29
 * @LastEditors: YW8862 2165628227@qq.com
 * @LastEditTime: 2025-04-09 20:52:41
 * @FilePath: /yw/linux/socket/tcp/net_cal/Socket.hpp
 * @Description: 这是默认设置,请设置`customMade`, 打开koroFileHeader查看配置 进行设置: https://github.com/OBKoro1/koro1FileHeader/wiki/%E9%85%8D%E7%BD%AE
 */
#pragma once
#include <iostream>
#include <sys/types.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <memory>
#include "inetAddr.hpp"
#include "log.hpp"
#include "status.h"

// 模板方法模式
namespace ns_socket
{
    const int BACKLOG = 8;
    using socketPtr = std::shared_ptr<Socket>;
    class Socket
    {
    public:
        virtual void createSocket() = 0;
        virtual void bindSocket(InetAddr &addr) = 0;
        virtual void listenSocket() = 0;
        virtual socketPtr accepter(InetAddr &addr) = 0;
        virtual bool connector(InetAddr &addr) = 0;

    public:
        void buildListenSocket(InetAddr &addr)
        {
            createSocket();
            bindSocket(addr);
            listenSocket();
        }

        void buildClientSocket(InetAddr &addr)
        {
            createSocket();
            connector(addr);
        }
    };

    class TcpSocket : public Socket
    {
    public:
        TcpSocket(int fd) : _sockfd(fd)
        {
        }

        /**
         * @brief :创建流式套接字
         */
        void createSocket() override
        {
            _sockfd = ::socket(AF_INET, SOCK_STREAM, 0);
            if (_sockfd < 0)
            {
                LOG(FATAL, "sockfd create fail,sockfd:%d", _sockfd);
                exit(SOCKET_ERROR);
            }
            LOG(INFO, "套接字创建成功,sockfd:%d", _sockfd)
        }

        /**
         * @brief 绑定socket
         * @param addr:需要绑定的地址
         */
        void bindSocket(InetAddr &addr) override
        {
            // 2.绑定
            struct sockaddr_in local;

            bzero(&local, sizeof(local));
            local.sin_family = AF_INET;
            local.sin_port = htons(addr.grtPort());
            local.sin_addr.s_addr = inet_addr(addr.getIp().c_str());

            int n = ::bind(_sockfd, (const struct sockaddr *)&local, sizeof(local));
            if (n < 0)
            {
                LOG(FATAL, "bind fail");
                exit(BIND_ERROR);
            }
            else
            {
                LOG(INFO, "bind Sucess");
            }
        }

        void listenSocket() override
        {
            // 3.tcp是面向连接的，在通信之前，必须先建立连接
            //  等待客户端向服务端连接申请
            int n = ::listen(_sockfd, BACKLOG);
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

        socketPtr accepter(InetAddr &addr) override
        {
            // 4.建立连接
            struct sockaddr_in peer;
            socklen_t len = sizeof(peer);
            int sockfd = accept(_sockfd, (struct sockaddr *)&peer, &len);
            if (sockfd < 0)
            {
                LOG(ERROR, "accept error");
                return nullptr;
            }
            addr = peer;
            socketPtr sock = std::make_shared<TcpSocket>(sockfd);
            return sock;
        }

        bool connector(InetAddr &addr) override
        {
            // 配置连接服务端信息
            struct sockaddr_in server;
            server.sin_addr.s_addr = inet_addr(ip.c_str());
            server.sin_family = AF_INET;
            server.sin_port = htons(addr.getPort());
            socklen_t len = sizeof(server);

            int n = connect(sockfd, (const struct sockaddr *)&server, len);
            if (n < 0)
            {
                std::cerr << "connect error, " << strerror(errno) << std::endl;

                exit(CONNECT_ERROR);
                return false;
            }
            *addr = server;
            return true;
        }

    private:
        int _sockfd;
    };

};