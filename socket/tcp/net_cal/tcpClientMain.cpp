#include <iostream>
#include <string>
#include <memory>
#include <ctime>
#include "Socket.hpp"
#include "Protocol.hpp"
#include "inetAddr.hpp"

using namespace socket_ns;
using namespace protocol_ns;

void Usage(std::string proc)
{
    std::cout << "Usage:\n\t" << proc << " serverip serverport\n"
              << std::endl;
}

// ./tcp_client serverip serverport
int main(int argc, char *argv[])
{
    if (argc != 3)
    {
        Usage(argv[0]);
        exit(1);
    }
    std::string serverip = argv[1];
    uint16_t serverport = std::stoi(argv[2]);
    InetAddr serveraddr(serverip, serverport);
    Factory factory;
    std::unique_ptr<Socket> cli = std::make_unique<TcpSocket>();
    bool res = cli->BuildClientSocket(serveraddr);
    std::string inbuffer;
    while (res)
    {
        sleep(1);
        auto req = factory.BuildRequest();

        // 2. 对请求进行序列化
        std::string send_str;
        req->Serialize(&send_str);

        std::cout << "Serialize: \n"
                  << send_str << std::endl;

        // 3. 添加长度报头
        send_str = Encode(send_str);
        std::cout << "Encode: \n"
                  << send_str << std::endl;
        // 4. "len"\r\n"{}"\r\n
        cli->Send(send_str);

        // 5. 读取应答
        int n = cli->Recv(&inbuffer);
        if (n <= 0)
            break;
        std::string package = Decode(inbuffer);
        if (package.empty())
            continue;
        // 6. 我能保证package一定是一个完整的应答！
        auto resp = factory.BuildResponse();
        // 6.1 反序列化
        resp->Deserialize(package);

        // 7. 拿到了结构化的应答吗
        std::cout << resp->_result << "[" << resp->_code << "]" << std::endl;
    }
}
