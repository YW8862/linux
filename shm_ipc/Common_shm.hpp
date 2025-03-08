#pragma once
#include <iostream>
#include <sys/ipc.h>
#include <sys/types.h>
#include <sys/shm.h>
#include <cstring>
#include <stdlib.h>
#include <unistd.h>

using namespace std;
#define SIZE 4096

const char* pathName = "/home/yw";
const int proj_id = 0x6666;


key_t getShmKey()
{
    key_t key = ftok(pathName,proj_id);
    if(key < 0)
    {
        cerr<<"ftok error,errno: "<<errno<<", error string: "<<strerror(errno)<<endl;
        exit(-1);
    }

    return key;
}

class shmIPC
{
public:
    shmIPC(key_t key)
    :_key(key),_shmid(0),_size(SIZE),_address(nullptr)
    {}

    ~shmIPC()
    {
        //解除挂接
        int n = shmdt(_address);
        if(n < 0)
        {
            exit(-1);
            cout<<"shm Deattach fail, errno: "<<errno<<" ,error: "<<strerror(errno)<<endl;
        }
        _address = nullptr;


        //删除共享内存
        n = shmctl(_shmid,IPC_RMID,nullptr);
        if(n < 0)
        {
            exit(-1);
            cerr<<"shm del fail,errno: "<<errno<<" ,strerror: "<<strerror(errno)<<endl;
        }
        else
        {
            cout<<"shm del sucess"<<endl;
        }

    }

    void createShm(size_t size = SIZE)
    {
        _size = size;
        int shmid = shmget(_key,_size,IPC_CREAT | IPC_EXCL);
        if(shmid < 0)
        {
            cerr<<"shm create fail,errno: "<<errno<<" ,strerror: "<<strerror(errno)<<endl;
            exit(-1);
        }
        _shmid = shmid;
    }

    void getShm()
    {
        int shmid = shmget(_key,_size,IPC_CREAT);
        if(shmid < 0)
        {
            cerr<<"shm get fail,errno: "<<errno<<" ,strerror: "<<strerror(errno)<<endl;
            exit(-1);   
        }
        _shmid = shmid;
    }

    void print()
    {
        struct shmid_ds shmds;
        int n = shmctl(_shmid,IPC_STAT,&shmds);
        if(n < 0)
        {
            cerr<<"shm print fail,errno: "<<errno<<" ,strerror: "<<strerror(errno)<<endl;
            exit(-1);
        }
        cout<<"shmds.shm_segsz: "<<shmds.shm_segsz<<endl;
        cout<<"shmds.shm_nattch: "<<shmds.shm_nattch<<endl;
        cout<<"shmds.shm_ctime: "<<shmds.shm_ctime<<endl;
        cout<<"shmds.shm_perm.__key: "<<shmds.shm_perm.__key<<endl;
        cout<<"shmid: "<<_shmid<<endl;
    }

    
    void *shmAttach()
    {
        void *address = shmat(_shmid,nullptr,0);
        if(address == (void*) (-1))
        {
            cout<<"shm attach fail, errno: "<<errno<<" ,error: "<<strerror(errno)<<endl;
            exit(-1);
        }
        _address = address;
        return address;
    }


private:
    key_t _key;
    int _shmid;
    size_t _size;
    void* _address;
};

