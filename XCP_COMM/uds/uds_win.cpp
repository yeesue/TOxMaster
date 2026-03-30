#include "uds_win.h"
#include "ui_uds_win.h"
#include <QSettings>
#include <QDebug>
#include <QMessageBox>
#include <QFileDialog>
#include "hexspinboxdelegate.h"
#include "lineeditdelegate.h"

UDS_Win::UDS_Win(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::UDS_Win)
{
    ui->setupUi(this);
    setWindowTitle("诊断服务");

    udsCmdModel = new QStandardItemModel();
    ui->treeView_cmd->setModel(udsCmdModel);
    ui->treeView_cmd->setContextMenuPolicy(Qt::CustomContextMenu);

    connect(ui->treeView_cmd, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(Slt_customContextMenuRequested(QPoint)));
    connect(ui->treeView_cmd, SIGNAL(clicked(QModelIndex)), this, SLOT(Slt_clicked(QModelIndex)));
    connect(ui->treeView_cmd, SIGNAL(doubleClicked(QModelIndex)), this, SLOT(Slt_doubleClicked(QModelIndex)));

    cmdInfoModel = new QStandardItemModel();
    ui->tableView_cmdInfo->setModel(cmdInfoModel);

    LineEditDelegate *lineEditDelegate = new LineEditDelegate();
    ui->tableView_cmdInfo->setItemDelegate(lineEditDelegate);

    connect(ui->tableView_cmdInfo, SIGNAL(doubleClicked(QModelIndex)), this, SLOT(Slt_doubleClickedInCmdInfo(QModelIndex)));
    connect(lineEditDelegate, SIGNAL(modelDataUpdated(QModelIndex,QString)), this, SLOT(Slt_modelDataUpdatedInCmdInfo(QModelIndex, QString)));

    attrModel = new QStandardItemModel();
    ui->tableView_attr->setModel(attrModel);

    HexSpinBoxDelegate *hexDelegate = new HexSpinBoxDelegate();
    ui->tableView_attr->setItemDelegate(hexDelegate);
    ui->tableView_attr->setEditTriggers(QAbstractItemView::DoubleClicked);

    connect(hexDelegate, SIGNAL(modelDataUpdated(QModelIndex,int)), this, SLOT(Slt_modelDataUpdated(QModelIndex, int)));


    udsFlowModel = new QStandardItemModel();
    ui->treeView_flow->setModel(udsFlowModel);
    ui->treeView_flow->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(ui->treeView_flow, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(Slt_customContextMenuRequestedTreeFlow(QPoint)));
    connect(ui->treeView_flow, SIGNAL(clicked(QModelIndex)), this, SLOT(Slt_clickedTreeFlow(QModelIndex)));
    connect(ui->treeView_flow, SIGNAL(doubleClicked(QModelIndex)), this, SLOT(Slt_doubleClickedTreeFlow(QModelIndex)));

    flowItemModel = new QStandardItemModel();
    ui->tableView_flowInfo->setModel(flowItemModel);
    ui->tableView_flowInfo->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(ui->tableView_flowInfo, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(Slt_customContextMenuRequestedTableFlow(QPoint)));


    initUI();
    readSetting();
    showCmdHashInTree();

    udsWorker = new NI_UDS_Worker();
}

UDS_Win::~UDS_Win()
{
    writeSetting();

    if(udsWorker)
    {
        delete udsWorker;
    }

    delete ui;
}

void UDS_Win::activeUds()
{
    on_actionActive_triggered();
}


void UDS_Win::Slt_clicked(QModelIndex index)
{
    curIndex = index;
    curItem = udsCmdModel->itemFromIndex(index);
    curIndexLevel = getTreeIndexLevel(index);
}

void UDS_Win::Slt_doubleClicked(QModelIndex index)
{
    curIndex = index;
    curItem = udsCmdModel->itemFromIndex(index);
    curIndexLevel = getTreeIndexLevel(index);
    if(curIndexLevel < 3) return;

    QStandardItem *selItem = udsCmdModel->itemFromIndex(index);
    if(!selItem) return;
    QString itemText = selItem->text();
    bool ok = false;
    quint8 cmd = itemText.mid(0, 2).toUInt(&ok, 16);
    int cmdIndex = curIndex.row();
    if(!ok) return;
    qDebug()<<"Select cmd(0x):"<<QString::number(cmd, 16);

    UDS_CMD_REQ *cmdIns = cmdHash.value(cmd).at(cmdIndex);

    curCmdIns = cmdIns;

    showCmdInsInfo(cmd, cmdIns);

}

void UDS_Win::Slt_modelDataUpdated(QModelIndex index, int value)
{
    qDebug()<<"attr index, row:"<<index.row()<<", col:"<<index.column()<<", value:"<<value;

    if(!curCmdIns) return;

    int row = index.row();

    CmdAttr attr = curCmdIns->attrList.at(row);

    if(attr.attrDataSize == 1)
    {
        quint8 pamValue = value;
        memcpy(curCmdIns->data + attr.attrDataIndex, &pamValue, 1);
    }
    else if(attr.attrDataSize == 2)
    {
        quint16 pamValue = value;
        reverseBytes((quint8*)&pamValue, 2);
        memcpy(curCmdIns->data + attr.attrDataIndex, (quint8*)&pamValue, 2);
    }
    else if(attr.attrDataSize == 4)
    {
        quint32 pamValue = value;
        reverseBytes((quint8*)&pamValue, 4);
        memcpy(curCmdIns->data + attr.attrDataIndex, (quint8*)&pamValue, 4);
    }

    updateCmdInsInfo(curCmdIns);

}

