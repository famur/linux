.PHONY:all
all:udpClient udpServer
CC=g++

udpClient:udpClient.cc
	$(CC) -o $@ $^ 
udpServer:udpServer.cc
	$(CC) -std=c++11 -o $@ $^

.PHONY:clean
clean:
	rm -f udpClient udpServer
