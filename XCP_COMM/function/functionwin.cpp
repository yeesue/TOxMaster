#include "functionwin.h"
#include "ui_functionwin.h"
#include "linedelegate.h"
#include <QDebug>
#include "rcdsetdlg.h"
#include "comboboxdelegate.h"

FunctionWin::FunctionWin(QWidget *parent, QString name) :
    QMainWindow(parent),
    workName(name),
    ui(new Ui::FunctionWin)
{
    ui->setupUi(this);
    setWindowTitle(workName);

    initFunMenuStyle();

    QStringList headers;
    headers << "名称" << "端口" << "设备" << "功能" << "状态" << "设置";

    QTreeView *tree = ui->treeView_functions;
    initFunTreeStyle();

    tree->setAlternatingRowColors(true);

    model_function = new QStandardItemModel();
    tree->setModel(model_function);
    tree->setContextMenuPolicy(Qt::CustomContextMenu);
    tree->setEditTriggers(QTreeView::DoubleClicked);
    tree->setSelectionBehavior(QTreeView::SelectRows);
    tree->setSelectionMode(QTreeView::SingleSelection);

    //tree->setAlternatingRowColors(true);
    tree->setFocusPolicy(Qt::NoFocus);
    tree->header()->setStretchLastSection(true);
    tree->header()->setDefaultAlignment(Qt::AlignCenter);

    model_function->setHorizontalHeaderLabels(headers);
    tree->header()->resizeSection(0, 300);
    tree->header()->resizeSection(1, 150);
    tree->header()->resizeSection(2, 150);
    tree->header()->resizeSection(3, 150);
    tree->header()->resizeSection(4, 150);

    ComboBoxDelegate *canPortComboBox = new ComboBoxDelegate(this);
    canPortComboBox->setItems(canPortArray);
    tree->setItemDelegateForColumn(1, canPortComboBox);

    ComboBoxDelegate *devTypeComboBox = new ComboBoxDelegate(this);
    devTypeComboBox->setItems(devTypeArray);
    tree->setItemDelegateForColumn(2, devTypeComboBox);

    ComboBoxDelegate *funTypeComboBox = new ComboBoxDelegate(this);
    funTypeComboBox->setItems(funTypeArray);
    tree->setItemDelegateForColumn(3, funTypeComboBox);


    LineDelegate *lineDelegate = new LineDelegate();
    tree->setItemDelegate(lineDelegate);

    showNIXnetIntfs(getNIXnetIntfs());

    connect(tree, &QTreeView::customContextMenuRequested, this, &FunctionWin::Slt_ContextMenuRequest);
    connect(lineDelegate, &LineDelegate::modelDataUpdated, this, &FunctionWin::Slt_ModelDataUpdated);
    connect(tree, &QTreeView::doubleClicked, this, &FunctionWin::Slt_DoubleClicked);

    connect(funTypeComboBox, &LineDelegate::modelDataUpdated, this, &FunctionWin::Slt_ModelDataUpdated);
    connect(canPortComboBox, &LineDelegate::modelDataUpdated, this, &FunctionWin::Slt_ModelDataUpdated);
    connect(devTypeComboBox, &LineDelegate::modelDataUpdated, this, &FunctionWin::Slt_ModelDataUpdated);

    readWinXml();

    ui->actionFunRun->setEnabled(true);
    ui->actionFunStop->setEnabled(false);
    ui->actionPamFactory->setEnabled(false);

    initSysPams();
}

FunctionWin::~FunctionWin()
{
    delete ui;
}

void FunctionWin::Slt_ContextMenuRequest(const QPoint &pos)
{
    QModelIndex curIndex = ui->treeView_functions->indexAt(pos);      //当前点击的元素的index

    QMenu menu;
    menu.addAction(QStringLiteral("New"), this, SLOT(Slt_New(bool)));
    menu.addAction(QStringLiteral("Open"), this, SLOT(Slt_Open(bool)));
    menu.addAction(QStringLiteral("Del"), this, SLOT(Slt_Del(bool)));
    menu.exec(QCursor::pos());
}


void FunctionWin::Slt_ModelDataUpdated(int row, int col, QString text)
{

    if(funList.count() <= row)
        return;

    FunStruct *fun = funList.at(row);

    if(col == 0)
    {
        fun->funName = text;

        QString fileName = workName.toLower() + "-" + fun->funName.toLower() + "-setting.ini";
        if(fun->funType == "CAN/CANFD")
        {
            fileName.replace(".ini", ".xml");
        }

        QString filePath_ref = "/work/" + workName + "/" + fileName;
        QString filePath = QApplication::applicationDirPath() + "/work/" + workName + "/" + fileName;

        if(fun->setFilePath != filePath_ref)
        {
            QFileInfo fileInfo(QApplication::applicationDirPath() + fun->setFilePath);
            if(fileInfo.isFile())
            {
                QFile file(QApplication::applicationDirPath() + fun->setFilePath);
                file.rename(filePath);
                fun->setFilePath = filePath_ref;
                qDebug()<<"function set file rename as:"<<fun->setFilePath;
            }
        }

    }
    else if(col == 1)
    {
        fun->intfName = text;
    }
    else if(col == 2)
    {
        fun->devType = text;
    }
    else if(col == 3)
    {
        fun->funType = text;
    }

    emit funUpdated();
}

void FunctionWin::Slt_DoubleClicked(QModelIndex index)
{
    int funIndex = index.row();
    int col = index.column();

    if(funList.count() <= funIndex)
        return;

    FunStruct *fun = funList.at(funIndex);
    qDebug()<<"funType doubleClick:"<<fun->funType;
    if(fun->funType.isEmpty())
        return;

    if(col == 5)
    {
        if(fun->funType == "XCP")
        {
            qInfo()<<"XCP设置: "<<fun->setFilePath;
            XcpSetDlg *xcpSet = new XcpSetDlg(nullptr, this->a2lProjectWin, workName, fun->funName, fun->intfName, fun->setFilePath);
            xcpSet->setAttribute(Qt::WA_DeleteOnClose);
            connect(xcpSet, &XcpSetDlg::xcpSetUpdated, this, &FunctionWin::funUpdated);
            if(fun->setFilePath.isEmpty())
            {
                //fun->setFilePath = xcpSet->getXcpSetFilePath();
                fun->setFilePath = xcpSet->getXcpSetFilePath().replace(QCoreApplication::applicationDirPath(), "");
            }
            xcpSet->show();
        }
        else if(fun->funType == "CAN/CANFD")
        {
            qInfo()<<"CAN/CANFD设置: "<<fun->setFilePath;

            if(fun->devType == "NI-XNET")
            {
                CanSetDlg *canSet = new CanSetDlg(nullptr, workName, fun->funName, fun->intfName, fun->setFilePath);
                canSet->setAttribute(Qt::WA_DeleteOnClose);
                connect(canSet, &CanSetDlg::canSetUpdated, this, &FunctionWin::funUpdated);
                if(fun->setFilePath.isEmpty())
                {
                    //fun->setFilePath = canSet->getCanSetFilePath();
                    fun->setFilePath = canSet->getCanSetFilePath().replace(QCoreApplication::applicationDirPath(), "");
                }
                canSet->show();
            }
            else if(fun->devType == "TS-CAN")
            {
                TsCanSetDlg *tsCanSet = new TsCanSetDlg(nullptr, workName, fun->funName, fun->intfName, fun->setFilePath);
                tsCanSet->setAttribute(Qt::WA_DeleteOnClose);
                connect(tsCanSet, &TsCanSetDlg::canSetUpdated, this, &FunctionWin::funUpdated);
                if(fun->setFilePath.isEmpty())
                {
                    fun->setFilePath = tsCanSet->getCanSetFilePath().replace(QCoreApplication::applicationDirPath(), "");
                }
                tsCanSet->show();
            }
            else if(fun->devType == "ZLG-CAN")
            {
                ZlgCanSetDlg *tsCanSet = new ZlgCanSetDlg(nullptr, workName, fun->funName, fun->intfName, fun->setFilePath);
                tsCanSet->setAttribute(Qt::WA_DeleteOnClose);
                connect(tsCanSet, &ZlgCanSetDlg::canSetUpdated, this, &FunctionWin::funUpdated);
                if(fun->setFilePath.isEmpty())
                {
                    fun->setFilePath = tsCanSet->getCanSetFilePath().replace(QCoreApplication::applicationDirPath(), "");
                }
                tsCanSet->show();
            }


        }
        else if(fun->funType == "UDS")
        {
            /*
            qInfo()<<"UDS设置: ";
            UdsSetDlg *udsSet = new UdsSetDlg(nullptr, workName, fun->funName, fun->intfName, fun->setFilePath);
            udsSet->setAttribute(Qt::WA_DeleteOnClose);
            connect(udsSet, SIGNAL(udsSetUpdated()), this, SIGNAL(funUpdated()));
            if(fun->setFilePath.isEmpty())
            {
                fun->setFilePath = udsSet->getUdsSetFilePath();
            }
            udsSet->show();
            */
        }
        else if(fun->funType == "WT3000")
        {
            qInfo()<<"WT3000设置: "<<fun->setFilePath;
            WT3000SetDlg *wt3000Set = new WT3000SetDlg(nullptr, workName, fun->funName, fun->intfName, fun->setFilePath);
            wt3000Set->setAttribute(Qt::WA_DeleteOnClose);
            connect(wt3000Set, &WT3000SetDlg::setUpdated, this, &FunctionWin::funUpdated);
            if(fun->setFilePath.isEmpty())
            {
                fun->setFilePath = wt3000Set->getSetFilePath().replace(QCoreApplication::applicationDirPath(), "");
            }
            wt3000Set->show();
        }
        else if(fun->funType == "WT5000")
        {
            qInfo()<<"WT5000设置: "<<fun->setFilePath;
            WT5000SetDlg *wt5000Set = new WT5000SetDlg(nullptr, workName, fun->funName, fun->intfName, fun->setFilePath);
            wt5000Set->setAttribute(Qt::WA_DeleteOnClose);
            connect(wt5000Set, &WT5000SetDlg::setUpdated, this, &FunctionWin::funUpdated);
            if(fun->setFilePath.isEmpty())
            {
                fun->setFilePath = wt5000Set->getSetFilePath().replace(QCoreApplication::applicationDirPath(), "");
            }
            wt5000Set->show();
        }
        else if(fun->funType == "INCA_COM")
        {
            qInfo()<<"INCA COM: "<<fun->setFilePath;
        }


        QString fileName = fun->setFilePath.split("/").last();
        //model_function->itemFromIndex(index)->setText(fileName);
        qDebug()<<"fun set file name:"<<fileName;
    }

}

