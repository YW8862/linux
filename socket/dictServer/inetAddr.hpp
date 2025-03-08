#pragma once

#include <iostream>
#include <sys/types.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>

//转换ip和port
class InetAddr
{
private:
    void getAddress(std::string *ip, uint16_t *port)
    {
        *port = ntohs(_addr.sin_port);
        *ip = inet_ntoa(_addr.sin_addr);
    }

public:
    InetAddr(const struct sockaddr_in &addr) : _addr(addr)
    {
        getAddress(&_ip,&_port);
    }
    ~InetAddr()
    {
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
