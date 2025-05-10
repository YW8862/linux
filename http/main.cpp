#include <iostream>
#include <string>
#include <cstring>
#include <functional>
#include "log.hpp"
#include "tcpServer.hpp"

void Usage(std::string process)
{
    std::cout<<"Usage: "<<process<<"Port"<<std::endl;
}

int main(int argc,char** argv)
{
    if(argc != 2)
    {
        Usage(argv[0]);
    }

    int port = std::stoi(argv[1]);
    
    HttpServer httpServer;
    TcpServer tcpServer(port,std::bind(&HttpServer::httpHandler,&httpServer,std::placeholders::_1));
    tcpServer.Loop();

    return 0;
}