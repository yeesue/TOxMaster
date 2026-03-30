#ifndef FUNCTIONWIN_H
#define FUNCTIONWIN_H

#include <QMainWindow>
#include <QStandardItemModel>
#include <QStandardItem>
#include "nixnet.h"
#include "xcpsetdlg.h"
#include "cansetdlg.h"
//#include "udssetdlg.h"
#include "wt3000setdlg.h"
#include "wt5000setdlg.h"
#include "tscansetdlg.h"
#include "zlgcansetdlg.h"
#include "common.h"
#include "globals.h"
#include "xcp_fun_thread.h"
#include "can_fun_thread.h"
//#include "uds_fun_thread.h"
#include "can_fun_thread_ts.h"
#include "can_fun_thread_zlg.h"
#include "wt3000_thread.h"
#include "wt5000_thread.h"
#include "inca_com_fun_thread.h"
#include "mdf_record_thread.h"
#include "tablewin.h"
#include "plotwin.h"
#include "mapwin.h"
#include "scriptwin.h"
//#include "seqscriptwin.h"
#include "pamfactorywin.h"
#include "DockWindow.h"
#include "autoexewin.h"

namespace Ui {
class FunctionWin;
}

class FunctionWin : public QMainWindow
{
    Q_OBJECT

public:
    explicit FunctionWin(QWidget *parent = nullptr, QString name = "");
    ~FunctionWin();

    QList<FunStruct *> getFunList() const;

    void setFunList(QList<FunStruct *> value);

    void setA2lProjectWin(A2LProjectWin *value);

    void setWorkName(const QString &value);

    QString getWinConfigPath() const;

    void setWinConfigPath(const QString &value);

    QString getWorkWinFilePath() const;
    void setWorkWinFilePath(const QString &value);

public slots:
    void Slt_TableWinDeleted(QWidget* win);
    void Slt_CalTableWinDeleted(QWidget* win);
    void Slt_PlotWinDeleted(QWidget* win);

    void Slt_WinUpdated();

signals:
    void runActive(bool);
    void recordActive(bool);
    void funUpdated();
    void workStateUpdated(QString, int);

private slots:
    void Slt_ContextMenuRequest(const QPoint& pos);
    void Slt_ModelDataUpdated(int row, int col, QString text);
    void Slt_DoubleClicked(QModelIndex index);

    void Slt_FunStateChanged(int state, int funIndex);
    void Slt_XcpMsg(QString msg);

    void Slt_CharValueUpdated(A2L_VarChar *charVar, qreal value);
    void Slt_PamValueUpdated(PARAM *pam, qreal value);

    void Slt_New(bool flag);
    void Slt_Open(bool flag);
    void Slt_Del(bool flag);

    void Slt_ShowRecordTimeInTimeEdit(QString timeStr);
    void Slt_cycleNumUpdated(quint32 curCycle);

    void on_actionDevices_triggered();

    void on_actionRun_triggered();

    void on_actionStop_triggered();

    void on_actionAdd_Table_triggered();

    void on_actionAdd_Cal_Table_triggered();

    void on_actionAdd_Chart_triggered();

    void on_actionFunRun_triggered();

    void on_actionFunStop_triggered();

    void on_actionRcdOn_triggered();

    void on_actionRcdOff_triggered();

    void on_actionSaveWin_triggered();

    void on_actionshowAllWin_triggered();

    void on_actionMapWin_triggered();

    void on_actionScript_triggered();

    void on_actionPamFactory_triggered();

    void on_actionRcdSet_triggered();

    void on_actionImportPams_triggered();

    void on_actionDockView_triggered();

    void on_actionAuto_triggered();

private:
    void initFunMenuStyle();
    void initFunTreeStyle();

    QStringList getNIXnetIntfs();
    void showNIXnetIntfs(QStringList intfs);

    QStringList getTSCanIntfs();
    void showTSCanIntfs(QStringList intfs);

    void showFunctions();
    QList<QStandardItem*> packFunInfoToItems(FunStruct *fun);

    int findIntfIndexByName(QString intfName);

    QString genDefaultTableWinName();
    QString genDefaultCalTableWinName();
    QString genDefaultChartWinName();

