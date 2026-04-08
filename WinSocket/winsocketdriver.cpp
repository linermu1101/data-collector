#include "winsocketdriver.h"

BOOL isSerach = false;
BOOL isStart  = false;
BOOL isHardStart = false;

void *revMsg(void* lpParameter)
{
    constexpr int MAX_BUFSIZE = 1500;
    SocketInfo *info = (SocketInfo*)lpParameter;
    WinSocketDriver *plug = static_cast<WinSocketDriver*>(info->data);
    sockaddr_in clientAddr;
#ifdef Q_OS_WIN
    int len = sizeof(clientAddr);
#else
    socklen_t len = sizeof(clientAddr);
#endif
    char* tempBuf;
    tempBuf = (char*)malloc(MAX_BUFSIZE);
    while (1) {
        int ret = recvfrom(info->sock, tempBuf, MAX_BUFSIZE, 0, (sockaddr*)&clientAddr, &len);        
        if (ret < 0) {
#ifdef Q_OS_WIN
            if (WSAGetLastError() != WSAEWOULDBLOCK) {
                // 处理其他错误
                std::cerr << "recvfrom error: " << WSAGetLastError() << std::endl;
            }
#else
            if (errno != EAGAIN && errno != EWOULDBLOCK) {
                // 处理其他错误
                std::cerr << "recvfrom error: " << errno << std::endl;
            }
#endif
            // 非阻塞模式下没有数据可用时，短暂休眠减少CPU占用
#ifdef Q_OS_WIN
            _sleep(1); // Windows平台使用_sleep，参数单位是毫秒
#else
            usleep(10); // 其他平台使用usleep，参数单位是微秒
#endif
            continue;
        }
        
        if (ret == 0) {
            // 连接关闭
            continue;
        }
        
        std::string desIp(inet_ntoa(clientAddr.sin_addr));
        
        plug->cnt++;    //包数
        
        // 快速将数据放入队列，避免在接收线程中处理耗时操作
        auto it = plug->threadMap.find(desIp.c_str());
        if (it == plug->threadMap.end()) {
            // 动态创建DataProcessor实例
            QString ipAddress = QString::fromStdString(desIp);
            DataProcessor *dataprocessor = new DataProcessor(desIp);
            plug->threadMap.insert(ipAddress, dataprocessor);
            it = plug->threadMap.find(ipAddress);
        }
        if (it != plug->threadMap.end()) {
            it.value()->pushData(tempBuf, ret);
        }
    }

    free(tempBuf);
    return nullptr;
}

WinSocketDriver::WinSocketDriver(QObject *parent)
    :DataTransferInterface(parent)
{   
#ifdef Q_OS_WIN
    WORD wSockVersion = MAKEWORD(2, 2);
    WSADATA wsadata;
    int ret = WSAStartup(wSockVersion,&wsadata);
    if (ret != 0) {
        return;
    }

    if (wsadata.wVersion != wSockVersion) {
        std::cout << "version error!" << std::endl;
        WSACleanup();
        return;
    }
#endif
    sock.sock = INVALID_SOCKET;
    sock.isVaild = true;
    sock.isFirst = true;

    //测试
//    DataProcessor* dataprocessor0 = new DataProcessor("192.168.0.1");
//    threadMap.insert("192.168.0.1",dataprocessor0);

    // DataProcessor* dataprocessor1 = new DataProcessor("192.168.0.2");
    // threadMap.insert("192.168.0.2",dataprocessor1);

    // DataProcessor* dataprocessor2 = new DataProcessor("192.168.0.3");
    // threadMap.insert("192.168.0.3",dataprocessor2);

    // DataProcessor* dataprocessor3 = new DataProcessor("192.168.0.4");
    // threadMap.insert("192.168.0.4",dataprocessor3);

    // DataProcessor* dataprocessor4 = new DataProcessor("192.168.0.5");
    // threadMap.insert("192.168.0.5",dataprocessor4);

    // DataProcessor* dataprocessor5 = new DataProcessor("192.168.0.6");
    // threadMap.insert("192.168.0.6",dataprocessor5);

    // DataProcessor* dataprocessor6 = new DataProcessor("192.168.0.7");
    // threadMap.insert("192.168.0.7",dataprocessor6);

    // DataProcessor* dataprocessor7 = new DataProcessor("192.168.0.8");
    // threadMap.insert("192.168.0.8",dataprocessor7);

    // DataProcessor* dataprocessor8 = new DataProcessor("192.168.0.9");
    // threadMap.insert("192.168.0.9",dataprocessor8);

    // DataProcessor* dataprocessor9 = new DataProcessor("192.168.0.10");
    // threadMap.insert("192.168.0.10",dataprocessor9);

    // DataProcessor* dataprocessor10 = new DataProcessor("192.168.0.11");
    // threadMap.insert("192.168.0.11",dataprocessor10);

    // 初始不创建DataProcessor实例，改为动态创建
    // 当接收到数据时，如果对应的IP地址没有DataProcessor，再创建
}

