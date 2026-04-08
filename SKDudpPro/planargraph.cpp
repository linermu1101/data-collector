#include "planargraph.h"
#include "parameter.h"
#include "mainview.h"
PlanarGraph::PlanarGraph(QString devName,QWidget *parent) : QWidget(parent)
{
    this->setWindowFlags((this->windowFlags()&~Qt::WindowCloseButtonHint)&~Qt::WindowContextHelpButtonHint);
    plot = new QCustomPlot(this);
    plot->setNoAntialiasingOnDrag(true);
    plot->xAxis->setAntialiased(false);
    plot->yAxis->setAntialiased(false);
    plot->setOpenGl(true);
    QHBoxLayout *layout = new QHBoxLayout(this);
    layout->addWidget(plot);
    plot->setBackground(QColor(0,0,0));

    plot->setAutoFillBackground(true);
    plot->setInteractions(QCP::iRangeDrag|QCP::iRangeZoom|QCP::iSelectAxes|QCP::iSelectPlottables|QCP::iSelectItems);
    plot->setNoAntialiasingOnDrag(true);

    this->setProperty("设备:",devName.toUInt());
    plotAddText(devName);

    connect(plot,SIGNAL(selectionChangedByUser()),this,SLOT(selectState()));
    connect(plot,SIGNAL(mousePress(QMouseEvent *)),this,SLOT(dealMousePressEvent(QMouseEvent *)));
    connect(plot->xAxis,SIGNAL(rangeChanged(QCPRange)),this,SLOT(updateAxisRange(QCPRange)));
    connect(plot->yAxis,SIGNAL(rangeChanged(QCPRange)),this,SLOT(updateAxisRange(QCPRange)));

    plot->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(plot,SIGNAL(customContextMenuRequested(const QPoint &)),this,SLOT(ContextMenu(const QPoint &)));

    contextMenu = new QMenu(plot);
    resetRange = new QAction("初始化显示范围",plot);
    maxWidget = new QAction("窗口最大化",plot);
    savePng = new QAction("保存图片",plot);

    contextMenu->addAction(resetRange);
    contextMenu->addAction(maxWidget);
    contextMenu->addAction(savePng);
    QFont newFont("Calibri", 12);
    contextMenu->setFont(newFont);

    connect(resetRange,&QAction::triggered,this,&PlanarGraph::resetPlotRange);
    connect(maxWidget,&QAction::triggered,this,&PlanarGraph::maxPlotWidget);
    connect(savePng,&QAction::triggered,this,&PlanarGraph::savePlot2Png);
}

