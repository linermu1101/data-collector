#ifndef PARAMETER_H
#define PARAMETER_H

#include <stdint.h>

namespace Ui {
class Parameter;
}

class Parameter {
public:
    //x起始
    static inline double plot2DXStart = 0;

    //x最终
    static inline double plot2DXFinal = 0;

    //y起始
    static inline double plot2DYStart = 0;

    //y最终
    static inline double plot2DYFinal = 0;

    //通道0DDR起始采集地址_只读
    static inline uint32_t CHANNEL0_DDRADDRESS = 0x20000000;

    //通道1DDR起始采集地址_只读
    static inline uint32_t CHANNEL1_DDRADDRESS = 0x0000000;

    //通道2DDR起始采集地址_只读
    static inline uint32_t CHANNEL2_DDRADDRESS = 0x60000000;

    //通道3DDR起始采集地址_只读
    static inline uint32_t CHANNEL3_DDRADDRESS = 0x40000000;

    //通道3DDR结束采集地址_只读
    static inline uint32_t CHANNEL2_DDRADDRESSEND = 0x80000000;

    //采样率判断寄存器_只读_1表500M_2表示1G_3表示1.25G
    static inline uint32_t SAMPLEJUDGMENT_REGISTER = 0x34;

    //FPGA版本号判断寄存器_只读
    static inline uint32_t FPGAVERSION_REGISTER = 0x30;

    //通道1剩余采样点数判断寄存器_只读
    static inline uint32_t CHANNEL1_REGISTERLASTVALUE = 0x38;

    //通道2剩余采样点数判断寄存器_只读
    static inline uint32_t CHANNEL2_REGISTERLASTVALUE = 0x3C;

    //启动采集寄存器地址_只写(写入采样时间)
    static inline uint32_t START_REGISTER = 0x00;

    //复位寄存器地址_只写
    static inline uint32_t RESET_REGISTER = 0x04;

    //设置通道基线寄存器地址_只写
    static inline uint32_t SETBASELINE_REGISTER = 0x14;

    //设置通道基线寄存器地址_只写
    static inline uint32_t TRIGGERMODE_REGISTER = 0x18;

    //采集卡槽数量
    static inline int ACQUISITIONCARDNUMBER = 254; //18;

    //采集循环次数
    static inline int LOOPNUMBER = 1;

    //500M采样率通道数
    static inline int MAXCHANNELNUMBER = 4;

    //触发下位机启动硬件触发寄存器_只写
    static inline int HARDTRIGGER_REGISTER = 0x08;

    //轮询下位机是否采集完毕寄存器_只读(暂定)
    static inline int COLLECTED_REGISTER = 0x70;

    //触发_只读(暂定)
    static inline int trigger_REGISTER = 0x74;

    //触发_只读(暂定)
    static inline int DDR_REGISTER = 0x78;

    //通道1和2基线_只读(暂定)
    static inline int BASELINE1AND2_REGISTER = 0x7C;

    //通道3和4基线_只读(暂定)
    static inline int BASELINE3AND4_REGISTER = 0x80;

    static inline int REGISTER_1 = 0x40;

    static inline int REGISTER_2 = 0x44;

    static inline int REGISTER_3 = 0x48;

    static inline int REGISTER_4 = 0x4C;

    static inline int REGISTER_5 = 0x50;

    static inline int REGISTER_6 = 0x54;

    static inline int REGISTER_7 = 0x58;

    static inline int REGISTER_8 = 0x5C;

    static inline int REGISTER_9 = 0x60;

    //采样率20M
    static inline int SAMPLINGRATE_20M = 20000000;

    //采样率125M
    static inline int SAMPLINGRATE_125M = 125000000;

    //采样率200M
    static inline int SAMPLINGRATE_200M = 200000000;

    //采样率250M
    static inline int SAMPLINGRATE_250M = 250000000;

    //采样率500M
    static inline int SAMPLINGRATE_500M = 500000000;

    //采样率1G
    static inline int SAMPLINGRATE_1G = 1000000000;
};

#endif // PARAMETER_H
