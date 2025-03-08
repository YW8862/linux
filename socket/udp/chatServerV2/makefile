.PHONY:all
all:udpServer udpClient

udpServer:udpServer.cpp
	g++ -o $@ $^ -g -std=c++14 -lpthread

udpClient:udpClient.cpp
	g++ -o $@ $^ -g -std=c++14 -lpthread

.PHONY:clean
clean:
	rm -f udpServer udpClient