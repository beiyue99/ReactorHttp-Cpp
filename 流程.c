



先new一个TcpServer，构造函数new出一个主线程的
m_mainLoop,每个Loop构造的时候都会new出一个自己
的调度者，如epoll调度者的构造函数初始化epoll根，
然后new出线程池实例，最后创建绑定监听m_lfd
紧接着调用TcpServer的run
1内部先调用线程池的run，
循环threadnum次，每次new出WorkerThread对象，
以序号命名，然后调用它们的run，其实就是真正的
创建线程，线程函数为running，然后会被条件变量
卡住，等待running为每个子线程new出evLoop之后
才返回。每个子线程调用evLoop的run,其实就是使
调度者启用调度器开始检测，如果epoll_wait检测
到激活的fd，会根据读写调用eventActive处理，具体
处理方法就是对应的读写回调函数。
然后调用processTaskQ处理fd相关任务（添加，删除，
修改）通过调度者的epollCtl函数操作
2再new出m_lfd的channel，加入主线程任务队列，然后
调用processTasKQ处理（添加到epoll树上）,然后启用
主反应堆的run,调度者启用调度器，这个fd只有读事件，
即新用户连入时调用accept连接读回调函数。主线程会
取出一个子线程的evLoop，处理这个返回的fd。

处理用于通讯的fd，会创建对应的TcpConnection,
构造函数参数就是fd和对应子线程的evLoop，会初始化
两个Buffer作为读写buff，会new出http请求和回应类
new出一个Channel封装fd，这次有读写和销毁回调函数，
然后加入任务队列，再处理，处理方式已经知道了，
就是调用epoll_Ctl处理










