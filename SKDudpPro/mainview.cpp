//请不要直接修改主分支！
#include "mainview.h"
#include "ui_mainview.h"
#include <QSettings>
#include <QtMath>
#include "ExporterFactory.h"

#include "QsLog.h"

MainView::MainView(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainView)
{
    ui->setupUi(this);

    makeUi();
    retranslateUi();
    parameterFilling();
    parameterTransferToSS(acquisitionCard);

    triggerEvent();

    showMaximized();

    parameterFill();
    getAlllogical();
    dataShowLogicDeal();

    //加载配置参数
    mainLogical->addcoefficient();
    coefficientVectorUpdate();


//    connect(resetBtn,&QPushButton::clicked,this,[&](){
//        printLogs("   执行复位");
//        QMessageBox::information(this,"成功信息","完成复位");
//    });
    connect(startBtn,&QPushButton::clicked,this,[&](){
        printLogs("   开始采集...");
        //autoIncreasedFlag = 1;
        if(dataInteractionLogicals->size() <= 0){
            printLogs("   无可供使用的采集卡！");
            QMessageBox::information(this, "错误信息", "无可供使用的采集卡!");
        }
    });
    connect(startBtn, &QPushButton::clicked, this,&MainView::startudpCollect);
    connect(analysisBtn,&QPushButton::clicked,this,&MainView::analysisBtnClicked);
    connect(historyFileButton,&QPushButton::clicked,this,&MainView::historyBtnClicked);
    connect(WaitBtn,&QPushButton::clicked,this,&MainView::WaitBtnClicked);
    connect(autoFindBtn,&QPushButton::clicked,this, &MainView::autoFindBtnClicked);
    connect(SaveFileButton,&QPushButton::clicked,this,&MainView::saveFileClicked);
    connect(historySaveFileButton,&QPushButton::clicked,this,&MainView::historySaveFileClicked);
    connect(AutoCalculateButton,&QPushButton::clicked,this,&MainView::autocalculated);
    connect(ClearButton,&QPushButton::clicked,this, &MainView::clearLogs);

    connect(AcardSetButton,&QPushButton::clicked,this, &MainView::AcardSetButtonClicked);
    connect(AcardReSetButton,&QPushButton::clicked,this, &MainView::AcardReSetButtonClicked);
    connect(this,&MainView::startDataShowLogic,dataShowLogical,&DataShowLogical::plotDraw);
    connect(dataShowLogical,&DataShowLogical::showOver,this,&MainView::receieveShowOver);
    connect(dataShowLogical,&DataShowLogical::letMainViewShowPlot,this,&MainView::mainViewDrawPlot);
    connect(planarGraph, &PlanarGraph::putLog, this, &MainView::printLogs);
    connect(this, &MainView::putOverLog, this, &MainView::printLogs);
    connect(this, &MainView::coefficientVectorUpdated, dataShowLogical, &DataShowLogical::handleCoefficientVector);
    connect(this, &MainView::coefficientbVectorUpdated, dataShowLogical, &DataShowLogical::handleCoefficientbVector);

    //全选按钮等
    connect(allChooseButton, &QPushButton::clicked, this, &MainView::allChooseClicked);

    interfaceConfigurationInitialization();
    connect(modeChangeBox, QOverload<int>::of(&QComboBox::currentIndexChanged), [=](int newIndex){
        switch (newIndex) {
        case(0):
            printLogs("  切换软件触发模式");
            statusIndicator->setStyleSheet(QString("background-color: red; border-radius: %1px;").arg(40));
            break;
        case(1):
            printLogs("  切换硬件触发模式");
            emit hardTriggerStart();
            printLogs("等待硬件触发");
            statusIndicator->setStyleSheet(QString("background-color: green; border-radius: %1px;").arg(40));
            break;
        }
    });

    if(loadPlugin()) {
        qDebug()<<"true";
    }



    dataProcessor = nullptr;
    csvExporter= nullptr;
    exportTask= nullptr;

    // 初始化丢包检测定时器
    acquisitionTimeoutTimer = new QTimer(this);
    connect(acquisitionTimeoutTimer, &QTimer::timeout, this, &MainView::handleAcquisitionTimeout);

    printLogs("   为确保最佳性能及充分利用Windows系统的资源，建议在首次使用前重启电脑。");
}


MainView::~MainView()
{
    unloadPlugin();
    delete ui;
}

//界面设计
void MainView::makeUi()
{
    readParameter();
    injectMainLogicalToThis();
    dataInteractionLogicals = mainLogical->returnDataInteractionLogicals();

    menubar = new QMenuBar();
    setMenuBar(menubar);

    helpMenu = new QMenu(QStringLiteral("帮助"));

    menubar->addMenu(helpMenu);

    toolBar = new QToolBar();
    this->addToolBar(toolBar);

    startBtn = new QPushButton();
    startBtn->setText("开始");
    startBtn->setIcon(QIcon(":/new/prefix1/img/0.jpg"));
    startBtn->setEnabled(false);

    analysisBtn = new QPushButton();
    analysisBtn->setText("解析");
    analysisBtn->setIcon(QIcon(":/new/prefix1/img/5.jpg"));

    autoFindBtn = new QPushButton();
    autoFindBtn->setText("自动寻找");
    autoFindBtn->setIcon(QIcon(":/new/prefix1/img/2.jpg"));
    WaitBtn = new QPushButton();
    WaitBtn->setText("等待实验");
    WaitBtn->setIcon(QIcon(":/new/prefix1/img/3.jpg"));
    SaveFileButton = new QPushButton();
    SaveFileButton->setText("当日数据导出");
    SaveFileButton->setIcon(QIcon(":/new/prefix1/img/1.jpg"));

    historyFileButton = new QPushButton();
    historyFileButton->setText("历史文件解析");
    historyFileButton->setIcon(QIcon(":/new/prefix1/img/1.jpg"));

    historySaveFileButton = new QPushButton();
    historySaveFileButton->setText("历史文件导出");
    historySaveFileButton->setIcon(QIcon(":/new/prefix1/img/1.jpg"));

    AutoCalculateButton = new QPushButton();
    AutoCalculateButton->setText("自动校正");
    AutoCalculateButton->setIcon(QIcon(":/new/prefix1/img/1.jpg"));

    toolBar->addWidget(startBtn);
    //toolBar->addWidget(resetBtn);
    toolBar->addWidget(analysisBtn);    
    toolBar->addWidget(autoFindBtn);
    toolBar->addWidget(WaitBtn);
    toolBar->addWidget(SaveFileButton);
    toolBar->addWidget(historyFileButton);
    toolBar->addWidget(historySaveFileButton);
    toolBar->addWidget(AutoCalculateButton);
    //toolBar->addWidget(readriggerButton);

    centralwidget = new QWidget(this);
    verticalLayout_6 = new QVBoxLayout(centralwidget);
    horizontalLayout_18 = new QHBoxLayout();
    horizontalLayout_18->setSpacing(12);

    verticalLayout_4 = new QVBoxLayout();
    verticalLayout_4->setSpacing(12);

    verticalLayout = new QVBoxLayout();
    horizontalLayout_3 = new QHBoxLayout();
    pickUpLabel = new QLabel("挑点选择");

    horizontalLayout_3->addWidget(pickUpLabel);
    horizontalSpacer_3 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);
    horizontalLayout_3->addItem(horizontalSpacer_3);
    verticalLayout->addLayout(horizontalLayout_3);

    horizontalLayout_4 = new QHBoxLayout();
    horizontalLayout_4->setSpacing(0);
    pickUpIcon = new QToolButton(centralwidget);
    horizontalLayout_4->addWidget(pickUpIcon);

    pickUpBox = new QComboBox(centralwidget);
    horizontalLayout_4->addWidget(pickUpBox);

    verticalLayout->addLayout(horizontalLayout_4);
    verticalLayout_4->addLayout(verticalLayout);

    horizontalLayout_14 = new QHBoxLayout();

    verticalLayout_2 = new QVBoxLayout();
    horizontalLayout_6 = new QHBoxLayout();
    earlyTriggerLabel = new QLabel(centralwidget);

    horizontalLayout_6->addWidget(earlyTriggerLabel);
    horizontalSpacer_4 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);
    horizontalLayout_6->addItem(horizontalSpacer_4);
    verticalLayout_2->addLayout(horizontalLayout_6);

    horizontalLayout_7 = new QHBoxLayout();
    horizontalLayout_7->setSpacing(0);
    earlyTriggerIcon = new QToolButton(centralwidget);
    horizontalLayout_7->addWidget(earlyTriggerIcon);

    PretriggerSpinBox = new QSpinBox(centralwidget);
    PretriggerSpinBox->setMaximum(65535);
    PretriggerSpinBox->setValue(0);

    horizontalLayout_7->addWidget(PretriggerSpinBox);
    verticalLayout_2->addLayout(horizontalLayout_7);
    horizontalLayout_14->addLayout(verticalLayout_2);

    verticalLayout_3 = new QVBoxLayout();
    horizontalLayout_8 = new QHBoxLayout();
    afterTriggerLabel = new QLabel(centralwidget);

    horizontalLayout_8->addWidget(afterTriggerLabel);
    horizontalSpacer_5 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);
    horizontalLayout_8->addItem(horizontalSpacer_5);
    verticalLayout_3->addLayout(horizontalLayout_8);

    horizontalLayout_9 = new QHBoxLayout();
    horizontalLayout_9->setSpacing(0);

    afterTriggerIcon = new QToolButton(centralwidget);
    horizontalLayout_9->addWidget(afterTriggerIcon);

    PosttriggerSpinBox = new QSpinBox(centralwidget);
    PosttriggerSpinBox->setMaximum(65535);
    PosttriggerSpinBox->setValue(0);

    horizontalLayout_9->addWidget(PosttriggerSpinBox);
    verticalLayout_3->addLayout(horizontalLayout_9);
    horizontalLayout_14->addLayout(verticalLayout_3);
    verticalLayout_4->addLayout(horizontalLayout_14);

    frame_5 = new QFrame(centralwidget);
    frame_5->setStyleSheet(QString::fromUtf8("background-color: rgb(218, 218, 218);"));
    frame_5->setFrameShape(QFrame::StyledPanel);
    frame_5->setFrameShadow(QFrame::Raised);
    horizontalLayout_11 = new QHBoxLayout(frame_5);

    inputChannelLabel = new QLabel(frame_5);
    horizontalLayout_11->addWidget(inputChannelLabel);

    horizontalSpacer_6 = new QSpacerItem(126, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);
    horizontalLayout_11->addItem(horizontalSpacer_6);

    verticalLayout_4->addWidget(frame_5);

    tabWidget = new QTabWidget(centralwidget);
    treeWidget = new QTreeWidget(centralwidget);
    for(int i=0;i<18;i++)
    {
            QTreeWidgetItem *item = new QTreeWidgetItem(treeWidget);
            new QTreeWidgetItem(item);
            new QTreeWidgetItem(item);
            new QTreeWidgetItem(item);
            new QTreeWidgetItem(item);
    }
    tabWidget->addTab(treeWidget,"channel");

    inputSettingLayout = new QVBoxLayout();

    QWidget* inputWidget = new QWidget();

    acquisitionsamplerateLayout = new QHBoxLayout();
    acquisitionsamplerate = new QLabel("采样率:");
    acquisitionsamplerateBox = new QComboBox();
    acquisitionsamplerateLayout->addWidget(acquisitionsamplerate);
    acquisitionsamplerateLayout->addWidget(acquisitionsamplerateBox);

    acquisitionTimeLayout = new QHBoxLayout();
    acquisitionTime = new QLabel("采集时间(us):");
    acquisitionTimeBox = new QSpinBox();
    acquisitionTimeLayout->addWidget(acquisitionTime);
    acquisitionTimeLayout->setContentsMargins(0,0,0,0);
    acquisitionTimeLayout->addWidget(acquisitionTimeBox);

    acquisitionNumberLayout = new QHBoxLayout();
    acquisitionNumberLable = new QLabel("采集记录号:");
    acquisitionNumberSpinBox = new QSpinBox();
    acquisitionNumberLayout->addWidget(acquisitionNumberLable);
    acquisitionNumberLayout->addWidget(acquisitionNumberSpinBox);

    acquisitionNumberAutomationLayout = new QHBoxLayout();
    acquisitionNumberAutomationLabel = new QLabel("采集记录号自增长:");//好像这个没用上
    acquisitionNumberAutomationComboBox = new QComboBox;
    acquisitionNumberAutomationLayout->addWidget(acquisitionNumberAutomationLabel);
    acquisitionNumberAutomationLayout->addWidget(acquisitionNumberAutomationComboBox);

    fileStorageLocationLayout = new QHBoxLayout();
    fileStorageLocationLabel = new QLabel("文件存储路径:");
    fileStirageLocationEdit = new QLineEdit();
    fileStirageLocationEdit->setFixedWidth(380);
    fileStorageLocationLayout->addWidget(fileStorageLocationLabel);
    fileStorageLocationLayout->addWidget(fileStirageLocationEdit);

    fileHistoryLocationLayout = new QHBoxLayout();
    fileHistoryLocationLabel = new QLabel("历史文件路径:");
    fileHistoryLocationEdit = new QLineEdit();
    fileHistoryLocationEdit->setFixedWidth(380);
    fileHistoryLocationLayout->addWidget(fileHistoryLocationLabel);
    fileHistoryLocationLayout->addWidget(fileHistoryLocationEdit);


    acquisitionFileNameLayout = new QHBoxLayout();
    acquisitionFileNameLabel = new QLabel("存储文件名:");
    acquisitionFileNameEdit = new QLineEdit();
    acquisitionFileNameEdit->setFixedWidth(380);
    acquisitionFileNameLayout->addWidget(acquisitionFileNameLabel);
    acquisitionFileNameLayout->addWidget(acquisitionFileNameEdit);

    // 新增自动编号复选框
    autoFileNamedCheckBox = new QCheckBox("自动编号");
    acquisitionFileNameLayout->addWidget(autoFileNamedCheckBox);

    QSettings collectSettings("parameterSKDFDA", "value");
    bool isAutoFileNameChecked = collectSettings.value("autoFileNamedChecked", false).toBool();
    autoFileNamedCheckBox->setChecked(isAutoFileNameChecked);

    cardNumberLayout = new QHBoxLayout();
    cardNumberLabel = new QLabel("板卡选择:");
    cardNumberBox = new QComboBox();
    AcardSetButton = new QPushButton("单独触发设置");
    AcardReSetButton = new QPushButton("同步触发");
    cardNumberLayout->addWidget(cardNumberLabel);
    cardNumberLayout->addWidget(cardNumberBox);
    cardNumberLayout->addWidget(AcardSetButton);
    cardNumberLayout->addWidget(AcardReSetButton);

    daqCaseLayout = new QHBoxLayout();
    daqCaseLabel = new QLabel("机箱选择:");
    daqCaseBox = new QComboBox();
    daqCaseLayout->addWidget(daqCaseLabel);
    daqCaseLayout->addWidget(daqCaseBox);

    collectModeLayout = new QHBoxLayout();
    collectModeLabel = new QLabel("采集接口:");
    collectModeBox = new QComboBox();
    collectModeLayout->addWidget(collectModeLabel);
    collectModeLayout->addWidget(collectModeBox);

    modeChangeLayout = new QHBoxLayout();
    modeChangeLabel = new QLabel("模式转换:");
    modeChangeBox = new QComboBox();
    modeChangeLayout->addWidget(modeChangeLabel);
    modeChangeLayout->addWidget(modeChangeBox);

    rawDataAnalysisLayout = new QHBoxLayout();
    rawDataAnalysisLabel = new QLabel("原始数据解析:");
    rawDataAnalysisBox = new QComboBox();
    rawDataAnalysisLayout->addWidget(rawDataAnalysisLabel);
    rawDataAnalysisLayout->addWidget(rawDataAnalysisBox);


    baseline1Layout = new QHBoxLayout();
    baseline1Label = new QLabel("基线1:");
    baseline1SpinBox = new QSpinBox();
    baseline1Layout->addWidget(baseline1Label);
    baseline1Layout->addWidget(baseline1SpinBox);

    baseline2Layout = new QHBoxLayout();
    baseline2Label = new QLabel("基线2:");
    baseline2SpinBox = new QSpinBox();
    baseline2Layout->addWidget(baseline2Label);
    baseline2Layout->addWidget(baseline2SpinBox);

    baseline3Layout = new QHBoxLayout();
    baseline3Label = new QLabel("基线3:");
    baseline3SpinBox = new QSpinBox();
    baseline3Layout->addWidget(baseline3Label);
    baseline3Layout->addWidget(baseline3SpinBox);

    baseline4Layout = new QHBoxLayout();
    baseline4Label = new QLabel("基线4:");
    baseline4SpinBox = new QSpinBox();
    baseline4Layout->addWidget(baseline4Label);
    baseline4Layout->addWidget(baseline4SpinBox);

    localLayout = new QHBoxLayout();
    localLabel = new QLabel("本地IP及端口:");
    localIpComboBox = new QComboBox();
    localIpComboBox->setMinimumWidth(200);
    //localIpComboBox->setMaximumWidth(2);
    udpHelper->localIPFind(localIpComboBox);

    localPortLineEdit = new QLineEdit();
    localPortLineEdit->setFixedWidth(100); // 设置端口输入框固定宽度
    localPortLineEdit->setText("8080");

    connectButton = new QPushButton("连接");
    connectButton->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
    connect(connectButton, &QPushButton::clicked, this, &MainView::connectToHost);

    testButton = new QPushButton("测试");
    testButton->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
    connect(testButton, &QPushButton::clicked, this, &MainView::connectTest);

    disconnectButton = new QPushButton("断开");
    disconnectButton->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
    disconnectButton->setEnabled(false);
    disconnectButton->setFixedWidth(0);
    connect(disconnectButton, &QPushButton::clicked, this, &MainView::disconnectFromHost);

    localLayout->addWidget(localLabel);
    localLayout->addWidget(localIpComboBox);
    localLayout->addWidget(localPortLineEdit);
    localLayout->addWidget(connectButton);
    localLayout->addWidget(testButton);
    localLayout->addWidget(disconnectButton);

    inputSettingLayout->addLayout(localLayout);
    inputSettingLayout->addLayout(acquisitionTimeLayout);
    inputSettingLayout->addLayout(acquisitionNumberLayout);
    inputSettingLayout->addLayout(acquisitionNumberAutomationLayout);
    inputSettingLayout->addLayout(fileStorageLocationLayout);
    inputSettingLayout->addLayout(fileHistoryLocationLayout);
    inputSettingLayout->addLayout(acquisitionFileNameLayout);
    inputSettingLayout->addLayout(cardNumberLayout);
    inputSettingLayout->addLayout(daqCaseLayout);
    inputSettingLayout->addLayout(acquisitionsamplerateLayout);
    inputSettingLayout->addLayout(collectModeLayout);
    inputSettingLayout->addLayout(modeChangeLayout);
    inputSettingLayout->addLayout(rawDataAnalysisLayout);
