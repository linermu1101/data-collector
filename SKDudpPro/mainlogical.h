#ifndef MAINLOGICAL_H
#define MAINLOGICAL_H

#include <QWidget>
#include <QDateTime>
#include <QVBoxLayout>
#include <QPushButton>
#include <QLabel>
#include <QMap>
#include <QDebug>
#include <QPlainTextEdit>
#include <windows.h>
#include <QThread>
#include <QMetaType>
#include <QtConcurrent/QtConcurrent>
#include <QtCore/QtGlobal>
#include <functional>

#include "cardinformation.h"
#include "parameter.h"
#include "acquisitioncard.h"
#include "datainteractionlogical.h"
#include "QsLog.h"

class MainLogical : public QWidget
{
    Q_OBJECT
public:
    explicit MainLogical(QWidget *parent = nullptr);
    explicit MainLogical(QWidget *parent = nullptr, AcquisitionCard* acquisitionCard = nullptr);
    ~MainLogical();

    QList<CardInformation*>* returnViewCardInformations();

 	QMap<int, QString> getCorrespondingMap() const;
    QMap<int, QString> getPCIESortedMap() const;
    QMap<QString, double> getchannel_coefficients() const;
    QMap<QString, double> getchannel_coefficientsb() const;
    void addcoefficient();

    QList<DataInteractionLogical *> *returnDataInteractionLogicals();

    QStringList getipList() const;

private:
    void initPCIENumber();

    QMap<int, QString> PCIEMap;
    QMap<int, QString> PCIESortedMap;
    QMap<int, QString> correspondingMap;
    QMap<QString, double> channel_coefficients;
    QMap<QString, double> channel_coefficientsb;
   // QMap<int, QString> correspondingMap;
    QStringList presetIpsAndPorts;
    QStringList ipList;
    QList<CardInformation*>* cardInformations;
    QList<int> insertionConditions;
    QList<int> sortIndexList;
    QList<QString> sortSeriesNumberList;
    AcquisitionCard* acquisitionCard;
    DataInteractionLogical *dataInteractionLogical;
    QList<QThread*> threads;
    QList<DataInteractionLogical*>* dataInteractionLogicals;

    void cardInformationInit();
    void slotSorting();
    void dataInteractionlogicalInit();
    void parameterToML(AcquisitionCard *acquisitionCard);
    void addRemoteHost();

};

#endif // MAINLOGICAL_H