void FunctionWin::Slt_FunStateChanged(int state, int funIndex)
{
    if(funIndex >= funList.count())
        return;
    QColor bgColor = Qt::gray;
    QString stateStr = "Stop";

    switch (state) {
    case -1:
        bgColor = Qt::red;
        stateStr = "Error";
        break;
    case 0:
        bgColor = Qt::gray;
        stateStr = "Stop";
        break;
    case 1:
        bgColor = Qt::yellow;
        stateStr = "Inited";
        break;
    case 2:
        bgColor = Qt::green;
        stateStr = "Setuped";
        break;
    case 3:
        bgColor = Qt::green;
        stateStr = "Running";
        break;
    default:
        break;
    }

    model_function->item(funIndex, 4)->setText(stateStr);
    model_function->item(funIndex, 4)->setBackground(bgColor);
}

void FunctionWin::Slt_XcpMsg(QString msg)
{
    msg = "[XCP]: " + msg;

    ui->statusbar->showMessage(msg);
    qInfo()<<msg;
}

void FunctionWin::Slt_CharValueUpdated(A2L_VarChar *charVar, qreal value)
{
    if(!charVar)
        return;
    if(charVar->funIndex == -1 || charVar->smKey.isEmpty())
        return;
    Xcp_Fun_Thread *xcpFunThread = (Xcp_Fun_Thread*)(funThreadHash.value(charVar->funIndex));
    if(!xcpFunThread)
        return;

    xcpFunThread->updateCharValueInSM(charVar, value);
}

void FunctionWin::Slt_PamValueUpdated(PARAM *pam, qreal value)
{
    //qDebug()<<"slt pamValueUpdated active";

    if(!pam)
        return;
    if(pam->funIndex == -1 || pam->smKey.isEmpty())
        return;

    if(pam->pamModule == 0)
    {

    }
    else if(pam->pamModule == 1)
    {
        //qDebug()<<"slt pamValueUpdated active in pamMoudule = 1";
        A2L_VarChar *charVar = (A2L_VarChar*)pam;

        Xcp_Fun_Thread *xcpFunThread = (Xcp_Fun_Thread*)(funThreadHash.value(charVar->funIndex));
        if(!xcpFunThread)
            return;
        xcpFunThread->updateCharValueInSM(charVar, value);
    }
    else if(pam->pamModule == 2)
    {

    }
    else
    {

    }

}

void FunctionWin::Slt_New(bool flag)
{
    int count = funList.count();

    int index = 0;
    QString name = "CAN" + QString::number(index);
    while(findIntfIndexByName(name) >= 0 && index < 10)
    {
        index++;
        name = "CAN" + QString::number(index);
    }

    FunStruct *fun = new FunStruct;
    fun->funName = "Fun_Default";
    fun->intfName = name;

    funList.append(fun);

    model_function->appendRow(packFunInfoToItems(fun));

    Slt_FunStateChanged(0, funList.count() - 1);

    emit funUpdated();
}

void FunctionWin::Slt_Open(bool flag)
{

}

void FunctionWin::Slt_Del(bool flag)
{
    QModelIndex curIndex = ui->treeView_functions->currentIndex();
    int rowIndex = curIndex.row();

    FunStruct *fun = funList.at(rowIndex);

    int ret = QMessageBox::warning(this, tr("Delete Warning"),
                                     tr("Confirm to delete fun [") + fun->funName + "]?",
                                     QMessageBox::Ok | QMessageBox::Cancel,
                                     QMessageBox::Cancel);
    if(ret == QMessageBox::Cancel)
        return;

    model_function->removeRow(rowIndex);
    funList.takeAt(rowIndex);

    emit funUpdated();
}

void FunctionWin::Slt_ShowRecordTimeInTimeEdit(QString timeStr)
{
    ui->timeEdit->setTime(QTime::fromString(timeStr, "hh:mm:ss"));
}

void FunctionWin::Slt_cycleNumUpdated(quint32 curCycle)
{
    QString msg = "Record cycle finished [" + QString::number(curCycle-1) +
            "] of [" + QString::number(rcdSet.cycleNum) + "] - cycle time(s) [" +
            QString::number(rcdSet.singleCycleTime_s) + "]";

    ui->statusbar->showMessage(msg);

    if(curCycle > rcdSet.cycleNum)
    {
        ui->actionRcdOn->setChecked(false);
        on_actionRcdOn_triggered();
    }
}

QStringList FunctionWin::getNIXnetIntfs()
{
    QStringList interfaceList;
    nxSessionRef_t sessionSys;
    nxStatus_t status = nxSystemOpen(&sessionSys);
    u32 intfRefs[20];
    if (status == nxSuccess)
    {
        status = nxGetProperty(sessionSys, nxPropSys_IntfRefs, sizeof(u32)*20, intfRefs);
        if (status == nxSuccess)
        {
            for (int i = 0; i < 20; i++)
            {
                u32 propSize = 0;
                if (intfRefs[i])
                {
                    status = nxGetPropertySize(intfRefs[i], nxPropIntf_Name, &propSize);
                    if(propSize <= 0 || propSize > 6)
                        continue;
                    char *intfNameChar = new char[propSize];
                    status = nxGetProperty(intfRefs[i], nxPropIntf_Name, propSize, intfNameChar);
                    QString intfName =  QString::fromStdString(intfNameChar);
                    if (!intfName.isEmpty() && intfName.contains("CAN"))
                        interfaceList << intfName;
                }

            }
        }
    }
    return interfaceList;
}

void FunctionWin::showNIXnetIntfs(QStringList intfs)
{
    QString msg;
    msg += "可用NI-XNET端口: ";
    foreach (QString intf, intfs) {
        msg += intf;
        msg += "/";
    }
    qDebug()<<msg;
    ui->statusbar->showMessage(msg);

}

QStringList FunctionWin::getTSCanIntfs()
{
    QStringList interfaceList;

    //TSCANLINApi *tsApiObj = new TSCANLINApi();

    //扫描获取可用同星设备
    uint32_t deviceCount = 0;
    quint32 retValue = tscan_scan_devices(&deviceCount);
    if(retValue == 0)
    {
        qDebug()<<"同星设备数量:"<<deviceCount;
    }
    else
    {
        qDebug()<<"获取同星设备数量,状态:"<<retValue;
    }


    for(int i = 0; i < deviceCount; i++)
    {
        QString deviceName = "CAN" + QString::number(i+1);
        interfaceList.append(deviceName);
    }

    return interfaceList;
}

void FunctionWin::showTSCanIntfs(QStringList intfs)
{
    QString msg;
    msg += "可用TS-CAN端口: ";
    foreach (QString intf, intfs) {
        msg += intf;
        msg += "/";
    }
    qDebug()<<msg;

    ui->statusbar->showMessage(ui->statusbar->currentMessage() + "|" + msg);
}