void UDS_Win::Slt_customContextMenuRequested(QPoint pos)
{
    QModelIndex index = ui->treeView_cmd->indexAt(pos);
    QStandardItem *item = udsCmdModel->itemFromIndex(index);
    int indexLevel = getTreeIndexLevel(index);

    if(indexLevel <= 1 || indexLevel > 3) return;

    curIndex = index;
    curItem = item;
    curIndexLevel = getTreeIndexLevel(index);

    //qDebug()<<"Index level:"<<indexLevel<<", row:"<<index.row()<<",col:"<<index.column();

    if(!curItem) return;

    QMenu *menu = NULL;
    if (menu)
    {
        delete menu;
        menu = NULL;
    }

    menu = new QMenu(ui->treeView_cmd);

    QAction *addServiceAction = menu->addAction("添加新服务");
    connect(addServiceAction, SIGNAL(triggered(bool)), this, SLOT(Slt_AddService(bool)));

    if(curIndexLevel == 3)
    {
        QAction *delServiceAction = menu->addAction("删除服务");
        connect(delServiceAction, SIGNAL(triggered(bool)), this, SLOT(Slt_DelService(bool)));
    }

    menu->move(cursor().pos());
    menu->show();

}

void UDS_Win::Slt_clickedTreeFlow(QModelIndex index)
{
    curFlowIndex = index;
    curFlowItem = udsFlowModel->itemFromIndex(index);
    curFlowIndexLevel = getFlowTreeIndexLevel(index);
}

void UDS_Win::Slt_doubleClickedTreeFlow(QModelIndex index)
{
    curFlowIndex = index;
    curFlowItem = udsFlowModel->itemFromIndex(index);
    curFlowIndexLevel = getFlowTreeIndexLevel(index);

    if(curFlowIndexLevel != 2) return;

    int flowIndex = curFlowIndex.row();

    UDS_FLOW *udsFlow = udsFlowList.at(flowIndex);
    curFlow = udsFlow;

    showUdsFlowInfo(udsFlow);

}

void UDS_Win::Slt_customContextMenuRequestedTreeFlow(QPoint pos)
{
    QModelIndex index = ui->treeView_flow->indexAt(pos);
    QStandardItem *item = udsFlowModel->itemFromIndex(index);
    int indexLevel = getFlowTreeIndexLevel(index);

    curFlowIndex = index;
    curFlowItem = item;
    curFlowIndexLevel = getFlowTreeIndexLevel(index);

    if(!curFlowItem) return;

    QMenu *menu = NULL;
    if (menu)
    {
        delete menu;
        menu = NULL;
    }

    menu = new QMenu(ui->treeView_flow);

    QAction *addFlowAction = menu->addAction("添加UDS流程");
    connect(addFlowAction, SIGNAL(triggered(bool)), this, SLOT(Slt_AddFlow(bool)));

    if(curFlowIndexLevel == 2)
    {
        QAction *delFlowAction = menu->addAction("删除UDS流程");
        connect(delFlowAction, SIGNAL(triggered(bool)), this, SLOT(Slt_DelFlow(bool)));
    }

    menu->move(cursor().pos());
    menu->show();
}

void UDS_Win::Slt_customContextMenuRequestedTableFlow(QPoint pos)
{
    QModelIndex index = ui->tableView_flowInfo->indexAt(pos);
    QStandardItem *item = flowItemModel->itemFromIndex(index);

    curFlowCmdIndex = index;
    curFlowCmdItem = item;

    QMenu *menu = NULL;
    if (menu)
    {
        delete menu;
        menu = NULL;
    }

    menu = new QMenu(ui->tableView_flowInfo);

    QAction *addFlowCmdAction = menu->addAction("添加步骤");
    connect(addFlowCmdAction, SIGNAL(triggered(bool)), this, SLOT(Slt_AddFlowCmd(bool)));

    QAction *addFlowCmdSeedKeyAction = menu->addAction("添加种子和密匙");
    connect(addFlowCmdSeedKeyAction, SIGNAL(triggered(bool)), this, SLOT(Slt_AddFlowCmdSeedKey(bool)));

    QAction *addFlowCmdTesterAction = menu->addAction("添加测试设备在线");
    connect(addFlowCmdTesterAction, SIGNAL(triggered(bool)), this, SLOT(Slt_AddFlowCmdTester(bool)));

    menu->addSeparator();

    QAction *delFlowCmdAction = menu->addAction("删除步骤");
    connect(delFlowCmdAction, SIGNAL(triggered(bool)), this, SLOT(Slt_DelFlowCmd(bool)));

    menu->addSeparator();

    menu->move(cursor().pos());
    menu->show();
}

void UDS_Win::Slt_doubleClickedInCmdInfo(QModelIndex index)
{
    if(index.row() != 0) return;
}

void UDS_Win::Slt_modelDataUpdatedInCmdInfo(QModelIndex index, QString value)
{
    if(!curCmdIns) return;
    if(index.row() != 0) return;

    //qDebug()<<"update name:"<<value;
    curCmdIns->name = value;

    udsCmdModel->itemFromIndex(curIndex)->setText(curCmdIns->preStr + " " + curCmdIns->name);
}

void UDS_Win::Slt_AddService(bool trigger)
{
    if(!curItem || curIndexLevel != 2) return;

    QString itemText = curItem->text();
    bool ok = false;
    quint8 cmd = itemText.mid(1, 2).toUInt(&ok, 16);

    //qDebug()<<"Select cmd from rightClick:"<<cmd;

    UDS_CMD_REQ *cmdIns = createCmdInsToHash(cmd, itemText);

    QString itemName = cmdIns->preStr + " " + cmdIns->name;
    QStandardItem *serviceItem = new QStandardItem(QIcon(":/icons/case.png"), itemName);

    curItem->appendRow(serviceItem);
}

void UDS_Win::Slt_DelService(bool trigger)
{
    if(curIndexLevel < 3) return;

    QString itemText = curItem->text();
    bool ok = false;
    quint8 cmd = itemText.mid(0, 2).toUInt(&ok, 16);
    int insIndex = curIndex.row();

    qDebug()<<"cmd:"<<cmd<<",level:"<<curIndexLevel<<", itemText:"<<itemText<<", index:"<<insIndex;
    removeCmdInsFromHash(cmd, itemText, insIndex);

    udsCmdModel->removeRow(curIndex.row(), curIndex.parent());
}

