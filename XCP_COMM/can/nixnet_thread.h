#ifndef NIXNET_THREAD_H
#define NIXNET_THREAD_H

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
#include "xml_gen.h"
#include "common.h"

using namespace std;

#define DEFAULT_INTF_NUM 20

void displayErrorAndExit(nxStatus_t status, nxSessionRef_t session, const char *source);

////////////////////////////////////////////////////////////////////////////////////////////////////////////
class NIXNET_R_Queue_Thread : public QThread
{
    Q_OBJECT
public:
    explicit NIXNET_R_Queue_Thread(QObject *parent = 0);
    ~NIXNET_R_Queue_Thread();

    void setIsStop(bool value);

    void setReadFrame(dbc_frame *value);

    void setSm(QSharedMemory *value);

    void setCanType(const quint32 &value);

    void setSession_readFrame_Queue(const nxSessionRef_t &value);

    void setIsArxml(bool value);

signals:
    void stateChanged(int);
    void readFrameRawDataReceived(quint32, quint8*, quint8);

protected:
    void run();

private:
    void unpackFrameQueue();

private:
    bool isStop = false;
    bool isArxml = false;

    quint32 canType = 0;  // 0 = can, 1 = canfd

    dbc_frame* readFrame = NULL;
    QSharedMemory* sm = NULL;

    nxSessionRef_t session_readFrame_Queue = 0;

    nxFrameCANFD_t *canfd_frameBuffer_Queue = NULL;
    u32 numOfBytesReturned = 0;
    int frameQueueSize = 10;

    bool errMsgShow = false;

};

////////////////////////////////////////////////////////////////////////////////////////////////////////////
class NIXNET_W_Queue_Thread : public QThread
{
    Q_OBJECT
public:
    explicit NIXNET_W_Queue_Thread(QObject *parent = 0);
    ~NIXNET_W_Queue_Thread();

    void setFrameQueueSize(int value);

    void setSession_WriteFrame_Queue(const nxSessionRef_t &value);

    void setSession_writeSigConv_Queue(const nxSessionRef_t &value);

    void setWriteFrame(dbc_frame *value);

    void setSm(QSharedMemory *value);

    void setIsStop(bool value);

    void setIsArxml(bool value);

    void setCanType(const quint32 &value);

signals:
    void stateChanged(int);

protected:
    void run();

private:
    void init();

    void sigRC(dbc_signal *sig, int min, int max, int step);
    void dataCRC(char *data, int startIndex, int endIndex, int crcIndex);

    bool packTransmitData();
    void copyFromSigConvBufToFrameBuf();
    bool copySndDataFromSmToBuf();
    void updateSndTimeStamp();

private:
    bool isStop = false;
    bool isArxml = false;
    quint32 canType = 0;     // 0 = can, 1 = canfd

    QTime time;
    quint64 sndAbsTime = 0;

    nxSessionRef_t session_WriteFrame_Queue = 0;
    nxSessionRef_t session_writeSigConv_Queue = 0;

    dbc_frame* writeFrame = NULL;
    QSharedMemory* sm = NULL;
    int frameQueueSize = 1;

    u8 *canfd_frameBuffer_Queue = NULL;
    u8 *canfd_frameBuffer_sigConv = NULL;
    u32 numOfBytesForFrames = 0;

    f64 *sigValueBuf = NULL;
    u32 sigNum = 0;
    u32 sizeOfsigValBuf = 0;
    f64 timeout_w = nxTimeout_None;

    bool errMsgShow = false;

};

///////////////////////////////////////////////////////////////////////////////////////////////////////////
class NIXNET_Thread : public QObject
{
    Q_OBJECT
public:
    explicit NIXNET_Thread(QObject *parent = 0);
    ~NIXNET_Thread();

    QString path;
    QString name;
    QString port;
    QString cluster;
    QString frames_r;
    QString frames_w;
    QString sigs_r;
    QString sigs_w;

    u32 canio_mode = 0;
    u64 baudrate = 0;
    u64 canfd_baudrate = 0;
    bool canTermination = false;

    bool customBD_on = false;
    bool customFDBD_on = false;
    qreal samplePoint_BD = 0;
    qreal samplePoint_FDBD = 0;
    bool samplePointError = false;

    bool readSessionCreateError = false;
    bool writeSessionCreateError = false;

    nxDatabaseRef_t databaseRef;
    nxDatabaseRef_t *clusterRef = NULL;
    u32 numOfClusterRef;
    u32 sizeOfClusterRef;
    char* clusterName;
    u32 sizeOfClusterName;
    nxDatabaseRef_t *frameRef = NULL;
    u32 numOfFrameRef;
    u32 sizeOfFrameRef;
    char* frameName;
    u32 sizeOfFrameName;

    QHash<QString, QSharedMemory *> getReadFrameSmHash() const;

    QHash<QString, QSharedMemory *> getWriteFrameSmHash() const;

public slots:
    bool deviceConnect();
    void deviceDisconnect();

    void setFrames(QList<dbc_frame*> frames);
    void setFrames_r(QList<dbc_frame*> frames);
    void setFrames_w(QList<dbc_frame*> frames);

signals:
    void stateChanged(int);
    void readFrameRawDataReceived(quint32, quint8*, quint8);

private:
    void initSession();

    void handleAfterSessionClosed();

    void xnetDbcModify();
    bool setSessionIntfCustomBaudrate(nxSessionRef_t sessionRef, u64 Baudrate, u32 samplePoint);
    bool setSessionIntfCustomFDBaudrate(nxSessionRef_t sessionRef, u64 FD_Baudrate, u32 samplePoint);

private:
    nxSessionRef_t sessionRef_readFrame = 0;
    nxSessionRef_t sessionRef_writeFrame = 0;

    nxSessionRef_t sessionRef_readSigConv = 0;
    nxSessionRef_t sessionRef_writeSigConv= 0;

    QList<nxSessionRef_t> sessionRefList_queueReadFrame;
    QList<nxSessionRef_t> sessionRefList_cyclicSendSigConv;
    QList<nxSessionRef_t> sessionRefList_queueWriteFrame;
    QList<nxSessionRef_t> sessionRefList_queueWriteSigConv;

    int writeFrameQueueSize = 1;

    QByteArray temp_path;
    QByteArray temp_name;
    QByteArray temp_port;
    QByteArray temp_cluster;
    QByteArray temp_frames_r;
    QByteArray temp_frames_w;

    QByteArray temp_sigs_r;
    QByteArray temp_sigs_w;

    const char* dbcPath;
    const char* dbcName;
    const char* can_interface;
    const char* dbcCluster;
    const char* dbcFrames_R;
    const char* dbcFrames_W;
    const char* dbcSigs_R;
    const char* dbcSigs_W;

    QList<dbc_frame*> dbc_frames;
    QList<dbc_frame*> dbc_frames_r;
    QList<dbc_frame*> dbc_frames_w;

    QStringList frames_r_List;
    QStringList frames_w_List;

    QHash<QString, QSharedMemory*> readFrameSmHash;
    QList<NIXNET_R_Queue_Thread*> queueReadThreadList;

    QHash<QString, QSharedMemory*> writeFrameSmHash;
    QList<NIXNET_W_Queue_Thread*> queueWriteThreadList;


};

#endif // NIXNET_THREAD_H
