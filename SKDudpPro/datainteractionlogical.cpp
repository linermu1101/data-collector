#include "datainteractionlogical.h"

#include <QCoreApplication>
#include<QElapsedTimer>
#include "QsLog.h"
#include <windows.h>
#include <io.h>

DataInteractionLogical::DataInteractionLogical(QObject *parent, int cardIndex, QString cardSerialNumber, CardInformation* cardInformation,AcquisitionCard* acquisitonCard) : QObject(parent)

{
    parameterCopy(cardIndex, cardSerialNumber, cardInformation, acquisitonCard);
    //sampingRateRemove();
    if(loadPlugin()) {
        //qDebug()<<"true1";
    }

}

DataInteractionLogical::~DataInteractionLogical()
{
    unloadPlugin();
}




//参数拷贝
void DataInteractionLogical::parameterCopy(int cardIndex, QString cardSerialNumber, CardInformation *cardInformation, AcquisitionCard *acquisitonCard)
{
    this->cardIndex = cardIndex;
    this->cardSerialNumber = cardSerialNumber;
    this->cardInformation = cardInformation;
    this->acquisitionCard = acquisitonCard;
}

//开始采集
// void DataInteractionLogical::startudpCollect()
// {
//     int acquisitionTimeValue = acquisitionCard->getAcquisitionTime();
//     int samplingRate = judgmentSamplingRate()/1000/1000;// 计算采样率
//     int bagnumber = acquisitionTimeValue * samplingRate*8/1000;// 计算数据包大小
//     emit sendBufferSize(CalculateSize(bagnumber));// 发送数据包大小
//     QString acquisitionTime = QString("%1").arg(acquisitionTimeValue * 4, 8, 16, QChar('0')).toUpper();
//     QString softtrigger = "03" + acquisitionTime;// 触发信号
//     QByteArray soft = QByteArray::fromHex(softtrigger.toUtf8());// 转换为字节数组
//     QString remoteIP = QString("192.168.0.%1").arg(cardIndex+1);// 目标设备IP
//     quint16 targetPort = 8080;// 端口号

//     driverPlugin->writeData(remoteIP, targetPort, soft);// 发送数据到目标设备
//     qDebug()<<"软件触发指令"<<softtrigger;
//        qDebug()<<"远端IP:"<<remoteIP;
// }

//硬件触发逻辑
// void DataInteractionLogical::hardTriggerLogic()
// {
//     int acquisitionTimeValue = acquisitionCard->getAcquisitionTime();
//     QString acquisitionTime = QString("%1").arg(acquisitionTimeValue * 4, 8, 16, QChar('0')).toUpper();
//     QString hardtrigger = "04" + acquisitionTime;
//     QByteArray hard = QByteArray::fromHex(hardtrigger.toUtf8());
//     QString remoteIP = QString("192.168.0.%1").arg(cardIndex+1);
//     quint16 targetPort = 8080;
//     driverPlugin->writeData(remoteIP, targetPort, hard);
//     qDebug()<<"硬件触发指令"<<hardtrigger;
// }