void UDS_Win::Slt_AddFlow(bool trigger)
{
    if(!curFlowItem || curIndexLevel > 2) return;

    UDS_FLOW *udsFlow = createFlow();

    QString itemName = udsFlow->flowName;
    QStandardItem *flowItem = new QStandardItem(QIcon(":/icons/flower.png"), itemName);

    if(curFlowItem->parent())
        curFlowItem->parent()->appendRow(flowItem);
    else
        curFlowItem->appendRow(flowItem);

    ui->treeView_flow->expandAll();

}

void UDS_Win::Slt_DelFlow(bool trigger)
{
    if(curFlowIndexLevel < 2) return;
    int flowIndex = curFlowIndex.row();

    UDS_FLOW *flow = udsFlowList.takeAt(flowIndex);

    udsFlowModel->removeRow(curFlowIndex.row(), curFlowIndex.parent());

    delete flow;
    flow = NULL;
}

void UDS_Win::Slt_AddFlowCmd(bool trigger)
{
    if(!curFlow) return;

    UDS_CMD_ITEM_IN_FLOW* cmdItem =  createCmdItemForFlow(curFlow);

}

void UDS_Win::Slt_AddFlowCmdSeedKey(bool trigger)
{

}

void UDS_Win::Slt_AddFlowCmdTester(bool trigger)
{

}

void UDS_Win::Slt_DelFlowCmd(bool trigger)
{

}

/*
void UDS_Win::on_pb_udsDemo_clicked()
{

    packUdsInfo();

    udsWorker->setUdsInfo(this->udsInfo);

    udsWorker->uds_demo();
}
*/

void UDS_Win::Slt_testerTimeOut()
{
    if(!udsWorker) return;

    udsWorker->uds_manl_testPresented(udsInfo.testerResponseRequired);
}

void UDS_Win::packUdsInfo()
{
    udsInfo.canIntf = ui->cb_Intf->currentText() + "@nixnet";
    udsInfo.baudrate = ui->sb_baudrate->value();
    udsInfo.baudrate_canfd = ui->sb_baudrate_canfd->value();
    udsInfo.canIoMode = ui->cb_IOMode->currentIndex();
    bool ok = false;
    udsInfo.transmitID = ui->le_transmitID->text().toUInt(&ok, 16);
    udsInfo.receiveID = ui->le_receiveID->text().toUInt(&ok, 16);

    udsInfo.maxDLC = ui->sb_maxDLC->value();

    udsInfo.securityConstant = ui->le_securityConstant->text().toUInt(&ok, 16);

    udsInfo.testerActive = ui->cb_Tester->isChecked();
    udsInfo.testerCycleTime = ui->sb_TesterCycleTime->value();
    udsInfo.testerResponseRequired = ui->cb_ResponseRequired->isChecked();

    udsInfo.seedKeyType = ui->cb_SeedKey->currentIndex();
}

void UDS_Win::initUI()
{
    //tab1

    ui->le_customData->setInputMask(QString("%1;").arg("", 64, QChar('h')));
    ui->le_transmitID->setInputMask(QString("%1;").arg("", 3, QChar('h')));
    ui->le_receiveID->setInputMask(QString("%1;").arg("", 3, QChar('h')));
    ui->le_securityConstant->setInputMask(QString("%1;").arg("", 8, QChar('h')));

    //tab2
    if(!udsCmdModel)
    {
        udsCmdModel = new QStandardItemModel();
    }

    ui->treeView_cmd->header()->setVisible(false);

    QStandardItem *rootItem = new QStandardItem(QIcon(":/icons/rongqi.png"), tr("基础诊断配置"));

    udsCmdModel->appendRow(rootItem);

    QStringList cmdNameList;
    cmdNameList<<tr("$10 诊断会话控制")<<tr("$11 ECU重启")<<tr("$14 清除诊断信息")<<tr("$19 读取DTC信息")
               <<tr("$22 根据标识符读取数据")<<tr("$23 根据地址读取内存")<<tr("$24 根据标识符读取缩放数据")<<tr("$27 安全访问")
               <<tr("$28 通讯控制")<<tr("$2A 按周期标识符读取数据")<<tr("$2C 动态定义数据标识符")<<tr("$2E 根据标识符写入数据")
               <<tr("$2F 按标识符输入输出控制")<<tr("$31 例程控制")<<tr("$34 请求下载")<<tr("$35 请求上传")
               <<tr("$36 传输数据")<<tr("$37 请求退出传输")<<tr("$3D 根据地址写入内存")<<tr("$3E 诊断仪在线")
               <<tr("$83 访问定时参数")<<tr("$84 安全数据传输")<<tr("$85 控制DTC设置")<<tr("$87 链接控制")
               <<tr("$343637 下载文件");

    QList<QStandardItem*> cmdItemList;

    for(int i = 0; i < cmdNameList.count(); i++)
    {
        QStandardItem *item = new QStandardItem(QIcon(":/icons/cangku.png"), cmdNameList.at(i));

        cmdItemList.append(item);
    }

    rootItem->appendRows(cmdItemList);

    ui->treeView_cmd->expandAll();

    //table cmdInfo
    QStringList headers;
    headers << "名称"<<"值";
    cmdInfoModel->setHorizontalHeaderLabels(headers);

    cmdInfoModel->setColumnCount(2);
    cmdInfoModel->setRowCount(3);
    ui->tableView_cmdInfo->setColumnWidth(0, 200);
    ui->tableView_cmdInfo->setColumnWidth(1, 200);

    QStandardItem *nameItem = new QStandardItem("服务名称");
    QStandardItem *requestNameItem = new QStandardItem(" 请求协议数据(只读)");
    QStandardItem *responseNameItem = new QStandardItem(" 应答协议数据(只读)");

    cmdInfoModel->setItem(0, 0, nameItem);
    cmdInfoModel->setItem(1, 0, requestNameItem);
    cmdInfoModel->setItem(2, 0, responseNameItem);

    cmdInfoModel->setItem(0, 1, new QStandardItem());
    cmdInfoModel->setItem(1, 1, new QStandardItem());
    cmdInfoModel->setItem(2, 1, new QStandardItem());

    //tab3
    if(!udsFlowModel)
    {
        udsFlowModel = new QStandardItemModel();
    }

    ui->treeView_flow->header()->setVisible(false);

    QStandardItem *flowRootItem = new QStandardItem(QIcon(":/icons/rongqi.png"), tr("诊断流程"));
    udsFlowModel->appendRow(flowRootItem);

    QStringList headers_flowItem;
    headers_flowItem << "使能" <<"类型" << "名称" << "请求服务(0x)" << "应答服务(0x)" << "延迟(ms)" << "重试";
    flowItemModel->setHorizontalHeaderLabels(headers_flowItem);

    flowItemModel->setColumnCount(7);

}

