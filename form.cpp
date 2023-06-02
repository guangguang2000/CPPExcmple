#include "form.h"
#include <iostream>
#include <random>
#include "rangebreak.h"
#include "define.h"
Form::Form()
{
    mStrategy = nullptr;
}

Form::~Form()
{
    mTrades.clear();
    delete mStrategy;
}

void Form::exe()
{
    QString startDay = mConfig["start_day"].toString("20230101");
    QString endDay = mConfig["start_day"].toString("20230201");
    QString instrument = mConfig["instrument"].toString("AUX888");
    auto bars = getData(instrument, startDay, endDay);
    runStrategy(bars);
    mBars = bars;

    cul_profit(mTrades, bars);
    show_result();
}

QList<Bar> Form::getData(const QString &instrument, const QString &startDay, const QString &endDay)
{
    return generateRandMarket(instrument, startDay,endDay);
}

QList<QList<double> *> Form::getLines()
{
    return mStrategy->getLines();
}

QList<TradeRecord> Form::getTrades()
{
    return mTrades;
}

void Form::send_order(bool isBuy, const QString &instrument, int volume, double price)
{    
    TradeRecord rec;
    rec.price = price;
    rec.datetime = mCurrentBar.time;
    rec.volume = volume;
    rec.id = instrument;
    rec.isBuy = isBuy;
    mTrades.push_back(rec);
}

std::map<QDate,double> Form::getClose(const QList<Bar> &mBars)
{
    std::map<QDate,double> r;
    for(auto bar : mBars)
    {
        r.insert({bar.time.date(), bar.close});
    }
    return r;
}

void Form::cul_profit(const QList<TradeRecord> &tradeRecords, const QList<Bar> &mBars)
{
    static double fee = 0;
    auto marketData = getClose(mBars);

    double netAssetValue = 0.0;
    int pos = 0;
    double lastclose = 0;

    for(auto m : marketData)
    {
        auto date = m.first;
        double closeprice = m.second;

        netAssetValue += pos * (closeprice - lastclose);
        auto traderecords = getTradesByDate(date,tradeRecords);
        int tradecount = 0;
        for (const auto& record : traderecords)
        {
            int volume = record.isBuy ? record.volume : -record.volume;
            pos += volume;
            netAssetValue += volume * (closeprice - record.price) - fee;
            tradecount ++;
        }
        resultRow d;
        d.closeprice = closeprice;
        d.date = date;
        d.pos = pos;
        d.tradecount = tradecount;
        d.profit = netAssetValue;
        mResultTable.append(d);

        lastclose = closeprice;
    }
}



QList<Bar> Form::generateRandMarket(const QString &instrument, const QString &startDay, const QString &endDay)
{
    QList<Bar> kLineData;

    unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
    std::default_random_engine generator(seed);
    std::uniform_real_distribution<double> priceDistribution(-10, 10);
    std::uniform_real_distribution<double> volumeDistribution(100, 1000);

    double last_price = 5000;
    auto daycount = QDate::fromString(startDay, DATEFORMAT).daysTo(QDate::fromString(endDay, DATEFORMAT));
    for(int i = 0; i<daycount; i++)
    {
        QDateTime time = QDateTime(QDate::fromString(startDay, DATEFORMAT).addDays(i),QTime(0,0,0));
        for(int minute=9.5*60; minute < 15*60; ++minute)
        {
            if (minute > 11.5*60 && minute < 13*60)
                continue;

            Bar bar;
            bar.symbol = instrument;
            bar.time = time.addSecs(minute*60);

            bar.open = int(last_price);

            bar.close = int(bar.open + priceDistribution(generator));
            if (bar.close > 6000)
                bar.close -= 30;
            if (bar.close < 2700)
                bar.close += 30;

            bar.high = fmax(bar.open,bar.close) + 1;
            bar.low = fmin(bar.open,bar.close) - 1 ;
            bar.volume = volumeDistribution(generator);
            bar.openInterest = 0.0;
            last_price = bar.close;

            kLineData.push_back(bar);
        }
    }
    return kLineData;
}

void Form::runStrategy(const QList<Bar>& bars)
{
    auto strategy = new RangeBreak();

    QJsonObject cfg;
    cfg["instrument"] = bars.at(0).symbol;
    strategy->setParam(cfg);

    strategy->setTradefunction(std::bind(&Form::send_order,this,                                        std::placeholders::_1,
                                         std::placeholders::_2,
                                         std::placeholders::_3,
                                         std::placeholders::_4));
    for(const auto &bar : bars)
    {
        mCurrentBar = bar;
        strategy->onBar(bar);
    }

    mStrategy = strategy;
}

QList<TradeRecord> Form::getTradesByDate(const QDate &date, const QList<TradeRecord> &tradeRecords)
{
    QList<TradeRecord> r;
    for(const auto &d : tradeRecords)
    {
        if (d.datetime.date() == date)
            r.append(d);
    }
    return r;
}

void Form::show_result()
{
    for(auto d : mResultTable)
    {
        std::cout << d.date.toString("yyyy-MM-dd").toStdString()
                  << " pos:" << d.pos
                  << " closeprice:" << d.closeprice
                  << " tradecount:" << d.tradecount
                  << " profit:" << d.profit
                  << std::endl;
    }
}