//    inputSettingLayout->addLayout(baseline1Layout);
//    inputSettingLayout->addLayout(baseline2Layout);
//    inputSettingLayout->addLayout(baseline3Layout);
//    inputSettingLayout->addLayout(baseline4Layout);


    inputWidget->setLayout(inputSettingLayout);
    tabWidget->addTab(inputWidget,"输入设置");
    tabWidget->setCurrentIndex(tabWidget->indexOf(inputWidget));
    verticalLayout_4->addWidget(tabWidget);

    logsprintLabel = new QLabel();
    logsPrint = new QPlainTextEdit();
    statusIndicator = new QLabel();
    statusIndicator->setFixedSize(80, 80);
    int indicatorSize = 80;
    int borderRadius = indicatorSize / 2;
    statusIndicator->setStyleSheet(QString("background-color: red; border-radius: %1px;").arg(borderRadius));
    ClearButton = new QPushButton("清除日志");
    QHBoxLayout *hLayout = new QHBoxLayout();
    hLayout->addWidget(statusIndicator, 0, Qt::AlignLeft);
    hLayout->addWidget(ClearButton, 0, Qt::AlignRight);
    hLayout->setContentsMargins(0, 0, 0, 0);
    QWidget *container = new QWidget();
    container->setLayout(hLayout);
    ClearButton->setFixedWidth(100);
    logsPrint->setReadOnly(true);
    verticalLayout_4->addWidget(logsprintLabel);
    verticalLayout_4->addWidget(logsPrint);
    verticalLayout_4->addWidget(container);
    horizontalLayout_18->addLayout(verticalLayout_4);

    verticalLayout_5 = new QVBoxLayout();
    verticalLayout_5->setSpacing(12);
    horizontalLayout_17 = new QHBoxLayout();

    scrollArea = new QScrollArea(centralwidget);
    widget = new QWidget();
    scrollArea->setStyleSheet(QString::fromUtf8("background-color: rgb(0, 0, 0);"));
    scrollArea->setFrameShape(QFrame::StyledPanel);
    scrollArea->setFrameShadow(QFrame::Raised);
    scrollArea->setSizePolicy(QSizePolicy::Preferred,QSizePolicy::Preferred);
    horizontalLayout_17->addWidget(scrollArea);
    channelShowLayout = new QHBoxLayout(centralwidget);

    // 添加全选按钮
    QVBoxLayout* verticalLayout = new QVBoxLayout();
    QHBoxLayout* horizontalLayout = new QHBoxLayout();
    QString btnText = QString("全选");
    allChooseButton = new QPushButton(btnText);
    allChooseButton->setStyleSheet("background-color: rgb(255, 255, 0);color: rgb(255, 0, 0);");
    allChooseButton->setCheckable(true);
    allChooseButton->setChecked(false);
    allChooseButton->setMaximumWidth(60);
    channelShowLayout->addWidget(allChooseButton);

    // 分页模式代码1
    // QVBoxLayout* pageLayout = new QVBoxLayout();
    // prevPageButton = new QPushButton("上一页");
    // nextPageButton = new QPushButton("下一页");
    // prevPageButton->setStyleSheet("background-color: rgb(255, 255, 0);color: rgb(255, 0, 0);");
    // nextPageButton->setStyleSheet("background-color: rgb(255, 255, 0);color: rgb(255, 0, 0);");
    // nextPageButton->setMaximumWidth(60);
    // prevPageButton->setMaximumWidth(60);
    // pageLayout->addWidget(prevPageButton);
    // pageLayout->addWidget(nextPageButton);
    // channelShowLayout->addLayout(pageLayout);

    //使用栅格布局
    QWidget* gridWidget = new QWidget();
    QGridLayout* gridLayout = new QGridLayout(gridWidget);
    channelShowLayout->addWidget(gridWidget);

    PCIESortedMap = mainLogical->getPCIESortedMap();

    if(dataInteractionLogicals->size()>0){
        // 分页模式代码2
        // currentPage = 0;
        // totalCards = dataInteractionLogicals->size();
        // cardsPerPage = 20;
        // int totalPages = (totalCards + cardsPerPage - 1) / cardsPerPage;
        // QString msg = QString(" 当前板卡总数%1 共%2页显示").arg(totalCards).arg(totalPages);
        // printLogs(msg);

        // connect(prevPageButton, &QPushButton::clicked, this, &MainView::previousPageClicked);
        // connect(nextPageButton, &QPushButton::clicked, this, &MainView::nextPageClicked);

        // updateCardPage();


        int column = 0;
        for (int key : PCIESortedMap.keys()) {
            // if (column >= 15) {
            //     qDebug() << "Too many cards, only displaying the first" ;
            //     break;
            // }

            QString btnText = QString("Card%1").arg(key);
            QPushButton* cardBtn = new QPushButton(btnText);
            cardBtn->setStyleSheet("background-color: rgb(255, 255, 0);color: rgb(255, 0, 0);");
            cardBtn->setCheckable(true);
            cardBtn->setChecked(false);
            cardButtons.append(cardBtn);
            gridLayout->addWidget(cardBtn, 0, column);
            connect(cardBtn, &QPushButton::clicked, this, &MainView::cardChooseClicked);

            QHBoxLayout* channelLayout = new QHBoxLayout();
            QList<QPushButton*> currentCardChannels;
            for (int j = 0; j < 4; j++) {
                QString btnText = QString("CH%1-%2").arg(key).arg(j);
                QPushButton* channelBtn = new QPushButton(btnText);
                channelBtn->setStyleSheet("background-color: rgb(255, 255, 0);color: rgb(255, 0, 0);");
                channelBtn->setCheckable(true);
                channelBtn->setChecked(false);
                currentCardChannels.append(channelBtn);
                channelButtons.append(channelBtn);
                channelLayout->addWidget(channelBtn);
            }
            gridLayout->addLayout(channelLayout, 1, column);
            cardToChannels[cardBtn] = currentCardChannels;
            column++;
        }
    }
    else{
        for(int k = 0; k < 2; k++) {
            QString btnText = QString("Card%1").arg(k);
            QPushButton* cardBtn = new QPushButton(btnText);
            cardBtn->setStyleSheet("background-color: rgb(255, 255, 0);color: rgb(255, 0, 0);");
            cardBtn->setCheckable(true);
            cardBtn->setChecked(false);
            cardButtons.append(cardBtn);
            gridLayout->addWidget(cardBtn, 0, k);
            connect(cardBtn, &QPushButton::clicked, this, &MainView::cardChooseClicked);

            QHBoxLayout* channelLayout = new QHBoxLayout();
            QList<QPushButton*> currentCardChannels;
            for (int j = 0; j < 4; j++) {
                QPushButton* channelBtn = new QPushButton(QString("CH%1-%2").arg(k).arg(j));
                channelBtn->setStyleSheet("background-color: rgb(255, 255, 0);color: rgb(255, 0, 0);");
                channelBtn->setCheckable(true);
                channelBtn->setChecked(false);
                currentCardChannels.append(channelBtn);
                channelButtons.append(channelBtn);
                channelLayout->addWidget(channelBtn);
            }
            gridLayout->addLayout(channelLayout, 1, k);
            cardToChannels[cardBtn] = currentCardChannels;
        }
    }

    widget->setLayout(channelShowLayout);
    scrollArea->setWidget(widget);
    verticalLayout_5->addLayout(horizontalLayout_17);

    //修改为按键状态反转，就调用其槽函数
    for(int i = 0; i < channelButtons.count(); i++){
        channelBtn[i] = channelButtons.at(i);
        connect(channelBtn[i],&QPushButton::toggled,this,[=](){
            buttonSlot();
        });

    }


    frame_9 = new QFrame(centralwidget);
    frame_9->setStyleSheet(QString::fromUtf8("background-color: rgb(229, 229, 229);"));
    frame_9->setFrameShape(QFrame::StyledPanel);
    frame_9->setFrameShadow(QFrame::Raised);
    verticalLayout_5->addWidget(frame_9);

    planarGraph = new PlanarGraph("curve",centralwidget);
    QList<QColor> plotColorList;
    plotColorList<<Qt::white<<Qt::yellow<<Qt::blue<<Qt::red;
    planarGraph->plot_2D_Init(Qt::black,4,plotColorList);

    verticalLayout_5->addWidget(planarGraph);
    repaintGraph(planarGraph);

    verticalLayout_5->setStretch(0, 12);
    verticalLayout_5->setStretch(1, 1);
    verticalLayout_5->setStretch(2, 100);

    horizontalLayout_18->addLayout(verticalLayout_5);

    horizontalLayout_18->setStretch(0, 10);
    horizontalLayout_18->setStretch(1, 25);

    verticalLayout_6->addLayout(horizontalLayout_18);

    setCentralWidget(centralwidget);
}

// 分页显示 刷新用
void MainView::updateCardPage() {
    // QWidget* oldgridWidget = channelShowLayout->itemAt(2)->widget();
    // if (oldgridWidget) {
    //     channelShowLayout->removeWidget(oldgridWidget);
    //     delete oldgridWidget;
    // }

    // QGridLayout* gridLayout = new QGridLayout();
    // QWidget* gridWidget = new QWidget();
    // gridWidget->setLayout(gridLayout);
    // channelShowLayout->addWidget(gridWidget, 0, Qt::AlignLeft);

    // int column = 0;
    // cardButtons.clear();
    // channelButtons.clear();
    // buttonInfos.clear();

    // int startIndex = currentPage * cardsPerPage;
    // int endIndex = qMin((currentPage + 1) * cardsPerPage, totalCards);

    // for (int i = startIndex; i < endIndex; ++i) {
    //     int key = PCIESortedMap.keys().at(i);

    //     // Create card button
    //     QString btnText = QString("Card%1").arg(key);
    //     QPushButton* cardBtn = new QPushButton(btnText);
    //     cardBtn->setStyleSheet("background-color: rgb(255, 255, 0);color: rgb(255, 0, 0);");
    //     cardBtn->setCheckable(true);
    //     cardBtn->setChecked(false);
    //     cardButtons.append(cardBtn);
    //     gridLayout->addWidget(cardBtn, 0, column);
    //     connect(cardBtn, &QPushButton::clicked, this, &MainView::cardChooseClicked);

    //     // Create channel buttons
    //     QHBoxLayout* channelLayout = new QHBoxLayout();
    //     QList<QPushButton*> currentCardChannels;
    //     for (int j = 0; j < 4; j++) {
    //         QString channelText = QString("CH%1-%2").arg(key).arg(j);
    //         QPushButton* channelBtn = new QPushButton(channelText);
    //         channelBtn->setStyleSheet("background-color: rgb(255, 255, 0);color: rgb(255, 0, 0);");
    //         channelBtn->setCheckable(true);
    //         channelBtn->setChecked(false);
    //         currentCardChannels.append(channelBtn);
    //         channelButtons.append(channelBtn);
    //         channelLayout->addWidget(channelBtn);
    //     }
    //     gridLayout->addLayout(channelLayout, 1, column);

    //     cardToChannels[cardBtn] = currentCardChannels;
    //     column++;
    // }


    // //修改为按键状态反转，就调用其槽函数
    // for(int i = 0; i < channelButtons.count(); i++){
    //     channelBtn[i] = channelButtons.at(i);
    //     disconnect(channelBtn[i], &QPushButton::toggled, this, nullptr);
    //     connect(channelBtn[i],&QPushButton::toggled,this,[=](){
    //         buttonSlot();
    //     });
    // }
}

void MainView::previousPageClicked() {
    if (currentPage > 0) {
        currentPage--;
        updateCardPage();
        QString msg = QString(" 当前选卡 页数 %1").arg(currentPage);
        printLogs(msg);


    }
    else{
        printLogs(" 当前选卡 第一页");
    }
}

void MainView::nextPageClicked() {
    int maxPage = (totalCards + cardsPerPage - 1) / cardsPerPage - 1;
    if (currentPage < maxPage) {
        currentPage++;
        updateCardPage();
        QString msg = QString(" 当前选卡 页数 %1").arg(currentPage);
        printLogs(msg);
    }
    else{
        printLogs(" 当前选卡 最后一页");
    }
}

//界面参数
void MainView::retranslateUi()
{
    this->setWindowTitle("YQ-DAQ-NET V250703 混采-重构解析版");

    pickUpLabel->setText("挑点选择");
    pickUpIcon->setIcon(QIcon(":/new/prefix1/img/lock.png"));
    earlyTriggerLabel->setText("预触发(us)");
    earlyTriggerIcon->setIcon(QIcon(":/new/prefix1/img/unlock.png"));
    PretriggerSpinBox->setValue(0);

    afterTriggerLabel->setText("后触发(us)");
    afterTriggerIcon->setIcon(QIcon(":/new/prefix1/img/unlock.png"));
    PosttriggerSpinBox->setValue(0);

    inputChannelLabel->setText("输入通道");
    acquisitionTimeBox->setMaximum(1000000);
    acquisitionNumberSpinBox->setMaximum(999999);
    baseline1SpinBox->setMaximum(100000);
    baseline2SpinBox->setMaximum(100000);
    baseline3SpinBox->setMaximum(100000);
    baseline4SpinBox->setMaximum(100000);
    //channelShowBox->setMaximum(100);

    QTreeWidgetItem *qtreewidgetitem = treeWidget->headerItem();
    qtreewidgetitem->setText(6, "偏置");
    qtreewidgetitem->setText(5, "输入范围");
    qtreewidgetitem->setText(4, "带宽");
    qtreewidgetitem->setText(3, "AC/DC");
    qtreewidgetitem->setText(2, "条款");
    qtreewidgetitem->setText(1, "路径");
    qtreewidgetitem->setText(0, "通道");

    const bool sortingEnabled = treeWidget->isSortingEnabled();
    treeWidget->setSortingEnabled(false);
    for(int i=0;i<18;i++)
    {
        QTreeWidgetItem * item = treeWidget->topLevelItem(i);

        item->setText(0,QString("M4i.4481-x8 SN168%1").arg(i+50));
        for(int j=0;j<4;j++){
            QTreeWidgetItem *child=item->child(j);
            child->setText(0,QString("Ai-CH%1").arg(i*4+j,2,10,QLatin1Char('0')));
            child->setText(1,QString("Buf."));
            child->setText(2,QString("50 Ω"));
            child->setText(3,QString("DC"));
            child->setText(4,QString("Full"));
            child->setText(5,QString("±10.00 V"));
            child->setText(6,QString("0 mV"));
        }
    }

    logsprintLabel->setText("日志打印");

}

//打印日志
void MainView::printLogs(QString logs)
{
    QDateTime currentDateTime = QDateTime::currentDateTime();
    QString dateTimeString = currentDateTime.toString("yyyy-MM-dd hh:mm:ss");
    logsPrint->appendPlainText(dateTimeString + logs);
}

QString MainView::getMonthDayPath() const {
    QDate currentDate = QDate::currentDate();
    int year = currentDate.year();
    int month = currentDate.month();
    int day = currentDate.day();

    QString basePath = acquisitionCard->getFileStorageLocation();
    QString yearPath = basePath + "/" + QString::number(year);
    QString monthdayPath = yearPath + "/" + QString::number(month).rightJustified(2, '0') + QString::number(day).rightJustified(2, '0');

    return monthdayPath;
}

//另存文件 原来已经注释掉的
// void MainView::saveFileClicked(){
//     int acquisitionNumber = acquisitionCard->getAcquisitionNumber();
//     int ChannelNumber=acquisitionCard->getSelectChannelNumber();
//     int PretriggerTime = this->acquisitionCard->getPretriggerTime();
//     int PosttriggerTime = this->acquisitionCard->getPosttriggerTime();
//     int sampingTime = acquisitionTimeBox->value();
//     QVector<QVector<double>> channelYData;
//     for ( int i = 0; i < ChannelNumber; i++) {
//         QVector<double>  tempChannelYData ;
//         channelYData.append(tempChannelYData);
//     }
//     QDate currentDate = QDate::currentDate();
//     int year = currentDate.year();
//     int month = currentDate.month();
//     int day = currentDate.day();

