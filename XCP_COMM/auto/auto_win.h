#ifndef AUTO_WIN_H
#define AUTO_WIN_H

#include <QMainWindow>
#include <QScriptEngine>
#include <QDir>
#include <QFile>
#include <QFileDialog>
#include <QStandardItemModel>
#include <QMessageBox>
#include "cmd_dlg.h"
#include "auto_struct.h"
#include "scriptobj.h"
#include "logwin.h"
#include "pamlistdlg.h"
#include "auto_pam.h"

namespace Ui {
class Auto_Win;
}


class Auto_Win : public QMainWindow
{
    Q_OBJECT

public:
    explicit Auto_Win(QWidget *parent = nullptr, QString name = "");
    ~Auto_Win();

    void setLogWin(LogWin *logWin);

    void setWorkName(const QString &value);

private slots:
    void showMenuInFTable(QPoint pos);

    void addRowSlot();
    void removeRowSlot();
    void pamChooseSlot();
    void pamChooseHandle(QList<PARAM*> pams);

    void treePressedSlot(QModelIndex index);

    void fileListDoubleClickedSlot(QModelIndex index);
    void openAutoFilePath(QString openFilePath);

    void jsFileListDoubleClickedSlot(QModelIndex index);

    void cmdInfoSlot(QStringList info);

    void insertFunSlot();
    void insertFunBeforeSlot();
    void deleteFunSlot();

    void on_actionReload_triggered();

    void on_actionAdd_triggered();

    void on_actionDelete_triggered();

    void on_actionNew_triggered();

    void on_actionclearFile_triggered();

    void on_actionSave_triggered();

    void on_actionOpen_triggered();

    void on_actioncmdEdit_triggered();

    void on_actionNotePad_triggered();

private:
    void getAutoFiles();
    void getjsScriptFiles();

private:
    Ui::Auto_Win *ui;

    QString workName = "";

    QStandardItemModel *autoModel = NULL;
    QHash<QString, QString> autoFilePathHash;

    QStandardItemModel *fileModel = NULL;

    QStandardItemModel *jsModel = NULL;
    QHash<QString, QString> jsFilePathHash;

    QProcess *notePadProcess = NULL;
    QString notePadPath = "";

    int selRowInTree = 0;
    QModelIndex selIndexInTree;

    int selRowInFile = 0;
    int selColInFile = 0;

    int insertRow = 0;

    QString curAutoFilePath;

    CMD_DLG *cmdWin = NULL;

};

#endif // AUTO_WIN_H
