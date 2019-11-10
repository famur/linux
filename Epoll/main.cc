#include"epoll.hpp"

int main(int argc, char *argv[])
{
    if(argc != 2)
    {
        cout << "Usage: " << argv[0] << " port" << endl;
        exit(1);
    }
    Server *sp = new Server(atoi(argv[1]));
    sp->InitServer();
    sp->Run();
    return 0;
}
