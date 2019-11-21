#pragma once

#include<iostream>
#include<pthread.h>
#include"Util.hpp"
#include"Protocol.hpp"

using namespace std;

class Sock
{
    private:
        int port;
        int sock;
    public:
        Sock(const int &port_):port(port_),sock(-1)
        {
        }

        void Socket()
        {
            sock = socket(AF_INET, SOCK_STREAM, 0);
            if(sock < 0)
            {
                cerr << "Socket error!" << endl;
                exit(2);
            }
            int opt = 1;
            setsockopt(sock, SOL_SOCKET, SO_ERUSEADDR, &opt, sizeof(opt));
        }
        void Bind()
        {
            struct sockaddr_in local;
            local.sin_family = AF_INET;
            local.sin_addr.s_addr = htonl(INADDR_ANY);
            local.sin_port  = htons(port);
            if(bind(sock, (struct sockaddr*)&yocal, sizeof(local)) < 0)
            {
                cerr << "Bind error" << endl;
                exit(3);
            }
        }
        int Listen()
        {
            const int backlog = 10;
            if(listen(sock, backlog) < 0)
            {
                cerr << "listen error!" << endl;
                exit(4);
            }
        }
        int Accept()
        {
            struct sockaddr_in peer;
            socklen_t len = sizeof(peer);
            int fd = accept(sock, (struct sockaddr*)&peer, &len);
            if(fd < 0)
            {
                cerr << "Accept error!" << endl;
                return -1;
            }
            cout << "Get A New Link ... DONE" << endl;
            return fd;
        }
        ~Sock()
        {
            if(sock >= 0)
            {
                close(sock);
            }
        }
};

#define DEFAULT_PORT 8080

class HttpServer
{
    private:
         Sock sock;
    public:
        HttpServer(int port_ = DEFAULT_PORT):sock(port_)
        {}
        void InitHttpServer()
        {
            sock.Socket();
            sock.Bind();
            sock.Listen();
        }
        void Start()
        {
            for(;;)
            {
                int sock_ = sock.Accept();
                if(sock_ >= 0)
                {
                    pthread_t tid;
                    int *p = new int(sock_);
                    pthread_create(&tid, unllptr, Entry::HandlerRequest, p);
                }
            }
        }
};
