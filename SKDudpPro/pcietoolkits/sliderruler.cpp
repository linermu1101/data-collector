#include "sliderruler.h"
#include "qpainter.h"
#include "qevent.h"
#include "qtimer.h"
#include "qdebug.h"

SliderRuler::SliderRuler(QWidget *parent) :
    QtCharts::QChartView(parent),
    mouseMiddleButtonPressed(false),
    mousePrePos(0,0)
{
    x_line = new QGraphicsLineItem();
    x_line->setPen(QPen(Qt::green));
    x_line->setZValue(2);
    y_line = new QGraphicsLineItem();
    y_line->setPen(QPen(Qt::green));
    y_line->setZValue(2);

    this->scene()->addItem(x_line);
    this->scene()->addItem(y_line);
//	value = 0.0;
    sliderCount=0;
	minValue = 0.0;
	maxValue = 100.0;

	precision = 0;
	longStep = 10;
	shortStep = 1;
	space = 20;

	bgColorStart = QColor(100, 100, 100);
	bgColorEnd = QColor(60, 60, 60);
    lineColor = QColor(0, 0, 0);

//	sliderColorTop = QColor(100, 184, 255);
//	sliderColorBottom = QColor(235, 235, 235);

	tipBgColor = QColor(255, 255, 255);
	tipTextColor = QColor(50, 50, 50);

	pressed = false;
//	currentValue = 0;
//	sliderLastPot = QPointF(space, longLineHeight / 2);

    this->setMinimumHeight(25);
}

void SliderRuler::resizeEvent(QResizeEvent *)
{
    resetVariables();
    //    setValue(currentValue);
}

void SliderRuler::mousePressEvent(QMouseEvent *pEvent)
{
    if (pEvent->button() == Qt::MiddleButton)
    {
        mouseMiddleButtonPressed = true;
        mousePrePos = pEvent->pos();
        this->setCursor(Qt::OpenHandCursor);
    }
}

void SliderRuler::mouseReleaseEvent(QMouseEvent *pEvent)
{
    if (pEvent->button() == Qt::MiddleButton)
    {
        mouseMiddleButtonPressed = false;
        this->setCursor(Qt::ArrowCursor);
    }

}

void SliderRuler::mouseMoveEvent(QMouseEvent *e)
{
    qDebug()<< this->chart()->mapToValue(e->pos());//把鼠标坐标值转化为画出的图中的坐标
    //以防父类的实例接收不到mouseMoveEvent事件。
    QChartView::mouseMoveEvent(e);//调用父类的重写方法。本行程序不同于e->ignore()，ignor是把事件继续向父控件（一般其父控件就是ui界面）传递，而不是向父类传递
    x_line->setLine(e->x(),0,e->x(),this->height());
    y_line->setLine(0,e->y(),this->width(),e->y());

}

void SliderRuler::wheelEvent(QWheelEvent *pEvent)
{
    qreal rVal = std::pow(0.999, pEvent->delta()); // 设置比例
    // 1. 读取视图基本信息
    QRectF oPlotAreaRect = this->chart()->plotArea();
    QPointF oCenterPoint = oPlotAreaRect.center();
    // 2. 水平调整
    oPlotAreaRect.setWidth(oPlotAreaRect.width() * rVal);
    // 3. 竖直调整
    oPlotAreaRect.setHeight(oPlotAreaRect.height() * rVal);

    QPointF oNewCenterPoint(2 * oCenterPoint - pEvent->pos() - (oCenterPoint - pEvent->pos()) / rVal);
    // 5. 设置视点
    oPlotAreaRect.moveCenter(oNewCenterPoint);
    // 6. 提交缩放调整
    this->chart()->zoomIn(oPlotAreaRect);

}

void SliderRuler::enterEvent(QEvent *pEvent)
{
    x_line->setVisible(true);
    y_line->setVisible(true);

}

void SliderRuler::leaveEvent(QEvent *pEvent)
{
    x_line->setVisible(false);
    y_line->setVisible(false);

}


