#ifndef DATALOADER_H
#define DATALOADER_H

#include <QString>
#include <QVector>
#include <QMap>

class DataLoader {
public:
    // 构造函数
    DataLoader(bool linearFixmode, const QString &filePath, int cardNumber, int channelNumber, const QMap<QString, double> &a, const QMap<QString, double> &b, long singleChannelDataNumber);

    // 加载数据
    bool loadData();

    // 偏移运算
    void applyOffset();

    // 获取处理后的数据
    const QVector<QVector<double>>& getData() const;

private:
    QString m_filePath;
    bool m_linearFixmode;   //开启配置文件校准模式 false为统一系数
    int m_cardNumber;
    int m_channelNumber;
    double m_scalingFactor;
    long m_singleChannelDataNumber;  // 每个通道的数据点数
    QVector<QVector<double>> m_channelYData;  // 存储每个通道的数据
    QMap<QString, double> m_a;  // 存储系数a
    QMap<QString, double> m_b;     // 存储系数b
};

#endif // DATALOADER_H
