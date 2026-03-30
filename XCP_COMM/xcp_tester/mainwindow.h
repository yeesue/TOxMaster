#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTableWidgetItem>
#include <QLineEdit>
#include <QThread>
#include <QFile>
#include <QDomDocument>
#include <QDir>
#include <QLabel>
#include <QHeaderView>
#include <QStandardItemModel>
#include <QTableView>
#include <QProgressBar>
#include <QFileDialog>
#include <QAxObject>
#include "globals.h"
#include "db_testbed.h"
#include "exceloperator.h"
#include "logwin.h"
#include "mapwin.h"
#include "workspacewin.h"

#include "settingwin.h"
#include "param.h"
#include "xcp_polling_thread.h"
#include "mdf_record_thread.h"
#include "delegate.h"
#include "calmanagewin.h"
#include "configwin.h"
#include "xcp_main_thread.h"

#include "plotwin.h"

#define LED_SIZE 20

namespace Ui {
class MainWindow;
}


struct TypeInfo{
    int Type = 0;
    int LengthBit = 0;

};

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

    void loadCurrentSetting();
    void loadCurrentSetting_2nd();

    //void configDevice();
    //void configDevice_2nd();


    void runDevice();
    void stopDevice();

    void startXCP();

signals:
    void writePamValueUpdated();
    void mdfRecordDataUpdated(char *data, int size);
    void xcpConnectedStatusChanged(bool connected);

    void recordActive(bool);

public slots:

protected:
    void closeEvent(QCloseEvent *event);

