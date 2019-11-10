#include "SelectServer.hpp"

int main(int argc, char *argv[])
{
    if(argc != 2)
    {
        cout << "Usage: " << argv[0] << " port" << endl;
        exit(1);
    }
    SelectServer *sp = new SelectServer(atoi(argv[1]));
    sp->InitServer();
    sp->Run();
    delete sp;
    return 0;
}
