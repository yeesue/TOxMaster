#ifndef SCRIPTWIN_H
#define SCRIPTWIN_H

#include <QMainWindow>
#include <QScriptEngine>
#include <QDir>
#include <QFile>
#include <QFileDialog>
#include <QMessageBox>
#include "scriptobj.h"
#include "globals.h"
#include "logwin.h"
#include <QTimer>
#include <QProcess>
#include <QDoubleSpinBox>
#include <QComboBox>
#include <QLineEdit>
#include <QTreeView>
#include <QStandardItemModel>
#include <QPushButton>
#include "incacomm.h"


#define SET_FILENAME  "script_setting.ini"
#define SET_UI_FILENAME  "script_ui_setting.ini"

struct scriptStruct {
    QString shortName = "";
    QString filePath = "";
    bool autoRun = false;
    int index = -1;

    int winPos_x = 20;
    int winPos_y = 20;
    int winWidth = 0;
    int winHeight = 0;

};


class ScriptThread : public QThread
{
    Q_OBJECT
public:
    Q_INVOKABLE ScriptThread(QObject *parent = 0);
    ~ScriptThread();

    void abortEvaluating();
    bool isEvaluating();

    bool getScriptRunning() const;

public slots:
    void setScriptFile(QString file, QString filePath);
    void setLog(LogWin *logWin);

protected:
    void run();

signals:
    void stateChanged(QString, QString);

private:
    void packScriptEngineProperty(QScriptEngine *engine);
    void setPamToScriptObject(QScriptEngine *engine, PARAM *pam);

private:    
    QString scriptFile;
    QString scriptFilePath;
    LogWin *Log = NULL;

    QScriptEngine *engine;
    bool scriptRunning = false;

    ScriptTimer *timer = NULL;
    QScriptValue timerObject;

};



namespace Ui {
class ScriptWin;
}


class ScriptWin : public QMainWindow
{
    Q_OBJECT

public:
    explicit ScriptWin(QWidget *parent = 0, QString name = "");
    ~ScriptWin();

    void setWorkName(const QString &value);

public slots:

    void runAll();
    void stopAll();

private slots:

    void showMenu(QPoint pos);
    void showMenu_UI(QPoint pos);

    void deleteSlot();
    void deleteSlot_UI();

    void executeSlot();
    void executeSlotInMainThread();

    void abortSlot();
    void abortSlot_UI();

    void reloadScriptSlot();
    void reloadUIScriptSlot();

    void qScriptExecuteInMainThread(QString scriptFile, QString scriptFilePath);
    void packScriptEngineProperty(QScriptEngine *engine);
    void setPamToScriptObject(QScriptEngine *engine, PARAM *pam);

    void packScriptEnginePropertyforMainThread(QScriptEngine *engine);

    void qScriptExecute(QString scriptFile, QString scriptFilePath);

    void executeAllScript();
    void abortAllScript();

    void executeAllScriptUI();
    void abortAllScriptUI();

    void initINCAObj();

    void on_actionLog_triggered();

    void on_actionLoad_triggered();

    void on_actionRun_triggered();

    void on_actionStop_triggered();

    void on_actionNotePad_triggered();

    void clickedSlot(QModelIndex index);
    void doubleClickedScriptSlot(QModelIndex index);
    void stateChangedSlot(QString scriptFile, QString stateStr);

    void clickedUISlot(QModelIndex index);
    void doubleClickedUIScriptSlot(QModelIndex index);
    void stateChangedUISlot(QString scriptFile, QString stateStr);

    void on_actionRefresh_triggered();

    void on_actionSave_triggered();

    void on_actionINCA_Connect_triggered();

    void on_actionINCA_Disconnect_triggered();

private:
    Ui::ScriptWin *ui;   

    QString workName;

    LogWin *Log = NULL;
    INCACOMM *incaObj = NULL;

    QHash<QString, ScriptThread*> scriptThreadHash; //单独线程中的脚本
    QHash<QString, QString> scriptFileHash;
    QStringList scriptNameList;
    QList<scriptStruct*> scriptList;
    QHash<QString, scriptStruct*> scriptHash;
    QStandardItemModel *scriptModel = NULL;
    QString selScript = "";
    int selScriptRow = 0;
    int selScriptCol = 0;
    QTreeView *treeView = NULL;

    //UI js file reference
    QHash<QString, QScriptEngine*> scriptEngineHash; //主线程中的脚本
    QHash<QString, QString> scriptFileHash_UI;
    QList<scriptStruct*> scriptList_UI;
    QHash<QString, scriptStruct*> scriptHash_UI;
    QStandardItemModel *scriptModel_UI = NULL;
    QString selScript_UI = "";
    int selScriptRow_UI = 0;
    int selScriptCol_UI = 0;
    QTreeView *treeView_UI = NULL;

    QProcess *notePadProcess = NULL;
    QString notePadPath = "";

private:
    void readSetting();
    void writeSetting();
    QStringList getScriptFiles();
    void saveScriptConfiguration();

    void readSettingUI();
    void writeSettingUI();
    QStringList getScriptUIFiles();
    void saveScriptUIConfiguration();


};



#endif // SCRIPTWIN_H
