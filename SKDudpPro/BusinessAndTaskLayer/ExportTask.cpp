#include "ExportTask.h"
#include "DataParser.h"

ExportTask::ExportTask(QMap<int, QVector<ChannelModel>> channels, const QMap<int, QString> &dataFilePaths, const QMap<int, QString> &iniFilePaths,
                       const QString &exportPath, IExporter* exporter, IDataProcessor* processor)
    : channels(channels), dataFilePaths(dataFilePaths), iniFilePaths(iniFilePaths), exportPath(exportPath),
    exporter(exporter), processor(processor), worker(nullptr)
{

}

void ExportTask::startExport() {
    QMutexLocker locker(&mutex);

    // 确保之前的 worker 已经被删除
    if (worker) {
        worker->disconnect();
        worker->requestInterruption();
        worker->wait();
        worker->deleteLater();
        worker = nullptr;
    }


    DataParser parser;
    connect(&parser, &DataParser::parsingInfo, [this](const QString &info) {
        emit exportFailed(info);
    });
    if (!parser.parseMutiData(dataFilePaths)) {
        //emit exportFailed("Failed to parse files.");
        return;
    }

    // 获取所有卡的数据
    QMap<int, AcquisitionData> selectedCardData = parser.getMutiAcquisitionData(processor);

    // 启动导出线程
    worker = new ExportWorker(channels, selectedCardData, exportPath, exporter);
    //worker->setParent(this);

    // 连接导出工作线程的信号
    QObject::connect(worker, &ExportWorker::finished, [this]() {
        emit exportFinished(exportPath);
        // worker->deleteLater();
        // worker = nullptr;
    });


    worker->start();
}
