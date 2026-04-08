#ifndef CHASSISCSVPXPORTER_H
#define CHASSISCSVPXPORTER_H

#include <QObject>
#include "IExporter.h"
#include "ChannelModel.h"
#include "../ChassisConfig.h"

/**
 * @brief ChassisCsvExporter CSV导出逻辑类
 * 按物理机箱分组存储 每个采样率只配置一个时间轴
 */
class ChassisCsvExporter : public IExporter {
    Q_OBJECT
public:
    ChassisCsvExporter();
    ~ChassisCsvExporter() override;
    void exportData(const QMap<int, QVector<ChannelModel>> &cards, const QMap<int, AcquisitionData> &cardData, const QString &exportPath) override;

private:
    void exportToSingleFile(const QMap<int, QVector<ChannelModel>> &cards, const QMap<int, AcquisitionData> &cardData, const QString &exportPath, const QString &chassisName = QString());
};

#endif // CHASSISCSVPXPORTER_H
