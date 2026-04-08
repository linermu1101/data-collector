#include "pciefunction.h"
#include <QDebug>
#include <QThread>

static int verbose_msg(const char* const fmt, ...) {
    int ret = 0;
    va_list args;
    if (1) {
        va_start(args, fmt);
        ret = vprintf(fmt, args);
        va_end(args);
    }
    return ret;
}

PcieFunction::PcieFunction(QObject *parent) : QObject(parent)
{
}

PcieFunction::PcieFunction(int popertyValue,QObject *parent) : QObject(parent)
{
    this->setProperty("设备",popertyValue);
    saveFileInf = new IniFileInf;
}

int PcieFunction::allocMemory()
{

    h2c_align_mem_tmp = allocate_buffer(0x800000,4096*2);
    c2h_align_mem_tmp = allocate_buffer(0x800000,4096*2);

    if(NULL == h2c_align_mem_tmp || NULL == c2h_align_mem_tmp) return -1;

    return 0;
}
void PcieFunction::openPcieDev(QString devPath)
{
    wchar_t  user_path_w[MAX_PATH+1];
    wchar_t  c2h0_path_w[MAX_PATH+1];
    wchar_t  h2c0_path_w[MAX_PATH+1];
    QString user_path = devPath+"\\user";
    QString c2h0_path = devPath+"\\c2h_0";
    QString h2c0_path = devPath+"\\h2c_0";

    mbstowcs(user_path_w, user_path.toUtf8().data(),  user_path.size() + 1);
    mbstowcs(h2c0_path_w, h2c0_path.toUtf8().data(),  h2c0_path.size() + 1);
    mbstowcs(c2h0_path_w, c2h0_path.toUtf8().data(),  c2h0_path.size() + 1);

    h_user = CreateFile(user_path_w,GENERIC_READ|GENERIC_WRITE,0,NULL,OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,NULL);
    if(h_user == INVALID_HANDLE_VALUE) return;
    h_c2h0 = CreateFile(c2h0_path_w,GENERIC_READ ,0,NULL,OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,NULL);
    if(h_c2h0 == INVALID_HANDLE_VALUE) return;
    h_h2c0 = CreateFile(h2c0_path_w,GENERIC_WRITE,0,NULL,OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,NULL);
    if(h_h2c0 == INVALID_HANDLE_VALUE) return;
    openFlag = true;
}

void PcieFunction::closePcieDev()
{
    if(!openFlag) return;
    CloseHandle(h_user);
    CloseHandle(h_c2h0);
    CloseHandle(h_h2c0);
    openFlag = false;
}