void UDS_Win::writeSetting()
{
    QString appPath = QCoreApplication::applicationDirPath();
    QSettings setting(appPath + "/setting/uds_setting.ini", QSettings::IniFormat);

    setting.clear();

    setting.beginGroup("UDSSetting");
    setting.setValue("CanIntf", ui->cb_Intf->currentText());
    setting.setValue("CanIoMode", ui->cb_IOMode->currentText());
    setting.setValue("Baudrate", ui->sb_baudrate->value());
    setting.setValue("Baudrate_CANFD", ui->sb_baudrate_canfd->value());
    setting.setValue("TransmitID", ui->le_transmitID->text());
    setting.setValue("ReceiveID", ui->le_receiveID->text());
    setting.setValue("maxDLC", ui->sb_maxDLC->value());
    setting.setValue("SecurityConstant", ui->le_securityConstant->text());

    setting.setValue("TesterActive", ui->cb_Tester->isChecked());
    setting.setValue("TesterCycleTime", ui->sb_TesterCycleTime->value());
    setting.setValue("TesterResponseRequired", ui->cb_ResponseRequired->isChecked());

    setting.setValue("DllActive", ui->cb_UseDllForSeedKey->isChecked());
    setting.setValue("DllPath", ui->le_dllPath->text());
    setting.setValue("SeedKeyMethod", ui->cb_SeedKey->currentText());

    setting.endGroup();

    setting.beginWriteArray("Cmds");
    QList<quint8> keyList = cmdHash.uniqueKeys();
    qSort(keyList.begin(), keyList.end());
    int index = 0;
    for(int i = 0; i < keyList.count(); i++)
    {
        quint8 key = keyList.at(i);

        QList<UDS_CMD_REQ*> cmdInsList = cmdHash.value(key);

        for(int j = 0; j < cmdInsList.count(); j++)
        {
            setting.setArrayIndex(index);

            UDS_CMD_REQ *cmdIns = cmdInsList.at(j);

            setting.setValue("CmdName", cmdIns->name);
            setting.setValue("CmdValue", cmdIns->cmd);
            setting.setValue("CmdDataLen", cmdIns->dataLen);
            setting.setValue("CmdPre", cmdIns->preStr);
            setting.setValue("CmdData8Bytes", *(quint64*)(cmdIns->data));

            index++;
        }

    }

    setting.endArray();

    setting.sync();

    qDebug()<<"uds setting write ok!";

}

void UDS_Win::readSetting()
{
    QString appPath = QCoreApplication::applicationDirPath();
    QSettings setting(appPath + "/setting/uds_setting.ini", QSettings::IniFormat);

    setting.beginGroup("UDSSetting");
    ui->cb_Intf->setCurrentText(setting.value("CanIntf").toString());
    ui->cb_IOMode->setCurrentText(setting.value("CanIoMode").toString());

    ui->sb_baudrate->setValue(setting.value("Baudrate").toUInt());
    ui->sb_baudrate_canfd->setValue(setting.value("Baudrate_CANFD").toUInt());

    ui->le_transmitID->setText(setting.value("TransmitID").toString());
    ui->le_receiveID->setText(setting.value("ReceiveID").toString());

    ui->sb_maxDLC->setValue(setting.value("maxDLC").toUInt());
    ui->le_securityConstant->setText(setting.value("SecurityConstant").toString());

    ui->cb_Tester->setChecked(setting.value("TesterActive").toBool());
    ui->sb_TesterCycleTime->setValue(setting.value("TesterCycleTime").toUInt());
    ui->cb_ResponseRequired->setChecked(setting.value("TesterResponseRequired").toBool());

    ui->cb_UseDllForSeedKey->setChecked(setting.value("DllActive").toBool());
    ui->le_dllPath->setText(setting.value("DllPath").toString());
    ui->cb_SeedKey->setCurrentText(setting.value("SeedKeyMethod").toString());

    setting.endGroup();

    int size = setting.beginReadArray("Cmds");
    qDebug()<<"reading cmd, size:"<<size;

    for(int i = 0; i < size; i++)
    {
        setting.setArrayIndex(i);

        quint8 cmd = (quint8)setting.value("CmdValue").toUInt();;

        QList<UDS_CMD_REQ*> cmdInsList = cmdHash.value(cmd);

        UDS_CMD_REQ *cmdIns = new UDS_CMD_REQ;
        cmdIns->cmd = (quint8)setting.value("CmdValue").toUInt();
        cmdIns->name = setting.value("CmdName").toString();
        cmdIns->preStr = setting.value("CmdPre").toString();
        cmdIns->dataLen = (quint8)setting.value("CmdDataLen").toUInt();
        *(quint64*)(cmdIns->data) = (quint64)(setting.value("CmdData8Bytes").toUInt());

        switch (cmd) {
        case 0x10:
            createAttrForCmd(cmdIns, tr("诊断会话类型"), 1, 0);

            break;
        case 0x11:
            createAttrForCmd(cmdIns, tr("重启类型"), 1, 0);

            break;
        case 0x22:
            createAttrForCmd(cmdIns, tr("数据标识符"), 2, 0);

            break;
        case 0x27:
            createAttrForCmd(cmdIns, tr("安全访问类型"), 1, 0);

            break;
        case 0x2E:                  // write data by id
            createAttrForCmd(cmdIns, tr("数据标识符"), 2, 0);

            break;
        case 0x2F:                    //IO ctrl by id;
            createAttrForCmd(cmdIns, tr("数据标识符"), 2, 0);
            createAttrForCmd(cmdIns, tr("输入输出控制参数"), 1, 2);

        case 0x31:                    // routine ctrl
            createAttrForCmd(cmdIns, tr("例程控制类型"), 1, 0);
            createAttrForCmd(cmdIns, tr("例程标识符"), 2, 1);
            break;
        case 0x3E:
            createAttrForCmd(cmdIns, tr("零子函数"), 1, 0);
            break;
        default:
            break;
        }

        cmdInsList.append(cmdIns);

        cmdHash.insert(cmd, cmdInsList);

        qDebug()<<"from setting, cmd:"<<cmd<<", name:"<<cmdIns->name<<", cmdIns count:"<<cmdInsList.count();
    }

    setting.endArray();

    ui->statusbar->showMessage("uds setting read done!");
    qDebug()<<"uds setting read ok!";
}

