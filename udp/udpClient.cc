#include<iostream>
#include<string>
#include<unistd.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<arpa/inet.h>
#include<cstring>
#include<stdlib.h>

void Usage(std::string proc)
{
    std::cerr << "Usage: "<< proc << " SERVER_IP  SERVER_PORT" << std::endl;
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

    struct sockaddr_in server;
    server.sin_family = AF_INET;
    server.sin_port = htons(atoi (argv[2]));
    server.sin_addr.s_addr = inet_addr(argv[1]);
    
    char buf[1024];
    for(;;)
    {
        std::cout << "please enter# ";
        std::cin >> buf;
        sendto(sock, buf, strlen(buf), 0, (struct sockaddr*)&server, sizeof(server));
        struct sockaddr_in peer;
        socklen_t len = sizeof(peer);
        ssize_t s = recvfrom(sock, buf, sizeof(buf)-1, 0, (struct sockaddr*)&peer, &len);
        if(s > 0)
        {
            buf[s] = 0;
            std::cout << "server echo# " << buf<< std::endl;
        }
    }

    close(sock);
    return 0;
}
