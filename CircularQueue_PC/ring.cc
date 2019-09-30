#include<iostream>
#include<semaphore.h>
#include<vector>
#include<time.h>
#include<stdlib.h>

using namespace std;

class RingQueue{
    private:
        vector<int> queue;
        int cap;
        sem_t sem_blank;
        sem_t sem_data;
        int c_step;
        int p_step;
    private:
        void P(sem_t &s)
        {
            sem_wait(&s);
        }
        void V(sem_t &s)
        {
            sem_post(&s);
        }
    public:
        RingQueue(int cap_ = 1024):cap(cap_),queue(cap_)
        {   
            c_step = p_step = 0;
            sem_init(&sem_blank, 0, cap);
            sem_init(&sem_data, 0 , 0);
        }
        //consumer
        void PopData(int &data)
        {
            P(sem_data);
            data = queue[c_step];
            c_step++;
            c_step %= cap;
            V(sem_blank);
        }
        //producter
        void PushData(const int &data)
        {
            P(sem_blank);
            queue[p_step] = data;
            p_step++;
            p_step %= cap;
            V(sem_data);
        }
        ~RingQueue()
        {
            sem_destroy(&sem_blank);
            sem_destroy(&sem_data);
        }
};

void *consumerRun(void *arg)
{
    int data;
    RingQueue *rq = (RingQueue*)arg;
    while(1)
    {
        rq->PopData(data);
        cout << "consumer data is : " << data << endl;
    }
}

void *producterRun(void *arg)
{
    
    RingQueue *rq = (RingQueue*)arg;
    while(1)
    {
        int data = rand() % 100 + 1;
        rq->PushData(data);
        cout << "producter data is : " << data << endl;
    }
}

int main()
{
    srand((unsigned long)time(NULL));
    RingQueue *rq = new RingQueue(8);
    pthread_t c,p;
    pthread_create(&c, NULL, consumerRun, (void*)rq);
    pthread_create(&p, NULL, producterRun, (void*)rq);

    pthread_join(c, NULL);
    pthread_join(p, NULL);
    delete rq;
    return 0;
}