void UDS_Win::showCmdHashInTree()
{
    QList<quint8> cmds = cmdHash.uniqueKeys();
    qSort(cmds.begin(), cmds.end());
    for(int i = 0; i < cmds.count(); i++)
    {
        quint8 cmd = cmds.at(i);

        int cmdIndex = udsCmdList.indexOf(cmd);
        qDebug()<<"show cmd:"<<cmd<<", index:"<<cmdIndex;

        if(!udsCmdModel) return;

        QStandardItem *rootItem = udsCmdModel->item(0, 0);
        if(!rootItem) return;

        QStandardItem *cmdItem = rootItem->child(cmdIndex, 0);
        if(!cmdItem) continue;

        qDebug()<<"item:"<<cmdItem->text();

        QList<UDS_CMD_REQ*> cmdInsList = cmdHash.value(cmd);
        for(int j = 0; j < cmdInsList.count(); j++)
        {
            UDS_CMD_REQ *cmdIns = cmdInsList.at(j);
            QStandardItem *item = new QStandardItem(QIcon(":/icons/case.png"), cmdIns->preStr + " " + cmdIns->name);

            cmdItem->appendRow(item);
        }
    }
}

int UDS_Win::getTreeIndexLevel(QModelIndex index)
{
    if(!(index.parent().isValid()))
    {
        return 1;
    }
    else
    {
        return getTreeIndexLevel(index.parent()) + 1;
    }

}

int UDS_Win::getFlowTreeIndexLevel(QModelIndex index)
{
    if(!(index.parent().isValid()))
    {
        return 1;
    }
    else
    {
        return getFlowTreeIndexLevel(index.parent()) + 1;
    }
}

bool UDS_Win::isCmdInsNameExisted(QString name, QList<UDS_CMD_REQ*> cmdReqList)
{
    foreach (UDS_CMD_REQ *cmdIns, cmdReqList) {
        if(name == cmdIns->name)
            return true;
    }

    return false;
}

bool UDS_Win::isFlowNameExisted(QString name, QList<UDS_FLOW *> flowList)
{
    foreach (UDS_FLOW *flow, flowList) {
        if(name == flow->flowName)
            return true;
    }

    return false;
}


UDS_CMD_REQ *UDS_Win::createCmdInsToHash(quint8 cmd, QString cmdName)
{
    UDS_CMD_REQ *cmdIns = new UDS_CMD_REQ;
    cmdIns->cmd = cmd;
    cmdIns->name = cmdName.mid(4, -1);

    switch (cmd) {
    case 0x10:
        cmdIns->data[0] = 0x00;
        cmdIns->dataLen = 1;

        createAttrForCmd(cmdIns, tr("诊断会话类型"), 1, 0);

        break;
    case 0x11:
        cmdIns->data[0] = 0x00;
        cmdIns->dataLen = 1;

        createAttrForCmd(cmdIns, tr("重启类型"), 1, 0);

        break;
    case 0x22:
        cmdIns->data[0] = 0x00;
        cmdIns->data[1] = 0x00;
        cmdIns->dataLen = 2;

        createAttrForCmd(cmdIns, tr("数据标识符"), 2, 0);

        break;
    case 0x27:
        cmdIns->data[0] = 0x00;

        cmdIns->dataLen = 1;    // mode + key

        createAttrForCmd(cmdIns, tr("安全访问类型"), 1, 0);

        break;
    case 0x2E:                  // write data by id
        cmdIns->data[0] = 0x00; // did-hi
        cmdIns->data[1] = 0x00; // did-low

        cmdIns->dataLen = 2;

        createAttrForCmd(cmdIns, tr("数据标识符"), 2, 0);

        break;
    case 0x2F:                    //IO ctrl by id;
        cmdIns->data[0] = 0x00;
        cmdIns->data[1] = 0x00;
        cmdIns->data[2] = 0x00;   //mode

        cmdIns->dataLen = 3;

        createAttrForCmd(cmdIns, tr("数据标识符"), 2, 0);
        createAttrForCmd(cmdIns, tr("输入输出控制参数"), 1, 2);

    case 0x31:                    // routine ctrl
        cmdIns->data[0] = 0x00;   // ctrl type
        cmdIns->data[1] = 0x00;
        cmdIns->data[2] = 0x00;

        cmdIns->dataLen = 3;

        createAttrForCmd(cmdIns, tr("例程控制类型"), 1, 0);
        createAttrForCmd(cmdIns, tr("例程标识符"), 2, 1);
        break;
    case 0x3E:
        cmdIns->data[0] = 0x00;

        cmdIns->dataLen = 1;

        createAttrForCmd(cmdIns, tr("零子函数"), 1, 0);
        break;
    default:
        break;
    }

    QString preStr = QString::number(cmdIns->cmd, 16).toUpper();
    for(int i = 0; i < cmdIns->dataLen; i++)
    {
        preStr += (" " + QString("%1").arg((cmdIns->data[i]), 2, 16, QLatin1Char('0')).toUpper());
    }
    //cmdIns->name = nameStr + " " + cmdIns->name;
    cmdIns->preStr = preStr;

    QList<UDS_CMD_REQ*> udsCmdReqList = cmdHash.value(cmd);
    int nameIndex = 1;
    QString tempName = cmdIns->name + QString::number(nameIndex);
    while(nameIndex <= 100 && isCmdInsNameExisted(tempName, udsCmdReqList))
    {
        tempName = cmdIns->name + QString::number(nameIndex);
        nameIndex ++;
    }
    cmdIns->name = tempName;

    udsCmdReqList.append(cmdIns);
    cmdHash.insert(cmd, udsCmdReqList);

    qDebug()<<"add cmd instance:"<<cmdIns->name;

    return cmdIns;
}

