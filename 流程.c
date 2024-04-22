


//Buffer类，维护一个m_data内存指针，有个函数
//socketRead(int fd)，该函数把fd的数据读到m_data里



//Channel类，围绕一个文件描述符m_fd，封装一些列东西
//比如回调函数，事件类型。以及两个关于写事件的函数
//分别是添加删除写事件和返回是否检测写事件



//调度者Dispacher类，包含m_name,m_channel,m_evloop;
//有添加，删除，修改和调度函数，均为虚函数，被子类重写
//Dispatcher(EventLoop* evloop) : m_evLoop(evloop)



//EpollDispacher子类，有m_epfd,m_events,m_maxNode；
//epollCtl(int op)函数，先创建ev结构体，ev.event
//或上channel里的事件，然后调用epoll_ctl操作。
//有个调度函数dispach，通过epoll_wait等待返回事件
//若有读写事件，通过eventActive调用对应的读写回调函数
//处理，这个函数是EventLoop类定义的函数



//EventLoop类，包含m_dispatcher指向子类实例的指针；
//①定义了存放ChannelElemElement*类型的任务队列m_tasKQ；
//②定义了存放fd和对应Channel*的容器m_channelMap；
//③包含m_threadID,m_threadName,用于本地通信的Pairfd[2]
//④构造函数以线程命名，如果不传参数默认是"MainThread"
//⑤构造函数内new出了m_dispatcher实例，把m_socketPair[1]
//封装为Channel，添加了ReadEvent事件，回调函数指定为
//readMessage函数:仅读取m_skPair[1]的数据就返回，然后
//调用addTask把Channel封装为ChannelElement添加到
//m_tasKQ,如果m_threadID==get_id(),说明是子线程，直接
//调用processTaskQ函数处理（添加，删除，修改）
//m_tasKQ的任务。
//⑥添加：把fd和channel加入m_channelMap，然后设置
//m_dispatcher的m_channel为channel,然后把这个
//fd添加到epoll树上
//删除：设置m_dispatcher的m_channel为channel，然后
//把fd从epoll树上删除，再调用destroyCallback释放资源
//修改同理
//⑦如果是主线程，调用taskWakeup唤醒sockPair[1]
//⑧有个run函数，调用dispatch处理事件，然后调用
//processTaskQ处理任务队列的任务



//WorkerThread类,包含m_thread，m_threadId,m_name,
//互斥锁条件变量以及一个*m_evLoop(反应堆实例指针)
// 这个evLoop会传给Tcpconnection使用
//构造函数初始化线程id和名字，run函数创建线程实例，
//线程函数是running，参数是线程对象自身,如果m_evLooP
//还没创建完，主线程会卡在条件变量这里
//running函数new一个m_evLoop,然后唤醒条件变量，
//然后调用evLoop的run函数启动调度器



//ThreadPoll类，包含主线程的反应堆实例指针m_mainLoop，
//线程数量，vector线程容器m_workerThreads，
//run函数创建指定数量的线程放入容器
//takeWorkerEventLoop返回子线程的反应堆实例，如果没有
//子线程，则返回主反应堆实例



//TcpServer类,有线程数量，主反应堆指针，线程池指针，
//监听的m_lfd，端口m_port。
//构造函数里new出m_mainLoop实例，调用空参构造函数
//new出了m_threadPool实例，然后调用setListen完成
//创建m_lfd，绑定，监听
//acceptConnection函数，先取出一个子线程的反应堆实例，
//然后把accept返回的cfd和子线程反应堆实例evLoop传入
//TcpConnection的构造函数参数里
//run函数调用线程池的run,初始化一个channel封装m_lfd，
//设置了读回调为TcpServer::acceptConnection,参m数为this，
//然后调用m_mainLoop的addTask把channel封装放入m_tasKQ;
//最后启动主反应堆的run函数



//TcpConnection类，有名字，自己的m_evLoop，m_channel,
//两个Buffer指针：读写buff，Http请求和回应类指针
//构造函数TcpConnection(int fd, EventLoop* evloop)
//new两个Buffer，new出Http请求和回应类
//初始化名字为 Conection-fd，封装fd为Channel给m_channel
//赋值，添加读写摧毁回调为processRead, processWrite，
//destroy，参数为自身，然后调用addTask把封装好的
//channel放入m_tasKQ
//processRead函数，把请求数据读到readBuf里，解析完之后，
//把要回给客户端的数据存在writebuf里。因为在一个线程执行，
//所以执行processRead时不会立即执行processWrite
//如果没有定义数据自动发送,只有等所有数据全部存入写Buffer
//才会发数据，数据量大的时候是不可行的














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










