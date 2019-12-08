#pragma once

#include<iostream>
#include<unistd.h>
#include<string>
#include<sstream>
#include<unordered_map>
#include<vector>
#include<stdlib.h>
#include<netinet/in.h>
#include<arpa/inet.h>
#include<sys/stat.h>
#include<utility>
#include<sys/types.h>
#include<sys/socket.h>
#include<sys/sendfile.h>
#include<fcntl.h>
#include"Util.hpp"

using namespace std;
#define WWW "./wwwroot"

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

        string path;
        string query_string;
        bool cgi;

        int recource_size;
        string suffix;
    public:
        HttpRequest():request_blank("\n"),path(WWW),recource_size(0),cgi(false)
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
        string &GetPath()
        {
            return path;
        }
        int GetRecourceSize()
        {
            return recource_size;
        }
        string GetSuffix()
        {
            return suffix;
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
            for(; it != v.end(); it++)
            {
                string k;
                string v;
                Util::MakeKV(*it, k, v);
                header_kv.insert(make_pair(k, v));
                //cout << "key :" << k << endl;
                //cout << "value :" << v << endl;
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

        void UriParse()
        {
            if(method == "POST")
            {
                cgi = true;
                path += uri;
            }
            else
            {
                size_t pos = uri.find("?");
                if(string::npos == pos)
                {
                    path += uri;
                }
                else
                {
                    cgi = true;
                    path += uri.substr(0, pos);
                    query_string = uri.substr(pos + 1);
                }
            }
            if(path[path.size()-1] == '/')
            {
                path += "index.html";
            }
            size_t pos = path.rfind(".");
            if(string::npos == pos)
            {
                suffix = ".html";
            }
            else
            {
                suffix = path.substr(pos);
            }
        }
        bool IsPathLegal()
        {
            struct stat st;
            if(stat(path.c_str(), &st) == 0)
            {
                if(st.st_mode & S_IFDIR)
                {
                    path += "/index.html";
                }
                else
                {
                    if((st.st_mode & S_IXUSR) || (st.st_mode & S_IXGRP) || (st.st_mode & S_IXOTH)) 
                    {
                        cgi = true;
                    }
                }
                recource_size = st.st_size;
                return true;
            }
            else
            {
                return false;
            }
        }
        bool IsCgi()
        {
            return cgi;
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

    private:
        int fd;
        int size;
    public:
        HttpResponse():response_blank("\r\n"),fd(-1)
        {

        }
        void MakeResponseLine(int code)
        {
            response_line = "HTTP/1.0";
            response_line += " ";
            response_line += Util::IntToString(code);
            response_line += " ";
            response_line += Util::CodeToDec(code);
            response_line += "\r\n";
        }
        void MakeResponseHeader(vector<string> &v)
        {
            auto it = v.begin();
            for(; it != v.end(); it++)
            {
                response_header += *it;
                response_header += "\r\n";
            }
        }
        void MakeResponseBody();
        void MakeResponse(HttpRequest *rq, int code)
        {
            MakeResponseLine(code);
            string suffix = rq->GetSuffix();
            size = rq->GetRecourceSize();
            vector<string> v;
            string s = Util::SuffixToType(suffix);
            v.push_back(s);
            string ct = "Content-Length: ";
            ct += Util::IntToString(size);
            v.push_back(ct); 
            MakeResponseHeader(v);
            string path = rq->GetPath();
            fd = open(path.c_str(), O_RDONLY);
            //MakeResponseBody();
        }
        string &GetResponseLine()
        {
            return response_line;
        }
        string &GetResponseHeader()
        {
            return response_header;
        }
        string &GetResponseBlank()
        {
            return response_blank;
        }
        int GetFd()
        {
            return fd;
        }
        int GetRecourceSize()
        {
            return size;
        }

        ~HttpResponse()
        {
            if(fd != -1)
            {
                close(fd);
            }
        }
};

class EndPoint{
    private:
        int sock;
    public:
        EndPoint(int _sock):sock(_sock)
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
                    line.push_back(x);
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
            cout << "body: " << body << endl;
        }
        void SendResponse(HttpResponse *rsp, bool cgi)
        {
            if(cgi)
            {

            }
            else
            {
                string &response_line = rsp->GetResponseLine();
                string &response_header = rsp->GetResponseHeader();
                string &response_blank = rsp->GetResponseBlank();
                int fd = rsp->GetFd();
                int size = rsp->GetRecourceSize();

                send(sock, response_line.c_str(), response_line.size(), 0);
                send(sock, response_header.c_str(), response_header.size(), 0);
                send(sock, response_blank.c_str(), response_blank.size(), 0);
                sendfile(sock, fd, NULL, size);
            }
        }
        ~EndPoint()
        {
            close(sock);
        }
};
class Entry
{
    public:

        static void ProcessCgi(HttpRequest*rq, HttpResponse*rsp, EndPoint* ep)
        {

        }
        static void *HandlerRequest(void *args)
        {
            int code = 200;
            int *p = (int*)args;
            int sock = *p;
            EndPoint *ep = new EndPoint(sock);
            HttpRequest *rq = new HttpRequest();
            HttpResponse *rsp = new HttpResponse();
            
            ep->RecvRequestLine(rq);
            rq->RequestLineParse();
            if(!rq->MethodIsLegal())
            {
                code = 404;
                goto end;
            }
            ep->RecvRequestHeader(rq);
            rq->RequestHeaderParse();
            if(rq->IsNeedRecv())
            {
                ep->RecvRequestBody(rq);
            }

            rq->UriParse();
            if(!rq->IsPathLegal())
            {
                //code = 404;
                goto end;
            }
            if(rq->IsCgi())
            {
                ProcessCgi(rq, rsp, ep);
            }
            else
            {   
                rsp->MakeResponse(rq, code);
                ep->SendResponse(rsp, false);
            }
end:
            delete ep;
            delete rq;
            delete rsp;
            delete p;
        }

};
