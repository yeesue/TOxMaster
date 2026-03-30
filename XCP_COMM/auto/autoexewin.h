#ifndef AUTOEXEWIN_H
#define AUTOEXEWIN_H

#include <QMainWindow>
#include <QScriptEngine>
#include <QDir>
#include <QFile>
#include <QFileDialog>
#include <QStandardItemModel>
#include <QMessageBox>
#include "scriptobj.h"
#include "logwin.h"
#include "sys_pam.h"
#include "auto_pam.h"
#include "auto_win.h"
#include "csv_win.h"
#include "csvsettingdlg.h"

namespace Ui {
class AutoExeWin;
}

struct AutoINFO{
    QString autoFilePath;
    QStandardItemModel *fileModel;
    quint32 startStep;
    quint32 endStep;
    quint32 curStep;
    quint32 nextStep;
};

class AutoExeWin : public QMainWindow
{
    Q_OBJECT

public:
    explicit AutoExeWin(QWidget *parent = nullptr, QString name = "");
    ~AutoExeWin();

    void setWorkName(const QString &value);

    void setAutoPam(AUTO_PAM *value);

    void setSysPam(SYS_PAM *value);

private slots:
    void getCmdScriptFiles();
    void getAutoFiles();
    void getJsScriptFiles();

    void openAutoFilePath(QString autoFilePath);
    void openSubAutoFilePath(QString autoFilePath);

    void initTable();
    void initSubTable();


    void autoFileRun();
    void timeoutHandle();

    //void autoTestRun_V2();
    //bool cmdExecuteWithArgs(QString cmd, QStringList args);

    void autoTestRun();
    void progCall(QString progName);

    void packObjects(QScriptEngine *engine);
    bool scriptFunExecuteWithArgs(QString scriptFile, QString scriptFilePath, QScriptValueList args);
    bool scriptExecute(QString scriptFile, QString scriptFilePath);


    void showMenuInFTable(QPoint pos);
    void setActiveSlot();

    void curStepChangedSlot();
    void autoRunChangedSlot();

    void on_actionLoad_triggered();

    void on_actionLog_triggered();

    void on_actionTestRun_triggered();

    void on_actionTestPause_triggered();

    void on_actionTestStop_triggered();

    void on_actionReload_triggered();

    void on_pb_Load_clicked();

    void on_actionAutoEditor_triggered();

    void on_actionCsvSetting_triggered();

    void on_actionCsv_triggered(bool checked);

signals:
    void autoLevelChanged(int);

private:
    void log(QString msg);
    void wait(quint32 msec);
    void msleep(quint32 msec);

    bool cmdCondition(QString cmdName, QStringList args, int condType);

    void transferPhyValueToRawData(A2L_VarChar *charVar, double value, char *data);
    void updateMapValueInSM(A2L_VarChar *map, quint16 index_x, quint16 index_y, char *data);

    bool setSMPamValue(QString smKey, QString pamName, qreal value);
    qreal getSMPamValue(QString smKey, QString pamName);
    QString getSMPamValueStr(QString smKey, QString pamName);

    bool setPamValue(QString modName, QString pamName, qreal value);
    qreal getPamValue(QString modName, QString pamName);
    QString getPamValueStr(QString modName, QString pamName);

    bool setMapPamValue(QString modName, QString pamName, int index_x, int index_y, qreal value);


private:
    Ui::AutoExeWin *ui;

    QString workName = "";

    QStandardItemModel *cmdModel = NULL;
    QHash<QString, QString> cmdFilePathHash;

    QStandardItemModel *jsModel = NULL;
    QHash<QString, QString> jsFilePathHash;

    QStandardItemModel *curFileModel = NULL;
    QString curFilePath;

    QList<AutoINFO> autoStack;

    QStandardItemModel *autoModel = NULL;
    QHash<QString, QString> autoFilePathHash;

    LogWin *Log = NULL;

    SYS_PAM *sysPam = NULL;
    AUTO_PAM *autoPam = NULL;

    CSVObject *csvObj = NULL;

    Auto_Win *autoEditor = NULL;

    QTime runTime;
    QTimer runTimer;

    int selRowInFile = 0;
    int selColInFile = 0;

    QStringList dataTypeEnum = {"UBYTE", "SBYTE", "UWORD", "SWORD",
                               "ULONG", "SLONG", "A_UINT64", "A_INT64",
                               "FLOAT32_IEEE", "FLOAT64_IEEE"};

    QScriptEngine *curScriptEngine = NULL;

    CSV_Win *csvWin = NULL;

    CsvSettingDlg *csvSetDlg = NULL;

};

#endif // AUTOEXEWIN_H
