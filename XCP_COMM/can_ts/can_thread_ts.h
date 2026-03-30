#ifndef CAN_THREAD_TS_H
#define CAN_THREAD_TS_H

#include <QThread>
#include <QDebug>
#include <QSharedMemory>
#include <QTime>
#include "dbcparser.h"
#include "common.h"

#include "lib/windows/libTSCAN.h"

/*
struct CRC_INFO{
    int crcIndex;
    int startIndex;
    int endIndex;
}
*/

quint8 fromDlcToFDlc(quint8 dlc);
quint8 fromFDlcToDlc(quint8 fdlc);

/////////////////////////////////////////////////////////
class Can_R_Thread_TS : public QThread
{
    Q_OBJECT
public:
    Can_R_Thread_TS(QObject *parent = 0);
    ~Can_R_Thread_TS();

    void setIsStop(bool value);

    void setCanType(const quint32 &value);

    void setReadFrameList(const QList<BO_ *> &value);

    void setModName(const QString &value);

    void setSmHash(const QHash<quint32, QSharedMemory *> &value);

    void setMdfRecordOn(bool value);

    void setCanIndex(const APP_CHANNEL &value);

    void setDeviceHandle(const size_t &value);

protected:
    void run();

signals:
    void canDataForRecord(quint8*,quint32,QString);
    void rcvDataUpdated(quint32 id, quint8 *buf, quint8 len);

private slots:
    void Slt_rcvDataUpdated(quint32 id, quint8 *buf, quint8 len);

private:

    void initReadBuf();
    void delReadBuf();

    void rcvCanDataHandle(qint32 num);
    void rcvCanFdDataHandle(quint32 num);

    bool isReadFrame(quint32 id);
    void setFilterFrames(QList<BO_*> frames);

#ifdef Q_OS_LINUX
#endif


private:
    size_t deviceHandle = 0;

    APP_CHANNEL canIndex = CHN1;

    QString modName = "";
    bool isStop = false;
    bool mdfRecordOn = false;

    quint32 canType = 0;

    qint32 rcvBufSize = 10;
    TLIBCAN *pCanDataRec = NULL;
    TLIBCANFD *pCanFDDataRec = NULL;

    quint8 canDataTemp[8];
    quint8 canFdDataTemp[64];

#ifdef Q_OS_LINUX

#endif
    QList<BO_*> readFrameList;

    QHash<quint32, QSharedMemory*> smHash;

};

/////////////////////////////////////////////////////////
class Can_W_Thread_TS : public QThread
{
    Q_OBJECT
public:
    Can_W_Thread_TS(QObject *parent = 0);

    void setIsStop(bool value);

    void setCanType(const quint32 &value);

    void setWriteFrameList(const QList<BO_ *> &value);

    void setModName(const QString &value);

    void setSmHash(const QHash<quint32, QSharedMemory *> &value);

    void setWriteFrame(BO_ *value);

    void setSm(QSharedMemory *value);

    void setCanIndex(const APP_CHANNEL &value);


    void setDeviceHandle(const size_t &value);

protected:
    void run();

signals:

private:
    void init();
    bool packTransmitData();
    void frameFromRawDataToValue(char *data, BO_ *frame);
    void sigRC(SG_ *sg, int min, int max, int step);
    void dataCRC(char *data, int startIndex, int endIndex, int crcIndex);

#ifdef Q_OS_WIN
    bool copySndDataFromSmToBuf();
#endif

    void updateSndTimeStamp();
    void endHandle();

#ifdef Q_OS_LINUX

#endif

private:
    size_t deviceHandle;

    quint32 deviceType = 0;
    quint32 deviceIndex = 0;
    quint8 canIndex = 0;

    QTime time;
    //quint64 sndAbsTime = 0;
    qreal sndAbsTime = 0;     // send time(s)

    QString modName = "";

    bool isStop = false;
    bool remoteCtrl = false;

    quint32 canType = 0;

