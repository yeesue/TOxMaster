#ifndef SETTINGWIN_H
#define SETTINGWIN_H

#include <QDialog>
#include <QSettings>
#include <QLineEdit>
#include <QTreeWidgetItem>
#include <QDebug>

#include "a2lprojectwin.h"
#include "pamseldlg.h"

namespace Ui {
class SettingWin;
}

#define SET_FILENAME  "xcp_comm_setting.ini"
#define SET_FILENAME_S  "xcp_setting.ini"
#define SET_FILENAME_S_2ND  "xcp_setting_2nd.ini"

struct XCP_PAM{
    QString pamName;
    QString pollType;
    qreal pamDefaultValue;
    QString caliType;
    QString daqEventTime;
    int     daqEventIndex;
};

struct Proj{
    QString Proj_name = "";

    QString intfName = "CAN1";
    QString a2lProjectName = "";
    QString a2lFilePath = "";
    QString hexFilePath = "";

    int index_XcpOnCan = 0;
    quint64 baudrate = 500000;
    quint32 id_CMD = 0;
    quint32 id_RES = 0;

    QList<A2L_Event> xcpEvents;

    QList<XCP_PAM> readPamList;
    QList<XCP_PAM> writePamList;
    QList<XCP_PAM> daqReadPamList;
};

class SettingWin : public QDialog
{
    Q_OBJECT

public:
    explicit SettingWin(QWidget *parent = 0, A2LProjectWin *a2lProjectWin = 0, int index = 1);
    ~SettingWin();

protected:
    void accept();

private slots:
    void showCustomContextMenuInRead(QPoint pos);
    void showCustomContextMenuInWrite(QPoint pos);

    void showCustomContextMenuInReadDAQ(QPoint pos);

    void addSlotRead(bool);
    void insertSlotRead(bool);
    void removeSlotRead(bool);

    void addSlotWrite(bool);
    void insertSlotWrite(bool);
    void removeSlotWrite(bool);
    void editSlot(bool);

    void addSlotReadDAQ(bool);
    void insertSlotReadDAQ(bool);
    void removeSlotReadDAQ(bool);

    void projComboTextChangedSlot(QString text);
    void measPamSelAcceptedSlot(QStringList measPamNames);
    void charPamSelAcceptedSlot(QStringList charPamNames);

    void measPamSelAcceptedSlotDAQ(QStringList measPamNames);

    void loadAllA2LProject();
    void setCurrentProj(Proj *proj);

    Proj *getProjByName(QString name);

    void projectWinClosedSlot();
    void curProjectChangedSlot(QString projectName);

    void on_pushButton_add_clicked();
    void on_pushButton_saveProj_clicked();

    void on_pb_findDev_clicked();

    void on_pb_projectManage_clicked();

private:
    void readSetting();
    void writeSetting();

private:
    Ui::SettingWin *ui;

    int setIndex = 1;

    QStringList projNameList;
    QList<Proj*> projList;
    Proj *curProj = NULL;

    QString curProjName = "";


    A2LProjectWin *a2lWin = NULL;
    A2L_Project *curA2LProj = NULL;

    XCPMaster *xcpMaster  = NULL;

    QStringList daqEventList;
};

#endif // SETTINGWIN_H