//     QString basePath = acquisitionCard->getFileStorageLocation();
//     QString yearPath = basePath + "/" + QString::number(year);
//     QString monthdayPath = yearPath + "/" + QString::number(month).rightJustified(2, '0') + QString::number(day).rightJustified(2, '0');
//    // QString dayPath = monthPath + "/" + QString::number(day);
//     QString acquisitionNumberPath = monthdayPath + "/" + QString::number(acquisitionNumber-1);
//     QString tempFileName = QString::number(acquisitionNumber-1);
//     QString tempPath0 = acquisitionNumberPath + "/" + tempFileName ;
//     QString datFilePath = tempPath0 +".DAT";

//     int tempCardNumber = -1;
//     for(int i = 0;i<Parameter::ACQUISITIONCARDNUMBER;i++){
//         int CardNumber=buttonInfos[i].cardInfo;
//         tempCardNumber = CardNumber;
//         break;
//     }
//     QString finalFileName = "Data";
//     QString tempsettingsPath = monthdayPath + "/" + QString::number(acquisitionNumber-1)
//             + "/" + QString::number(tempCardNumber) + "_"
//             + finalFileName;
//     QString destinationSettingsFile = tempsettingsPath +".INI";
//     QSettings settings(destinationSettingsFile, QSettings::IniFormat);

//     int samplingRate = settings.value("samplingRate").toInt();
//     int sampRate = (samplingRate / 1000 / 1000);

//     QFile tempFile(datFilePath);
//     char tempBuffer[2];
//     int time = 0;
//     if (PretriggerTime != 0 && PosttriggerTime != 0) {
//         time = 1;
//     } else if (PretriggerTime != 0) {
//         time = PretriggerTime + sampingTime ;
//     } else if (PosttriggerTime != 0) {
//         time =sampingTime-PosttriggerTime;
//     } else {
//         time = sampingTime;
//     }
//     long singleChannelDataNumber = sampRate * time;

//     if(tempFile.open(QIODevice::ReadOnly)){
//         for (int i = 0; i < ChannelNumber; i++){
//             double scalingFactor = 0.0002892135;


//             for ( int j = 0; j < singleChannelDataNumber; j++){
//                 tempFile.read(tempBuffer, sizeof(tempBuffer));
//                 qint16 val = (qint16) ((tempBuffer[1] & 0xFF) << 8) | (tempBuffer[0] & 0xFF);
//                 int decimalVal = static_cast<int>(val);
//                 double result = decimalVal *scalingFactor;

//                 channelYData[i].append(static_cast<double>(result));
//             }
//         }
//     }
//     tempFile.close();
//     QString finalFileName0 = "CSVFile";
//     QString AcquisitionFileName = acquisitionCard->getAcquisitionFileName();
//     QString CSVFilePath = monthdayPath + "/" +finalFileName0;
//     QString CSVFileName = CSVFilePath + "/" + AcquisitionFileName ;
//     if (!QDir().exists(CSVFilePath)) {
//         QDir().mkpath(CSVFilePath);
//     }
//     QString datFilePathfinal = CSVFileName +".CSV";
//     writeDataToCSV(datFilePathfinal, channelYData);


// }

//另存文件
// void MainView::saveFileCommon(int acquisitionNumber, int channelNumber, int PretriggerTime, int PosttriggerTime, int sampingTime, const QString& basePath)
// {
//     QString fileName = acquisitionFileNameEdit->text();
//     if (fileName.isEmpty()) {
//         QMessageBox::warning(this, "警告", "请输入存储文件名");
//         return;
//     }


//     QVector<QVector<double>> channelYData;
//     for ( int i = 0; i < channelNumber; i++) {
//         QVector<double>  tempChannelYData ;
//         channelYData.append(tempChannelYData);
//     }

//     QString acquisitionNumberPath = basePath + "/" + QString::number(acquisitionNumber-1);
//     QString tempFileName = QString::number(acquisitionNumber-1);
//     QString tempPath0 = acquisitionNumberPath + "/" + tempFileName;
//     QString datFilePath = tempPath0 + ".DAT";

//     int tempCardNumber = -1;
//     for(int i = 0;i<Parameter::ACQUISITIONCARDNUMBER;i++){
//         int CardNumber=buttonInfos[i].cardInfo;
//         tempCardNumber = CardNumber;
//         break;
//     }

//     QString finalFileName = "Data";
//     QString tempsettingsPath = basePath + "/" + tempFileName
//                                + "/" + QString::number(tempCardNumber) + "_"
//                                + finalFileName;
//     QString destinationSettingsFile = tempsettingsPath + ".INI";
//     QSettings settings(QDir::cleanPath(destinationSettingsFile), QSettings::IniFormat);

//     int samplingRate = settings.value("samplingRate").toInt();
//     int sampRate = (samplingRate / 1000 / 1000);

//     int time = sampingTime;
//     if (PretriggerTime != 0 && PosttriggerTime != 0) {
//         time = 1;
//     } else if (PretriggerTime != 0) {
//         time += PretriggerTime;
//     } else if (PosttriggerTime != 0) {
//         time -= PosttriggerTime;
//     }
//     long singleChannelDataNumber = sampRate * time;

//     //数据加载器 内含数据校准
//     boolean linearFixmode = true; // 配置校准开关
//     DataLoader loader(linearFixmode, datFilePath, tempCardNumber, channelNumber, m_channel_coefficients, m_channel_coefficientsb, singleChannelDataNumber);


//     //CSV路径
//     QString finalFileName0 = "CSVFile";
//     QString AcquisitionFileName = acquisitionCard->getAcquisitionFileName();
//     QString CSVFilePath = basePath + "/" + finalFileName0;
//     QString CSVFileName = CSVFilePath + "/" + AcquisitionFileName;
//     if (!QDir().exists(CSVFilePath)) {
//         QDir().mkpath(CSVFilePath);
//     }
//     QString datFilePathfinal = CSVFileName + ".CSV";


//     if (loader.loadData()) {
//         const auto& channelYData = loader.getData();
//         writeDataToCSV(datFilePathfinal, channelYData);
//     } else {
//         QMessageBox::information(nullptr, "提示", "数据保存失败: " + datFilePathfinal);
//     }
// }

/**
 * @brief 当日采集数据导出
 */
void MainView::saveFileClicked() {
    // int acquisitionNumber = acquisitionCard->getAcquisitionNumber();
    // int channelNumber = acquisitionCard->getSelectChannelNumber();
    // int PretriggerTime = this->acquisitionCard->getPretriggerTime();
    // int PosttriggerTime = this->acquisitionCard->getPosttriggerTime();
    // int sampingTime = acquisitionTimeBox->value();
    // QString monthdayPath = getMonthDayPath();
    // saveFileCommon(acquisitionNumber, channelNumber, PretriggerTime, PosttriggerTime, sampingTime, monthdayPath);

    QString fileName = acquisitionFileNameEdit->text();
    if (fileName.isEmpty()) {
        QMessageBox::warning(this, "警告", "请输入存储文件名");
        return;
    }


    QString todayPrefixPath = getSaveFilePathPrefix();
    QString csvPrefixPath = getSaveCSVFilePath();            //  导出的csv文件夹路径
    int acquisitionNumber = acquisitionNumberSpinBox->value()-1;
    QString exportPath = QString("%1/%2.csv").arg(csvPrefixPath).arg(fileName);
    exportCommon(todayPrefixPath, acquisitionNumber, exportPath);
}


/**
 * @brief 历史采集数据导出
 */
void MainView::historySaveFileClicked() {
    // int acquisitionNumber = acquisitionCard->getAcquisitionNumber();
    // int channelNumber = acquisitionCard->getSelectChannelNumber();
    // int PretriggerTime = this->acquisitionCard->getPretriggerTime();
    // int PosttriggerTime = this->acquisitionCard->getPosttriggerTime();
    // int sampingTime = acquisitionTimeBox->value();
    // QString basePath = fileHistoryLocationEdit->text();
    // saveFileCommon(acquisitionNumber, channelNumber, PretriggerTime, PosttriggerTime, sampingTime, basePath);

    QString fileName = acquisitionFileNameEdit->text();
    if (fileName.isEmpty()) {
        QMessageBox::warning(this, "警告", "请输入存储文件名");
        return;
    }

    QString historyPrefixPath = getHistorySaveFilePathPrefix();     //  历史数据文件路径
    QString csvPrefixPath = getHistorySaveCSVFilePath();            //  导出的csv文件夹路径
    int acquisitionNumber = acquisitionNumberSpinBox->value();
    QString exportPath = QString("%1/%2.csv").arg(csvPrefixPath).arg(fileName);

    exportCommon(historyPrefixPath, acquisitionNumber, exportPath);
}


void MainView::onExportFinished(const QString &exportPath) {
    // 导出完成后的处理
    // qDebug() << "Export finished!";
    // emit putOverLog(" 导出完成 保存在：" + exportPath);

    // 创建一个消息框询问用户是否要打开保存目录
    QMessageBox msgBox;
    msgBox.setText("导出已完成");
    msgBox.setInformativeText("是否要打开保存目录?");
    msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
    msgBox.setDefaultButton(QMessageBox::Yes);

    int ret = msgBox.exec();

    // 检查用户的响应
    if (ret == QMessageBox::Yes) {
        // 用户选择了"是", 打开保存目录
        QFileInfo fileInfo(exportPath);
        QString dirPath = fileInfo.absolutePath();
        QDesktopServices::openUrl(QUrl::fromLocalFile(dirPath));
    }
}

void MainView::exportCommon(QString prefixPath, int acquisitionNumber, QString exportPath)
{
    QString logMessage = QString(" 准备导出数据...");
    printLogs(logMessage);

    //加载配置参数
    mainLogical->addcoefficient();
    coefficientVectorUpdate();

    // 设置目标
    QMap<int, QString> dataFilePaths;
    QMap<int, QString> iniFilePaths;
    QMap<int, QVector<ChannelModel>> channels;

    QString allButtonTexts;
    for(int i = 0; i < buttonInfos.size(); i++) {
        qDebug() << "按钮文本：" << buttonInfos[i].text;
        qDebug() << "card" << buttonInfos[i].cardInfo;
        qDebug() << "channel" << buttonInfos[i].channelInfo;
        allButtonTexts += buttonInfos[i].text + " ";

        QString dataFilePath = QString("%1/%2_Data.DAT").arg(prefixPath).arg(buttonInfos[i].cardInfo);
        QString iniFilePath = QString("%1/%2_Data.INI").arg(prefixPath).arg(buttonInfos[i].cardInfo);
        dataFilePaths.insert(buttonInfos[i].cardInfo, dataFilePath);
        iniFilePaths.insert(buttonInfos[i].cardInfo, iniFilePath);

        printLogs(dataFilePath);

        //这里通道总数和采样率先写死 后面根据获取的卡参数动态赋值 暂时没有取值..
        ChannelModel channel(buttonInfos[i].text, buttonInfos[i].cardInfo, buttonInfos[i].channelInfo, /* totalChannels */ 4, /* samplingRate */ 500000000);

        if (!channels.contains(buttonInfos[i].cardInfo)) {
            channels.insert(buttonInfos[i].cardInfo, QVector<ChannelModel>());
        }
        channels[buttonInfos[i].cardInfo].append(channel);
    }

    // if(buttonInfos.size() > 40)
    // {
    //     QMessageBox::information(this, "提示", "单次导出通道数不超过40个");
    //     return;
    // }

    if(buttonInfos.size() <= 0 )
    {
        QMessageBox::information(this, "提示", "请勾选需要导出的通道");
        return;
    }

    printLogs("当前选中通道：" + allButtonTexts);

    if(dataProcessor)
    {
        delete dataProcessor;
        dataProcessor = nullptr;
    }
    if(csvExporter)
    {
        delete csvExporter;
        csvExporter = nullptr;
    }
    if(exportTask)
    {
        delete exportTask;
        exportTask = nullptr;
    }


    // 创建数据处理器 （如果需要校准数据）
    dataProcessor = new DataProcessing(m_channel_coefficients, m_channel_coefficientsb);
    // 创建 CSV 导出器实例
    csvExporter = ExporterFactory::createExporter();

    IExporter* exporterInterface = csvExporter;

    if (!exporterInterface) {
        qDebug() << "【错误】导出器接口为空！";
        return;
    }

    // 创建 ExportTask 实例
    exportTask = new ExportTask(channels, dataFilePaths, iniFilePaths, exportPath, csvExporter, dataProcessor);

    // 连接导出进度信号
    QObject::connect(exportTask, &ExportTask::exportFinished, this, &MainView::onExportFinished, Qt::QueuedConnection);
    QObject::connect(exportTask, &ExportTask::exportFailed, [this](const QString &error) {
        // 处理导出失败
        qDebug() << "Export failed:" << error;
        emit putOverLog(" 导出失败:" + error);
    });

    bool connected = QObject::connect(exporterInterface, &IExporter::exportProgress, this, [this](const QString &info) {
            qDebug() << "【收到信号】Export info:" << info;
            emit putOverLog(" 导出信息:" + info);
        }, Qt::QueuedConnection);

    if (!connected) {
        qDebug() << "【错误】信号连接失败！";
    }

    // 开始导出
    exportTask->startExport();
}


void MainView::connectTest()
{
    qDebug() << "connectTest";
    QMessageBox::information(this, "提示", "开发中");
}

void MainView::connectToHost()
{
    // // 设置本地UDP端口
    // QString localIp = localIpComboBox->currentText();
    // quint16 localPort = localPortLineEdit->text().toUInt();

    // // 本地
    // _driverPlugin->enableDriver(localIp, localPort);
    // disconnectButton->setEnabled(true);
    // connectButton->setEnabled(false);

    try {
        // 检查 _driverPlugin 是否为空
        if (!_driverPlugin) {
            throw std::runtime_error("Driver plugin is not initialized.");
        }

        // 设置本地UDP端口
        QString localIp = localIpComboBox->currentText();
        bool portOk;
        quint16 localPort = localPortLineEdit->text().toUInt(&portOk);

        // 验证输入有效性
        if (localIp.isEmpty() || !portOk) {
            throw std::invalid_argument("Invalid IP or Port provided.");
        }

        // 本地
        //_driverPlugin->enableDriver("0.0.0.0", localPort);
        _driverPlugin->enableDriver(localIp, localPort);
        disconnectButton->setEnabled(true);
        connectButton->setEnabled(false);

        // 采集按钮就绪
        startBtn->setEnabled(true);
    } catch (const std::exception& e) {
        qCritical() << "Error connecting to host:" << e.what();
        QMessageBox::information(this, "提示", e.what());
    }
}

void MainView::disconnectFromHost()
{
    _driverPlugin->disableDriver();
    disconnectButton->setEnabled(false);
    connectButton->setEnabled(true);
}

// void MainView::startudpCollect()
// {
//     startBtn->setEnabled(false);

//     int acquisitionTimeValue = acquisitionCard->getAcquisitionTime();
//     int index =acquisitionCard->getTriggerModeIndex();
//     // int samplingRate = 250;
//     // int bagnumber = acquisitionTimeValue * samplingRate*8/1000;

//     // 1G
//     int samplingRate = judgmentSamplingRate()/1000/1000;
//     int dataGroupSize = 8;
//     if(1000 == samplingRate)
//     {
//         dataGroupSize = 4;
//     }
//     int bagnumber = acquisitionTimeValue * samplingRate * dataGroupSize/1000;
//     qDebug() << "samplingRate" << samplingRate << " dataGroupSize " << dataGroupSize;
//     QString msg = QString("  当前采样率 %1").arg(samplingRate);
//     printLogs(msg);


//     emit sendBufferSize(CalculateSize(bagnumber));
//     QString acquisitionTime = QString("%1").arg(acquisitionTimeValue * 4, 8, 16, QChar('0')).toUpper();
//     if(1000 == samplingRate)
//     {
//         acquisitionTime = QString("%1").arg(acquisitionTimeValue * 2, 8, 16, QChar('0')).toUpper();
//     }

//     QString softtrigger = "03" + acquisitionTime;
//     QString hardtrigger = "04" + acquisitionTime;
//     QByteArray soft = QByteArray::fromHex(softtrigger.toUtf8());
//     QByteArray hard = QByteArray::fromHex(hardtrigger.toUtf8());
//     quint16 targetPort = 8080;
//     for (int i = 0; i < ipList.size(); ++i) {
//             QString remoteIP = ipList[i];
//             if (index == 0) {
//                 // 软件触发
//                 _driverPlugin->writeData(remoteIP, targetPort, soft);
//                 qDebug() << "remoteIP" << remoteIP;
//                 qDebug() << "Software Trigger - hexValue" << softtrigger;
//             } else if (index == 1) {
//                 // 硬件触发
//                 _driverPlugin->writeData(remoteIP, targetPort, hard);
//                 qDebug() << "remoteIP" << remoteIP;
//                 qDebug() << "Hardware Trigger - hexValue" << hardtrigger;
//             }
//         }

//     QTimer::singleShot(3000, [this](){
//         startBtn->setEnabled(true);
//     });
// }


