#pragma once

#include <iostream>
#include <string>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <unistd.h>


int openDev(const std::string &dev,int flag)
{
    int fd = open(dev.c_str(),flag);
    if(fd < 0)
    {
        std::cerr<<"open error"<<std::endl;
        return -1;
    }
    return fd;
}