    TableWin *addReadTable(QString winName);
    TableWin *addWriteTable(QString winName);
    PlotWin *addPlotWin(QString winName);

    bool writeWinXml();
    bool readWinXml();
    void loadRecordSetting();

    void packGlobalPamHash();
    void packGlobalMapPamHash();

    void runAllFun();
    void stopAllFun();

    void setAllFunDaqRun(bool runFlg);

    void waitForXcpThreadStartFinished(Xcp_Fun_Thread *xcpFunThread, quint32 waitTime_ms);
    //void waitForUdsThreadStartFinished(Uds_Fun_Thread *udsFunThread, quint32 waitTime_ms);
    void waitForCanThreadStartFinished(Can_Fun_Thread *canFunThread, quint32 waitTime_ms);
    void waitForTsCanThreadStartFinished(Can_Fun_Thread_TS *canFunThread, quint32 waitTime_ms);
    void waitForZlgCanThreadStartFinished(Can_Fun_Thread_ZLG *canFunThread, quint32 waitTime_ms);
    void waitForRecordFinished(MDF_Record_Thread *mdfRecordThread, quint32 waitTime_ms);

    void waitForWT3000ThreadStartFinished(WT3000_Thread *wt3000FunThread, quint32 waitTime_ms);
    void waitForWT5000ThreadStartFinished(WT5000_Thread *wt5000FunThread, quint32 waitTime_ms);

    void sleep(int msec);

    void showCharMapsInMapWin();

    void initAndActiveMdfRecord();
    void endMdfRecord();
    QList<PARAM*> fromMeasToPams(QList<A2L_VarMeas*> measPamList, quint32 &blockSize);

    void initSysPams();

private:
    Ui::FunctionWin *ui;

    QString workName = "";

    QString workWinFilePath = "";

    QStandardItemModel *model_function = NULL;

    bool funRunning = false;
    QList<FunStruct*> funList;
    QHash<int, QThread*> funThreadHash;

    A2LProjectWin *a2lProjectWin = NULL;

    bool winShow = false;
    QString winConfigPath = "";
    QHash<QString, QWidget*> winHash;
    QHash<QString, QAction*> actionHash;
    QList<TableWin*> tableWinList;
    QList<TableWin*> calTableWinList;
    QList<PlotWin*> plotWinList;

    QHash<QString, QList<A2L_VarMeas*>> funMeasListHash;
    QHash<QString, QList<A2L_VarChar*>> funCharListHash;
    QHash<QString, QList<A2L_VarChar*>> funMapCharListHash;
    QHash<QString, QList<PARAM*>> funPamListHash;
    QHash<QString, QList<PARAM*>> funWritePamListHash;
    QHash<QString, QList<PARAM*>> funMapPamListHash;

    QHash<QString, QHash<A2L_VarChar*, QSharedMemory*>> xcpFunMapSmHash;

    QStringList funTypeArray = {"XCP", "CAN/CANFD", "UDS", "WT3000", "WT5000", "INCA_COM"};
    QStringList devTypeArray = {"NI-XNET", "TS-CAN", "ZLG-CAN"};
    QStringList canPortArray = {"CAN0", "CAN1", "CAN2", "CAN3", "CAN4", "CAN5",
                                "CAN6", "CAN7", "CAN8", "CAN9", "CAN10", "CAN11",
                                "CAN12", "CAN13", "CAN14", "CAN15", "CAN16", "CAN17",
                                "CAN18", "CAN19", "CAN20"};

    bool recordOn = false;
    int recordRate_ms = 100;
    MDF_Record_Thread *mdfRecordIns = NULL;
    QThread *recordThread = NULL;
    QString mdfFileName = "mdf_data";
    RecordStruct rcdSet;

    MapWin *mapWin = NULL;

    ScriptWin *scriptWin = NULL;

    AutoExeWin *autoExeWin = NULL;

    PamFactoryWin *pamFactoryWin = NULL;

    DockWindow *dockWin = NULL;

    AUTO_PAM *autoPam = NULL;
    SYS_PAM *sysPam = NULL;

};

#endif // FUNCTIONWIN_H