void UDS_Win::removeCmdInsFromHash(quint8 cmd, QString cmdInsName, int index)
{
    QList<UDS_CMD_REQ*> udsCmdReqList = cmdHash.value(cmd);

    UDS_CMD_REQ *cmdIns = udsCmdReqList.at(index);
    QString itemText = cmdIns->preStr + " " + cmdIns->name;
    if(itemText == cmdInsName)
    {
        UDS_CMD_REQ* cmdIns = udsCmdReqList.takeAt(index);

        cmdHash.insert(cmd, udsCmdReqList);

        qDebug()<<"remove cmd instance:"<<cmdIns->name;

        delete cmdIns;
        cmdIns = NULL;
    }

}

void UDS_Win::showCmdInsInfo(quint8 cmd, UDS_CMD_REQ *cmdIns)
{
    cmdInfoModel->item(0, 1)->setText(cmdIns->name);
    //QStandardItem *cmdItem = cmdInfoModel->item(0, 1);

    QString reqDataStr = QString::number(cmdIns->cmd, 16).toUpper();
    for(int i = 0; i < cmdIns->dataLen; i++)
    {
        reqDataStr += (" " + QString("%1").arg((cmdIns->data[i]), 2, 16, QLatin1Char('0')).toUpper());
    }
    cmdInfoModel->item(1, 1)->setText(reqDataStr);

    QString resDataStr = QString::number(cmdIns->cmd + 0x40, 16).toUpper();
    for(int i = 0; i < cmdIns->dataLen; i++)
    {
        resDataStr += (" " + QString("%1").arg((cmdIns->data[i]), 2, 16, QLatin1Char('0')).toUpper());
    }
    cmdInfoModel->item(2, 1)->setText(resDataStr);

    if(!attrModel)
    {
        attrModel = new QStandardItemModel();
    }

    attrModel->clear();

    QStringList headers;
    headers<<tr("名称")<<tr("值");
    attrModel->setHorizontalHeaderLabels(headers);

    QList<CmdAttr> attrList = cmdIns->attrList;

    attrModel->setColumnCount(2);
    attrModel->setRowCount(attrList.count());
    ui->tableView_attr->setColumnWidth(0, 200);
    ui->tableView_attr->setColumnWidth(1, 200);

    for(int i = 0; i < attrList.count(); i++)
    {
        CmdAttr attr = attrList.at(i);

        QStandardItem *attrNameItem = new QStandardItem(attr.attrName);

        QString attrDataStr = "0x";
        for(int i = attr.attrDataIndex; i < (attr.attrDataIndex + attr.attrDataSize); i++)
        {
            attrDataStr += QString("%1").arg((cmdIns->data[i]), 2, 16, QLatin1Char('0')).toUpper();
        }
        QStandardItem *attrValueItem = new QStandardItem(attrDataStr);

        attrModel->setItem(i, 0, attrNameItem);
        attrModel->setItem(i, 1, attrValueItem);
    }
}

void UDS_Win::updateCmdInsInfo(UDS_CMD_REQ *cmdIns)
{
    if(!cmdInfoModel || !cmdIns) return;

    QString preStr = QString::number(cmdIns->cmd, 16).toUpper();
    for(int i = 0; i < cmdIns->dataLen; i++)
    {
        preStr += (" " + QString("%1").arg((cmdIns->data[i]), 2, 16, QLatin1Char('0')).toUpper());
    }
    cmdIns->preStr = preStr;

    QString reqDataStr = QString::number(cmdIns->cmd, 16);
    for(int i = 0; i < cmdIns->dataLen; i++)
    {
        reqDataStr += (" " + QString("%1").arg((cmdIns->data[i]), 2, 16, QLatin1Char('0')).toUpper());
    }
    cmdInfoModel->item(1, 1)->setText(reqDataStr);

    QString resDataStr = QString::number(cmdIns->cmd + 0x40, 16);
    for(int i = 0; i < cmdIns->dataLen; i++)
    {
        resDataStr += (" " + QString("%1").arg((cmdIns->data[i]), 2, 16, QLatin1Char('0')).toUpper());
    }
    cmdInfoModel->item(2, 1)->setText(resDataStr);

    udsCmdModel->itemFromIndex(curIndex)->setText(cmdIns->preStr + " " + cmdIns->name);
}

void UDS_Win::createAttrForCmd(UDS_CMD_REQ *cmdIns, QString attrName, quint8 attrSize, quint8 attrIndex)
{
    CmdAttr attr;
    attr.attrName = attrName;
    attr.attrDataSize = attrSize;
    attr.attrDataIndex = attrIndex;

    cmdIns->attrList.append(attr);
}

void UDS_Win::createTimerForTester()
{
    if(!testerTimer)
    {
        testerTimer = new QTimer();
    }

    testerTimer->setInterval(udsInfo.testerCycleTime);
    connect(testerTimer, SIGNAL(timeout()), this, SLOT(Slt_testerTimeOut()));

    testerTimer->start();
}

void UDS_Win::deleteTimerOfTester()
{
    if(!testerTimer) return;

    testerTimer->stop();
    delete testerTimer;
    testerTimer = NULL;
}

void UDS_Win::reverseBytes(quint8 *data, int len)
{
    if(!data) return;
    for(int i = 0; i < len/2; i++)
    {
        quint8 temp = data[i];
        data[i] = data[len-1-i];
        data[len-1-i] = temp;
    }
}

