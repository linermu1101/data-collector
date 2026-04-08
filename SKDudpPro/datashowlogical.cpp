#include "datashowlogical.h"

#include <QCoreApplication>
#include <QDateTime>

DataShowLogical::DataShowLogical(AcquisitionCard* acquisitionCard)
{
    parameterTransfer(acquisitionCard);
}

void DataShowLogical::plotDraw(QString destinationDataFile, QString destinationSettingsFile)
{
    QFile tempFile(destinationDataFile);
    QFile settingsFile(destinationSettingsFile);


    if (!settingsFile.exists()) {
        emit fileNotFoundSignal(destinationSettingsFile);
        emit showOver();
        return;
    }

    QSettings settings(destinationSettingsFile, QSettings::IniFormat);

    int samplingRate = settings.value("samplingRate").toInt();
    int sampingTime = settings.value("acquisitionTime").toInt();
    int PretriggerTime = this->acquisitionCard->getPretriggerTime();
    int PosttriggerTime = this->acquisitionCard->getPosttriggerTime();
    int pickUpMode = acquisitionCard->getPickUpIndex();
    int ChannelNumber=acquisitionCard->getSelectChannelNumber();
    int imageRefreshInterval = 0;
    int jumpTime = 1;
    double xNumber = 0.0;
    char tempBuffer[2];
    int count = 0;
    long loopNumber = 0;
    long progressLoopNumber = 0;
    int drawLoopNumber = 0;
    double maxY = 0;
    double minY = 0;
    double tempY = 0;
    double maxX = 0;
    double minX = 0;
    int firstYTimes = 0;
    int deleteDataTime = 0;
    int waitDrawTime = 10;
    int rawDataAnalysisIndex = acquisitionCard->getRawDataAnalysisIndex();
    int time = 0;
    if (PretriggerTime != 0 && PosttriggerTime != 0) {
        time = 1;
    } else if (PretriggerTime != 0) {
        time = PretriggerTime + sampingTime ;
    } else if (PosttriggerTime != 0) {
        time =sampingTime-PosttriggerTime;
    } else {
        time = sampingTime;
    }
    QList<int> channelStore;
    QList<QVector<double>*> channelXData;
    QList<QVector<double>*> channelYData;


    for ( int i = 0; i < ChannelNumber; i++) {
        QVector<double>* tempChannelXData = new QVector<double>;
        QVector<double>* tempChannelYData = new QVector<double>;

        channelXData.append(tempChannelXData);
        channelYData.append(tempChannelYData);



        emit letMainViewShowPlot(channelXData[i] , channelYData[i], minX, maxX, static_cast<double>(minY), static_cast<double>(maxY), i);
    }

    switch (pickUpMode) {
    case 0:
        jumpTime = samplingRate / 100000000;
        imageRefreshInterval = 1000000;
        deleteDataTime = 2500;
        waitDrawTime = 750;
        break;
    case 1:
        jumpTime = samplingRate / 50000000;
        imageRefreshInterval = 200000;
        deleteDataTime = 1000;
        waitDrawTime = 350;
        break;
    case 2:
        jumpTime = samplingRate / 10000000;
        imageRefreshInterval = 200000;
        deleteDataTime = 1000;
        waitDrawTime = 100;
        break;
    }

    if(rawDataAnalysisIndex != 0) {
        switch (samplingRate) {
            case 20000000:
                imageRefreshInterval = 250000;
                deleteDataTime = 2000;
                waitDrawTime = 700;
                break;
            case 250000000:
            imageRefreshInterval = 1000000;
            deleteDataTime = 2500;
            waitDrawTime = 750;
                break;
            case 500000000:
                imageRefreshInterval = 1000000;
                deleteDataTime = 4000;
                waitDrawTime = 800;
                break;
            case 1000000000:
                imageRefreshInterval = 1000000;
                deleteDataTime = 8000;
                waitDrawTime = 900;
                break;
            case 1250000000:
                imageRefreshInterval = 1000000;
                deleteDataTime = 12000;
                waitDrawTime = 1000;
                break;
        }
    }


    if(tempFile.open(QIODevice::ReadOnly)) {

        for (int i = 0; i < ChannelNumber; i++) {//11
            QString ch = QString("CH%1-%2").arg(acquisitionCard->getDisplayCardNumber()).arg(i);
            qDebug() << QStringLiteral("通道序号：") << i
                     << QStringLiteral("系数a：") << coefficientVector[ch]
                     << QStringLiteral("系数b：") << coefficientbVector[ch];

            minY = 0;
            maxY = 0;
            xNumber = 0;
            //double scalingFactor = 0.0002892135;
             //double scalingFactor = 1.0;
                if (PretriggerTime != 0 && PosttriggerTime != 0) {
                    xNumber = 1;
                } else if (PretriggerTime != 0) {
                    xNumber = -PretriggerTime;
                } else if (PosttriggerTime != 0) {
                    xNumber = PosttriggerTime;
                } else {
                    xNumber = 0;
                }

                int   singleChannelDataNumber = (samplingRate / 1000 / 1000) * time;
                double  samplingInterval = 1000.0 * 1000.0 / samplingRate;

//qDebug()<<"singleChannelDataNumber"<<singleChannelDataNumber;
//qDebug()<<"samplingInterval"<<samplingInterval;
//qDebug()<<"countFirstsampRate"<<countFirstsampRate;

                long percentageAnalysisDataVolume =  singleChannelDataNumber / 10;
                long startRawData = percentageAnalysisDataVolume * (rawDataAnalysisIndex - 1);
                long endRawData = percentageAnalysisDataVolume * rawDataAnalysisIndex;


                for ( int j = 0; j < singleChannelDataNumber; j++) {//33

                    tempFile.read(tempBuffer, sizeof(tempBuffer));

                    if(rawDataAnalysisIndex != 0) {//44

                        if(rawDataAnalysisIndex == 11) {//55

                            qint16 val = (qint16) ((tempBuffer[0] & 0xFF) << 8) | (tempBuffer[1] & 0xFF);
                            int decimalVal = static_cast<int>(val);
//                            if (val & 0x8000) {
//                                int complement =~(val) + 1;
//                                 decimalVal = -complement;
//                            }


                            double result = decimalVal *coefficientVector[ch];

                            if (firstYTimes == 0) {
                                minY = result;
                                maxY = result;
                                firstYTimes++;
                            }

                            tempY = result;
                            minY = minY < tempY ? minY : tempY;
                            maxY = maxY > tempY ? maxY : tempY;
                            maxX = maxX > xNumber ? maxX : xNumber;
                            // double ave = (minY + maxY) / 2; //此处疑是bug
                            // int coefficientVectorb = coefficientbVector[i];
                            // result +=ave*coefficientVectorb;

                            //系数b处理
                            result += coefficientbVector[ch];

                            channelXData[i]->append(xNumber);
                            channelYData[i]->append(static_cast<double>(result));

                            if (drawLoopNumber % imageRefreshInterval == 0) {
                                emit letMainViewShowPlot(channelXData[i] , channelYData[i], minX, maxX, static_cast<double>(minY), static_cast<double>(maxY), i);
                                delay_ms(waitDrawTime);
                            }

                            drawLoopNumber++;
                        }//55

                        if(j >= startRawData && j <= endRawData) {//66
                            qint16 val = (qint16) ((tempBuffer[0] & 0xFF) << 8) | (tempBuffer[1] & 0xFF);
                            int decimalVal = static_cast<int>(val);
                            double result = decimalVal *coefficientVector[ch];

                            // 标定完成后，将以下decimalVal修改为result,即乘上系数的值
                            if (firstYTimes == 0) {
                                minY = result;
                                maxY =result;
                                firstYTimes++;
                            }

                            tempY = result;
                            minY = minY < tempY ? minY : tempY;
                            maxY = maxY > tempY ? maxY : tempY;
                            maxX = maxX > xNumber ? maxX : xNumber;

                            //系数b处理
                            result += coefficientbVector[ch];

                            channelXData[i]->append(xNumber);
                            channelYData[i]->append(static_cast<double>(result));

                            if (drawLoopNumber % imageRefreshInterval == 0) {
                                emit letMainViewShowPlot(channelXData[i] , channelYData[i], minX, maxX, static_cast<double>(minY), static_cast<double>(maxY), i);
                                delay_ms(waitDrawTime);
                            }

                            drawLoopNumber++;
                        }//66
                    }//44

                    if ((loopNumber % jumpTime == 0) && (rawDataAnalysisIndex == 0)) {


                        qint16 val = (qint16) ((tempBuffer[0] & 0xFF) << 8) | (tempBuffer[1] & 0xFF);
                        int decimalVal = static_cast<int>(val);
                        double result = decimalVal * coefficientVector[ch];//scalingFactor;//


                        if (firstYTimes == 0) {
                            minY = result;
                            maxY = result;
                            firstYTimes++;
                        }

                        tempY = result;
                        minY = minY < tempY ? minY : tempY;
                        maxY = maxY > tempY ? maxY : tempY;
                        maxX = maxX > xNumber ? maxX : xNumber;
                        //int ave = (minY + maxY) / 2;

                        //系数b处理
                        result += coefficientbVector[ch];

                        channelXData[i]->append(xNumber);
                        channelYData[i]->append(static_cast<double>(result));

                        if (drawLoopNumber % imageRefreshInterval == 0) {
                            // //系数b处理
                            // if (!channelYData.isEmpty() && channelYData[i] != nullptr) {
                            //     // double maxValue = *std::max_element(channelYData[i]->begin(), channelYData[i]->end());
                            //     // double minValue = *std::min_element(channelYData[i]->begin(), channelYData[i]->end());
                            //     // double avg = (minValue + maxValue) / 2;
                            //     //qDebug() << "Max Value: " << maxValue << "Min Value: " << minValue;

                            //     for (int m = 0; m < channelYData[i]->size(); ++m) {
                            //         //(*channelYData[i])[m] += avg * coefficientbVector[i];
                            //         (*channelYData[i])[m] += coefficientbVector[i];
                            //     }
                            // }

                            emit letMainViewShowPlot(channelXData[i] , channelYData[i], minX, maxX, static_cast<double>(minY), static_cast<double>(maxY), i);
                            delay_ms(waitDrawTime);
                        }

                        drawLoopNumber++;
                    }

                    xNumber += samplingInterval;
                    progressLoopNumber++;
                    loopNumber++;
                    count++;
                }//33


                emit letMainViewShowPlot(channelXData[i] , channelYData[i], minX, maxX, static_cast<double>(minY), static_cast<double>(maxY), i);
                loopNumber = 0;
        }//11
    } else {
        emit fileNotFoundSignal(destinationDataFile);
        emit showOver();
        return;
    }

    dataAway(minX, maxX, minY, maxY);
    delay_ms(deleteDataTime);

    for ( int i = 0; i < ChannelNumber; i++) {
        delete channelXData[i];
        delete channelYData[i];
    }

    tempFile.close();
    emit showOver();
}
void DataShowLogical::handleCoefficientVector(const QMap<QString, double>& vector)
{
    coefficientVector=vector;
   // qDebug() << coefficientVector;
}

void DataShowLogical::handleCoefficientbVector(const QMap<QString, double> &vector)
{
    coefficientbVector=vector;
   // qDebug() << coefficientbVector;
}
void DataShowLogical::dataAway(double xStart, double xFinal, double yStart, double yFinal)
{
    Parameter::plot2DXStart = xStart;
    Parameter::plot2DXFinal = xFinal;
    Parameter::plot2DYStart = yStart;
    Parameter::plot2DYFinal = yFinal;
}

void DataShowLogical::parameterTransfer(AcquisitionCard *acquisitionCard)
{
    this->acquisitionCard = acquisitionCard;
}

void DataShowLogical::delay_ms(int mes)
{
    QTime dieTime = QTime::currentTime().addMSecs(mes);
    while(dieTime>QTime::currentTime()){
        QCoreApplication::processEvents(QEventLoop::AllEvents,100);
    }
}



