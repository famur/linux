#pragma once

#include<iostream>
#include<stdlib.h>
#include<unistd.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<arpa/inet.h>
#include<netinet/in.h>
#include<sys/select.h>

#define SIZE sizeof(fd_set)*8
using namespace std;

class SelectServer{
    private:
        int port;
        int lsock;
    public:
        SelectServer(int _port = 8888):port(_port),lsock(-1)
         { }
        void InitServer()
        {
            lsock = socket(AF_INET, SOCK_STREAM, 0);
            if(lsock < 0)
            {
                cerr << "socket error" << endl;
                exit(2);
            }
            struct sockaddr_in local;
            local.sin_family = AF_INET;
            local.sin_addr.s_addr = htonl(INADDR_ANY);
            local.sin_port = htons(port);

            if(bind(lsock, (struct sockaddr*)&local, sizeof(local)) < 0)
            {
                cerr << "bind error" << endl;
                exit(3);
            }
            if(listen(lsock, 5) < 0)
            {
                cerr << "listen error" << endl;
                exit(4);
            }
            int opt = 1;
            setsockopt(lsock, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
        }
        void Run()
        {
            int fd_array[SIZE];
            int i = 0;
            for(; i < SIZE; i++)
            {
                fd_array[i] = -1;
            }
            fd_set rfds;
            fd_array[0] = lsock;
            int max = lsock;
            for(;;)
            {
                struct sockaddr_in peer;
                socklen_t len = sizeof(peer);
                struct timeval timeout = {5, 0};
                FD_ZERO(&rfds);
                for(i = 0 ; i < SIZE; i++)
                {
                    if(fd_array[i] == -1)
                    {
                        continue;
                    }

                    FD_SET(fd_array[i], &rfds);
                    if(fd_array[i] > max)
                    {
                        max = fd_array[i];
                    }
                }
               switch(select(max+1, &rfds, nullptr, nullptr, &timeout))
               {
                   case 0:
                       cout << "timeout ... " << endl;
                       break;
                   case -1:
                       cerr << "select error! " << endl;
                       break;
                   default:
                       for(i = 0; i < SIZE; i++)
                       {
                           if(fd_array[i] == -1)
                           {
                               continue;
                           }
                           if(FD_ISSET(fd_array[i], &rfds))
                           {
                               if(fd_array[i] == lsock)
                               {
                                   cout<<"losck..."<<endl;
                                     int fd = accept(lsock, (struct sockaddr*)&peer, &len);
                                     if(fd < 0)
                                     {
                                        cerr << "accept error!" << endl;
                                        continue;
                                     }
                                     cout << "get a new client!" << endl;
                                     int j = 1;
                                     for(; j < SIZE; j++)
                                     {
                                         if(fd_array[j] == -1)
                                         {
                                             break;
                                         }
                                     }
                                     if(j == SIZE)
                                     {
                                         close(fd);
                                         cout << "fd_array is full" << endl;
                                     }
                                     else
                                     {
                                         fd_array[j] = fd;
                                     }
                               }
                               else
                               {
                                   cout<<"sock..."<<endl;
                                    char buf[1024];
                                    ssize_t s = recv(fd_array[i], buf, sizeof(buf)-1, 0);
                                    if(s > 0)
                                    {
                                        buf[s] = 0;
                                        cout << "client# " << buf << endl;
                                    }
                                    else if(s == 0)
                                    {
                                        cout << "client quit!" << endl;
                                        close(fd_array[i]);
                                        fd_array[i] = -1;
                                    }
                                    else
                                    {
                                        cerr << "recv error" << endl;
                                        close(fd_array[i]);
                                        fd_array[i] = -1;
                                    }
                               }
                           }
                       }
                       break;

               }
            }
        }

        ~SelectServer()
        {
            if(lsock >= 0)
            {
                close(lsock);
            }
        }
};