void FunctionWin::showFunctions()
{
    model_function->clear();

    QStringList headers;
    headers << "名称" << "端口" << "设备" << "功能" << "状态" << "设置";
    model_function->setHorizontalHeaderLabels(headers);
    int width = ui->treeView_functions->width();
    ui->treeView_functions->header()->resizeSection(0, 300);
    ui->treeView_functions->header()->resizeSection(1, 150);
    ui->treeView_functions->header()->resizeSection(2, 150);
    ui->treeView_functions->header()->resizeSection(3, 150);
    ui->treeView_functions->header()->resizeSection(4, 150);

    for(int i = 0; i < funList.count(); i++)
    {
        FunStruct *fun = funList.at(i);

        model_function->appendRow(packFunInfoToItems(fun));

        Slt_FunStateChanged(0, i);
    }

    ui->treeView_functions->resizeColumnToContents(0);
}

QList<QStandardItem *> FunctionWin::packFunInfoToItems(FunStruct *fun)
{
    QStandardItem *funNameItem = new QStandardItem(QIcon(":/icon/icon/can.png"), fun->funName);

    QStandardItem *intfItem = new QStandardItem(QIcon(":/icon/icon/collect.png"), fun->intfName);
    intfItem->setTextAlignment(Qt::AlignCenter);   

    QStandardItem *devTypeItem = new QStandardItem(fun->devType);
    devTypeItem->setTextAlignment(Qt::AlignCenter);

    QStandardItem *funTypeItem = new QStandardItem(QIcon(":/icon/icon/fun.png"), fun->funType);
    funTypeItem->setTextAlignment(Qt::AlignCenter);

    QStandardItem *statusItem = new QStandardItem(QIcon(":/icon/icon/lib.png"), "");
    statusItem->setTextAlignment(Qt::AlignCenter);

    QStandardItem *setItem = new QStandardItem(QIcon(":/icon/icon/setting.png"), "[setting]: " + fun->setFilePath.split("/").last());
    setItem->setTextAlignment(Qt::AlignCenter);

    QList<QStandardItem*> itemList;
    itemList << funNameItem << intfItem << devTypeItem << funTypeItem << statusItem << setItem;

    return itemList;
}

int FunctionWin::findIntfIndexByName(QString intfName)
{
    if(funList.isEmpty())
        return -1;

    for(int i = 0; i < funList.count(); i++)
    {
        if(funList.at(i)->intfName == intfName)
            return i;
    }
    return -1;
}

QString FunctionWin::genDefaultTableWinName()
{
    for(int i = 0; i < 100; i++)
    {
        QString winName = "Table_";
        winName += QString::number(i);
        if(winHash.uniqueKeys().indexOf(winName) == -1)
            return winName;
    }

    return "";
}

QString FunctionWin::genDefaultCalTableWinName()
{
    for(int i = 0; i < 100; i++)
    {
        QString winName = "Table_Cal_";
        winName += QString::number(i);
        if(winHash.uniqueKeys().indexOf(winName) == -1)
            return winName;
    }

    return "";
}

QString FunctionWin::genDefaultChartWinName()
{
    for(int i = 0; i < 100; i++)
    {
        QString winName = "Chart_";
        winName += QString::number(i);
        if(winHash.uniqueKeys().indexOf(winName) == -1)
            return winName;
    }

    return "";
}

TableWin *FunctionWin::addReadTable(QString winName)
{
    TableWin *table = new TableWin(this, winName, 0);

    connect(this, &FunctionWin::runActive, table, &TableWin::Slt_RunActive);
    connect(table, &TableWin::winDeleted, this, &FunctionWin::Slt_TableWinDeleted);
    connect(table, &TableWin::winUpdated, this, &FunctionWin::Slt_WinUpdated);

    tableWinList.append(table);
    winHash.insert(table->getWinName(), table);

    QAction *tableAction = new QAction(QIcon(":/icon/icon/tableWin.png"), table->getWinName(), this);
    connect(tableAction, &QAction::triggered, table, &TableWin::Slt_Trigger);


    QMenu *tableMenu = ui->menuTable;
    tableMenu->addAction(tableAction);

    actionHash.insert(table->getWinName(), tableAction);

    return table;
}

TableWin *FunctionWin::addWriteTable(QString winName)
{
    TableWin *table = new TableWin(this, winName, 1);

    connect(this, &FunctionWin::runActive, table, &TableWin::Slt_RunActive);
    connect(table, &TableWin::winDeleted, this, &FunctionWin::Slt_CalTableWinDeleted);
    connect(table, &TableWin::winUpdated, this, &FunctionWin::Slt_WinUpdated);

    calTableWinList.append(table);
    winHash.insert(table->getWinName(), table);

    QAction *tableAction = new QAction(QIcon(":/icon/icon/tableWinWrite.png"), table->getWinName(), this);
    connect(tableAction, &QAction::triggered, table, &TableWin::Slt_Trigger);

    QMenu *tableMenu = ui->menuTableCal;
    tableMenu->addAction(tableAction);

    actionHash.insert(table->getWinName(), tableAction);

    return table;
}

PlotWin *FunctionWin::addPlotWin(QString winName)
{
    PlotWin *plot = new PlotWin(this);
    plot->setWinName(winName);

    connect(this, &FunctionWin::runActive, plot, &PlotWin::Slt_RunActive);
    connect(plot, &PlotWin::winDeleted, this, &FunctionWin::Slt_PlotWinDeleted);
    connect(plot, &PlotWin::winUpdated, this, &FunctionWin::Slt_WinUpdated);

    plotWinList.append(plot);
    winHash.insert(plot->getWinName(), plot);

    QAction *plotAction = new QAction(QIcon(":/icon/icon/chartWin.png"), plot->getWinName(), this);
    connect(plotAction, &QAction::triggered, plot, &PlotWin::Slt_Triggered);


    QMenu *plotMenu = ui->menuChart;
    plotMenu->addAction(plotAction);

    actionHash.insert(plot->getWinName(), plotAction);

    return plot;
}

bool FunctionWin::writeWinXml()
{
    QString appPath = QApplication::applicationDirPath();
    QString fileName = "win_config-" + workName.toLower() +".xml";
    QString winXmlPath = appPath + "/work/" + workName + "/" + fileName;

    workWinFilePath = winXmlPath;


    if(winXmlPath.isEmpty())
        return false;

    QFile xmlFile(winXmlPath);
    if (!xmlFile.open(QFile::WriteOnly | QFile::Text))
    {
        qDebug()<<tr("Cannot write file %1:\n%2.").arg(winXmlPath).arg(xmlFile.errorString());
        return false;
    }
    qDebug()<<"保存窗口配置文件xml:"<<winXmlPath;

    QDomDocument xml("WIN_CONFIG");

    QDomElement root = xml.createElement("Root");
    xml.appendChild(root);

    QDomElement readTableRootElem = xml.createElement("TableReadRoot");

    root.appendChild(readTableRootElem);

    for(int i = 0; i < tableWinList.count(); i++)
    {
        TableWin *table = tableWinList.at(i);

        QDomElement readTableElem = xml.createElement("ReadTable_" + QString::number(i));
        readTableRootElem.appendChild(readTableElem);

        readTableElem.setAttribute("WinName", table->getWinName());

        QRect winRect = table->geometry();
        readTableElem.setAttribute("xPos", winRect.x());
        readTableElem.setAttribute("yPos", winRect.y());
        readTableElem.setAttribute("width", winRect.width());
        readTableElem.setAttribute("height", winRect.height());

        QList<PamInfo> pams = table->getReadPams();
        for(int j = 0; j < pams.count(); j++)
        {
            PamInfo pamInfo = pams.at(j);
            QDomElement pamElem = xml.createElement("Pam_" + QString::number(j));
            readTableElem.appendChild(pamElem);

            pamElem.setAttribute("ModName", pamInfo.modName);
            pamElem.setAttribute("PamName", pamInfo.pamName);
        }
    }


    QDomElement writeTableRootElem = xml.createElement("TableWriteRoot");

    root.appendChild(writeTableRootElem);

    for(int i = 0; i < calTableWinList.count(); i++)
    {
        TableWin *table = calTableWinList.at(i);

        QDomElement writeTableElem = xml.createElement("WriteTable_" + QString::number(i));
        writeTableRootElem.appendChild(writeTableElem);

        writeTableElem.setAttribute("WinName", table->getWinName());

        QRect winRect = table->geometry();
        writeTableElem.setAttribute("xPos", winRect.x());
        writeTableElem.setAttribute("yPos", winRect.y());
        writeTableElem.setAttribute("width", winRect.width());
        writeTableElem.setAttribute("height", winRect.height());

        QList<PamInfo> pams = table->getWritePams();
        for(int j = 0; j < pams.count(); j++)
        {
            PamInfo pamInfo = pams.at(j);
            QDomElement pamElem = xml.createElement("Pam_" + QString::number(j));
            writeTableElem.appendChild(pamElem);

            pamElem.setAttribute("ModName", pamInfo.modName);
            pamElem.setAttribute("PamName", pamInfo.pamName);
            pamElem.setAttribute("PamSetValue", QString::number(pamInfo.pamSetValue, 'g', 8));
            //qDebug()<<"pam setValue:"<<pamInfo.pamSetValue;
        }
    }

    QDomElement plotWinRootElem = xml.createElement("PlotWinRoot");

    root.appendChild(plotWinRootElem);

    for(int i = 0; i < plotWinList.count(); i++)
    {
        PlotWin *plot = plotWinList.at(i);

        QDomElement plotWinElem = xml.createElement("PlotWin_" + QString::number(i));
        plotWinRootElem.appendChild(plotWinElem);

        plotWinElem.setAttribute("WinName", plot->getWinName());

        QRect winRect = plot->geometry();
        plotWinElem.setAttribute("xPos", winRect.x());
        plotWinElem.setAttribute("yPos", winRect.y());
        plotWinElem.setAttribute("width", winRect.width());
        plotWinElem.setAttribute("height", winRect.height());

        ChartProp chartProp = plot->getChartProp();
        plotWinElem.setAttribute("timeGap", chartProp.timeGap);

        QList<PamInfo> pams = plot->getPamInfoList();
        for(int j = 0; j < pams.count(); j++)
        {
            PamInfo pamInfo = pams.at(j);
            QDomElement pamElem = xml.createElement("Pam_" + QString::number(j));
            plotWinElem.appendChild(pamElem);

            pamElem.setAttribute("ModName", pamInfo.modName);
            pamElem.setAttribute("PamName", pamInfo.pamName);

            pamElem.setAttribute("AutoScale", pamInfo.autoScale ? 1 : 0);
            pamElem.setAttribute("ShowMin", pamInfo.showMin);
            pamElem.setAttribute("ShowMax", pamInfo.showMax);
        }
    }

    QString xmlString = xml.toString();
    QTextStream out(&xmlFile);
    out<<xmlString;

    ui->statusbar->showMessage("窗口配置文件xml保存");

    return true;

}