//数据传输函数
void PcieFunction::c2h_transfer(PlanarGraph *plot,SettingPara cardPara)
{
    int val = this->property("设备").toInt();
    if(val != cardPara.cardIndex)  return;
    int cnt=0;
    int total_cnt = 0;
    int RT_Length;
    double timeLine = 0;
    if(cardPara.address == RealTime_Phase_Addr){
        switch(cardPara.pickUpMode)
        {
        case 0:
            RT_Length = 100000 * 2;
            break;
        case 1:
            RT_Length = 10000 * 2;
            break;
        case 2:
            RT_Length = 5000 * 2;
            break;
        case 3:
            RT_Length = 2000 * 2;
            break;
        case 4:
            RT_Length = 1000 * 2;
            break;
        }
        ddrFlag = false;
    }else{
        RT_Length = cardPara.singleLength/100;
        ddrFlag = true;
    }
    if(!ddrFlag && (cardPara.mode == 1))
    {
        cardPara.totalLength *= 5;
        RT_Length *= 5;
        specturm = cardPara.c_time/(cardPara.totalLength/(5*2));
    }else{
        specturm = cardPara.c_time / (cardPara.totalLength/2);
    }
    total_cnt = cardPara.totalLength/RT_Length;

    QVector<QCPGraphData> vecdata1,vecdata2,vecdata3,vecdata4,vecdata5,pNullVec;
    QCPGraphData gp_data1,gp_data2,gp_data3,gp_data4,gp_data5;
    QSharedPointer<QCPGraphDataContainer> ch1_graph,ch2_graph,ch3_graph,ch4_graph,ch5_graph;
    ch1_graph = plot->plot->graph(0)->data();
    ch2_graph = plot->plot->graph(1)->data();
    ch3_graph = plot->plot->graph(2)->data();
    ch4_graph = plot->plot->graph(3)->data();
    ch5_graph = plot->plot->graph(4)->data();
    bool first = true;
    bool ddrSave = false;
    if(cardPara.saveCnt)
    {
        ddrSave = saveFileInf->save_List[cardPara.saveCnt-1];
    }
    unsigned char *t_buf;
    if(cardPara.mode)
        t_buf=(unsigned char*)malloc(cardPara.totalLength);
    else
        t_buf =(unsigned char*)malloc(1);
    int addr1,addr2,addr3,addr4,addr5;
    addr1 = 0;
    addr2 = cardPara.totalLength/5;
    addr3 = cardPara.totalLength*2/5;
    addr4 = cardPara.totalLength*3/5;
    addr5 = cardPara.totalLength*4/5;

    for(;;)
    {
        if((cnt == total_cnt)||isStop){
            fclose(dataHandle);
            if(!ddrFlag)
            {
                unsigned char val;
                val = 1;
                write_device(h_user,0x04,4,&val);
                emit replotChart(cardPara.cardIndex);
            }
            if(!isStop)
                emit ddrEnd(cardPara.cardIndex);
            return;
        }

        int16_t temp1,temp2,temp3,temp4,temp5;
        unsigned char *buffer;
        buffer = (unsigned char*)malloc(RT_Length);
        if(ddrFlag){
           int size = read_device(h_c2h0,cardPara.address + cnt * RT_Length,RT_Length,buffer);
           if(size<RT_Length) continue;
           if(ddrSave)
           {
                fwrite(buffer,1,RT_Length,dataHandle);
           }
       }
       else
       {
           int size = read_device(h_c2h0,cardPara.address,RT_Length,buffer);
           if(size<RT_Length) continue;
           int16_t  *dBuffer = (int16_t*)buffer;
            if(cardPara.mode == 0)
            {
                fwrite(buffer,1,RT_Length,dataHandle);
                for(int m=0;m<RT_Length/2;m++)
                {
                      temp1 = *(dBuffer+m);
                      gp_data1.key      = timeLine;
                      gp_data1.value    = (double)temp1 / 8 * cardPara.coefficient;
                      timeLine += specturm;
                      vecdata1.append(gp_data1);
                }

                if(first)
                {
                    ch1_graph->set(vecdata1,true);
                    first = false;
                }
                else
                    ch1_graph->add(vecdata1,true);
                vecdata1.clear();
                vecdata1.swap(pNullVec);
            }
            else
            {
                int k=0;
                for(int m=0;m<RT_Length/10;m++)
                {
                      temp1 = *(dBuffer+k);
                      temp2 = *(dBuffer+k+1);
                      temp3 = *(dBuffer+k+2);
                      temp4 = *(dBuffer+k+3);
                      temp5 = *(dBuffer+k+4);
                      *(t_buf+addr1+k*2) = temp1;
                      *(t_buf+addr2+k*2) = temp2;
                      *(t_buf+addr3+k*2) = temp3;
                      *(t_buf+addr4+k*2) = temp4;
                      *(t_buf+addr5+k*2) = temp5;

                      gp_data1.key      = timeLine;
                      gp_data1.value    = temp1/8*cardPara.coefficient;
                      gp_data2.key      = timeLine;
                      gp_data2.value    = temp2/8*cardPara.coefficient;
                      gp_data3.key      = timeLine;
                      gp_data3.value    = temp3/8*cardPara.coefficient;
                      gp_data4.key      = timeLine;
                      gp_data4.value    = temp4/8*cardPara.coefficient;
                      gp_data5.key      = timeLine;
                      gp_data5.value    = temp5/8*cardPara.coefficient;
                      timeLine += specturm;
                      vecdata1.append(gp_data1);
                      vecdata2.append(gp_data2);
                      vecdata3.append(gp_data3);
                      vecdata4.append(gp_data4);
                      vecdata5.append(gp_data5);
                      k+=5;
                }

                ch1_graph->add(vecdata1,true);
                ch2_graph->add(vecdata2,true);
                ch3_graph->add(vecdata3,true);
                ch4_graph->add(vecdata4,true);
                ch5_graph->add(vecdata5,true);

                vecdata1.clear();
                vecdata2.clear();
                vecdata3.clear();
                vecdata4.clear();
                vecdata5.clear();
                vecdata1.swap(pNullVec);
                vecdata2.swap(pNullVec);
                vecdata3.swap(pNullVec);
                vecdata4.swap(pNullVec);
                vecdata5.swap(pNullVec);
            }
        }
        cnt++;
        free(buffer);
        if(cnt == total_cnt){
            if(cardPara.mode && !ddrFlag){
                fwrite(t_buf,1,cardPara.totalLength,dataHandle);
            }
        }
    }
    if(cardPara.mode && !ddrFlag)
    {

        fwrite(t_buf,1,cardPara.totalLength,dataHandle);
    }
    free(t_buf);
}

void PcieFunction::h2c_transfer(unsigned int address,unsigned int size,unsigned char *buffer,int index)
{
   int val = this->property("设备").toInt();

   if(val != index)  return;

    memcpy(h2c_align_mem_tmp,buffer,size);
    write_device(h_h2c0,address,size,buffer);
}

void PcieFunction::user_write(unsigned int address,unsigned int size,unsigned char *buffer,int index)
{
  int val = this->property("设备").toInt();
  if(val != index)  return;
  write_device(h_user,address,size,buffer);
}

void PcieFunction::user_read(unsigned int address,unsigned int size,unsigned char *buffer,int index)
{
  int val = this->property("设备").toInt();
  if(val != index)  return;
   read_device(h_user,address,size,buffer);
}

void PcieFunction::user_read_IRQ(int index,int timeout)
{

    int val = this->property("设备").toInt();
    if(val != index)  return;
    unsigned char buf[4];
    UINT32 data;
    int elapsed;
    QElapsedTimer elapsedTimer;
    //定义个计时器 用于检测超时
    elapsedTimer.start();
    for(;;)
    {

        if(trigEnd)
        {
            trigEnd = false;
            return;
        }
        read_device(h_user,0x0C,4,buf);
        data = buf[3]<<24|buf[2]<<16|buf[1]<<8|buf[0];
        if(data == 1)
        {
            unsigned char val;
            val = 0;
            emit startCollection();
            user_write(12,4,&val,index);
            return;
        }

        //超时
        elapsed = elapsedTimer.elapsed()/1000;
        if(elapsed >timeout)
        {
            emit timeoutIRQ();
            trigEnd = true;
        }
    }
}

BYTE* PcieFunction::allocate_buffer(UINT32 size, UINT32 alignment)
{
    if (size == 0) {
        size = 4;
    }

    if (alignment == 0) {
        SYSTEM_INFO sys_info;
        GetSystemInfo(&sys_info);
        alignment = sys_info.dwPageSize;
    }
    return (BYTE*)_aligned_malloc(size, alignment);
}

int PcieFunction::read_device(HANDLE device, unsigned int address, DWORD size, BYTE *buffer)
{
    DWORD rd_size = 0;
    unsigned int transfers;
    unsigned int i=0;

    LARGE_INTEGER addr;
    memset(&addr,0,sizeof(addr));
    addr.QuadPart = address;
    if ((int)INVALID_SET_FILE_POINTER == SetFilePointerEx(device, addr, NULL, FILE_BEGIN)) {
        return -3;
    }
    transfers = (unsigned int)(size / MAX_BYTES_PER_TRANSFER);
    for (i = 0; i<transfers; i++)
    {
        if (!ReadFile(device, (void *)(buffer + i*MAX_BYTES_PER_TRANSFER), (DWORD)MAX_BYTES_PER_TRANSFER, &rd_size, NULL))
        {
            return -1;
        }
        if (rd_size != MAX_BYTES_PER_TRANSFER)
        {
            return -2;
        }
    }
    if (!ReadFile(device, (void *)(buffer + i*MAX_BYTES_PER_TRANSFER), (DWORD)(size - i*MAX_BYTES_PER_TRANSFER), &rd_size,NULL))
    {
        return -1;
    }
    if (rd_size != (size - i*MAX_BYTES_PER_TRANSFER))
    {
        return -2;
    }
    return size;
}

int PcieFunction::write_device(HANDLE device, unsigned int address, DWORD size, BYTE *buffer)
{
    DWORD wr_size = 0;
    unsigned int transfers;
    unsigned int i;
    transfers = (unsigned int)(size / MAX_BYTES_PER_TRANSFER);

    if (INVALID_SET_FILE_POINTER == SetFilePointer(device, address, NULL, FILE_BEGIN)) {
      fprintf(stderr, "Error setting file pointer, win32 error code: %ld\n", GetLastError());
        return -3;
    }


    for (i = 0; i<transfers; i++)
    {
        if (!WriteFile(device, (void *)(buffer + i*MAX_BYTES_PER_TRANSFER), MAX_BYTES_PER_TRANSFER, &wr_size, NULL))
        {
            return -1;
        }
        if (wr_size != MAX_BYTES_PER_TRANSFER)
        {
            return -2;
        }
    }
    if (!WriteFile(device, (void *)(buffer + i*MAX_BYTES_PER_TRANSFER), (DWORD)(size - i*MAX_BYTES_PER_TRANSFER), &wr_size, NULL))
    {
        return -1;
    }
    if (wr_size != (size - i*MAX_BYTES_PER_TRANSFER))
    {
        return -2;
    }
    return size;
}

PcieFunction::~PcieFunction()
{
    delete  saveFileInf;
}
