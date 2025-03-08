#include "Common.hpp"

int main()
{
    fifo f1(path);
    int rfd = open(path.c_str(),O_RDONLY);
    if(rfd <0)
    {
        cerr<<"open fail"<<" ,error "<<errno<<" , errstring: "<<strerror(errno)<<endl;
        return 1;
    }
    char buff[1024];
    while(true)
    {
        // TO DO
        ssize_t size = read(rfd,buff,sizeof(buff)-1);
        if(size >0)
        {
            buff[size] = 0;
            cout<<"read sucess,content is:"<<buff<<endl;
        }
        else
        {
            cout<<"read fail"<<" errstring: "<<strerror(errno)<<endl;
            break;
        }
    }
    close(rfd);
    return 0;
}