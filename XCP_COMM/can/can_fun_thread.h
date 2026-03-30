#ifndef CAN_FUN_THREAD_H
#define CAN_FUN_THREAD_H

#include <QThread>
#include <QDomDocument>
#include "param.h"
#include "nixnet_thread.h"
#include "arxmlparser.h"
#include "xml_gen.h"
#include "can_thread_ts.h"

class  Can_Fun_Thread : public QThread
{
    Q_OBJECT
public:
    Can_Fun_Thread(QObject *parent = 0);
    ~Can_Fun_Thread();

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
    void Slt_CanStateChanged(int state);
    void Slt_ReadFrameRawDataReceived(quint32 id, quint8* data, quint8 len);

    void Slt_ReadFrameSmUpdated(quint32 id, quint8* data, quint8 len);
    void Slt_FrameSigValueUpdated();

    void Slt_WriteFrameSigValueUpdated();

private:
    void init();
    bool startCanComm();
    void stopCanComm();

    bool readSettingXml();
    void setCanCommInfo();

    void loadDatabase(QString dbFilePath);

    void loadDbc(QString filePath);

    void loadArxml(QString filePath);
    dbc_Cluster* loadDbcFromArxml(QString filePath);

    QList<dbc_frame*> getFramesByNames(QStringList frameNameList);

    QList<PARAM*> fromDbcFrameSigsToPams(dbc_frame *frame, QString key);
    void packFramesIntoPamsHash();


private:
    FunStruct *fun = NULL;
    int funIndex = -1;
    CanConfig canConfig;

    int curState = 0;

    NIXNET_Thread *nixnetIns = NULL;

    Can_Thread_TS *tscanIns = NULL;

    XML_GEN xml_ins;


    QStringList frameList;
    QByteArray temp_path;
    QByteArray temp_name;
    char* dbcPath;
    char* dbcName;

    bool isArxml = false;

    QString sel_arxmlPath = "";
    QString sel_arCluster = "";
    QString sel_dbcPath = "";

    QString sel_cluster = "";
    QString sel_interface = "";

    QString sel_frames_r = "";
    QString sel_frames_w = "";
    QString sel_sigs_r = "";
    QString sel_sigs_w = "";

    u64 sel_baudrate = 0;
    u32 sel_canio_mode = 0;
    u64 sel_canfd_baudrate = 0;
    bool sel_termination_on = false;

    quint16 sel_read_period = 10;
    quint16 sel_write_period = 100;

    bool sel_CustomBD_on = false;
    bool sel_CustomFDBD_on = false;
    qreal sel_SamplePoint_BD = 87.5;    //87.5%
    qreal sel_SamplePoint_FDBD = 87.5;  // 87.5%

    QList<dbc_frame*> dbc_frames;
    QList<dbc_frame*> dbc_frames_r;
    QList<dbc_frame*> dbc_frames_w;

    QHash<quint32, dbc_frame*> idFrameHash;
    QHash<quint32, dbc_frame*> idSelFrameHash;

    QList<PARAM*> readPamList;
    QList<PARAM*> writePamList;

    QHash<QString, QList<PARAM*>> readPamListHash;
    QHash<QString, QList<PARAM*>> writePamListHash;

    QHash<QString, QSharedMemory*> readFrameSmHash;
    QHash<QString, QSharedMemory*> writeFrameSmHash;


    QHash<QString, dbc_Cluster*> clusterHash;
    dbc_Cluster *curCluster = NULL;

    int indexOfEditSig = -1;

    bool runFlag = false;
    bool canSettingLoad = false;
    bool fdReBuildFlag = true;
    bool parserOnArxml = false;
    bool canStartFinished = false;

    QStringList genDbcFileList;
    QHash<QString, QString> clusterDbcHash;
    AR_XML_PACKAGES *arXmlPackages = NULL;
    QList<dbc_Cluster*> canClusters;

    quint64 timeStamp_on = 0;
    quint64 timeStamp_start = 0;

    //QTimer *timer = NULL;
};

#endif // CAN_FUN_THREAD_H
