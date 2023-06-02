#include "marketchart.h"
#include <QScatterSeries>
MarketChart::MarketChart()
    : mChart(new QChart(nullptr))
    , mChartView(new ChartView(mChart))    
{    
    mChart->setTitle("Price candle");
    //mChart->setAnimationOptions(QChart::SeriesAnimations);
    mChart->setAnimationOptions(QChart::NoAnimation);
    mChartView->setRenderHint(QPainter::Antialiasing);

    mChart->legend()->setVisible(true);
    mChart->legend()->setAlignment(Qt::AlignBottom);
    mChart->setBackgroundBrush(QColor(Qt::darkGray));
}

void MarketChart::setBars(const QList<Bar> &bars)
{
    auto candleSeries =new QCandlestickSeries(mChart);
    candleSeries->setIncreasingColor(QColor(Qt::red));
    candleSeries->setDecreasingColor(QColor(Qt::green));
    candleSeries->setName(bars.begin()->symbol);

    QStringList categories;

    for(const auto& bar : bars)
    {
        QCandlestickSet *set = new QCandlestickSet();
        set->setHigh(bar.high);
        set->setLow(bar.low);
        set->setOpen(bar.open);
        set->setClose(bar.close);
        set->setTimestamp(bar.time.toMSecsSinceEpoch());

        candleSeries->append(set);
        categories << bar.time.toString("yyyyMMdd hh:mm");
    }
    candleSeries->setBodyOutlineVisible(false);
    mChart->addSeries(candleSeries);
    mChart->createDefaultAxes();
    QBarCategoryAxis *axisX = qobject_cast<QBarCategoryAxis *>(mChart->axes(Qt::Horizontal).at(0));
    axisX->setVisible(false);
    axisX->setCategories(categories);
    axisX->setRange(categories[std::max(0, int(categories.size() - ChartView::MaxViewCandleNum))], categories.back());
    mChartView->resizeWindow();
}

void MarketChart::setLine(const QString& name,  const QList<double> &line, const QColor &color)
{
    QLineSeries *series = new QLineSeries(mChart);
    for(int i=0; i<line.size(); ++i)
    {
        series->append(i, line[i]);
    }
    series->setName(name);
    mChart->addSeries(series);

    QBarCategoryAxis *axisX = qobject_cast<QBarCategoryAxis *>(mChart->axes(Qt::Horizontal).at(0));
    QValueAxis *axisY       = qobject_cast<QValueAxis *>(mChart->axes(Qt::Vertical).at(0));
    series->attachAxis(axisX);
    series->attachAxis(axisY);
    series->setColor(color);
}

void MarketChart::setPoint(const QString &name, const QList<QPointF> &points, const QColor &color)
{
    QScatterSeries *series = new QScatterSeries(mChart);
    series->setPointLabelsVisible(true);
    series->setPointLabelsFormat("(@xPoint,@yPoint)");
    series->setPointLabelsClipping(true);
    series->setPointLabelsColor(Qt::blue);

    for(auto point : points)
    {
        series->append(point);
    }
    series->setName(name);
    mChart->addSeries(series);

    QBarCategoryAxis *axisX = qobject_cast<QBarCategoryAxis *>(mChart->axes(Qt::Horizontal).at(0));
    QValueAxis *axisY       = qobject_cast<QValueAxis *>(mChart->axes(Qt::Vertical).at(0));
    series->attachAxis(axisX);
    series->attachAxis(axisY);
    series->setColor(color);
}
