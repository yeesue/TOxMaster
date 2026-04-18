#ifndef DATA_UPDATER_H
#define DATA_UPDATER_H

#include <QObject>
#include <QTimer>
#include <QMutex>
#include <QList>
#include "a2l_varmeas.h"
#include "a2l_varchar.h"

class DataUpdater : public QObject
{
    Q_OBJECT

public:
    explicit DataUpdater(QObject *parent = nullptr);
    ~DataUpdater();

    void setMeasVars(const QList<A2L_VarMeas*> &measVars);
    void setDaqMeasVars(const QList<A2L_VarMeas*> &daqMeasVars);
    void setCharVars(const QList<A2L_VarChar*> &charVars);
    void setUpdateRate(int rate_ms);

    void start();
    void stop();

    bool isRunning() const;

signals:
    void pollReadTimeUpdated(const QString &time);
    void daqReadTimeUpdated(const QString &time);
    void caliWriteTimeUpdated(const QString &time);
    void measVarsUpdated(const QList<double> &values);
    void daqMeasVarsUpdated(const QList<double> &values);

private slots:
    void onUpdateTimerTimeout();

private:
    QTimer *m_updateTimer;
    QMutex m_mutex;
    QList<A2L_VarMeas*> m_measVars;
    QList<A2L_VarMeas*> m_daqMeasVars;
    QList<A2L_VarChar*> m_charVars;
    int m_updateRate;
    bool m_running;
};

#endif // DATA_UPDATER_H