bool FunctionWin::readWinXml()
{
    QString appPath = QApplication::applicationDirPath();
    QString fileName = "win_config-" + workName.toLower() +".xml";
    QString winXmlPath = appPath + "/work/" + workName + "/" + fileName;

    workWinFilePath = winXmlPath;


    if(winXmlPath.isEmpty())
        return false;

    QFile xmlFile(winXmlPath);
    if (!xmlFile.open(QFile::ReadOnly | QFile::Text))
    {
        qDebug()<<tr("Cannot read file %1:\n%2.").arg(winXmlPath).arg(xmlFile.errorString());
        return false;
    }
    qDebug()<<"读取窗口配置文件xml:"<<winXmlPath;

    QDomDocument xml("WIN_CONFIG");
    if(!xml.setContent(&xmlFile))
    {
        xmlFile.close();
        return false;
    }
    xmlFile.close();

    QDomElement rootElem = xml.documentElement();

    QDomElement elem = rootElem.firstChildElement();

    while(!elem.isNull())
    {
        QString elemStr = elem.tagName();

        if(elemStr == "TableReadRoot")
        {
            QDomElement readTableElem = elem.firstChildElement();

            while(!readTableElem.isNull())
            {
                QString winName = readTableElem.attribute("WinName");

                int xPos = readTableElem.attribute("xPos").toInt();
                int yPos = readTableElem.attribute("yPos").toInt();
                int width = readTableElem.attribute("width").toInt();
                int height = readTableElem.attribute("height").toInt();

                QRect winRect(xPos, yPos, width, height);

                TableWin *table = addReadTable(winName);

                table->setGeometry(winRect);

                QList<PamInfo> pamInfoList;

                QDomElement pamElem = readTableElem.firstChildElement();

                while(!pamElem.isNull())
                {
                    QString modName = pamElem.attribute("ModName");
                    QString pamName = pamElem.attribute("PamName");

                    PamInfo pamInfo;
                    pamInfo.modName = modName;
                    pamInfo.pamName = pamName;
                    pamInfoList.append(pamInfo);

                    pamElem = pamElem.nextSiblingElement();
                }

                table->setReadPams(pamInfoList);

                readTableElem = readTableElem.nextSiblingElement();
            }

        }
        else if(elemStr == "TableWriteRoot")
        {
            QDomElement writeTableElem = elem.firstChildElement();

            while(!writeTableElem.isNull())
            {
                QString winName = writeTableElem.attribute("WinName");

                int xPos = writeTableElem.attribute("xPos").toInt();
                int yPos = writeTableElem.attribute("yPos").toInt();
                int width = writeTableElem.attribute("width").toInt();
                int height = writeTableElem.attribute("height").toInt();

                QRect winRect(xPos, yPos, width, height);

                TableWin *table = addWriteTable(winName);

                table->setGeometry(winRect);

                QList<PamInfo> pamInfoList;

                QDomElement pamElem = writeTableElem.firstChildElement();

                while(!pamElem.isNull())
                {
                    QString modName = pamElem.attribute("ModName");
                    QString pamName = pamElem.attribute("PamName");
                    double pamSetValue = pamElem.attribute("PamSetValue").toDouble();

                    PamInfo pamInfo;
                    pamInfo.modName = modName;
                    pamInfo.pamName = pamName;
                    pamInfo.pamSetValue = pamSetValue;
                    pamInfoList.append(pamInfo);

                    pamElem = pamElem.nextSiblingElement();
                }

                table->setWritePams(pamInfoList);

                writeTableElem = writeTableElem.nextSiblingElement();
            }
        }
        else if(elemStr == "PlotWinRoot")
        {
            QDomElement plotWinElem = elem.firstChildElement();

            while(!plotWinElem.isNull())
            {
                QString winName = plotWinElem.attribute("WinName");

                int xPos = plotWinElem.attribute("xPos").toInt();
                int yPos = plotWinElem.attribute("yPos").toInt();
                int width = plotWinElem.attribute("width").toInt();
                int height = plotWinElem.attribute("height").toInt();

                QRect winRect(xPos, yPos, width, height);

                PlotWin *plot = addPlotWin(winName);

                plot->setGeometry(winRect);

                ChartProp chartProp;
                chartProp.timeGap = plotWinElem.attribute("timeGap").toUInt();

                plot->setChartProp(chartProp);

                QList<PamInfo> pamInfoList;

                QDomElement pamElem = plotWinElem.firstChildElement();

                while(!pamElem.isNull())
                {
                    QString modName = pamElem.attribute("ModName");
                    QString pamName = pamElem.attribute("PamName");
                    bool autoScale = pamElem.attribute("AutoScale").toUInt() > 1;
                    double showMin = pamElem.attribute("ShowMin").toDouble();
                    double showMax = pamElem.attribute("ShowMax").toDouble();

                    PamInfo pamInfo;
                    pamInfo.modName = modName;
                    pamInfo.pamName = pamName;

                    pamInfo.autoScale = autoScale;
                    pamInfo.showMin = showMin;
                    pamInfo.showMax = showMax;

                    pamInfoList.append(pamInfo);

                    pamElem = pamElem.nextSiblingElement();
                }

                plot->setPamInfoList(pamInfoList);

                plotWinElem = plotWinElem.nextSiblingElement();
            }

        }

        elem = elem.nextSiblingElement();
    }

    return true;
}

void FunctionWin::loadRecordSetting()
{
    QString workRecordDir = QApplication::applicationDirPath() + "/work/" + workName + "/record";
    QString recordSetFilePath = workRecordDir + "/record_setting.ini";
    QSettings setting(recordSetFilePath, QSettings::IniFormat);

    qInfo()<<"read record setting:" + setting.fileName();

    rcdSet.recordFileName = setting.value("RecordDataFileName").toString();
    rcdSet.cyclicRecord = setting.value("CycleRecord").toBool();
    rcdSet.cycleNum = setting.value("NumberOfCycle").toUInt();
    rcdSet.singleCycleTime_s = setting.value("SingleCycleTime").toUInt();

    mdfFileName = rcdSet.recordFileName;
}

void FunctionWin::packGlobalPamHash()
{
    QStringList modList = Globals::funPamListHash.uniqueKeys();
    for(int i = 0; i < modList.count(); i++)
    {
        QString modName = modList.at(i);
        QList<PARAM*> pams = Globals::funPamListHash.value(modName);
        foreach (PARAM *pam, pams) {
            Globals::pamHash.insert(pam->Name + "." + modName, pam);
        }
    }

    QStringList modList_write = Globals::funWritePamListHash.uniqueKeys();
    for(int i = 0; i < modList_write.count(); i++)
    {
        QString modName = modList_write.at(i);
        QList<PARAM*> pams = Globals::funWritePamListHash.value(modName);
        foreach (PARAM *pam, pams) {
            Globals::pamHash.insert(pam->Name + "." + modName, pam);
        }
    }
}

