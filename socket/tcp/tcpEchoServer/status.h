#ifndef __STATUS_H__
#define __STATUS_H__

enum
{
    SOCKET_ERROR = 1,
    BIND_ERROR,
    LISTEN_ERROR,
    USAGE_ERROR,
    CONNECT_ERROR
};

enum Level
{
    DEBUG = 0,
    INFO,
    WARNING,
    ERROR,
    FATAL
};

#endif