void SliderRuler::paintEvent(QPaintEvent *)
{
	//绘制准备工作,启用反锯齿
	QPainter painter(this);
	painter.setRenderHint(QPainter::Antialiasing);

	//绘制背景
	drawBg(&painter);
	//绘制标尺
	drawRule(&painter);
	//绘制滑块
    drawSlider(&painter);
	//绘制当前值的提示
//	drawTip(&painter);
}

void SliderRuler::drawBg(QPainter *painter)
{
	painter->save();
	painter->setPen(Qt::NoPen);
	QLinearGradient bgGradient(0, 0, 0, height());
	bgGradient.setColorAt(0.0, bgColorStart);
	bgGradient.setColorAt(1.0, bgColorEnd);
	painter->setBrush(bgGradient);
	painter->drawRect(rect());
	painter->restore();
}

void SliderRuler::drawRule(QPainter *painter)
{
	painter->save();
    painter->setPen(lineColor);

	//绘制横向标尺底部线,居中	
    double initX = space;
    double initY = (double)height() / 2;
    lineLeftPot = QPointF(initX, initY);
    lineRightPot = QPointF(width() - initX, initY);
	painter->drawLine(lineLeftPot, lineRightPot);
	//绘制纵向标尺刻度	
    double length = width() - 2 * space;
    //计算每一格移动多少
    double increment = length / (maxValue - minValue);
	//根据范围值绘制刻度值及刻度值
    for (double i = minValue; i <= maxValue; i = i + shortStep) {
        if ((int)((i+0.00001)*10)%(int)(longStep*10) == 0) {
            QPointF topPot(initX, initY - longLineHeight);
            QPointF bottomPot(initX, initY);
			painter->drawLine(topPot, bottomPot);

            QString strValue = QString("%1").arg((double)i, 0, 'f', precision);
			double textWidth = fontMetrics().width(strValue);
			double textHeight = fontMetrics().height();
            QPointF textPot(initX - textWidth / 2, initY + textHeight);
			painter->drawText(textPot, strValue);
		} else {
            QPointF topPot(initX, initY - shortLineHeight);
            QPointF bottomPot(initX, initY);
			painter->drawLine(topPot, bottomPot);
		}

        initX += increment * shortStep;
	}
	painter->restore();
}

void SliderRuler::drawSlider(QPainter *painter)
{
    painter->save();
    for(int i=0; i<sliderCount; i++)
    {
        //绘制滑块上部分三角形
        sliderTopPotList[i] = QPointF(sliderLastPotList[i].x(), lineLeftPot.y() - longLineHeight / 4);
        sliderLeftPotList[i] = QPointF(sliderTopPotList[i].x() - width() / 150, sliderTopPotList[i].y() - sliderTopHeight);
        sliderRightPotList[i] = QPointF(sliderTopPotList[i].x() + width() / 150, sliderTopPotList[i].y() - sliderTopHeight);
        painter->setPen(sliderColorTopList[i]);
        painter->setBrush(sliderColorTopList[i]);

        QVector<QPointF> potVec;
        potVec.append(sliderTopPotList[i]);
        potVec.append(sliderLeftPotList[i]);
        potVec.append(sliderRightPotList[i]);
        painter->drawPolygon(potVec);

        //绘制滑块下部分矩形
        double indicatorLength = sliderRightPotList[i].x() - sliderLeftPotList[i].x();

        QPointF handleBottomRightPot(sliderLeftPotList[i].x() + indicatorLength, sliderLeftPotList[i].y() - sliderBottomHeight);
        sliderRect = QRectF(sliderLeftPotList[i], handleBottomRightPot);

        QPointF tipRectTopLeftPot(sliderRect.topRight().x() + 2, sliderRect.topRight().y());
        QString strValue = QString("%1").arg(currentValueList[i], 0, 'f', precision);

        double textLength = fontMetrics().width(strValue);
        double textHeight = fontMetrics().height();
        QPointF tipRectBottomRightPot(tipRectTopLeftPot.x() + textLength + 10, tipRectTopLeftPot.y() - textHeight + 5);
        tipRect = QRectF(tipRectTopLeftPot, tipRectBottomRightPot);

        painter->setPen(sliderColorBottomList[i]);
        painter->setBrush(sliderColorBottomList[i]);
        painter->drawRect(sliderRect);
    }
        painter->restore();
}

