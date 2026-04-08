#include "CsvExporter.h"
#include <QFile>
#include <QTextStream>
#include <QDir>


CsvExporter::CsvExporter() {
}

CsvExporter::~CsvExporter() {
}


//导出数据到指定路径
void CsvExporter::exportData(const QMap<int, QVector<ChannelModel>> &cards, const QMap<int, AcquisitionData> &cardData, const QString &exportPath) {
    // 打开文件以写入模式
    QFile file(exportPath);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        if (file.error() == QFile::FileError::OpenError) {
            emit exportProgress(tr("无法打开文件: %1，可能文件已被其他进程打开或没有写权限").arg(exportPath));
        } else {
            emit exportProgress(tr("无法打开文件: %1，错误信息: %2").arg(exportPath).arg(file.errorString()));
        }
        return;
    }

    // 创建文本流对象
    QTextStream out(&file);

    QStringList header;
    QVector<QVector<double>> allChannelData; // 用于存储所有的 channelData
    QVector<QVector<double>> allCardXAxis; // 用于存储每个卡对应的X轴数据
    int maxChannelDataSize = 0; // 用于存储最大的 channelData.size() 值

    // 加载每张卡的X数据到allCardXAxis
    QVector<double> xAxis;
    QVector<double> channelLastData;
    for (const auto &channelList : cards) {
        header << "card_time(us)";
        double  percardSamplingInterval = 0;
        QVector<double> i;
        // 遍历每张选择卡的选中的通道
        for (const auto &channel : channelList) {
            int cardNumber = channel.getCardNumber();
            int channelIndex = channel.getChannelIndex();
            AcquisitionData percardData = cardData[cardNumber];
            int percardSamplingRate = percardData.getSamplingRate();
            percardSamplingInterval = 1000.0 * 1000.0 / percardSamplingRate; // x轴间隔
            QVector<double> tempData = percardData.getCalibratedChannelData(channelIndex);

            if(tempData.size() > 0)
            {
                channelLastData = tempData;
            }
            else{
                QString msg = QString("%1_%2通道 数据解析未成功").arg(cardNumber).arg(channelIndex);
                emit exportProgress(msg);
            }

            allChannelData.append(tempData);

            if (channelLastData.size() > maxChannelDataSize) {
                maxChannelDataSize = channelLastData.size();
            }
            header << channel.getChannelName();
        }

        // 创建当前卡的X轴数据
        xAxis.clear();
        for(int x = 0; x < channelLastData.size(); ++x) {
            xAxis.append(x * percardSamplingInterval);
        }
        allCardXAxis.append(xAxis);
    }


    // 写入 CSV 文件的表头
    out << header.join(",") << "\n";

    // 写入 CSV 文件的数据行
    auto keys = cards.keys(); // 获取所有键值，即所有的cardNumber

    for (int row = 0; row < maxChannelDataSize; ++row) {
        // 每一列遍历
        // 列-卡遍历
        for (int c = 0; c < cards.size(); ++c) {
            // 写入 X 轴值（即时间戳）
            if (row < allCardXAxis[c].size()) {
                out << allCardXAxis[c][row];
            } else {
                out << ""; // 如果数据不足，写入空白
            }
            out << ",";

            int cardKey = keys[c];
            QVector<ChannelModel> channelList = cards.value(cardKey);

            for (int j = 0; j < channelList.size(); ++j) {
                ChannelModel channel = channelList[j];

                int cardNumber = channel.getCardNumber();
                int channelIndex = channel.getChannelIndex();
                AcquisitionData percardData = cardData[cardNumber];
                int percardSamplingRate = percardData.getSamplingRate();
                const QVector<double>& channelData = percardData.getCalibratedChannelData(channelIndex);

                if (row < channelData.size()) {
                    out << channelData[row];
                } else {
                    out << ""; // 如果数据不足，写入空白
                }
                if ( (j < channelList.size() - 1) && (c< cards.size() - 1)) {

                }

                out << ",";

            }
        }
        out << "\n";
    }

    // 关闭文件
    file.close();
    emit exportProgress("导出完成");
}
