#include<iostream>
#include<queue>
#include<unistd.h>
#include<pthread.h>

using namespace std;

class Task{
    private:
        int x;
        int y;
    public:
        Task()
        {
            x = 0;
            y = 0;
        }
        Task(int _x, int _y):x(_x), y(_y)
        {

        }
        int Run()
        {
            return x + y;
        }
        ~Task()
        {

        }
};

class ThreadPool{
    private:
        int num;
        queue<Task> q;
        pthread_mutex_t lock;
        pthread_cond_t cond;
    public:
        void LockQueue()
        {
            pthread_mutex_lock(&lock);
        }
        void UnlockQueue()
        {
            pthread_mutex_lock(&lock);
        }
        bool IsQueueEmpty()
        {
            return q.size() == 0 ? true : false;
        }
        void ThreadWait()
        {
            pthread_cond_wait(&cond, &lock);
        }
        void PopTask(Task &t)
        {
            t = q.front();
            q.pop();
        }
        void PushTask(Task &t)
        {
            q.push(t);
        }
        void NotifyThread()
        {
            pthread_cond_signal(&cond);
        }
    public:
        ThreadPool(int num_ = 6):num(num_)
        {
            pthread_mutex_init(&lock, NULL);
            pthread_cond_init(&cond, NULL);
        }
      static void *HandlerTask(void *arg)
        {
            pthread_detach(pthread_self());
            ThreadPool *tp = (ThreadPool*)arg;
            for(;;)
            {
               tp-> LockQueue();
                if(tp->IsQueueEmpty())
                {
                   tp->ThreadWait();
                }
                Task t;
                tp->PopTask(t);
                tp->UnlockQueue();
                int result = t.Run();
        cout << "Thread : " << pthread_self() << ", result : " << result <<endl;
            }
        }

        void InitThreadPool()
        {
            int i = 0;
            pthread_t pid;
            for(; i < num; i++)
            {
                pthread_create(&pid, NULL, HandlerTask, (void*)this);
            }
        }
        void AddTask(Task &t)
        {
            LockQueue();
            PushTask(t);
            UnlockQueue();
            NotifyThread();
        }
        ~ThreadPool()
        {
            pthread_mutex_destroy(&lock);
            pthread_cond_destroy(&cond);
        }
};
int main()
{
    ThreadPool *tp = new ThreadPool(5);
    tp->InitThreadPool();
    int count = 0;
    for(;;)
    {
        int x = count % 1000 + 100;
        int y = count % 2000 + 300;
        count++;
        Task t(x, y);
        tp->AddTask(t);
        sleep(1);
    }
    delete tp;
}
