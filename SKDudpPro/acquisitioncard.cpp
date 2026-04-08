#include "acquisitioncard.h"

AcquisitionCard::AcquisitionCard()
{
    acquisitionTime = 0;
    pickUpIndex = 0;
    samplingRateIndex = 0;
    displayPassageNumerIndex = 0;
    triggerModeIndex = 0;
    rawDataAnalysisIndex = 0;
    selectChannelNumber = 0;
}

int AcquisitionCard::getAcquisitionTime() const {
    return acquisitionTime;
}

void AcquisitionCard::setAcquisitionTime(int time) {
    acquisitionTime = time;
}

QString AcquisitionCard::getFileStorageLocation() const {
    return fileStorageLocation;
}

void AcquisitionCard::setFileStorageLocation(const QString &location) {
    fileStorageLocation = location;
}

QString AcquisitionCard::getAcquisitionFileName() const {
    return acquisitionFileName;
}

void AcquisitionCard::setAcquisitionFileName(const QString &fileName) {
    acquisitionFileName = fileName;
}

int AcquisitionCard::getPickUpIndex() const {
    return pickUpIndex;
}

void AcquisitionCard::setPickUpIndex(int index) {
    pickUpIndex = index;
}

int AcquisitionCard::getSamplingRateIndex() const {
    return samplingRateIndex;
}

void AcquisitionCard::setSamplingRateIndex(int index) {
    samplingRateIndex = index;
}

int AcquisitionCard::getDisplayCardNumber() const {
    return displayCardNumber;
}

void AcquisitionCard::setDisplayCardNumber(int number) {
    displayCardNumber = number;
}

int AcquisitionCard::getDisplayPassageNumberIndex() const {
    return displayPassageNumerIndex;
}

void AcquisitionCard::setDisplayPassageNumberIndex(int index) {
    displayPassageNumerIndex = index;
}

int AcquisitionCard::getTriggerModeIndex() const {
    return triggerModeIndex;
}

void AcquisitionCard::setTriggerModeIndex(int index) {
    triggerModeIndex = index;
}

int AcquisitionCard::getRawDataAnalysisIndex() const{
    return rawDataAnalysisIndex;
}

void AcquisitionCard::setRawDataAnalysisIndex(int index) {
    rawDataAnalysisIndex = index;
}

int AcquisitionCard::getPretriggerTime() const
{
    return PretriggerTime;
}

void AcquisitionCard::setPretriggerTime(int time)
{
    PretriggerTime = time;
}

int AcquisitionCard::getPosttriggerTime() const
{
    return PosttriggerTime;
}

void AcquisitionCard::setPosttriggerTime(int time)
{
    PosttriggerTime = time;
}

int AcquisitionCard::getSelectChannelNumber() const
{
    return selectChannelNumber;
}

void AcquisitionCard::setSelectChannelNumber(int number)
{
    selectChannelNumber = number;
}

void AcquisitionCard::setAcquisitionNumber(int index) {
    acquisitionNumber = index;
}

int AcquisitionCard::getAcquisitionNumber() const {
    return acquisitionNumber;
}

int AcquisitionCard::getAcquisitionNumberAutomationIndex() const {
    return acquisitionNumberAutomationIndex;
}

void AcquisitionCard::setAcquisitionNumberAutomationIndex(int index) {
    acquisitionNumberAutomationIndex = index;
}


int AcquisitionCard::getbaseline1() const
{
    return baseline1;
}

void AcquisitionCard::setbaseline1(int value)
{
     baseline1 = value;
}

int AcquisitionCard::getbaseline2() const
{
    return baseline2;
}

void AcquisitionCard::setbaseline2(int value)
{
     baseline2 = value;
}

int AcquisitionCard::getbaseline3() const
{
    return baseline3;
}

void AcquisitionCard::setbaseline3(int value)
{
     baseline3 = value;
}

int AcquisitionCard::getbaseline4() const
{
    return baseline4;
}

void AcquisitionCard::setbaseline4(int value)
{
    baseline4 = value;
}

int AcquisitionCard::getfirstsampleratecount() const
{
    return countFirstsampRate;
}

void AcquisitionCard::setfirstsampleratecount(int value)
{
    countFirstsampRate = value;
}

int AcquisitionCard::getacquisitionsamplerateIndex() const
{
    return acquisitionsamplerateIndex;
}

void AcquisitionCard::setacquisitionsamplerateIndex(int index)
{
    acquisitionsamplerateIndex=index;
}


/**
 * 根据当前设置的采样率索引获取每个采样点所需的字节数。
 * 
 * @return 返回每个采样点占用的字节数（单位：字节）。
 *         当采样率索引为4时，返回4；其他情况下，默认返回8。
 */
int AcquisitionCard::getbytesPerPoint() const
{
    // 根据采样率索引的值确定每个采样点的字节数
    int bytesPerPoint = 8; // 默认情况下，每个采样点占用8个字节
    if (acquisitionsamplerateIndex == 4) // 当采样率索引为4时
    {
        bytesPerPoint = 4; // 每个采样点占用4个字节
    }

    return bytesPerPoint; // 返回每个采样点的字节数
}