void FunctionWin::packGlobalMapPamHash()
{
    QStringList modList_map = Globals::funMapPamListHash.uniqueKeys();
    for(int i = 0; i < modList_map.count(); i++)
    {
        QString modName = modList_map.at(i);
        QList<PARAM*> pams = Globals::funMapPamListHash.value(modName);
        foreach (PARAM *pam, pams) {
            Globals::mapPamHash.insert(pam->Name + "." + modName, pam);
        }
    }
}

void FunctionWin::runAllFun()
{

    Globals::funWritePamListHash.insert(autoPam->getSmKey(), autoPam->getPamList());
    Globals::funWritePamListHash.insert(sysPam->getSmKey(), sysPam->getPamList());

    for(int i = 0; i < funList.count(); i++)
    {
        FunStruct *fun = funList.at(i);

        QThread *funThread = NULL;
        //if(fun->funType.isEmpty() || fun->intfName.isEmpty() || fun->setFilePath.isEmpty())
        if(fun->funType.isEmpty() || fun->intfName.isEmpty())
        {
            funThreadHash.insert(i, funThread);
            continue;
        }
        else if(fun->funType == "XCP")
        {
            Xcp_Fun_Thread *xcpFunThread = new Xcp_Fun_Thread();
            xcpFunThread->setA2lWin(this->a2lProjectWin);
            xcpFunThread->setFunInfo(fun, i);
            xcpFunThread->preFun();
            xcpFunThread->start();

            connect(xcpFunThread, &Xcp_Fun_Thread::xcpFunStateChanged, this, &FunctionWin::Slt_FunStateChanged);
            connect(xcpFunThread, &Xcp_Fun_Thread::xcpMsg, this, &FunctionWin::Slt_XcpMsg);

            waitForXcpThreadStartFinished(xcpFunThread, 10000);

            funThreadHash.insert(i, xcpFunThread);

            //funPamListHash.insert(xcpFunThread->getSmKeyRead(), xcpFunThread->getPamList());
            //funPamListHash.insert(xcpFunThread->getSmKeyReadDAQ(), xcpFunThread->getDaqPamList());

            Globals::funPamListHash.insert(xcpFunThread->getSmKeyRead(), xcpFunThread->getPamList());
            Globals::funPamListHash.insert(xcpFunThread->getSmKeyReadDAQ(), xcpFunThread->getDaqPamList());

            //funWritePamListHash.insert(xcpFunThread->getSmKeyWrite(), xcpFunThread->getCharValuePamList());
            Globals::funWritePamListHash.insert(xcpFunThread->getSmKeyWrite(), xcpFunThread->getCharValuePamList());

            //funMapPamListHash.insert(xcpFunThread->getMapListKey(), xcpFunThread->getCharCurveMapPamList());
            Globals::funMapPamListHash.insert(xcpFunThread->getMapListKey(), xcpFunThread->getCharCurveMapPamList());


        }
        else if(fun->funType == "UDS")
        {
            /*
            Uds_Fun_Thread *udsFunThread = new Uds_Fun_Thread(this);
            udsFunThread->setFunInfo(fun, i);
            udsFunThread->preFun();
            udsFunThread->setUdsRun(true);

            connect(udsFunThread, &UDS_Fun_Thread::udsFunStateChanged, this, &FunctionWin::Slt_FunStateChanged);

            udsFunThread->start();

            waitForUdsThreadStartFinished(udsFunThread, 3000);
            funThreadHash.insert(i, udsFunThread);

            funPamListHash.insert("RP_UDS_" + fun->funName, udsFunThread->getUdsReadPamList());
            funWritePamListHash.insert("WP_UDS_" + fun->funName, udsFunThread->getUdsWritePamList());
            */
        }
        else if(fun->funType == "CAN/CANFD")
        {
            if(fun->devType == "NI-XNET")
            {
                Can_Fun_Thread *canFunThread = new Can_Fun_Thread(this);
                canFunThread->setFunInfo(fun, i);

                connect(canFunThread, &CAN_Fun_Thread::canFunStateChanged, this, &FunctionWin::Slt_FunStateChanged);

                canFunThread->start();

                waitForCanThreadStartFinished(canFunThread, 2000);

                funThreadHash.insert(i, canFunThread);

                Globals::funPamListHash.unite(canFunThread->getReadPamListHash());
                Globals::funWritePamListHash.unite(canFunThread->getWritePamListHash());

            }
            else if(fun->devType == "TS-CAN")
            {
                Can_Fun_Thread_TS *tsCanFunThread = new Can_Fun_Thread_TS(this);
                tsCanFunThread->setFunInfo(fun, i);

                connect(tsCanFunThread, &CAN_Fun_Thread_TS::canFunStateChanged, this, &FunctionWin::Slt_FunStateChanged);

                tsCanFunThread->start();

                waitForTsCanThreadStartFinished(tsCanFunThread, 3000);

                funThreadHash.insert(i, tsCanFunThread);

                qDebug()<<"check pamlist hash";

                Globals::funPamListHash.unite(tsCanFunThread->getReadPamListHash());
                Globals::funWritePamListHash.unite(tsCanFunThread->getWritePamListHash());
            }
            else if(fun->devType == "ZLG-CAN")
            {
                Can_Fun_Thread_ZLG *zlgCanFunThread = new Can_Fun_Thread_ZLG(this);
                zlgCanFunThread->setFunInfo(fun, i);

                connect(zlgCanFunThread, &CAN_Fun_Thread_ZLG::canFunStateChanged, this, &FunctionWin::Slt_FunStateChanged);

                zlgCanFunThread->start();

                waitForZlgCanThreadStartFinished(zlgCanFunThread, 3000);

                funThreadHash.insert(i, zlgCanFunThread);

                qDebug()<<"check pamlist hash";

                Globals::funPamListHash.unite(zlgCanFunThread->getReadPamListHash());
                Globals::funWritePamListHash.unite(zlgCanFunThread->getWritePamListHash());
            }


        }
        else if(fun->funType == "WT3000")
        {
            WT3000_Thread *wt3000FunThread = new WT3000_Thread();
            wt3000FunThread->setFunInfo(fun, i);

            connect(wt3000FunThread, &WT3000_Fun_Thread::wtFunStateChanged, this, &FunctionWin::Slt_FunStateChanged);

            wt3000FunThread->start();

            waitForWT3000ThreadStartFinished(wt3000FunThread, 3000);

            funThreadHash.insert(i, wt3000FunThread);

            funPamListHash.insert(wt3000FunThread->getSmKey(), wt3000FunThread->getReadPamList());            
            Globals::funPamListHash.insert(wt3000FunThread->getSmKey(), wt3000FunThread->getReadPamList());

            //funWritePamListHash.unite(wt3000FunThread->getWritePamListHash());

        }
        else if(fun->funType == "WT5000")
        {
            WT5000_Thread *wt5000FunThread = new WT5000_Thread();
            wt5000FunThread->setFunInfo(fun, i);

            connect(wt5000FunThread, &WT5000_Fun_Thread::wtFunStateChanged, this, &FunctionWin::Slt_FunStateChanged);

            wt5000FunThread->start();

            waitForWT5000ThreadStartFinished(wt5000FunThread, 3000);

            funThreadHash.insert(i, wt5000FunThread);

            //funPamListHash.insert(wt5000FunThread->getSmKey(), wt5000FunThread->getReadPamList());
            Globals::funPamListHash.insert(wt5000FunThread->getSmKey(), wt5000FunThread->getReadPamList());

            //funWritePamListHash.unite(wt5000FunThread->getWritePamListHash());

        }
        else if(fun->funType == "INCA_COM")
        {
            INCA_COM_Fun_Thread *incaComFunThread = new INCA_COM_Fun_Thread();
            incaComFunThread->setFunInfo(fun, i);

            connect(incaComFunThread, &INCA_COM_Fun_Thread::funStateChanged, this, &FunctionWin::Slt_FunStateChanged);

            incaComFunThread->connectToINCA();

            INCACOMM *incaCom = incaComFunThread->getIncaCom();
        }
        else
        {
            funThreadHash.insert(i, funThread);

        }
    }

    //Globals::funMeasListHash = funMeasListHash;
    //Globals::funCharListHash = funCharListHash;

    //Globals::funPamListHash = funPamListHash;
    //Globals::funWritePamListHash = funWritePamListHash;
    //Globals::funMapPamListHash = funMapPamListHash;

    packGlobalPamHash();
    packGlobalMapPamHash();

    funRunning = true;

    emit workStateUpdated(workName, 1);

    emit runActive(true);

    if(pamFactoryWin)
    {
        pamFactoryWin->startPamUpdate();
    }

}

