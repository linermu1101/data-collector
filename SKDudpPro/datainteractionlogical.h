#ifndef DATAINTERACTIONLOGICAL_H
#define DATAINTERACTIONLOGICAL_H

#include <QObject>
#include "windows.h"
#include "setupapi.h"
#include "initguid.h"
#include "acquisitioncard.h"
#include "cardinformation.h"
#include "datatransferinterface.h"
#include "parameter.h"
#include <strsafe.h>
#include <QMap>
#include <QDebug>
#include <QThread>
#include <QFile>
#include <QDateTime>
#include <QDir>
#include <QSettings>
#include <QPluginLoader>

#pragma comment(lib,"setupapi.lib")

#define MAX_PATH_LENGTH 260
#define FPGA_DDR_START_ADDR 0X00000000
#define MAX_BYTES_PER_TRANSFER 0X800000



class DataInteractionLogical : public QObject
{
    //不加无法使用信号槽机制
    Q_OBJECT
public:
    explicit DataInteractionLogical(QObject *parent = nullptr);
    DataInteractionLogical(QObject *parent = nullptr, int cardIndex = -1, QString cardSerialNumber = nullptr
                           , CardInformation* cardInformation = nullptr,AcquisitionCard* acquisitionCard = nullptr);

    void configurationFileForParsing(QString path);
    int judgmentSamplingRate();
    int getbytesPerPoint();
    int getTempIndex();
    QString settingsPath;

    ~DataInteractionLogical();

public slots:
    // void startudpCollect();
    // void hardTriggerLogic();
    void revUdpData(char* data, int size, const QString &name);


signals:
    void stopCollect();
    void sendBufferSize(int size);
    void dataReceivedFromIp(const QString &ipAddress); // 通知数据接收
    void hardTriggerStartInformation(int index);

private:
    int cardIndex;
    QString cardSerialNumber;
    CardInformation* cardInformation;
    AcquisitionCard* acquisitionCard;
    DataTransferInterface*driverPlugin;


    void parameterCopy(int cardIndex, QString cardSerialNumber, CardInformation *cardInformation, AcquisitionCard *acquisitonCard);

    QFile *fileMake();

    void delayMsDI(int mes);

    bool loadPlugin();
    bool unloadPlugin();

};

#endif // DATAINTERACTIONLOGICAL_H
