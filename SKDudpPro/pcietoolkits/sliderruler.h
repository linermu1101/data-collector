#ifndef SLIDERSliderRuler_H
#define SLIDERSliderRuler_H

#include <QWidget>
#include <QGraphicsLineItem>
#include <QtCharts/QChartView>
#include <QObject>
#include "math.h"

class SliderRuler : public QtCharts::QChartView
{
	Q_OBJECT
public:
    explicit SliderRuler(QWidget *parent = 0);
//    virtual ~SliderRuler();
protected:
    void resizeEvent(QResizeEvent *);
    virtual void mousePressEvent(QMouseEvent *pEvent) override;
    virtual void mouseReleaseEvent(QMouseEvent *pEvent) override;
    virtual void mouseMoveEvent(QMouseEvent *e) override;
    virtual void wheelEvent(QWheelEvent *pEvent) override;
    virtual void enterEvent(QEvent *pEvent) override;
    virtual void leaveEvent(QEvent *pEvent) override;
    void paintEvent(QPaintEvent *);
	void drawBg(QPainter *painter);
	void drawRule(QPainter *painter);
	void drawSlider(QPainter *painter);
	void drawTip(QPainter *painter);

private:
    int sliderCount;            //滑块总数
    QList<double> valueList;               //目标值
    QList<double> currentValueList;        //当前值
	double maxValue;            //最小值
	double minValue;            //最大值
	int precision;              //精确度,小数点后几位
    double longStep;               //长线条等分步长
    double shortStep;              //短线条等分步长
	int space;                  //间距

    bool mouseMiddleButtonPressed;
    QPoint mousePrePos;
    QGraphicsLineItem* x_line;
    QGraphicsLineItem* y_line;
    int mouseOldPosOfX = 0;

	QColor bgColorStart;        //背景渐变开始颜色
	QColor bgColorEnd;          //背景渐变结束颜色
	QColor lineColor;           //线条颜色

//	QColor sliderColorTop;      //滑块上部颜色
//	QColor sliderColorBottom;   //滑块下部颜色

    QList<QColor> sliderColorTopList;      //滑块上部颜色
    QList<QColor> sliderColorBottomList;   //滑块下部颜色

	QColor tipBgColor;          //当前值背景色
	QColor tipTextColor;        //当前值文字颜色

	bool pressed;               //是否鼠标按下

	double longLineHeight;      //长刻度高度
	double shortLineHeight;     //短刻度高度
	double sliderTopHeight;     //滑块上部高度
	double sliderBottomHeight;  //滑块底部高度

//	QPointF sliderLastPot;      //滑块最后的坐标
//	QPointF sliderTopPot;       //滑块顶部坐标
//	QPointF sliderLeftPot;      //滑块左边坐标
//	QPointF sliderRightPot;     //滑块右边坐标

    QList<QPointF> sliderLastPotList;       //滑块最后的坐标
    QList<QPointF> sliderTopPotList;        //滑块顶部坐标
    QList<QPointF> sliderLeftPotList;       //滑块左边坐标
    QList<QPointF> sliderRightPotList;      //滑块右边坐标

	QRectF sliderRect;          //滑块矩形区域
	QRectF tipRect;             //提示信息矩形区域
	QPointF lineLeftPot;        //主线左边坐标
	QPointF lineRightPot;       //主线右边坐标

private slots:
	void resetVariables();

public:
    int getSliderCount() const
    {
        return sliderCount;
    }
    double getValue(int index) const
	{
        return valueList[index];
	}
	double getMinValue() const
	{
		return minValue;
	}
	double getMaxValue() const
	{
		return maxValue;
	}
	int getPrecision() const
	{
		return precision;
	}
	int getLongStep() const
	{
		return longStep;
	}
	int getShortStep() const
	{
		return shortStep;
	}
	int getSpace() const
	{
		return space;
	}
	QColor getBgColorStart()const
	{
		return bgColorStart;
	}
	QColor getBgColorEnd()const
	{
		return bgColorEnd;
	}
	QColor getLineColor()const
	{
		return lineColor;
	}

    QColor getSliderColorTop(int index)const
	{
        return sliderColorTopList[index];
	}
    QColor getSliderColorBottom(int index)const
	{
        return sliderColorBottomList[index];
	}

	QColor getTipBgColor()const
	{
		return tipBgColor;
	}
	QColor getTipTextColor()const
	{
		return tipTextColor;
	}
public slots:
	//设置最大最小值-范围值
	void setRange(double minValue, double maxValue);
	void setRange(int minValue, int maxValue);

	//设置目标值
    void setValue(int index,double value);
    void setValue(int index,int value);

	//设置精确度
	void setPrecision(int precision);
	//设置线条等分步长
    void setStep(double longStep, double shortStep);
	//设置间距
	void setSpace(int space);

	//设置背景颜色
	void setBgColor(QColor bgColorStart, QColor bgColorEnd);
	//设置线条颜色
	void setLineColor(QColor lineColor);

    //添加滑块
    void addSlider(QColor color);

	//设置滑块颜色
    void setSliderColor(int index, QColor sliderColorTop, QColor sliderColorBottom);
	//设置提示信息背景
	void setTipBgColor(QColor tipBgColor, QColor tipTextColor);

signals:
    void valueChanged(int index,double value);
    void sendXpos(int nPos);
};

#endif // SLIDERSliderRuler_H
