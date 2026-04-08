QT       += core gui printsupport opengl network sql charts

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

LIBS += -lopengl32

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

DEFINES += QT_DEPRECATED_WARNINGS
DEFINES += QCUSTOMPLOT_USE_OPENGL


INCLUDEPATH += $$PWD/Module/Header
INCLUDEPATH += $$PWD/../3rdparty/QsLog
include($$PWD/../3rdparty/QsLog/QsLog.pri)
INCLUDEPATH += $$PWD/DataLayer/Model
INCLUDEPATH += $$PWD/DataLayer/Parser
INCLUDEPATH += $$PWD/DataLayer/Processing
INCLUDEPATH += $$PWD/BusinessAndTaskLayer
INCLUDEPATH += $$PWD/BusinessAndTaskLayer/Workers
INCLUDEPATH += $$PWD/BusinessAndTaskLayer/Exporters

SOURCES += \
    BusinessAndTaskLayer/ChassisConfig.cpp \
    BusinessAndTaskLayer/ExportTask.cpp \
    BusinessAndTaskLayer/Exporters/ChassisCsvExporter.cpp \
    BusinessAndTaskLayer/Exporters/CsvExporter.cpp \
    BusinessAndTaskLayer/Exporters/ExporterFactory.cpp \
    BusinessAndTaskLayer/Workers/ExportWorker.cpp \
    DataLayer/Model/AcquisitionData.cpp \
    DataLayer/Model/ChannelModel.cpp \
    DataLayer/Parser/DataParser.cpp \
    DataLayer/Processing/DataProcessing.cpp \
    acquisitioncard.cpp \
    cardinformation.cpp \
    datainteractionlogical.cpp \
    # dataloader.cpp \
    datashowlogical.cpp \
    main.cpp \
    mainlogical.cpp \
    mainview.cpp \
    pcietoolkits/pciefunction.cpp \
    pcietoolkits/sliderruler.cpp \
    planargraph.cpp \
    qcustomplot.cpp \
    udp.cpp

HEADERS += \
    BusinessAndTaskLayer/ChassisConfig.h \
    BusinessAndTaskLayer/ExportTask.h \
    BusinessAndTaskLayer/Exporters/ChassisCsvExporter.h \
    BusinessAndTaskLayer/Exporters/CsvExporter.h \
    BusinessAndTaskLayer/Exporters/ExporterFactory.h \
    BusinessAndTaskLayer/Exporters/IExporter.h \
    BusinessAndTaskLayer/Workers/ExportWorker.h \
    DataLayer/Model/AcquisitionData.h \
    DataLayer/Model/ChannelModel.h \
    DataLayer/Parser/DataParser.h \
    DataLayer/Processing/DataProcessing.h \
    DataLayer/Processing/IDataProcessor.h \
    acquisitioncard.h \
    cardinformation.h \
    datainteractionlogical.h \
    # dataloader.h \
    datashowlogical.h \
    datatransferinterface.h \
    mainlogical.h \
    mainview.h \
    parameter.h \
    pcietoolkits/header.h \
    pcietoolkits/pciefunction.h \
    pcietoolkits/sliderruler.h \
    planargraph.h \
    qcustomplot.h \
    udp.h

FORMS += \
    mainview.ui



# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

win32: LIBS += -lSetupAPI

win32:CONFIG(release, debug|release): LIBS += -L$$PWD/pcietoolkits/resources/ -lfreeglut -lOpengl32
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/pcietoolkits/resources/ -lfreeglut -lOpengl32

INCLUDEPATH += $$PWD/Module
DEPENDPATH += $$PWD/Module

RESOURCES += \
    res.qrc
