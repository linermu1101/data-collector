#ifndef DATAPROCESSING_H
#define DATAPROCESSING_H

#include "IDataProcessor.h"
#include "AcquisitionData.h"
#include <QVector>
#include <QDebug>
#include <QMessageBox>

class DataProcessing : public IDataProcessor {
public:
    // 构造函数接收各通道的校准参数
    DataProcessing(const QMap<QString, double>& a, const QMap<QString, double>& b);

    // 实现处理方法
    void process(AcquisitionData &data) override;

private:
    QMap<QString, double> m_a; // 存储每个通道的校准系数 a
    QMap<QString, double> m_b; // 存储每个通道的校准系数 b
};

#endif // DATAPROCESSING_H
