#ifndef __LOG_HPP__
#define __LOG_HPP__
#include <cstdio>
#include <string>
#include <ctime>
#include <iostream>
#include <sys/types.h>
#include <unistd.h>
#include <cstdarg>
#include <pthread.h>
#include <fstream>
#include "status.h"

bool isSave = true;
const std::string logName = "log.txt";

#define LOG(level, format, ...)                                       \
    do                                                                \
    {                                                                 \
        logMessage(__FILE__, __LINE__, level, format, ##__VA_ARGS__); \
    } while (0);

#define LOGTOFILE()    \
    do                 \
    {                  \
        isSave = true; \
    } while (0);

#define LOGTOSCREEN()    \
    do                   \
    {                    \
        isSave = false; \
    } while (0);

char buffer[1024];

std::string levelToString(int level)
{
    switch (level)
    {
    case DEBUG:
        return "DEBUG";
    case INFO:
        return "INFO";
    case WARNING:
        return "WARNING";
    case ERROR:
        return "ERROR";
    case FATAL:
        return "FATAL";
    default:
        return "UNKNOW";
    }
}

//获取时间转为字符串形式
std::string getTimeString()
{
    time_t curTime = time(nullptr);
    struct tm *formatTime = localtime(&curTime);
    if (formatTime == nullptr)
        return "";
    char buffer[1024];
    snprintf(buffer, sizeof(buffer), "%d-%d-%d %d:%d:%d",
             formatTime->tm_year + 1900,
             formatTime->tm_mon + 1,
             formatTime->tm_mday,
             formatTime->tm_hour,
             formatTime->tm_min,
             formatTime->tm_sec);
    return buffer;
}

void saveLog(const std::string &fileName, const std::string &message)
{
    std::ofstream out(fileName, std::ios::app);
    if (!out.is_open())
    {
        return;
    }
    out << message<<std::endl;
    out.close();
}

// 日志的格式，日志等级、时间、代码的所在文件名和行数、日志的内容
void logMessage(std::string fileName, int line, int level, const char *format, ...)
{
    std::string levelstr = levelToString(level);
    std::string timestr = getTimeString();

    va_list arg;
    va_start(arg, format);
    vsnprintf(buffer, sizeof(buffer), format, arg);
    va_end(arg);

    std::string message =
        " [ " + timestr + " ]" +
        " [ " + levelstr + " ]" +
        " [ " + fileName + " ]" +
        " [ pid:" + std::to_string(getpid()) + " ]" +
        " [ " + std::to_string(line) + " ] " +
        " [ " + buffer + " ] ";

    if (!isSave)
    {
        std::cout << message << std::endl;
    }
    else
    {
        saveLog(logName, message);
    }
}

#endif
