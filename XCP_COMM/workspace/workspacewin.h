#ifndef WORKSPACEWIN_H
#define WORKSPACEWIN_H

#include <QMainWindow>
#include <QStandardItemModel>
#include <QStandardItem>
#include "functionwin.h"
#include "QSettings"
#include "mainwindow.h"
#include "wt5000/win_wt5000.h"
#include "wt3000/win_wt3000.h"
#include "autoexewin.h"


struct WorkStruct {
    QString workName = "Default";
    bool activeStatus = false;

    QString winConfigXmlPath = "";
    FunctionWin *funWin = NULL;

};

namespace Ui {
class WorkSpaceWin;
}

class WorkSpaceWin : public QMainWindow
{
    Q_OBJECT

public:
    explicit WorkSpaceWin(QWidget *parent = nullptr);
    ~WorkSpaceWin();

    void setA2lProjectWin(A2LProjectWin *value);

signals:
    void workNameUpdated(int index);

protected:
    void closeEvent(QCloseEvent *event);

private slots:
    void Slt_ContextMenuRequest(const QPoint& pos);
    void Slt_ModelDataUpdated(int row, int col, QString text);
    void Slt_Clicked(QModelIndex index);
    void Slt_DoubleClicked(QModelIndex index);

    void Slt_WorkNameUpdated(int index);
    void Slt_funUpdated();
    void Slt_workStateUpdated(QString workName, int state);

    void Slt_New(bool flag);
    void Slt_Copy(bool flag);
    void Slt_Open(bool flag);
    void Slt_Del(bool flag);
    void Slt_Hide(bool flag);

    void on_actionSave_triggered();

    void on_actionA2L_triggered();

    void on_actionLog_triggered();

    void on_actionXcpTester_triggered();

    void on_actionWT5000_triggered();

    void on_actionWT3000_triggered();

    void on_actionAutoExe_triggered();

private:
    void initWorkTreeStyle();

    int findWorkIndexByName(QString name);
    void showWorkspaces();

    void saveWorkspaces();
    void readWorkspaces();

    WorkStruct *createWork(QString workName);
    WorkStruct *copyWork(WorkStruct *work);
    void delWork(WorkStruct *work);

    void copyPath(QString src, QString dst);

    void appendWorkRow(QString workName);

    void putFunWidget(FunctionWin *funWin);

private:
    Ui::WorkSpaceWin *ui;

    LogWin *logWin = NULL;

    QStandardItemModel *model_workspace = NULL;
    QStandardItem *rootItem = NULL;
    QList<WorkStruct*> workList;

    QString oldText = "";

    A2LProjectWin *a2lProjectWin = NULL;

    FunctionWin *curFunWin = NULL;

    WIN_WT5000 *win_wt5000 = NULL;
    WIN_WT3000 *win_wt3000 = NULL;

    AutoExeWin *autoExeWin = NULL;

};

#endif // WORKSPACEWIN_H
