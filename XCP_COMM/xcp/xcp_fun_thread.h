#ifndef XCP_FUN_THREAD_H
#define XCP_FUN_THREAD_H

#include <QThread>
#include <QDomDocument>
#include "xcpmaster.h"
#include "a2l_varchar.h"
#include "a2l_varmeas.h"
#include "charpamcheck.h"
#include "xcp_polling_thread.h"
#include "a2lprojectwin.h"
#include "param.h"
#include "common.h"
#include "calicsv.h"

class Xcp_Fun_Thread : public QThread
{
    Q_OBJECT
public:
    Xcp_Fun_Thread(QObject *parent = 0, A2L_Project *a2lProject = nullptr);
    ~Xcp_Fun_Thread();

    void preFun();

    void setCharsDownloadActive();

    void setMdfRecordStatus(bool status);

    void setA2lWin(A2LProjectWin *value);

    void setCurA2LProject(A2L_Project *value);

    void setPollingRun(bool value);

    void setDaqRun(bool value);

    bool getXcpSetupOk() const;

    bool getXcpCanInitOk() const;

    XCPMaster *getXcpMaster() const;

    XCP_Polling_Thread *getXcpPollThread() const;

    void setA2lEventList(const QList<A2L_Event *> &value);

    void setA2lEventCanIdList(const QList<A2L_EventCanId *> &value);

    void setXcpConfig(const XcpConfig &value);

    void setFunInfo(FunStruct *value, int index);


    QList<A2L_VarMeas *> getMeasPamList() const;

    QList<A2L_VarMeas *> getDaqMeasPamList() const;

    QList<PARAM *> getPamList() const;

    QList<PARAM *> getDaqPamList() const;

    QList<PARAM *> getCharValuePamList() const;

    QList<PARAM *> getCharCurveMapPamList() const;

    QList<A2L_VarChar *> getCharPamList() const;

    QList<A2L_VarChar *> getCharMapPamList() const;

    QString getSmKeyRead() const;

    QString getSmKeyReadDAQ() const;

    QString getSmKeyWrite() const;

    QString getMapListKey() const;

    bool getXcpStartFinished() const;

    QHash<A2L_VarChar *, QSharedMemory *> getSmMapWriteHash() const;

    int getXcpState() const;

public slots:
    void updatePamValueInSM(PARAM* charPam, double value);
    void updateCharValueInSM(A2L_VarChar* charVar, double value);

    void updatePamMapValueInSM(PARAM* charMapPam, int index_x, int index_y, double value);
    void updateCharMapValueInSM(A2L_VarChar* charVar, int index_x, int index_y, double value);

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

    void xcpFunStateChanged(int, int); // -1 = error, 0 = stop, 1 = canInit, 2 = xcpSetup, 3 = daqRun

    void sigDownload();
    void pollingStatusActive(bool);
    void daqRunStatusActive(bool);

    void recordTimeUpdated(QString);
    void recordActive(bool);

private slots:
    void fromReadSMToMeasVars();
    void fromReadRawDataToMeasVars(char *data, quint64 size);

private:
    bool readSetting();
    void loadA2lProject();
    void getA2LPamsByNames();

    bool readSetting_simple();
    void importA2LPamsFromCsv();


    //void importMeasFromExcel(QString excelPath, QList<A2L_VarMeas *> &measList, quint64 &byteSize);
    //void importCharsFromExcel(QString excelPath, QList<A2L_VarChar *> &charList, quint64 &byteSize);


    bool importMeasFromCsv(QString csvPath, QList<A2L_VarMeas *> &measList, quint64 &byteSize);
    bool importCharsFromCsv(QString csvPath, QList<A2L_VarChar *> &charList, quint64 &byteSize);

    bool createPams();

    QList<CalInfo> importCalInfoListFromCsv(QString csvPath);
    QList<A2L_VarChar*> getCharVarListFromCalInfoList(A2L_Project *a2lProject, QList<CalInfo> calList);
    bool downloadCalList(A2L_Project *a2lProject, QList<CalInfo> calList);

    bool charVars_Download(QList<A2L_VarChar*> charVars);

    void init();
    void startXCP();
    void stopXCP();

    bool xcpDaqConfig();
    bool setXcpDaqStartStop(bool status);


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

    void updateMapValueInSM(A2L_VarChar *map, quint16 index_x, quint16 index_y, char *data);


private:
    FunStruct *fun = NULL;
    int funIndex = -1;
    XcpConfig xcpConfig;

    bool xcpCanInitOk = false;
    bool xcpSetupOk = false;
    bool xcpPollingOk = false;
    bool xcpCaliOk = false;

    bool daqRun = true;
    bool pollingRun = true;

    bool xcpStartFinished = false;
    bool xcpStopFinished = false;
    int xcpState = 0;

    A2LProjectWin *a2lWin = NULL;
    A2L_Project *curA2LProject = NULL;

    QString calCsvPath = "";
    QList<CalInfo> calInfoList;

    QList<A2L_Event*> a2lEventList;
    QList<A2L_EventCanId*> a2lEventCanIdList;

    XCPMaster *xcpMaster = NULL;
    XCP_Polling_Thread *xcpPollThread = NULL;
    CharPamCheck *charPamCheckThread = NULL;
    MapCharPamCheckThread *mapCharPamCheckThread = NULL;

    QList<A2L_VarMeas*> measPamList;
    quint64 sizeRead = 0;
    QSharedMemory *smRead = NULL;
    QString smKeyRead = "";

    QList<A2L_VarMeas*> daqMeasPamList;
    quint64 sizeReadDAQ;
    QSharedMemory *smReadDAQ = NULL;
    QString smKeyReadDAQ = "";

    QList<A2L_VarChar*>  charPamList;
    quint64 sizeWrite;
    QSharedMemory *smWrite = NULL;
    QString smKeyWrite = "";

    QList<A2L_VarChar*>  csvCharPamList;
    quint64 sizeWriteCsv;

    QList<A2L_VarChar*>  charMapPamList;
    QStringList smMapKeyList;
    QList<QSharedMemory*> smMapWriteList;
    QHash<A2L_VarChar*, QSharedMemory*> smMapWriteHash;
    QString mapListKey = "";

    QStringList dataTypeEnum = {"UBYTE", "SBYTE", "UWORD", "SWORD",
                               "ULONG", "SLONG", "A_UINT64", "A_INT64",
                               "FLOAT32_IEEE", "FLOAT64_IEEE"};

    QStringList charTypeList = {"VALUE", "VAL_BLK", "CURVE", "MAP"};
};

#endif // XCP_FUN_THREAD_H
