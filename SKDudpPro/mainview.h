#ifndef MAINVIEW_H
#define MAINVIEW_H

#include <QMainWindow>
#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QCheckBox>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QFrame>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenu>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QSpinBox>
#include <QtWidgets/QTabWidget>
#include <QtWidgets/QTableWidget>
#include <QtWidgets/QToolBar>
#include <QtWidgets/QToolButton>
#include <QtWidgets/QTreeWidget>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>
#include <QDataStream>
#include "planargraph.h"
#include "acquisitioncard.h"
#include "mainlogical.h"
#include "datashowlogical.h"
#include "datatransferinterface.h"
#include <QUdpSocket>
#include "udp.h"
#include <QPluginLoader>

//#include "dataloader.h"
#include "DataParser.h"
#include "AcquisitionData.h"
#include "DataProcessing.h"
#include "IDataProcessor.h"
#include "CsvExporter.h"
#include "IExporter.h"
#include "ExportTask.h"
#include "ChannelModel.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainView; }
QT_END_NAMESPACE

class MainView : public QMainWindow
{
    Q_OBJECT

public:
    MainView(QWidget *parent = nullptr);
    ~MainView();

public slots:
    void receieveShowOver();
    void statisticalCollectionTimes();
    void mainViewDrawPlot(QVector<double>* channelXData, QVector<double>* channelYData, double minX, double maxX, double minY, double maxY, int index);
    void mainViewFileNotFound(QString filePath);
    // void beforeHardTriggerStart(int value);
    //void saveFileCommon(int acquisitionNumber, int channelNumber, int PretriggerTime, int PosttriggerTime, int sampingTime, const QString& basePath);
    void saveFileClicked();
	void historySaveFileClicked();
    void onExportFinished(const QString &exportPath);
    void exportCommon(QString prefixPath, int acquisitionNumber, QString exportPath);
    void autocalculated();
    QString getMonthDayPath() const;
    void connectToHost();
    void connectTest();
    void disconnectFromHost();
    void startudpCollect();
    void handleAcquisitionTimeout(); // 处理采集超时
    void handleDataReceived(const QString &ipAddress); // 处理数据接收通知

signals:
    void startDataShowLogic(QString dataFile, QString settingsFile);
    void hardTriggerStart();
    void putOverLog(QString logs);
 	void acardStatusChanged(int index, bool isChecked);
    void resetAcardStatuses();
    void sendBufferSize(const QString &ipAddress, int size);
    // void coefficientVectorUpdated(const QVector<double>& vector);
    // void coefficientbVectorUpdated(const QVector<double>& vector);
    void coefficientVectorUpdated(const QMap<QString, double>& vector);
    void coefficientbVectorUpdated(const QMap<QString, double>& vector);
private:
    Ui::MainView *ui;

    int totalCards;
    int cardsPerPage;
    int currentPage;
    QPushButton* prevPageButton;
    QPushButton* nextPageButton;

    QToolBar* toolBar;
    QPushButton *startBtn;
    QPushButton* resetBtn;
    QPushButton* analysisBtn;

    QPushButton* autoFindBtn;
    QPushButton* WaitBtn;
    QWidget *centralwidget;
    QWidget *widget;
    QTabWidget* tabWidget;
    QTableWidget* tableWidget;
    //QPushButton* channelBtn[78];
    QPushButton* channelBtn[1078];

    QVBoxLayout *verticalLayout;
    QVBoxLayout *verticalLayout_2;
    QVBoxLayout *verticalLayout_3;
    QVBoxLayout *verticalLayout_4;
    QVBoxLayout *verticalLayout_5;
    QVBoxLayout *verticalLayout_6;

    QHBoxLayout *horizontalLayout_18;
    QHBoxLayout *horizontalLayout_3;
    QLabel *pickUpLabel;
    QSpacerItem *horizontalSpacer_3;
    QHBoxLayout *horizontalLayout_4;
    QToolButton *pickUpIcon;
    QComboBox *pickUpBox;
    QHBoxLayout *horizontalLayout_14;

    QHBoxLayout *horizontalLayout_6;
    QLabel *earlyTriggerLabel;
    QSpacerItem *horizontalSpacer_4;
    QHBoxLayout *horizontalLayout_7;
    QToolButton *earlyTriggerIcon;
    QSpinBox * PretriggerSpinBox;

    QHBoxLayout *horizontalLayout_8;
    QLabel *afterTriggerLabel;
    QSpacerItem *horizontalSpacer_5;
    QHBoxLayout *horizontalLayout_9;
    QToolButton *afterTriggerIcon;
    QSpinBox *PosttriggerSpinBox;
    QHBoxLayout *horizontalLayout_15;
    QFrame *frame_4;
    QHBoxLayout *horizontalLayout_10;
    QFrame *frame_5;
    QHBoxLayout *horizontalLayout_11;
    QLabel *inputChannelLabel;
    QSpacerItem *horizontalSpacer_6;
    QTreeWidget *treeWidget;

