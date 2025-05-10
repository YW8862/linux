#pragma once

#include <iostream>
#include <string>
#include <vector>

class HttpServer
{
public:
    HttpServer() {}

    std::string httpHandler(const std::string &request)
    {
        std::cout << "-------------------------------------" << std::endl;
        std::cout << request << std::endl;


        std::string response = "HTTP/1.0 200 OK\r\n";
        response += "\r\n";
        response += "<html><body><h1>hello world</h1></body></html>";

        return response;
    }

    ~HttpServer() {}
};