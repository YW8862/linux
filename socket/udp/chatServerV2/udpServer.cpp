#include <iostream>
#include <memory>
#include <functional>
#include "udpServer.hpp"
#include "status.h"
#include "messageRoute.hpp"


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

    //创建消息转发
    MessageRoute messageRoute;

    std::unique_ptr<UdpServer> usvr = std::make_unique<UdpServer>(port,std::bind(&MessageRoute::sendMesage,&messageRoute,std::placeholders::_1,std::placeholders::_2,std::placeholders::_3));
    usvr->initServer();
    usvr->start();
    return 0;
}