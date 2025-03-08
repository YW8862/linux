.PHONY:all
all:pipeClient pipeServer

pipeClient:pipeClient.cpp
	g++ -o $@ $^ -std=c++11

pipeServer:pipeServer.cpp
	g++ -o $@ $^ -std=c++11

.PHONY:clean
clean:
	rm -f pipeClient pipeServer