#pragma once

#include <iostream>
#include <vector>
#include <set>
#include <string>
#include <algorithm>
#include <functional>
#include <unordered_map>
#include <sys/types.h>
#include <sys/socket.h>
#include "inetAddr.hpp"
#include "threadPool.hpp"
#include "task.hpp"

using task_t = std::function<void()>;

// 管理在线用户（基于IP:Port作为唯一标识）
class MessageRoute
{
public:
    MessageRoute() 
    {
        pthread_mutex_init(&_usersMutex,nullptr);
    }
    
    ~MessageRoute() {}

    void addUser(const InetAddr &user)
    {
        if (!isExists(user))
        {
            _users.push_back(user);
        }
    }

    void delUser(InetAddr user)
    {
        // 如果不存在，直接返回
        if (!isExists(user))
            return;
        LockGuard lockGuard(&_usersMutex);
        std::vector<InetAddr>::iterator it = _users.begin();
        while (it != _users.end() && *it != user)
        {
            it++;
        }
        _users.erase(it);
    }

    void routeHelper(int sockfd, std::string message, InetAddr sender)
    {
        LockGuard lockGuard(&_usersMutex);
        // 2.进行消息转发
        for (auto user : _users)
        {
            // 格式化消息
            std::string sendMessage = "[" + sender.getIp() + ":" + std::to_string(sender.getPort()) + "]#" + message;
            struct sockaddr_in clientAddr = user.getAddr();
            socklen_t len = sizeof(clientAddr);
            ::sendto(sockfd, sendMessage.c_str(), sendMessage.size(), 0, (struct sockaddr *)&clientAddr, len);
        }
    }

    void sendMesage(int sockfd, const std::string message, InetAddr sender)
    {
        // 1.插入用户
        addUser(sender);
        // 1.1用户输入QUIT，表示用户退出
        if (message == "QUIT")
        {
            delUser(sender);
        }
        // 构建任务对象
        task_t t = std::bind(&MessageRoute::routeHelper, this, sockfd, message, sender);
        ThreadPool<task_t>::getInstance()->enQueue(t);
    }

private:
    bool isExists(const InetAddr &user)
    {
        for (auto &_user : _users)
        {
            if (_user == user)
                return true;
        }
        return false;
    }

private:
    std::vector<InetAddr> _users;
    pthread_mutex_t _usersMutex;
};