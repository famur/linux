#pragma once

#include<iostream>
#include<unistd.h>
#include<sstream>
#include<stdlib.h>
#include<netinet/in.h>
#include<arpa/inet.h>
#include<sys/types.h>
#include<sys/socket.h>

using namespace std;

class HttpRequest{
    private:
        string request_line;
        string request_header;
        string request_blank;
        string request_body;

    private:
        string method;
        string uri;
        string version;
    public:
        HttpRequest()
        {

        }
        string &GetRequestLine()
        {
            return request_line;
        }
        void RequestLineParse()
        {
            stringstream ss(request_line);
            ss >> method >> uri >> version;
            cout << "method : " << method << endl;
            cout << "uri    : " << uri << endl;
            cout << "version: " << version<< endl;
        }
        bool MethodIsLegal()
        {

        }
        ~HttpRequest()
        {

        }
};

class HttpResponse{
    private:
        string response_line;
        string response_header;
        string response_blank;
        string response_body;
    public:
        HttpResponse()
        {

        }
        ~HttpResponse()
        {
        }
};

class EndPoint{
    private:
        int sock;
    public:
        EndPoint(int sock)
        {

        }
        int RecvLine(string &line)
        {
            char x = 'k';
            while(x != '\n')
            {
                ssize_t s = recv(sock, &x, 1, 0);
                if(s > 0)
                {
                    if(x == '\r')
                    {
                        if(recv(sock, &x, 1, MSG_PEEK) > 0)
                        {
                            if(x == '\n')
                            {
                                recv(sock, &x, 1, 0);
                            }
                            else
                            {
                                x = '\n';
                            }
                        }
                        else
                        {
                            x = '\n';
                        }
                    }
                    else
                    {
                        line.push_back(x);
                    }
                }
                else
                {
                    x = '\n'
                    line.push_back(x);
                }
            }
            return line.size();
        }
        void RecvRequestLine(HttpRequest *rq)
        {
            RecvLine(rq->GetRequestLine());
        }
        ~EndPoint()
        {
        }
};
class Entry
{
    public:
        static void *HandlerRequest(void *args)
        {
            int *p = (int*)args;
            int sock = *p;
            EndPoint *ep = new EndPoint(sock);
            HttpRequest *rq = new HttpRequest();
            HttpResponse *rsp = new HttpResponse();
            
            ep->RecvRequestLine(rq);
            rq->RequestLineParse();
            if(!rq->MethodIsLegal)
            {

            }
            

            delete ep;
            delete rq;
            delete rsp;
            delete p;

        }

};
