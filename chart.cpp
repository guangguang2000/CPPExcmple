#include <QtCharts/QBarCategoryAxis>
#include <QtCharts/QCandlestickSeries>
#include <QtCharts/QChartView>
#include <QtCharts/QValueAxis>
#include <QtCore/QDateTime>
#include <QtCore/QFile>
#include <QtWidgets/QApplication>
#include <QtWidgets/QMainWindow>
#include <QtWidgets>
#include <QtCharts>
#include <QDebug>
#include <QObject>
#include "candlestickdatareader.h"

class ChartView : public QChartView
{
public:
    ChartView(QChart* chart = nullptr);
    virtual ~ChartView();
protected:
    virtual void mouseMoveEvent(QMouseEvent *pEvent) override;
    virtual void mousePressEvent(QMouseEvent *pEvent) override;
    virtual void mouseReleaseEvent(QMouseEvent *pEvent) override;
    virtual void wheelEvent(QWheelEvent *pEvent) override;
    virtual void enterEvent(QEnterEvent *pEvent)override;
    virtual void leaveEvent(QEvent *pEvent)override;

private:
    bool leftButtonPressed;
    QPoint prePos;

    QGraphicsLineItem* x_line;
    QGraphicsLineItem* y_line;

    QGraphicsSimpleTextItem* cursor_text;
};



ChartView::ChartView(QChart *chart):
    QChartView(chart),
    leftButtonPressed(false) ,
    prePos(0, 0)
{    
    x_line = new QGraphicsLineItem(); 
    x_line->setPen(QPen(QColor( 100, 100, 100 )));
    x_line->setZValue(2);
    y_line = new QGraphicsLineItem();
    y_line->setPen(QPen(QColor( 100, 100, 100 )));
    y_line->setZValue(2);
    
    this->scene()->addItem(x_line);
    this->scene()->addItem(y_line);

    cursor_text = new QGraphicsSimpleTextItem;
    this->scene()->addItem(cursor_text);

}
ChartView::~ChartView()
{

}
void ChartView::mouseMoveEvent(QMouseEvent *pEvent)
{
    if (leftButtonPressed)
    {
        QPoint oDeltaPos = pEvent->pos() - prePos;
        this->chart()->scroll(-oDeltaPos.x(), oDeltaPos.y());
        prePos = pEvent->pos();
    }        

    x_line->setLine(pEvent->pos().rx(),0,pEvent->pos().rx(),this->height());
    y_line->setLine(0,pEvent->pos().ry(),this->width(),pEvent->pos().ry());

    auto valpos = chart()->mapToValue(pEvent->pos());
    int x = int(valpos.x());
    QCandlestickSeries *candle = qobject_cast<QCandlestickSeries *>(chart()->series().at(0));
    if (x<0) x = 0;
    if (x>=candle->count()) x = candle->count()-1;
    auto d = candle->sets().at(x)->close();
    cursor_text->setText(QString("sit:%1:%2").arg(x).arg(d));
  
    auto pos = pEvent->pos();
    pos.setY(pos.ry()-20);
    pos.setX(pos.rx()+10);
    cursor_text->setPos(pos);

    __super::mouseMoveEvent(pEvent);
}

void ChartView::mousePressEvent(QMouseEvent *pEvent)
{
    if (pEvent->button() == Qt::LeftButton)
    {
        leftButtonPressed = true;
        prePos = pEvent->pos();
        this->setCursor(Qt::OpenHandCursor);
    }
    __super::mousePressEvent(pEvent);
}

void ChartView::mouseReleaseEvent(QMouseEvent *pEvent)
{
    if (pEvent->button() == Qt::LeftButton)
    {
        leftButtonPressed = false;
        this->setCursor(Qt::ArrowCursor);
    }
    __super::mouseReleaseEvent(pEvent);
}

void ChartView::wheelEvent(QWheelEvent *pEvent)
{
    qreal rVal;
    if (pEvent->angleDelta().y() > 0)
        rVal = 0.99;
    else
        rVal = 1.01;


    QRectF oPlotAreaRect = this->chart()->plotArea();
    QPointF oCenterPoint = oPlotAreaRect.center();
    oPlotAreaRect.setWidth(oPlotAreaRect.width() * rVal);
    oPlotAreaRect.setHeight(oPlotAreaRect.height() * rVal);
    QPointF oNewCenterPoint(2 * oCenterPoint - pEvent->position() - (oCenterPoint - pEvent->position()) / rVal);
    oPlotAreaRect.moveCenter(oNewCenterPoint);
    this->chart()->zoomIn(oPlotAreaRect);
  
    __super::wheelEvent(pEvent);
}

void ChartView::enterEvent(QEnterEvent *pEvent)
{
    x_line->setVisible(true);
    y_line->setVisible(true);
    cursor_text->setVisible(true);
    __super::enterEvent(pEvent);
}

void ChartView::leaveEvent(QEvent *pEvent)
{
    x_line->setVisible(false);
    y_line->setVisible(false);
    cursor_text->setVisible(false);
    __super::leaveEvent(pEvent);
}

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    QCandlestickSeries *acmeSeries = new QCandlestickSeries();
    acmeSeries->setName("Acme Ltd");
    acmeSeries->setIncreasingColor(QColor(Qt::green));
    acmeSeries->setDecreasingColor(QColor(Qt::red));

    QLineSeries *closeSeries = new QLineSeries();
    closeSeries->setName("close");
    closeSeries->setColor(QColor(Qt::black));
  
    QFile acmeData(":acme");
    if (!acmeData.open(QIODevice::ReadOnly | QIODevice::Text))
        return 1;

    QStringList categories;

    CandlestickDataReader dataReader(&acmeData);
    int i = 0;
    while (!dataReader.atEnd()) {
        QCandlestickSet *set = dataReader.readCandlestickSet();
        if (set) {
            acmeSeries->append(set);
            closeSeries->append(QPointF(i++, set->close()));
            categories << QDateTime::fromMSecsSinceEpoch(set->timestamp()).toString("dd");            
        }
    }

    QChart *chart = new QChart();
    chart->addSeries(acmeSeries);
    chart->addSeries(closeSeries);
    chart->setTitle("Acme Ltd Historical Data (July 2015)");
    chart->setAnimationOptions(QChart::SeriesAnimations);


    chart->createDefaultAxes();

    QBarCategoryAxis *axisX = qobject_cast<QBarCategoryAxis *>(chart->axes(Qt::Horizontal).at(0));
    axisX->setCategories(categories);

    QValueAxis *axisY = qobject_cast<QValueAxis *>(chart->axes(Qt::Vertical).at(0));
    axisY->setMax(axisY->max() * 1.01);
    axisY->setMin(axisY->min() * 0.99);    
　　
    chart->legend()->setVisible(true);
    chart->legend()->setAlignment(Qt::AlignBottom);

    QChartView *chartView = new ChartView(chart);  
    chartView->setRenderHint(QPainter::Antialiasing);

    QMainWindow window;
    window.setCentralWidget(chartView);
    window.resize(800, 600);
    window.show();

    return a.exec();
}
