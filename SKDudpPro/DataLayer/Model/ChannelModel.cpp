#include "ChannelModel.h"

ChannelModel::ChannelModel(const QString& name, int cardNum, int index, int totalChannels, int rate)
    : channelName(name),
    cardNumber(cardNum),
    channelIndex(index),
    channels(totalChannels),
    samplingRate(rate) {}

int ChannelModel::getCardNumber() const {
    return cardNumber;
}

QString ChannelModel::getChannelName() const {
    return channelName;
}

int ChannelModel::getChannelIndex() const {
    return channelIndex;
}

int ChannelModel::getSamplingRate() const {
    return samplingRate;
}

int ChannelModel::getChannels() const {
    return channels;
}
