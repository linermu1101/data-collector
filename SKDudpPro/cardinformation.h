#ifndef CARDINFORMATION_H
#define CARDINFORMATION_H
#include "parameter.h"


class CardInformation
{
public:
    CardInformation();

    int getChannelNumber() const;
    int getSamplingRate() const;
    int getInsertionCondition() const;
    int getChannelCondition() const;
    int getChannel0StorageCondition() const;
    int getChannel1StorageCondition() const;
    int getChannel2StorageCondition() const;
    int getChannel3StorageCondition() const;



    void setChannelNumber(int channel);
    void setSamplingRate(int rate);
    void setInsertionCondition(int condition);
    void setChannelCondition(int condition);
    void setChannel0StorageCondition(int condition);
    void setChannel1StorageCondition(int condition);
    void setChannel2StorageCondition(int condition);
    void setChannel3StorageCondition(int condition);



private:

    //通道数量
    int channelNumber;

    //采样率
    int samplingRate;



    //是否上电
    int insertionCondition;

    int channelCondition;

    //通道0是否存储
    int channel0StorageCondition;

    //通道1是否存储
    int channel1StorageCondition;

    //通道2是否存储
    int channel2StorageCondition;

    //通道3是否存储
    int channel3StorageCondition;


};

#endif // CARDINFORMATION_H
