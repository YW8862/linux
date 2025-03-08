#include <iostream>
#include <memory>
#include "udpServer.hpp"
#include "status.h"

void Usage(std::string proc)
{
    std::cout << "Usage:\n\t" << proc << "local_port" << std::endl;
}

int main(int argc, char *argv[])
{
    if(argc != 2)
    {
        Usage(argv[0]);
        exit(USAGE_ERROR);
    }
    //获取端口号
    uint16_t port = std::stoi(argv[1]); 

    LOGTOFILE();
    //LOGTOSCREEN();
    std::unique_ptr<UdpServer> usvr = std::make_unique<UdpServer>(port);
    usvr->initServer();
    usvr->start();
    return 0;
}