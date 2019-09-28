#include<stdio.h>
#include<pthread.h>
#include<unistd.h>

int tickets=200;
pthread_mutex_t lock;

void *GetTickets(void *args)
{ 
    while(1)
    {
        pthread_mutex_lock(&lock);
        if(tickets > 0)
        {
            usleep(100000);
            printf("get 1 ticket no. is : %d\n", tickets--);
            pthread_mutex_unlock(&lock);
        }
        else
        {   
            printf("%s ... quit\n", (char*)args);
            pthread_mutex_unlock(&lock);
            break;
        }
    }
    pthread_exit((void*)0);
}

int main ()
{
    pthread_t tid1, tid2, tid3, tid4;
    pthread_mutex_init(&lock, NULL);

    pthread_create(&tid1, NULL, GetTickets, "thread 1");
    pthread_create(&tid2, NULL, GetTickets, "thread 2");
    pthread_create(&tid3, NULL, GetTickets, "thread 3");
    pthread_create(&tid4, NULL, GetTickets, "thread 4");

    pthread_join(tid1, NULL);
    pthread_join(tid2, NULL);
    pthread_join(tid3, NULL);
    pthread_join(tid4, NULL);

    pthread_mutex_destroy(&lock);

}
