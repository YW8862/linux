#include "Common.hpp"

int main()
{
    
    int wfd = open(path.c_str(),O_WRONLY);
    //打开管道失败
    if(wfd <0)
    {
        cerr<<"open fail"<<" ,errorno "<<errno<<" , errstring: "<<strerror(errno)<<endl;
        return 1;
    }
    string buff;
    while(true)
    {   
        // TO DO
        cout<<"please enter your messaage: ";
        //读取用户输入
        getline(cin,buff);
        if(buff == "quit")
        {
            break;
        }
        //向管道写信息
        ssize_t size = write(wfd,buff.c_str(),buff.size());
        if(size < 0)
        {
            cout<<"write fail"<<" errstring: "<<strerror(errno)<<endl;
            break;
        }
    }
    close(wfd);
    return 0;
}