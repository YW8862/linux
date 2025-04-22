#include <iostream>
#include <string>
#include <cstring>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <netinet/in.h>

enum ExitCode
{
    USAGE_ERR = 1,
    SOCK_ERR
};

enum Status
{
    NEW,
    CONNECTING,
    CONNECTED,
    DISCONNECTED, // 失联
    CLOSED        // 重连失败
};

static const int defaultSockfd = -1;
static const int defaultRetryInterval = 1;
static const int defaultMaxRetries = 5;

class ClientConnection
{
public:
    ClientConnection(const std::string &ip, uint16_t port)
        : _sockfd(defaultSockfd),
          _ip(ip),
          _port(port),
          _status(Status::NEW),
          _retryInterval(defaultRetryInterval),
          _maxRetries(defaultMaxRetries)
    {
    }
    ~ClientConnection() {}

public:
    void connect()
    {
        _sockfd = socket(AF_INET,SOCK_STREAM,0);
        if(_sockfd < 0)
        {
            //创建套接字失败
            std::cerr<<"sockfd create error: "<<_sockfd<<std::endl;
            exit(SOCK_ERR);
        }

        sockaddr_in server;
        bzero(&server,sizeof(server));
        inet_pton(AF_INET,_ip.c_str(),&server.sin_addr.s_addr);
        //server.sin_addr.s_addr = inet_addr(_ip.c_str());
        server.sin_family = AF_INET;
        server.sin_port =  htons(_port);

        int n = ::connect(_sockfd,(struct sockaddr*)&server,sizeof(server));
        //n<0，链接失败
        if(n < 0)
        {
            //清除资源
            disConnect();
            //更新状态
            _status = DISCONNECTED;
            //返回
            return;
        }
        _status = CONNECTED;
    }
    void reConnect()
    {
        _status = CONNECTING;
        int cnt = 0;
        while(true)
        {
            connect();
            //重连成功
            if(_status == CONNECTED)
            {
                break;
            }

            cnt++;
            if(cnt > _maxRetries)
            {
                _status = CLOSED;
                std::cerr<<"重连失败，请检查网络"<<std::endl;
                break;
            }
            std::cout<<"正在进行第"<<cnt<<"次断线重连"<<std::endl;
            sleep(_retryInterval);
        }
    }

    void disConnect()
    {
        if(_sockfd > defaultSockfd)
        {
            close(_sockfd);
            _sockfd = defaultSockfd;
            _status = CLOSED;
        }
    }
    // 进行正常IO通信
    void process()
    {
        while(true)
        {
            std::string  message = "hello server";
            ssize_t n = write(_sockfd,message.c_str(),message.size());
            if(n > 0)
            {
                char buffer[1024];
                ssize_t m = read(_sockfd,(void*)buffer,sizeof(buffer)-1);
                if(m > 0)
                {
                    buffer[m] = '\0';
                    std::cout<<"server echo"<<buffer<<std::endl;
                }
                //对方链接关闭或者是读取失败
                else if(m <= 0)
                {
                    _status = DISCONNECTED;
                    break;
                }
            }
            else
            {
                std::cerr<<"send error"<<std::endl;
                disConnect();
                break;
            }
            sleep(1);
        }

    }

    // 返回链接的状态
    Status status()
    {
        return _status;
    }

private:
    int _sockfd;
    std::string _ip;
    uint16_t _port;
    Status _status;
    int _retryInterval; // 重试的时间间隔
    int _maxRetries;    // 重试的最大次数
};

class TcpClient
{
public:
    TcpClient(const std::string &ip, uint16_t port)
        : _connection(ip, port)
    {
    }

    ~TcpClient() {}

    void execute()
    {
        while (true)
        {
            switch (_connection.status())
            {
            case Status::NEW:
                // 发起链接
                _connection.connect();
                break;
            case Status::CONNECTED:
                //
                _connection.process();
                break;
            case Status::DISCONNECTED:
                _connection.reConnect();
                break;
            case Status::CLOSED:
                _connection.disConnect();
                return;
            default:
                break;
            }
        }
    }

private:
    ClientConnection _connection;
};

void Usage(std::string process)
{
    std::cout << "Usage: " << process << " serverIP ServerPort" << std::endl;
}

int main(int argc, char **argv)
{
    if (argc != 3)
    {
        Usage(argv[0]);
        exit(USAGE_ERR);
    }
    std::string serverip = argv[1];
    uint16_t serverPort = atoi(argv[2]);
    TcpClient tcpclient(serverip,serverPort);
    tcpclient.execute();

    return 0;
}