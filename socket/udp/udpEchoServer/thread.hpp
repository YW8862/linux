#ifndef __THREAD_HPP__
#define __THREAD_HPP__

#include <iostream>
#include <string>
#include <pthread.h>
#include <functional>
#include <unistd.h>

// 封装thread类
namespace ThreadModule
{
    using func_t = std::function<void(std::string)>;
    class Thread
    {
    public:
        Thread(func_t func,  std::string name = "none-name")
            : _func(func), _threadName(name), _stop(true)
        {
            
        }

        ~Thread()
        {}

        bool start()
        {
            int n = pthread_create(&_tid, nullptr, threadRoutine, this);
            if (!n)
            {
                _stop = false;
                return true;
            }
            else
            {
                return false;
            }
        }

        void detach()
        {
            if (!_stop)
            {
                pthread_detach(_tid);
            }
        }

        void join() const
        {
            if (!_stop)
            {
                pthread_join(_tid, nullptr);
                //cout << "thread:" << _tid << " quit...." << endl;
            }
        }

        void excute()
        {
            _func(_threadName);
        }

        void Stop()
        {
        }

        std::string getName()
        {
            return _threadName;
        }

    private:
        static void *threadRoutine(void *args)
        {
            Thread *This = static_cast<Thread *>(args);
            This->excute();
            return nullptr;
        }

    private:
        pthread_t _tid;
        std::string _threadName;
        func_t _func;
        bool _stop;
    };
}

#endif