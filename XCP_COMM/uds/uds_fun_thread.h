#ifndef UDS_FUN_THREAD_H
#define UDS_FUN_THREAD_H

#include <QObject>
#include <QThread>
#include <QDomDocument>
#include <QSharedMemory>
#include "common.h"
#include "ni_uds_worker.h"

class Uds_Fun_Thread : public QThread
{
    Q_OBJECT
public:
    Uds_Fun_Thread(QObject *parent = 0);
    ~Uds_Fun_Thread();

    enum UDS_STATE {
        INIT,
        STARTING,
        START_OK,
        WORKING,
        ERR,
        STOP,
        END
    };
    Q_ENUM(UDS_STATE)

    void preFun();

    void waitForUdsActive(bool udsActive, quint32 waitTime);

    void setFunInfo(FunStruct *value, int index);

    void setUdsInfo(const UDSInfo &value);

    bool getUdsStartFinished() const;

    void setIsStop(bool value);

    void setUdsRun(bool value);

    QList<PARAM *> getUdsReadPamList() const;

    QList<PARAM *> getUdsWritePamList() const;

protected:
    void run();

signals:
    void udsMsg(QString);

    void udsFunStateChanged(int, int);

private slots:

private:
    bool readSetting();

    void createPams();
    bool createPamXml();

    void init();
    bool startUDS();
    bool stopUDS();
    void workUDS();

    void fromWritePamToReqPams();
    void packWritePamList(QSharedMemory *writeSm, int writeSmSize);
    void fromWriteSmToReqPams(QSharedMemory *writeSm, int writeSmSize);
    void updateUdsCmdActiveInSm(QSharedMemory *writeSm, quint8 value);
    void packResPamsToReadSm(QSharedMemory *readSm, int readSmSize);
    void updateReadPamList();

    void createSM();
    void deleteSM();

    void reverseBytes(quint8 *data, int len);
    void waitMs(quint32 mSec);

private:
    FunStruct *fun = NULL;
    int funIndex = -1;
    UDSInfo udsInfo;

    NI_UDS_Worker *udsWorker = NULL;

    UDS_STATE curState = INIT;
    UDS_STATE lastState = INIT;

    bool isStop = false;
    bool udsRun = false;
    QTime time;

    bool udsStartFinished = false;

    quint8 udsActiveReq = 0;
    quint8 udsActiveAct = 0;

    quint8 udsCmdActive = 0;
    quint8 udsCmd = 0x00;
    quint8 udsReqDataLen = 0;     // 定义为cmd之后的数据长度(不包括cmd)
    quint8 udsReqData[8];

    quint8 udsResDataLen = 0;
    quint8 udsResData[52];

    QList<PARAM*> udsReadPamList;
    QList<PARAM*> udsWritePamList;

};

#endif // UDS_FUN_THREAD_H
