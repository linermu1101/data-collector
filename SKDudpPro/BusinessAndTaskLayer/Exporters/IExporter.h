#ifndef IEXPORTER_H
#define IEXPORTER_H

#include <QObject>
#include <QString>
#include <QMap>
#include <QVector>
#include "ChannelModel.h"
#include "AcquisitionData.h"

class IExporter : public QObject {
    Q_OBJECT

public:
    explicit IExporter(QObject *parent = nullptr) {}
    virtual void exportData(const QMap<int, QVector<ChannelModel>> &cards,
                            const QMap<int, AcquisitionData> &cardData,
                            const QString &exportPath) = 0;
    virtual ~IExporter() = default;  // 改为 public 析构函数

signals:
    void exportProgress(const QString &message);
};

#endif // IEXPORTER_H