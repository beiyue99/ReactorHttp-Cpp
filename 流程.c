


//Buffer�࣬ά��һ��m_data�ڴ�ָ�룬�и�����
//socketRead(int fd)���ú�����fd�����ݶ���m_data��



//Channel�࣬Χ��һ���ļ�������m_fd����װһЩ�ж���
//����ص��������¼����͡��Լ���������д�¼��ĺ���
//�ֱ������ɾ��д�¼��ͷ����Ƿ���д�¼�



//������Dispacher�࣬����m_name,m_channel,m_evloop;
//����ӣ�ɾ�����޸ĺ͵��Ⱥ�������Ϊ�麯������������д
//Dispatcher(EventLoop* evloop) : m_evLoop(evloop)



//EpollDispacher���࣬��m_epfd,m_events,m_maxNode��
//epollCtl(int op)�������ȴ���ev�ṹ�壬ev.event
//����channel����¼���Ȼ�����epoll_ctl������
//�и����Ⱥ���dispach��ͨ��epoll_wait�ȴ������¼�
//���ж�д�¼���ͨ��eventActive���ö�Ӧ�Ķ�д�ص�����
//�������������EventLoop�ඨ��ĺ���



//EventLoop�࣬����m_dispatcherָ������ʵ����ָ�룻
//�ٶ����˴��ChannelElemElement*���͵��������m_tasKQ��
//�ڶ����˴��fd�Ͷ�ӦChannel*������m_channelMap��
//�۰���m_threadID,m_threadName,���ڱ���ͨ�ŵ�Pairfd[2]
//�ܹ��캯�����߳������������������Ĭ����"MainThread"
//�ݹ��캯����new����m_dispatcherʵ������m_socketPair[1]
//��װΪChannel�������ReadEvent�¼����ص�����ָ��Ϊ
//readMessage����:����ȡm_skPair[1]�����ݾͷ��أ�Ȼ��
//����addTask��Channel��װΪChannelElement��ӵ�
//m_tasKQ,���m_threadID==get_id(),˵�������̣߳�ֱ��
//����processTaskQ����������ӣ�ɾ�����޸ģ�
//m_tasKQ������
//����ӣ���fd��channel����m_channelMap��Ȼ������
//m_dispatcher��m_channelΪchannel,Ȼ������
//fd��ӵ�epoll����
//ɾ��������m_dispatcher��m_channelΪchannel��Ȼ��
//��fd��epoll����ɾ�����ٵ���destroyCallback�ͷ���Դ
//�޸�ͬ��
//����������̣߳�����taskWakeup����sockPair[1]
//���и�run����������dispatch�����¼���Ȼ�����
//processTaskQ����������е�����



//WorkerThread��,����m_thread��m_threadId,m_name,
//���������������Լ�һ��*m_evLoop(��Ӧ��ʵ��ָ��)
// ���evLoop�ᴫ��Tcpconnectionʹ��
//���캯����ʼ���߳�id�����֣�run���������߳�ʵ����
//�̺߳�����running���������̶߳�������,���m_evLooP
//��û�����꣬���̻߳Ῠ��������������
//running����newһ��m_evLoop,Ȼ��������������
//Ȼ�����evLoop��run��������������



//ThreadPoll�࣬�������̵߳ķ�Ӧ��ʵ��ָ��m_mainLoop��
//�߳�������vector�߳�����m_workerThreads��
//run��������ָ���������̷߳�������
//takeWorkerEventLoop�������̵߳ķ�Ӧ��ʵ�������û��
//���̣߳��򷵻�����Ӧ��ʵ��



//TcpServer��,���߳�����������Ӧ��ָ�룬�̳߳�ָ�룬
//������m_lfd���˿�m_port��
//���캯����new��m_mainLoopʵ�������ÿղι��캯��
//new����m_threadPoolʵ����Ȼ�����setListen���
//����m_lfd���󶨣�����
//acceptConnection��������ȡ��һ�����̵߳ķ�Ӧ��ʵ����
//Ȼ���accept���ص�cfd�����̷߳�Ӧ��ʵ��evLoop����
//TcpConnection�Ĺ��캯��������
//run���������̳߳ص�run,��ʼ��һ��channel��װm_lfd��
//�����˶��ص�ΪTcpServer::acceptConnection,��m��Ϊthis��
//Ȼ�����m_mainLoop��addTask��channel��װ����m_tasKQ;
//�����������Ӧ�ѵ�run����



//TcpConnection�࣬�����֣��Լ���m_evLoop��m_channel,
//����Bufferָ�룺��дbuff��Http����ͻ�Ӧ��ָ��
//���캯��TcpConnection(int fd, EventLoop* evloop)
//new����Buffer��new��Http����ͻ�Ӧ��
//��ʼ������Ϊ Conection-fd����װfdΪChannel��m_channel
//��ֵ����Ӷ�д�ݻٻص�ΪprocessRead, processWrite��
//destroy������Ϊ����Ȼ�����addTask�ѷ�װ�õ�
//channel����m_tasKQ
//processRead���������������ݶ���readBuf�������֮��
//��Ҫ�ظ��ͻ��˵����ݴ���writebuf���Ϊ��һ���߳�ִ�У�
//����ִ��processReadʱ��������ִ��processWrite
//���û�ж��������Զ�����,ֻ�е���������ȫ������дBuffer
//�Żᷢ���ݣ����������ʱ���ǲ����е�














��newһ��TcpServer�����캯��new��һ�����̵߳�
m_mainLoop,ÿ��Loop�����ʱ�򶼻�new��һ���Լ�
�ĵ����ߣ���epoll�����ߵĹ��캯����ʼ��epoll����
Ȼ��new���̳߳�ʵ������󴴽��󶨼���m_lfd
�����ŵ���TcpServer��run
1�ڲ��ȵ����̳߳ص�run��
ѭ��threadnum�Σ�ÿ��new��WorkerThread����
�����������Ȼ��������ǵ�run����ʵ����������
�����̣߳��̺߳���Ϊrunning��Ȼ��ᱻ��������
��ס���ȴ�runningΪÿ�����߳�new��evLoop֮��
�ŷ��ء�ÿ�����̵߳���evLoop��run,��ʵ����ʹ
���������õ�������ʼ��⣬���epoll_wait���
�������fd������ݶ�д����eventActive��������
���������Ƕ�Ӧ�Ķ�д�ص�������
Ȼ�����processTaskQ����fd���������ӣ�ɾ����
�޸ģ�ͨ�������ߵ�epollCtl��������
2��new��m_lfd��channel���������߳�������У�Ȼ��
����processTasKQ������ӵ�epoll���ϣ�,Ȼ������
����Ӧ�ѵ�run,���������õ����������fdֻ�ж��¼���
�����û�����ʱ����accept���Ӷ��ص����������̻߳�
ȡ��һ�����̵߳�evLoop������������ص�fd��

��������ͨѶ��fd���ᴴ����Ӧ��TcpConnection,
���캯����������fd�Ͷ�Ӧ���̵߳�evLoop�����ʼ��
����Buffer��Ϊ��дbuff����new��http����ͻ�Ӧ��
new��һ��Channel��װfd������ж�д�����ٻص�������
Ȼ�����������У��ٴ�������ʽ�Ѿ�֪���ˣ�
���ǵ���epoll_Ctl����










