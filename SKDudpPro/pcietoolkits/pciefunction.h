#ifndef PCIEFUNCTION_H
#define PCIEFUNCTION_H

#include <QObject>
#include "windows.h"
#include "setupapi.h"
#include "initguid.h"
#include "planargraph.h"
#include <strsafe.h>
#include "header.h"

#pragma comment(lib, "setupapi.lib")

DEFINE_GUID(GUID_DEVINTERFACE_XDMA,
            0x74c7e4a9, 0x6d5d, 0x4a70, 0xbc, 0x0d, 0x20, 0x69, 0x1d, 0xff, 0x9e, 0x9d);

#define MAX_PATH_LENGEH 260
#define FPGA_DDR_START_ADDR 0x00000000
#define MAX_BYTES_PER_TRANSFER  0x800000

enum DataRegAddress{
    RealTime_Phase_Addr     =   0x0,
    DDR3_Reference_Addr     =   0x99000000,
    DDR3_Measure_Addr       =   0x80000000,
    DDR3_Phase1_Addr        =   0xB2000000,
    DDR3_Phase2_Addr        =   0xCB000000,
    DDR3_Phase3_Addr        =   0xE4000000,
};

class IniFileInf{
public:
    //实时通道
    QStringList realTime_ini_List;
    QStringList realTime_id_List;

    //挑点通道
    QStringList ini_List;
    QList<BOOL> save_List;
    QStringList id_List;
};

class PcieFunction : public QObject
{
    Q_OBJECT
public:
    explicit PcieFunction(int popertyValue,QObject *parent = nullptr);
    explicit PcieFunction(QObject *parent = nullptr);
    ~PcieFunction();
    bool isStop = false;
    FILE *dataHandle;
    FILE *infHandle;
    bool isSave=false;
    bool trigEnd = false;
    double specturm;
    int ddrcnt = 0;
    int ddrcount = 0;

    HANDLE h_user;
    HANDLE h_c2h0;
    HANDLE h_h2c0;

    static int read_device(HANDLE device, unsigned int address, DWORD size, BYTE *buffer);
    static int write_device(HANDLE device, unsigned int address, DWORD size, BYTE *buffer);

    IniFileInf *saveFileInf;

public slots:
    void openPcieDev(QString devPath);
    int  allocMemory();
    void c2h_transfer(PlanarGraph *plot,SettingPara cardPara);
    void h2c_transfer(UINT32 address,UINT32 size,unsigned char *buffer,int index);
    void user_write(UINT32 address,UINT32 size,unsigned char *buffer,int index);
    void user_read(UINT32 address,UINT32 size,unsigned char *buffer,int index);
    void user_read_IRQ(int index,int timeout);
    void closePcieDev();

private:  
    unsigned char *h2c_align_mem_tmp;
    unsigned char *c2h_align_mem_tmp;

    bool openFlag = false;
    bool ddrFlag = false;

    static int scanPcieDevice(GUID guid, QStringList *dev_path, DWORD32 len_devpath);
    static BYTE* allocate_buffer(UINT32 size, UINT32 alignment);


signals:
    void replotChart(int index);
    void startCollection();
    void ddrEnd(int index);
    void write2RFM2g(unsigned char *buffer,unsigned long length);
    void timeoutIRQ();

};

#endif // PCIEFUNCTION_H
