#ifndef CHANNELMODEL_H
#define CHANNELMODEL_H

#include <QVector>
#include <QString>

/**
 * @brief 通道信息类
 */
class ChannelModel {
public:
    ChannelModel(const QString& name, int cardNum, int index, int totalChannels, int rate);

    int getCardNumber() const;
    QString getChannelName() const;
    int getChannelIndex() const;
    int getSamplingRate() const;
    int getChannels() const;

private:
    QString channelName;    // 通道命名
    int cardNumber;         // 所属卡号
    int channelIndex;       // 通道序号
    int channels;           // 通道总数
    int samplingRate;       // 采样率
};

#endif // CHANNELMODEL_H