private slots:

    void sltXcpMsg(QString msg);
    void sltXcpCanInitStatus(int status);
    void sltXcpConnStatus(int status);
    void sltXcpCaliStatus(int status);
    void sltXcpPollingStatus(int status);
    void sltXcpDaqRunStatus(int status);

    void sltXcpMsg_2nd(QString msg);
    void sltXcpCanInitStatus_2nd(int status);
    void sltXcpConnStatus_2nd(int status);
    void sltXcpCaliStatus_2nd(int status);
    void sltXcpPollingStatus_2nd(int status);
    void sltXcpDaqRunStatus_2nd(int status);

    void updateCharValueInSM(A2L_VarChar *charVar, qreal value);
    void modelDataUpdatedSlot(int rowIndex, double value);
    void sltModelDataUpdated(int rowIndex, double value);

    void modelDataUpdatedSlot_2nd(int rowIndex, double value);
    void sltModelDataUpdated_2nd(int rowIndex, double value);

    void readSetting_Simple();
    void writeSetting_Simple();

    void readSetting_Simple_2nd();
    void writeSetting_Simple_2nd();

    void readCurSetting();
    void readCurSetting_2nd();
    TypeInfo getPamInfoFromType(QString type);
    void getA2LPamsByNames();    
    void getA2LPamsByNames_2nd();

    void showMeasAndCharsInTable();
    void showMeasAndCharsInTable_2nd();

    void showMeasAndCharsInTableView();

    void showCharMapsInMapWin();
    void showCharMapsInMapWin_2nd();

    void SltDataUpdateInTable();
    void SltDataUpdateInTable_2nd();

    QString getHexDataString(char *data, quint64 size);

    void fromReadRawDataToPamValue(char *data, quint64 size);
    void fromReadSMToPamValue();
    void fromPamsToWriteRawData(char *data, quint64 size);
    void fromPamsToWriteSM();

    void fromReadSMToMeasVars();
    void fromReadRawDataToMeasVars(char *data, quint64 size);
    void fromCharVarsToWriteSM();
    void fromCharVarsToWriteRawData(char *data, quint64 size);

    void fromMapCharVarsToMapWriteSM();

    bool charVars_Download(QList<A2L_VarChar*> charVars);

    int from_intel2userdata(char *data, quint64 size, int startbit, int lenbit, quint64 *userdata);
    void value2IntelData(char *data, PARAM* pam);
    quint8 rotateLeft(quint8 val, quint8 n);

    void value2IntelData(char *data, A2L_VarChar* charVar);

    PARAM *findPamByName(QList<PARAM*> pamList, QString pamName);
    A2L_VarChar *findCharVarByName(QList<A2L_VarChar*> charPamList, QString charName);

    QString getPollReadTime();
    QString getDaqReadTime();
    QString getCaliWriteTime();

    QString getPollReadTime_2nd();
    QString getDaqReadTime_2nd();
    QString getCaliWriteTime_2nd();

    bool genXML();
    QList<PARAM*> transferMeasToPams(QList<A2L_VarMeas*> measPamList);
    QList<PARAM*> fromMeasToPams(QList<A2L_VarMeas*> measPamList, quint32 &blockSize);
    QHash<quint16, QList<PARAM*>> getPamHashFromMeasHash(QHash<quint16, QList<A2L_VarMeas*>> measHash);
    QHash<QString, QList<PARAM*>> getdgPamHashFromdgMeasHash(QHash<QString, QList<A2L_VarMeas*>> measHash);

    void showRecordTimeInTimeEdit(QString timeStr);

    void createSM();
    void deleteSM();
    void createSMInMAP();
    void deleteSMInMAP();

    void killModrive();

    void setAcceptSlot();
    void setLED(QLabel *label, int color, int size);

    void pollingStatusChangedSlot(bool pollingStatus);
    void caliStatusChangedSlot(bool caliStatus);

    void pollingStatusChangedSlot_2nd(bool pollingStatus);
    void caliStatusChangedSlot_2nd(bool caliStatus);

    void on_actionLoad_triggered();
    void on_actionSetting_triggered();
    void on_actionConnect_triggered();    
    void on_actionDisconnect_triggered();

    void on_actionRaw_triggered();
    void on_actionA2l_triggered();

    void on_actionVisual_triggered();
    void on_actionStop_triggered();

    void on_actionRecord_triggered();

    void on_actionMap_triggered();

    void transferPhyValueToRawData(A2L_VarChar *charVar, double value, char *data);

    //use excel as setting-save
    bool delExcel(QString filePath);
    void convertToColName(int data, QString &res);
    QString to26AlphabetString(int data);

    bool exportMeasToExcel(QList<A2L_VarMeas*> meaVars, QString excelPath);
    bool exportCharsToExcel(QList<A2L_VarChar*> charVars, QString excelPath);
    bool exportXcpPamsToExcel();

    void on_actionSaveToExcel_triggered();

    void getA2LPamsFromExcel();
    void getA2LPamsFromExcel_2nd();
    void importMeasFromExcel(QString excelPath, QList<A2L_VarMeas *> &measList, quint64 &byteSize);
    void importCharsFromExcel(QString excelPath, QList<A2L_VarChar *> &charList, quint64 &byteSize);

    void on_actionFromExcel_triggered();

    void on_actionXcpOn_triggered();
    void on_actionXcpOff_triggered();
    void on_actionUnlock_triggered();
    void on_actionCan_triggered();
    void on_actionExt_triggered();
    void on_actionShow_triggered();
    void on_actionCanBind_triggered();

    //use database as setting-save
    void getA2LPamsFromDB();
    void getMeasPamsFromDB();
    void getCharPamsFromDB();
    void on_actionfromDB_triggered();

    bool saveMeasToDB();
    bool saveCalisToDB();
    void on_actiontoDB_triggered();

    void on_actionCalCsv_triggered();

    void on_actionLog_triggered();

    void on_actionConfigWin_triggered();

    void on_actionChart_triggered();

    void on_actionGauge_triggered();

    void on_actionXCP_On_1st_triggered();

    void on_actionXCP_Off_1st_triggered();

    void on_actionXCP_Load_1st_triggered();

    void on_actionXCP_Set_1st_triggered();

    void on_actionXCP_On_2nd_triggered();

    void on_actionXCP_Off_2nd_triggered();

    void on_actionXCP_Load_2nd_triggered();

    void on_actionXCP_Set_2nd_triggered();

    void on_actionRcdOn_triggered();

    void on_actionRcdOff_triggered();

    void on_actionRcdSet_triggered();

    void on_actionVisual_2nd_triggered();

    void on_actionStop_2nd_triggered();

    void on_actionCalCsv_2nd_triggered();

    void on_actionMap_2nd_triggered();

    void on_actionXCP1_On_triggered();

    void on_actionXCP1_Off_triggered();


    void on_actionWorkMagWin_triggered();

private:
    void initMdfRecord();
    void endMdfRecord();