UDS_FLOW *UDS_Win::createFlow()
{
    UDS_FLOW *udsFlow = new UDS_FLOW;

    int nameIndex = 1;
    QString tempName = udsFlow->flowName + QString::number(nameIndex);
    while(nameIndex <= 100 && isFlowNameExisted(tempName, udsFlowList))
    {
        tempName = udsFlow->flowName + QString::number(nameIndex);
        nameIndex ++;
    }
    udsFlow->flowName = tempName;

    udsFlowList.append(udsFlow);

    return udsFlow;
}

void UDS_Win::showUdsFlowInfo(UDS_FLOW *flow)
{
    flowItemModel->clear();

    QStringList headers;
    headers<< "使能" <<"类型" << "名称" << "请求服务(0x)" << "应答服务(0x)" << "延迟(ms)" << "重试";
    flowItemModel->setHorizontalHeaderLabels(headers);

    QList<UDS_CMD_ITEM_IN_FLOW*> cmdItemList = flow->cmdItemList;

    flowItemModel->setColumnCount(7);
    flowItemModel->setRowCount(cmdItemList.count());
    ui->tableView_flowInfo->setColumnWidth(0, 50);
    ui->tableView_flowInfo->setColumnWidth(1, 150);
    ui->tableView_flowInfo->setColumnWidth(2, 150);
    ui->tableView_flowInfo->setColumnWidth(3, 200);
    ui->tableView_flowInfo->setColumnWidth(4, 200);
    ui->tableView_flowInfo->setColumnWidth(5, 100);
    ui->tableView_flowInfo->setColumnWidth(6, 100);

    for(int i = 0; i < cmdItemList.count(); i++)
    {
        UDS_CMD_ITEM_IN_FLOW *cmdItem = cmdItemList.at(i);

        QStandardItem *enableItem = new QStandardItem();
        QStandardItem *typeItem = new QStandardItem();

        QStandardItem *nameItem = new QStandardItem(cmdItem->name);

        UDS_CMD_REQ *cmdIns = cmdItem->cmdIns;

        QString reqDataStr = QString("%1").arg((cmdIns->cmd), 2, 16, QLatin1Char('0')).toUpper();
        for(int i = cmdIns->dataLen; i < cmdIns->dataLen; i++)
        {
            reqDataStr += (" " + QString("%1").arg((cmdIns->data[i]), 2, 16, QLatin1Char('0')).toUpper());
        }

        QStandardItem *reqDataItem = new QStandardItem(reqDataStr);

        QString resDataStr = QString("%1").arg((cmdIns->cmd + 0x40), 2, 16, QLatin1Char('0')).toUpper();
        for(int i = cmdIns->dataLen; i < cmdIns->dataLen; i++)
        {
            resDataStr += (" " + QString("%1").arg((cmdIns->data[i]), 2, 16, QLatin1Char('0')).toUpper());
        }

        QStandardItem *resDataItem = new QStandardItem(resDataStr);

        QStandardItem *delayTimeItem = new QStandardItem(QString::number(cmdItem->postTime));
        QStandardItem *retryItem = new QStandardItem(QString::number(cmdItem->retryTime));

        flowItemModel->setItem(i, 0, enableItem);
        flowItemModel->setItem(i, 1, typeItem);
        flowItemModel->setItem(i, 2, nameItem);
        flowItemModel->setItem(i, 3, reqDataItem);
        flowItemModel->setItem(i, 4, resDataItem);
        flowItemModel->setItem(i, 5, delayTimeItem);
        flowItemModel->setItem(i, 6, retryItem);
    }
}

UDS_CMD_ITEM_IN_FLOW *UDS_Win::createCmdItemForFlow(UDS_FLOW *udsFlow)
{
    if(!udsFlow) return NULL;

    QList<UDS_CMD_ITEM_IN_FLOW*> cmdItemList = udsFlow->cmdItemList;

    UDS_CMD_ITEM_IN_FLOW *cmdItem = new UDS_CMD_ITEM_IN_FLOW;

    int nameIndex = 1;
    QString tempName = cmdItem->name + QString::number(nameIndex);
    while(nameIndex <= 100 && isCmdItemNameExisted(tempName, cmdItemList))
    {
        tempName = cmdItem->name + QString::number(nameIndex);
        nameIndex ++;
    }
    cmdItem->name = tempName;

    createTableItemForCmdItem(cmdItem);

    cmdItemList.append(cmdItem);
}

void UDS_Win::createTableItemForCmdItem(UDS_CMD_ITEM_IN_FLOW *cmdItem)
{
    if(!cmdItem || !flowItemModel) return;

    QCheckBox *checkBox = new QCheckBox(this);
    checkBox->setChecked(cmdItem->enable);
    QWidget *w = new QWidget(this);
    QHBoxLayout *layout = new QHBoxLayout();
    layout->addWidget(checkBox);
    layout->setMargin(0);
    layout->setAlignment(checkBox, Qt::AlignCenter);
    w->setLayout(layout);

    QComboBox *cb_type = new QComboBox(this);
    QStringList typeList;
    typeList<<"基础服务"<<"种子与密匙"<<"诊断在线";
    cb_type->addItems(typeList);
    cb_type->setFrame(false);
    cb_type->setCurrentIndex(cmdItem->type);


    QStandardItem *enableItem = new QStandardItem();
    QStandardItem *typeItem = new QStandardItem();
    QStandardItem *nameItem = new QStandardItem(cmdItem->name);

    UDS_CMD_REQ *cmdIns = cmdItem->cmdIns;

    QString reqDataStr = "";
    QString resDataStr = "";
    if(cmdIns)
    {
        reqDataStr = QString("%1").arg((cmdIns->cmd), 2, 16, QLatin1Char('0')).toUpper();
        for(int i = cmdIns->dataLen; i < cmdIns->dataLen; i++)
        {
            reqDataStr += (" " + QString("%1").arg((cmdIns->data[i]), 2, 16, QLatin1Char('0')).toUpper());
        }

        resDataStr = QString("%1").arg((cmdIns->cmd + 0x40), 2, 16, QLatin1Char('0')).toUpper();
        for(int i = cmdIns->dataLen; i < cmdIns->dataLen; i++)
        {
            resDataStr += (" " + QString("%1").arg((cmdIns->data[i]), 2, 16, QLatin1Char('0')).toUpper());
        }
    }

    QStandardItem *reqDataItem = new QStandardItem(reqDataStr);
    QStandardItem *resDataItem = new QStandardItem(resDataStr);
    QStandardItem *delayTimeItem = new QStandardItem(QString::number(cmdItem->postTime));
    delayTimeItem->setTextAlignment(Qt::AlignCenter);
    QStandardItem *retryItem = new QStandardItem(QString::number(cmdItem->retryTime));
    retryItem->setTextAlignment(Qt::AlignCenter);

    QList<QStandardItem*> itemList;
    itemList << enableItem <<typeItem << nameItem << reqDataItem << resDataItem << delayTimeItem << retryItem;

    flowItemModel->appendRow(itemList);

    ui->tableView_flowInfo->setIndexWidget(enableItem->index(), w);
    ui->tableView_flowInfo->setIndexWidget(typeItem->index(), cb_type);
}

