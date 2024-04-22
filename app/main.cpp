#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include "../include/TcpServer.h"
#include<cstring>
#include<string.h>
int main(int argc, char* argv[])
{
#if 1
    if (argc < 3)
    {
        printf("./a.out port path\n");
        return -1;
    }
    unsigned short port = atoi(argv[1]);
    // 切换服务器的工作路径
    chdir(argv[2]);
#else
    unsigned short port = 8080;
    //切换工作目录
    chdir("./web-http");
#endif
    // 启动服务器
    TcpServer* server = new TcpServer(port, 4);
    server->run();

    return 0;
}
