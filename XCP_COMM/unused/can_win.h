#ifndef CAN_WIN_H
#define CAN_WIN_H

#include <QMainWindow>
#include "nixnet_thread.h"
#include <QTreeWidgetItem>
#include <QDebug>
#include <QFileDialog>
#include <QDrag>
#include <QMimeData>
#include <QMouseEvent>
#include <QTimer>
#include <QStandardItemModel>
#include <QStandardItem>
#include "globals.h"

#include "ui_can_win.h"
#include "xml_gen.h"
#include "delegate_can.h"


class CAN_WIN : public QMainWindow
{
    Q_OBJECT

public:
    explicit CAN_WIN(QWidget *parent = 0);
    ~CAN_WIN();

    bool setSessionInfo();
    QStringList getSMKeys();


    QTimer* timer_r_show = new QTimer(this);
    QTimer* timer_w_pack = new QTimer(this);


    //QString sel_dbcpath;
    QString sel_cluster;
    //QString sel_interface;
    QString sel_frames_r;
    QString sel_frames_w;
    QString sel_sigs_r;
    QString sel_sigs_w;    

    QStringList frameList;

    QByteArray temp_path;
    QByteArray temp_name;

    char* dbcPath;
    char* dbcName;


    //u64 sel_baudrate;
    int sel_read_period;
    int sel_write_period;

    void setInterface(QString intfName);

    void runIntf();
    void stopIntf();


public slots:

    void loadDBC_XNET();

    void loadMsgToTree();
    void loadMsgToModel();

    bool on_action_Connect_triggered();

    void on_action_Disconnect_triggered();

    void on_action_Load_DBC_triggered();

    void updateCANData_R();

    void updateCANRawData_R();

    void updateCANData_W();

    void updateCANRawData_W();

    void updateCANSigValueInSM_W(int index, qreal value);

    void itemValueCheck(QTreeWidgetItem*item, int column);

    void treeMouseDoubleClicked(QTreeWidgetItem*, int column);

    void inputSlot();

    void on_actionLoad_triggered();

    QList<dbc_frame*> getFramesByNames(QStringList frameNameList);

    void uploadWriteSigValue();


    bool on_actionLoadXML_triggered();

    QList<dbc_signal*> getReadSignals();
    QList<dbc_signal*> getWriteSignals();
    QString getSelIntf();
    qreal* getSigValueBufferReadPtr();
    qreal* getSigValueBufferWritePtr();
    bool getReadThreadRunningStatus();

    int getWriteSigIndexByName(QString sigName);

    bool getState();

    void canStateChangedSlot(bool state);

    void winClosedSlot(QString str);

    void writeModelDataUpdatedSlot(QModelIndex index, double value);

private:

    Ui::CAN_WIN *ui;

    nxStatus_t status_dbc;

    nxDatabaseRef_t databaseRef;
    nxDatabaseRef_t *clusterRef;
    u32 numOfClusterRef;
    u32 sizeOfClusterRef;
    char* clusterName;
    u32 sizeOfClusterName;
    nxDatabaseRef_t *frameRef;
    u32 numOfFrameRef;
    u32 sizeOfFrameRef;
    char* frameName;
    u32 sizeOfFrameName;
    u32 frameId;

    QLineEdit *input;
    QTreeWidgetItem *curItem;

    nixnet_Thread *nixnet_ins;

    XML_GEN xml_ins;

    QString sel_dbcpath;
    QString sel_interface;
    u64 sel_baudrate;
    u32 sel_canio_mode;
    u64 sel_canfd_baudrate;
    bool sel_termination_on = false;

    QList<dbc_Cluster*> clusters;
    QHash<QString, dbc_Cluster*> clusterHash;

    bool sel_CustomBD_on = false;
    bool sel_CustomFDBD_on = false;
    qreal sel_SamplePoint_BD = 87.5;    //87.5%
    qreal sel_SamplePoint_FDBD = 87.5;  // 87.5%

    QList<dbc_frame*> dbc_frames;
    QList<dbc_frame*> dbc_frames_r;
    QList<dbc_frame*> dbc_frames_w;

    QList<dbc_signal*> dbc_signals_r;
    QList<dbc_signal*> dbc_signals_w;

    int indexOfEditSig = -1;

    QStandardItemModel *model_r;
    QStandardItemModel *model_w;

    bool runFlag = false;
    bool intfSettingLoad = false;

    bool fdReBuildFlag = true;

    QStandardItemModel readTreeModel;
    QStandardItemModel writeTreeModel;
    QTreeWidgetItem *lastOpen = NULL;


protected:
    bool nativeEvent(const QByteArray &eventType, void *message, long *result);
    void closeEvent(QCloseEvent *event);

private slots:

    void on_actionClearFrame_triggered();
    void on_actionClearFrameR_triggered();
    void on_actionClearFrameW_triggered();

    void on_actionReadSIG_triggered();
    void on_actionWriteSIG_triggered();

    void sigValueWriteUpdate();
    void sigValueReadUpdate();

    void clusterChangedSlot(QString clusterName);
    void showClusterInfo(dbc_Cluster *cluster);

    void on_actionSaveXML_triggered();

    void on_actionFD_Rebuild_triggered();

signals:
    void winClosed(QString title);
    void canStateChanged(QString, bool);
};

#endif // MAINWINDOW_H
