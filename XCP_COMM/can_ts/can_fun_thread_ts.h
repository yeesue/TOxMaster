#ifndef CAN_FUN_THREAD_TS_H
#define CAN_FUN_THREAD_TS_H

#include <QThread>
#include <QDomDocument>
#include "param.h"
#include "arxmlparser.h"
#include "xml_gen.h"
#include "can_thread_ts.h"

class  Can_Fun_Thread_TS : public QThread
{
    Q_OBJECT
public:
    Can_Fun_Thread_TS(QObject *parent = 0);
    ~Can_Fun_Thread_TS();

    void setFunInfo(FunStruct *value, int index);

    QHash<QString, QList<PARAM *> > getReadPamListHash() const;

    bool getCanStartFinished() const;


    QHash<QString, QList<PARAM *> > getWritePamListHash() const;

public slots:

protected:
    void run();

signals:
    void canFunStateChanged(int, int);
    void canDataForRecord(quint8*, quint32, QString);

private slots:
    void Slt_CanStateChanged(quint32, Can_Thread_TS::CAN_STATE);
    void Slt_ReadFrameRawDataReceived(quint32 id, quint8* data, quint8 len);

    void Slt_FrameSigValueUpdated();

    void Slt_WriteFrameSigValueUpdated();

private:
    void init();
    bool startCan();
    void stopCan();

    void waitForCanThreadStateReady(Can_Thread_TS *thread, quint32 waitTime_ms);
    void wait_ms(int msec);

    BO_* getBoById(quint32 id);
    SG_* getSgByName(BO_* bo, QString sgName);
    bool loadXml();

    void setCanCommInfo();

    void loadDbc(QString dbcFile);

    dbc_Cluster* loadDbcFromArxml(QString filePath);

    QList<PARAM*> fromDbcFrameSigsToPams(BO_ *frame, QString key);
    void packFramesIntoPamsHash();


private:
    FunStruct *fun = NULL;
    int funIndex = -1;
    CanConfig canConfig;

    Can_Thread_TS::CAN_STATE curState = Can_Thread_TS::UNCONN;

    Can_Thread_TS *tscanIns = NULL;
    CLUSTER_ *m_cluster = NULL;

    QString deviceType = "";
    int deviceIndex = 0;       // device Index
    int canIndex = 0;          // can Index

    QString selReadFrames = "";
    QString selWriteFrames = "";
    QString selReadSigs = "";
    QString selWriteSigs= "";

    u32 canIoMode = 0;
    u32 canfdStandard = 0;
    u64 canBaudrate = 0;
    u64 canfdBaudrate = 0;
    bool terminationEnable = false;

    quint16 sel_read_period = 10;
    quint16 sel_write_period = 100;

    bool enableCustomBD = false;
    bool enableCustomFDBD = false;
    qreal samplePointBD = 87.5;    //87.5%
    qreal ssamplePointFDBD = 87.5;  // 87.5%

    QList<BO_*> dbc_frames_r;
    QList<BO_*> dbc_frames_w;

    QList<PARAM*> readPamList;
    QList<PARAM*> writePamList;

    QHash<QString, QList<PARAM*>> readPamListHash;
    QHash<QString, QList<PARAM*>> writePamListHash;

    QHash<QString, QSharedMemory*> readFrameSmHash;
    QHash<QString, QSharedMemory*> writeFrameSmHash;

    bool canStartFinished = false;

    quint64 timeStamp_on = 0;
    quint64 timeStamp_start = 0;
};

#endif // CAN_FUN_THREAD_TS_H
