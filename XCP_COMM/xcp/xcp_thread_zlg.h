#ifndef XCP_THREAD_ZLG_H
#define XCP_THREAD_ZLG_H

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
#include "can_zlg/zlgcan_driver.h"

/////////////////////////////////////////////////////////////////
class XCP_R_Thread_ZLG : public QThread
{
    Q_OBJECT
public:
    explicit XCP_R_Thread_ZLG(QObject *parent = 0);
    ~XCP_R_Thread_ZLG();

    CHANNEL_HANDLE channelHandle = 0;

    quint32 canIndex = 0;
    quint32 canType = 0;
    quint32 id_Xcp_SlaveToMaster = 0x00;

    QList<quint32> eventIdList;

    void setIsStop(bool value);

    void setRcvBufSize(int value);

    bool getResDataReady() const;
    void setResDataReady(bool value);

protected:
    void run();

signals:
    void RESDataReady_ZLG(quint8 *data, quint32 numBytes);
    void curFrameTimeChanged(quint64 timeStamp);

    void ODTDataReady_ZLG(quint8 *data, quint32 numBytes);

public:
    void unpackCanFrame(quint32 num);
    void unpackCanFdFrame(quint32 num);

private:
    bool isStop = false;
    bool resDataReady = false;
    int rcvBufSize = 100;
    int returnBufSize = 100;
    quint64 frameTimeStamp = 0;
    quint64 frameTimeStamp_last = 0;

    ZCAN_Receive_Data *pCanDataRec = NULL;
    ZCAN_ReceiveFD_Data *pCanFDDataRec = NULL;

    quint8 rcvBuf[16];

};
/////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////
class XCP_W_Thread_ZLG : public QThread
{
    Q_OBJECT
public:
    explicit XCP_W_Thread_ZLG(QObject *parent = 0);
    ~XCP_W_Thread_ZLG();

    void setWriteOnceEnable(bool enable);
    bool writeOnceSucceed();

    void setCMDPayLoad(quint8 *data, quint32 numByte = 1);

    CHANNEL_HANDLE channelHandle;
    quint32 deviceType = 0;
    quint32 deviceIndex = 0;
    quint8 canIndex = 0;
    quint32 canType = 0;

    quint32 id_Xcp_MasterToSlave = 0;

    quint32 numOfFrame = 0;
    quint32 numOfBytesForFrames = 0;

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

    ZCAN_Transmit_Data pCanDataSnd;
    ZCAN_TransmitFD_Data pCanFDDataSnd;

    ZCANDataObj *pTransmit = NULL;
};
/////////////////////////////////////////////////////////////////
class XCP_Thread_ZLG : public QObject
{
    Q_OBJECT
public:
    explicit XCP_Thread_ZLG(QObject *parent = nullptr);
    ~XCP_Thread_ZLG();

    QStringList getCANInterface();

    DEVICE_HANDLE deviceHandle = INVALID_DEVICE_HANDLE;
    CHANNEL_HANDLE channelHandle = INVALID_CHANNEL_HANDLE;
    ZCAN_CHANNEL_INIT_CONFIG canPortInitConfig;       //config of can
    ZCAN_DEVICE_INFO deviceInfo;

    quint32 deviceType = 0;       // device type like: USBCANFD-200U ...
    quint32 deviceIndex = 0;      // device index: the index of same devices
    quint32 canIndex = 0;         // can port index: the index of can port in a device
    quint32 canType = 0;          // 0=can, 1=canfd

    bool isCANFD = false;

    quint32 canfd_standard = 0;
    bool isCustomBaud = false;
    quint32 canfd_abitBaudrate = 500000;
    quint32 canfd_dbitBaudrate = 2000000;
    int resistanceEnable = 0;
    quint8 maxDLC = 8;

    quint32 id_CMD = 0x00;
    quint32 id_RES = 0x00;
    QList<quint32> eventIdList;

    XCP_R_Thread_ZLG *thread_RES = NULL;
    XCP_W_Thread_ZLG *thread_CMD = NULL;

public slots:
    bool XCP_CAN_Init();
    void XCP_CAN_Stop();

};

#endif // XCP_THREAD_ZLG_H