void MainView::startudpCollect()
{
    QLOG_INFO() << ">>>>>>>> startudpCollect begin...";
    startBtn->setEnabled(false);

    acquisitionExecutionTimes = 0 ;// 统计计数清零。
    
    // 丢包检测初始化
    isAcquisitionRunning = true;
    cardDataReceivedMap.clear();
    for (const QString &ip : ipList) {
        cardDataReceivedMap.insert(ip, false);
    }
    
    // 启动采集超时定时器
    acquisitionTimeoutTimer->start(acquisitionTimeoutInterval);

    int acquisitionTimeValue = acquisitionCard->getAcquisitionTime();
    int index =acquisitionCard->getTriggerModeIndex();

    for (int i = 0; i < ipList.size(); ++i) {
        QString remoteIP = ipList[i];


        int samplingRate = judgmentSamplingRate(remoteIP)/1000/1000;
        int dataGroupSize = 8;
        if(1000 == samplingRate) //1G是8其余是4
        {
            dataGroupSize = 4;
        }
        int bagnumber = acquisitionTimeValue * samplingRate * dataGroupSize/1000;

        // // 向上取整 适应非倍数的采样率 更合理波形有问题先不加
        // double intermediateResult = acquisitionTimeValue * samplingRate * dataGroupSize / 1000.0;
        // int bagnumber = qCeil(intermediateResult);

        QLOG_INFO() << "samplingRate" << samplingRate << " dataGroupSize " << dataGroupSize<<  " bagnumber " << bagnumber;
        QLOG_INFO() << "acquisitionTimeValue" << acquisitionTimeValue;
        QString msg = QString(" 当前IP %1  采样率 %2").arg(remoteIP).arg(samplingRate);
        printLogs(msg);
        QLOG_INFO() << msg;

        emit sendBufferSize(remoteIP, CalculateSize(bagnumber));
        // // 为了防止丢包现象 采样时间多了1us 后期会恢复
        // QString acquisitionTime = QString("%1").arg( (acquisitionTimeValue+1) * 4, 8, 16, QChar('0')).toUpper();
        QString acquisitionTime = QString("%1").arg( (acquisitionTimeValue) * 4, 8, 16, QChar('0')).toUpper();

        QString softtrigger = "03" + acquisitionTime;
        QString hardtrigger = "04" + acquisitionTime;
        QByteArray soft = QByteArray::fromHex(softtrigger.toUtf8());
        QByteArray hard = QByteArray::fromHex(hardtrigger.toUtf8());
        quint16 targetPort = 8080;


        if (index == 0) {
            // 软件触发
            _driverPlugin->writeData(remoteIP, targetPort, soft);
            QLOG_INFO() << "remoteIP" << remoteIP;
            QLOG_INFO() << "Software Trigger - hexValue" << softtrigger;
        } else if (index == 1) {
            // 硬件触发
            _driverPlugin->writeData(remoteIP, targetPort, hard);
            // 再发两次硬件触发
            // _driverPlugin->writeData(remoteIP, targetPort, hard);
            // _driverPlugin->writeData(remoteIP, targetPort, hard);
            QLOG_INFO() << "remoteIP" << remoteIP;
            QLOG_INFO() << "Hardware Trigger - hexValue" << hardtrigger;
        }
    }

    QTimer::singleShot(3000, [this](){
        startBtn->setEnabled(true);
    });
}

//硬件触发前信息处理
// void MainView::beforeHardTriggerStart(int value)
// {
//     printLogs("  采集卡" + QString::number(value) + "执行硬件触发采集");

//     hardTriggerMutex.lock();

//     beforeHardTriggerStartIndex++;

//     hardTriggerMutex.unlock();

//     statusIndicator->setStyleSheet(QString("background-color: yellow; border-radius: %1px;").arg(40));

//     if(beforeHardTriggerStartIndex == dataInteractionLogicals->size()){
//         autoIncreasedFlag = 1;
//         beforeHardTriggerStartIndex = 0;
//     }
// }


//默认参数填充
void MainView::parameterFilling()
{
    QStringList pickUpList;
    QStringList triggerModeList;
    QStringList daqCaseList;
    QStringList collectModeList;

    QStringList cardNumberList;
    QStringList rawDataAnalysisList;
    QStringList acquisitionNumberAutomations;
    QStringList acquisitionsamplerateList;
    pickUpList.append("0.01us");
    pickUpList.append("0.02us");
    pickUpList.append("0.1us");

    triggerModeList.append("软件触发");
    triggerModeList.append("硬件触发");


    daqCaseList.append("YD1-125M");
    daqCaseList.append("YD1-200M");
    daqCaseList.append("YD2-125M");
    daqCaseList.append("YD2-200M");
    daqCaseList.append("YD3-200M");
    daqCaseList.append("YD3-250M");
    daqCaseList.append("YD4-500M");
    daqCaseList.append("YD5-500M");
    daqCaseList.append("YD6-1G");
    daqCaseList.append("YD7-500M");
    daqCaseList.append("YD8-1G");
    daqCaseList.append("YD9-2X");
    daqCaseList.append("YD9-3X");
    daqCaseList.append("YD9-4X");
    daqCaseList.append("YD9-5X");
    daqCaseList.append("YD9-6X");
    daqCaseList.append("YD9-7X");
    daqCaseList.append("YD9-8X");


    collectModeList.append("网口并发采集");
    //collectModeList.append("网口限流采集");

    int insertionCondition = -1;
    for(int i = 0;i<Parameter::ACQUISITIONCARDNUMBER;i++){
        insertionCondition = cardInformations->at(i)->getInsertionCondition();
        if(insertionCondition == 1){
            cardNumberList.append(QString("卡%1").arg(i));
        }
    }
    rawDataAnalysisList.append("no");
    rawDataAnalysisList.append("0 - 10");
    rawDataAnalysisList.append("10 - 20");
    rawDataAnalysisList.append("20 - 30");
    rawDataAnalysisList.append("30 - 40");
    rawDataAnalysisList.append("40 - 50");
    rawDataAnalysisList.append("50 - 60");
    rawDataAnalysisList.append("60 - 70");
    rawDataAnalysisList.append("70 - 80");
    rawDataAnalysisList.append("80 - 90");
    rawDataAnalysisList.append("90 - 100");
    rawDataAnalysisList.append("全解析");

    acquisitionNumberAutomations.append("yes");
    acquisitionNumberAutomations.append("no");

    acquisitionsamplerateList.append("125M");
    acquisitionsamplerateList.append("200M");
    acquisitionsamplerateList.append("250M");
    acquisitionsamplerateList.append("500M");
    acquisitionsamplerateList.append("1G");
    acquisitionsamplerateList.append("AUTO");

    pickUpBox->addItems(pickUpList);
    //pickUpBox->setCurrentIndex(1);
    modeChangeBox->addItems(triggerModeList);
    daqCaseBox->addItems(daqCaseList);
    collectModeBox->addItems(collectModeList);
    cardNumberBox->addItems(cardNumberList);
    rawDataAnalysisBox->addItems(rawDataAnalysisList);
    //rawDataAnalysisBox->setCurrentIndex(11);
    acquisitionNumberAutomationComboBox->addItems(acquisitionNumberAutomations);
    acquisitionsamplerateBox->addItems(acquisitionsamplerateList);
   // acquisitionsamplerateBox->setCurrentIndex(1);
    //AcardInfos.resize(AcardList.size());

    QSettings collectSettings("parameterSKDFDA", "value");

    QString daqCaseBoxText = collectSettings.value("daqCaseBoxText","").toString();
    daqCaseBox->setCurrentText(daqCaseBoxText);

    autosetAcquisitionSampleRate(daqCaseBoxText, acquisitionsamplerateBox);

    int collectModeIndex = collectSettings.value("collectModeIndex", 0).toInt();
    collectModeBox->setCurrentIndex(collectModeIndex);
}

void MainView::autosetAcquisitionSampleRate(const QString &daqCase, QComboBox *acquisitionsamplerateBox)
{
    QRegularExpression re("-([0-9]+)(M|G|X)");
    QRegularExpressionMatch match = re.match(daqCase);
    if (match.hasMatch()) {
        QString numberStr = match.captured(1);
        QString unit = match.captured(2);
        bool ok;
        int number = numberStr.toInt(&ok);
        if (ok) {
            // 根据提取的数字和单位设置采样率
            QString sampleRate;
            switch (number) {
            case 125:
                sampleRate = "125M";
                break;
            case 200:
                sampleRate = "200M";
                break;
            case 250:
                sampleRate = "250M";
                break;
            case 500:
                sampleRate = "500M";
                break;
            case 1:
                if (unit == "G") {
                    sampleRate = "1G";
                } else {
                    sampleRate = ""; // 处理不匹配的情况
                }
                break;
            case 2:
            case 3:
            case 4:
            case 5:
            case 6:
            case 7:
            case 8:
                if (unit == "X") {
                    sampleRate = "AUTO";
                } else {
                    sampleRate = ""; // 处理不匹配的情况
                }
                break;
            default:
                sampleRate = ""; // 默认值或错误处理
                break;
            }

            if (!sampleRate.isEmpty()) {
                int index = acquisitionsamplerateBox->findText(sampleRate);
                if (index != -1) {
                    acquisitionsamplerateBox->setCurrentIndex(index);

                    //更新注册表和缓存
                    QSettings collectSettings("parameterSKDFDA", "value");
                    collectSettings.setValue("acquisitionsamplerate", index);
                    acquisitionCard->setacquisitionsamplerateIndex(index);

                    printLogs("   自动识别机箱采样率 已选择" + sampleRate + " 请核对");
                } else {
                    printLogs("   未找到匹配的采样率选项: " + sampleRate);
                }
            } else {
                printLogs("   未自动识别该机箱采样率 请核对选择");
            }
        } else {
            printLogs("   autosetAcquisitionSampleRate转换数字失败 请检查输入格式");
        }
    } else {
        printLogs("   autosetAcquisitionSampleRate正则表达式匹配失败 请检查输入格式");
    }
}

//曲线设置
void MainView::repaintGraph(PlanarGraph *planarGraph)
{
    planarGraph->update_bkColor("black");
    QColor whiteTextColor = Qt::green;
    QFont labelFont("Calibri", 16);
    QFont tickLableFont("Calibri", 12);
    planarGraph->plot->xAxis->setBasePen(QPen(whiteTextColor));
    planarGraph->plot->yAxis->setBasePen(QPen(whiteTextColor));
    planarGraph->plot->xAxis->setTickPen(QPen(whiteTextColor));
    planarGraph->plot->yAxis->setTickPen(QPen(whiteTextColor));
    planarGraph->plot->xAxis->setSubTickPen(QPen(whiteTextColor));
    planarGraph->plot->yAxis->setSubTickPen(QPen(whiteTextColor));
    planarGraph->plot->xAxis->setTickLabelColor(whiteTextColor);
    planarGraph->plot->yAxis->setTickLabelColor(whiteTextColor);

    planarGraph->plot->xAxis->setLabel("time / us");
    planarGraph->plot->xAxis->setLabelColor(Qt::white);
    planarGraph->plot->yAxis->setLabel("voltage / V");
    planarGraph->plot->yAxis->setLabelColor(Qt::white);
    planarGraph->plot->xAxis->setLabelFont(labelFont);
    planarGraph->plot->yAxis->setLabelFont(labelFont);
    planarGraph->plot->xAxis->setTickLabelFont(tickLableFont);
    planarGraph->plot->yAxis->setTickLabelFont(tickLableFont);

//    planarGraph->devText->setTextColor(Qt::white);

    planarGraph->plot->replot();
}


void MainView::acquisitionFileNameStore()
{
    int acquisitionNumber = acquisitionNumberSpinBox->value();
    QString acquisitionFileName = "export" + QDateTime::currentDateTime().toString("yyMMdd_hhmm");
    if(!autoFileNamedCheckBox->isChecked())
    {
        acquisitionFileNameEdit->setText(acquisitionFileName);
    }
    else{
        QString strAcquisitionNumber = QString::number(acquisitionNumber);
        acquisitionFileNameEdit->setText(strAcquisitionNumber);
    }
}

//参数保存
void MainView::parameterStore()
{
    QSettings collectSettings("parameterSKDFDA", "value");

    int acquisitionTime = acquisitionTimeBox->value();
    int pickUpIndex  = pickUpBox->currentIndex();
    int cardNumberIndex = cardNumberBox->currentIndex();
    int triggerModeIndex = modeChangeBox->currentIndex();
    QString daqCaseBoxText = daqCaseBox->currentText();
    int collectModeIndex = collectModeBox->currentIndex();
    int rawDataAnalysisIndex = rawDataAnalysisBox->currentIndex();
    int PretriggerTime = PretriggerSpinBox->value();
    int PosttriggerTime = PosttriggerSpinBox->value();
    int acquisitionNumber = acquisitionNumberSpinBox->value();
    int acquisitionNumberAutomationIndex = acquisitionNumberAutomationComboBox->currentIndex();
    int acquisitionsamplerateIndex = acquisitionsamplerateBox->currentIndex();
    QString fileStorageLocation = fileStirageLocationEdit->text();
    QString fileHistoryLocation = fileHistoryLocationEdit->text();
    QString acquisitionFileName = acquisitionFileNameEdit->text();

    int baseline1 = baseline1SpinBox->value();
    int baseline2 = baseline2SpinBox->value();
    int baseline3 = baseline3SpinBox->value();
    int baseline4 = baseline4SpinBox->value();

    collectSettings.setValue("acquisitionTime",acquisitionTime);
    collectSettings.setValue("pickUpIndex",pickUpIndex);
    collectSettings.setValue("acquisitionNumber", acquisitionNumber);
    collectSettings.setValue("cardNumberIndex",cardNumberIndex);
    collectSettings.setValue("triggerModeIndex",triggerModeIndex);
    collectSettings.setValue("daqCaseBoxText",daqCaseBoxText);
    collectSettings.setValue("collectModeIndex",collectModeIndex);
    collectSettings.setValue("rawDataAnalysisIndex",rawDataAnalysisIndex);
    collectSettings.setValue("PretriggerTime", PretriggerTime);
    collectSettings.setValue("PosttriggerTime", PosttriggerTime);
    collectSettings.setValue("acquisitionNumberAutomationIndex", acquisitionNumberAutomationIndex);
    collectSettings.setValue("baseline1", baseline1);
    collectSettings.setValue("baseline2", baseline2);
    collectSettings.setValue("baseline3", baseline3);
    collectSettings.setValue("baseline4", baseline4);
    collectSettings.setValue("fileStorageLocation", fileStorageLocation);
    
    // 新增：保存 autoFileNamedCheckBox 的选中状态
    bool isAutoFileNameChecked = autoFileNamedCheckBox->isChecked();
    collectSettings.setValue("autoFileNamedChecked", isAutoFileNameChecked);

    if(fileHistoryLocation!="")
    {
        collectSettings.setValue("fileHistoryLocation", fileHistoryLocation);
    }

    // collectSettings.setValue("acquisitionFileName", acquisitionFileName);    //存储文件名不记忆
    //collectSettings.setValue("acquisitionsamplerate", acquisitionsamplerateIndex);

}

//读取参数
void MainView::readParameter()
{
    QSettings collectSettings("parameterSKDFDA", "value");

    int acquisitonTime = collectSettings.value("acquisitionTime",0).toInt();
    int pickUpIndex = collectSettings.value("pickUpIndex",0).toInt();
    int triggerModeIndex = collectSettings.value("triggerModeIndex",0).toInt();

    int cardNumberIndex = collectSettings.value("caedNumberIndex",0).toInt();
    int rawDataAnalysisIndex = collectSettings.value("rawDataAnalysisIndex",0).toInt();
    int PretriggerTime = collectSettings.value("PretriggerTime", 0).toInt();
    int PosttriggerTime = collectSettings.value("PosttriggerTime", 0).toInt();
    int acquisitionNumber = collectSettings.value("acquisitionNumber", 0).toInt();
    int acquisitionNumberAutomationIndex = collectSettings.value("acquisitionNumberAutomationIndex", 0).toInt();
    int channelNumber10V = collectSettings.value("channelNumber10V", 0).toInt();
    int baseline1 = collectSettings.value("baseline1", 0).toInt();
    int baseline2 = collectSettings.value("baseline2", 0).toInt();
    int baseline3 = collectSettings.value("baseline3", 0).toInt();
    int baseline4 = collectSettings.value("baseline4", 0).toInt();
    int AcardIndex = collectSettings.value("AcardIndex", 0).toInt();
    int acquisitionsamplerateIndex = collectSettings.value("acquisitionsamplerate", 0).toInt();
    QString fileStorageLocation = collectSettings.value("fileStorageLocation", "").toString();
    QString acquisitionFileName = collectSettings.value("acquisitionFileName", "").toString();

    acquisitionCardSet(acquisitonTime,pickUpIndex,triggerModeIndex,rawDataAnalysisIndex,PretriggerTime,PosttriggerTime,
                       acquisitionNumber,acquisitionNumberAutomationIndex,fileStorageLocation,acquisitionFileName,acquisitionsamplerateIndex);
}