void PlanarGraph::plot_2D_Init(QColor bkColor,uint8_t num,QList<QColor> &pColor)
{
    tracerEnable = false;  //游标默认不跟随曲线

    plot->setAutoFillBackground(true);
    plot->setInteractions(QCP::iRangeDrag|QCP::iRangeZoom|QCP::iSelectAxes|QCP::iSelectPlottables|QCP::iSelectLegend);
    plot->setNoAntialiasingOnDrag(true);
    plot->setBackground(bkColor);
    plot->setBackgroundScaled(true);
    plot->setBackgroundScaledMode(Qt::IgnoreAspectRatio);

    cList = pColor;

    QFont labelfont;
    labelfont.setFamily("Calibri");
    labelfont.setPixelSize(14);

    QPen axisPen,tickerPen,gridPen;
    axisPen.setColor(QColor(0,0,0));
    axisPen.setWidthF(1.5);
    tickerPen.setColor(Qt::green);
    tickerPen.setWidthF(1.5);
    gridPen.setColor(Qt::green);
    gridPen.setWidthF(0.8);
    gridPen.setStyle(Qt::SolidLine);
    //x轴
    plot->xAxis->setTickLabelFont(font());
    plot->xAxis->setLabelFont(labelfont);
    plot->xAxis->setTickLabelColor(Qt::white);
    plot->xAxis->setRange(0,4000);
    plot->xAxis->setTicks(true);
    plot->xAxis->setTickLabels(true);
    plot->xAxis->setSubTicks(true);
    plot->xAxis->setBasePen(axisPen);
    plot->xAxis->ticker()->setTickCount(10);
    plot->xAxis->setTickPen(tickerPen);
    plot->xAxis->setSubTickPen(tickerPen);
    plot->xAxis->grid()->setVisible(true);
    plot->xAxis->grid()->setPen(gridPen);
    plot->xAxis->grid()->setSubGridPen(gridPen);
    //y轴
    plot->yAxis->setTickLabelColor(Qt::white);
    plot->yAxis->setTickLabelFont(font());
    plot->yAxis->setLabelFont(labelfont);
    plot->yAxis->setRange(0,100);
    plot->yAxis->setTicks(true);
    plot->yAxis->setTickLabels(true);
    plot->yAxis->setSubTicks(true);
    plot->yAxis->setBasePen(axisPen);
    plot->yAxis->ticker()->setTickCount(8);
    plot->yAxis->setTickPen(tickerPen);
    plot->yAxis->setSubTickPen(tickerPen);
    plot->yAxis->grid()->setVisible(true);
    plot->yAxis->grid()->setPen(gridPen);
    plot->yAxis->grid()->setSubGridPen(gridPen);

//    添加曲线
//    for(int i=0; i<num; i++){
//        addCurve(pColor[i],i);
//     }

//    labelfont.setPixelSize(10);
//    if(num > 1){
//        plot->legend->setVisible(true);
//        plot->legend->setFont(labelfont);
//        plot->legend->setSelectedParts(QCPLegend::spNone);
//        plot->legend->setBorderPen(QPen(Qt::black));
//        plot->legend->setTextColor(Qt::black);
//        plot->legend->setBrush(QBrush(QColor(230,230,230,150)));
//        connect(plot, SIGNAL(legendDoubleClick(QCPLegend*,QCPAbstractLegendItem*,QMouseEvent*)),
//                this, SLOT(legendDoubleClick(QCPLegend*,QCPAbstractLegendItem*)));
//    }

    // 游标参数初始化
    tracer = new QCPItemTracer(plot);
    tracer->setPen(QPen(Qt::white));
    tracer->setBrush(QBrush(Qt::white));
    tracer->setStyle(QCPItemTracer::tsCrosshair);
    tracer->setSize(5);
    tracer->setSelectable(false);


    tracerLabel = new QCPItemText(plot);
    tracerLabel->setLayer("overlay");
    tracerLabel->setPen(QPen(Qt::white));
    tracerLabel->setColor(QColor(Qt::white));

    tracerLabel->setPositionAlignment(Qt::AlignLeft | Qt::AlignBottom);
    tracerLabel->position->setParentAnchor(tracer->position);



    connect(plot,SIGNAL(mouseMove(QMouseEvent*)),this,SLOT(mouseMove1(QMouseEvent*)));

    connect(plot, &QCustomPlot::selectionChangedByUser, this, &PlanarGraph::selectionChange);

    plot->replot(QCustomPlot::rpImmediateRefresh);


}
// 获取 diffVoltage
double PlanarGraph::getDiffVoltage() const {
    return diffVoltage;
}

void PlanarGraph::plot_2D_Init0(QColor bkColor, uint8_t num, QList<QColor> &pColor)
{

    cList = pColor;

//    添加曲线
    for(int i=0; i<num; i++){
        addCurve(pColor[i],i);
     }


    plot->replot(QCustomPlot::rpImmediateRefresh);
}

