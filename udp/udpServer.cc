#include<iostream>
#include<string>
#include<unistd.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<arpa/inet.h>
#include<cstring>
#include<stdlib.h>
#include<unordered_map>

std::unordered_map<std::string, std::string> dict = 
{
    {"apple", "苹果" },
    {"kiwi fruit", "猕猴桃"},
    {"pear", "梨"},
    {"banana", "香蕉"}
};
void Usage(std::string proc)
{
    std::cerr << "Usage: "<< proc << " IP PORT" << std::endl;
    std::cerr << "Version: 0.0.1" <<std::endl;
}

int main(int argc, char*argv[])
{
    if(argc != 3)
    {
        Usage(argv[0]);
        return 1;
    }
    int sock = socket(AF_INET, SOCK_DGRAM, 0);
    if(sock < 0)
    {
        std::cerr << "socket error" << std::endl;
        return 2;
    }

    std::cout << "sock: " << sock << std::endl;
    struct sockaddr_in local;
    local.sin_family = AF_INET;
    local.sin_port = htons(atoi (argv[2]));
    local.sin_addr.s_addr = inet_addr(argv[1]);
    
    if(bind(sock, (struct sockaddr*)&local, sizeof(local)) < 0)
    {
        std::cerr << "bin error" << std::endl;
        return 3;
    }

    char buf[1024];
    for(;;)
    {
        struct sockaddr_in peer;
        socklen_t len = sizeof(peer);
        ssize_t s = recvfrom(sock, buf, sizeof(buf)-1, 0, (struct sockaddr*)&peer, &len);
        if(s > 0)
        {
            buf[s] = 0;
            std::cout << "client# " << buf << std::endl;
            std::string key = buf;
            std::string value;
            auto it = dict.find(key);
            if(it != dict.end())
            {
                value = it->second;
            }
            else
            {
                value = "Unknow";
            }
            sendto(sock, value.c_str(), value.size(), 0, (struct sockaddr*)&peer, len);
        }
    }

    close(sock);
    return 0;
}
