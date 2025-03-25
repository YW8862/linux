#ifndef __COMMAND_HPP__
#define __COMMAND_HPP__
#include <string>
#include <iostream>
#include <fstream>
#include <stdio.h>
#include <set>
#include <unistd.h>
#include "log.hpp"
std::string ConfigPath = "./safe.txt";

class Command
{
private:
    // 加载合法命令
    void loadConfig(std::string)
    {
        std::ifstream in(_configPath);
        if (!in.is_open())
        {
            LOG(FATAL, "safeCommand load failed");
            return;
        }
        std::string cmd;
        while (std::getline(in, cmd))
        {
            _safeCommand.insert(cmd);
            LOG(INFO, "load command [%s] sucess", cmd.c_str());
        }
        in.close();
    }

    std::string prefix(std::string cmd)
    {
        if(cmd.empty()) return "";
        auto pos = cmd.find(" ");
        if(pos == std::string::npos)
            return cmd;
        return cmd.substr(0,pos);
    }

    // 检查命令是否合法
    bool check(std::string cmd)
    {
        std::string prefixString = prefix(cmd);

        return _safeCommand.find(prefixString) != _safeCommand.end();
    }

public:
    Command(std::string configPath = ConfigPath) : _configPath(ConfigPath)
    {
        loadConfig(_configPath);
    }

    ~Command() {}

    std::string excute(const std::string &cmd)
    {
        std::string result;
        if (check(cmd))
        {
            FILE *fp = popen(cmd.c_str(), "r");
            if (fp == nullptr)
            {
                return "failed";
            }

            char buffer[1024];
            while (fgets(buffer, sizeof(buffer), fp) != nullptr)
            {
                result += buffer;
            }
            fclose(fp);
        }
        else
        {
            result = "invalid command";
        }
        return result;
    }

private:
    std::set<std::string> _safeCommand;
    std::string _configPath;
};

#endif