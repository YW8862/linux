#pragma once

#include <iostream>
#include <vector>
#include <string>
#include <algorithm>
#include <unordered_map>
#include "inetAddr.hpp"
#include "threadPool.hpp"
#include "task.hpp"


// 管理在线用户（基于IP:Port作为唯一标识）
class MessageRoute
{
public:
    MessageRoute() { }
    ~MessageRoute() { }

    // 根据用户IP和端口生成唯一键
    std::string generateKey(const InetAddr &user)
    {
        return user.getIp() + ":" + std::to_string(user.getPort());
    }

    // 更新用户信息，无论用户是否存在，都设置为最新的地址信息
    void updateUser(const InetAddr &user)
    {
        std::string key = generateKey(user);
        if (_users.find(key) == _users.end())
        {
            LOG(INFO, "User %s added.", key.c_str());
        }
        else
        {
            LOG(DEBUG, "User %s updated.", key.c_str());
        }
        _users.insert({key,user});
    }


    // 删除用户：当用户发送 "quit" 或其他退出信号时调用
    void removeUser(const InetAddr &user)
    {
        std::string key = generateKey(user);
        auto it = _users.find(key);
        if (it != _users.end())
        {
            _users.erase(it);
            LOG(INFO, "User %s removed.", key.c_str());
        }
        else
        {
            LOG(WARNING, "Attempt to remove non-existent user %s.", key.c_str());
        }
    }

    // 转发消息：将消息发送给所有非发送者的用户
    void sendMessage(const InetAddr &sender, const std::string &message, const int &sockfd)
    {
        for (const auto &pair : _users)
        {
            // 如果该用户不是发送者，则转发消息
            if (pair.second != sender)
            {
                ThreadPool<task>::getInstance()->enQueue(task(sender, message, pair.second, sockfd));
            }
        }
    }

private:
    std::unordered_map<std::string, InetAddr> _users;
};