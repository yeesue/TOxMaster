#ifndef XCP_THREAD_H
#define XCP_THREAD_H

#include "nixnet.h"

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
#include "nixnet_thread.h"
#include "common/smart_ptr.h"

#define   DEFAULT_INTF_NUM   20

//void displayErrorAndExit(nxStatus_t status, nxSessionRef_t session, const char *source);
/////////////////////////////////////////////////////////////////
class XCP_R_Thread : public QThread
{
    Q_OBJECT
public:
    explicit XCP_R_Thread(QObject *parent = 0);
    ~XCP_R_Thread();

    void setReadOnceEnable(bool enable);
    void setResDataReady(bool ready);
    bool getResDataReady();

    nxStatus_t status_R_XNET = 0;
    nxSessionRef_t session_R_XNET = 0;

    nxStatus_t status_R_XNET_Stream = 0;
    nxSessionRef_t session_R_XNET_Stream = 0;

    nxStatus_t status_R_SigConv = 0;
    nxSessionRef_t session_R_SigConv = 0;

    u32 numOfFrame = 0;
    u32 numOfBytesForFrames_Stream = 0;

    //此处需要用数组，事先分配好空间
    nxFrameCAN_t *frameBuffer_R_Stream = NULL;
    f64 timeout_r_Stream = nxTimeout_None;
    u32 *numOfBytesReturned_Stream = new u32;

    bool isCanFd = false;
    nxFrameCANFD_t *fdFrameBuffer_R_Stream = NULL;

    quint8 byteOrder_R = INTEL;

    nxTimestamp_t *FrameTimeStampBuffer_Stream = NULL;
    nxTimestamp_t *FrameTimeStampBuffer_Old_Stream = NULL;

    void setIsStop(bool value);

    void setFrameStreamSize(int value);

    void setId_Xcp_SlaveToMaster(const quint32 &value);

private:
    void unpackFrameStream();
    void unpackFdFrameStream();

private:

    bool isStop = false;
    bool readOnceEnable = false;
    bool resDataReady = false;

    int FrameStreamSize = 1;
    quint32 id_Xcp_SlaveToMaster = 0;


protected:
    void run();

signals:
    void RESDataReady(ByteArrayPtr data, quint32 numBytes);
    void curFrameTimeChanged(quint64 timeStamp);

    void ODTDataReady(ByteArrayPtr data, quint32 numBytes);

};
/////////////////////////////////////////////////////////////////
class XCP_R_Event_Thread : public QThread
{
    Q_OBJECT
public:
    explicit XCP_R_Event_Thread(QObject *parent = 0);
    ~XCP_R_Event_Thread();

    nxStatus_t status_R_Event_XNET_Stream = 0;
    nxSessionRef_t session_R_Event_XNET_Stream = 0;

    u32 numOfFrame = 0;
    u32 numOfBytesForFrames_Stream = 0;

    //此处需要用数组，事先分配好空间
    nxFrameCAN_t *frameBuffer_R_Event_Stream = NULL;
    f64 timeout_r_Stream = nxTimeout_None;
    u32 *numOfBytesReturned_Stream = new u32;

    bool isCanFd = false;
    nxFrameCANFD_t *fdFrameBuffer_R_Event_Stream = NULL;

    quint8 byteOrder_R = INTEL;

    nxTimestamp_t *FrameTimeStampBuffer_Stream = NULL;
    nxTimestamp_t *FrameTimeStampBuffer_Old_Stream = NULL;

    void setIsStop(bool value);

    void setFrameStreamSize(int value);

    void setEventIdList(const QList<quint32> &value);

private:
    void unpackEventFrameStream();
    void unpackEventFdFrameStream();

private:

    bool isStop = false;
    bool readOnceEnable = false;

    int FrameStreamSize = 30;
    QList<quint32> eventIdList;

protected:
    void run();

signals:
    void curFrameTimeChanged(quint64 timeStamp);
    void ODTDataReady(ByteArrayPtr data, quint32 numBytes);

};
/////////////////////////////////////////////////////////////////
class XCP_W_Thread : public QThread
{
    Q_OBJECT
public:
    explicit XCP_W_Thread(QObject *parent = 0);
    ~XCP_W_Thread();

    void setWriteOnceEnable(bool enable);
    bool writeOnceSucceed();

    void setCMDPayLoad(quint8 *data, u32 numByte = 1);

    nxStatus_t status_W_XNET = 0;
    nxSessionRef_t session_W_XNET = 0;

    u32 numOfFrame = 0;
    u32 numOfBytesForFrames = 0;

    //此处需要用数组，事先分配好空间
    nxFrameCAN_t *frameBuffer_W = NULL;
    nxFrameCAN_t *frameBuffer_W_temp = NULL;
    u32 numOfBytesReturned;

    bool isCanFd = false;
    nxFrameCANFD_t *fdFrameBuffer_W = NULL;
    nxFrameCANFD_t *fdFrameBuffer_W_temp = NULL;

    f64 timeout_w = nxTimeout_None;

private:
    bool isStop = false;
    bool writeOnceEnable = false;
    bool writeSucceed = false;

protected:
    void run();

signals:
    void CMDDataSend(ByteArrayPtr data, quint32 numBytes);

};
/////////////////////////////////////////////////////////////////
class XCP_Thread : public QObject
{
    Q_OBJECT
public:
    explicit XCP_Thread(QObject *parent = 0);
    ~XCP_Thread();

    QStringList getCANInterface();

    QString path;
    QString name;
    QString port;
    QString cluster;
    QString frames_r;
    QString frames_w;
    QString frames_r_event;


    u64 baudrate;

    nxStatus_t status_dbc = 0;
    nxDatabaseRef_t databaseRef;
    nxDatabaseRef_t *clusterRef = NULL;
    u32 numOfClusterRef;
    u32 sizeOfClusterRef;
    char* clusterName;
    u32 sizeOfClusterName;
    nxDatabaseRef_t *frameRef;
    u32 numOfFrameRef;
    u32 sizeOfFrameRef;
    char* frameName;
    u32 sizeOfFrameName;

    XCP_R_Thread *thread_RES = NULL;
    XCP_W_Thread *thread_CMD = NULL;
    XCP_R_Event_Thread *thread_EVENT = NULL;

    bool isCanFd = false;
    quint8 maxDLC = 8;

public slots:
    bool XCP_CAN_Init();
    void XCP_CAN_Stop();


signals:

private:
    void preSession();
    void afterSession();

private:
    nxStatus_t status_RES_R = 0;
    nxStatus_t status_RES_R_Stream = 0;
    nxStatus_t status_CMD_W = 0;
    nxStatus_t status_EVENT_R_Stream = 0;

    nxSessionRef_t sessionRef_RES_R = 0;
    nxSessionRef_t sessionRef_RES_R_Stream = 0;
    nxSessionRef_t sessionRef_CMD_W = 0;
    nxSessionRef_t sessionRef_EVENT_R_Stream = 0;

    int frameStreamSize = 100;
    int frameStreamSize_EVENT = 100;

    QByteArray temp_path;
    QByteArray temp_name;
    QByteArray temp_port;
    QByteArray temp_cluster;
    QByteArray temp_frames_r;
    QByteArray temp_frames_w;
    QByteArray temp_frames_r_event;

    const char* xcp_dbcPath;
    const char* xcp_dbcName;
    const char* xcp_can_interface;
    const char* xcp_dbcCluster;
    const char* Frame_RES;
    const char* Frame_CMD;
    const char* Frames_EVENT;

};

#endif // XCP_THREAD_H