void PlanarGraph::addCurve(QColor color,int i)
{
    plot->addGraph();

    QPen pen;
    if(plot->openGl())
        pen.setWidth(2);
    else
        pen.setWidth(1);

    pen.setColor(color);

    if(i<10){
        pen.setStyle(Qt::SolidLine);
    }else if(i<20){
        pen.setStyle(Qt::DashLine);
    }else if(i<30){
        pen.setStyle(Qt::DotLine);
    }else if(i<40){
        pen.setStyle(Qt::DashDotLine);
    }else{
        pen.setStyle(Qt::CustomDashLine);
        QVector<qreal>dashes;
        dashes<<4<<2<<1<<3;
        pen.setDashPattern(dashes);
    }
    plot->graph()->setPen(pen);
//    switch (i) {
//    case 0:plot->graph(0)->setName("通道 0");
//        break;
//    case 1:plot->graph(1)->setName("通道 1");
//        break;
//    case 2:plot->graph(2)->setName("通道 2");
//        break;
//    case 3:plot->graph(3)->setName("通道 3");
//        break;
//    case 4:plot->graph(4)->setName("通道 4");
//        break;
//    }

    plot->graph()->setSelectable(QCP::stNone);
    plot->replot(QCustomPlot::rpQueuedRefresh);
}

void PlanarGraph::legendDoubleClick(QCPLegend *legend, QCPAbstractLegendItem *item)
{
    Q_UNUSED(legend);

    for(int i = 0;i<plot->graphCount();i++)
    {
        if(plot->legend->itemWithPlottable(plot->graph(i))==item)
        {
            QPen pen;
            if(plot->graph(i)->visible())
            {
                pen.setColor(QColor("black"));
                plot->graph(i)->setVisible(false);
            }
            else
            {
                pen.setColor(cList[i]);
                plot->graph(i)->setVisible(true);
            }
            plot->graph(i)->setPen(pen);
            plot->replot(QCustomPlot::rpQueuedRefresh);
            return;
        }
    }
}

QSharedPointer<QCPGraphDataContainer> PlanarGraph::resetCurve(QVector<double> &x,QVector<double> &y,uint8_t index)
{
    plot->graph(index)->setData(x,y);
    plot->replot();
    return plot->graph(index)->data();
}

//QSharedPointer<QCPGraphDataContainer> PlanarGraph::resetCurve(QVector<double> &x,QVector<double> &y,bool m_flag)
//{
//    if(m_flag)
//        plot->graph()->setData(x,y);
//    return plot->graph()->data();
//}

void PlanarGraph::selectState()
{

    if(plot->xAxis->selectedParts() == QCPAxis::spAxis || plot->xAxis->selectedParts() == QCPAxis::spTickLabels){
        plot->xAxis->setSelectedParts(QCPAxis::spAxis|QCPAxis::spTickLabels);
        QList<QCPAxis*> axis;
        axis<<plot->xAxis;
        plot->axisRect()->setRangeZoomAxes(axis);
    }else if(plot->yAxis->selectedParts() == QCPAxis::spAxis || plot->yAxis->selectedParts() == QCPAxis::spTickLabels){
        plot->yAxis->setSelectedParts(QCPAxis::spAxis|QCPAxis::spTickLabels);
        QList<QCPAxis*> axis;
        axis<<plot->yAxis;
        plot->axisRect()->setRangeZoomAxes(axis);
    }else{
        QList<QCPAxis*> axis;
        axis<<plot->xAxis<<plot->yAxis;
        plot->axisRect()->setRangeZoomAxes(axis);
    }

}

void PlanarGraph::updateAxisRange(QCPRange range)
{
//    Q_UNUSED(range);
     QSettings collectSettings("parameterSKDFDA", "value");
    int PretriggerTime = collectSettings.value("PretriggerTime", 0).toInt();
    int PosttriggerTime = collectSettings.value("PosttriggerTime", 0).toInt();
    int LowerTime = 0;
    if (PretriggerTime != 0 && PosttriggerTime != 0) {
        LowerTime = 1;
    } else if (PretriggerTime != 0) {
         LowerTime = -PretriggerTime;
    } else if (PosttriggerTime != 0) {
        LowerTime = PosttriggerTime;
    } else {
        LowerTime = 0;
    }
    if(plot->xAxis->range().lower < LowerTime){
        plot->xAxis->setRangeLower(LowerTime);
    }
//    if(plot->yAxis->range().lower < 0){
//        plot->yAxis->setRangeLower(0);
//    }
}

