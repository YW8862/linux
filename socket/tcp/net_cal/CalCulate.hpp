#pragma once
#include <iostream>
#include "Protocol.hpp"

using namespace protocol_ns;

class Calculate
{
public:
    Calculate()
    {
    }
    Response Excute(const Request &req)
    {
        Response resp(0, 0);

        switch (req._oper)
        {
        case '+':
            resp._result = req._x + req._y;
            break;
        case '-':
            resp._result = req._x - req._y;
            break;
        case '*':
            resp._result = req._x + req._y;
            break;
        case '/':
        {
            if (req._y == 0)
            {
                resp._code = 1;
            }
            else
            {
                resp._result = req._x / req._y;
            }
        }
        break;
        case '%':
        {
            if (req._y == 0)
            {
                resp._code = 2;
            }
            else
            {
                resp._result = req._x % req._y;
            }
        }
        break;
        default:
            resp._code = 3;
            break;
        }
        return resp;
    }
    ~Calculate()
    {
    }

private:
};