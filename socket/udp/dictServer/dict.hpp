#include <iostream>
#include <fstream>
#include <string>
#include <map>
#include <unordered_map>
#include "log.hpp"

namespace dict_ns
{
    const std::string defaultConfigPath = "./dict.txt";
    const std::string sep = ": ";
    class dict
    {
    public:
        dict(const std::string &configPath = defaultConfigPath) : _configPath(configPath)
        {
            load();
        }

        ~dict()
        {
        }
        std::string translate(const std::string word)
        {
            std::unordered_map<std::string, std::string>::iterator iter = _dict.find(word);
            // 表示没有找到
            if (iter == _dict.end())
            {
                return "";
            }
            return iter->second;
        }

    private:
        // 加载配置信息
        bool load()
        {
            std::ifstream in(_configPath);
            if (!in.is_open())
            {
                LOG(FATAL, "open cinfig %s fail", _configPath.c_str());
                return false;
            }
            // 加载文件
            std::string line;
            while (std::getline(in, line))
            {
                // 如果该行没有数据，直接返回即可
                if (line.empty())
                {
                    continue;
                }
                size_t pos = line.find(sep);
                if (pos == std::string::npos)
                {
                    continue;
                }
                // string::substr(pos,len);pos:开始位置,len:长度
                std::string chinese = line.substr(0, pos);
                if (chinese.empty())
                {
                    continue;
                }
                std::string english = line.substr(pos + sep.size());
                if (english.empty())
                {
                    continue;
                }
                _dict.insert({chinese, english});
                //LOG(INFO, "load pair [%s]:[%s] scuess", chinese.c_str(), english.c_str());
            }
            // 加载完毕，关闭文件
            in.close();
            return true;
        }

    private:
        std::unordered_map<std::string, std::string> _dict;
        std::string _configPath;
    };
};