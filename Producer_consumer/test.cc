#include<iostream>
#include<time.h>
#include<unistd.h>
#include<pthread.h>
#include<stdlib.h>
#include<queue>

using namespace std;

class BlockQueue
{
private:

    queue<int> q;
    int cap;
    pthread_mutex_t lock;
    pthread_cond_t p_cond;
    pthread_cond_t c_cond;

private:

   void LockQueue()
    {
        pthread_mutex_lock(&lock);
    }

   void UnlockQueue()
    {
        pthread_mutex_unlock(&lock);
    }

    bool QueueIsFull()
    {
        return q.size() == cap;
    }
    
    bool QueueIsEmpty()
    {
        return q.size() == 0;
    }

    void ProductWait()
    {
        cout << " product wait.." << endl;
        pthread_cond_wait(&p_cond, &lock);
    }

    void ConsumerWait()
    {
        cout << " consumer wait.." <<endl;
        pthread_cond_wait(&c_cond, &lock);
    }

    void SignalProducter()
    {   
        cout << " product signal P" << endl;
        pthread_cond_signal(&p_cond);
    }

    void SignalConsumer()
    {   
        cout << " consumer signal C" << endl;
        pthread_cond_signal(&c_cond);
    }

public:
    BlockQueue(int cap_ = 32):cap(cap_)
    {
        pthread_mutex_init(&lock, NULL);
        pthread_cond_init(&c_cond, NULL);
        pthread_cond_init(&p_cond, NULL);
    }

    ~BlockQueue()
    {
        pthread_mutex_destroy(&lock);
        pthread_cond_destroy(&c_cond);
        pthread_cond_destroy(&p_cond);
    }

    void PushData(int data)
    {   
        LockQueue();
        if(QueueIsFull())
        {
            cout << " queue is full " << endl;
            SignalConsumer();
            ProductWait();
        }
        q.push(data);
        UnlockQueue();
    }

    void PopData(int &out)
    {
        LockQueue();
        if(QueueIsEmpty())
        {
            cout << " queue is empty " << endl;
            SignalProducter();
            ConsumerWait();
        }
        out = q.front();
        q.pop();
        UnlockQueue();
    }

};

void *consumer(void *arg)
{
    int data;
    BlockQueue *bq = (BlockQueue*)arg;
    while(1)
    {
        bq->PopData(data);
        cout << "consumer data is : " << data << endl;
    }
}

void *producter(void *arg)
{
    BlockQueue *bq = (BlockQueue*)arg;
    while(1)
    {
        int data = rand()%100 + 1;
        bq->PushData(data);
        cout<<"product data is :"<< data <<endl;
        sleep(2);
    }
}

int main()
{
    srand((unsigned long)time(NULL));
    BlockQueue *bq = new BlockQueue(4);
    pthread_t t1,t2;
    
    pthread_create(&t1,NULL, consumer, (void*)bq);
    pthread_create(&t2,NULL, producter, (void*)bq);
    

    pthread_join(t1, NULL);
    pthread_join(t2, NULL);

}
