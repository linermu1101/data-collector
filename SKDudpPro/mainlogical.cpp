#include "mainlogical.h"
#include <QDir>
#include <QDebug>
#include <QMessageBox>

MainLogical::MainLogical(QWidget *parent,AcquisitionCard* acquisitionCard)
    : QWidget{parent}
{
    parameterToML(acquisitionCard);
    initPCIENumber();
    addRemoteHost();
    slotSorting();

    cardInformationInit();
    dataInteractionlogicalInit();
    addcoefficient();
}

MainLogical::~MainLogical()
{
    for (QThread* thread : threads) {
            if (thread->isRunning()) {
                thread->quit(); // 或者使用 thread->requestInterruption(); 如果线程中有合适的处理逻辑
                thread->wait(); // 等待线程完全结束
            }
        }
        threads.clear(); // 清空列表
}

QList<CardInformation *> *MainLogical::returnViewCardInformations()
{
    return cardInformations;
}

QMap<int, QString> MainLogical::getPCIESortedMap() const
{
     return PCIESortedMap;
}

QList<DataInteractionLogical *> *MainLogical::returnDataInteractionLogicals()
{
    return dataInteractionLogicals;
}

QStringList MainLogical::getipList() const
{
    return ipList;
}

void MainLogical::initPCIENumber()
{
    // PCIEMap.insert(0, "192.168.0.1");
    // PCIEMap.insert(1, "192.168.0.2");
    // PCIEMap.insert(2, "192.168.0.3");
    // PCIEMap.insert(3, "192.168.0.4");
    // PCIEMap.insert(4, "192.168.0.5");
    // PCIEMap.insert(5, "192.168.0.6");
    // PCIEMap.insert(6, "192.168.0.7");
    // PCIEMap.insert(7, "192.168.0.8");
    // PCIEMap.insert(8, "192.168.0.9");
    // PCIEMap.insert(9, "192.168.0.10");
    // PCIEMap.insert(10, "192.168.0.11");
    // PCIEMap.insert(11, "192.168.0.12");
    // PCIEMap.insert(12, "192.168.0.13");
    // PCIEMap.insert(13, "192.168.0.14");
    // PCIEMap.insert(14, "192.168.0.15");
    // PCIEMap.insert(15, "192.168.0.16");
    // PCIEMap.insert(16, "192.168.0.17");
    // PCIEMap.insert(17, "192.168.0.18");

    for (int i = 0; i < 254; ++i)
    {
        QString ipAddress = QString("192.168.0.%1").arg(i+1);
        PCIEMap.insert(i, ipAddress);
    }
}

void MainLogical::cardInformationInit()
{
    cardInformations = new QList<CardInformation*>;
    QList<int> sortedKeys = PCIESortedMap.keys();

    for(int i = 0; i < Parameter::ACQUISITIONCARDNUMBER; i++) {
        CardInformation* cardInformation = new  CardInformation();
        cardInformations->append(cardInformation);
    }

    for ( int i = 0; i < PCIESortedMap.size(); i++) {
        int tempKey = sortedKeys.at(i);
        cardInformations->at(tempKey)->setInsertionCondition(1);

    }
}

void MainLogical::slotSorting()
{
    for (int i = 0; i < PCIEMap.size(); i++) {
        int PCIEIndex = i;
        QString PCIESerialNumber = PCIEMap.value(i);

        for(int j = 0; j < ipList.size(); j++) {

            if(ipList[j] == PCIESerialNumber) {
                sortIndexList.append(PCIEIndex);
                sortSeriesNumberList.append(PCIESerialNumber);
                PCIESortedMap.insert(PCIEIndex, PCIESerialNumber);
            }
        }
    }

}

QMap<int, QString> MainLogical::getCorrespondingMap() const
{
    return correspondingMap;
}

QMap<QString, double> MainLogical::getchannel_coefficients() const
{
    return channel_coefficients;
}

