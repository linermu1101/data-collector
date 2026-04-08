#ifndef CSVEXPORTER_H
#define CSVEXPORTER_H

#include "IExporter.h"
#include "ChannelModel.h"
#include "../ChassisConfig.h"

class CsvExporter : public IExporter {
    Q_OBJECT
public:
    CsvExporter();
    ~CsvExporter() override;
    void exportData(const QMap<int, QVector<ChannelModel>> &cards, const QMap<int, AcquisitionData> &cardData, const QString &exportPath) override;
};

#endif // CSVEXPORTER_H
