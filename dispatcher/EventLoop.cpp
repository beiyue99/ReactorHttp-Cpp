#include "../include/EventLoop.h"
#include <assert.h>
#include <sys/socket.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "../include/SelectDispatcher.h"
#include "../include/PollDispatcher.h"
#include "../include/EpollDispatcher.h"

EventLoop::EventLoop() : EventLoop(string())
{
}



EventLoop::EventLoop(const string threadName)
{
    m_isQuit = true;    // 默认没有启动
    m_threadID = this_thread::get_id();
    m_threadName = threadName == string() ? "MainThread" : threadName;
    m_dispatcher = new EpollDispatcher(this);
    // map
    m_channelMap.clear();

}

EventLoop::~EventLoop()
{
}

int EventLoop::run()
{
    m_isQuit = false;
    // 比较线程ID是否正常
    if (m_threadID != this_thread::get_id())
    {
        return -1;
    }
    // 循环进行事件处理
    while (!m_isQuit)
    {
        m_dispatcher->dispatch();    // 超时时长 2s
        processTaskQ();
    }
    return 0;
}




int EventLoop::eventActive(int fd, int event)
{
    if (fd < 0)
    {
        return -1;
    }
    // 取出channel
    Channel* channel = m_channelMap[fd];
    assert(channel->getSocket() == fd);
    if (event & (int)FDEvent::ReadEvent && channel->readCallback)
    {
        channel->readCallback(const_cast<void*>(channel->getArg()));
    }
    if (event & (int)FDEvent::WriteEvent && channel->writeCallback)
    {
        channel->writeCallback(const_cast<void*>(channel->getArg()));
    }
    return 0;
}

int EventLoop::addTask(Channel* channel, ElemType type)
{
    // 加锁, 保护共享资源
    m_mutex.lock();
    // 创建新节点
    ChannelElement* node = new ChannelElement;
    node->channel = channel;
    node->type = type;
    m_taskQ.push(node);
    m_mutex.unlock();

    if (m_threadID == this_thread::get_id())
    {
        processTaskQ();
    }

    return 0;
}


//处理任务队列的任务
int EventLoop::processTaskQ()
{
    // 取出头结点
    while (!m_taskQ.empty())
    {
        m_mutex.lock();
        ChannelElement* node = m_taskQ.front();
        m_taskQ.pop();  // 删除节点
        m_mutex.unlock();
        Channel* channel = node->channel;
        //调用add
        if (node->type == ElemType::ADD)
        {
            // 添加
            add(channel);
        }
        else if (node->type == ElemType::DELETE)
        {
            // 删除
            remove(channel);
        }
        else if (node->type == ElemType::MODIFY)
        {
            // 修改
            modify(channel);
        }
        delete node;
    }
    return 0;
}

int EventLoop::add(Channel* channel)
{
    int fd = channel->getSocket();
    // 找到fd对应的数组元素位置, 并存储
    if (m_channelMap.find(fd) == m_channelMap.end())
    {
        m_channelMap.insert(make_pair(fd, channel));
        m_dispatcher->setChannel(channel);
        int ret = m_dispatcher->add();
        return ret;
    }
    return -1;
}

int EventLoop::remove(Channel* channel)
{
    int fd = channel->getSocket();
    if (m_channelMap.find(fd) == m_channelMap.end())
    {
        return -1;
    }
    m_dispatcher->setChannel(channel);
    int ret = m_dispatcher->remove();
    return ret;
}

int EventLoop::modify(Channel* channel)
{
    int fd = channel->getSocket();
    if (m_channelMap.find(fd) == m_channelMap.end())
    {
        return -1;
    }
    m_dispatcher->setChannel(channel);
    int ret = m_dispatcher->modify();
    return ret;
}




//删除channel，关闭socket
int EventLoop::freeChannel(Channel* channel)
{
    auto it = m_channelMap.find(channel->getSocket());
    if (it != m_channelMap.end())
    {
        m_channelMap.erase(it);
        close(channel->getSocket());
        delete channel;
    }
    return 0;
}


