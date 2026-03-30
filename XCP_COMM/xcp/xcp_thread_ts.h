#ifndef XCP_THREAD_TS_H
#define XCP_THREAD_TS_H

#include <QObject>
#include <QObject>
#include <QThread>
#include <QTimer>
#include <QTime>
#include <QDateTime>
#include <string>
#include <QMutex>
#include <QSharedMemory>
#include <QFile>
#include <QDataStream>
#include "config.h"
#include "xcp_common.h"
//#include "TSCANDef.h"
//#include "TSCANLINApi.h"
#include "lib/windows/libTSCAN.h"

quint8 fromDlcToFDlc(quint8 dlc);
quint8 fromFDlcToDlc(quint8 fdlc);

/////////////////////////////////////////////////////////////////
class XCP_R_Thread_TS : public QThread
{
    Q_OBJECT
public:
    explicit XCP_R_Thread_TS(QObject *parent = 0);
    ~XCP_R_Thread_TS();

    size_t deviceHandle = 0;
    APP_CHANNEL canIndex = CHN1;
    quint32 canType = 0;
    quint32 id_Xcp_SlaveToMaster = 0x18FF828C;

    QList<quint32> eventIdList;

    void setIsStop(bool value);

    void setRcvBufSize(int value);

    bool getResDataReady() const;
    void setResDataReady(bool value);

    void startRcv();
    void stopRcv();

protected:
    void run();

signals:
    void RESDataReady_TS(quint8 *data, quint32 numBytes);
    void curFrameTimeChanged(quint64 timeStamp);

    void ODTDataReady_TS(quint8 *data, quint32 numBytes);

public:
    void unpackCanFrame();
    void unpackCanFdFrame();
/*
    void rcvCanMsgHandle(const TLIBCAN *canData);
    void rcvCanFdMsgHandle(const TLIBCANFD *canFdData);
    void rcvCanDataHandle(const TLIBCAN *canData);
    void rcvCanFdDataHandle(TLIBCANFD canFdData);
*/
    void initReadBuf();
    void delReadBuf();

private:
    bool isStop = false;
    bool resDataReady = false;
    int rcvBufSize = 100;
    int returnBufSize = 100;
    quint64 frameTimeStamp = 0;
    quint64 frameTimeStamp_last = 0;

    TLIBCAN *pCanDataRec = NULL;
    TLIBCANFD *pCanFDDataRec = NULL;

};
/////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////
class XCP_W_Thread_TS : public QThread
{
    Q_OBJECT
public:
    explicit XCP_W_Thread_TS(QObject *parent = 0);
    ~XCP_W_Thread_TS();

    void setWriteOnceEnable(bool enable);
    bool writeOnceSucceed();

    void setCMDPayLoad(quint8 *data, u32 numByte = 1);

    //TSCANLINApi *tsApiObj = NULL;
    size_t deviceHandle;
    quint32 deviceType = 0;
    quint32 deviceIndex = 0;
    quint8 canIndex = 0;
    quint32 canType = 0;

    quint32 id_Xcp_MasterToSlave = 0;

    u32 numOfFrame = 0;
    u32 numOfBytesForFrames = 0;

    //此处需要用数组，事先分配好空间


protected:
    void run();

signals:
    void CMDDataSend(quint8 *data, quint32 numBytes);

private:
    bool packTransmitData();

private:
    bool isStop = false;
    bool writeOnceEnable = false;
    bool writeSucceed = false;

    TLIBCAN pCanDataSnd;
    TLIBCANFD pCanFDDataSnd;
};
/////////////////////////////////////////////////////////////////
class XCP_Thread_TS : public QObject
{
    Q_OBJECT
public:
    explicit XCP_Thread_TS(QObject *parent = nullptr);
    ~XCP_Thread_TS();

    QStringList getCANInterface();

    size_t deviceHandle = 0;
    APP_CHANNEL canIndex = CHN1;

    quint32 canType = 0;          // 0=can, 1=canfd
    int canfd_standard = 0;  // 0 = can, 1 = iso canfd, 2 = no iso canfd
    bool isCustomBaud = false;
    double canfd_abitBaudrate = 500;   //kbps
    double canfd_dbitBaudrate = 500;   //kbps
    int resistanceEnable = 1;
    quint8 maxDLC = 8;

    quint32 id_CMD = 0x00;
    quint32 id_RES = 0x00;
    QList<quint32> eventIdList;

    XCP_R_Thread_TS *thread_RES = NULL;
    XCP_W_Thread_TS *thread_CMD = NULL;
    //XCP_R_Event_Thread *thread_EVENT = NULL;

    QTimer *timer = NULL;

public slots:
    bool XCP_CAN_Init();
    void XCP_CAN_Stop();

signals:

private:
    quint32 initTsApiObj();


};

#endif // XCP_THREAD_TS_H
