#include <iostream>
#include <memory>
#include <functional>
#include <cstring>
#include <string>
#include "tcpServer.hpp"
#include "status.h"


void Usage(std::string proc)
{
    std::cout << "Usage:\n\t" << proc << "local_port" << std::endl;
}


int main(int argc,char *argv[])
{
    if(argc != 2)
    {
        Usage(argv[0]);
        exit(-1);
    }

    uint16_t port = std::stoi(argv[1]);
    
    std::unique_ptr<TcpServer> tsvr = std::make_unique<TcpServer>();
    tsvr->init();
    tsvr->loop();
    return 0;
}