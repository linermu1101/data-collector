#include "ExportWorker.h"

// 构造函数：初始化导出工作线程
// 参数 cardData: 卡号与采集数据的映射
// 参数 exportPath: 数据导出的路径
// 参数 exporter: 导出器对象
ExportWorker::ExportWorker(const QMap<int, QVector<ChannelModel>> &channels, const QMap<int, AcquisitionData> &cardData, const QString &exportPath, IExporter *exporter)
    : channels(channels), cardData(cardData), exportPath(exportPath), exporter(exporter) {}

// 线程执行的主函数：执行数据导出操作
void ExportWorker::run() {
    // 使用导出器导出数据
    exporter->exportData(channels, cardData, exportPath);

    // 导出完成后发射 finished 信号
    emit finished();
}
