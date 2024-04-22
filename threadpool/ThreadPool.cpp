#include "../include/ThreadPool.h"
#include <assert.h>
#include <stdlib.h>

ThreadPool::ThreadPool(EventLoop* mainLoop, int count)
{
    m_index = 0;
    m_isStart = false;
    m_mainLoop = mainLoop;
    m_threadNum = count;
    m_workerThreads.clear();
}

ThreadPool::~ThreadPool()
{
    for (auto item : m_workerThreads)
    {
        delete item;
    }
}

//创建线程池里面的线程，并启动他们
void ThreadPool::run()
{
    assert(!m_isStart);
    if (m_mainLoop->getThreadID() != this_thread::get_id())
    {
        exit(0);
    }
    m_isStart = true;
    if (m_threadNum > 0)
    {
        for (int i = 0; i < m_threadNum; ++i)
        {
            WorkerThread* wjfThread = new WorkerThread(i);
            wjfThread->run();
            m_workerThreads.push_back(wjfThread);
        }
    }
}

EventLoop* ThreadPool::takeWorkerEventLoop()
{
    assert(m_isStart);
    if (m_mainLoop->getThreadID() != this_thread::get_id())
    {
        exit(0);
    }
    // 从线程池中找一个子线程, 然后取出里边的反应堆实例
    EventLoop* evLoop = m_mainLoop;
    if (m_threadNum > 0)
    {
        evLoop = m_workerThreads[m_index]->getEventLoop();
        m_index = ++m_index % m_threadNum;  //循环调度
    }
    return evLoop;  //没有worker返回主反应堆实例
}