void FunctionWin::stopAllFun()
{
    if(pamFactoryWin)
    {
        pamFactoryWin->stopPamUpdate();
    }

    for(int i = 0; i < funList.count(); i++)
    {
        FunStruct *fun = funList.at(i);

        if(funThreadHash.keys().indexOf(i) == -1)
            continue;

        QThread *funThread = funThreadHash.value(i);
        if(!funThread)
            continue;

        if(fun->funType == "XCP")
        {
            Xcp_Fun_Thread *xcpFunThread = (Xcp_Fun_Thread*)funThread;
            xcpFunThread->quit();
            xcpFunThread->wait();

            delete xcpFunThread;
            xcpFunThread = NULL;
        }
        else if(fun->funType == "UDS")
        {
            /*
            Uds_Fun_Thread *udsFunThread = (Uds_Fun_Thread*)funThread;

            udsFunThread->setUdsRun(false);

            udsFunThread->waitForUdsActive(false, 3000);

            udsFunThread->setIsStop(true);

            udsFunThread->quit();
            udsFunThread->wait();

            delete udsFunThread;
            udsFunThread = NULL;

            Slt_FunStateChanged(0, i);
            */

        }
        else if(fun->funType == "CAN/CANFD")
        {
            Can_Fun_Thread *canFunThread = (Can_Fun_Thread*)funThread;

            canFunThread->quit();
            canFunThread->wait();

            delete canFunThread;
            canFunThread = NULL;
        }
        else if(fun->funType == "WT3000")
        {
            WT3000_Thread *wt3000FunThread = (WT3000_Thread*)funThread;

            wt3000FunThread->setStopFlg(true);

            wt3000FunThread->quit();
            wt3000FunThread->wait();

            delete wt3000FunThread;
            wt3000FunThread = NULL;
        }
        else if(fun->funType == "WT5000")
        {
            WT5000_Thread *wt5000FunThread = (WT5000_Thread*)funThread;

            wt5000FunThread->setStopFlg(true);

            wt5000FunThread->quit();
            wt5000FunThread->wait();

            delete wt5000FunThread;
            wt5000FunThread = NULL;
        }
        else if(fun->funType == "INCA_COM")
        {
            INCA_COM_Fun_Thread *incaComFunThread = (INCA_COM_Fun_Thread*)funThread;
            incaComFunThread->disconnectFromINCA();

            delete incaComFunThread;
            incaComFunThread = NULL;
        }
        else
        {

        }
    }

    funRunning = false;

    funThreadHash.clear();

    //funMeasListHash.clear();
    //funCharListHash.clear();

    //funPamListHash.clear();
    //funWritePamListHash.clear();
    //funMapPamListHash.clear();

    Globals::funMeasListHash.clear();
    Globals::funCharListHash.clear();
    Globals::funPamListHash.clear();
    Globals::funWritePamListHash.clear();
    Globals::funMapPamListHash.clear();

    Globals::mapPamHash.clear();

    Globals::pamHash.clear();

    emit workStateUpdated(workName, 0);
}

void FunctionWin::setAllFunDaqRun(bool runFlg)
{
    for(int i = 0; i < funList.count(); i++)
    {
        FunStruct *fun = funList.at(i);

        if(funThreadHash.keys().indexOf(i) == -1)
            continue;

        QThread *funThread = funThreadHash.value(i);
        if(!funThread)
            continue;

        if(fun->funType == "XCP")
        {
            Xcp_Fun_Thread *xcpFunThread = (Xcp_Fun_Thread*)funThread;

            xcpFunThread->setDaqRun(runFlg);
            xcpFunThread->setPollingRun(runFlg);
        }
    }
}

void FunctionWin::waitForXcpThreadStartFinished(Xcp_Fun_Thread *xcpFunThread, quint32 waitTime_ms)
{
    if(!xcpFunThread)
        return;
    int count = waitTime_ms / 500;
    int index = 0;
    while(!xcpFunThread->getXcpStartFinished() && xcpFunThread->getXcpState() != -1 && index <= count)
    {
        sleep(500);
        index++;
    }
    return;
}
/*
void FunctionWin::waitForUdsThreadStartFinished(Uds_Fun_Thread *udsFunThread, quint32 waitTime_ms)
{
    if(!udsFunThread)
        return;
    int count = waitTime_ms / 500;
    int index = 0;
    while(!udsFunThread->getUdsStartFinished() && index <= count)
    {
        sleep(500);
        index++;
    }
    return;
}
*/
void FunctionWin::waitForCanThreadStartFinished(Can_Fun_Thread *canFunThread, quint32 waitTime_ms)
{
    if(!canFunThread)
        return;
    int count = waitTime_ms / 500;
    int index = 0;
    while(!canFunThread->getCanStartFinished() && index <= count)
    {
        sleep(500);
        index++;
    }
    return;
}

void FunctionWin::waitForTsCanThreadStartFinished(Can_Fun_Thread_TS *canFunThread, quint32 waitTime_ms)
{
    if(!canFunThread)
        return;
    int count = waitTime_ms / 500;
    int index = 0;
    while(!canFunThread->getCanStartFinished() && index <= count)
    {
        sleep(500);
        index++;
    }
    return;
}

void FunctionWin::waitForZlgCanThreadStartFinished(Can_Fun_Thread_ZLG *canFunThread, quint32 waitTime_ms)
{
    if(!canFunThread)
        return;
    int count = waitTime_ms / 500;
    int index = 0;
    while(!canFunThread->getCanStartFinished() && index <= count)
    {
        sleep(500);
        index++;
    }
    return;
}

void FunctionWin::waitForRecordFinished(MDF_Record_Thread *mdfRecordThread, quint32 waitTime_ms)
{
    if(!mdfRecordThread)
        return;
    int count = waitTime_ms / 500;
    int index = 0;
    while(!mdfRecordThread->getRecordFinished() && index <= count)
    {
        sleep(500);
        index++;
    }
    return;
}

void FunctionWin::waitForWT3000ThreadStartFinished(WT3000_Thread *wt3000FunThread, quint32 waitTime_ms)
{
    if(!wt3000FunThread)
        return;
    int count = waitTime_ms / 500;
    int index = 0;
    while(!wt3000FunThread->getWtState() && index <= count)
    {
        sleep(500);
        index++;
    }
    return;
}

void FunctionWin::waitForWT5000ThreadStartFinished(WT5000_Thread *wt5000FunThread, quint32 waitTime_ms)
{
    if(!wt5000FunThread)
        return;
    int count = waitTime_ms / 500;
    int index = 0;
    while(!wt5000FunThread->getWtState() && index <= count)
    {
        sleep(500);
        index++;
    }
    return;
}

void FunctionWin::sleep(int msec)
{
    QTime dieTime = QTime::currentTime().addMSecs(msec);
    while( QTime::currentTime() < dieTime )
        QCoreApplication::processEvents(QEventLoop::AllEvents, 100);
}

void FunctionWin::showCharMapsInMapWin()
{
    if(mapWin == NULL)
        return;

    funMapCharListHash.clear();
    xcpFunMapSmHash.clear();

    for(int i = 0; i < funList.count(); i++)
    {
        FunStruct *fun = funList.at(i);

        if(funThreadHash.keys().indexOf(i) == -1)
            continue;

        QThread *funThread = funThreadHash.value(i);
        if(!funThread)
            continue;

        if(fun->funType == "XCP")
        {
            Xcp_Fun_Thread *xcpFunThread = (Xcp_Fun_Thread*)funThread;

            funMapCharListHash.insert(fun->funName, xcpFunThread->getCharMapPamList());
            xcpFunMapSmHash.insert(fun->funName, xcpFunThread->getSmMapWriteHash());

            bool xcpConnected = xcpFunThread->getXcpState() >= 2;
            if(xcpConnected) mapWin->setXcpConnected(xcpConnected);

        }

    }

    mapWin->setXcpFunMapListHash(funMapCharListHash);
    mapWin->setXcpFunMapSmHash(xcpFunMapSmHash);
    mapWin->show();
}

