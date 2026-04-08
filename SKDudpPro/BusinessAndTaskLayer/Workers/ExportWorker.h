#ifndef EXPORTWORKER_H
#define EXPORTWORKER_H

#include <QThread>
#include <QMap>
#include <QString>
#include "IExporter.h"
#include "ChannelModel.h"

class AcquisitionData;

// 导出工作线程类
class ExportWorker : public QThread {
    Q_OBJECT

public:
    // 构造函数：初始化导出工作线程
    // 参数 cardData: 卡号与采集数据的映射
    // 参数 exportPath: 数据导出的路径
    // 参数 exporter: 导出器对象
    ExportWorker(const QMap<int, QVector<ChannelModel>> &channels, const QMap<int, AcquisitionData> &cardData, const QString &exportPath, IExporter *exporter);

protected:
    // 线程执行的主函数：执行数据导出操作
    void run() override;

signals:
    void finished();          // 导出完成信号

private:
    QMap<int, QVector<ChannelModel>> channels;
    QMap<int, AcquisitionData> cardData; // 卡号对应的数据
    QString exportPath;                   // 导出路径
    IExporter *exporter;                  // 导出器对象
};

#endif // EXPORTWORKER_H