    QList<BO_*> writeFrameList;
    QHash<quint32, QSharedMemory*> smHash;

    BO_ *writeFrame = NULL;
    QSharedMemory *sm = NULL;
    quint16 transRate = 100;

#ifdef Q_OS_WIN
    TLIBCAN pCanDataSnd;
    TLIBCANFD pCanFDDataSnd;
#endif

#ifdef Q_OS_LINUX

#endif
    QList<CRC_INFO> crcInfoList;
};

/////////////////////////////////////////////////////////
class Can_Thread_TS : public QThread
{
    Q_OBJECT
public:
    Can_Thread_TS(QObject *parent = 0);
    ~Can_Thread_TS();

    enum CAN_STATE {
        UNCONN,

        CONN_OK,
        CONN_ERR,

        SETVALUE_OK,
        SETVALUE_ERR,

        START_OK,
        START_ERR,

        R_W_ING,
        R_W_ERR,

        DISCONN_OK,
        DISCONN_ERR,

        ERR_HANDLE,
        STOP,
        END
    };
    Q_ENUM(CAN_STATE)

    void setIsStop(bool value);

    void setCanStop(bool value);

    void setCanIndex(const APP_CHANNEL &value);

    void setCanType(const quint32 &value);

    void setCanfd_standard(const quint32 &value);

    void setCanfd_abitBaudrate(const quint32 &value);

    void setCanfd_dbitBaudrate(const quint64 &value);

    void setResistanceEnable(const quint32 &value);

    void setIsCustomBaud(bool value);

    void setReadFrameList(const QList<BO_ *> &value);

    void setWriteFrameList(const QList<BO_ *> &value);

    void setModName(const QString &value);

    void setReadSmHash(const QHash<quint32, QSharedMemory *> &value);

    void setWriteSmHash(const QHash<quint32, QSharedMemory *> &value);

    void setMdfRcdOn(bool flag);

    Can_Thread_TS::CAN_STATE getCurState() const;

    void setStopWithoutClose(bool value);

    size_t getDeviceHandle() const;
    void setDeviceHandle(const size_t &value);

    QHash<QString, QSharedMemory *> getReadFrameSmHash() const;

    QHash<QString, QSharedMemory *> getWriteFrameSmHash() const;

protected:
    void run();

signals:
    void stateChanged(quint32, Can_Thread_TS::CAN_STATE);

    void canDataForRecord(quint8*,quint32,QString);

public slots:

private:
    void initTsApiObj();

    void initConnection();

#ifdef Q_OS_WIN
    void setFiltersForFrames(QList<BO_*> frames);
#endif

#ifdef Q_OS_LINUX

#endif

private:
    CAN_STATE curState = UNCONN;
    CAN_STATE lastState = UNCONN;
    bool isStop = false;
    bool canStop = false;      // false = can run, true = can stop
    bool stopWithoutClose = false;

    size_t deviceHandle = 0;

    APP_CHANNEL canIndex = CHN1;

    QString modName = "";

    quint32 canType = 0;          // 0=can, 1=canfd

    bool isCANFD = false;

    int canfd_standard = 0;

    bool isCustomBaud = false;

    double canfd_abitBaudrate = 500;   //kbps

    double canfd_dbitBaudrate = 2000;   //kbps

    int resistanceEnable = 0;

    Can_R_Thread_TS *canRcvThread = NULL;
    Can_W_Thread_TS *canSndThread = NULL;
    QList<Can_W_Thread_TS*> canSndThreadList;

    QList<BO_*> readFrameList;
    QList<BO_*> writeFrameList;

    QHash<quint32, QSharedMemory*> readSmHash;
    QHash<quint32, QSharedMemory*> writeSmHash;

    QHash<QString, QSharedMemory*> readFrameSmHash;
    QHash<QString, QSharedMemory*> writeFrameSmHash;

#ifdef Q_OS_LINUX

#endif

};

#endif // CAN_THREAD_TS_H
