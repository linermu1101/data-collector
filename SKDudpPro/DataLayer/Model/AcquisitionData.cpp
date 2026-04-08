#include "AcquisitionData.h"

/**
 * @brief 采集卡数据模型
 */
AcquisitionData::AcquisitionData(const QVector<QVector<int16_t>> &channelData, const QVector<QVector<double>> &calibratedChannelData)
    : channelData(channelData),calibratedChannelData(calibratedChannelData) {}
int AcquisitionData::getCardNumber() const {
    return cardNumber;
}

void AcquisitionData::setCardNumber(int cardNumber) {
    this->cardNumber = cardNumber;
}

int AcquisitionData::getSamplingRate() const {
    return samplingRate;
}

void AcquisitionData::setSamplingRate(int samplingRate) {
    this->samplingRate = samplingRate;
}


int AcquisitionData::getAcquisitionTime() const {
    return acquisitionTime;
}

void AcquisitionData::setAcquisitionTime(int acquisitionTime) {
    this->acquisitionTime = acquisitionTime;
}

int AcquisitionData::getPickUp() const {
    return pickUp;
}

void AcquisitionData::setPickUp(int pickUp) {
    this->pickUp = pickUp;
}


int AcquisitionData::getNumberOfChannels() const {
    return channelData.size();
}

void AcquisitionData::clearChannelData() {
    channelData.clear();
    calibratedChannelData.clear();
}

const QVector<int16_t>& AcquisitionData::getChannelData(int channelIndex) const {
    return channelData[channelIndex];
}
void AcquisitionData::setChannelData(int channelIndex, const QVector<int16_t>& data) {
    // 确保通道索引有效
    if (channelIndex >= 0 && channelIndex < getNumberOfChannels()) {
        // 设置通道数据
        channelData[channelIndex] = data;
    }
}


// 设置校准后的通道数据
void AcquisitionData::setCalibratedChannelData(int channelIndex, const QVector<double>& calibratedData) {
    // 确保通道索引有效
    if (channelIndex >= 0 && channelIndex < getNumberOfChannels()) {
        // 初始化校准后的数据存储空间
        if (calibratedChannelData.size() <= channelIndex) {
            calibratedChannelData.resize(channelIndex + 1);
        }
        calibratedChannelData[channelIndex] = calibratedData; // 保存校准后的数据
    }
}


// 获取校准后的通道数据
const QVector<double>& AcquisitionData::getCalibratedChannelData(int channelIndex) const {
    // 确保通道索引有效
    if (channelIndex >= 0 && channelIndex < calibratedChannelData.size()) {
        return calibratedChannelData[channelIndex]; // 返回校准后的数据
    }
    // 如果通道索引无效，返回一个空的 QVector<double>
    static QVector<double> emptyData;
    return emptyData;
}
