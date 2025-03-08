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

    std::string getIp()const
    {
        return _ip;
    }

    uint16_t getPort()const
    {
        return _port;
    }

    bool operator==(const InetAddr& inetAddr)const
    {
        return (_addr.sin_addr.s_addr == inetAddr._addr.sin_addr.s_addr)
                && (_addr.sin_family == inetAddr._addr.sin_family)
                && (_addr.sin_port == inetAddr._port)
                && (_addr.sin_zero == inetAddr._addr.sin_zero)
                && (_ip == inetAddr._ip)
                && (_port == inetAddr._port);
    }

    bool operator!=(const InetAddr& inetAddr)const
    {
        return ! (*this == inetAddr);
    }

private:
    struct sockaddr_in _addr;
    std::string _ip;
    uint16_t _port;
};
