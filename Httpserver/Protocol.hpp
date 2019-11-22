#pragma once

#include<iostream>
#include<unistd.h>
#include<sstream>
#include<unordered_map>
#include<vector>
#include<stdlib.h>
#include<netinet/in.h>
#include<arpa/inet.h>
#include<sys/types.h>
#include<sys/socket.h>
#include"Util.hpp"

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
        unordered_map<string,string> header_kv;

    public:
        HttpRequest():request_blank("\n")
        {
        }
        string &GetRequestLine()
        {
            return request_line;
        }

        string &GetRequestHeader()
        {
            return request_header;
        }
        string &GetRequestBody()
        {
            return request_body;
        }
        void RequestLineParse()
        {
            stringstream ss(request_line);
            ss >> method >> uri >> version;
            Util::StringToUpper(method);
            cout << "method : " << method << endl;
            cout << "uri    : " << uri << endl;
            cout << "version: " << version<< endl;
        }
        bool MethodIsLegal()
        {
            if(method != "GET" && method != "POST")
            {
                return false;
            }
            return true;
        }

        void RequestHeaderParse()
        {
            vector<string> v;
            Util::TansfromToVector(request_header, v);
            auto it = v.begin();

            for(; it!= v.end(); it++)
            {
                string k;
                string v;
                Util:: MakeKV(*it, k, v);
                header_kv.insert(make_pair(k,v));
            }
        }

        bool IsNeedRecv()
        {
            return method == "POST";
        }

        int GetContentLength()
        {
            auto it = header_kv.find("Content-Length");
            if(it == header_kv.end())
            {
                return -1;
            }
            return Util::StringToInt(it->second);
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
        HttpResponse():response_blank("\n")
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
        EndPoint(int _sock)
            :sock(_sock)
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
                    x = '\n';
                    line.push_back(x);
                }
            }
            return line.size();
        }
        void RecvRequestLine(HttpRequest *rq)
        {
            RecvLine(rq->GetRequestLine());
        }

        void RecvRequestHeader(HttpRequest *rq)
        {
            string &hp = rq->GetRequestHeader();
            do
            {
                string line = "";
                RecvLine(line);
                if(line == "\n")
                {
                    break;
                }
                hp += line;
            }while(1);
        }
        void RecvRequestBody(HttpRequest *rq)
        {
            int len = rq->GetContentLength();
            string &body = rq->GetRequestBody();
            char c;
            while(len--)
            {
                if(recv(sock, &c, 1, 0) > 0)
                {
                    body.push_back(c);
                }
            }
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
            if(!rq->MethodIsLegal())
            {
                goto end;
            }
            ep->RecvRequestHeader(rq);
            rq->RequestHeaderParse();
            if(rq->IsNeedRecv())
            {
                ep->RecvRequestBody(rq);
            }

end:
            delete ep;
            delete rq;
            delete rsp;
            delete p;

        }

};