//参数转移到试图
void MainView::parameterFill()
{
    int acquisitionTime = acquisitionCard->getAcquisitionTime();
    int pickUpIndex = acquisitionCard->getPickUpIndex();
    int modeChangeIndex = acquisitionCard->getTriggerModeIndex();
    int rawDataAnalysisIndex = acquisitionCard->getRawDataAnalysisIndex();
    int PretriggerTime = acquisitionCard->getPretriggerTime();
    int PosttriggerTime =acquisitionCard->getPosttriggerTime();
    int acquisitionNumber = acquisitionCard->getAcquisitionNumber();
    int acquisitionNumberAutomationIndex = acquisitionCard->getAcquisitionNumberAutomationIndex();
    int baseline1 = acquisitionCard->getbaseline1();
    int baseline2 = acquisitionCard->getbaseline2();
    int baseline3 = acquisitionCard->getbaseline3();
    int baseline4 = acquisitionCard->getbaseline4();
    int acquisitionsamplerateIndex = acquisitionCard->getacquisitionsamplerateIndex();
    QString fileStorageLocation = acquisitionCard->getFileStorageLocation();


    acquisitionTimeBox->setValue(acquisitionTime);
    pickUpBox->setCurrentIndex(pickUpIndex);
    modeChangeBox->setCurrentIndex(modeChangeIndex);
    rawDataAnalysisBox->setCurrentIndex(rawDataAnalysisIndex);
    PretriggerSpinBox->setValue(PretriggerTime);
    PosttriggerSpinBox->setValue(PosttriggerTime);
    acquisitionNumberSpinBox->setValue(acquisitionNumber);
    acquisitionNumberAutomationComboBox->setCurrentIndex(acquisitionNumberAutomationIndex);
    baseline1SpinBox->setValue(baseline1);
    baseline2SpinBox->setValue(baseline2);
    baseline3SpinBox->setValue(baseline3);
    baseline4SpinBox->setValue(baseline4);
    //acquisitionsamplerateBox->setCurrentIndex(acquisitionsamplerateIndex);
    fileStirageLocationEdit->setText(fileStorageLocation);

    // 根据是否自动命名设置存储文件名
    acquisitionFileNameStore();


    QSettings collectSettings("parameterSKDFDA", "value");
    // 加载文件历史路径
    QString fileHistoryLocation = collectSettings.value("fileHistoryLocation").toString();
    if (!fileHistoryLocation.isEmpty()) {
        fileHistoryLocationEdit->setText(fileHistoryLocation);
    }

    // 新增：加载自动编号设置
    bool isAutoFileNameChecked = collectSettings.value("autoFileNamedChecked", false).toBool();
    autoFileNamedCheckBox->setChecked(isAutoFileNameChecked);

    // collectSettings.setValue("acquisitionFileName", acquisitionFileName);    //存储文件名不记忆
}

//采集卡设置
void MainView::acquisitionCardSet(int acquisitionTime, int pickUpIndex, int triggerModeIndex,int rawDataAnalysisIndex,int PretriggerTime,int PosttriggerTime,
                                  int acquisitionNumber,int acquisitionNumberAutomationIndex,QString fileStorageLocation,QString acquisitionFileName,int acquisitionsamplerateIndex)
{
    acquisitionCard = new AcquisitionCard();
    acquisitionCard->setAcquisitionTime(acquisitionTime);
    acquisitionCard->setPickUpIndex(pickUpIndex);
    acquisitionCard->setTriggerModeIndex(triggerModeIndex);
    acquisitionCard->setRawDataAnalysisIndex(rawDataAnalysisIndex);
    acquisitionCard->setPretriggerTime(PretriggerTime);
    acquisitionCard->setPosttriggerTime(PosttriggerTime);
    acquisitionCard->setAcquisitionNumber(acquisitionNumber);
    acquisitionCard->setAcquisitionNumberAutomationIndex(acquisitionNumberAutomationIndex);
    // acquisitionCard->setbaseline1(baseline1);
    // acquisitionCard->setbaseline2(baseline2);
    // acquisitionCard->setbaseline3(baseline3);
    // acquisitionCard->setbaseline4(baseline4);
    acquisitionCard->setFileStorageLocation(fileStorageLocation);
    acquisitionCard->setAcquisitionFileName(acquisitionFileName);
    acquisitionCard->setacquisitionsamplerateIndex(acquisitionsamplerateIndex);
}

void MainView::getAlllogical()
{
    cardInformations = this->returnCardInformations();
    mainLogical = this->returnMainLogical();
    dataInteractionLogicals = mainLogical->returnDataInteractionLogicals();
    ipList = mainLogical->getipList();
}


// void MainView::startParsingHistory()
// {
//     // QDate currentDate = QDate::currentDate();
//     // int year = currentDate.year();
//     // int month = currentDate.month();
//     // int day = currentDate.day();
//     int acquisitionNumber = 0;
//     QSet<QString> tempDataFilePathSet;
//     QSet<QString> tempSettingsFilePathSet;

//     acquisitionNumber = acquisitionNumberSpinBox->value();
//         //匹配上电卡
//     for(int i = 0; i < buttonInfos.size(); i++) {
//         int cardNumber =  buttonInfos[i].cardInfo;
//         qDebug()<<"cardNumber:"<<cardNumber;
//         QString fileName = "Data";

//         // QString basePath = acquisitionCard->getFileStorageLocation();
//         // QString yearPath = basePath + "/" + QString::number(year);
//         // QString monthdayPath = yearPath + "/" + QString::number(month).rightJustified(2, '0') + QString::number(day).rightJustified(2, '0');
//         //QString dayPath = monthPath + "/" + QString::number(day);

//         QString tempDataFilePath = fileHistoryLocationEdit->text() + "/" + QString::number(acquisitionNumber-1)
//                                    + "/" + QString::number(cardNumber) + "_"
//                                    + fileName + ".DAT";
//         // qDebug() << "tempDataFilePath"<<tempDataFilePath;

//         QString tempSettingsFilePath = fileHistoryLocationEdit->text() + "/" + QString::number(acquisitionNumber-1)
//                                        + "/" + QString::number(cardNumber) + "_"
//                                        + fileName + ".INI";
//         tempDataFilePathSet.insert(tempDataFilePath);
//         tempSettingsFilePathSet.insert(tempSettingsFilePath);
//     }
//     tempDataFilePathList = tempDataFilePathSet.toList();
//     tempSettingsFilePathList = tempSettingsFilePathSet.toList();
//     QMap<QString, int> filePathToCardNumberInt;
//     QRegularExpression regex("(\\d+)_.*\\.DAT");
//     for (int i = 0; i < tempDataFilePathList.size(); ++i) {
//         QRegularExpressionMatch match = regex.match(tempDataFilePathList[i]);
//         if (match.hasMatch()) {
//             QString cardNumber = match.captured(1);
//             int cardNumberInt = cardNumber.toInt();
//             filePathToCardNumberInt.insert(tempDataFilePathList[i], cardNumberInt);
//         }
//     }
//     tempDataFilePathList.clear();

//     // 获取排序后的 cardNumberInt 列表
//     QList<int> sortedCardNumberIntList = filePathToCardNumberInt.values();
//     std::sort(sortedCardNumberIntList.begin(), sortedCardNumberIntList.end());

//     if(sortedCardNumberIntList.isEmpty())
//     {
//         qDebug() << "未选择卡";
//         QMessageBox::warning(this, "警告", "未选择采集卡");
//         return;
//     }

//     // 输出排序后的文件路径列表
//     //qDebug() << "Sorted file paths by cardNumberInt:";
//     for (int cardNumberInt : sortedCardNumberIntList) {
//         // 根据 cardNumberInt 获取对应的文件路径
//         QString filePath = filePathToCardNumberInt.key(cardNumberInt);
//         tempDataFilePathList.append(filePath);
//         // qDebug() << filePath;
//     }



//     qDebug()<<"tempDataFilePathList"<<tempDataFilePathList;
//     qDebug()<<"tempSettingsFilePathList"<<tempSettingsFilePathList;
//     if(tempDataFilePathList.empty()){
//         return;
//     }
//     mergeHistoryFiles();
// }


QString MainView::getSaveFilePathPrefix()
{
    QDate currentDate = QDate::currentDate();
    int year = currentDate.year();
    int month = currentDate.month();
    int day = currentDate.day();

    QString basePath = acquisitionCard->getFileStorageLocation();
    QString yearPath = basePath + "/" + QString::number(year);
    QString monthdayPath = yearPath + "/" + QString::number(month).rightJustified(2, '0') + QString::number(day).rightJustified(2, '0');

    QDir yearDir;
    QDir monthdayDir;

    if (!yearDir.exists(yearPath)) {
        yearDir.mkpath(yearPath);
    }

    if (!monthdayDir.exists(monthdayPath)) {
        monthdayDir.mkpath(monthdayPath);
    }
    int acquisitionNumber = acquisitionCard->getAcquisitionNumber();
    QString acquisitionNumberPath = monthdayPath + "/" + QString::number(acquisitionNumber-1);

    if (!QDir().exists(acquisitionNumberPath)) {
        QDir().mkpath(acquisitionNumberPath);
    }

    QLOG_INFO() << "getSaveFilePathPrefix..." << acquisitionNumberPath;
    return acquisitionNumberPath;
}


QString MainView::getSaveCSVFilePath()
{
    QDate currentDate = QDate::currentDate();
    int year = currentDate.year();
    int month = currentDate.month();
    int day = currentDate.day();

    QString basePath = acquisitionCard->getFileStorageLocation();
    QString yearPath = basePath + "/" + QString::number(year);
    QString monthdayPath = yearPath + "/" + QString::number(month).rightJustified(2, '0') + QString::number(day).rightJustified(2, '0');

    QDir yearDir;
    QDir monthdayDir;

    if (!yearDir.exists(yearPath)) {
        yearDir.mkpath(yearPath);
    }

    if (!monthdayDir.exists(monthdayPath)) {
        monthdayDir.mkpath(monthdayPath);
    }

    QString csvPath = monthdayPath + "/CSVFile";

    if (!QDir().exists(csvPath)) {
        QDir().mkpath(csvPath);
    }

    QLOG_INFO() << "getSaveCSVFilePath..." << csvPath;
    return csvPath;
}

QString MainView::getHistorySaveFilePathPrefix()
{
    int acquisitionNumber = acquisitionCard->getAcquisitionNumber();
    QString acquisitionNumberPath = fileHistoryLocationEdit->text() + "/" + QString::number(acquisitionNumber);

    QLOG_INFO() << "getHistorySaveFilePathPrefix..." << acquisitionNumberPath;
    return acquisitionNumberPath;
}

QString MainView::getHistorySaveCSVFilePath()
{
    QString csvPath = fileHistoryLocationEdit->text() + "/CSVFile";

    if (!QDir().exists(csvPath)) {
        QDir().mkpath(csvPath);
    }

    QLOG_INFO() << "getHistorySaveCSVFilePath..." << csvPath;
    return csvPath;
}

// 新版解析
void MainView::startParsingV2(QString datPathPrefix)
{
    int ChannelNumber = acquisitionCard->getSelectChannelNumber();

    QList<QVector<double>*> channelXData;
    QList<QVector<double>*> channelYData;

    double maxX = 0;
    double minX = 0;
    double maxY = 0;
    double minY = 0;
    double tempY = 0;

    for ( int i = 0; i < ChannelNumber; i++) {
        QVector<double>* tempChannelXData = new QVector<double>;
        QVector<double>* tempChannelYData = new QVector<double>;

        channelXData.append(tempChannelXData);
        channelYData.append(tempChannelYData);
    }

    // 创建数据处理器 （如果需要校准数据）
    IDataProcessor* dataProcessor = new DataProcessing(m_channel_coefficients, m_channel_coefficientsb);

    //依次按选择通道解析数据
    for (int i = 0; i < ChannelNumber; i++) {
        DataParser dataParser;
        connect(&dataParser, &DataParser::parsingInfo, this, &MainView::printLogs);


        const ButtonInfo &buttonInfo = buttonInfos[i];
        int cardNumber = buttonInfo.cardInfo;
        int channelNumber = buttonInfo.channelInfo;
        qDebug() << QStringLiteral("buttonInfo：") << buttonInfo.text << cardNumber << channelNumber;

        QString datPath = QString("%1\\%2_Data.DAT").arg(datPathPrefix).arg(cardNumber);
        bool rst =  dataParser.parseChannelDatFile(cardNumber, channelNumber, datPath);
        if(rst)
        {
            AcquisitionData* processedData = dataParser.getAcquisitionChannelDataPtr(dataProcessor);

            double xNumber = 0.0;
            QVector<double> channelData = processedData->getCalibratedChannelData(channelNumber);
            for (int j = 0; j < channelData.size(); ++j) {
                double  samplingInterval = 1000.0 * 1000.0 / processedData->getSamplingRate();

                double result =  static_cast<double>(channelData[j]);

                tempY = result;
                minY = minY < tempY ? minY : tempY;
                maxY = maxY > tempY ? maxY : tempY;
                maxX = maxX > xNumber ? maxX : xNumber;

                channelXData[i]->append(xNumber);
                channelYData[i]->append(result);

                xNumber += samplingInterval;
            }
            mainViewDrawPlot(channelXData[i] , channelYData[i], minX, maxX, static_cast<double>(minY), static_cast<double>(maxY), i);
        }

    }

    for ( int i = 0; i < ChannelNumber; i++) {
        delete channelXData[i];
        delete channelYData[i];
    }

    delete dataProcessor;
    dataProcessor = nullptr;
}

//文件匹配
// void MainView::startParsing()
// {
//     QDate currentDate = QDate::currentDate();
//     int year = currentDate.year();
//     int month = currentDate.month();
//     int day = currentDate.day();
//     int acquisitionNumber = 0;
//     QSet<QString> tempDataFilePathSet;
//     QSet<QString> tempSettingsFilePathSet;

//     acquisitionNumber = acquisitionNumberSpinBox->value();
//         //匹配上电卡
//     for(int i = 0; i < buttonInfos.size(); i++) {
//        int cardNumber =  buttonInfos[i].cardInfo;
//             //qDebug()<<"cardNumber:"<<cardNumber;
//             QString fileName = "Data";

//             QString basePath = acquisitionCard->getFileStorageLocation();
//             QString yearPath = basePath + "/" + QString::number(year);
//             QString monthdayPath = yearPath + "/" + QString::number(month).rightJustified(2, '0') + QString::number(day).rightJustified(2, '0');
//             //QString dayPath = monthPath + "/" + QString::number(day);

//             QString tempDataFilePath = monthdayPath + "/" + QString::number(acquisitionNumber-1)
//                     + "/" + QString::number(cardNumber) + "_"
//                     + fileName + ".DAT";
//             qDebug() << tempDataFilePath<<tempDataFilePath;

//             QString tempSettingsFilePath = monthdayPath + "/" + QString::number(acquisitionNumber-1)
//                     + "/" + QString::number(cardNumber) + "_"
//                     + fileName + ".INI";
//             tempDataFilePathSet.insert(tempDataFilePath);
//             tempSettingsFilePathSet.insert(tempSettingsFilePath);
//     }
//     tempDataFilePathList = tempDataFilePathSet.toList();
//     tempSettingsFilePathList = tempSettingsFilePathSet.toList();
//     QMap<QString, int> filePathToCardNumberInt;
//     QRegularExpression regex("(\\d+)_.*\\.DAT");
//     for (int i = 0; i < tempDataFilePathList.size(); ++i) {
//             QRegularExpressionMatch match = regex.match(tempDataFilePathList[i]);
//             if (match.hasMatch()) {
//                 QString cardNumber = match.captured(1);
//                 int cardNumberInt = cardNumber.toInt();
//                 filePathToCardNumberInt.insert(tempDataFilePathList[i], cardNumberInt);
//             }
//      }
//     tempDataFilePathList.clear();

//     // 获取排序后的 cardNumberInt 列表
//     QList<int> sortedCardNumberIntList = filePathToCardNumberInt.values();
//     std::sort(sortedCardNumberIntList.begin(), sortedCardNumberIntList.end());

//     // 输出排序后的文件路径列表
//     qDebug() << "Sorted file paths by cardNumberInt:";
//     for (int cardNumberInt : sortedCardNumberIntList) {
//         // 根据 cardNumberInt 获取对应的文件路径
//         QString filePath = filePathToCardNumberInt.key(cardNumberInt);
//         tempDataFilePathList.append(filePath);
//         qDebug() << filePath;
//     }

//     qDebug()<<"tempDataFilePathList"<<tempDataFilePathList;
//     qDebug()<<"tempSettingsFilePathList"<<tempSettingsFilePathList;
//     if(tempDataFilePathList.empty()){
//         return;
//     }
//     mergeFiles();
// }

//创建多线程数据解析
void MainView::dataShowLogicDeal()
{
    dataShowLogical = new DataShowLogical(acquisitionCard);
    QThread* dataShowDealThread = new QThread();
    dataShowLogical->moveToThread(dataShowDealThread);
    dataShowDealThread->start();
}