void FunctionWin::initAndActiveMdfRecord()
{
    if(!mdfRecordIns)
    {
        mdfRecordIns = new MDF_Record_Thread();
    }
    mdfRecordIns->setWorkName(workName);
    mdfRecordIns->setRecordFileName(this->mdfFileName);

    connect(this, &FunctionWin::recordActive, mdfRecordIns, &MdfRecord::setRecordStatus_v2);
    connect(mdfRecordIns, &MdfRecord::recordTime, this, &FunctionWin::Slt_ShowRecordTimeInTimeEdit);
    connect(mdfRecordIns, &MdfRecord::cycleNumUpdated, this, &FunctionWin::Slt_cycleNumUpdated);


    for(int i = 0; i < funList.count(); i++)
    {
        FunStruct *fun = funList.at(i);

        if(funThreadHash.keys().indexOf(i) == -1)
            continue;

        QThread *funThread = funThreadHash.value(i);
        if(!funThread)
            continue;

        if(fun->funType == "XCP")
        {
            Xcp_Fun_Thread *xcpFunThread = (Xcp_Fun_Thread*)funThread;

            XCPMaster *xcpMaster =xcpFunThread->getXcpMaster();
            if(xcpMaster)
            {
                QHash<QString, QList<A2L_VarMeas*>> dgNameVarsHash = xcpMaster->getDgNameVarHash();
                QStringList keys = dgNameVarsHash.uniqueKeys();
                foreach (QString dgName, keys) {
                    QList<A2L_VarMeas*> meas = dgNameVarsHash.value(dgName);
                    quint32 blockSize = 0;
                    QList<PARAM*> pams = fromMeasToPams(meas, blockSize);

                    mdfRecordIns->addDgPams(dgName, pams, blockSize);
                }

                connect(xcpMaster, &XCPMaster::ODTDataForRecord, mdfRecordIns, &MdfRecord::mdf_record_slot_v2);

            }
            XCP_Polling_Thread *xcpPollingThread = xcpFunThread->getXcpPollThread();
            if(xcpPollingThread)
            {
                connect(xcpPollingThread, &XCP_Polling_Thread::pollDataForRecord, mdfRecordIns, &MdfRecord::mdf_record_slot_v2);
            }
        }
        else if(fun->funType == "CAN/CANFD")
        {
            if(fun->devType == "NI-XNET")
            {
                Can_Fun_Thread *canFunThread = (Can_Fun_Thread*)funThread;

                QHash<QString, QList<PARAM*>> readPamListHash = canFunThread->getReadPamListHash();

                QStringList keys = readPamListHash.uniqueKeys();
                foreach (QString dgName, keys) {
                    QList<PARAM*> pamList = readPamListHash.value(dgName);
                    PARAM *lastPam = pamList.last();
                    quint32 blockSize = lastPam->smSize;
                    qDebug()<<"dgName:"<<dgName<<", blockSize:"<<blockSize;

                    mdfRecordIns->addDgPams(dgName, pamList, blockSize);
                }

                connect(canFunThread, &CAN_Fun_Thread::canDataForRecord, mdfRecordIns, &MdfRecord::mdf_record_slot_v2);

            }
            else if(fun->devType == "TS-CAN")
            {

            }
            else if(fun->devType == "ZLG-CAN")
            {

            }

        }
        else if(fun->funType == "WT3000")
        {
            WT3000_Thread *wt3000FunThread = (WT3000_Thread*)funThread;

            QString dgName = wt3000FunThread->getSmKey();
            QList<PARAM*> pamList = wt3000FunThread->getReadPamList();

            PARAM *lastPam = pamList.last();
            quint32 blockSize = lastPam->smSize;

            qDebug()<<"dgName:"<<dgName<<", blockSize:"<<blockSize;

            mdfRecordIns->addDgPams(dgName, pamList, blockSize);

            connect(wt3000FunThread, &WT3000_Fun_Thread::wtDataForRecord, mdfRecordIns, &MdfRecord::mdf_record_slot_v2);

        }
        else if(fun->funType == "WT5000")
        {
            WT5000_Thread *wt5000FunThread = (WT5000_Thread*)funThread;

            QString dgName = wt5000FunThread->getSmKey();
            QList<PARAM*> pamList = wt5000FunThread->getReadPamList();

            PARAM *lastPam = pamList.last();
            quint32 blockSize = lastPam->smSize;

            qDebug()<<"dgName:"<<dgName<<", blockSize:"<<blockSize;

            mdfRecordIns->addDgPams(dgName, pamList, blockSize);

            connect(wt5000FunThread, &WT5000_Fun_Thread::wtDataForRecord, mdfRecordIns, &MdfRecord::mdf_record_slot_v2);

        }
    }

    if(!recordThread)
    {
        recordThread = new QThread();
        mdfRecordIns->moveToThread(recordThread);
        connect(recordThread, &QThread::finished, mdfRecordIns, &QObject::deleteLater);

    }
    if(!recordThread->isRunning())
    {
        recordThread->start();
    }

    qDebug()<<"== mdf record init in main thread ==";
}

void FunctionWin::endMdfRecord()
{
    for(int i = 0; i < funList.count(); i++)
    {
        FunStruct *fun = funList.at(i);

        if(funThreadHash.keys().indexOf(i) == -1)
            continue;

        QThread *funThread = funThreadHash.value(i);
        if(!funThread)
            continue;

        if(fun->funType == "XCP")
        {
            Xcp_Fun_Thread *xcpFunThread = (Xcp_Fun_Thread*)funThread;

            XCPMaster *xcpMaster =xcpFunThread->getXcpMaster();
            if(xcpMaster)
            {
                disconnect(xcpMaster, &XCPMaster::ODTDataForRecord, mdfRecordIns, &MdfRecord::mdf_record_slot_v2);

            }

            XCP_Polling_Thread *xcpPollingThread = xcpFunThread->getXcpPollThread();
            if(xcpPollingThread)
            {
                disconnect(xcpPollingThread, &XCP_Polling_Thread::pollDataForRecord, mdfRecordIns, &MdfRecord::mdf_record_slot_v2);
            }
        }
        else if(fun->funType == "CAN/CANFD")
        {
            Can_Fun_Thread *canFunThread = (Can_Fun_Thread*)funThread;
            disconnect(canFunThread, &CAN_Fun_Thread::canDataForRecord, mdfRecordIns, &MdfRecord::mdf_record_slot_v2);
        }
        else if(fun->funType == "WT3000")
        {
            WT3000_Thread *wt3000FunThread = (WT3000_Thread*)funThread;
            disconnect(wt3000FunThread, &WT3000_Fun_Thread::wtDataForRecord, mdfRecordIns, &MdfRecord::mdf_record_slot_v2);
        }
    }

    waitForRecordFinished(mdfRecordIns, 30000);

    qDebug()<<"begin to del record thread";
    if(recordThread)
    {
        recordThread->quit();
        recordThread->wait();

        delete recordThread;
        recordThread = NULL;

        mdfRecordIns = NULL;
    }
    qDebug()<<"del record thread end";

    qDebug()<<"==mdf record end in main thread.==";

}

QList<PARAM *> FunctionWin::fromMeasToPams(QList<A2L_VarMeas *> measPamList, quint32 &blockSize)
{
    QList<PARAM*> pamList;

    if(measPamList.isEmpty())
        return pamList;

    quint32 startBitIndex = 0;
    quint32 startByteIndex = 0;

    PARAM *timePam = new PARAM();
    timePam->Unit = "s";
    timePam->DefaultValue = 0;
    timePam->LowerLimit = 0;
    timePam->UpperLimit = __DBL_MAX__;
    timePam->ScalingFactor = 1;
    timePam->ScalingOffset = 0;
    timePam->LengthBit = 64;
    timePam->StartBit = 0;
    timePam->ByteOrder = 0;
    timePam->Type = 3;
    timePam->Name = "Time";

    pamList.append(timePam);

    startBitIndex = 64;
    blockSize += 8;

    for(int i=0; i < measPamList.count(); i++)
    {
        A2L_VarMeas *measVar = measPamList.at(i);
        PARAM *varPam = new PARAM;

        QString dataType = measVar->DataType;
        int type = 0;
        if(dataType == "UBYTE" || dataType == "UWORD" || dataType == "ULONG" || dataType == "A_UINT64")
            type = 0;
        else if(dataType == "SBYTE" || dataType == "SWORD" || dataType == "SLONG" || dataType == "A_INT64")
            type = 1;
        else if(dataType == "FLOAT32_IEEE")
            type = 2;
        else if(dataType == "FLOAT64_IEEE")
            type = 3;

        varPam->Unit = measVar->Unit;
        varPam->DefaultValue = 0;
        varPam->LowerLimit = measVar->LowerLimit;
        varPam->UpperLimit = measVar->UpperLimit;
        varPam->ScalingFactor = measVar->ScalingFactor;
        varPam->ScalingOffset = measVar->ScalingOffset;
        varPam->LengthBit = measVar->DataSizeAG*8;
        varPam->StartBit = startBitIndex;
        varPam->ByteOrder = 0;
        varPam->Type = type;
        varPam->Name = measVar->Name;

        startBitIndex += (measVar->DataSizeAG * 8);
        blockSize += measVar->DataSizeAG;

        pamList.append(varPam);
    }

    return pamList;
}

void FunctionWin::initSysPams()
{
    if(!autoPam)
    {
        autoPam = new AUTO_PAM(this);
    }

    if(!sysPam)
    {
        sysPam = new SYS_PAM(this);
    }
}

QString FunctionWin::getWorkWinFilePath() const
{
    return workWinFilePath;
}

void FunctionWin::setWorkWinFilePath(const QString &value)
{
    workWinFilePath = value;
}

void FunctionWin::setWinConfigPath(const QString &value)
{
    winConfigPath = value;
}

QString FunctionWin::getWinConfigPath() const
{
    return winConfigPath;
}

