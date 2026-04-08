#include "cardinformation.h"

CardInformation::CardInformation()
{
    channelNumber = 0;
    samplingRate = 0;
    insertionCondition = 0;
    channelCondition = 0;
    channel0StorageCondition = 1;
    channel1StorageCondition = 1;
    channel2StorageCondition = 1;
    channel3StorageCondition = 1;
}

int CardInformation::getChannelNumber() const
{
    return channelNumber;
}

int CardInformation::getSamplingRate() const
{
    return samplingRate;
}

int CardInformation::getInsertionCondition() const
{
    return insertionCondition;
}

int CardInformation::getChannelCondition() const
{
    return channelCondition;
}

int CardInformation::getChannel0StorageCondition() const
{
    return channel0StorageCondition;
}

int CardInformation::getChannel1StorageCondition() const
{
    return channel1StorageCondition;
}

int CardInformation::getChannel2StorageCondition() const
{
    return channel2StorageCondition;
}

int CardInformation::getChannel3StorageCondition() const
{
    return channel3StorageCondition;
}

void CardInformation::setChannelNumber(int channel)
{
    channelNumber = channel;
}

void CardInformation::setSamplingRate(int rate)
{
    samplingRate = rate;
}

void CardInformation::setInsertionCondition(int condition)
{
    insertionCondition = condition;
}

void CardInformation::setChannelCondition(int condition)
{
    channelCondition = condition;
}

void CardInformation::setChannel0StorageCondition(int condition)
{
    channel0StorageCondition = condition;
}

void CardInformation::setChannel1StorageCondition(int condition)
{
    channel1StorageCondition = condition;
}

void CardInformation::setChannel2StorageCondition(int condition)
{
    channel2StorageCondition = condition;
}

void CardInformation::setChannel3StorageCondition(int condition)
{
    channel3StorageCondition = condition;
}