void MainView::triggerEvent()
{
    connect(acquisitionTimeBox,QOverload<int>::of(&QSpinBox::valueChanged),[&](int newValue){
        acquisitionCard->setAcquisitionTime(newValue);
        parameterStore();
    });
    connect(pickUpBox,QOverload<int>::of(&QComboBox::currentIndexChanged),[&](int newIndex){
        acquisitionCard->setPickUpIndex(newIndex);
        parameterStore();
    });
    QObject::connect(PretriggerSpinBox, QOverload<int>::of(&QSpinBox::valueChanged), [&](int newValue) {
          acquisitionCard->setPretriggerTime(newValue);
          parameterStore();
        });
    QObject::connect(PosttriggerSpinBox, QOverload<int>::of(&QSpinBox::valueChanged), [&](int newValue) {
          acquisitionCard->setPosttriggerTime(newValue);
          parameterStore();
        });
    QObject::connect(acquisitionNumberSpinBox, QOverload<int>::of(&QSpinBox::valueChanged), [&](int newValue) {
          acquisitionCard->setAcquisitionNumber(newValue);
          parameterStore();
        });
    QObject::connect(fileStirageLocationEdit, &QLineEdit::textChanged, [&](const QString &newText) {
          acquisitionCard->setFileStorageLocation(newText);
          parameterStore();
      });

    QObject::connect(fileHistoryLocationEdit, &QLineEdit::textChanged, [&](const QString &newText) {
        parameterStore();
    });

    QObject::connect(acquisitionFileNameEdit, &QLineEdit::textChanged, [&](const QString &newText) {
          acquisitionCard->setAcquisitionFileName(newText);
          parameterStore();
      });

    // 是否自动保存
    QObject::connect(autoFileNamedCheckBox, &QCheckBox::toggled, [&](bool checked) {
            acquisitionFileNameStore();
            parameterStore();
        });

    QObject::connect(baseline1SpinBox, QOverload<int>::of(&QSpinBox::valueChanged), [&](int newValue) {
          acquisitionCard->setbaseline1(newValue);
          parameterStore();
        });
    QObject::connect(baseline2SpinBox, QOverload<int>::of(&QSpinBox::valueChanged), [&](int newValue) {
          acquisitionCard->setbaseline2(newValue);
          parameterStore();
        });
    QObject::connect(baseline3SpinBox, QOverload<int>::of(&QSpinBox::valueChanged), [&](int newValue) {
          acquisitionCard->setbaseline3(newValue);
          parameterStore();
        });
    QObject::connect(baseline4SpinBox, QOverload<int>::of(&QSpinBox::valueChanged), [&](int newValue) {
          acquisitionCard->setbaseline4(newValue);
          parameterStore();
        });


    QObject::connect(acquisitionNumberAutomationComboBox, QOverload<int>::of(&QComboBox::currentIndexChanged), [&](int newIndex) {
          acquisitionCard->setAcquisitionNumberAutomationIndex(newIndex);
          parameterStore();
      });
    QObject::connect(acquisitionsamplerateBox, QOverload<int>::of(&QComboBox::currentIndexChanged), [&](int newIndex) {
          acquisitionCard->setacquisitionsamplerateIndex(newIndex);
          parameterStore();
      });
    connect(modeChangeBox,QOverload<int>::of(&QComboBox::currentIndexChanged),[&](int newIndex){
        acquisitionCard->setTriggerModeIndex(newIndex);
        parameterStore();
    });

    connect(daqCaseBox,QOverload<int>::of(&QComboBox::currentIndexChanged),[&](int newIndex){
        parameterStore();

        QMessageBox::information(this, "提示", "选择机箱后会自动重启软件生效");

        // 获取应用程序的可执行文件路径
        QString appPath = QApplication::applicationFilePath();
        qDebug() << "Application path:" << appPath;

        // 使用 QProcess 启动新的实例
        if (!QProcess::startDetached(appPath)) {
            QMessageBox::critical(nullptr, "错误", "无法自动重启该程序，请检查软件不在包含中文、空格等有效目录后，进行手工重启" + appPath);
            return;
        }

        QTimer::singleShot(1000, [] {
            QApplication::quit();
        });
    });

    connect(collectModeBox,QOverload<int>::of(&QComboBox::currentIndexChanged),[&](int newIndex){
        parameterStore();
    });
    connect(rawDataAnalysisBox,QOverload<int>::of(&QComboBox::currentIndexChanged),[&](int newIndex){
        switch (newIndex) {
        case 0:
            pickUpBox->setEnabled(true);
            acquisitionCard->setRawDataAnalysisIndex(newIndex);
            break;
        default:
            pickUpBox->setEnabled(false);
            if(rawDataChangeTime == 0) {
                //QMessageBox::information(nullptr, "注意!", "这个操作会导致挑点显示失败");
                acquisitionCard->setRawDataAnalysisIndex(newIndex);
                rawDataChangeTime++;
            }

            rawDataChangeTime = 0;
            break;
        }
    });
}

void MainView::interfaceConfigurationInitialization()
{
    // 触发模式选择不记忆 均初始化软件模式
    modeChangeBox->setCurrentIndex(0);
}

void MainView::buttonSlot()
{
    QPushButton *optBtn = qobject_cast<QPushButton *>(sender());
    QString buttonText = optBtn->text(); // 获取按钮上的文本
    QStringList parts = buttonText.split('-');
    if(parts.size() == 2){
        QString part1 = parts.at(0);
        QString part2 = parts.at(1);
        int cardInfo = part1.mid(2).toInt(); // 提取第一个数字
        int channelInfo = part2.toInt(); // 提取第二个数字
        // 检查 buttonInfos 列表中是否已经存在相同的按钮信息
        bool isExist = false;
        for(int i = 0; i < buttonInfos.size(); i++) {
            if(buttonInfos[i].text == buttonText) {
                isExist = true;
                // 如果存在，就删除这个成员
                buttonInfos.removeAt(i);
                break;
            }
         }
        // 如果不存在，就添加新的 ButtonInfo 对象
        if(!isExist) {
            ButtonInfo info;
            info.text = buttonText;
            info.isClicked = !info.isClicked;
            info.cardInfo = cardInfo;
            info.channelInfo = channelInfo;
            buttonInfos.append(info);
        }
    }


    int channelCount = 0;
    for(int i = 0; i < channelButtons.count(); i++){
        int flag = channelBtn[i]->isChecked();
        if(flag == 1){
            channelCount++;
        }
    }
    acquisitionCard->setSelectChannelNumber(channelCount);

   }


// QFile *MainView::FileMake()
// {
//     QDate currentDate = QDate::currentDate();
//     int year = currentDate.year();
//     int month = currentDate.month();
//     int day = currentDate.day();
//     QString basePath = acquisitionCard->getFileStorageLocation();
//     QString yearPath = basePath + "/" + QString::number(year);
//     QString monthdayPath = yearPath + "/" + QString::number(month).rightJustified(2, '0') + QString::number(day).rightJustified(2, '0');

//     QDir yearDir;
//     QDir monthdayDir;
//     //QDir dayDir;

//     if (!yearDir.exists(yearPath)) {
//         yearDir.mkpath(yearPath);
//     }

//     if (!monthdayDir.exists(monthdayPath)) {
//         monthdayDir.mkpath(monthdayPath);
//     }

//     int acquisitionNumber = acquisitionCard->getAcquisitionNumber();
//     QString acquisitionNumberPath = monthdayPath + "/" + QString::number(acquisitionNumber-1);
//     if (!QDir().exists(acquisitionNumberPath)) {
//         QDir().mkpath(acquisitionNumberPath);
//     }
//     QString tempFileName = QString::number(acquisitionNumber-1);
//     QString tempPath = acquisitionNumberPath + "/" + tempFileName;

//     QString datFilePath = tempPath +".DAT";


//     QFile* fileTempFile = new QFile(datFilePath);

//     return fileTempFile;

// }

// QFile *MainView::HistoryFileMake()
// {
//     int acquisitionNumber = acquisitionCard->getAcquisitionNumber();
//     QString acquisitionNumberPath = fileHistoryLocationEdit->text()  + "/" + QString::number(acquisitionNumber-1);
//     if (!QDir().exists(acquisitionNumberPath)) {
//         QDir().mkpath(acquisitionNumberPath);
//     }
//     QString tempFileName = QString::number(acquisitionNumber);
//     QString tempPath = acquisitionNumberPath + "/" + tempFileName;

//     QString datFilePath = tempPath +".DAT";


//     QFile* fileTempFile = new QFile(datFilePath);

//     return fileTempFile;

// }


// void MainView::mergeHistoryFiles()
// {
//     int sampingTime = this->acquisitionCard->getAcquisitionTime();
//     int PretriggerTime = this->acquisitionCard->getPretriggerTime();
//     int PosttriggerTime = this->acquisitionCard->getPosttriggerTime();
//     int acquisitionNumber = acquisitionCard->getAcquisitionNumber();
//     // QString basePath = acquisitionCard->getFileStorageLocation();
//     // QDate currentDate = QDate::currentDate();
//     // int year = currentDate.year();
//     // int month = currentDate.month();
//     // int day = currentDate.day();

//     // QString yearPath = basePath + "/" + QString::number(year);
//     // QString monthdayPath = yearPath + "/" + QString::number(month).rightJustified(2, '0') + QString::number(day).rightJustified(2, '0');
//     int time = 0;
//     if (PretriggerTime != 0 && PosttriggerTime != 0) {
//         time = 1;
//     } else if (PretriggerTime != 0) {
//         time = PretriggerTime + sampingTime ;
//     } else if (PosttriggerTime != 0) {
//         time =sampingTime-PosttriggerTime;
//     } else {
//         time = sampingTime;
//     }
//     int tempCardNumber = buttonInfos[0].cardInfo;

//     QString finalFileName = "Data";
//     QString tempsettingsPath = fileHistoryLocationEdit->text()  + "/" + QString::number(acquisitionNumber-1)
//                                + "/" + QString::number(tempCardNumber) + "_"
//                                + finalFileName;
//     QString destinationSettingsFile = tempsettingsPath +".INI";
//     QSettings settings(destinationSettingsFile, QSettings::IniFormat);

//     int samplingRate = settings.value("samplingRate").toInt();
//     int sampRate = (samplingRate / 1000 / 1000);
//     long singleChannelDataNumber = sampRate * time;
//     qDebug() << "mergeHistoryFiles_sampRate"<<sampRate;

//     QFile* tempFile = HistoryFileMake();
//     QString tempFilePath = tempFile->fileName();
//     QFile outputFile(tempFilePath);
//     if (!outputFile.open(QIODevice::WriteOnly)) {
//         qDebug() << "Failed to open output file";
//         return;
//     }

//     // 逐个读取输入文件中指定范围的数据并写入到输出文件中
//     for (int i = 0; i < tempDataFilePathList.size(); ++i) {
//         QFile inputFile(tempDataFilePathList[i]);
//         if (!inputFile.open(QIODevice::ReadOnly)) {
//             qDebug() << "Failed to open input file" << tempDataFilePathList[i];
//             printLogs("未找到数据文件" + tempDataFilePathList[i]);
//             continue;
//         }
//         QRegularExpression regex("(\\d+)_.*\\.DAT");
//         QRegularExpressionMatch match = regex.match(tempDataFilePathList[i]);
//         QString cardNumber = match.captured(1);
//         int cardNumberInt = cardNumber.toInt();

//         // 查找与当前文件卡号匹配的ButtonInfo
//         QList<ButtonInfo> matchingButtonInfos;
//         for (const ButtonInfo &buttonInfo : buttonInfos) {
//             if (buttonInfo.cardInfo == cardNumberInt) {
//                 matchingButtonInfos.append(buttonInfo);
//             }
//         }
//         // 如果没有匹配的ButtonInfo，跳过当前文件
//         if (matchingButtonInfos.isEmpty()) {
//             qDebug() << "No matching ButtonInfo for input file" << tempDataFilePathList[i];
//             inputFile.close();
//             continue;
//         }
//         for (const ButtonInfo &buttonInfo : matchingButtonInfos) {
//             int startPosition  = buttonInfo.channelInfo * singleChannelDataNumber*2;
//             inputFile.seek(startPosition);
//             QByteArray data = inputFile.read(singleChannelDataNumber*2);
//             if (data.size() != singleChannelDataNumber*2) {
//                 qDebug() << "Failed to read history data from input file" << tempDataFilePathList[i];
//                 inputFile.close();
//                 continue;
//             }
//             outputFile.write(data);
//         }
//         // 关闭输入文件
//         inputFile.close();
//     }

//     //       // 关闭输出文件
//     outputFile.close();

//     QString acquisitionNumberPath = fileHistoryLocationEdit->text() + "/" + QString::number(acquisitionNumber-1);
//     QString tempPath = acquisitionNumberPath + "/" + QString::number(acquisitionNumber-1);
//     QString datFilePath = tempPath +".DAT";
//     //       qDebug()<<"datFilePath"<<datFilePath;
//     //       qDebug()<<"destinationSettingsFile"<<destinationSettingsFile;

//     // 使用新版绘制 但是先可导出
//     // emit startDataShowLogic(datFilePath, destinationSettingsFile);
// }

// void MainView::mergeFiles()
// {
//     int sampingTime = this->acquisitionCard->getAcquisitionTime();
//     int PretriggerTime = this->acquisitionCard->getPretriggerTime();
//     int PosttriggerTime = this->acquisitionCard->getPosttriggerTime();
//     int acquisitionNumber = acquisitionCard->getAcquisitionNumber();
//     QString basePath = acquisitionCard->getFileStorageLocation();
//     QDate currentDate = QDate::currentDate();
//     int year = currentDate.year();
//     int month = currentDate.month();
//     int day = currentDate.day();

//     QString yearPath = basePath + "/" + QString::number(year);
//     QString monthdayPath = yearPath + "/" + QString::number(month).rightJustified(2, '0') + QString::number(day).rightJustified(2, '0');
//     int time = 0;
//     if (PretriggerTime != 0 && PosttriggerTime != 0) {
//         time = 1;
//     } else if (PretriggerTime != 0) {
//         time = PretriggerTime + sampingTime ;
//     } else if (PosttriggerTime != 0) {
//         time =sampingTime-PosttriggerTime;
//     } else {
//         time = sampingTime;
//     }
//    int tempCardNumber = buttonInfos[0].cardInfo;

//     QString finalFileName = "Data";
//     QString tempsettingsPath = monthdayPath + "/" + QString::number(acquisitionNumber-1)
//             + "/" + QString::number(tempCardNumber) + "_"
//             + finalFileName;
//     QString destinationSettingsFile = tempsettingsPath +".INI";
//     QSettings settings(destinationSettingsFile, QSettings::IniFormat);

//     int samplingRate = settings.value("samplingRate").toInt();
//     int sampRate = (samplingRate / 1000 / 1000);
//     long singleChannelDataNumber = sampRate * time;
//     qDebug() << "mergeFiles_sampRate"<<sampRate;
//     QString msg = QString("mergeFiles_sampRate %1").arg(sampRate);
//     printLogs(msg);

//     QFile* tempFile = FileMake();
//     QString tempFilePath = tempFile->fileName();
//     QFile outputFile(tempFilePath);
//        if (!outputFile.open(QIODevice::WriteOnly)) {
//            qDebug() << "Failed to open output file";
//            return;
//        }

//        // 逐个读取输入文件中指定范围的数据并写入到输出文件中
//        for (int i = 0; i < tempDataFilePathList.size(); ++i) {
//            QFile inputFile(tempDataFilePathList[i]);
//            if (!inputFile.open(QIODevice::ReadOnly)) {
//                qDebug() << "Failed to open input file" << tempDataFilePathList[i];
//                continue;
//            }
//            QRegularExpression regex("(\\d+)_.*\\.DAT");
//            QRegularExpressionMatch match = regex.match(tempDataFilePathList[i]);
//            QString cardNumber = match.captured(1);
//            int cardNumberInt = cardNumber.toInt();

//            // 查找与当前文件卡号匹配的ButtonInfo
//            QList<ButtonInfo> matchingButtonInfos;
//            for (const ButtonInfo &buttonInfo : buttonInfos) {
//                if (buttonInfo.cardInfo == cardNumberInt) {
//                    matchingButtonInfos.append(buttonInfo);
//                }
//            }
//            // 如果没有匹配的ButtonInfo，跳过当前文件
//            if (matchingButtonInfos.isEmpty()) {
//                qDebug() << "No matching ButtonInfo for input file" << tempDataFilePathList[i];
//                inputFile.close();
//                continue;
//            }
//            for (const ButtonInfo &buttonInfo : matchingButtonInfos) {
//                int startPosition  = buttonInfo.channelInfo * singleChannelDataNumber*2;
//                inputFile.seek(startPosition);
//                QByteArray data = inputFile.read(singleChannelDataNumber*2);
//                if (data.size() != singleChannelDataNumber*2) {
//                            qDebug() << "Failed to read data from input file" << tempDataFilePathList[i];
//                            inputFile.close();
//                            continue;
//                        }
//               outputFile.write(data);
//            }
//            // 关闭输入文件
//            inputFile.close();
//        }

// //       // 关闭输出文件
//        outputFile.close();

//        QString acquisitionNumberPath = monthdayPath + "/" + QString::number(acquisitionNumber-1);
//        QString tempPath = acquisitionNumberPath + "/" + QString::number(acquisitionNumber-1);
//        QString datFilePath = tempPath +".DAT";
// //       qDebug()<<"datFilePath"<<datFilePath;
// //       qDebug()<<"destinationSettingsFile"<<destinationSettingsFile;
//      // 临时先取消这里  emit startDataShowLogic(datFilePath, destinationSettingsFile);
// }

QColor MainView::generateColor(int index)
{
    int hue = (index * 36) % 360; // 色相值
    return QColor::fromHsv(hue, 255, 255); // 使用 HSV 颜色空间生成颜色
}

