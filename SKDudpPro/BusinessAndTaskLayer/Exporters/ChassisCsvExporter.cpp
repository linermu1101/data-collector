#include "ChassisCsvExporter.h"
#include <QFile>
#include <QTextStream>
#include <QDir>
#include <QFileInfo>
#include <QDebug>
#include "QsLog.h"

ChassisCsvExporter::ChassisCsvExporter() {
}

ChassisCsvExporter::~ChassisCsvExporter() {
}

/**
 * @brief 导出机箱数据到CSV文件
 * 
 * 将采集卡数据按照机箱配置分类，导出为指定路径的CSV文件。
 * 每个机箱的数据会保存在独立的子目录中。
 * 
 * @param cards 采集卡列表，键为卡号，值为通道模型数组
 * @param cardData 采集卡数据映射表，包含每个卡号的实际采集数据
 * @param exportPath 导出文件的完整路径，包含文件名
 */
void ChassisCsvExporter::exportData(const QMap<int, QVector<ChannelModel>> &cards, const QMap<int, AcquisitionData> &cardData, const QString &exportPath) {
    // 获取机箱配置实例
    ChassisConfig* chassisConfig = ChassisConfig::getInstance();

    // 记录导出开始日志
    QLOG_INFO() << "导出开始，目标路径：" << exportPath;
    QLOG_INFO() << "所有卡号：" << cards.keys();

    // 解析导出路径信息
    QFileInfo fileInfo(exportPath);
    QString baseDir = fileInfo.absolutePath();
    QString fileName = fileInfo.fileName();

    // 构建机箱到采集卡的映射关系
    QMap<QString, QMap<int, QVector<ChannelModel>>> chassisToCards;

    // 遍历所有采集卡，建立按机箱分类的映射关系
    for (auto it = cards.constBegin(); it != cards.constEnd(); ++it) {
        int cardNumber = it.key();
        QString chassisName = chassisConfig->getChassisForCard(cardNumber);

        if (chassisName.isEmpty()) {
            chassisName = "default";
        }

        chassisToCards[chassisName][cardNumber] = it.value();
    }

    // 如果没有找到任何机箱配置，跳过导出
    if (chassisToCards.isEmpty()) {
        qDebug() << "警告：未找到任何机箱配置，跳过导出";
        return;
    }

    // 遍历每个机箱，创建对应的目录并导出数据
    for (auto it = chassisToCards.constBegin(); it != chassisToCards.constEnd(); ++it) {
        QString chassisName = it.key();
        const QMap<int, QVector<ChannelModel>> &chassisCards = it.value();

        QString chassisDir = baseDir + "/" + chassisName;
        QDir dir;
        if (!dir.exists(chassisDir)) {
            if (!dir.mkpath(chassisDir)) {
                emit exportProgress(tr("无法创建机箱目录: %1").arg(chassisDir));
                continue;
            }
        }

        // 构建当前机箱的导出路径
        QString chassisExportPath = chassisDir + "/" + fileName;

        // 导出单个机箱数据
        exportToSingleFile(chassisCards, cardData, chassisExportPath, chassisName);
    }

    // 发送导出结束信号
    emit exportProgress("导出结束");
}

/**
 * 将机箱数据导出到单个CSV文件
 * @param cards 通道模型映射，按卡号分组
 * @param cardData 采集数据映射，按卡号分组
 * @param exportPath 导出文件路径
 * @param chassisName 机箱名称
 */
void ChassisCsvExporter::exportToSingleFile(const QMap<int, QVector<ChannelModel>> &cards, const QMap<int, AcquisitionData> &cardData, const QString &exportPath, const QString &chassisName)
{
    /**
     * 创建并打开输出文件
     * 如果文件打开失败，发送错误进度信号并返回
     */
    QFile file(exportPath);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QString errorMsg = tr("无法打开文件: %1，错误信息: %2").arg(exportPath).arg(file.errorString());
        qDebug() << "【导出失败】原因：" << errorMsg;
        emit exportProgress(errorMsg);
        return;
    }

    QTextStream out(&file);

    QStringList header;
    QMap<int, QVector<QVector<double>>> groupedChannelData;
    QMap<int, QVector<double>> groupedXAxis;

    /**
     * 按采样率对通道进行分组，并收集校准后的通道数据
     */
    QMap<int, QVector<ChannelModel>> samplingRateToChannels;
    for (const auto &channelList : cards) {
        for (const auto &channel : channelList) {
            int cardNumber = channel.getCardNumber();
            int channelIndex = channel.getChannelIndex();
            AcquisitionData percardData = cardData[cardNumber];
            int samplingRate = percardData.getSamplingRate();
            samplingRateToChannels[samplingRate].append(channel);
            QVector<double> tempData = percardData.getCalibratedChannelData(channelIndex);

            groupedChannelData[samplingRate].append(tempData);
        }
    }

    /**
     * 为每个采样率生成X轴时间戳
     * 构建CSV文件的头部信息
     */
    QMapIterator<int, QVector<ChannelModel>> mit(samplingRateToChannels);
    while (mit.hasNext()) {
        mit.next();
        int samplingRate = mit.key();
        const QVector<ChannelModel> &channels = mit.value();
        double samplingInterval = 1000.0 * 1000.0 / samplingRate;
        QVector<double> xAxis;
        if (!groupedChannelData[samplingRate].isEmpty()) {
            int maxSize = 0;
            for (const auto &data : groupedChannelData[samplingRate]) {
                if (data.size() > maxSize) {
                    maxSize = data.size();
                }
            }
            xAxis.reserve(maxSize);
            for (int i = 0; i < maxSize; ++i) {
                xAxis.append(i * samplingInterval);
            }
            groupedXAxis[samplingRate] = xAxis;
        }

        header << QString("time(us)@%1MHz").arg(samplingRate / 1000000);
        for (const auto &channel : channels) {
            header << channel.getChannelName();
        }
    }

    out << header.join(",") << "\n";

    /**
     * 确定需要写入的最大行数
     */
    int maxRow = 0;
    QMapIterator<int, QVector<QVector<double>>> it(groupedChannelData);
    while (it.hasNext()) {
        it.next();
        const QVector<QVector<double>> &channelsData = it.value();
        for (const auto &data : channelsData) {
            if (data.size() > maxRow) {
                maxRow = data.size();
            }
        }
    }

    /**
     * 写入数据行到CSV文件
     * 每行包含时间戳和对应的所有通道数据
     */
    for (int row = 0; row < maxRow; ++row) {
        QMapIterator<int, QVector<QVector<double>>> it(groupedChannelData);
        while (it.hasNext()) {
            it.next();
            int samplingRate = it.key();
            const QVector<QVector<double>> &channelsData = it.value();
            const QVector<double> &xAxis = groupedXAxis[samplingRate];

            if (row < xAxis.size()) {
                out << xAxis[row];
            } else {
                out << "";
            }
            out << ",";

            for (const auto &data : channelsData) {
                if (row < data.size()) {
                    out << data[row];
                } else {
                    out << "";
                }
                out << ",";
            }
        }
        out << "\n";
    }

    /**
     * 关闭文件并发送完成信号
     */
    file.close();
    qDebug() << "【导出完成】即将发射 exportProgress 信号";
    emit exportProgress(tr("机箱 %1导出完成 生成路径 %2").arg(chassisName).arg(exportPath));
}
