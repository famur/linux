#include<iostream>
#include<cstring>
#include<cstdio>
#include<string>
#include<unistd.h>
#include<stdlib.h>
#include<sys/wait.h>
#include<sys/types.h>

#define NUM 32
using namespace std;

int main()
{
    char buff[1024]={0};
    for(;;)
    {
        string tips = "[xxxlocalhost YYY]# ";
        cout << tips;
        fgets(buff, sizeof(buff)-1, stdin);
        buff[strlen(buff)-1] = 0;

        char *argv[NUM];
        argv[0] = strtok(buff," ");
        int i = 0;
        while(argv[i] != NULL)
         {
            i++;
            argv[i] = strtok(NULL," ");
         }
        pid_t id = fork();
        if(id == 0)
        {
            cout << "child running ..." << endl;
            execvp(argv[0],argv);
            exit(123);
        }
        else
        {
            int status = 0;
            waitpid(id, &status, 0);
            cout << "Exit Code: " << WEXITSTATUS(status) << endl;
        }
    }
    return 0;
}