WinSocketDriver::~WinSocketDriver()
{
    sock.isVaild = false;
#ifdef Q_OS_WIN
    WSACleanup();
#endif
    disableDriver();
}

void WinSocketDriver::enableDriver(const QString hostName, unsigned short port)
{
    int ret;
    //创建UDP套接字
    sock.sock = socket(AF_INET,SOCK_DGRAM,IPPROTO_UDP);
    if (sock.sock == INVALID_SOCKET) {
        std::cout << "create sock error!" << std::endl;
        return;
    }

    //设置缓存区大小及非阻塞状态
    int bufferSize = 16*1024*1024; // 增加到16MB
    setsockopt(sock.sock,SOL_SOCKET,SO_RCVBUF,(const char*)&bufferSize,sizeof(bufferSize));
    
    // 验证缓冲区大小是否设置成功
    int actualBufferSize;
    int size = sizeof(actualBufferSize);
    getsockopt(sock.sock, SOL_SOCKET, SO_RCVBUF, (char*)&actualBufferSize, &size);
    std::cout << "UDP接收缓冲区大小: " << actualBufferSize << " bytes" << std::endl;
#ifdef Q_OS_WIN
    unsigned long nonBlocking = 1;
    ioctlsocket(sock.sock,FIONBIO, &nonBlocking);
#else
    fcntl(sock.sock,F_SETFL,O_NONBLOCK);
#endif

    //设置连接的地址信息
    sockaddr_in addr;
    addr.sin_family = AF_INET;
#ifdef Q_OS_WIN
    addr.sin_addr.S_un.S_addr = inet_addr(hostName.toLocal8Bit().data());
#else
    addr.sin_addr.s_addr = inet_addr(hostName.toLocal8Bit().data());
#endif

    addr.sin_port = htons(port);
    //将套接字与地址信息连接
    ret = bind(sock.sock, (struct sockaddr*)&addr, sizeof(addr));
    if (ret == -1) {
        std::cout << "bind sock error!" << std::endl;
#ifdef Q_OS_WIN
        closesocket(sock.sock);
#else
        close(sock.sock);
#endif
        return;
    }
    sock.ip = hostName.toLocal8Bit().data();
    sock.port = port;
    sock.data = this;
    if(sock.isFirst){
        pthread_create(&revThreadId,NULL,revMsg,&sock);
        pthread_detach(revThreadId);
        sock.isFirst = false;
    }
}

void WinSocketDriver::disableDriver()
{
    if (sock.sock == INVALID_SOCKET) {
        return;
    }
#ifdef Q_OS_WIN
      closesocket(sock.sock);
#else
      close(sock.sock);
#endif
    releaseAll();
}

void WinSocketDriver::writeData(const QString &name,int port,QByteArray &val)
{
    if(sock.sock == INVALID_SOCKET) return;

    int tolen = 666;
    struct sockaddr_in desAddr;
    desAddr.sin_family = AF_INET;
    desAddr.sin_port = htons(sock.port);

    desAddr.sin_addr.S_un.S_addr = inet_addr(name.toLocal8Bit().data());
    sendto(sock.sock,val,val.size(),0,(struct sockaddr*)&desAddr,tolen);

}

void WinSocketDriver::releaseAll()
{
    QHash<QString,DataProcessor*> nullThreadMap;
    threadMap.swap(nullThreadMap);
    sock.ipList.clear();
}

