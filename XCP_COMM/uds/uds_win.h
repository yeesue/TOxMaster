#ifndef UDS_WIN_H
#define UDS_WIN_H

#include <QMainWindow>
#include <QStandardItemModel>
#include <QHash>
#include <QTimer>
#include "ni_uds_worker.h"

namespace Ui {
class UDS_Win;
}

class UDS_Win : public QMainWindow
{
    Q_OBJECT

public:
    explicit UDS_Win(QWidget *parent = nullptr);
    ~UDS_Win();

    void activeUds();

private slots:
    //void on_pb_udsDemo_clicked();

    void Slt_clicked(QModelIndex index);
    void Slt_doubleClicked(QModelIndex index);
    void Slt_modelDataUpdated(QModelIndex index, int value);
    void Slt_customContextMenuRequested(QPoint pos);

    void Slt_clickedTreeFlow(QModelIndex index);
    void Slt_doubleClickedTreeFlow(QModelIndex index);
    void Slt_customContextMenuRequestedTreeFlow(QPoint pos);

    void Slt_customContextMenuRequestedTableFlow(QPoint pos);

    void Slt_doubleClickedInCmdInfo(QModelIndex index);
    void Slt_modelDataUpdatedInCmdInfo(QModelIndex index, QString value);

    void Slt_AddService(bool trigger);
    void Slt_DelService(bool trigger);

    void Slt_AddFlow(bool trigger);
    void Slt_DelFlow(bool trigger);

    void Slt_AddFlowCmd(bool trigger);
    void Slt_AddFlowCmdSeedKey(bool trigger);
    void Slt_AddFlowCmdTester(bool trigger);
    void Slt_DelFlowCmd(bool trigger);

    void on_actionSave_triggered();

    void on_actionActive_triggered();

    void on_pb_udsExecute_clicked();

    void on_pb_selDll_clicked();

private slots:
    void Slt_testerTimeOut();

private:
    void packUdsInfo();
    void initUI();

    void writeSetting();
    void readSetting();

    void showCmdHashInTree();

    int getTreeIndexLevel(QModelIndex index);
    int getFlowTreeIndexLevel(QModelIndex index);
    bool isCmdInsNameExisted(QString name, QList<UDS_CMD_REQ *> cmdReqList);
    bool isFlowNameExisted(QString name, QList<UDS_FLOW*> flowList);

    UDS_CMD_REQ* createCmdInsToHash(quint8 cmd, QString cmdName);
    void removeCmdInsFromHash(quint8 cmd, QString cmdInsName, int index);

    void showCmdInsInfo(quint8 cmd, UDS_CMD_REQ *cmdIns);
    void updateCmdInsInfo(UDS_CMD_REQ *cmdIns);
    void createAttrForCmd(UDS_CMD_REQ *cmdIns, QString attrName, quint8 attrSize, quint8 attrIndex);

    void createTimerForTester();
    void deleteTimerOfTester();

    void reverseBytes(quint8 *data, int len);

    UDS_FLOW* createFlow();
    void showUdsFlowInfo(UDS_FLOW *flow);
    UDS_CMD_ITEM_IN_FLOW* createCmdItemForFlow(UDS_FLOW *udsFlow);
    void createTableItemForCmdItem(UDS_CMD_ITEM_IN_FLOW* cmdItem);

    bool isCmdItemNameExisted(QString name, QList<UDS_CMD_ITEM_IN_FLOW*> itemList);


private:
    Ui::UDS_Win *ui;


    NI_UDS_Worker *udsWorker = NULL;

    UDSInfo udsInfo;

    QStandardItemModel *udsCmdModel = NULL;
    QStandardItemModel *cmdInfoModel = NULL;
    QStandardItemModel *attrModel = NULL;

    QStandardItem *curItem = NULL;
    QModelIndex curIndex;
    int curIndexLevel = 0;

    UDS_CMD_REQ* curCmdIns = NULL;

    QList<quint8> udsCmdList = {0x10, 0x11, 0x14, 0x19,
                                0x22, 0x23, 0x24, 0x27,
                                0x28, 0x2A, 0x2C, 0x2E,
                                0x2F, 0x31, 0x34, 0x35,
                                0x36, 0x37, 0x3D, 0x3E,
                                0x83, 0x84, 0x85, 0x87};

    QHash<quint8, QList<UDS_CMD_REQ*>> cmdHash;

    QTimer *testerTimer = NULL;

    QStandardItemModel *udsFlowModel = NULL;

    QStandardItem *curFlowItem = NULL;
    QModelIndex curFlowIndex;
    int curFlowIndexLevel = 0;

    UDS_FLOW *curFlow = NULL;
    QList<UDS_FLOW*> udsFlowList;

    QStandardItemModel *flowItemModel = NULL;
    QStandardItem *curFlowCmdItem = NULL;
    QModelIndex curFlowCmdIndex;
};

#endif // UDS_WIN_H
