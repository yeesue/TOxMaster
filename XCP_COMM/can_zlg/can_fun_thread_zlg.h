#ifndef CAN_FUN_THREAD_ZLG_H
#define CAN_FUN_THREAD_ZLG_H

#include <QThread>
#include <QDomDocument>
#include "param.h"
#include "arxmlparser.h"
//#include "xml_gen.h"
#include "can_thread_zlg.h"

class  Can_Fun_Thread_ZLG : public QThread
{
    Q_OBJECT
public:
    Can_Fun_Thread_ZLG(QObject *parent = 0);
    ~Can_Fun_Thread_ZLG();

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
    void Slt_CanStateChanged(quint32, Can_Thread_ZLG::CAN_STATE);
    void Slt_ReadFrameRawDataReceived(quint32 id, quint8* data, quint8 len);

    void Slt_FrameSigValueUpdated();

    void Slt_WriteFrameSigValueUpdated();

private:
    void init();
    bool startCan();
    void stopCan();

    void waitForCanThreadStateReady(Can_Thread_ZLG *thread, quint32 waitTime_ms);
    void wait_ms(int msec);
    
    void initDeviceTypeNumHash();

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

    Can_Thread_ZLG::CAN_STATE curState = Can_Thread_ZLG::UNOPEN;

    Can_Thread_ZLG *zlgcanIns = NULL;
    CLUSTER_ *m_cluster = NULL;
    
    QString modName = "";

    QString deviceType = "";
    quint32 deviceTypeIndex = 0;
    int deviceIndex = 0;       // device Index
    int canIndex = 0;          // can Index

    QString selReadFrames = "";
    QString selWriteFrames = "";
    QString selReadSigs = "";
    QString selWriteSigs= "";

    quint32 canIoMode = 0;
    quint32 canfdStandard = 0;
    quint64 canBaudrate = 0;
    quint64 canfdBaudrate = 0;
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

    QHash<quint32, QSharedMemory*> readSmHash;
    QHash<quint32, QSharedMemory*> writeSmHash;
    
    QHash<QString, QSharedMemory*> readFrameSmHash;
    QHash<QString, QSharedMemory*> writeFrameSmHash;

    bool canStartFinished = false;

    quint64 timeStamp_on = 0;
    quint64 timeStamp_start = 0;
    
    QHash<QString, quint32> deviceTypeNumHash;
};

#endif // CAN_FUN_THREAD_ZLG_H
