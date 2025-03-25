#ifndef __THREAD_HPP__
#define __THREAD_HPP__

#include <iostream>
#include <string>
#include <pthread.h>
#include <functional>
#include <unistd.h>
using namespace std;

// 封装thread类
namespace ThreadModule
{
    using function_t = function<void(string)>;
    class Thread
    {
    public:
        Thread(function_t func, string name = "none-name")
            : _func(func), _threadName(name), _stop(true)
        {
        }

        ~Thread()
        {
        }

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
                // cout << "thread:" << _tid << " quit...." << endl;
            }
        }

        void excute()
        {
            _func(_threadName);
        }

        void Stop()
        {
        }

        string getName()
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
        string _threadName;
        function_t _func;
        bool _stop;
    };
}

#endif