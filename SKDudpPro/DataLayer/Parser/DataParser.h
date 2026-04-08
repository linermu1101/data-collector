#ifndef DATAPARSER_H
#define DATAPARSER_H

#include <QObject>
#include <QFile>
#include <QVector>
#include <QSettings>
#include <QMap>
#include "AcquisitionData.h"
#include "IDataProcessor.h"

/**
 * @brief 原始数据解析器
 */
class DataParser : public QObject{
    Q_OBJECT
    
public:
    explicit DataParser(QObject *parent = nullptr);
    ~DataParser();

    // 解析多个文件
    bool parseMutiData(const QMap<int, QString> &dataFilePaths);
    // 获取数据
    QMap<int, AcquisitionData> getMutiAcquisitionData(IDataProcessor* processor) const;

    // 解析单一通道
    bool parseChannelDatFile(int cardNumber, int channelNumber, const QString &dataFilePath);
    // 获取单个通道数据
    AcquisitionData* getAcquisitionChannelDataPtr(IDataProcessor* processor) const;
signals:
    void parsingInfo(const QString &info);
    
private:
    bool parseDatWholeFile(int cardNumber, const QString &dataFilePath);

    AcquisitionData *m_acquisitionData;  // 存储卡信息
    QMap<int, AcquisitionData> cardData; // 存储采集数据
};

#endif // DATAPARSER_H