// void MainView::writeDataToCSV(const QString &filePath, const QVector<QVector<double> > &data)
// {
//     qDebug() << "开始写入数据到文件：" << filePath;
//     QFile file(filePath);
//        if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
//            qDebug() << "无法打开文件：" << file.errorString();
//            return;
//        }

//        QTextStream out(&file);

//        for (int j = 0; j < buttonInfos.size(); ++j) {
//                out << buttonInfos[j].text; // 写入按钮文本
//                if (j < buttonInfos.size() - 1) {
//                    out << ","; // 写入逗号分隔符
//                } else {
//                    out << "\n"; // 写入换行符
//                }
//            }

//        int numRows = data[0].size(); // 假设所有数据组的大小相同

//        // 遍历每一行
//        for (int i = 0; i < numRows; ++i) {
//            // 遍历每一列
//            for (int j = 0; j < data.size(); ++j) {
//                out << data[j][i]; // 写入当前数据组的当前数据点
//                if (j < data.size() - 1) {
//                    out << ","; // 写入逗号分隔符
//                } else {
//                    out << "\n"; // 写入换行符
//                }
//            }
//        }

//        file.close();

//        // 弹出提示框
//          QMessageBox::information(nullptr, "提示", "数据保存完成");
// }



//接收完成
void MainView::receieveShowOver()
{
    switch (acquisitionCard->getTriggerModeIndex()) {
    case(0):
       statusIndicator->setStyleSheet(QString("background-color: red; border-radius: %1px;").arg(40));
        break;
    case(1):
        QString currentStyle = statusIndicator->styleSheet();
       if (!currentStyle.contains("background-color: green")){
           emit hardTriggerStart();
           printLogs("等待硬件触发");
           statusIndicator->setStyleSheet(QString("background-color: green; border-radius: %1px;").arg(40));
       }
        break;
    }

}

// 处理数据接收通知
void MainView::handleDataReceived(const QString &ipAddress)
{
    if (cardDataReceivedMap.contains(ipAddress)) {
        cardDataReceivedMap[ipAddress] = true;
        QLOG_INFO() << "Data received from:" << ipAddress;
    }
}

// 处理采集超时
void MainView::handleAcquisitionTimeout()
{
    if (!isAcquisitionRunning) {
        acquisitionTimeoutTimer->stop();
        return;
    }
    
    acquisitionMutex.lock();
    
    // 停止定时器
    acquisitionTimeoutTimer->stop();
    
    // 检查哪些卡没有收到数据
    QStringList lostCards;
    for (const QString &ip : cardDataReceivedMap.keys()) {
        if (!cardDataReceivedMap[ip]) {
            lostCards.append(ip);
        }
    }
    
    if (!lostCards.isEmpty()) {
        // 记录丢包信息
        QString lostCardMsg = QString("   采集超时！以下卡未收到数据：%1").arg(lostCards.join(", "));
        printLogs(lostCardMsg);
        QLOG_INFO() << lostCardMsg;
        
        // 显示错误信息
        QMessageBox::warning(this, "采集警告", lostCardMsg + "\n已跳过这些卡继续处理。");
    }
    
    // 确保采集完成流程执行
    if (!autoIncreasedFlag) {
        autoIncreasedFlag = 1;
        
        QString overLogs = QString("#####采集完成（部分卡数据丢失） 记录号%1 #####").arg(acquisitionNumberSpinBox->value());
        emit putOverLog(overLogs);
        QLOG_INFO() << overLogs;
        
        // 增加采集序号
        int acquisitionNewNumber = acquisitionNumberSpinBox->value() + 1;
        acquisitionNumberSpinBox->setValue(acquisitionNewNumber);
        
        autoIncreasedFlag = 0;
    }
    
    // 重置采集状态
    isAcquisitionRunning = false;
    cardDataReceivedMap.clear();
    
    acquisitionMutex.unlock();
    
    // 重新启用开始按钮
    startBtn->setEnabled(true);
}

// 采集完成统计
void MainView::statisticalCollectionTimes()
{
    acquisitionMutex.lock();
    acquisitionExecutionTimes++;
    int currentCount = acquisitionExecutionTimes;
    int totalCount = dataInteractionLogicals->size();

    if (currentCount == totalCount && !autoIncreasedFlag) {
        autoIncreasedFlag = 1;

        // 注意：这里不要直接清零，除非确定不会再有并发访问
        // 最好是在触发完动作后再清零，否则可能导致重复触发
        acquisitionExecutionTimes = 0;
        
        // 停止采集超时定时器
        acquisitionTimeoutTimer->stop();
        
        // 重置采集状态
        isAcquisitionRunning = false;
        cardDataReceivedMap.clear();
        
        //startParsing();
        acquisitionMutex.unlock();

        QTimer::singleShot(1500, [=]() {
            QString overLogs =  QString("#####全部采集完成 记录号%1 #####").arg(acquisitionNumberSpinBox->value());
            emit putOverLog(overLogs);
            QLOG_INFO() << overLogs;

            int acquisitionNewNumber = acquisitionNumberSpinBox->value() + 1;
            acquisitionNumberSpinBox->setValue(acquisitionNewNumber);
            // acquisitionCard->setAcquisitionNumber(acquisitionNewNumber);
            autoIncreasedFlag = 0;

            // 自增后更新文件名输入框
            acquisitionFileNameStore();
            
            // 启用开始按钮
            startBtn->setEnabled(true);
        });

        // if((acquisitionCard->getAcquisitionNumberAutomationIndex() == 0) && (autoIncreasedFlag == 1)) {
        //     //

        // }
    } else {
        acquisitionMutex.unlock();
    }

    QString processLog = QString("采集计划总卡数：%1 已完成卡数 %2 ").arg(totalCount).arg(currentCount);
    QLOG_INFO() << processLog;
    emit putOverLog(processLog);
}

//主界面绘图
void MainView::mainViewDrawPlot(QVector<double> *channelXData, QVector<double> *channelYData, double minX, double maxX, double minY, double maxY, int index)
{
    planarGraph->resetCurve(*channelXData, *channelYData, index);
    planarGraph->plot->xAxis->setRange(minX, maxX);
    planarGraph->plot->yAxis->setRange(static_cast<double>(minY), static_cast<double>(maxY));
}

void MainView::mainViewFileNotFound(QString filePath)
{
    QMessageBox::critical(nullptr, "无法找到DATA文件", "无法找到文件: " + filePath);
}


void MainView::historyBtnClicked()
{
    planarGraph->plot->clearGraphs();
    planarGraph->plot->replot();
    for (int i = 0; i < channelButtons.count(); i++)
        channelBtn[i]->setStyleSheet("background-color: rgb(255, 255, 0);color: rgb(0, 0, 0);");
    int ChannelNumber=acquisitionCard->getSelectChannelNumber();

    qDebug() << "Selected Channel Number:" << ChannelNumber;
    // if(ChannelNumber > 40)
    // {
    //     QMessageBox::information(this, "提示", "同时解析通道数不超过40个");
    //     return;
    // }
    if(ChannelNumber <= 0 )
    {
        QMessageBox::information(this, "提示", "未选择任何通道，请先选择");
        return;
    }


    QList<QColor> plotColorList;
    int ColorIndex = 0;
    for (int i = 0; i < ChannelNumber; ++i) {
        // 使用不同的颜色，这里可以根据需要修改颜色的生成方式
        QColor color = generateColor(i);
        for (int j = ColorIndex; j < channelButtons.count(); j++){
            if(channelBtn[j]->isChecked()){
                // 修改按钮背景颜色
                QString oldStyleSheet = channelBtn[j]->styleSheet();
                QString newStyleSheet = QString("%1 background-color: %2; color: black;").arg(oldStyleSheet).arg(color.name());
                channelBtn[j]->setStyleSheet(newStyleSheet);


                // qDebug() << "Index:" << j << "Color:" << color.name();
                ColorIndex = j+1;
                break;

            }
        }
        plotColorList << color;
    }
    planarGraph->plot_2D_Init0(Qt::black,ChannelNumber,plotColorList);

    plotColorList.clear();


    std::sort(buttonInfos.begin(), buttonInfos.end(), [](const ButtonInfo &a, const ButtonInfo &b) {
        if (a.cardInfo != b.cardInfo)
            return a.cardInfo < b.cardInfo;
        return a.channelInfo < b.channelInfo;
    });

    mainLogical->addcoefficient();
    coefficientVectorUpdate();

    for(int i = 0; i < buttonInfos.size(); i++) {
        qDebug() << "按钮文本：" << buttonInfos[i].text;
        qDebug() << "card" << buttonInfos[i].cardInfo;
        qDebug() << "channel" << buttonInfos[i].channelInfo;
    }
    startParsingV2(getHistorySaveFilePathPrefix());

    // 旧版解析 后面会去掉
    // startParsingHistory();
}

//分析按钮点击
void MainView::analysisBtnClicked()
{
    planarGraph->plot->clearGraphs();
    planarGraph->plot->replot();
    for (int i = 0; i < channelButtons.count(); i++)
        channelBtn[i]->setStyleSheet("background-color: rgb(255, 255, 0);color: rgb(0, 0, 0);");
    int ChannelNumber=acquisitionCard->getSelectChannelNumber();//选中的总的通道数
    QLOG_INFO() << "Selected Channel Number:" << ChannelNumber;
    // if(ChannelNumber > 40)
    // {
    //     QMessageBox::information(this, "提示", "同时解析通道数不超过40个");
    //     return;
    // }

    if(ChannelNumber <= 0 )
    {
        QMessageBox::information(this, "提示", "未选择任何通道，请先选择");
        return;
    }


    QList<QColor> plotColorList;
    int ColorIndex = 0;
    for (int i = 0; i < ChannelNumber; ++i) {
        // 使用不同的颜色，这里可以根据需要修改颜色的生成方式
        QColor color = generateColor(i);
        for (int j = ColorIndex; j < channelButtons.count(); j++){
            if(channelBtn[j]->isChecked()){
                // 修改按钮背景颜色
                QString oldStyleSheet = channelBtn[j]->styleSheet();
                QString newStyleSheet = QString("%1 background-color: %2; color: black;").arg(oldStyleSheet).arg(color.name());
                channelBtn[j]->setStyleSheet(newStyleSheet);


               // qDebug() << "Index:" << j << "Color:" << color.name();
                ColorIndex = j+1;
                break;

            }
         }
        plotColorList << color;
    }
    planarGraph->plot_2D_Init0(Qt::black,ChannelNumber,plotColorList);

    plotColorList.clear();


    std::sort(buttonInfos.begin(), buttonInfos.end(), [](const ButtonInfo &a, const ButtonInfo &b) {
        if (a.cardInfo != b.cardInfo)
            return a.cardInfo < b.cardInfo;
        return a.channelInfo < b.channelInfo;
        });

    mainLogical->addcoefficient();
    coefficientVectorUpdate();

    for(int i = 0; i < buttonInfos.size(); i++) {
       qDebug() << "按钮文本：" << buttonInfos[i].text;
       qDebug() << "card" << buttonInfos[i].cardInfo;
       qDebug() << "channel" << buttonInfos[i].channelInfo;
    }
    startParsingV2(getSaveFilePathPrefix());

    //旧版后面去掉
    //startParsing();
}


void MainView::coefficientVectorUpdate()
{
    // coefficientVector.clear();
    // coefficientbVector.clear();

    m_channel_coefficients = mainLogical->getchannel_coefficients();
    m_channel_coefficientsb = mainLogical->getchannel_coefficientsb();
    // for (const ButtonInfo& button : buttonInfos) {
    //     QString channel = button.text; // 假设 text 就是通道名称

    //     // 检查 channel_coefficients 中是否存在该通道的系数
    //     if (channel_coefficients.contains(channel)) {
    //         double coefficient = channel_coefficients[channel]; // 获取系数
    //         double coefficientb = channel_coefficientsb[channel]; // 获取系数
    //         coefficientVector.push_back(coefficient); // 将系数添加到 vector 中
    //         coefficientbVector.push_back(coefficientb); // 将系数添加到 vector 中

    //         qDebug() << QStringLiteral("通道：") << channel
    //                  << QStringLiteral("系数a：") << coefficient
    //                  << QStringLiteral("系数b：") << coefficientb;
    //     } else {
    //         qDebug() << "未找到对应的系数，通道: " << channel;
    //     }
    // }
    emit coefficientVectorUpdated(m_channel_coefficients);
    emit coefficientbVectorUpdated(m_channel_coefficientsb);
} 

//等待实验按钮点击
void MainView::WaitBtnClicked()
{
    planarGraph->plot->clearGraphs();
    planarGraph->plot->replot();
    switch (acquisitionCard->getTriggerModeIndex()) {
    case(0):
       statusIndicator->setStyleSheet(QString("background-color: red; border-radius: %1px;").arg(40));
        break;
    case(1):
        QString currentStyle = statusIndicator->styleSheet();
       if (!currentStyle.contains("background-color: green")){
           emit hardTriggerStart();
           printLogs("等待硬件触发");
           statusIndicator->setStyleSheet(QString("background-color: green; border-radius: %1px;").arg(40));
       }
        break;
    }
}

//自动寻找按钮点击
void MainView::autoFindBtnClicked()
{
    planarGraph->autoFind();
}

void MainView::AcardSetButtonClicked()
{
    int index = 0;
    QString currentText = cardNumberBox->currentText();
    qDebug()<<"currentText"<<currentText;
    QRegularExpression re("卡(\\d+)");
    QRegularExpressionMatch match = re.match(currentText);
    if (match.hasMatch()) {
        QString numberStr = match.captured(1);
         index = numberStr.toInt();
    }
    //int index = AcardchooseBox->currentIndex();
    if (index >= 0 && index < AcardInfos.size()) {
            AcardInfos[index].isChecked = true;
        }
//    for (int i = 0; i < AcardInfos.size(); ++i) {
//            qDebug() << "Acard" << i << ": isChecked =" << AcardInfos[i].isChecked;
//    }
    QString overLogs = QString("卡%1触发已设置").arg(index);
    emit putOverLog(overLogs);
    emit acardStatusChanged(index, true);
}

void MainView::AcardReSetButtonClicked()
{
    for (int i = 0; i < AcardInfos.size(); ++i) {
            AcardInfos[i].isChecked = false;
        }
    QString overLogs = QString("同步触发");
    emit putOverLog(overLogs);
    emit resetAcardStatuses();
}

void MainView::clearLogs()
{
    logsPrint->clear();
}

void MainView::delayMsDI(int mes)
{
    QTime dieTime = QTime::currentTime().addMSecs(mes);
    while(dieTime>QTime::currentTime()){
        QCoreApplication::processEvents(QEventLoop::AllEvents,100);
    }
}

int MainView::getSamplingRate(const QString &settingsFilePath)
{
    QSettings settings(settingsFilePath, QSettings::IniFormat);
    return settings.value("samplingRate").toInt();
}


// int MainView::getHistoryDataSamplingRateForCard(int cardInfo)
// {
//     int acquisitionNumber = acquisitionCard->getAcquisitionNumber();
//     // QString basePath = acquisitionCard->getFileStorageLocation();
//     // QDate currentDate = QDate::currentDate();
//     // int year = currentDate.year();
//     // int month = currentDate.month();
//     // int day = currentDate.day();
//     // QString yearPath = basePath + "/" + QString::number(year);
//     // QString monthdayPath = yearPath + "/" + QString::number(month).rightJustified(2, '0') + QString::number(day).rightJustified(2, '0');

//     QString finalFileName = "Data";
//     QString tempSettingsPath = fileHistoryLocationEdit->text() + "/" + QString::number(acquisitionNumber)
//                                + "/" + QString::number(cardInfo) + "_"
//                                + finalFileName;
//     QString destinationSettingsFile = tempSettingsPath + ".INI";

//     return getSamplingRate(destinationSettingsFile);
// }

// int MainView::getSamplingRateForCard(int cardInfo)
// {
//     int acquisitionNumber = acquisitionCard->getAcquisitionNumber();
//     QString basePath = acquisitionCard->getFileStorageLocation();
//     QDate currentDate = QDate::currentDate();
//     int year = currentDate.year();
//     int month = currentDate.month();
//     int day = currentDate.day();
//     QString yearPath = basePath + "/" + QString::number(year);
//     QString monthdayPath = yearPath + "/" + QString::number(month).rightJustified(2, '0') + QString::number(day).rightJustified(2, '0');

//     QString finalFileName = "Data";
//     QString tempSettingsPath = monthdayPath + "/" + QString::number(acquisitionNumber)
//             + "/" + QString::number(cardInfo) + "_"
//             + finalFileName;
//     QString destinationSettingsFile = tempSettingsPath + ".INI";

//     return getSamplingRate(destinationSettingsFile);
// }

