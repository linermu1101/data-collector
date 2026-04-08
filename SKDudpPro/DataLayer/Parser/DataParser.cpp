#include "DataParser.h"
#include <QtEndian>
#include <QDebug>
#include <QFileInfo>
#include <QDir>
#include <QSettings>

DataParser::DataParser(QObject *parent) : QObject(parent)
{
    QVector<QVector<int16_t>> allChannelData(4);
    QVector<QVector<double>> allcalibratedChannelData(4);
    m_acquisitionData = new AcquisitionData(allChannelData, allcalibratedChannelData);
}

DataParser::~DataParser() {
    delete m_acquisitionData;
    m_acquisitionData = nullptr;
}


bool DataParser::parseMutiData(const QMap<int, QString> &dataFilePaths) {
    for (auto it = dataFilePaths.constBegin(); it != dataFilePaths.constEnd(); ++it) {
        int cardNumber = it.key();
        if (!parseDatWholeFile(cardNumber, it.value())) {
            emit parsingInfo(QString("解析失败: CH%1 路径%2 ").arg(cardNumber).arg(it.value()));
            return false;
        }
    }
    return true;
}


bool DataParser::parseDatWholeFile(int cardNumber, const QString &dataFilePath) {
    QFile file(dataFilePath);
    if (!file.open(QIODevice::ReadOnly)) return false;

    QByteArray fileData = file.readAll();
    file.close();

    QString iniFilePath = dataFilePath;
    iniFilePath.replace(".DAT", ".INI");

    QSettings iniFile(iniFilePath, QSettings::IniFormat);
    // 从配置文件中读取采样率、采集时间和拾取标志
    int samplingRate = iniFile.value("samplingRate").toInt();
    int acquisitionTime = iniFile.value("acquisitionTime").toInt();
    int pickUp = iniFile.value("pickUp").toInt();

    // 计算采样点数
    int samplePoints = ( samplingRate / 1000000)* acquisitionTime;

    // 每个通道的数据字节数
    int channelSize = samplePoints * sizeof(int16_t);

    QVector<QVector<int16_t>> allChannelData(4);
    QVector<QVector<double>> allcalibratedChannelData(4);
    AcquisitionData acquisitionData(allChannelData, allcalibratedChannelData);

    // 解析每个通道的数据
    for (int channelIndex = 0; channelIndex < 4; ++channelIndex) {
        QVector<int16_t> channelData;

        int offset = channelIndex * channelSize;
        // 检查通道序号的有效性
        if (offset + channelSize > fileData.size()) {
            qDebug() << "解析通道跳过: " << channelIndex;
            //emit parsingInfo(QString("解析通道跳过: CH%1-%2 采样率%3M 采样时间%4us").arg(cardNumber).arg(channelIndex).arg(samplingRate/ 1000000).arg(acquisitionTime));
            continue;
        }

        for (int i = 0; i < samplePoints; ++i) {
            int16_t dataPoint = *reinterpret_cast<int16_t*>(fileData.data() + offset + i * sizeof(int16_t));
            dataPoint = qFromBigEndian(dataPoint);
            channelData.append(dataPoint);
        }

        acquisitionData.setChannelData(channelIndex, channelData);
    }

    // 创建 AcquisitionData 实例 添加到卡Map
    acquisitionData.setCardNumber(cardNumber);
    acquisitionData.setSamplingRate(samplingRate);
    acquisitionData.setAcquisitionTime(acquisitionTime);
    acquisitionData.setPickUp(pickUp);

    cardData.insert(cardNumber, acquisitionData);

    return true;
}

QMap<int, AcquisitionData> DataParser::getMutiAcquisitionData(IDataProcessor* processor) const {
    QMap<int, AcquisitionData> processedData;

    for (auto it = cardData.constBegin(); it != cardData.constEnd(); ++it) {
        int cardNumber = it.key();
        AcquisitionData data = it.value();

        // 如果提供了处理器，则进行处理
        if (processor) {
            processor->process(data); // 处理数据（校准等）
        }

        processedData.insert(cardNumber, data); // 添加到结果Map
    }

    return processedData; // 返回处理后的数据Map
}

bool DataParser::parseChannelDatFile(int cardNumber, int channelNumber, const QString &dataFilePath) {
    QFile file(dataFilePath);
    if (!file.open(QIODevice::ReadOnly)) {
        emit parsingInfo(QString("解析无法打开文件: CH%1-%2 %3").arg(cardNumber).arg(channelNumber).arg(dataFilePath));
        return false;
    }

    QByteArray fileData = file.readAll();
    file.close();

    QString iniFilePath = QDir::cleanPath(dataFilePath);
    iniFilePath.replace(".DAT", ".INI");

    QSettings iniFile(iniFilePath, QSettings::IniFormat);
    // 从配置文件中读取采样率、采集时间和拾取标志
    int samplingRate = iniFile.value("samplingRate").toInt();
    qDebug() << "读取的采样率: " << samplingRate;
    int acquisitionTime = iniFile.value("acquisitionTime").toInt();
    int pickUp = iniFile.value("pickUp").toInt();

    // 计算采样点数
    int samplePoints = ( samplingRate / 1000000)* acquisitionTime;

    QVector<int16_t> channelData; // 存储通道数据


    // 每个通道的数据字节数
    int channelSize = samplePoints * sizeof(int16_t);

    // 计算通道在文件中的偏移量
    int offset = channelNumber * channelSize;
   
   // 检查通道序号的有效性
    if (offset + channelSize > fileData.size()) {
        qDebug() << "解析通道序号无效: " << channelNumber;
        emit parsingInfo(QString("解析通道序号无效: CH%1-%2 采样率%3M 采样时间%4us").arg(cardNumber).arg(channelNumber).arg(samplingRate/ 1000000).arg(acquisitionTime));
        return false;
    }

    // 解析通道的数据
    for (int i = 0; i < samplePoints; ++i) {
        // 读取两个字节并进行d端反转
        int16_t dataPoint = *reinterpret_cast<int16_t*>(fileData.data() + offset + i * sizeof(int16_t));
        dataPoint = qFromBigEndian(dataPoint);
        channelData.append(dataPoint);
    }

    // 获取对应卡的数据
    m_acquisitionData->setCardNumber(cardNumber);
    m_acquisitionData->setSamplingRate(samplingRate);
    m_acquisitionData->setAcquisitionTime(acquisitionTime);
    m_acquisitionData->setPickUp(pickUp);
    // 添加通道数据到对应卡的数据中
    m_acquisitionData->setChannelData(channelNumber, channelData);

    emit parsingInfo(QString("成功解析通道 CH%1-%2  采样率%3M 采样时间%4us").arg(cardNumber).arg(channelNumber).arg(samplingRate/ 1000000).arg(acquisitionTime));
    return true;
}

AcquisitionData* DataParser::getAcquisitionChannelDataPtr(IDataProcessor* processor) const {
    if (!m_acquisitionData) {
        return nullptr; // 或者抛出异常，取决于你的需求
    }

    // 如果提供了处理器，则进行处理
    if (processor) {
        processor->process(*m_acquisitionData); // 处理数据（校准等）
    }

    return m_acquisitionData; // 返回处理后的数据指针
}




