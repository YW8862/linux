#ifndef __TASK_HPP__
#define __TASK_HPP__
#include <string>
#include <iostream>
#include <functional>
#include "inetAddr.hpp"

using namespace std;

// class task
// {
// private:
//     int sockfd;
//     InetAddr client;

// };


//using task = function<void()>;

// class task
// {
// public:
//     task()
//     {}

//     task(int a,int b):_a(a),_b(b),_result(0)
//     {}

//     void excute()
//     {
//         _result = _a + _b;
//     }

//     string resultToString()
//     {
//         return to_string(_a) + " + " + to_string(_b) + " = " + to_string(_result);
//     }

//     string questionToString()
//     {
//         return to_string(_a) + " + " + to_string(_b) + " = ?";
//     }

//     void operator ()()
//     {
//         this->excute();
//     }

// private:
//     int _a;
//     int _b;
//     int _result;
// };

#endif