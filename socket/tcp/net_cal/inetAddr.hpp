#pragma once

#include <iostream>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>

class InetAddr
{
private:
    void GetAddress(std::string *ip, uint16_t *port)
    {
        *port = ntohs(_addr.sin_port);
        *ip = inet_ntoa(_addr.sin_addr);
    }

public:
    InetAddr(const struct sockaddr_in &addr) : _addr(addr)
    {
        GetAddress(&_ip, &_port);
    }
    InetAddr(const std::string &ip, uint16_t port) : _ip(ip), _port(port)
    {
        _addr.sin_family = AF_INET;
        _addr.sin_port = htons(_port);
        _addr.sin_addr.s_addr = inet_addr(_ip.c_str());
    }
    InetAddr()
    {
    }

    bool operator==(const InetAddr &addr)
    {
        // if(_ip == addr._ip)
        if (_ip == addr._ip && _port == addr._port) // 方便测试
        {
            return true;
        }
        return false;
    }
    // bool operator = (const struct sockaddr_in &addr)
    // {
    //     _addr = addr;
    // }
    struct sockaddr_in getAddr()
    {
        return _addr;
    }

    std::string getIp()
    {
        return _ip;
    }
    uint16_t getPort()
    {
        return _port;
    }
    ~InetAddr()
    {
    }

private:
    struct sockaddr_in _addr;
    std::string _ip;
    uint16_t _port;
};