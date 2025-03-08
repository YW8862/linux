.PHONY:all
all:shmClient shmServer

shmClient:shmClient.cpp
	g++ -o $@ $^ -std=c++11

shmServer:shmServer.cpp
	g++ -o $@ $^ -std=c++11

.PHONY:clean
clean:
	rm -f shmClient shmServer