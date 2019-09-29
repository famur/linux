#include<iostream>
#include<pthread.h>
#include<unistd.h>

using namespace std;

pthread_cond_t cond;
pthread_mutex_t mutex;


void *Wait(void *arg)
{
    while(1)
    {
        pthread_cond_wait(&cond,&mutex);
        cout << "active..." <<endl;
    }

}

void *Signal(void *arg)
{
    while(1)
    {
        pthread_cond_signal(&cond);
        sleep(1);
    }

}

int main()
{
    
    pthread_cond_init(&cond,NULL);
    pthread_mutex_init(&mutex,NULL);
    
    pthread_t p1,p2;
    pthread_create(&p1,NULL,Wait,NULL);
    pthread_create(&p2,NULL,Signal,NULL);

    pthread_join(p1,NULL);
    pthread_join(p2,NULL);

    pthread_mutex_destroy(&mutex);
    pthread_cond_destroy(&cond);

}

