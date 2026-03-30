#ifndef CHARTTHREAD_H
#define CHARTTHREAD_H

#include <QObject>
#include <QThread>
#include "param.h"

class ChartThread : public QThread
{
    Q_OBJECT
public:
    ChartThread(QObject *parent = nullptr);
    ~ChartThread();

    void setIsStop(bool value);

    void setPamList(const QList<PARAM *> &value);

    void setRate(int value);

protected:
    void run();

signals:
    void dataUpdated(double, QList<double>);

private:

private:
    bool isStop = false;
    int rate = 100;
    QList<PARAM*> pamList;

};

#endif // CHARTTHREAD_H
