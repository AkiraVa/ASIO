#ifndef THREADPOOL_H
#define THREADPOOL_H


#include <iostream>
#include <mutex>
#include <condition_variable>
#include <thread>
#include <list>
#include <chrono>
#include <future>

using namespace std;

template<typename T>
class SyncQueue{
    //检查是否为满
    bool isNotFull()
    {
        bool flag = (m_queue.size() == m_maxSize);
        if(flag)
            cout<<"buffer is full,wait... ID:"<<this_thread::get_id()<<endl;
        return !flag;
    }

    //检查是否为空
    bool isNotEmpty()
    {
        bool flag = (m_queue.size() == 0);
        if(flag)
            cout<<"buffer is empty,wait... ID:"<<this_thread::get_id()<<endl;
        return !flag;
    }

public:
    //构造函数的默认最大queue参数为CPU核数，以达到最优效率
    SyncQueue(int queue_size = thread::hardware_concurrency()):m_maxSize(queue_size),m_stop(false){}

    //同步服务层
    template<typename F>
    void put(F&& x)
    {
        std::unique_lock<mutex> locker(m_mutex);
        m_full_con.wait(locker,[this](){ return m_stop || isNotFull(); });
        if(m_stop)
            return;
        m_queue.push_back(forward<F>(x));
        m_empty_con.notify_one();
    }

    //异步服务层
    void get(T& x)
    {
       std::unique_lock<mutex> locker(m_mutex);
       m_empty_con.wait(locker,[this](){ return m_stop || isNotEmpty(); });
       if(m_stop)
           return;
       x = m_queue.front();
       m_queue.pop_front();
       m_full_con.notify_one();
    }

    void get(list<T>& list)
    {
        std::unique_lock<mutex> locker(m_mutex);
        m_empty_con.wait(locker, [this](){ return m_stop || isNotEmpty(); });
        if(m_stop)
            return;
        list = move(m_queue);
        m_full_con.notify_one();
    }

    //停止接口
    void stop()
    {
        lock_guard<mutex> locker(m_mutex);
        m_stop = true;
        m_empty_con.notify_all();
        m_full_con.notify_all();
    }

private:
    std::mutex m_mutex;
    std::condition_variable m_empty_con;
    std::condition_variable m_full_con;
    std::list<T> m_queue;
    int m_maxSize;
    bool m_stop;

};


template<typename T>
class ThreadPool
{
    //typedef void(*func)(void);

public:
    ThreadPool(int MaxNumThread = thread::hardware_concurrency()):m_queue(100)
    {
        Start(MaxNumThread);
    }
    //初始化线程组让其开始执行
     void Start(int MaxNumThread)
     {
         stop_flag = false;
         for(int i=0; i<MaxNumThread; ++i)
            thread_group.push_back(make_shared<thread>(&ThreadPool::Run,this));

     }
     //线程组执行函数
     void Run()
     {
        while(!stop_flag)
        {
            function<void(void)> task;
            m_queue.get(task);
            if(stop_flag)
                return;
            task();
        }
     }
    //添加任务事件
     template<typename U>
     void Add(U&& task)
     {
         m_queue.put(forward<U>(task));
     }
     //停止线程组工作
     void Stop()
     {
         static once_flag of;
         call_once(of,[&]()
         {
             m_queue.stop();
             stop_flag = true;
             for(auto tmp : thread_group)
                 tmp->join();
             thread_group.clear();

         });
     }

    ~ThreadPool()
    {
        Stop();
    }

private:
    SyncQueue<T> m_queue;
    list<shared_ptr<thread>> thread_group;
    atomic_bool stop_flag;
};

#endif // THREADPOOL_H
