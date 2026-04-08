#ifndef DATATRANSFERINTERFACE_H
#define DATATRANSFERINTERFACE_H

#include <QObject>
#include <QHash>
#include <QMutex>
#include <QMutexLocker>
#include <QDebug>
#include <QDataStream>

#include <QThread>
#include "QsLog.h"

const QString kAllDevice = "ALL";

struct SourceBuffer
{
    char *buffer;
    int bufferLength;
    int capacity;
};

#define CalculateSize(val) (1005*val)

class DataProcessor : public QObject
{
    Q_OBJECT
public:
    explicit DataProcessor(const std::string ip,QObject *parent = nullptr):QObject(parent)
      ,name(ip.data())
      ,currentBuffer(nullptr)
      ,bufferSize(0)
    {
        buffer_1.capacity = 0;
        buffer_2.capacity = 0;
        // 默认分配较大的缓冲区，后续可以根据需要调整
        allocSize(name, CalculateSize(10000)); // 增加到约10MB
    }

    ~DataProcessor(){
        freeBuffer();
    }



    void append(char* data,int size){
        memcpy(currentBuffer->buffer+currentBuffer->bufferLength,data,size);
        currentBuffer->bufferLength += size;
    }

    void reset(){
        buffer_1.bufferLength = 0;
        buffer_2.bufferLength = 0;
        currentBuffer = &buffer_1;
        index = false;
    }

    void freeBuffer(){
        cus_free(&buffer_1);
        cus_free(&buffer_2);
    }

    void toggle(){
        index = !index;
        if(index){
            buffer_2.bufferLength = 0;
            currentBuffer = &buffer_2;
        }else{
            buffer_1.bufferLength = 0;
            currentBuffer = &buffer_1;
        }
    }

    int cus_size(){
        return currentBuffer->bufferLength;
    }

public slots:
    void allocSize(const QString &ipAddress, long long ssize){
        if (this->name == ipAddress) {
            QLOG_INFO() << " name "<< this->name << " ipAddress "<< ipAddress  <<"bufferSize:"<<ssize;
            QLOG_INFO()<<" allocSize.. ";
            bufferSize = ssize;
            cus_alloc(&buffer_1,bufferSize);
            cus_alloc(&buffer_2,bufferSize);
            currentBuffer = &buffer_1;
            index = false;

            QLOG_INFO() << "当前缓冲区容量：" << currentBuffer->capacity
                        << " 已用长度：" << currentBuffer->bufferLength;
        }
    }

    void pushData(char *data,int size){
        // 检查当前缓冲区是否足够容纳新数据
        if (currentBuffer->bufferLength + size > currentBuffer->capacity) {
            // 如果当前缓冲区不足，先切换到另一个缓冲区
            toggle();
        }
        
        append(data, size);
        QLOG_INFO() << this->name << " pushData:" << size << " bufferSize:" << bufferSize << "cus_size:" << cus_size();
        
        // 当缓冲区达到一定大小或时间间隔时，触发数据处理信号
        // 这样可以平衡延迟和吞吐量
        if (cus_size() >= bufferSize / 2) { // 当缓冲区达到一半容量时就处理
            QLOG_INFO() << "emit dataTransferSignal...";
            emit dataTransferSignal(currentBuffer->buffer, currentBuffer->bufferLength, name);
            toggle();
        }
    }

signals:
    void dataTransferSignal(char* data,int size,const QString &name);

private:
    void cus_alloc(SourceBuffer *buf,int size){
        if(buf->capacity){
            QLOG_INFO()<<" realloc.. ";
            buf->buffer = (char *)realloc(buf->buffer ,size+1);
        }else{
            QLOG_INFO()<<" malloc.. ";
            buf->buffer = (char *)malloc(size+1);
        }
        buf->buffer[size] = '\0';
        buf->bufferLength = 0;
        buf->capacity = size;
    }

    void cus_free(SourceBuffer *buf){
        if(buf->capacity != 0){
            QLOG_INFO()<<" cus_free.. ";
            free(buf->buffer);
            buf->bufferLength = 0;
            buf->capacity = 0;
        }
    }

    QString name;
    SourceBuffer buffer_1;
    SourceBuffer buffer_2;
    SourceBuffer* currentBuffer;
    int bufferSize;
    bool index;
};

class DataTransferInterface : public QObject
{
    Q_OBJECT
public:
    enum DriverTypeEnum{
        kNone,
        kXDMA,
        kNetWork
    };

    enum CMD{
        kDeviceInf          =   0xA0,
        kThreshold          =   0xA1,
        kDecay              =   0xA3,
        kSpectrumTime       =   0xA4,
        kStart              =   0xA5,
        kStop               =   0xA6,
        kAddress            =   0xA7,
        kMode               =   0xA8,
        kSubtractBaseline   =   0xA9,
        kHardTrigger        =   0xAA,
        kBaselineChannel    =   0xAC,
    };

    DataTransferInterface(QObject *parent = nullptr):QObject(parent){};
    virtual ~DataTransferInterface() {} //虚析构函数，c++多态
    virtual void enableDriver(const QString name="", unsigned short port=-1) = 0;
    virtual void writeData(const QString &name,int port,QByteArray &val) = 0;
    QHash<QString,DataProcessor*> threadMap;
    int cnt = 0;
public slots:
    virtual void disableDriver() = 0;
};
#define DataTransferPluginIID   "dataTransfer"
Q_DECLARE_INTERFACE(DataTransferInterface,DataTransferPluginIID)
#endif // DATATRANSFERINTERFACE_H
