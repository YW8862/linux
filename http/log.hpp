
#pragma once
#include <iostream>
#include <string>
#include <mutex>
#include <atomic>
#include <sstream>
#include <fstream>

const std::string logPath = "log.txt";

enum LogLevel
{
    DEBUG,
    INFO,
    WARNING,
    ERROR,
    FATAL
};

class Logger
{
public:
    Logger(const Logger &) = delete;
    Logger &operator=(const Logger &) = delete;

    // 获取单例
    static Logger &instance()
    {
        static Logger instance;
        return instance;
    }

    // 设置最低提示等级
    void setLevel(LogLevel level)
    {
        currentLevel.store(level);
    }

    void log(LogLevel level, const std::string &message)
    {
        // 如果等级低于最小级别，直接忽略
        if (level < currentLevel.load())
            return;

        const char *levelNames[] = {"DEBUG", "INFO", "WARNING", "ERROR", "FATAL"};

        switch (saveToFile)
        {
        case true:
            // to do
            // 写入到文件
            logToFile(message);
            break;
        case false:
            // to do
            // 直接打印到标准错误
            std::lock_guard<std::mutex> lock(logMutex);
            std::cerr << "[" << levelNames[static_cast<int>(level)] << "] " << message << '\n';
            break;
        }
    }

    void logToFile(const std::string &message)
    {
        std::lock_guard<std::mutex> lock(logMutex);

        std::ofstream out(logPath, std::ios::app);
        if (!out.is_open())
        {
            return;
        }
        out << message << std::endl;
        // out.write(message.c_str(),message.size());
        out.close();
    }

    std::atomic<LogLevel> currentLevel;

    void setLogToScreen()
    {
        saveToFile = false;
    }

    void setLogToFile()
    {
        saveToFile = true;
    }

private:
    Logger() : currentLevel(INFO), saveToFile(false) {} // 构造初始化原子变量

    std::mutex logMutex;
    bool saveToFile;
};


class LogStream : public std::ostringstream
{
public:
    explicit LogStream(LogLevel level) : level(level)
    {
    }
    ~LogStream()
    {
        auto currentLevel = Logger::instance().currentLevel.load();
        if (level >= currentLevel)
        {
            Logger::instance().log(level, this->str());
        }
    }

private:
    LogLevel level;
};

#define LOG(level) LogStream(level)

#define LOGTOSCREEN() Logger::instance().setLogToScreen()

#define LOGTOFILE() Logger::instance().setLogToFile()