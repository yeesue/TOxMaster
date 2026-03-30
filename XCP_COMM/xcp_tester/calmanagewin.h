#ifndef CALMANAGEWIN_H
#define CALMANAGEWIN_H

#include <QMainWindow>
#include "xcpmaster.h"
#include "a2l_project.h"
#include "calicsv.h"
#include "xcp_main_thread.h"

namespace Ui {
class CalManageWin;
}

class CalManageWin : public QMainWindow
{
    Q_OBJECT

public:
    explicit CalManageWin(QWidget *parent = nullptr, XCPMaster *xcp = nullptr, A2L_Project *a2lProject = nullptr);
    ~CalManageWin();

    void setXcpMaster(XCPMaster *value);

    void setCurA2lProject(A2L_Project *value);

    QList<A2L_VarChar *> getDownloadCharList() const;

    void setXcpMainThread(XCP_Main_Thread *value);

private slots:
    void on_actionLoad_triggered();

private slots:
    void SltLoadingProgress(int percentage);

    void on_actionDownload_triggered();

    void on_actionExport_triggered();

    void on_actionImport_triggered();

    void on_actionDownload_Import_triggered();

private:
    void showCalListInTable();
    void showCharVarsInTable();

    QList<A2L_VarChar*> getCharVarListFromCalInfoList(QList<CalInfo> calList);
    bool downloadCalList(QList<CalInfo> calList);

    bool downloadCalChars(QList<A2L_VarChar*> charVarList);

    //use excel as setting-save
    bool delExcel(QString filePath);
    void convertToColName(int data, QString &res);
    QString to26AlphabetString(int data);
    bool exportCalsToExcel(QList<A2L_VarChar*> charVars, QString excelPath);

    void importCalsFromExcel(QString excelPath);

private:
    Ui::CalManageWin *ui;

    XCPMaster *xcpMaster = NULL;
    A2L_Project *curA2lProject = NULL;
    XCP_Main_Thread *xcpMainThread = NULL;

    QList<CalInfo> calList;
    QList<A2L_VarChar*> charVarList;

    QStringList charTypeList = {"VALUE", "VAL_BLK", "CURVE", "MAP"};
};

#endif // CALMANAGEWIN_H
