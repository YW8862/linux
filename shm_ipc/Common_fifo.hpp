#ifndef __COMMON_FIFO_HPP__
#define __COMMON_FIFO_HPP__

#include <iostream>
#include <string>
#include <sys/types.h>
#include <sys/stat.h>
#include <cstring>
#include <cerrno>
#include <unistd.h>
#include <fcntl.h>
#include <cassert>
using namespace std;

// 管道路径
string path = "./fifo";
// 创建mode
#define Mode 0666

class fifo
{
public:
    fifo(const string &path) : _path(path)
    {
        umask(0);
        int n = mkfifo(_path.c_str(), Mode);
        // 管道创建失败，打印失败信息
        if (n < 0)
        {
            cerr << "mkfifo fail" << " ,errorno " << errno << " , errstring: " << strerror(errno) << endl;
        }
        else
        {
            cout<<"mkfifo sucess"<<endl;
        }
    }
    ~fifo()
    {
        // 通信结束，删除管道
        int n = unlink(_path.c_str());
        if (n == 0)
        {
            cout << "remove fifo file" << _path << " sucess " << endl;
        }
        else
        {
            cout << "remove fifo file" << _path << " fail " << endl;
        }
    }

private:
    string _path;
};

class Sync
{
public:
    Sync()
    : _wfd(-1), _rfd(-1)
    {
        cout<<"Sync"<<endl;
    }

    ~Sync()
    {
        if(_wfd != -1)
        {
            close(_wfd);
        }
        if(_rfd != -1)
        {
            close(_rfd);
        }
    }

    //以只读方式打开
    void openRead()
    {
        int rfd = open(path.c_str(),O_RDONLY);
        if(rfd < 0)
        {
            cerr<<"openRead fail,errno is"<<errno<<"error string is"<<strerror(errno)<<endl;
            exit(1);
        }
        else 
        {
            cout<<"openRead sucess"<<endl;
        }
        _rfd = rfd;
    }
    //以只写方式打开
    void openWrite()
    {   
        int wfd = open(path.c_str(),O_WRONLY);
        if(wfd < 0)
        {
            cout<<"openWrite fail,errno is"<<errno<<" ,error string is"<<strerror(errno)<<endl;
            exit(1);
        }
        else 
        {
            cout<<"openWrite sucess"<<endl;
        }
        _wfd = wfd;
    }

    bool wait()
    {
        char c = 0;
        ssize_t n = read(_rfd,&c,sizeof(c));
        if(n <= 0)
        {
            cout<<"wait error"<<endl;
            return false;
        }
        return true;
    }

    void wakeUp()
    {
        char wake = 'A';
        ssize_t n = write(_wfd,&wake,sizeof(wake));
        if(n < 0)
        {
            cout<<"wake up error"<<endl;
        }
    }

private:
    int _wfd;
    int _rfd;
};

#endif