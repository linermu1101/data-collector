#ifndef HEADER_H
#define HEADER_H
#ifdef __cplusplus
#include "stdint.h"
#include "windows.h"
extern "C"
{
#endif
#ifdef __cplusplus
//#include "rfm2g_api.h"
}
#endif
#include <QDebug>


struct SettingPara{
    uint32_t address;
    int cardIndex;
    int mode;
    int saveCnt;
    int coefficient;
    int pickUpMode;
    float c_time;
    int singleLength;//单帧字节数
    int totalLength;//字节总数
    bool startFlag = false;;
};

//注：如果这里不设置2字节对齐， 默认为4字节对齐，则结构体的总字节大小并不是122byte，而是128byte
#pragma pack(push) //  保存原来的字节对齐状态
#pragma pack(2) // 两字节对齐
struct InfPara{
    char filetype[10];
    int16_t channelId;
    char channelName[12];
    long addr;
    float freq;
    long len;
    long post;
    uint16_t maxDat;
    float lowRang;
    float highRang;
    float factor;
    float Offset;
    char unit[8];
    float Dly;
    int16_t attribDt;
    int16_t datWth;
    //这里的备用字段都是用不到的，所以在这里直接赋初值
    int16_t sparI1=0;
    int16_t sparI2=0;
    int16_t sparI3=0;
    float sparF1=0;
    float sparF2=0;
    char sparC1[8]={""};
    char sparC2[16]={""};
    char sparC3[10]={""};
};
#pragma pack(pop) // 恢复原来的字节对齐状态

#endif // HEADER_H
