#include "DataLoader.h"
#include <QFile>
#include <QDebug>

// 构造函数
DataLoader::DataLoader(bool linearFixmode, const QString &filePath, int cardNumber, int channelNumber, const QMap<QString, double> &a, const QMap<QString, double> &b, long singleChannelDataNumber)
    :m_linearFixmode(linearFixmode), m_filePath(filePath),m_cardNumber(cardNumber), m_channelNumber(channelNumber), m_scalingFactor(0.0002892135),
    m_singleChannelDataNumber(singleChannelDataNumber), m_a(a), m_b(b)
{
    m_channelYData.clear();
    for ( int i = 0; i < channelNumber; i++) {
        QVector<double>  tempChannelYData ;
        m_channelYData.append(tempChannelYData);
    }
}

// 加载数据
bool DataLoader::loadData() {
    QFile tempFile(m_filePath);
    if (!tempFile.open(QIODevice::ReadOnly)) {
        qDebug() << "Failed to open file:" << m_filePath;
        return false;
    }

    char tempBuffer[2];
    for (int i = 0; i < m_channelNumber; ++i) {
        QString ch = QString("CH%1-%2").arg(m_cardNumber).arg(i);
        QVector<double> &channelData = m_channelYData[i];  // 初始化每个通道的数据向量
        for (long j = 0; j < m_singleChannelDataNumber; ++j) {
            if (tempFile.read(tempBuffer, sizeof(tempBuffer)) != sizeof(tempBuffer)) {
                qDebug() << "Failed to read data from file.";
                return false;
            }
            qint16 val = (qint16)(((unsigned char)tempBuffer[0] << 8) | (unsigned char)tempBuffer[1]);
            double result = val;
            if(m_linearFixmode)
            {
                result = val* m_a[ch];
            }
            else{
                result = val* m_scalingFactor;
            }

            channelData.append(result);
        }
    }
    tempFile.close();

    // 线性变换
    if(m_linearFixmode)
    {
        applyOffset();
    }
    return true;
}

// 偏移运算
void DataLoader::applyOffset() {
    // 检查a和b的大小是否匹配
    if (m_a.size() != m_b.size()) {
        qWarning("Vectors a and b must have the same size.");
        return;
    }

    for (int i = 0; i < m_channelYData.size(); ++i) {
        auto &data = m_channelYData[i];

        // // 初始化最大值和最小值
        // double maxVal = 0;
        // double minVal = 0;

        // for (double &value : data) {
        //     if (value > maxVal) {
        //         maxVal = value;
        //     }
        //     if (value < minVal) {
        //         minVal = value;
        //     }
        // }
        // double avg = (minVal + maxVal) / 2;

        QString ch = QString("CH%1-%2").arg(m_cardNumber).arg(i);
        for (double &value2 : data) {
            //value2 = value2 + m_b.at(i)*avg;
            value2 = value2 + m_b[ch];
        }
    }
}

// 获取处理后的数据
const QVector<QVector<double>>& DataLoader::getData() const {
    return m_channelYData;
}
