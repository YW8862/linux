#include "Common_shm.hpp"
#include "Common_fifo.hpp"

int main()
{
    //0.创建监管对象
    fifo f1(path);
    Sync sync;
    sync.openWrite();   
    //1.创建对象
    shmIPC shmipc(getShmKey());
    //2.获取共享内存
    shmipc.getShm();
    //3.挂接共享内存
    char *address = (char*) shmipc.shmAttach() ;
    cout<< address <<endl;

    //TO DO
    int i=10;
    memset(address,0,SIZE);
    for(int i=0;i<10;i++)
    {
        address[i] = i + 'A';
        sleep(2);
        sync.wakeUp();
    }

    return 0;
}