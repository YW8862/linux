/*
 * @Author: YW8862 2165628227@qq.com
 * @Date: 2025-04-09 14:06:59
 * @LastEditors: YW8862 2165628227@qq.com
 * @LastEditTime: 2025-04-09 21:13:24
 * @FilePath: /yw/linux/socket/tcp/net_cal/inetAddr.hpp
 * @Description: 这是默认设置,请设置`customMade`, 打开koroFileHeader查看配置 进行设置: https://github.com/OBKoro1/koro1FileHeader/wiki/%E9%85%8D%E7%BD%AE
 */
#pragma once

#include <iostream>
#include <sys/types.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>

class InetAddr
{
private:
    void getAddress(std::string *ip, uint16_t *port)
    {
        *port = ntohs(_addr.sin_port);
        *ip = inet_ntoa(_addr.sin_addr);
    }

public:
    InetAddr()
    {
        
    }

    InetAddr(const std::string &ip,const uint16_t port):_ip(ip)._port(port)
    {
        _addr.sin_family = AF_INET;
        _addr.sin_port = htons(_port);
        _addr.sin_addr.s_addr = inet_addr(_ip);
    }

    InetAddr(const struct sockaddr_in &addr) : _addr(addr)
    {
        getAddress(&_ip,&_port);
    }
    
    ~InetAddr()
    {
    }

    InetAddr operator=(InetAddr other)
    {
        if(&other == this)
        {
            return *this;
        }
        _addr = other._addr;
        _ip = other._ip;
        _port = other._port;
    }

    std::string getIp()
    {
        return _ip;
    }

    uint16_t getPort()
    {
        return _port;
    }

private:
    struct sockaddr_in _addr;
    std::string _ip;
    uint16_t _port;
};
