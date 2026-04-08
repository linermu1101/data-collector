#ifndef PLANARGRAPH_H
#define PLANARGRAPH_H

#include <QObject>
#include <QElapsedTimer>
#include "qcustomplot.h"
#include "../parameter.h"

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
    QCPColorMap *colorMap;
    QVector<double> ch_xData,ch_yData;
    QCPTextElement *devText;

    QVector<double> xNumber,dataNumber;
    QSharedPointer<QCPGraphDataContainer> ch_graph;

    void plot_2D_Init(QColor bkColor,uint8_t num,QList<QColor> &pColor);
    void depth_Plot_Init(QColor bkColor,QColor axisColor);
    void update_bkColor(QColor bkColor);
    void updateGraphData(bool first_flag);
    void changeGraphName(int index, QString name);
    QSharedPointer<QCPGraphDataContainer> resetCurve(QVector<double> &x,QVector<double> &y,uint8_t index);
//    QSharedPointer<QCPGraphDataContainer> resetCurve(QVector<double> &x,QVector<double> &y,bool m_flag);


private slots:
    void selectState();
    void updateAxisRange(QCPRange range);
    void dealMousePressEvent(QMouseEvent *event);
    void legendDoubleClick(QCPLegend *legend, QCPAbstractLegendItem *item);

    void ContextMenu(const QPoint &pos);

    void resetPlotRange();
    void maxPlotWidget();
    void savePlot2Png();

signals:
    void windowFullorRestoreSize(int devIndex,bool full);

private:
    QList<QColor> cList;

    QMenu *contextMenu;
    QAction *resetRange;
    QAction *maxWidget;
    QAction *savePng;

    void addCurve(QColor color,int i);

    void plotAddText(QString m_devName);


//    void Equipment(QString Equipment_number);
};

#endif // PLANARGRAPH_H
