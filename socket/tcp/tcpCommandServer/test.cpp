#include "command.hpp"
#include <iostream>
#include <string>
#include <fstream>
#include <string>

int main()
{
    Command cmd;
    std::string cmdString;
    while (true)
    {
        std::cout << "enter command#";
        std::getline(std::cin, cmdString);
        std::cout << cmd.excute(cmdString);
    }
    return 0;
}