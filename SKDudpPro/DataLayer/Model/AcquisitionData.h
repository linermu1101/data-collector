#ifndef ACQUISITIONDATA_H
#define ACQUISITIONDATA_H

#include <QVector>

/**
 * @brief 采集数据类
 * 该类用于存储和管理采集卡采集到的数据，包括卡号、采样率、采集时间、拾取标志，
 * 以及各通道的原始数据和校准后的数据
 */
class AcquisitionData {
public:
    /**
     * @brief 构造函数
     * 初始化采集数据对象
     *  
     * @param channelData 各通道的原始数据
     * @param samplingRate 采样率
     * @param acquisitionTime 采集时间
     * @param pickUp 拾取标志
     */
    AcquisitionData(const QVector<QVector<int16_t>> &channelData, const QVector<QVector<double>> &calibratedChannelData);

    AcquisitionData() = default;

    /**
     * @brief 获取卡号
     * 
     * @return int 卡号
     */
    int getCardNumber() const;

    void setCardNumber(int cardNumber);

    /**
     * @brief 获取采样率
     * 
     * @return int 采样率
     */
    int getSamplingRate() const;

    void setSamplingRate(int samplingRate);

    /**
     * @brief 获取采集时间
     * 
     * @return int 采集时间
     */
    int getAcquisitionTime() const;

    void setAcquisitionTime(int acquisitionTime);

    /**
     * @brief 获取拾取标志
     * 
     * @return int 拾取标志
     */
    int getPickUp() const;
    void setPickUp(int pickUp);

    /**
     * @brief 获取通道数量
     * 
     * @return int 通道数量
     */
    int getNumberOfChannels() const;

    /**
     * @brief 获取指定通道的原始数据
     * 
     * @param channelIndex 通道索引
     * @return const QVector<int16_t>& 指定通道的原始数据
     */
    const QVector<int16_t>& getChannelData(int channelIndex) const;
    void setChannelData(int channelIndex, const QVector<int16_t>& data);
    void clearChannelData();

    /**
     * @brief 设置指定通道的校准后数据
     * 
     * @param channelIndex 通道索引
     * @param calibratedData 校准后的数据
     */
    void setCalibratedChannelData(int channelIndex, const QVector<double>& calibratedData);

    /**
     * @brief 获取指定通道的校准后数据
     * 
     * @param channelIndex 通道索引
     * @return const QVector<double>& 指定通道的校准后数据
     */
    const QVector<double>& getCalibratedChannelData(int channelIndex) const;

private:
    int cardNumber;         // 卡号
    int samplingRate;      // 采样率
    int acquisitionTime;    // 采集时间
    int pickUp;            // 拾取标志
    QVector<QVector<int16_t>> channelData; // 各通道数据
    QVector<QVector<double>> calibratedChannelData; // 各通道校准后数据
};

#endif // ACQUISITIONDATA_H
