.PHONY:all
all:server Cgi
Cgi:Cgi.cc
	g++ -o $@ $^
server:HttpServer.cc
	g++ -o $@ $^ -std=c++11 -lpthread
.PHONY:clean
clean:
	rm -f server Cgi