void DataInteractionLogical::revUdpData(char *data, int size, const QString &name)
{
    QLOG_INFO() << "Received data from:" << name;
    QLOG_INFO() << "size:" << size;
    //QLOG_INFO() << data;


    int eachbagsize = 1005;
    int totalDataSize = 0;

    // 计算总共有多少个 1005 字节的数据块
    int batchSize = size / eachbagsize;

    // 计算新 buffer 的大小
    long long bytesLength = batchSize * 1000; // 每个有效数据块包含 1000 字节数据

    // 为 buffer 分配内存
    unsigned char *buffer = (unsigned char*)malloc(bytesLength);


    // 遍历每1005个数据块
    for (int batch = 0; batch < batchSize; batch++) {
        int startIndex = batch * eachbagsize;

        // 检查是否有足够的数据来处理当前块
        if (startIndex + eachbagsize <= size) {
            // 从 data[5] 到 data[1004] 复制数据（1000 字节）
            if (startIndex + 1004 < size) {
                memcpy(buffer + totalDataSize, data + startIndex + 5, 1000);
                totalDataSize += 1000;
            }
        }
    }
    
    int bytesPerPoint = getbytesPerPoint();//acquisitionCard->getbytesPerPoint(); // 每个数据点的字节数  8|4
    int channels = bytesPerPoint / 2; // 通道数   4|2

    size_t dataPointsPerChannel = bytesLength / bytesPerPoint; // 每个数据点 8 字节（4 通道，每个通道 2 字节）
        unsigned char *channelBuffers[channels];
        for (int i = 0; i < channels; ++i) {
            channelBuffers[i] = (unsigned char*)malloc(bytesLength / channels);
            memset(channelBuffers[i], 0, bytesLength/channels);
        }


        // 将数据分配到通道缓冲区
        for (size_t i = 0; i < dataPointsPerChannel; ++i) {
            for (int channel = 0; channel < channels; ++channel) {
                size_t srcOffset = (i * channels + channel) * 2; // 计算源数据的偏移量
                size_t dstOffset = i * 2; // 计算目标数据的偏移量

                // 复制 2 字节的数据到相应的通道缓冲区
                memcpy(&channelBuffers[channel][dstOffset], &buffer[srcOffset], 2);
            }
        }

    QFile* tempFile = fileMake();
    if (tempFile->open(QIODevice::WriteOnly)) {
        size_t channelDataSize = bytesLength / channels; // 每个通道的数据大小

        // 提高文件写入性能的优化：
        // 1. 批量写入所有通道数据，减少系统调用次数
        // 2. 减少频繁的flush操作，只在必要时执行
        // 3. 移除不必要的强制落盘操作，这会严重影响性能
        for (int channel = 0; channel < channels; ++channel) {
            tempFile->write(reinterpret_cast<const char *>(channelBuffers[channel]), channelDataSize);
        }
        
        // 只执行一次flush操作，将所有数据写入磁盘缓存
        tempFile->flush();
        
        qint64 fileSize = tempFile->size();
        QLOG_INFO() << "File size:" << fileSize << "bytes" << name;
    }
    tempFile->close();
    
    // 注意：这里移除了强制落盘操作(FlushFileBuffers)，这会显著提高性能
    // 但可能会在系统崩溃时丢失最后写入的数据
    // 如果数据完整性要求极高，可以保留，但会影响性能
    

    free(buffer);
    for (int i = 0; i < channels; ++i) {
        free(channelBuffers[i]);
    }
    // 发送数据接收通知
    emit dataReceivedFromIp(name);
    
    emit stopCollect();
    QLOG_INFO() << "stopCollect.." << name;
}




//获取采样率
// int DataInteractionLogical::judgmentSamplingRate()
// {
//     int tempSamplingRate = -1;

//     switch (acquisitionCard->getacquisitionsamplerateIndex()) {
//         case(0):
//             tempSamplingRate = Parameter::SAMPLINGRATE_125M;
//             break;
//         case(1):
//             tempSamplingRate = Parameter::SAMPLINGRATE_200M;
//             break;
//         case(2):
//             tempSamplingRate = Parameter::SAMPLINGRATE_250M;
//             break;
//         case(3):
//             tempSamplingRate = Parameter::SAMPLINGRATE_500M;
//             break;
//        case(4):
//            tempSamplingRate = Parameter::SAMPLINGRATE_1G;
//            break;
//     }

//     return tempSamplingRate;
// }


int DataInteractionLogical::judgmentSamplingRate()
{
    int tempSamplingRate = -1; // 默认值，表示未找到匹配的采样率

    if (cardIndex < 59) {
        tempSamplingRate = Parameter::SAMPLINGRATE_500M;
    }
    else if (cardIndex >= 59 && cardIndex < 119) {
        tempSamplingRate = Parameter::SAMPLINGRATE_200M;
        //tempSamplingRate = Parameter::SAMPLINGRATE_250M;  // 此处板卡根据实际情况统一起来 200M 或者 250M
    }
    else if (cardIndex >= 119 && cardIndex < 179) {
        //tempSamplingRate = Parameter::SAMPLINGRATE_125M;
        tempSamplingRate = Parameter::SAMPLINGRATE_200M; // 12.11临时用200M卡替换125
    }
    else if (cardIndex >= 179 && cardIndex < 239) {
        tempSamplingRate = Parameter::SAMPLINGRATE_1G;
    }

    return tempSamplingRate;
}