int MainView::judgmentSamplingRate(QString remoteIP)
{
    int tempSamplingRate = -1; // 默认值，表示未找到匹配的采样率

    // 将 QString 类型的 IP 转换为 QHostAddress
    QHostAddress address(remoteIP);
    if (!address.isInSubnet(QHostAddress("192.168.0.0"), 24)) {
        return tempSamplingRate; // 如果不在 192.168.0.x 子网中，返回默认值
    }

    // 获取 IP 地址的最后一部分（即主机部分）
    quint32 ipInt = address.toIPv4Address();
    quint8 lastOctet = static_cast<quint8>(ipInt & 0xFF);

    // 根据提供的规则判断采样率
    if (lastOctet >= 2 && lastOctet <= 59) {
        tempSamplingRate = Parameter::SAMPLINGRATE_500M;
    }
    else if (lastOctet >= 60 && lastOctet <= 119) {
        tempSamplingRate = Parameter::SAMPLINGRATE_200M; //12.11仍然用200M
        //tempSamplingRate = Parameter::SAMPLINGRATE_250M; // 此处板卡根据实际情况统一起来 200M 或者 250M
    }
    else if (lastOctet >= 120 && lastOctet <= 179) {
        tempSamplingRate = Parameter::SAMPLINGRATE_200M;//这里硬件有毛刺临时改成200M
    }
    else if (lastOctet >= 180 && lastOctet <= 239) {
        tempSamplingRate = Parameter::SAMPLINGRATE_1G;
    }

    return tempSamplingRate;
}

//全选按钮点击
void MainView::allChooseClicked()
{
    //return;
    bool isChecked = allChooseButton->isChecked();
    for (QPushButton* button : cardButtons) {
        button->setChecked(isChecked);
    }
    for (QPushButton* button : channelButtons) {
        button->setChecked(isChecked);
    }

}

//根据卡选
void MainView::cardChooseClicked()
{
    QPushButton* cardButton = qobject_cast<QPushButton*>(sender());
    if (cardButton) {
        bool isChecked = cardButton->isChecked();
        QList<QPushButton*> channels = cardToChannels[cardButton];
        for (QPushButton* channelButton : channels) {
            channelButton->setChecked(isChecked);
        }
    }

}


//返回采集卡信息
QList<CardInformation *> *MainView::returnCardInformations()
{
    return cardInformations;
}

//参数保存
void MainView::settingsParameterStore()
{/*
    QSettings collectSettings("settingsParameterSKDFDA", "value");

    for (int i = 0; i < Parameter::ACQUISITIONCARDNUMBER; i++) {
        collectSettings.setValue("channel0StorageConditionCard" + QString::number(i), cardInformations->at(i)->getChannel0StorageCondition());
        collectSettings.setValue("channel1StorageConditionCard" + QString::number(i), cardInformations->at(i)->getChannel1StorageCondition());
        collectSettings.setValue("channel2StorageConditionCard" + QString::number(i), cardInformations->at(i)->getChannel2StorageCondition());
        collectSettings.setValue("channel3StorageConditionCard" + QString::number(i), cardInformations->at(i)->getChannel3StorageCondition());
    }*/
}


//逻辑层注入
void MainView::injectMainLogicalToThis()
{
    mainLogical = new MainLogical(nullptr, acquisitionCard);

    this->cardInformations = mainLogical->returnViewCardInformations();
}

//参数转移
void MainView::parameterTransferToSS(AcquisitionCard* acquisitionCard)
{
    this->acquisitionCard = acquisitionCard;
}

//返回逻辑层
MainLogical *MainView::returnMainLogical()
{
    return mainLogical;
}

void MainView::autocalculated()
{
    double voltage = planarGraph->getDiffVoltage();
    qDebug() << "Current diffVoltage in PlanarGraph: " << voltage;
    buttonchannel = -1;
    buttoncard = -1;
    for (int i = 0; i < channelButtons.count(); i++)
        channelBtn[i]->setStyleSheet("background-color: rgb(255, 255, 0);color: rgb(0, 0, 0);");
    int ChannelNumber=acquisitionCard->getSelectChannelNumber();//选中的总的通道数
    QList<QColor> plotColorList;
    int ColorIndex = 0;
    for (int i = 0; i < ChannelNumber; ++i) {
        // 使用不同的颜色，这里可以根据需要修改颜色的生成方式
        if (ChannelNumber != 1) {
            QMessageBox::warning(this, "选择错误", "校准时必须且只能选择一个通道！");
            return; // 加return,提前退出，限制多选
        }
        QColor color = generateColor(i);
        for (int j = ColorIndex; j < channelButtons.count(); j++){
            if(channelBtn[j]->isChecked()){
                // 修改按钮背景颜色
                QString oldStyleSheet = channelBtn[j]->styleSheet();
                QString newStyleSheet = QString("%1 background-color: %2; color: black;").arg(oldStyleSheet).arg(color.name());
                channelBtn[j]->setStyleSheet(newStyleSheet);


               // qDebug() << "Index:" << j << "Color:" << color.name();
                ColorIndex = j+1;
                break;

            }
         }
        plotColorList << color;
    }
    planarGraph->plot_2D_Init0(Qt::black,ChannelNumber,plotColorList);

    plotColorList.clear();


    std::sort(buttonInfos.begin(), buttonInfos.end(), [](const ButtonInfo &a, const ButtonInfo &b) {
        if (a.cardInfo != b.cardInfo)
            return a.cardInfo < b.cardInfo;
        return a.channelInfo < b.channelInfo;
        });

    mainLogical->addcoefficient();
    coefficientVectorUpdate();

    for(int i = 0; i < buttonInfos.size(); i++) {
       buttoncard = buttonInfos[i].cardInfo;//将已选择的卡号和通道号赋给变量
       buttonchannel = buttonInfos[i].channelInfo;
       if (buttonchannel != -1 && buttoncard != -1) {
           qDebug() << "选中的通道是: CH" << buttonchannel;
           qDebug() << "对应的卡号是: " << buttoncard;
       } else {
           QMessageBox::information(this, "未选择通道", "请选择通道！");
       }

    }


       //以上为找在页面中所选择的卡号部分，以下为校准部分// 用于存储对应的卡号(不要把mainview的变量调入其他文件，只允许将其他模块中的变量调过来（通过设置getbianliang这种）)

           int channel = buttoncard;  // 用于记录当前数采卡卡号，保持状态
           int index = buttonchannel;    // 用于记录当前通道号，保持状态
           // 打开配置文件
           QFile file("../channel_coefficient.ini");
           if (!file.open(QIODevice::ReadWrite | QIODevice::Text)) {
               QMessageBox::critical(this, "错误", "无法打开配置文件！");
               return;
           }

           // 读取文件内容
           QTextStream stream(&file);
           QString content = stream.readAll();
           file.close();

           // 询问是校正逗号前系数还是逗号后系数
           QMessageBox::StandardButton choice = QMessageBox::question(this, "选择校正项",
               QString("当前通道: %1-%2\n请选择要校正的系数：\n\n是否校正逗号前系数？")
               .arg(channel).arg(index),  // 使用 .arg() 来正确替换占位符
               QMessageBox::Yes | QMessageBox::No);

           if (choice == QMessageBox::Yes) {
               // 校正逗号前系数
               double newCoefficient = 0.000578427 / voltage;  // 使用 diffVoltage 计算新系数
               qDebug() << QString("计算得到的新逗号前系数: %1").arg(newCoefficient);

               // 正则表达式替换逗号前系数
               QString chIndex = QString("CH%1-%2").arg(channel).arg(index);
               QString replacement = QString("%1:%2,%3").arg(chIndex).arg(newCoefficient, 0, 'f', 10).arg(0);  // 默认逗号后值为0

               // 替换配置内容
               content.replace(QRegularExpression(QString("(%1):([0-9\\.]+),([0-9]+)").arg(chIndex)), replacement);

               // 写回配置文件
               if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
                   QMessageBox::critical(this, "错误", "无法写入配置文件！");
                   return;
               }
               stream << content;
               file.close();

               qDebug() << QString("配置文件已更新（逗号前系数）：%1 -> %2").arg(chIndex).arg(replacement);

               // 等待下一次点击按钮进行校正逗号后系数
               return;
           }

           // 校正逗号后系数
           bool newValOk;
           double newValue = QInputDialog::getDouble(this, "输入逗号后值",
                                                     QString("请输入 %1-%2 逗号后的新值：").arg(channel).arg(index),
                                                     0.0, -1000000.0, 1000000.0, 10, &newValOk);
           if (!newValOk) {
               return;  // 用户取消输入
           }

           // 正则表达式替换逗号后值
           QString chIndex = QString("CH%1-%2").arg(channel).arg(index);
           QString replacement = QString("%1:%2,%3").arg(chIndex).arg(0.000578427 / voltage, 0, 'f', 10).arg(newValue);

           // 替换配置内容
           content.replace(QRegularExpression(QString("(%1):([0-9\\.]+),([0-9]+)").arg(chIndex)), replacement);

           // 写回配置文件
           if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
               QMessageBox::critical(this, "错误", "无法写入配置文件！");
               return;
           }
           stream << content;
           file.close();

           qDebug() << QString("配置文件已更新（逗号后系数）：%1 -> %2").arg(chIndex).arg(replacement);
           QMessageBox::information(this, "校正完成", "配置文件已成功更新！");
   //planarGraph->autocalculate();
}
   //原在PlanarGraph中执行的版自动校正，已废除
//void PlanarGraph::autocalculate()
//{
//    int channel = 0;  // 用于记录当前数采卡卡号，保持状态
//    int index = 0;    // 用于记录当前通道号，保持状态

//    // 打开配置文件
//    QFile file("../channel_coefficient.ini");
//    if (!file.open(QIODevice::ReadWrite | QIODevice::Text)) {
//        QMessageBox::critical(this, "错误", "无法打开配置文件！");
//        return;
//    }

//    // 读取文件内容
//    QTextStream stream(&file);
//    QString content = stream.readAll();
//    file.close();

//    // 询问是校正逗号前系数还是逗号后系数
//    QMessageBox::StandardButton choice = QMessageBox::question(this, "选择校正项",
//        QString("当前通道: %1-%2\n请选择要校正的系数：\n\n是否校正逗号前系数？")
//        .arg(channel).arg(index),  // 使用 .arg() 来正确替换占位符
//        QMessageBox::Yes | QMessageBox::No);

//    if (choice == QMessageBox::Yes) {
//        // 校正逗号前系数
//        double newCoefficient = 0.000578427 / diffVoltage;  // 使用 diffVoltage 计算新系数
//        qDebug() << QString("计算得到的新逗号前系数: %1").arg(newCoefficient);

//        // 正则表达式替换逗号前系数
//        QString chIndex = QString("CH%1-%2").arg(channel).arg(index);
//        QString replacement = QString("%1:%2,%3").arg(chIndex).arg(newCoefficient, 0, 'f', 10).arg(0);  // 默认逗号后值为0

//        // 替换配置内容
//        content.replace(QRegularExpression(QString("(%1):([0-9\\.]+),([0-9]+)").arg(chIndex)), replacement);

//        // 写回配置文件
//        if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
//            QMessageBox::critical(this, "错误", "无法写入配置文件！");
//            return;
//        }
//        stream << content;
//        file.close();

//        qDebug() << QString("配置文件已更新（逗号前系数）：%1 -> %2").arg(chIndex).arg(replacement);

//        // 等待下一次点击按钮进行校正逗号后系数
//        return;
//    }

//    // 校正逗号后系数
//    bool newValOk;
//    double newValue = QInputDialog::getDouble(this, "输入逗号后值",
//                                              QString("请输入 %1-%2 逗号后的新值：").arg(channel).arg(index),
//                                              0.0, -1000000.0, 1000000.0, 10, &newValOk);
//    if (!newValOk) {
//        return;  // 用户取消输入
//    }

//    // 正则表达式替换逗号后值
//    QString chIndex = QString("CH%1-%2").arg(channel).arg(index);
//    QString replacement = QString("%1:%2,%3").arg(chIndex).arg(0.000578427 / diffVoltage, 0, 'f', 10).arg(newValue);

//    // 替换配置内容
//    content.replace(QRegularExpression(QString("(%1):([0-9\\.]+),([0-9]+)").arg(chIndex)), replacement);

//    // 写回配置文件
//    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
//        QMessageBox::critical(this, "错误", "无法写入配置文件！");
//        return;
//    }
//    stream << content;
//    file.close();

//    qDebug() << QString("配置文件已更新（逗号后系数）：%1 -> %2").arg(chIndex).arg(replacement);

// //    // 增加通道号，每4个通道后切换卡号
// //    index++;
// //    if (index > 3) {
// //        index = 0;   // 通道号从0开始循环
// //        channel++;   // 完成4个通道后，卡号加1
// //    }

//    QMessageBox::information(this, "校正完成", "配置文件已成功更新！");
//}











bool MainView::loadPlugin()
{
    QDir pluginsDir(qApp->applicationDirPath());

#ifdef Q_OS_WIN
    pluginsDir.cd("plugin");
#else
    pluginsDir.cd("release");
    pluginsDir.cd("plugin");
#endif
    foreach (QString fileName, pluginsDir.entryList(QDir::Files)){
        QPluginLoader pluginLoader(pluginsDir.absoluteFilePath(fileName));
        QObject *plugin = pluginLoader.instance();
        if (plugin) {
            _driverPlugin = qobject_cast<DataTransferInterface *>(plugin);
            if (_driverPlugin){
                _driverPlugin->setParent(nullptr);
                _mThread = new QThread(this);
                _driverPlugin->moveToThread(_mThread);
                _mThread->start();
                // 远端端口
                int count =0;
                for ( int i = 0; i < dataInteractionLogicals->size(); i++) {
                   // connect(startBtn, &QPushButton::clicked, dataInteractionLogicals->at(i), &DataInteractionLogical::startudpCollect);
                    connect(dataInteractionLogicals->at(i), &DataInteractionLogical::stopCollect, this, &MainView::statisticalCollectionTimes);
                    connect(dataInteractionLogicals->at(i), &DataInteractionLogical::dataReceivedFromIp, this, &MainView::handleDataReceived);
                    //connect(this, &MainView::hardTriggerStart, dataInteractionLogicals->at(i), &DataInteractionLogical::hardTriggerLogic);
                   // connect(dataInteractionLogicals->at(i), &DataInteractionLogical::hardTriggerStartInformation, this, &MainView::beforeHardTriggerStart);

                    DataInteractionLogical *dataInteractionLogical = dataInteractionLogicals->at(i);
                    int index = dataInteractionLogical->getTempIndex();
                    qDebug() << "当前卡的索引:" << index;
                    if(index > 253)
                    {
                        qWarning() << "意外的索引:" << index;
                    }

                    QString ipAddress = QString("192.168.0.%1").arg(index+1);

                    // switch (index) {
                    //  case 0:
                    //      ipAddress = "192.168.0.1";
                    //      break;
                    //  case 1:
                    //      ipAddress = "192.168.0.2";
                    //      break;
                    //  case 2:
                    //      ipAddress = "192.168.0.3";
                    //      break;
                    //  case 3:
                    //      ipAddress = "192.168.0.4";
                    //      break;
                    //  case 4:
                    //      ipAddress = "192.168.0.5";
                    //      break;
                    //  case 5:
                    //      ipAddress = "192.168.0.6";
                    //      break;
                    //  case 6:
                    //      ipAddress = "192.168.0.7";
                    //      break;
                    //  case 7:
                    //      ipAddress = "192.168.0.8";
                    //      break;
                    //  case 8:
                    //      ipAddress = "192.168.0.9";
                    //      break;
                    //  case 9:
                    //      ipAddress = "192.168.0.10";
                    //      break;
                    //  case 10:
                    //      ipAddress = "192.168.0.11";
                    //      break;
                    //  default:
                    //      // 处理意外的索引
                    //      qWarning() << "意外的索引:" << index;
                    //      continue;
                    //  }

                     // 检查 IP 地址是否存在于线程映射中
                     if (_driverPlugin->threadMap.contains(ipAddress)) {
                         DataProcessor *processor = _driverPlugin->threadMap[ipAddress];

                         // 连接信号到 DataProcessor 的槽函数
                         connect(processor, &DataProcessor::dataTransferSignal, dataInteractionLogical, &DataInteractionLogical::revUdpData);
                        // connect(dataInteractionLogical, &DataInteractionLogical::sendBufferSize, processor, &DataProcessor::allocSize);
                         connect(this, &MainView::sendBufferSize, processor, &DataProcessor::allocSize);
                         count++;
                     } else {
                         // 处理缺失的线程映射条目
                         qDebug() << "线程映射中不包含 IP 地址的条目:" << ipAddress;
                     }
                }
 qDebug() << "count"<<count;
                return true;
            }
        }
    }
}

bool MainView::unloadPlugin()
{
    QDir pluginsDir(qApp->applicationDirPath());
    pluginsDir.cdUp();
    pluginsDir.cd("plugin");
    foreach (QString fileName,pluginsDir.entryList(QDir::Files)){
        QPluginLoader PluginLoader(pluginsDir.absoluteFilePath(fileName));
        PluginLoader.unload();
    }
    _mThread->quit();
    _mThread->wait();
    _driverPlugin->deleteLater();
    return true;
}


// 实现 closeEvent
void MainView::closeEvent(QCloseEvent *event)
{
    // 逻辑：打印日志、弹出确认框等
    qDebug() << "窗口正在关闭...";

    // 可以在这里添加其他操作，例如：
    // QMessageBox::StandardButton reply;
    // reply = QMessageBox::question(this, "退出", "确定要退出吗？",
    //                               QMessageBox::Yes | QMessageBox::No);
    //
    // if (reply == QMessageBox::Yes) {
    //     event->accept();  // 接受关闭事件
    // } else {
    //     event->ignore();  // 忽略关闭事件，窗口不关闭
    // }

    this->disconnectFromHost();

    // 默认接受关闭事件
    event->accept();
}
