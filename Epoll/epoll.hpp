#pragma once

#include<iostream>
#include<stdlib.h>
#include<string>
#include<unistd.h>
#include<sys/socket.h>
#include<sys/types.h>
#include<arpa/inet.h>
#include<sys/epoll.h>
#include<netinet/in.h>

using namespace std;

class Server{
    private:
        int port;
        int lsock;
        int epfd;
    public:
        Server(int port_ = 8888):port(port_),lsock(-1),epfd(-1)
        {
        }
        void InitServer()
        {
            lsock = socket(AF_INET, SOCK_STREAM, 0);
            if(lsock < 0)
            {
                cerr << "socket error!" << endl;
                exit(2);
            }
            struct sockaddr_in local;
            local.sin_family = AF_INET;
            local.sin_addr.s_addr = htonl(INADDR_ANY);
            local.sin_port = htons(port);
            
            if(bind(lsock, (struct sockaddr*)&local, sizeof(local)) < 0)
            {
                cerr << "bind error!" << endl;
                exit(3);
            }
            if(listen(lsock, 5) < 0)
            {
                cerr << "listen error!" << endl;
                exit(4);
            }
            
            int opt = 1;
            setsockopt(lsock, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
            if( ( epfd = epoll_create(256)) < 0)
            {
                cerr << "epoll_create error!" << endl;
                exit(5);
            }
        }

        void HanderEvents(int epfd, struct epoll_event *revs, int num)
        {
            struct epoll_event ev;
            for(int i = 0; i < num; i++)
            {
                int sock = revs[i].data.fd;
                if(revs[i].events & EPOLLIN)
                {
                    if(sock == lsock)
                    {
                        struct sockaddr_in peer;
                        socklen_t len = sizeof(peer);
                        int new_sock = accept(lsock, (struct sockaddr*)&peer, &len);
                        if(new_sock < 0)
                        {
                            cerr << "accept error!" << endl;
                            continue;
                        }
                        cout << "get a new link" << endl;
                        ev.events = EPOLLIN;
                        ev.data.fd = new_sock;
                        epoll_ctl(epfd, EPOLL_CTL_ADD, new_sock, &ev);
                    }
                    else
                    {
                        char buf[1024];
                        ssize_t s = recv(sock, buf, sizeof(buf)-1,0);
                        if(s > 0)
                        {
                            buf[s] = 0;
                            cout << buf << endl;
                            ev.events = EPOLLOUT;
                            ev.data.fd = sock;
                            epoll_ctl(epfd, EPOLL_CTL_MOD, sock, &ev);
                        }
                        else if(s == 0)
                        {
                            cout << "link .... close" << endl;
                            close(sock);
                            epoll_ctl(epfd, EPOLL_CTL_DEL, sock, nullptr);
                        }
                        else
                        {
                            cout << "recv error!" << endl;
                            close(sock);
                            epoll_ctl(epfd, EPOLL_CTL_DEL, sock, nullptr);
                        }
                    }
                }
                else if(revs[i].events & EPOLLOUT)
                {
                    string http_echo = "HTTP/1.1 200 OK\r\n\r\n<html><body><h1> hello EPOLL Server </h1></body></html>";
                    send(sock, http_echo.c_str(), http_echo.size(), 0);
                    close(sock);
                    epoll_ctl(epfd, EPOLL_CTL_DEL, sock, nullptr);
                }
            }
        }
        void Run()
        {
            struct epoll_event ev;
            ev.events = EPOLLIN;
            ev.data.fd = lsock;
            epoll_ctl(epfd, EPOLL_CTL_ADD, lsock, &ev);
            struct epoll_event revs[128];
            for(;;)
            {
                int timeout = 1000;
                int num = 0;
                switch((num = epoll_wait(epfd, revs, 128, timeout)))
                {
                    case 0:
                        cout << "time out ..." << endl;
                        break;
                    case -1:
                        cout << "epoll wait error ..." << endl;
                        break;
                    default:
                        HanderEvents(epfd, revs, num);
                        break;
                }
            }
        }
        ~Server()
        {
            
        }
};