int DataInteractionLogical::getbytesPerPoint()
{
    int bytesPerPoint = 8;
    int tempSamplingRate = judgmentSamplingRate();

    if(tempSamplingRate == Parameter::SAMPLINGRATE_1G)
    {
        bytesPerPoint = 4;
    }

    return bytesPerPoint;

}

int DataInteractionLogical::getTempIndex()
{
    return cardIndex;
}


//创建文件
QFile *DataInteractionLogical::fileMake()
{
    QDate currentDate = QDate::currentDate();
    int year = currentDate.year();
    int month = currentDate.month();
    int day = currentDate.day();

    QString basePath = acquisitionCard->getFileStorageLocation();
    QString yearPath = basePath + "/" + QString::number(year);
    QString monthdayPath = yearPath + "/" + QString::number(month).rightJustified(2, '0') + QString::number(day).rightJustified(2, '0');

    QDir yearDir;
    QDir monthdayDir;
    //QDir dayDir;

    if (!yearDir.exists(yearPath)) {
        yearDir.mkpath(yearPath);
    }

    if (!monthdayDir.exists(monthdayPath)) {
        monthdayDir.mkpath(monthdayPath);
    }

    int acquisitionNumber = acquisitionCard->getAcquisitionNumber();

    QLOG_INFO() << " fileMake. acquisitionNumber" << acquisitionNumber;

    int tempCardNumber = cardIndex;
    QString tempFileName = "Data";
    QString acquisitionNumberPath = monthdayPath + "/" + QString::number(acquisitionNumber);

    if (!QDir().exists(acquisitionNumberPath)) {
        QDir().mkpath(acquisitionNumberPath);
    }
    QString tempPath = acquisitionNumberPath + "/" + QString::number(tempCardNumber) + "_"
            + tempFileName;

    QString datFilePath = tempPath +".DAT";

    configurationFileForParsing(tempPath);

    QFile* fileTempFile = new QFile(datFilePath);

    return fileTempFile;

}

//文件配置
void DataInteractionLogical::configurationFileForParsing(QString path)
{
    settingsPath = path+".INI";
    QSettings settings(settingsPath,QSettings::IniFormat);
    settings.setValue("samplingRate",judgmentSamplingRate());
    settings.setValue("cardNumber",acquisitionCard->getDisplayCardNumber());
    settings.setValue("pickUp",acquisitionCard->getPickUpIndex());
    settings.setValue("acquisitionTime",acquisitionCard->getAcquisitionTime());
    settings.setValue("channel0Store",cardInformation->getChannel0StorageCondition());
    settings.setValue("channel1Store",cardInformation->getChannel1StorageCondition());
    settings.setValue("channel2Store",cardInformation->getChannel2StorageCondition());
    settings.setValue("channel3Store",cardInformation->getChannel3StorageCondition());
}

//延迟函数
void DataInteractionLogical::delayMsDI(int mes)
{
    QTime dieTime = QTime::currentTime().addMSecs(mes);
    while(dieTime>QTime::currentTime()){
        QCoreApplication::processEvents(QEventLoop::AllEvents,100);
    }
}

bool DataInteractionLogical::loadPlugin()
{
    QDir pluginsDir(qApp->applicationDirPath());

#ifdef Q_OS_WIN
    pluginsDir.cd("plugin");
#else
    pluginsDir.cd("release");
    pluginsDir.cd("plugin");
#endif
    foreach (QString fileName, pluginsDir.entryList(QDir::Files)){
        QPluginLoader pluginLoader(pluginsDir.absoluteFilePath(fileName));
        QObject *plugin = pluginLoader.instance();
        if (plugin) {
            driverPlugin = qobject_cast<DataTransferInterface *>(plugin);
                return true;
            }
        }
    }


bool DataInteractionLogical::unloadPlugin()
{
    QDir pluginsDir(qApp->applicationDirPath());
    pluginsDir.cdUp();
    pluginsDir.cd("plugin");
    foreach (QString fileName,pluginsDir.entryList(QDir::Files)){
        QPluginLoader PluginLoader(pluginsDir.absoluteFilePath(fileName));
        PluginLoader.unload();
    }
    driverPlugin->deleteLater();
    return true;
}