void SliderRuler::drawTip(QPainter *painter)
{
//	if (!pressed) {
//		return;
//	}

//	painter->save();
//	painter->setPen(tipTextColor);
//	painter->setBrush(tipBgColor);
//	painter->drawRect(tipRect);
//	QString strValue = QString("%1").arg(currentValue, 0, 'f', precision);
//	painter->drawText(tipRect, Qt::AlignCenter, strValue);
//	painter->restore();
}

void SliderRuler::addSlider(QColor color)
{
    sliderCount += 1;
    sliderColorTopList.append(color);      //滑块上部颜色
    sliderColorBottomList.append(color);   //滑块下部颜色
    sliderLastPotList.append(QPointF(space, longLineHeight / 2));       //滑块最后的坐标
    sliderTopPotList.append(QPointF(0,0));        //滑块顶部坐标
    sliderLeftPotList.append(QPointF(0,0));       //滑块左边坐标
    sliderRightPotList.append(QPointF(0,0));      //滑块右边坐标
    valueList.append(0);
    currentValueList.append(0);
    update();
}

void SliderRuler::resetVariables()
{
	longLineHeight = height() / 5;
	shortLineHeight = height() / 7;
	sliderTopHeight = height() / 7;
	sliderBottomHeight = height() / 6;
}

void SliderRuler::setRange(double minValue, double maxValue)
{
	//如果最小值大于或者等于最大值以及最小值小于0则不设置
	if (minValue >= maxValue || minValue < 0) {
		return;
	}

	this->minValue = minValue;
	this->maxValue = maxValue;
    for(int i=0; i<sliderCount; i++)
    {
        setValue(i,minValue);
    }
}

void SliderRuler::setRange(int minValue, int maxValue)
{
	setRange((double)minValue, (double)maxValue);
}

void SliderRuler::setValue(int index,double value)
{
    if(index >= sliderCount) return;
    //值小于最小值或者值大于最大值则无需处理
    if (value < minValue || value > maxValue) {
        return;
    }
    double lineWidth = width() - 2 * space;

    double ratio = (double)value / (maxValue - minValue);
    double x = lineWidth * ratio;
    double newX = x + space;
    double y = space + longLineHeight - 10;
    sliderLastPotList[index] = QPointF(newX, y);

    this->valueList[index] = value;
    this->currentValueList[index] = value;
    emit valueChanged(index,value);
	update();
}

void SliderRuler::setValue(int index,int value)
{
    if(index >= sliderCount) return;
    setValue(index,(double)value);
}

void SliderRuler::setPrecision(int precision)
{
	this->precision = precision;
	update();
}

void SliderRuler::setStep(double longStep, double shortStep)
{
    //短步长不能超过长步长
	if (longStep < shortStep) {
		return;
	}

	this->longStep = longStep;
	this->shortStep = shortStep;
	update();
}

void SliderRuler::setSpace(int space)
{
	this->space = space;
	update();
}

void SliderRuler::setBgColor(QColor bgColorStart, QColor bgColorEnd)
{
	this->bgColorStart = bgColorStart;
	this->bgColorEnd = bgColorEnd;
	update();
}

void SliderRuler::setLineColor(QColor lineColor)
{
	this->lineColor = lineColor;
	update();
}

void SliderRuler::setSliderColor(int index, QColor sliderColorTop, QColor sliderColorBottom)
{
    if(index >= sliderCount) return;
    this->sliderColorTopList[index] = sliderColorTop;
    this->sliderColorBottomList[index] = sliderColorBottom;
	update();
}

void SliderRuler::setTipBgColor(QColor tipBgColor, QColor tipTextColor)
{
	this->tipBgColor = tipBgColor;
	this->tipTextColor = tipTextColor;
	update();
}
