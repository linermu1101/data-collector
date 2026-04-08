#ifndef EXPORTTASK_H
#define EXPORTTASK_H

#include <QObject>
#include <QMap>
#include <QMutex>
#include <QMutexLocker>
#include "IDataProcessor.h"
#include "IExporter.h"
#include "ExportWorker.h"
#include "ChannelModel.h"

class ExportTask : public QObject {
    Q_OBJECT

public:
    ExportTask(QMap<int, QVector<ChannelModel>> channels, const QMap<int, QString> &dataFilePaths, const QMap<int, QString> &iniFilePaths,
               const QString &exportPath, IExporter* exporter, IDataProcessor* processor = nullptr);

    void startExport();

signals:
    void exportFinished(const QString &exportPath);                   // 导出完成信号
    void exportFailed(const QString &error); // 导出失败信号

private:
    QMap<int, QVector<ChannelModel>> channels; // 目标通道集
    QMap<int, QString> dataFilePaths; // 数据文件路径
    QMap<int, QString> iniFilePaths;   // 配置文件路径
    QString exportPath;                 // 导出路径
    IExporter* exporter;                // 导出器
    IDataProcessor* processor;          // 数据处理器
    ExportWorker* worker;
    QMutex mutex;
};

#endif // EXPORTTASK_H
