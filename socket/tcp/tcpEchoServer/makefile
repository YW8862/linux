.PHONY:all
all:tcpServer tcpClient

tcpServer:tcpServer.cpp
	g++ -o $@ $^ -g -std=c++14 -lpthread

tcpServer:tcpClient.cpp
	g++ -o $@ $^ -g -std=c++14 -lpthread

.PHONY:clean
clean:
	rm -f tcpServer udpCltcpClientient