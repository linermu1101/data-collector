#ifndef DATASHOWLOGICAL_H
#define DATASHOWLOGICAL_H

#include <QObject>
#include <QFile>
#include <QSettings>
#include "acquisitioncard.h"
#include "QRegularExpression"
#include "parameter.h"
#include "QDebug"

class DataShowLogical : public QObject
{
    Q_OBJECT
public:
    explicit DataShowLogical(QObject *parent = nullptr);
    DataShowLogical(AcquisitionCard *acquisitionCard);

public slots:
    void plotDraw(QString destinationDataFile, QString firstdestinationSettingsFile);
    void handleCoefficientVector(const QMap<QString, double>& vector);
    void handleCoefficientbVector(const QMap<QString, double>& vector);

signals:
    void showOver();
    void fileNotFoundSignal(QString filePath);
    void letMainViewShowPlot(QVector<double>* channelXData, QVector<double>* channelYData, double minX, double maxX, double minY, double maxY, int index);


private:
    AcquisitionCard* acquisitionCard;//在这个函数中定义一个acquisitionCard，调用AcquisitionCard的类(class)（在acquisitionCard.h中）
    QString acquisitionNumber;
    QString cardNumber;
    QMap<QString, double> coefficientVector;
    QMap<QString, double> coefficientbVector;

    void dataAway(double xStart, double xFinal, double yStart, double yFinal);
    void parameterTransfer(AcquisitionCard *acquisitionCard);
    void delay_ms(int mes);
    void analysisNumber(QString filePath);

};

#endif // DATASHOWLOGICAL_H
