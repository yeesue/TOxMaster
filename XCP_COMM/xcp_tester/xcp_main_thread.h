#ifndef XCP_MAIN_THREAD_H
#define XCP_MAIN_THREAD_H

#include <QThread>
#include <QDomDocument>
#include "xcpmaster.h"
#include "a2l_varchar.h"
#include "a2l_varmeas.h"
#include "charpamcheck.h"
#include "xcp_polling_thread.h"
#include "settingwin.h"
#include "a2lprojectwin.h"
#include "mdf_record_thread.h"

class XCP_Main_Thread : public QThread
{
    Q_OBJECT
public:
    XCP_Main_Thread(QObject *parent = 0, A2LProjectWin *win = nullptr);
    ~XCP_Main_Thread();

    void setCharsDownloadActive();

    void setMdfRecordStatus(bool status);

    void setMeasPamList(const QList<A2L_VarMeas *> &value);

    void setDaqMeasPamList(const QList<A2L_VarMeas *> &value);

    void setCharPamList(const QList<A2L_VarChar *> &value);

    void setCharMapPamList(const QList<A2L_VarChar *> &value);

    void setDownloadCharList(const QList<A2L_VarChar *> &value);

    void setA2lWin(A2LProjectWin *value);

    QList<A2L_VarMeas *> getMeasPamList() const;

    QList<A2L_VarMeas *> getDaqMeasPamList() const;

    QList<A2L_VarChar *> getCharPamList() const;

    QList<A2L_VarChar *> getCharMapPamList() const;

    void setCurProj(Proj value);

    void setCurA2LProject(A2L_Project *value);

    void setSmKeyRead(const QString &value);

    void setSmKeyReadDAQ(const QString &value);

    void setSmKeyWrite(const QString &value);


    void setSizeRead(const quint64 &value);

    void setSizeReadDAQ(const quint64 &value);

    void setSizeWrite(const quint64 &value);

    void setPollingRun(bool value);

    void setDaqRun(bool value);


    bool getXcpSetupOk() const;

    bool getXcpCanInitOk() const;

    XCPMaster *getXcpMaster() const;

    XCP_Polling_Thread *getXcpPollThread() const;

    void setA2lEventList(const QList<A2L_Event *> &value);

    void setA2lEventCanIdList(const QList<A2L_EventCanId *> &value);

public slots:
    void updateCharValueInSM(A2L_VarChar* charVar, double value);


protected:
    void run();

signals:
    void pollingStatusChanged(bool);
    void caliStatusChanged(bool);

    void writePamValueUpdated();

    void xcpMsg(QString);
    void xcpCanInitStatus(int);
    void xcpConnStatus(int);
    void xcpCaliStatus(int);
    void xcpPollingStatus(int);
    void xcpDaqRunStatus(int);

    void sigDownload();
    void pollingStatusActive(bool);
    void daqRunStatusActive(bool);

    void recordTimeUpdated(QString);
    void recordActive(bool);

private slots:
    void fromReadSMToMeasVars();
    void fromReadRawDataToMeasVars(char *data, quint64 size);

private:
    //void readCurSetting();
    bool genXML();

    bool charVars_Download(QList<A2L_VarChar*> charVars);

    void init();
    void startXCP();
    void stopXCP();

    void initMdfRecord();

    bool xcpDaqConfig();
    bool setXcpDaqStartStop(bool status);

    void getA2LPamsByNames();
    void createSM();
    void deleteSM();
    void createSMInMAP();
    void deleteSMInMAP();

    void fromCharVarsToWriteSM();
    void fromCharVarsToWriteRawData(char *data, quint64 size);

    void fromMapCharVarsToMapWriteSM();

    void transferPhyValueToRawData(A2L_VarChar *charVar, double value, char *data);
    void value2IntelData(char *data, A2L_VarChar* charVar);

    QHash<quint16, QList<PARAM*>> getPamHashFromMeasHash(QHash<quint16, QList<A2L_VarMeas*>> measHash);
    QList<PARAM*> transferMeasToPams(QList<A2L_VarMeas*> measPamList);

private:
    bool xcpCanInitOk = false;
    bool xcpSetupOk = false;
    bool xcpPollingOk = false;
    bool xcpCaliOk = false;

    bool daqRun = true;
    bool pollingRun = true;

    Proj curProj;
    A2LProjectWin *a2lWin = NULL;
    A2L_Project *curA2LProject = NULL;

    QList<A2L_Event*> a2lEventList;
    QList<A2L_EventCanId*> a2lEventCanIdList;

    XCPMaster *xcpMaster = NULL;
    XCP_Polling_Thread *xcpPollThread = NULL;
    CharPamCheck *charPamCheckThread = NULL;
    MapCharPamCheckThread *mapCharPamCheckThread = NULL;

    MDF_Record_Thread *mdfRecordIns = NULL;
    QThread *recordThread = NULL;

    QList<A2L_VarMeas*> measPamList;
    quint64 sizeRead = 0;
    QSharedMemory *smRead = NULL;
    QString smKeyRead = "";
    QHash<A2L_VarMeas*, QSharedMemory*> measSMHash;

    QList<A2L_VarMeas*> daqMeasPamList;
    quint64 sizeReadDAQ;
    QSharedMemory *smReadDAQ = NULL;
    QString smKeyReadDAQ = "";

    QList<A2L_VarChar*>  charPamList;
    quint64 sizeWrite;
    QSharedMemory *smWrite = NULL;
    QString smKeyWrite = "";

    QList<A2L_VarChar*>  charMapPamList;
    QStringList smMapKeyList;
    QList<QSharedMemory*> smMapWriteList;
    QHash<A2L_VarChar*, QSharedMemory*> smMapWriteHash;

    QStringList dataTypeEnum = {"UBYTE", "SBYTE", "UWORD", "SWORD",
                               "ULONG", "SLONG", "A_UINT64", "A_INT64",
                               "FLOAT32_IEEE", "FLOAT64_IEEE"};
};

#endif // XCP_MAIN_THREAD_H
