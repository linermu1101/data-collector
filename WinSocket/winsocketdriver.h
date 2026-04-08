#ifndef WINSOCKETDRIVER_H
#define WINSOCKETDRIVER_H

#include "datatransferinterface.h"
#include <iostream>
#include <algorithm>
#include <unordered_map>
#include <mutex>
#include <pthread.h>
#include <QObject>
#include <QThread>

//Windows系统
#ifdef Q_OS_WIN
#include <winsock2.h>
#endif

//Linux系统
#ifdef Q_OS_LINUX
#include <sys/socket.h>     //提供socket函数及数据结构
#include <sys/types.h>      //数据类型定义
#include <sys/ioctl.h>      //提供对I/O控制的函数
#include <arpa/inet.h>      //提供IP地址转换函数
#include <netinet/in.h>     //定义数据结构sockaddr_in
#include <netdb.h>          //提供设置及获取域名的函数
#include <unistd.h>         //提供通用的文件、目录、程序及进程操作的函数
#include <error.h>          //提供错误号error的定义，用于错误处理
#include <fcntl.h>          //提供对文件控制的函数
#include <time.h>           //提供有关时间的函数
#include <signal.h>         //提供对信号操作的函数
#define INVALID_SOCKET -1
using BOOL = bool;
using PVOID = void*;
using DWORD = unsigned long;
#endif

#include <QVariant>
#include <QDebug>
#include <QTimer>
#include <QElapsedTimer>

typedef struct WinSocket_Info
{
#ifdef Q_OS_WIN
  SOCKET sock;
#else
  int sock;
#endif
  BOOL isFirst;
  BOOL isVaild;
  std::string ip;
  uint16_t port;
  std::vector<std::string> ipList;
  PVOID data;
}SocketInfo;


class WinSocketDriver : public DataTransferInterface
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID DataTransferPluginIID)//导出plugin
    Q_INTERFACES(DataTransferInterface)
public:
    explicit WinSocketDriver(QObject *parent = nullptr);
    ~WinSocketDriver();
    void enableDriver(const QString hostName = "", unsigned short port = 8080) override;
    void disableDriver() override;
    void writeData(const QString &name,int port,QByteArray &val) override;

    QElapsedTimer elapsedTimer;  // 添加计时器成员变量
    bool timerStarted = false;   // 标志是否已经开始计时
private:
    void releaseAll();
    SocketInfo sock;
    pthread_t revThreadId;
    QThread *dataThread;


};

#endif // WINSOCKETDRIVER_H