    QVBoxLayout* inputSettingLayout;

    QHBoxLayout* acquisitionsamplerateLayout;
    QLabel* acquisitionsamplerate;
    QComboBox* acquisitionsamplerateBox;

    QHBoxLayout* acquisitionTimeLayout;
    QLabel* acquisitionTime;
    QSpinBox* acquisitionTimeBox;

    QHBoxLayout* acquisitionNumberLayout;
    QLabel* acquisitionNumberLable;
    QSpinBox* acquisitionNumberSpinBox;

    QHBoxLayout* acquisitionNumberAutomationLayout;
    QLabel* acquisitionNumberAutomationLabel;
    QComboBox* acquisitionNumberAutomationComboBox;

    QHBoxLayout* fileStorageLocationLayout;
    QLabel* fileStorageLocationLabel;
    QLineEdit* fileStirageLocationEdit;

    QHBoxLayout* fileHistoryLocationLayout;
    QLabel* fileHistoryLocationLabel;
    QLineEdit* fileHistoryLocationEdit;

    QHBoxLayout* acquisitionFileNameLayout;
    QLabel* acquisitionFileNameLabel;
    QLineEdit* acquisitionFileNameEdit;
    QCheckBox* autoFileNamedCheckBox;

    QHBoxLayout* channelShowLayout;


    QHBoxLayout* cardNumberLayout;
    QLabel* cardNumberLabel;
    QComboBox* cardNumberBox;

    QHBoxLayout* modeChangeLayout;
    QLabel* modeChangeLabel;
    QComboBox* modeChangeBox;

    QHBoxLayout* daqCaseLayout;//机箱选择
    QLabel* daqCaseLabel;
    QComboBox* daqCaseBox;

    QHBoxLayout* collectModeLayout;
    QLabel* collectModeLabel;
    QComboBox* collectModeBox;

    QHBoxLayout* rawDataAnalysisLayout;
    QLabel* rawDataAnalysisLabel;
    QComboBox* rawDataAnalysisBox;

      QHBoxLayout* baseline1Layout;
    QLabel* baseline1Label;
    QSpinBox* baseline1SpinBox;

    QHBoxLayout* baseline2Layout;
    QLabel* baseline2Label;
    QSpinBox* baseline2SpinBox;

    QHBoxLayout* baseline3Layout;
    QLabel* baseline3Label;
    QSpinBox* baseline3SpinBox;

    QHBoxLayout* baseline4Layout;
    QLabel* baseline4Label;
    QSpinBox* baseline4SpinBox;
	
	QHBoxLayout*localLayout;
    QComboBox *localIpComboBox;
    udp *udpHelper;
    QPushButton *connectButton;
    QPushButton *testButton;
    QPushButton *disconnectButton;
    QLineEdit *localPortLineEdit;

    QPushButton*AcardSetButton;
    QPushButton* AcardReSetButton;
    QPushButton*readriggerButton;
    QPushButton*SaveFileButton;
    QPushButton* historyFileButton;
    QPushButton *historySaveFileButton;
    QPushButton *AutoCalculateButton;//自动校准

    QLabel* localLabel;
    QLabel *logsprintLabel;
    QPlainTextEdit *logsPrint;
    QPushButton* ClearButton;
    QLabel* statusIndicator;
    QHBoxLayout *horizontalLayout_17;
    QFrame *channelShowFrame;
    QScrollArea* scrollArea;
    QPushButton* allChooseButton;
    QList<QPushButton*> cardButtons;
    QList<QPushButton*> channelButtons;

    QSpacerItem *horizontalSpacer_10;
    QToolButton *toolButton_15;
    QFrame *frame_9;

    QMenuBar *menubar;
    QMenu *filemenu;
    QMenu *newMenu;
    QMenu *helpMenu;

    PlanarGraph *planarGraph;
    QList<QColor> plotColorList;

    QTimer* timer;
    AcquisitionCard* acquisitionCard;
    MainLogical* mainLogical;
    CardInformation* cardInformation;
    DataShowLogical* dataShowLogical;
    DataInteractionLogical* dataInteractionLogical;
    QList<DataInteractionLogical*>* dataInteractionLogicals;
    QList<CardInformation*>* cardInformations;
    QMutex acquisitionMutex;
    QMutex triggerMutex;
    QMutex hardTriggerMutex;
    QThread* _mThread;
    DataTransferInterface*_driverPlugin;

    //存储每个cardButton和其对应的channelButtons的关系
    QMap<QPushButton*, QList<QPushButton*>> cardToChannels;
    QMap<int, QString> PCIESortedMap;
    // QVector<double> coefficientVector;
    // QVector<double> coefficientbVector;

    QMap<QString, double> m_channel_coefficients;
    QMap<QString, double> m_channel_coefficientsb;


