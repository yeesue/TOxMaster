#ifndef XCP_POLLING_THREAD_H
#define XCP_POLLING_THREAD_H

#include <QThread>
#include <QMutex>
#include <QWaitCondition>
#include <QElapsedTimer>
#include "xcpmaster.h"
#include "a2l_varchar.h"
#include "a2l_varmeas.h"
#include "charpamcheck.h"

class XCP_Polling_Thread : public QThread
{
    Q_OBJECT
public:
    XCP_Polling_Thread(QObject *parent = 0, XCPMaster *master = 0);
    ~XCP_Polling_Thread();

    void setMeasPamList(QList<A2L_VarMeas*> measList);
    void setCharPamList(QList<A2L_VarChar*> charList);
    void setPollRunFlag(bool runFlag);
    void setSMKey(QString smKeyRead);
    void setMeasRate(int rate_ms);

    void setSmReadSize(int value);

    void setSmRead(QSharedMemory *value);

    void setRecordOn(bool value);

    void setXcpName(const QString &value);

    void setIsStop(bool value);
    void stop();  // 新增：安全停止线程

protected:
    void run();

signals:
    void measPamsValueUpdated();

    void caliStatusChanged(bool curCaliStatus);
    void pollingStatusChanged(bool curPollingStatus);

    void pollingSucceed(qreal absTime);
    void pollDataForRecord(quint8* buf,quint32 size, int dgList);
    void pollDataForRecord(quint8* buf,quint32 size, QString dgName);

public slots:
    void addCaliActionSlot(QVariant caliData);
    void addMapCaliActionSlot(QVariant mapCaliData);

    void pollingSucceedSlot(qreal absTime);
    void sltPollingStatusActive(bool status);

private:
    XCPMaster *xcpMaster = NULL;
    QString xcpName = "";

    QList<A2L_VarMeas*> measPamList;
    QList<A2L_VarChar*> charPamList;


    bool isStop = false;
    bool pollRunFlag = false;
    int measRate_ms = 1;

    bool pollingOk = false;
    bool caliOk = false;
    bool recordOn = true;

    QSharedMemory *smRead = NULL;
    QString smKeyRead = "";
    int smReadSize = 0;

    QList<Cali_Pair> caliPairList;
    QList<Cali_Pair> mapCaliPairList;
    
    // 线程同步机制
    QMutex m_mutex;
    QWaitCondition m_condition;
    bool m_running = true;
};

#endif // XCP_POLLING_THREAD_H
