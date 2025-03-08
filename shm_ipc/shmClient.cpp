#include "Common_shm.hpp"
#include "Common_fifo.hpp"

int main()
{
    //0.创建监视对象
    Sync sync;
    sync.openRead();
    //1.创建对象
    shmIPC shmipc(getShmKey());
    //2.创建共享内存
    shmipc.createShm(SIZE);
    //3.挂接共享内存
    char *address = (char*) shmipc.shmAttach() ;
    cout<< address <<endl;

    //TO DO
    while(true)
    {
        bool isWakeUp = sync.wait();
        if(isWakeUp)
            cout<<"shm content: "<<address<<endl;
        else
            break;
    }
    return 0;
}