/*
 * 左键点击事件：去判断在坐标轴界内，发射信号，打印日志
*/
void PlanarGraph::dealMousePressEvent(QMouseEvent *event)
{

    if(event->button() == Qt::RightButton){
         plot->setSelectionRectMode(QCP::SelectionRectMode::srmNone);
    }else if(event->button() == Qt::MiddleButton){
        plot->selectionRect()->setBrush(QColor(200,200,200,50));
        plot->setSelectionRectMode(QCP::SelectionRectMode::srmZoom);
        plot->axisRect()->setRangeZoomFactor(0.5);
        plot->axisRect()->setRangeZoomAxes(plot->xAxis,plot->yAxis);
    }else{
        plot->setSelectionRectMode(QCP::SelectionRectMode::srmNone);
        // Get the range of the x axis
        QCPRange xRange = plot->xAxis->range();
        double xLower = xRange.lower; // The lower limit of the x axis
        double xUpper = xRange.upper; // The upper limit of the x axis

        // Get the range of the y axis
        QCPRange yRange = plot->yAxis->range();
        double yLower = yRange.lower; // The lower limit of the y axis
        double yUpper = yRange.upper; // The upper limit of the y axis
        double time = plot->xAxis->pixelToCoord(event->pos().x());
        double voltage = plot->yAxis->pixelToCoord(event->pos().y());

        if (time>=xLower && time<=xUpper && voltage>=yLower && voltage<=yUpper){
//        qDebug()<<event->pos().x()<<event->pos().y();
            QString logs = QString("  Time = %1us, Voltage = %2V").arg(time).arg(voltage);

        
            emit putLog(logs); //传出打印日志信号
            logCount++;
            if (logCount % 2 == 0){
                double diffTime = abs(time - lastTime);
                diffVoltage = abs(voltage - lastVoltage);
                QString logs = QString("  diffTime = %1us, diffVoltage = %2V \n====================").arg(diffTime).arg(diffVoltage);
                emit putLog(logs); //传出打印日志信号
            }
            lastTime = time;
            lastVoltage = voltage;
        }
    }

}

void PlanarGraph::update_bkColor(QColor bkColor)
{
    plot->setBackground(bkColor);
    plot->replot(QCustomPlot::rpImmediateRefresh);
}

void PlanarGraph::updateGraphData(bool first_flag)
{
    xNumber.resize(ADDRESS);
    for(int i=0; i<ADDRESS; i++){
        xNumber[i] = i;
    }
    dataNumber.resize(ADDRESS);
    ch_graph = resetCurve(xNumber,dataNumber,first_flag);
}

void PlanarGraph::changeGraphName(int index, QString name)
{
    plot->graph(index)->setName(name);
}

void PlanarGraph::plotAddText(QString m_devName)
{
    plot->plotLayout()->insertRow(0);
    devText   = new QCPTextElement(plot);
//    devText->setMarginGroup();

    devText->setText(m_devName);

    devText->setTextFlags(Qt::AlignHCenter|Qt::AlignVCenter);

    devText->setFont(QFont("Times New Roman",16));

    QCPLayoutGrid *m_layout = new QCPLayoutGrid;
    m_layout->addElement(0,0,new QCPTextElement(plot,""));
    m_layout->addElement(0,1,devText);
    m_layout->addElement(0,2,new QCPTextElement(plot,""));
    m_layout->setMargins(QMargins(0,0,0,0));

    plot->plotLayout()->addElement(0,0,m_layout);
    plot->plotLayout()->setRowSpacing(0);
}

void PlanarGraph::ContextMenu(const QPoint &pos)
{
    Q_UNUSED(pos);
    contextMenu->exec(QCursor::pos());
}

