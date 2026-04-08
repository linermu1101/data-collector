#ifndef ACQUISITIONCARD_H
#define ACQUISITIONCARD_H

#include <QString>
class AcquisitionCard
{
public:
    AcquisitionCard();

    int getAcquisitionTime() const;
    void setAcquisitionTime(int time);

    int getPickUpIndex() const;
    void setPickUpIndex(int index);

    int getSamplingRateIndex() const;
    void setSamplingRateIndex(int index);

    QString getFileStorageLocation() const;
    void setFileStorageLocation(const QString &location);

    QString getAcquisitionFileName() const;
    void setAcquisitionFileName(const QString &fileName);

    int getDisplayCardNumber() const;
    void setDisplayCardNumber(int number);


    int getDisplayPassageNumberIndex() const;
    void setDisplayPassageNumberIndex(int index);

    int getTriggerModeIndex() const;
    void setTriggerModeIndex(int index);

    int getRawDataAnalysisIndex() const;
    void setRawDataAnalysisIndex(int index);

    int getSelectChannelNumber() const;
    void setSelectChannelNumber(int number);

    int getPretriggerTime() const;
    void setPretriggerTime(int time);

    int getPosttriggerTime() const;
    void setPosttriggerTime(int time);

    int getAcquisitionNumber() const;
    void setAcquisitionNumber(int index);

    int getAcquisitionNumberAutomationIndex() const;
    void setAcquisitionNumberAutomationIndex(int index);

    int getbaseline1() const;
    void setbaseline1(int value);

    int getbaseline2() const;
    void setbaseline2(int value);

    int getbaseline3() const;
    void setbaseline3(int value);

    int getbaseline4() const;
    void setbaseline4(int value);

    int getfirstsampleratecount() const;
    void setfirstsampleratecount(int value);

    int getacquisitionsamplerateIndex() const;
    void setacquisitionsamplerateIndex(int index);

    int getbytesPerPoint() const;


private:

    //采样时间
    int acquisitionTime;

    //挑点选项索引
    int pickUpIndex;

    //采样率索引
    int samplingRateIndex;

    //存储文件路径
    QString fileStorageLocation;

    //存储文件名
    QString acquisitionFileName;

   int displayCardNumber;

    int displayPassageNumerIndex;

    //触发形式
    int triggerModeIndex;

    //原始数据分段解析
    int rawDataAnalysisIndex;

    //选择通道数
    int selectChannelNumber;

    //前触发时间
    int PretriggerTime;

    //后触发时间
    int PosttriggerTime;

    //采样号
    int acquisitionNumber;

    //采样号是否自动增长
    int acquisitionNumberAutomationIndex;

    //基线1
    int baseline1;

    //基线2
    int baseline2;

    //基线3
    int baseline3;

    //基线4
    int baseline4;

    //
    int countFirstsampRate;

    //采样率索引
    int acquisitionsamplerateIndex;
};

#endif // ACQUISITIONCARD_H