QMap<QString, double> MainLogical::getchannel_coefficientsb() const
{
    return channel_coefficientsb;
}
void MainLogical::dataInteractionlogicalInit()
{
    int acquisitionCardNumber = PCIESortedMap.size();
    dataInteractionLogicals = new QList<DataInteractionLogical*>;

    for (int i = 0; i < acquisitionCardNumber; i++)
    {
        int tempIndex = sortIndexList[i];
        QString tempSeriesNumber = sortSeriesNumberList[i];
        CardInformation *tempCardInformation = cardInformations->at(tempIndex);

        QThread *tempThread = new QThread();
        threads.append(tempThread);

        DataInteractionLogical *dataInteractionLogical = new DataInteractionLogical(nullptr, tempIndex, tempSeriesNumber, tempCardInformation, acquisitionCard);
        dataInteractionLogicals->append(dataInteractionLogical);
        dataInteractionLogical->moveToThread(tempThread);

        tempThread->start();
    }
}



void MainLogical::parameterToML(AcquisitionCard *acquisitionCard)
{
    this->acquisitionCard = acquisitionCard;
}

void MainLogical::addcoefficient()
{
    // 当前工作目录
    qDebug() << QStringLiteral("addcoefficient 当前工作目录:") << QDir::currentPath();

    QFile file("../channel_coefficient.ini");
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qDebug() << "无法打开文件!";
        QMessageBox::warning(this,"警告","未找到校准配置文件channel_coefficient.ini");
        return;
    }

    QTextStream in(&file);
    QString line;
    while (!in.atEnd()) {
        line = in.readLine();
        //qDebug() << "读取到的行: " << line;

        // 先按空格分割出每个channel:coefficient对
        QStringList pairs = line.split(' ', QString::SkipEmptyParts);
        for (const QString& pair : pairs) {
            // 再按冒号分割channel和coefficient
            QStringList parts = pair.split(':');
            if (parts.size() == 2) {
                QString channel = parts[0].trimmed();
                QString coefficientStr = parts[1].trimmed();
                QStringList coefficientParts = coefficientStr.split(',');
                if (coefficientParts.size() == 2){
                    double coefficienta = coefficientParts[0].trimmed().toDouble();
                    channel_coefficients[channel] = coefficienta;
                    double coefficientb = coefficientParts[1].trimmed().toDouble();
                    channel_coefficientsb[channel] = coefficientb;
                }
            } else {
                qDebug() << "无效的键值对: " << pair;
            }
        }
    }

    //            if (channel_coefficientsb.isEmpty()) {
    //                qDebug() << "channel_coefficients 是空的，未解析到任何数据。";
    //            } else {
    //                for (auto it = channel_coefficientsb.begin(); it != channel_coefficientsb.end(); ++it) {
    //                    qDebug() << it.key() << ": " << it.value();
    //                }
    //            }

    file.close();
}

void MainLogical::addRemoteHost()
{
    QSettings collectSettings("parameterSKDFDA", "value");

    QString daqCaseText = collectSettings.value("daqCaseBoxText","").toString();
    if(daqCaseText == "")
    {
        QMessageBox::warning(nullptr, "警告", "机箱参数加载异常");
    }

    // 当前工作目录
    qDebug() << QStringLiteral("addRemoteHost 当前工作目录:") << QDir::currentPath() << " daqCaseText " << daqCaseText;

    QString paraFile = "../presetIP" + daqCaseText + ".ini";
    QFile file(paraFile); // 修改为正确的配置文件名

    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QMessageBox::warning(nullptr, "警告", "网络配置文件未找到"+ paraFile );
        return;
    }

    QTextStream in(&file);
    while (!in.atEnd()) {
        QString line = in.readLine();
        if (!line.isEmpty()) {
            presetIpsAndPorts.append(line);
        }
    }

    file.close();


    for (const QString &ipAndPort : presetIpsAndPorts) {
        QStringList ipPortList = ipAndPort.split(":");
        if (ipPortList.size() != 2) {
            qDebug() << "Invalid IP:Port format in line:" << ipAndPort;
            continue;
        }
        QString presetIp = ipPortList.at(0);
        //QString presetPort = ipPortList.at(1);

        ipList.append(presetIp);  // 将解析出的IP地址加入到新的列表中
    }
}