/*
 * 先去获取范围，把游标限定于界内，避免与坐标轴重叠，导致坐标轴功能失效
*/
void PlanarGraph::mouseMove1(QMouseEvent *e)
{
    // Get the range of the x axis
    QCPRange xRange = plot->xAxis->range();
    double xLower = xRange.lower; // The lower limit of the x axis
    double xUpper = xRange.upper; // The upper limit of the x axis

    // Get the range of the y axis
    QCPRange yRange = plot->yAxis->range();
    double yLower = yRange.lower; // The lower limit of the y axis
    double yUpper = yRange.upper; // The upper limit of the y axis
    double time = plot->xAxis->pixelToCoord(e->pos().x());
    double voltage = plot->yAxis->pixelToCoord(e->pos().y());
    if (time>=xLower && time<=xUpper && voltage>=yLower && voltage<=yUpper){
        tracer->position->setCoords(time, voltage);
        //设置游标说明（tracerLabel）的内容
        tracerLabel->setText(QString("  Time = %1us, Voltage = %2V").arg(time).arg(voltage));
        plot->replot();
    }

}

// 吸附于某条曲线
void PlanarGraph::selectionChange()
{
    for (int i = 0; i < plot->graphCount(); ++i)
    {
        QCPGraph *graph = plot->graph(i);
        QCPPlottableLegendItem *item = plot->legend->itemWithPlottable(graph);
        if (item->selected() || graph->selected())//选中了哪条曲线或者曲线的图例
        {
            tracerGraph = graph;
            tracerEnable = true;
            qDebug()<<"曲线选择："<<i;
        }
    }

}

void PlanarGraph::resetPlotRange()
{
    plot->xAxis->setRange(Parameter::plot2DXStart, Parameter::plot2DXFinal);
    plot->yAxis->setRange(Parameter::plot2DYStart, Parameter::plot2DYFinal);
    plot->replot();
}

void PlanarGraph::maxPlotWidget()
{
    UINT8 index = this->property("设备:").toUInt();

    if(maxWidget->text() == "窗口最大化")
    {
        maxWidget->setText("窗口大小还原");
        plot->yAxis->ticker()->setTickCount(10);
        emit windowFullorRestoreSize(index,true);
    }
    else
    {
        maxWidget->setText("窗口最大化");
        plot->yAxis->ticker()->setTickCount(5);
        emit windowFullorRestoreSize(index,false);
    }
    plot->replot(QCustomPlot::rpImmediateRefresh);
}

void PlanarGraph::savePlot2Png()
{
    QString path =QFileDialog::getSaveFileName(this,"保存图片","../","PNG(*.png);;JPG(*.jpg)");
    plot->savePng(path,800,600);
}


