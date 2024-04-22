#include "../include/Buffer.h"
#include <stdio.h>
#include <stdlib.h>
#include <sys/uio.h>
#include <string.h>
#include <unistd.h>
#include <strings.h>
#include <sys/socket.h>

Buffer::Buffer(int size):m_capacity(size)
{
    m_data = (char*)malloc(size);
    memset(m_data, 0, size);
}


//释放m_data空间
Buffer::~Buffer()
{
    if (m_data != nullptr)
    {
        free(m_data);
    }
}



void Buffer::extendRoom(int size)
{
    if (writeableSize() >= size)
    {
        return;
    }
    // 剩余的可写的内存 + 已读的内存 > size   （内存合并够用）
    else if (m_readPos + writeableSize() >= size)
    {
        // 得到未读的内存大小
        int readable = readableSize();
        // 移动内存
        memcpy(m_data, m_data + m_readPos, readable);
        // 更新位置
        m_readPos = 0;
        m_writePos = readable;
    }
    // 3. 内存不够用 - 扩容
    else
    {
        void* temp = realloc(m_data, m_capacity + size);
        if (temp == NULL)
        {
            return; // 失败了
        }
        memset((char*)temp + m_capacity, 0, size);
        // 更新数据
        m_data = static_cast<char*>(temp);
        m_capacity += size;
    }
}

int Buffer::appendString(const char* data, int size)
{
    if (data == nullptr || size <= 0)
    {
        return -1;
    }
    // 扩容
    extendRoom(size);
    // 数据拷贝
    memcpy(m_data + m_writePos, data, size);
    m_writePos += size;
    return 0;
}

int Buffer::appendString(const char* data)
{
    int size = strlen(data);
    int ret = appendString(data, size);
    return ret;
}

int Buffer::appendString(const string data)
{
    int ret = appendString(data.data());
    return ret;
}

int Buffer::socketRead(int fd)
{
    struct iovec vec[2];
    //struct iovec {
    //    void* iov_base;  // 指向数据的起始地址
    //    size_t iov_len;   // 数据的长度
    //};

    //剩余可写内存大小
    int writeable = writeableSize();
    vec[0].iov_base = m_data + m_writePos;
    vec[0].iov_len = writeable;
    char* tmpbuf = (char*)malloc(40960);
    vec[1].iov_base = tmpbuf;
    vec[1].iov_len = 40960;
    int result = readv(fd, vec, 2);
    if (result == -1)
    {
        return -1;
    }
    else if (result <= writeable)
    {
        m_writePos += result;
    }
    else
    {
        m_writePos = m_capacity; //表示可写内存已被占满
        appendString(tmpbuf, result - writeable);  
        //buffer的缓冲区用完了，剩余的数据肯定存在了临时缓冲区tempbuf里
        //先扩容，再把剩余的的result - writeable个字节内容拷贝到m_data里面
    }
    free(tmpbuf);
    return result;
}


//找到\r\n位置
char* Buffer::findCRLF()
{
    char* ptr = (char*)memmem(m_data + m_readPos, readableSize(), "\r\n", 2);
    return ptr;
}

int Buffer::sendData(int socket)
{
    // 判断有无数据
    int readable = readableSize();
    if (readable > 0)
    {
        int count = send(socket, m_data + m_readPos, readable, MSG_NOSIGNAL);  //表示在发送数据时不产生 SIGPIPE 信号
        if (count > 0)
        {
            m_readPos += count;
            usleep(10);
        }
        else 
        {
            perror("send");
        }
        return count;
    }
    return 0;
}

