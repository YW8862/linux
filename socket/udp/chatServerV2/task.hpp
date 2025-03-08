#ifndef __TASK_HPP__
#define __TASK_HPP__
#include <string>
#include <iostream>
#include <functional>
#include "inetAddr.hpp"
#include <arpa/inet.h>
#include <strings.h> // for bzero
using namespace std;

class task
{
public:
    // 构造函数接受发送者、消息、目标用户以及套接字描述符
    task(const InetAddr &sender, const std::string &message, const InetAddr &destUser, int sockfd)
        : _sender(sender), _message(message), _destUser(destUser), _sockfd(sockfd)
    {
    }

    void execute()
    {
        // 构造要发送的完整消息，例如 "192.168.1.100:1234 Hello everyone"
        std::string fullMessage = _sender.getIp() + ":" + std::to_string(_sender.getPort()) + " " + _message;

        // 根据目标用户的信息构造目标地址
        struct sockaddr_in target;
        bzero(&target, sizeof(target));
        target.sin_family = AF_INET;
        target.sin_port = htons(_destUser.getPort());
        inet_pton(AF_INET, _destUser.getIp().c_str(), &target.sin_addr);

        socklen_t len = sizeof(target);
        sendto(_sockfd, fullMessage.c_str(), fullMessage.size(), 0, (const struct sockaddr *)&target, len);
    }

    // 允许对象直接以函数形式调用，从而执行任务
    void operator()()
    {
        this->execute();
    }

private:
    std::string _message;
    InetAddr _destUser;
    int _sockfd;
    InetAddr _sender;
};

#endif