private:
    Ui::MainWindow *ui;

    LogWin *logWin = NULL;

    Proj curProj;

    QList<PARAM*> readPamList;
    quint64 sizeRead;
    QSharedMemory *smRead = NULL;
    QString smKeyRead = "";
    QList<A2L_VarMeas*> measPamList;
    QHash<A2L_VarMeas*, QSharedMemory*> measSMHash;

    QList<PARAM*> daqReadPamList;
    quint64 sizeReadDAQ;
    QSharedMemory *smReadDAQ = NULL;
    QString smKeyReadDAQ = "";
    QList<A2L_VarMeas*> daqMeasPamList;

    QList<PARAM*> writePamList;
    quint64 sizeWrite;
    QSharedMemory *smWrite = NULL;
    QString smKeyWrite = "";
    QList<A2L_VarChar*> charPamList;
    QList<A2L_VarChar*> charMapPamList;

    QStringList smMapKeyList;
    QList<QSharedMemory*> smMapWriteList;
    QHash<A2L_VarChar*, QSharedMemory*> smMapWriteHash;

    bool showRawData = false;
    bool xcpCanInitOk = false;
    bool xcpSetupOk = false;
    bool xcpPollingOk = false;
    bool xcpCaliOk = false;

    QTimer *updateTimer = NULL;
    int updateRate_ms = 333;

    SettingWin *setWin = NULL;
    SettingWin *setWin_2nd = NULL;
    A2LProjectWin *a2lWin = NULL;

    //xcp 1st
    A2L_Project *curA2LProject = NULL;
    XCP_Main_Thread *xcpMainThread = NULL;
    XCPMaster *xcpMaster = NULL;
    XCP_Polling_Thread *xcpPollThread = NULL;
    CharPamCheck *charPamCheckThread = NULL;
    MapCharPamCheckThread *mapCharPamCheckThread = NULL;

    quint64 pollStartTime = 0;
    qreal pollTime = 0;
    quint64 daqStartTime = 0;
    qreal daqTime = 0;
    quint64 caliStartTime = 0;
    qreal caliTime = 0;

    QList<A2L_Event*> eventList;
    QList<A2L_EventCanId*> eventCanIdList;

    //xcp 2nd
    A2L_Project *curA2LProject_2nd = NULL;
    XCP_Main_Thread *xcpMainThread_2nd = NULL;
    XCPMaster *xcpMaster_2nd = NULL;

    quint64 pollStartTime_2nd = 0;
    qreal pollTime_2nd = 0;
    quint64 daqStartTime_2nd = 0;
    qreal daqTime_2nd = 0;
    quint64 caliStartTime_2nd = 0;
    qreal caliTime_2nd = 0;

    QList<A2L_Event*> eventList_2nd;
    QList<A2L_EventCanId*> eventCanIdList_2nd;

    Proj curProj_2nd;

    QList<PARAM*> readPamList_2nd;
    quint64 sizeRead_2nd;
    QString smKeyRead_2nd = "";
    QList<A2L_VarMeas*> measPamList_2nd;

    QList<PARAM*> daqReadPamList_2nd;
    quint64 sizeReadDAQ_2nd;
    QString smKeyReadDAQ_2nd = "";
    QList<A2L_VarMeas*> daqMeasPamList_2nd;

    QList<PARAM*> writePamList_2nd;
    quint64 sizeWrite_2nd;
    QString smKeyWrite_2nd = "";
    QList<A2L_VarChar*> charPamList_2nd;
    QList<A2L_VarChar*> charMapPamList_2nd;

    QStringList smMapKeyList_2nd;
    QList<QSharedMemory*> smMapWriteList_2nd;
    QHash<A2L_VarChar*, QSharedMemory*> smMapWriteHash_2nd;

    //////

    QStandardItemModel readModel;
    QStandardItemModel writeModel;
    QTableView readTableView;
    QTableView writeTableView;
    DoubleSpinBoxDelegate delegate;

    QProgressBar *progBar = NULL;

    bool recordOn = false;
    int recordRate_ms = 100;
    MDF_Record_Thread *mdfRecordIns = NULL;
    QThread *recordThread = NULL;
    QString mdfFileName = "Default_mdf";

    MapWin *mapWin = NULL;
    MapWin *mapWin_2nd = NULL;

    QStringList dataTypeEnum = {"UBYTE", "SBYTE", "UWORD", "SWORD",
                               "ULONG", "SLONG", "A_UINT64", "A_INT64",
                               "FLOAT32_IEEE", "FLOAT64_IEEE"};

    bool showActions = true;
    QString verifyPassword = "LyxCspLqt_@_#_&_122505";


    bool extCal = false;
    bool showData = true;
    bool autoStart = false;

    QProcess *process = NULL;

    DB_Testbed *db = NULL;

    CalManageWin *calMagWin = NULL;
    CalManageWin *calMagWin_2nd = NULL;
    ConfigWin *configWin = NULL;

};

#endif // MAINWINDOW_H
