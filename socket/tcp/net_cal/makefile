.PHONY:all
all:tcpServer

tcpServer:tcpServer.cpp
	g++ -o $@ $^ -g -std=c++14 -lpthread

# tcpClient:tcpClient.cpp
# 	g++ -o $@ $^ -g -std=c++14 -lpthread

.PHONY:clean
clean:
	rm -f tcpServer 