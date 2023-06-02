#ifndef MARKETCHART_H
#define MARKETCHART_H

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
#include "data_function.h"
#include "chartview.h"
class MarketChart
{
public:
    MarketChart();

    void setBars(const QList<Bar>& bars);

    void setLine(const QString& name,  const QList<double>& line, const QColor& color = Qt::green);

    void setPoint(const QString& name,  const QList<QPointF>& point, const QColor& color = Qt::red);

    QChartView* view() { return mChartView; }

protected:
    QChart *mChart;
    ChartView *mChartView;
};

#endif // MARKETCHART_H
