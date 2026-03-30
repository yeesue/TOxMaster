#include "chartthread.h"
#include <QTime>
#include <QDebug>
#include <QMetaType>

ChartThread::ChartThread(QObject *parent)
{

}

ChartThread::~ChartThread()
{

}

void ChartThread::run()
{
    qRegisterMetaType<QList<double>>("QList<double>");
    while(!isStop)
    {
        msleep(rate);

        static QTime timeStart = QTime::currentTime();
        // calculate two new data points:
        double key = timeStart.msecsTo(QTime::currentTime())/1000.0; // time elapsed since start of demo, in seconds
        static double lastPointKey = 0;
        if (key - lastPointKey > 0.002) // at most add point every 2 ms
        {
            QList<double> valueList;
            for(int i = 0; i < pamList.count(); i++)
            {
                PARAM *pam = pamList.at(i);

                if(!pam)
                {
                    valueList.append(0);
                    continue;
                }

                valueList.append(pam->getValue());
            }

            emit dataUpdated(key, valueList);
        }
    }

}

void ChartThread::setRate(int value)
{
    rate = value;
}

void ChartThread::setPamList(const QList<PARAM *> &value)
{
    pamList = value;
}

void ChartThread::setIsStop(bool value)
{
    isStop = value;
}
