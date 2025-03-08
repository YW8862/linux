#ifndef __COMMON_HPP__
#define __COMMON_HPP__

#include <iostream>
#include <string>
#include <sys/types.h>
#include <sys/stat.h>
#include <cstring>
#include <cerrno>
#include <unistd.h>
#include <fcntl.h>
using namespace std;

//管道路径
string path = "./fifo";
//创建mode
#define Mode 0666



class fifo
{
public:
    fifo(const string&path):_path(path)
    {
        umask(0);
        int n = mkfifo(_path.c_str(),Mode);
        //管道创建失败，打印失败信息
        if(n < 0)
        {
            cerr<<"mkfifo fail"<<" ,errorno "<<errno<<" , errstring: "<<strerror(errno)<<endl;
        }
    }
    ~fifo()
    {
        //通信结束，删除管道
        int n = unlink(_path.c_str());
        if(n == 0)
        {
            cout<<"remove fifo file"<<_path<<" sucess "<<endl;
        }
        else
        {
            cout<<"remove fifo file"<<_path<<" fail "<<endl;
        }
    }

private:
    string _path;
};

#endif