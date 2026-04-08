#ifndef PLANARGRAPH_H
#define PLANARGRAPH_H

#include <QObject>
#include <QElapsedTimer>
#include "qcustomplot.h"
//#include "mainview.h"

#include "acquisitioncard.h"
#define ADDRESS 4000
#define NX 1025
#define NY 1001

//2D平面图表
class PlanarGraph : public QWidget
{
    Q_OBJECT
public:
    explicit PlanarGraph(QString devName,QWidget *parent = nullptr);


    QCustomPlot *plot;
    QCPItemTracer* tracer;
    QCPItemText* tracerLabel;
    QCPGraph *tracerGraph;      //吸附曲线
    bool tracerEnable;          //是否吸附
    QCPColorMap *colorMap;
    QVector<double> ch_xData,ch_yData;
    QCPTextElement *devText;

    QVector<double> xNumber,dataNumber;
    QSharedPointer<QCPGraphDataContainer> ch_graph;

    void plot_2D_Init(QColor bkColor,uint8_t num,QList<QColor> &pColor);
    void plot_2D_Init0(QColor bkColor,uint8_t num,QList<QColor> &pColor);
    void depth_Plot_Init(QColor bkColor,QColor axisColor);
    void update_bkColor(QColor bkColor);
    void updateGraphData(bool first_flag);
    void changeGraphName(int index, QString name);
    void autoFind();
    void autocalculate();
    double getDiffVoltage() const;//这个这个放public，但是所取的返回值需要放在private中，否则会闪退

    QSharedPointer<QCPGraphDataContainer> resetCurve(QVector<double> &x,QVector<double> &y,uint8_t index);
//    QSharedPointer<QCPGraphDataContainer> resetCurve(QVector<double> &x,QVector<double> &y,bool m_flag);


private slots:
    void selectState();
    void updateAxisRange(QCPRange range);
    void dealMousePressEvent(QMouseEvent *event);
    void legendDoubleClick(QCPLegend *legend, QCPAbstractLegendItem *item);

    void ContextMenu(const QPoint &pos);
    void mouseMove1(QMouseEvent *e);
    void selectionChange();

    void resetPlotRange();
    void maxPlotWidget();
    void savePlot2Png();


signals:
    void windowFullorRestoreSize(int devIndex,bool full);
    void putLog(QString logs);

private:
    QList<QColor> cList;

    QMenu *contextMenu;
    QAction *resetRange;
    QAction *maxWidget;
    QAction *savePng;
    QRubberBand* rubberBand;
    QPoint rubberOrigin;
    QTimer* dataTimer;
    QTimer* cusTimer;
    AcquisitionCard* acquisitionCard;
    QList<QPushButton*> cardButtons;
    QList<QPushButton*> channelButtons;
    QPushButton* channelBtn[78];

    double diffVoltage;  // 添加成员变量
    double lastTime;
    double lastVoltage;
    int logCount;
    int xCount;
    bool chooseFlag;
    void shizixian();
    void addCurve(QColor color,int i);

    void plotAddText(QString m_devName);





//    void Equipment(QString Equipment_number);
};

#endif // PLANARGRAPH_H