    int beforeHardTriggerStartIndex = 0;
    int rawDataChangeTime = 0;
    int acquisitionExecutionTimes = 0;
    int triggerTimes = 0;
    int autoIncreasedFlag = 0;
    int dataAnalysisFlag = 0;
    int buttoncard;
    int buttonchannel;
    QStringList tempDataFilePathList;
    QStringList tempSettingsFilePathList;
    
    // 丢包检测相关成员
    QMap<QString, bool> cardDataReceivedMap; // 记录每张卡的数据接收状态
    QTimer* acquisitionTimeoutTimer; // 采集超时定时器
    int acquisitionTimeoutInterval = 10000; // 采集超时时间(ms)
    bool isAcquisitionRunning = false; // 标记是否正在采集过程中

    QString SettingsFilePath;
    QStringList ipList;

    void makeUi();
    void informationInject(QTableWidget* tableWidget, int rowCount, int cloumnCount);
    void retranslateUi();

    void updateCardPage();
    void previousPageClicked();
    void nextPageClicked();

    //根据机箱选择自动设置采样率
    void autosetAcquisitionSampleRate(const QString &daqCase, QComboBox *acquisitionsamplerateBox);

    //打印日志
    void printLogs(QString logs);
    void analysisBtnClicked();
    void historyBtnClicked();
    void WaitBtnClicked();
    void autoFindBtnClicked();
    void AcardSetButtonClicked();
    void AcardReSetButtonClicked();
    void clearLogs();
    void delayMsDI(int mes);
    int getSamplingRate(const QString& settingsFilePath);
    //int getSamplingRateForCard(int cardInfo);
    //int getHistoryDataSamplingRateForCard(int cardInfo);
    int judgmentSamplingRate(QString remoteIP);

    //全选
    void allChooseClicked();
    //根据卡选
    void cardChooseClicked();

    //默认参数填充
    void parameterFilling();

    //曲线设置
    void repaintGraph(PlanarGraph* planarGraph);

    // 文件名自动设置
    void acquisitionFileNameStore();

    //参数保存
    void parameterStore();

    //读取参数
    void readParameter();

    //参数转移到界面
    void parameterFill();

    //采集卡设置
    void acquisitionCardSet(int acquisitionTime,int pickUpIndex,int triggerModeIndex,int rawDataAnalysisIndex,int PretriggerTime,int PosttriggerTime,
                            int acquisitionNumber,int acquisitionNumberAutomationIndex, QString fileStorageLocation,QString acquisitionFileName,int acquisitionsamplerateIndex);
    //参数转移
    void getAlllogical();
    //void startParsing();
    void startParsingV2(QString datPathPrefix);
    QString getSaveFilePathPrefix();
    QString getSaveCSVFilePath();
    QString getHistorySaveCSVFilePath();
    QString getHistorySaveFilePathPrefix();
    //void startParsingHistory();
    void dataShowLogicDeal();
    void triggerEvent();
    void interfaceConfigurationInitialization();
    void hardwareTriggerShow();
//    void hardwareTriggerShowButton();
//    void interfaceConfigurationInitialization();
//    void softwareTriggerShowButton();
    void buttonSlot();
    //QFile *FileMake();
    //QFile *HistoryFileMake();
    //void mergeFiles();
    //void mergeHistoryFiles();
    QColor generateColor(int index);
    //void writeDataToCSV(const QString& filePath, const QVector<QVector<double>>& data);

    //更新参数
    void coefficientVectorUpdate();

    //按钮信息的结构体
    struct ButtonInfo {
        QString text; // 按钮的文本
        bool isClicked=false; // 是否被点击
        int cardInfo; //卡槽
        int channelInfo; //通道
    };
    QList<ButtonInfo> buttonInfos; // 存储所有按钮的信息
    struct AcardInfo {
        bool isChecked;
        AcardInfo() : isChecked(false) {}
    };
    QVector<AcardInfo> AcardInfos; // 存储所有卡槽的信息


    //通道逻辑层注入
    void injectMainLogicalToThis();

    void parameterTransferToSS(AcquisitionCard* acquisitionCard);

    //通道逻辑层返回
    MainLogical* returnMainLogical();

    bool loadPlugin();
    bool unloadPlugin();

    //返回卡信息
    QList<CardInformation*>* returnCardInformations();
    void settingsParameterStore();
    QList<int> channel0SettingsParameterStoreGet();
    QList<int> channel1SettingsParameterStoreGet();
    QList<int> channel2SettingsParameterStoreGet();
    QList<int> channel3SettingsParameterStoreGet();

    void initSettingsDatainject();

    IDataProcessor* dataProcessor;//
    IExporter* csvExporter;//
    ExportTask* exportTask;//

protected:
    // 重写 closeEvent 函数
    void closeEvent(QCloseEvent *event) override;
};
#endif // MAINVIEW_H
