#ifndef XCPSETDLG_H
#define XCPSETDLG_H

#include <QDialog>
#include <QSettings>
#include "a2lprojectwin.h"
#include "pamseldlg.h"
#include "common.h"
#include "calicsv.h"

namespace Ui {
class XcpSetDlg;
}


class XcpSetDlg : public QDialog
{
    Q_OBJECT

public:
    explicit XcpSetDlg(QWidget *parent = nullptr, A2LProjectWin *a2lProjectWin = nullptr,
                       QString work_name = "", QString fun_name = "",
                       QString intf_name = "", QString set_file_path = "");
    ~XcpSetDlg();

    QString getXcpSetFilePath() const;

    QString getXcpSetFileName() const;

protected:
    void accept();

signals:
    void xcpSetUpdated();

private slots:
    void Slt_ProjectWinClosed();
    void Slt_CurProjectChanged(QString projectName);
    void Slt_memSegIndexChanged(int index);
    void Slt_memPageIndexChanged(int index);

    void Slt_ShowMenuInRead(QPoint pos);
    void Slt_ShowMenuInWrite(QPoint pos);
    void Slt_ShowMenuInReadDAQ(QPoint pos);

    void Slt_AddRead(bool);
    void Slt_InsertRead(bool);
    void Slt_RemoveRead(bool);

    void Slt_AddWrite(bool);
    void Slt_InsertWrite(bool);
    void Slt_RemoveWrite(bool);

    void Slt_AddReadDAQ(bool);
    void Slt_InsertReadDAQ(bool);
    void Slt_RemoveReadDAQ(bool);

    void Slt_MeasPamSelAccepted(QStringList measPamNames);
    void Slt_CharPamSelAccepted(QStringList charPamNames);
    void Slt_MeasPamSelAcceptedDAQ(QStringList measPamNames);

    void on_pb_projectManage_clicked();

    void on_pb_loadCalCsv_clicked();

    void on_pb_exportPams_clicked();

    void on_pb_seedKeyDll_released();

    void on_pb_setDaqTimeForSelPam_released();

    void on_pb_upPamDaq_released();

    void on_pb_downPamDaq_released();

    void on_pb_upPamWrite_released();

    void on_pb_downPamWrite_released();

    void on_pb_importIncaPams_released();

    void on_pb_calculateBusload_released();

private:
    void loadA2lProject();
    void packPamInfoToConfig();
    void unpackPamInfoToTable();

    void showXcpSetFromA2lProj(A2L_Project *a2lProject);
    void showXcpSetFromConfig();

    void readSetting();
    void writeSetting();

    void showCalListInTable(QList<CalInfo> calList);
    void loadCalInfoList(QString csvFile);
    QList<A2L_VarChar*> getCharVarListFromCalInfoList(QList<CalInfo> calList);

    /*
    //use excel as setting-save
    bool delExcel(QString filePath);
    void convertToColName(int data, QString &res);
    QString to26AlphabetString(int data);

    bool exportMeasToExcel(QList<A2L_VarMeas*> meaVars, QString excelPath);
    bool exportCharsToExcel(QList<A2L_VarChar*> charVars, QString excelPath);
    */

    //use csv as setting-save
    bool getA2lPamsAndExportToCsv();
    bool delFile(QString filePath);
    bool exportMeasToCsv(QList<A2L_VarMeas*> meaVars, QString csvPath);
    bool exportCharsToCsv(QList<A2L_VarChar*> charVars, QString csvPath);

    QStringList importPamNamesFromIncaPamExcel(QString excelPath);
    QString checkPamIsMeas(QString pamName);

private:
    Ui::XcpSetDlg *ui;

    A2LProjectWin *a2lWin = NULL;
    A2L_Project *curA2LProj = NULL;

    QStringList daqEventNameList;

    QString xcpSetFileName = "";
    QString xcpSetFilePath = "";
    QString workName = "";
    QString funName = "";
    QString intfName = "";

    XcpConfig xcpConfig;

    QStringList pollTypeList = {"Polling_100ms", "Polling_200ms", "Polling_500ms", "Polling_1000ms"};

    QString calCsvFile = "";

    A2L_MemorySegment *curMemorySegment = NULL;
};

#endif // XCPSETDLG_H
