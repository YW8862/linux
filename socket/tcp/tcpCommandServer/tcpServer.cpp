#include <iostream>
#include <memory>
#include <functional>
#include <cstring>
#include <string>
#include "tcpServer.hpp"
#include "status.h"
#include "command.hpp"

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
    LOGTOFILE();
    uint16_t port = std::stoi(argv[1]);
    
    Command cmd;
    func_t func = std::bind(&Command::excute,&cmd,std::placeholders::_1);

    std::unique_ptr<TcpServer> tsvr = std::make_unique<TcpServer>(htons(port),func);
    tsvr->init();
    tsvr->loop();
    return 0;
}