bool UDS_Win::isCmdItemNameExisted(QString name, QList<UDS_CMD_ITEM_IN_FLOW *> itemList)
{
    foreach (UDS_CMD_ITEM_IN_FLOW *cmdItem, itemList) {
        if(name == cmdItem->name)
            return true;
    }

    return false;
}

void UDS_Win::on_actionSave_triggered()
{
    writeSetting();
}

void UDS_Win::on_actionActive_triggered()
{
    if(!udsWorker)
    {
        udsWorker = new NI_UDS_Worker(this);
    }
    packUdsInfo();
    udsWorker->setUdsInfo(udsInfo);

    if(ui->actionActive->isChecked() || udsWorker->getIsActive() == false)
    {
        if(udsWorker->uds_run())
        {
            ui->statusbar->showMessage("uds start ok!");

            //createTimerForTester();
        }
        else
        {
            ui->statusbar->showMessage("uds start error.");
            ui->actionActive->setChecked(false);
        }
    }
    else
    {
        //deleteTimerOfTester();

        if(udsWorker->uds_exit())
        {
            ui->statusbar->showMessage("uds stop ok!");
        }
        else
        {
            ui->statusbar->showMessage("uds stop error.");
        }
    }
}

void UDS_Win::on_pb_udsExecute_clicked()
{
    if(!udsWorker) return;
    if(!curCmdIns)
    {
        return;
    }

    if(!udsWorker->getIsActive())
    {
        QMessageBox::information(this,
                                 tr("提示"),
                                 tr("执行指令前请先激活！"),
                                 QMessageBox::Ok);
        return;
    }

    quint8 cmd = curCmdIns->cmd;

    switch (cmd) {
    case 0x10:
    {
        quint8 sessionMode = 0x00;
        sessionMode = curCmdIns->data[0];
        udsWorker->uds_manl_diagSessionControl(sessionMode);
        break;
    }
    case 0x11:
    {
        quint8 resetType = 0x00;
        resetType = curCmdIns->data[0];
        break;
    }
    case 0x22:
    {
        quint16 did = 0x0000;
        did = *(quint16*)(curCmdIns->data);
        reverseBytes((quint8*)&did, 2);
        quint8 *dataOut = curCmdIns->data + 2;
        qint32 lenOut = 4;
        udsWorker->uds_manl_readDataByIdentifier(did, dataOut, &lenOut);

        break;
    }
    case 0x27:
    {
        quint8 mode = 0x00;
        mode = curCmdIns->data[0];
        quint8 *seed = curCmdIns->data + 1;
        quint16 lenOfSeed = 4;
        quint8 *key = seed + lenOfSeed;
        quint16 lenOfKey = 4;
        udsWorker->uds_manl_saftyPass(mode, seed, lenOfSeed, key, lenOfKey);

        break;
    }
    case 0x2E:
    {
        quint16 did = 0x0000;
        did = *(quint16*)(curCmdIns->data);
        reverseBytes((quint8*)&did, 2);

        quint8 *dataIn = curCmdIns->data + 2;
        quint8 lenIn = 2;
        udsWorker->uds_manl_writeDataByIdentifier(did, dataIn, lenIn);
        break;
    }
    case 0x2F:
    {
        quint16 did = 0x0000;
        quint8 mode = 0x00;
        did = *(quint16*)(curCmdIns->data);
        reverseBytes((quint8*)&did, 2);
        mode = curCmdIns->data[2];

        quint8 *dataIn = curCmdIns->data + 3;
        quint32 lenIn = 4;
        quint8 *dataOut = dataIn + lenIn;
        quint32 lenOut = 4;
        udsWorker->uds_manl_ioControlByIdentifier(did, mode, dataIn, lenIn, dataOut, lenOut);

        break;
    }
    case 0x31:
    {
        quint8 mode = 0x00;
        mode = curCmdIns->data[0];
        quint16 did = 0x0000;
        did = *(quint16*)(curCmdIns->data + 1);
        reverseBytes((quint8*)&did, 2);

        quint8 *dataIn = curCmdIns->data + 3;
        quint32 lenIn = 1;
        dataIn[0] = 0x01;
        quint8 *dataOut = dataIn + lenIn;
        quint32 lenOut = 4;

        udsWorker->uds_manl_routineControl(did, mode, dataIn, lenIn, dataOut, lenOut);

        break;
    }
    case 0x3E:
    {
        quint8 mode = 0x00;
        mode = curCmdIns->data[0];
        bool responseRequired = true;
        if(mode == 0x80) responseRequired = false;
        udsWorker->uds_manl_testPresented(responseRequired);
        break;
    }
    default:
        break;
    }

}

void UDS_Win::on_pb_selDll_clicked()
{
    QString appPath = QApplication::applicationDirPath();
    QString filePath = QFileDialog::getOpenFileName(this,tr("Load dll file..."),appPath, tr("dbc file(*.dll *.DLL)"));
    if(filePath.isEmpty()) return;
    qDebug()<<"Selected dll file path = "<<filePath;

    ui->le_dllPath->setText(filePath);
}
