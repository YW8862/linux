#pragma once

#include <iostream>
#include <string>
#include <memory>
#include <unistd.h>
#include <sys/types.h>
#include <jsoncpp/json/json.h>

namespace protocol_ns
{
    const std::string SEP = "\r\n";

    // 我们把tcp中读到的报文，可能读到半个，也可能读到1个半等， TCP 粘报问题
    // 解决TCP的粘报问题
    std::string Encode(const std::string &json_str)
    {
        int json_str_len = json_str.size();
        std::string proto_str = std::to_string(json_str_len);
        proto_str += SEP;
        proto_str += json_str;
        proto_str += SEP;
        return proto_str;
    }

    std::string Decode(std::string &inbuffer)
    {
        auto pos = inbuffer.find(SEP);
        if (pos == std::string::npos)
            return std::string();
        std::string len_str = inbuffer.substr(0, pos);
        if (len_str.empty())
            return std::string();
        int packlen = std::stoi(len_str);

        int total = packlen + len_str.size() + 2 * SEP.size();
        if (inbuffer.size() < total)
            return std::string();

        std::string package = inbuffer.substr(pos + SEP.size(), packlen);
        inbuffer.erase(0, total);
        return package;
    }

    // 报文 = 报头+有效载荷
    // "有效载荷的长度"\r\n"有效载荷"\r\n
    // "len"\r\n"_x _op _y"\r\n  -> len: 有效载荷的长度，约定\r\n是分隔符，不参与统计

    // 就是结构化数据 能像网络中直接发送吗？不能，也不建议
    // 其他问题？class Reqeust req(10, 20, "*"); req->字节？ 直接发送req对象，class Reqeust req
    class Request
    {
    public:
        Request()
        {
        }
        Request(int x, int y, char oper) : _x(x), _y(y), _oper(oper)
        {
        }
        bool Serialize(std::string *out)
        {
            // 转换成为字符串
            Json::Value root;
            root["x"] = _x;
            root["y"] = _y;
            root["oper"] = _oper;

            Json::FastWriter writer;
            // Json::StyledWriter writer;
            *out = writer.write(root);
            return true;
        }
        bool Deserialize(const std::string &in) // 你怎么知道你读到的in 就是完整的一个请求呢？
        {
            Json::Value root;
            Json::Reader reader;
            bool res = reader.parse(in, root);
            if (!res)
                return false;

            _x = root["x"].asInt();
            _y = root["y"].asInt();
            _oper = root["oper"].asInt();
            return true;
        }

    public:
        int _x;
        int _y;
        char _oper; // "+-*/%" _x _oper _y
    }; // --- "字符串"

    class Response
    {
    public:
        Response()
        {
        }
        Response(int result, int code) : _result(result), _code(code)
        {
        }
        bool Serialize(std::string *out)
        {
            // 转换成为字符串
            Json::Value root;
            root["result"] = _result;
            root["code"] = _code;

            Json::FastWriter writer;
            // Json::StyledWriter writer;
            *out = writer.write(root);
            return true;
        }
        bool Deserialize(const std::string &in)
        {
            Json::Value root;
            Json::Reader reader;
            bool res = reader.parse(in, root);
            if (!res)
                return false;

            _result = root["result"].asInt();
            _code = root["code"].asInt();
            return true;
        }

    public:
        int _result; // 结果
        int _code;   // 0:success 1: 除0 2: 非法操作 3. 4. 5
    }; // --- "字符串"

    class Factory
    {
    public:
        Factory()
        {
            srand(time(nullptr) ^ getpid());
            opers = "+/*/%^&|";
        }

        std::shared_ptr<Request> BuildRequest()
        {
            int x = rand() % 10 + 1;
            usleep(x * 10);
            int y = rand() % 5; // [01,2,3,4]
            usleep(y * x * 5);
            char oper = opers[rand() % opers.size()];
            std::shared_ptr<Request> req = std::make_shared<Request>(x, y, oper);
            return req;
        }
        std::shared_ptr<Response> BuildResponse()
        {
            return std::make_shared<Response>();
        }
        ~Factory()
        {
        }

    private:
        std::string opers;
    };
}