/*********************深度图***********************/
void PlanarGraph::depth_Plot_Init(QColor bkColor,QColor axisColor)
{
    plot->setAutoFillBackground(true);
    plot->setNoAntialiasingOnDrag(true);//关闭拖动期间的抗锯齿
    plot->setBackground(bkColor);
    plot->setBackgroundScaled(true);
    plot->setBackgroundScaledMode(Qt::IgnoreAspectRatio);

    QFont font("Times New Roman",11);

    plot->setInteractions(QCP::iRangeDrag|QCP::iRangeZoom);
    plot->setOpenGl(false);
    plot->axisRect()->setupFullAxesBox(true);
    plot->xAxis->setLabel("ADC Channel");
    plot->yAxis->setLabel("PSD Ratio");
    plot->xAxis->setLabelFont(font);
    plot->yAxis->setLabelFont(font);

    plot->xAxis->ticker()->setTickCount(10);
    plot->yAxis->ticker()->setTickCount(10);

    QPen axisPen,tickerPen,gridPen;
    axisPen.setColor(axisColor);
    axisPen.setWidthF(1.5);
    tickerPen.setColor(axisColor);
    tickerPen.setWidthF(1.5);
    gridPen.setColor(axisColor);
    gridPen.setWidthF(1);
    gridPen.setStyle(Qt::DashLine);

    //x轴
    plot->xAxis->setLabelColor(axisColor);
    plot->xAxis->setTickLabelColor(axisColor);
    plot->xAxis->setTicks(true);
    plot->xAxis->setTickLabels(true);
    plot->xAxis->setSubTicks(true);
    plot->xAxis->setBasePen(axisPen);
    plot->xAxis->setTickPen(tickerPen);
    plot->xAxis->setSubTickPen(tickerPen);
    plot->xAxis->grid()->setVisible(true);
    plot->xAxis->grid()->setPen(gridPen);
    plot->xAxis->grid()->setSubGridPen(gridPen);

    plot->xAxis2->setBasePen(axisPen);
    plot->xAxis2->setTickPen(tickerPen);
    plot->xAxis2->setSubTickPen(tickerPen);
    plot->xAxis2->grid()->setPen(gridPen);
    plot->xAxis2->grid()->setSubGridPen(gridPen);

    //y轴
    plot->yAxis->setLabelColor(axisColor);
    plot->yAxis->setTickLabelColor(axisColor);
    plot->yAxis->setTicks(true);
    plot->yAxis->setTickLabels(true);
    plot->yAxis->setSubTicks(true);
    plot->yAxis->setBasePen(axisPen);
    plot->yAxis->setTickPen(tickerPen);
    plot->yAxis->setSubTickPen(tickerPen);
    plot->yAxis->grid()->setVisible(true);
    plot->yAxis->grid()->setPen(gridPen);
    plot->yAxis->grid()->setSubGridPen(gridPen);

    plot->yAxis2->setBasePen(axisPen);
    plot->yAxis2->setTickPen(tickerPen);
    plot->yAxis2->setSubTickPen(tickerPen);
    plot->yAxis2->grid()->setPen(gridPen);
    plot->yAxis2->grid()->setSubGridPen(gridPen);


    //初始化画布,设置点为1024*1000
    colorMap = new QCPColorMap(plot->xAxis,plot->yAxis);
    colorMap->data()->setSize(NX,NY);
    colorMap->data()->setRange(QCPRange(0,1024),QCPRange(0,1000));

    //初始化色级
    QCPColorScale *scale = new QCPColorScale(plot);
    plot->plotLayout()->addElement(1,1,scale);
    scale->setType(QCPAxis::atRight);
    scale->setDataRange(QCPRange(0,350));
    scale->setRangeDrag(false);
    scale->setRangeZoom(false);
    scale->axis()->ticker()->setTickCount(6);
    scale->axis()->setTickLabelColor(axisColor);
    colorMap->setColorScale(scale);

    //初始化色梯
    QCPColorGradient *gradient = new QCPColorGradient();
    gradient->setColorStopAt(0.15,QColor(0,100,255));
    gradient->setColorStopAt(0.3,QColor(0,255,255));
    gradient->setColorStopAt(0.45,QColor(0,255,120));
    gradient->setColorStopAt(0.6,QColor(255,255,0));
    gradient->setColorStopAt(0.75,QColor(255,140,0));
    gradient->setColorStopAt(0.9,QColor(200,20,0));
    gradient->setColorStopAt(1,QColor(130,13,0));
    colorMap->setGradient(*gradient);
    colorMap->rescaleDataRange(true);

    //设置色级显示的高度大小
    QCPMarginGroup *marginGroup = new QCPMarginGroup(plot);
    plot->axisRect()->setMarginGroup(QCP::msBottom|QCP::msTop, marginGroup);
    scale->setMarginGroup(QCP::msBottom|QCP::msTop, marginGroup);
    plot->rescaleAxes();

    //初始化曲线界面
//    QVector<QVector<DWORD32>> Psd_data;
//    Psd_data.resize(NX);
//    for(int i=0; i<NX; i++){
//        Psd_data[i].resize(NY);
//    }
    for(int i=0;i<NX;i++)
    {
        for(int j=0; j<NY; j++)
        {
//              Psd_data[i][j] = 0;
              colorMap->data()->setAlpha(i,j,0);
        }
    }
    plot->replot(QCustomPlot::rpQueuedRefresh);
}

void PlanarGraph::autoFind()
{
    plot->xAxis->setRange(Parameter::plot2DXStart, Parameter::plot2DXFinal);
    plot->yAxis->setRange(Parameter::plot2DYStart, Parameter::plot2DYFinal);
    plot->replot();
//    qDebug() << "自动寻找完成" << endl;
}
