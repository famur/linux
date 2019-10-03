#include<isotream>
#include<queue>
using namespace std;

class Task{
    private:
        int x;
        int y;
    public:
        Task()
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

class ThreaPool{
    private:
        int mun;
        queue<Task> q;
        thread_mutex_t lock;
        thread_cond_t cond;
    pubilc:
        ThreadPoll()
        {

        }
        void *HandlerTask(void *arg)
        {
            for(;;)
            {
                LockQueue();
                if(IsQueueEmpty())
                {
                    ThreadWait();
                }
                Task t;
                POPTask(t);
                UnlockQueue();
                int result = t.Run();
                cout << "Thread : " << pthread_self() << ", result : " << result << endl;
            }
        }

        void InitThreadPool()
        {
            int i = 0;
            pthread_t pid;
            for(; i < mun; i++)
            {
                pthread_create(&tid, NULL, HandlerTask, NULL);
            }
        }
        void PushTask(Task &t)
        {

        }
        ~ThreadPool()
        {

        }
}
int main()
{
    return 0;
}