void FunctionWin::setWorkName(const QString &value)
{
    workName = value;

    setWindowTitle(workName);

    for(int i = 0; i < funList.count(); i++)
    {
        FunStruct *fun = funList.at(i);
        if(!fun)
            continue;

        QString fileName = workName.toLower() + "-" + fun->funName.toLower() + "-setting.ini";
        if(fun->funType == "CAN/CANFD")
        {
            fileName.replace(".ini", ".xml");
        }

        QString filePath_ref = "/work/" + workName + "/" + fileName;
        QString filePath = QApplication::applicationDirPath() + "/work/" + workName + "/" + fileName;

        QString filePath_last = QApplication::applicationDirPath() + "/work/" + workName + "/" +
                                fun->setFilePath.split("/").last();

        if(fun->setFilePath == filePath_ref)
            continue;

        QFileInfo fileInfo(filePath_last);
        if(fileInfo.isFile())
        {
            QFile file(filePath_last);
            file.rename(filePath);


            fun->setFilePath = filePath_ref;

            qDebug()<<"=== Rename fun set file as:"<<fun->setFilePath;
        }
    }

    if(scriptWin)
    {
        scriptWin->setWorkName(workName);
    }

    emit funUpdated();
}


void FunctionWin::setA2lProjectWin(A2LProjectWin *value)
{
    a2lProjectWin = value;
}

void FunctionWin::Slt_TableWinDeleted(QWidget *win)
{
    TableWin *table = (TableWin*)win;
    int index = tableWinList.indexOf(table);
    if(index == -1)
        return;

    QString winName = table->getWinName();

    winHash.remove(winName);
    tableWinList.removeOne(table);

    table->setAttribute(Qt::WA_DeleteOnClose);
    table->close();

    QAction *action = actionHash.value(winName);
    ui->menuTable->removeAction(action);
    actionHash.remove(winName);
    delete action;

    writeWinXml();

}

void FunctionWin::Slt_CalTableWinDeleted(QWidget *win)
{
    TableWin *table = (TableWin*)win;
    int index = calTableWinList.indexOf(table);
    if(index == -1)
        return;

    QString winName = table->getWinName();

    winHash.remove(winName);
    calTableWinList.removeOne(table);

    table->setAttribute(Qt::WA_DeleteOnClose);
    table->close();

    QAction *action = actionHash.value(winName);
    ui->menuTableCal->removeAction(action);
    actionHash.remove(winName);
    delete action;

    writeWinXml();
}

void FunctionWin::Slt_PlotWinDeleted(QWidget *win)
{
    PlotWin *plot = (PlotWin*)win;
    int index = plotWinList.indexOf(plot);
    if(index == -1)
        return;

    QString winName = plot->getWinName();

    winHash.remove(winName);
    plotWinList.removeOne(plot);

    plot->setAttribute(Qt::WA_DeleteOnClose);
    plot->close();

    QAction *action = actionHash.value(winName);
    ui->menuTable->removeAction(action);
    actionHash.remove(winName);
    delete action;

    writeWinXml();
}

void FunctionWin::Slt_WinUpdated()
{
    writeWinXml();
}

void FunctionWin::setFunList(QList<FunStruct*> value)
{
    funList = value;
    showFunctions();
}

QList<FunStruct*> FunctionWin::getFunList() const
{
    return funList;
}

void FunctionWin::on_actionDevices_triggered()
{
    showNIXnetIntfs(getNIXnetIntfs());

    showTSCanIntfs(getTSCanIntfs());
}

void FunctionWin::on_actionFunRun_triggered()
{
    stopAllFun();

    runAllFun();

    ui->actionFunRun->setEnabled(false);
    ui->actionFunStop->setEnabled(true);
    ui->actionPamFactory->setEnabled(true);
}

void FunctionWin::on_actionFunStop_triggered()
{
    stopAllFun();

    ui->actionFunRun->setEnabled(true);
    ui->actionFunStop->setEnabled(false);
    ui->actionPamFactory->setEnabled(false);

    if(dockWin)
    {
        delete dockWin;
        dockWin = NULL;
    }
}

void FunctionWin::on_actionRun_triggered()
{
    if(ui->actionRun->isChecked())
    {
        setAllFunDaqRun(false);
    }
    else
    {
        setAllFunDaqRun(true);
    }

}

void FunctionWin::on_actionStop_triggered()
{
    setAllFunDaqRun(false);
}


void FunctionWin::on_actionAdd_Table_triggered()
{
    QString winName = genDefaultTableWinName();
    if(winName.isEmpty())
        return;

    TableWin *table = addReadTable(winName);

    table->show();

    writeWinXml();
}

void FunctionWin::on_actionAdd_Cal_Table_triggered()
{
    QString winName = genDefaultCalTableWinName();
    if(winName.isEmpty())
        return;

    TableWin *table = addWriteTable(winName);

    table->show();

    writeWinXml();
}

void FunctionWin::on_actionAdd_Chart_triggered()
{
    QString winName = genDefaultChartWinName();
    if(winName.isEmpty())
        return;

    PlotWin *plot = addPlotWin(winName);

    plot->show();

    writeWinXml();
}

void FunctionWin::on_actionRcdOn_triggered()
{
    if(!funRunning)
        return;

    if(!ui->actionRcdOn->isChecked())
    {
        if(mdfRecordIns)
        {
            if(mdfRecordIns->getRecord_on())
            {
                emit recordActive(false);
            }
        }

        endMdfRecord();

        ui->led_Record->setText("visulize but not record");
        ui->led_Record->setProperty("ledState", "idle");
        ui->led_Record->style()->unpolish(ui->led_Record);
        ui->led_Record->style()->polish(ui->led_Record);

    }
    else
    {
        loadRecordSetting();

        initAndActiveMdfRecord();

        emit recordActive(true);

        QString preStr = rcdSet.cyclicRecord ? "Cycle-" : "Single-";
        ui->led_Record->setText(preStr + "Recording...");
        ui->led_Record->setProperty("ledState", "recording");
        ui->led_Record->style()->unpolish(ui->led_Record);
        ui->led_Record->style()->polish(ui->led_Record);

    }

}

void FunctionWin::on_actionRcdOff_triggered()
{
    emit recordActive(false);

    endMdfRecord();

    ui->actionRcdOn->setEnabled(true);
    ui->actionRcdOff->setEnabled(true);

    ui->led_Record->setText("visulize but not record");
    ui->led_Record->setProperty("ledState", "idle");
    ui->led_Record->style()->unpolish(ui->led_Record);
    ui->led_Record->style()->polish(ui->led_Record);

}

void FunctionWin::on_actionSaveWin_triggered()
{
    writeWinXml();
}

void FunctionWin::on_actionshowAllWin_triggered()
{

    QStringList winKeys = winHash.uniqueKeys();
    for(int i = 0; i < winKeys.count(); i++)
    {
        QString winName = winKeys.at(i);
        QWidget *win = winHash.value(winName);

        if(win)
        {
            winShow ? win->hide() : win->show();
        }
    }
    winShow = !winShow;
}


void FunctionWin::on_actionMapWin_triggered()
{
    if(mapWin == NULL)
    {
        mapWin = new MapWin(this);
    }

    showCharMapsInMapWin();

    mapWin->show();
}

void FunctionWin::on_actionScript_triggered()
{
    if(scriptWin == NULL)
    {
        scriptWin = new ScriptWin(this, workName);
    }
    scriptWin->setWorkName(workName);
    scriptWin->show();
}

void FunctionWin::on_actionPamFactory_triggered()
{
    if(pamFactoryWin == NULL)
    {
        pamFactoryWin = new PamFactoryWin(this);
        connect(this, &FunctionWin::runActive, pamFactoryWin, &PamFactoryWin::updatePamView);
    }

    pamFactoryWin->updatePamView(true);

    pamFactoryWin->show();
}

void FunctionWin::on_actionRcdSet_triggered()
{
    RcdSetDlg *rcdSetDlg = new RcdSetDlg(this, workName);
    rcdSetDlg->show();
}

void FunctionWin::on_actionImportPams_triggered()
{

}

void FunctionWin::on_actionDockView_triggered()
{
    if(!dockWin)
    {
        dockWin = new DockWindow(this, workName);
    }

    dockWin->setWorkName(this->workName);
    dockWin->restoreAll();
    dockWin->show();
}

void FunctionWin::on_actionAuto_triggered()
{

    if(autoExeWin == NULL)
    {
        autoExeWin = new AutoExeWin(this, workName);
    }

    autoExeWin->setSysPam(sysPam);
    autoExeWin->setAutoPam(autoPam);

    autoExeWin->show();
}

void FunctionWin::initFunMenuStyle()
{
    // 菜单栏样式已移至全局样式表 res/style.qss
}

void FunctionWin::initFunTreeStyle()
{
    // 树视图样式已移至全局样式表 res/style.qss
}
