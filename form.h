#ifndef FORM_H
#define FORM_H

#include <QObject>
#include <QJsonObject>
#include "data_function.h"

#include "rangebreak.h"

struct TradeRecord
{
    QDateTime  datetime;
    QString id;
    bool isBuy;
    int volume;
    double price;
};

class Form
{
    struct resultRow
    {
        QDate date;
        int   pos;
        int   tradecount;
        double closeprice;
        double profit;
    };
public:
    Form();
    ~Form();

    void exe();

    QList<Bar> data() { return mBars;}

    QList<QList<double>*> getLines();

    QList<TradeRecord> getTrades();

private:
    QList<Bar> getData(const QString& instrument, const QString& startDay, const QString& endDay);
    QList<Bar> generateRandMarket(const QString& instrument, const QString& startDay, const QString& endDay);
    void runStrategy(const QList<Bar>& bars);
    QList<TradeRecord> getTradesByDate(const QDate& date, const QList<TradeRecord> &tradeRecords);
    std::map<QDate,double> getClose(const QList<Bar> &mBars);
    void show_result();

    QList<TradeRecord> mTrades;
    QList<Bar> mBars;
    QList<resultRow> mResultTable;

    void send_order(bool isBuy, const QString& instrument, int volume, double price);

    void cul_profit(const QList<TradeRecord>& trades, const QList<Bar>& mBars);

    Bar mCurrentBar;
    QJsonObject mConfig;
    RangeBreak  *mStrategy;
};

#endif // FORM_H
