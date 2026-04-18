#include "data_updater.h"

DataUpdater::DataUpdater(QObject *parent) : QObject(parent)
    , m_updateTimer(new QTimer(this))
    , m_updateRate(333)
    , m_running(false)
{
    connect(m_updateTimer, &QTimer::timeout, this, &DataUpdater::onUpdateTimerTimeout);
}

DataUpdater::~DataUpdater()
{
    stop();
    delete m_updateTimer;
}

void DataUpdater::setMeasVars(const QList<A2L_VarMeas*> &measVars)
{
    QMutexLocker locker(&m_mutex);
    m_measVars = measVars;
}

void DataUpdater::setDaqMeasVars(const QList<A2L_VarMeas*> &daqMeasVars)
{
    QMutexLocker locker(&m_mutex);
    m_daqMeasVars = daqMeasVars;
}

void DataUpdater::setCharVars(const QList<A2L_VarChar*> &charVars)
{
    QMutexLocker locker(&m_mutex);
    m_charVars = charVars;
}

void DataUpdater::setUpdateRate(int rate_ms)
{
    m_updateRate = rate_ms;
    if (m_updateTimer->isActive()) {
        m_updateTimer->setInterval(m_updateRate);
    }
}

void DataUpdater::start()
{
    if (!m_running) {
        m_running = true;
        m_updateTimer->setInterval(m_updateRate);
        m_updateTimer->start();
    }
}

void DataUpdater::stop()
{
    if (m_running) {
        m_running = false;
        m_updateTimer->stop();
    }
}

bool DataUpdater::isRunning() const
{
    return m_running;
}

void DataUpdater::onUpdateTimerTimeout()
{
    QMutexLocker locker(&m_mutex);
    
    // 模拟获取时间信息
    // 实际应用中，这里应该从共享内存或其他数据源获取时间信息
    QString pollReadTime = "0.000";
    QString daqReadTime = "0.000";
    QString caliWriteTime = "0.000";
    
    emit pollReadTimeUpdated(pollReadTime);
    emit daqReadTimeUpdated(daqReadTime);
    emit caliWriteTimeUpdated(caliWriteTime);
    
    // 批量更新测量变量
    QList<double> measValues;
    for (int i = 0; i < m_measVars.size(); i++) {
        A2L_VarMeas *measVar = m_measVars.at(i);
        if (measVar) {
            measValues.append(measVar->getValue());
        }
    }
    if (!measValues.isEmpty()) {
        emit measVarsUpdated(measValues);
    }
    
    // 批量更新DAQ测量变量
    QList<double> daqMeasValues;
    for (int i = 0; i < m_daqMeasVars.size(); i++) {
        A2L_VarMeas *measVar = m_daqMeasVars.at(i);
        if (measVar) {
            daqMeasValues.append(measVar->getValue());
        }
    }
    if (!daqMeasValues.isEmpty()) {
        emit daqMeasVarsUpdated(daqMeasValues);
    }
}
