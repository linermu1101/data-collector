#include "DataProcessing.h"


DataProcessing::DataProcessing(const QMap<QString, double>& a, const QMap<QString, double>& b)
    : m_a(a), m_b(b) {
}


// 处理方法，对每个通道应用校准
void DataProcessing::process(AcquisitionData &data) {
    // 遍历每个通道
    for (size_t channelIndex = 0; channelIndex < data.getNumberOfChannels(); ++channelIndex) {
        QString ch = QString("CH%1-%2").arg(data.getCardNumber()).arg(channelIndex);
        // 获取当前通道的数据
        const QVector<int16_t>* samplesPtr = &data.getChannelData(channelIndex);
        if (!samplesPtr) {
            qDebug() << " DataProcessing::process  获取通道数据失败 ";
        }
        const QVector<int16_t>& samples = *samplesPtr;

        // 提取当前通道的校准系数
        double a = m_a[ch];
        double b = m_b[ch];

        // 创建一个新的 QVector<double> 用于存储校准后的数据
        QVector<double> calibratedSamples(samples.size());

        // 应用校准公式
        for (int i = 0; i < samples.size(); ++i) {
            calibratedSamples[i] = a * static_cast<double>(samples[i]) + b;
        }

        // 设置校准后的数据
        data.setCalibratedChannelData(channelIndex, calibratedSamples);
    }
}
