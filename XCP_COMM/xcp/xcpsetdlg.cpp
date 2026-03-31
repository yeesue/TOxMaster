#include "xcpsetdlg.h"
#include "ui_xcpsetdlg.h"
#include <QMenu>
#include <QFileDialog>
#include "exceloperator.h"
#include <QDir>
#include <QTextStream>
#include <QProgressDialog>
#include <QDesktopWidget>
#include <qtconcurrentrun.h>
#include <QFutureWatcher>
#include <QtConcurrentMap>

XcpSetDlg::XcpSetDlg(QWidget *parent, A2LProjectWin *a2lProjectWin, QString work_name, QString fun_name, QString intf_name, QString set_file_path) :
    QDialog(parent), a2lWin(a2lProjectWin),
    workName(work_name), funName(fun_name),
    intfName(intf_name), xcpSetFilePath(set_file_path),
    ui(new Ui::XcpSetDlg)
{
    ui->setupUi(this);
    setWindowTitle("xcp setting(" + workName.toLower() + "/" + funName.toLower() + ")");

    //showMaximized();
    ui->tabWidget->setCurrentIndex(0);

    QDesktopWidget *deskTop =  QApplication::desktop();
    resize(deskTop->width()/2, deskTop->height()/4*3);

    QStringList header_r, header_w;
    header_r << QString::fromLocal8Bit("Pam Read") << QString::fromLocal8Bit("Type") << QString::fromLocal8Bit("Init Value");
    ui->tableWidget_Read->setHorizontalHeaderLabels(header_r);
    ui->tableWidget_Read->setColumnWidth(0, 300);
    ui->tableWidget_Read->setColumnWidth(1, 120);
    ui->tableWidget_Read->setColumnWidth(2, 100);
    ui->tableWidget_Read->setContextMenuPolicy(Qt::CustomContextMenu);


    header_w << QString::fromLocal8Bit("Pam Write") << QString::fromLocal8Bit("Init Value") << QString::fromLocal8Bit("Type");
    ui->tableWidget_Write->setHorizontalHeaderLabels(header_w);
    ui->tableWidget_Write->setColumnWidth(0, 300);
    ui->tableWidget_Write->setColumnWidth(1, 120);
    ui->tableWidget_Write->setColumnWidth(2, 100);
    ui->tableWidget_Write->setContextMenuPolicy(Qt::CustomContextMenu);

    ui->tableWidget_Read_DAQ->setHorizontalHeaderLabels(header_r);
    ui->tableWidget_Read_DAQ->setColumnWidth(0, 300);
    ui->tableWidget_Read_DAQ->setColumnWidth(1, 120);
    ui->tableWidget_Read_DAQ->setColumnWidth(2, 100);
    ui->tableWidget_Read_DAQ->setContextMenuPolicy(Qt::CustomContextMenu);
    //ui->tableWidget_Read_DAQ->setSelectionMode(QAbstractItemView::MultiSelection);

    ui->tableWidget_calInfoList->setColumnCount(7);
    QStringList header;
    header << "Cal Name"<<"Type"<<"Unit"<<"xAxis"<<"yAxis"<<"zValue"<<"Check";
    ui->tableWidget_calInfoList->setHorizontalHeaderLabels(header);

    if(a2lWin)
    {
        connect(a2lWin, &A2LProjectWin::winClosed, this, &XcpSetDlg::Slt_ProjectWinClosed);
        Slt_ProjectWinClosed();
    }

    xcpSetFilePath = QApplication::applicationDirPath() + xcpSetFilePath;
    QString fileName = workName.toLower() + "-" + funName.toLower() + "-setting.ini";
    QString filePath = QApplication::applicationDirPath() + "/work/" + workName + "/" + fileName;

    if(set_file_path.isEmpty())
    {
        xcpSetFileName = fileName;
        xcpSetFilePath = filePath;

        QFileInfo fileInfo(xcpSetFilePath);
        if(!fileInfo.isFile())
        {
            QFile file(xcpSetFilePath);
            qInfo()<<"xcp fun setting file new created:"<<fileName;
        }
    }
    else
    {
        QFileInfo fileInfo(xcpSetFilePath);
        if(xcpSetFilePath != filePath && fileInfo.isFile())
        {
            QFile file(xcpSetFilePath);
            bool ok = file.rename(filePath);
            if(ok)
            {
                qDebug()<<"xcp fun setting file rename as :"<<fileName;
                xcpSetFileName = fileName;
                xcpSetFilePath = filePath;
            }
        }
    }


    connect(ui->cb_project, QOverload<const QString&>::of(&QComboBox::currentIndexChanged), this, &XcpSetDlg::Slt_CurProjectChanged);

    readSetting();

    ui->cb_useA2lSetFile->setChecked(xcpConfig.useA2lSetFile);
    ui->cb_usePamsInExcel->setChecked(xcpConfig.usePamsInExcel);

    ui->cb_useSeedKey->setChecked(xcpConfig.useSeedKey);
    ui->le_seedKeyDllPath->setText(xcpConfig.seedKeyDllPath);

    loadA2lProject();

    if(!xcpConfig.a2lProjectName.isEmpty())
    {
        ui->cb_project->setCurrentText(xcpConfig.a2lProjectName); // active slot curProjectChanged(QString)
    }

    if(!curA2LProj)
    {
        showXcpSetFromConfig();
    }

    unpackPamInfoToTable();

    connect(ui->tableWidget_Read, &QTableWidget::customContextMenuRequested, this, &XcpSetDlg::Slt_ShowMenuInRead);
    connect(ui->tableWidget_Write, &QTableWidget::customContextMenuRequested, this, &XcpSetDlg::Slt_ShowMenuInWrite);
    connect(ui->tableWidget_Read_DAQ, &QTableWidget::customContextMenuRequested, this, &XcpSetDlg::Slt_ShowMenuInReadDAQ);
}

XcpSetDlg::~XcpSetDlg()
{
    delete ui;
}

void XcpSetDlg::accept()
{
    writeSetting();
    QDialog::accept();
}

void XcpSetDlg::Slt_ProjectWinClosed()
{
    ui->cb_project->clear();
    ui->cb_project->addItem("");
    ui->cb_project->addItems(a2lWin->getProjectNameList());

    ui->cb_project->setCurrentText("");
}

void XcpSetDlg::Slt_CurProjectChanged(QString projectName)
{
    if(projectName.isEmpty())
    {
        curA2LProj = NULL;

        ui->le_DaqType->clear();
        ui->cb_XcpOnCan->clear();
        ui->cb_Event->clear();
        ui->cb_EventTime->clear();
        ui->cb_EventCanIdList->clear();
        ui->cb_DaqList->clear();
        ui->cb_DaqListCanId->clear();
        ui->sb_CMD_ID->setValue(0);
        ui->sb_RES_ID->setValue(0);
        ui->sb_DAQMBL->setValue(0);
        ui->sb_DAQMEM->setValue(0);

        return;
    }

    A2L_Project *a2lProject =  a2lWin->getProjectByName(projectName);

    curA2LProj = a2lProject;
    qDebug()<<"Current A2L project:"<<curA2LProj->getA2LFileName();

    showXcpSetFromA2lProj(curA2LProj);

    // save config to struct
    xcpConfig.workName = this->workName;
    xcpConfig.intfName = this->intfName;
    xcpConfig.a2lProjectName = ui->cb_project->currentText();
    xcpConfig.a2lFilePath = curA2LProj->getA2LFileName();
    xcpConfig.hexFilePath = curA2LProj->getA2LRefHexFileName();

    xcpConfig.index_XcpOnCan = ui->cb_XcpOnCan->currentIndex();
    xcpConfig.baudrate = ui->cb_Baudrate->currentText().toUInt();
    xcpConfig.id_CMD = ui->sb_CMD_ID->value();
    xcpConfig.id_RES = ui->sb_RES_ID->value();

    xcpConfig.fdBaudrate = ui->cb_FdBaudrate->currentText().toUInt();
    xcpConfig.maxDLC = ui->sb_maxDLC->value();
    xcpConfig.isCanFD = ui->cb_isCANFD->isChecked();

}

void XcpSetDlg::Slt_memSegIndexChanged(int index)
{
    if(!curA2LProj) return;

    QList<A2L_MemorySegment*> a2lMemSegList = curA2LProj->getA2lMemSegList();
    A2L_MemorySegment *memSeg = a2lMemSegList.at(index);
    if(!memSeg) return;
    curMemorySegment = memSeg;

    ui->le_MemType->setText(memSeg->memType);
    ui->le_MemAttr->setText(memSeg->memAttr);
    ui->le_PrgType->setText(memSeg->prgType);
    ui->le_MemStartAddress->setText(QString::number(memSeg->memAddress, 16).toUpper());
    ui->sb_MemSize->setValue(memSeg->memSize);

    ui->sb_SegmentIndex->setValue(memSeg->segIndex);
    ui->sb_PageNumber->setValue(memSeg->pageNum);

    ui->le_ChecksumType->setText(memSeg->checksumType);
    ui->sb_MaxBlockSize->setValue(memSeg->maxBlockSize);

    disconnect(ui->cb_MemPageList, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &XcpSetDlg::Slt_memPageIndexChanged);
    ui->cb_MemPageList->clear();
    QStringList pageIndexList;
    for(int i = 0; i < memSeg->pageNum; i++)
    {
        pageIndexList.append(QString::number(i));
    }
    ui->cb_MemPageList->addItems(pageIndexList);

    Slt_memPageIndexChanged(0);
    connect(ui->cb_MemPageList, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &XcpSetDlg::Slt_memPageIndexChanged);

}

void XcpSetDlg::Slt_memPageIndexChanged(int index)
{
    if(!curA2LProj) return;

    A2L_MemorySegment *memSeg = curMemorySegment;
    if(!memSeg) return;

    QList<A2L_Page*> pageList = memSeg->a2lPageList;
    A2L_Page *page = pageList.at(index);
    if(!page) return;

    //qDebug()<<"page index:"<<index;
    ui->sb_PageIndex->setValue(page->pageIndex);
    ui->le_EcuAccessType->setText(page->ecuAccessType);
    ui->le_XcpReadAccessType->setText(page->xcpReadAccessType);
    ui->le_XcpWriteAccessType->setText(page->xcpWriteAccessType);

}

void XcpSetDlg::Slt_ShowMenuInRead(QPoint pos)
{
    QMenu *menu = NULL;
    if (menu)
    {
        delete menu;
        menu = NULL;
    }

    menu = new QMenu(ui->tableWidget_Read);

    QAction *addAction = menu->addAction("Add");
    QAction *insertAction = menu->addAction("Insert");
    QAction *removeAction = menu->addAction("remove");

    connect(addAction, &QAction::triggered, this, &XcpSetDlg::Slt_AddRead);
    connect(insertAction, &QAction::triggered, this, &XcpSetDlg::Slt_InsertRead);
    connect(removeAction, &QAction::triggered, this, &XcpSetDlg::Slt_RemoveRead);

    menu->move(cursor().pos());
    menu->show();
}

void XcpSetDlg::Slt_ShowMenuInWrite(QPoint pos)
{
    QMenu *menu = NULL;
    if (menu)
    {
        delete menu;
        menu = NULL;
    }

    menu = new QMenu(ui->tableWidget_Write);

    QAction *addAction = menu->addAction("Add");
    QAction *insertAction = menu->addAction("Insert");
    QAction *removeAction = menu->addAction("remove");

    connect(addAction, &QAction::triggered, this, &XcpSetDlg::Slt_AddWrite);
    connect(insertAction, &QAction::triggered, this, &XcpSetDlg::Slt_InsertWrite);
    connect(removeAction, &QAction::triggered, this, &XcpSetDlg::Slt_RemoveWrite);

    menu->move(cursor().pos());
    menu->show();
}

void XcpSetDlg::Slt_ShowMenuInReadDAQ(QPoint pos)
{
    QMenu *menu = NULL;
    if (menu)
    {
        delete menu;
        menu = NULL;
    }

    menu = new QMenu(ui->tableWidget_Read_DAQ);

    QAction *addAction = menu->addAction("Add");
    QAction *insertAction = menu->addAction("Insert");
    QAction *removeAction = menu->addAction("remove");

    connect(addAction, &QAction::triggered, this, &XcpSetDlg::Slt_AddReadDAQ);
    connect(insertAction, &QAction::triggered, this, &XcpSetDlg::Slt_InsertReadDAQ);
    connect(removeAction, &QAction::triggered, this, &XcpSetDlg::Slt_RemoveReadDAQ);

    menu->move(cursor().pos());
    menu->show();
}

void XcpSetDlg::Slt_AddRead(bool)
{
    if(curA2LProj == NULL)
        return;

    PAMSELDlg *pamselDlg = new PAMSELDlg(this, curA2LProj);
    connect(pamselDlg, &PAMSELDlg::measPamSelAccepted, this, &XcpSetDlg::Slt_MeasPamSelAccepted);
    pamselDlg->show();
}

void XcpSetDlg::Slt_InsertRead(bool)
{
    int curRow = ui->tableWidget_Read->currentRow();
    ui->tableWidget_Read->insertRow(curRow);

    QComboBox *cb_type = new QComboBox();
    cb_type->addItems(pollTypeList);

    ui->tableWidget_Read->setItem(curRow, 0, new QTableWidgetItem("Param_R_"+QString::number(curRow)));
    ui->tableWidget_Read->setCellWidget(curRow, 1, cb_type);
    ui->tableWidget_Read->setItem(curRow, 2, new QTableWidgetItem(QString::number(0)));
}

void XcpSetDlg::Slt_RemoveRead(bool)
{
    //int curRow = ui->tableWidget_Read->currentRow();
    //ui->tableWidget_Read->removeRow(curRow);

    QList<QTableWidgetItem*> selItems = ui->tableWidget_Read->selectedItems();
    QList<int> selRows;
    foreach (QTableWidgetItem *item, selItems) {
        int row = item->row();
        if(selRows.indexOf(row) == -1)
        {
            selRows.append(row);
        }
    }

    std::sort(selRows.begin(), selRows.end(), std::greater<int>());

    foreach (int row, selRows) {
        ui->tableWidget_Read->removeRow(row);
    }
}

void XcpSetDlg::Slt_AddWrite(bool)
{
    if(curA2LProj == NULL)
        return;
    PAMSELDlg *pamselDlg = new PAMSELDlg(this, curA2LProj);
    connect(pamselDlg, &PAMSELDlg::charPamSelAccepted, this, &XcpSetDlg::Slt_CharPamSelAccepted);
    pamselDlg->show();
}

void XcpSetDlg::Slt_InsertWrite(bool)
{
    int curRow = ui->tableWidget_Write->currentRow();
    ui->tableWidget_Write->insertRow(curRow);

    ui->tableWidget_Write->setItem(curRow, 0, new QTableWidgetItem("Param_W_"+QString::number(curRow)));
    ui->tableWidget_Write->setItem(curRow, 1, new QTableWidgetItem(QString::number(0)));
}

void XcpSetDlg::Slt_RemoveWrite(bool)
{
    //int curRow = ui->tableWidget_Write->currentRow();
    //ui->tableWidget_Write->removeRow(curRow);

    QList<QTableWidgetItem*> selItems = ui->tableWidget_Write->selectedItems();
    QList<int> selRows;
    foreach (QTableWidgetItem *item, selItems) {
        int row = item->row();
        if(selRows.indexOf(row) == -1)
        {
            selRows.append(row);
        }
    }

    std::sort(selRows.begin(), selRows.end(), std::greater<int>());

    foreach (int row, selRows) {
        ui->tableWidget_Write->removeRow(row);
    }
}

void XcpSetDlg::Slt_AddReadDAQ(bool)
{
    if(curA2LProj == NULL)
        return;

    PAMSELDlg *pamselDlg = new PAMSELDlg(this, curA2LProj);
    connect(pamselDlg, &PAMSELDlg::measPamSelAccepted, this, &XcpSetDlg::Slt_MeasPamSelAcceptedDAQ);
    pamselDlg->show();
}

void XcpSetDlg::Slt_InsertReadDAQ(bool)
{

}

void XcpSetDlg::Slt_RemoveReadDAQ(bool)
{
    //int curRow = ui->tableWidget_Read_DAQ->currentRow();
    //ui->tableWidget_Read_DAQ->removeRow(curRow);

    QList<QTableWidgetItem*> selItems = ui->tableWidget_Read_DAQ->selectedItems();
    QList<int> selRows;
    foreach (QTableWidgetItem *item, selItems) {
        int row = item->row();
        if(selRows.indexOf(row) == -1)
        {
            selRows.append(row);
        }
    }

    std::sort(selRows.begin(), selRows.end(), std::greater<int>());

    foreach (int row, selRows) {
        ui->tableWidget_Read_DAQ->removeRow(row);
    }
}

void XcpSetDlg::Slt_MeasPamSelAccepted(QStringList measPamNames)
{
    if(measPamNames.isEmpty())
        return;
    for(int i = 0; i < measPamNames.count(); i++)
    {
        int curRow = ui->tableWidget_Read->rowCount();
        ui->tableWidget_Read->insertRow(curRow);

        QComboBox *cb_type = new QComboBox();
        cb_type->addItems(pollTypeList);

        QTableWidgetItem *item0 = new QTableWidgetItem(measPamNames.at(i));
        QTableWidgetItem *item2 = new QTableWidgetItem(QString::number(0));
        item2->setTextAlignment(Qt::AlignCenter);

        ui->tableWidget_Read->setItem(curRow, 0, item0);
        ui->tableWidget_Read->setCellWidget(curRow, 1, cb_type);
        ui->tableWidget_Read->setItem(curRow, 2, item2);
    }
    ui->tableWidget_Read->resizeRowsToContents();
    ui->tableWidget_Read->resizeColumnToContents(0);
}

void XcpSetDlg::Slt_CharPamSelAccepted(QStringList charPamNames)
{
    if(charPamNames.isEmpty())
        return;
    for(int i = 0; i < charPamNames.count(); i++)
    {
        A2L_VarChar *charVar = curA2LProj->getCharVarByName(charPamNames.at(i));

        if(!charVar)
            continue;

        int curRow = ui->tableWidget_Write->rowCount();
        ui->tableWidget_Write->insertRow(curRow);

        QTableWidgetItem *item0 = new QTableWidgetItem(charPamNames.at(i));
        QTableWidgetItem *item1 = new QTableWidgetItem(QString::number(charVar->getValue(), 'g', 6));
        QTableWidgetItem *item2 = new QTableWidgetItem(charVar->charType);
        item1->setTextAlignment(Qt::AlignCenter);
        item2->setTextAlignment(Qt::AlignCenter);

        ui->tableWidget_Write->setItem(curRow, 0, item0);
        ui->tableWidget_Write->setItem(curRow, 1, item1);
        ui->tableWidget_Write->setItem(curRow, 2, item2);
    }
    ui->tableWidget_Write->resizeRowsToContents();
    ui->tableWidget_Write->resizeColumnToContents(0);
}

void XcpSetDlg::Slt_MeasPamSelAcceptedDAQ(QStringList measPamNames)
{
    if(measPamNames.isEmpty())
        return;
    QStringList eventTimeList = curA2LProj->getEventTimeList();
    for(int i = 0; i < measPamNames.count(); i++)
    {
        int curRow = ui->tableWidget_Read_DAQ->rowCount();
        ui->tableWidget_Read_DAQ->insertRow(curRow);

        QComboBox *cb_type = new QComboBox();

        cb_type->addItems(eventTimeList);
        cb_type->setCurrentIndex(eventTimeList.count() - 1);

        QTableWidgetItem *item0 = new QTableWidgetItem(measPamNames.at(i));
        QTableWidgetItem *item2 = new QTableWidgetItem(QString::number(0));
        item2->setTextAlignment(Qt::AlignCenter);

        ui->tableWidget_Read_DAQ->setItem(curRow, 0, item0);
        ui->tableWidget_Read_DAQ->setCellWidget(curRow, 1, cb_type);
        ui->tableWidget_Read_DAQ->setItem(curRow, 2, item2);
    }

    ui->tableWidget_Read_DAQ->resizeRowsToContents();
    ui->tableWidget_Read_DAQ->resizeColumnToContents(0);
}

void XcpSetDlg::on_pb_projectManage_clicked()
{
    if(a2lWin == NULL)
    {
        //a2lWin = new A2LProjectWin(this);
        //connect(a2lWin, SIGNAL(winClosed()), this, SLOT(projectWinClosedSlot()));
        return;
    }

    a2lWin->show();
}

void XcpSetDlg::loadA2lProject()
{
    if(!a2lWin)
        return;
    //if(xcpConfig.a2lFilePath.isEmpty() || xcpConfig.hexFilePath.isEmpty())
    if(xcpConfig.a2lFilePath.isEmpty()) //修改为无hex也能加载
        return;

    a2lWin->addA2LProject(xcpConfig.a2lFilePath, xcpConfig.hexFilePath);

    ui->cb_project->clear();
    ui->cb_project->addItem("");
    ui->cb_project->addItems(a2lWin->getProjectNameList());

}

void XcpSetDlg::packPamInfoToConfig()
{
    QList<XcpPam> pamList;
    int rowCount_R = ui->tableWidget_Read->rowCount();
    for (int i = 0; i < rowCount_R; i++)
    {
        XcpPam pam;
        pam.pamName = ui->tableWidget_Read->item(i,0)->text();
        pam.pollType = ((QComboBox*)ui->tableWidget_Read->cellWidget(i,1))->currentText();
        pam.pamDefaultValue = ui->tableWidget_Read->item(i, 2)->text().toDouble();

        pamList.append(pam);
    }
    xcpConfig.pollPamList = pamList;

    pamList.clear();
    int rowCount_W = ui->tableWidget_Write->rowCount();
    for (int i = 0; i < rowCount_W; i++)
    {
        XcpPam pam;
        pam.pamName = ui->tableWidget_Write->item(i,0)->text();
        pam.pamDefaultValue = ui->tableWidget_Write->item(i, 1)->text().toDouble();
        pam.caliType = ui->tableWidget_Write->item(i, 2)->text();
        pamList.append(pam);
    }
    xcpConfig.calPamList = pamList;

    pamList.clear();
    int rowCount_R_DAQ = ui->tableWidget_Read_DAQ->rowCount();
    for (int i = 0; i < rowCount_R_DAQ; i++)
    {
        XcpPam pam;
        pam.pamName = ui->tableWidget_Read_DAQ->item(i,0)->text();
        pam.daqEventTime = ((QComboBox*)ui->tableWidget_Read_DAQ->cellWidget(i,1))->currentText();
        pam.daqEventIndex = ((QComboBox*)ui->tableWidget_Read_DAQ->cellWidget(i,1))->currentIndex();
        pam.pamDefaultValue = ui->tableWidget_Read_DAQ->item(i, 2)->text().toDouble();

        pamList.append(pam);
    }
    xcpConfig.daqPamList = pamList;
}

void XcpSetDlg::unpackPamInfoToTable()
{
    ui->tableWidget_Read->clearContents();
    ui->tableWidget_Write->clearContents();
    ui->tableWidget_Read_DAQ->clearContents();

    QList<XcpPam> pamList = xcpConfig.pollPamList;
    ui->tableWidget_Read->setRowCount(pamList.count());
    for(int i = 0; i < pamList.count(); i++)
    {
        XcpPam pam = pamList.at(i);
        QTableWidgetItem *nameItem = new QTableWidgetItem(pam.pamName);
        QComboBox *cb_type = new QComboBox();
        cb_type->addItems(pollTypeList);
        cb_type->setCurrentText(pam.pollType);
        QTableWidgetItem *valueItem = new QTableWidgetItem(QString::number(pam.pamDefaultValue, 'g', 6));
        valueItem->setTextAlignment(Qt::AlignCenter);

        ui->tableWidget_Read->setItem(i, 0, nameItem);
        ui->tableWidget_Read->setCellWidget(i, 1, cb_type);
        ui->tableWidget_Read->setItem(i, 2, valueItem);
    }

    pamList = xcpConfig.calPamList;
    ui->tableWidget_Write->setRowCount(pamList.count());
    for(int i = 0; i < pamList.count(); i++)
    {
        XcpPam pam = pamList.at(i);

        QTableWidgetItem *nameItem = new QTableWidgetItem(pam.pamName);
        QTableWidgetItem *valueItem = new QTableWidgetItem(QString::number(pam.pamDefaultValue, 'g', 6));
        QTableWidgetItem *typeItem = new QTableWidgetItem(pam.caliType);
        valueItem->setTextAlignment(Qt::AlignCenter);
        typeItem->setTextAlignment(Qt::AlignCenter);

        ui->tableWidget_Write->setItem(i, 0, nameItem);
        ui->tableWidget_Write->setItem(i, 1, valueItem);
        ui->tableWidget_Write->setItem(i, 2, typeItem);
    }

    pamList = xcpConfig.daqPamList;
    ui->tableWidget_Read_DAQ->setRowCount(pamList.count());
    for(int i = 0; i < pamList.count(); i++)
    {
        XcpPam pam = pamList.at(i);
        QTableWidgetItem *nameItem = new QTableWidgetItem(pam.pamName);
        QComboBox *cb_type = new QComboBox();
        cb_type->addItems(daqEventNameList);
        cb_type->setCurrentText(pam.daqEventTime);
        QTableWidgetItem *valueItem = new QTableWidgetItem(QString::number(pam.pamDefaultValue, 'g', 6));
        valueItem->setTextAlignment(Qt::AlignCenter);

        ui->tableWidget_Read_DAQ->setItem(i, 0, nameItem);
        ui->tableWidget_Read_DAQ->setCellWidget(i, 1, cb_type);
        ui->tableWidget_Read_DAQ->setItem(i, 2, valueItem);
    }

    ui->tableWidget_Read->resizeRowsToContents();
    ui->tableWidget_Read->resizeColumnToContents(0);
    ui->tableWidget_Write->resizeRowsToContents();
    ui->tableWidget_Write->resizeColumnToContents(0);
    ui->tableWidget_Read_DAQ->resizeRowsToContents();
    ui->tableWidget_Read_DAQ->resizeColumnToContents(0);
}

void XcpSetDlg::showXcpSetFromA2lProj(A2L_Project *a2lProject)
{
    if(!a2lProject) return;

    qDebug()<<"***Show xcp config from a2l project.";

    A2L_Daq *xcpDaqInfo = a2lProject->getDaqInfo();
    if (xcpDaqInfo) ui->le_DaqType->setText(xcpDaqInfo->DaqType);

    ui->cb_XcpOnCan->clear();
    ui->cb_XcpOnCan->addItems(a2lProject->getXcpOnCanInstanceNameList());

    ui->cb_Event->clear();
    ui->cb_Event->addItems(a2lProject->getEventNameList());

    ui->cb_EventTime->clear();
    ui->cb_EventTime->addItems(a2lProject->getEventTimeList());

    daqEventNameList = a2lProject->getEventTimeList();

    ui->cb_DaqList->clear();
    ui->cb_DaqList->addItems(a2lProject->getDaqListList());

    ui->cb_DaqListCanId->clear();
    ui->cb_DaqListCanId->addItems(a2lProject->getDaqListCanIdList());

    ui->cb_EventCanIdList->clear();
    ui->cb_EventCanIdList->addItems(a2lProject->getEventCanIdList());

    A2L_Xcp_On_Can *xcpOnCanInfo = a2lProject->getXcpOnCanInfo(ui->cb_XcpOnCan->currentIndex());
    if (xcpOnCanInfo)
    {
        ui->sb_CMD_ID->setValue(xcpOnCanInfo->CAN_ID_MASTER);
        ui->sb_RES_ID->setValue(xcpOnCanInfo->CAN_ID_SLAVE);

        ui->cb_Baudrate->setCurrentText(QString::number(xcpOnCanInfo->BAUDRATE));
        ui->sb_DAQMBL->setValue(xcpOnCanInfo->MAX_BUS_LOAD);

        qDebug()<<"fd baud:"<<xcpOnCanInfo->FD_BAUDRATE;
        qDebug()<<"max dlc:"<<xcpOnCanInfo->MAX_DLC;
        if(xcpOnCanInfo->FD_BAUDRATE > 0)
        {
            ui->cb_FdBaudrate->setCurrentText(QString::number(xcpOnCanInfo->FD_BAUDRATE));
        }
        if(xcpOnCanInfo->MAX_DLC > 8)
        {
            ui->sb_maxDLC->setValue(xcpOnCanInfo->MAX_DLC);
        }
        if(xcpOnCanInfo->IsCANFD)
        {
            ui->cb_isCANFD->setChecked(xcpOnCanInfo->IsCANFD);
        }
    }

    A2L_Daq_Memory_Consumption *daqMemory = a2lProject->getDaqMemoryConsumption();
    if (daqMemory) ui->sb_DAQMEM->setValue(daqMemory->Daq_Memory_Limit);

    disconnect(ui->cb_MemSegList, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &XcpSetDlg::Slt_memSegIndexChanged);
    ui->cb_MemSegList->clear();
    QList<A2L_MemorySegment*> a2lMemSegList = a2lProject->getA2lMemSegList();
    QStringList memSegNameList;
    foreach (A2L_MemorySegment* memSeg, a2lMemSegList) {
        memSegNameList.append(memSeg->memName);
    }

    ui->cb_MemSegList->addItems(memSegNameList);
    ui->cb_MemSegList->setCurrentIndex(0);
    Slt_memSegIndexChanged(0);
    connect(ui->cb_MemSegList, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &XcpSetDlg::Slt_memSegIndexChanged);

    ui->cb_daqTimeSet->clear();
    ui->cb_daqTimeSet->addItems(a2lProject->getEventNameList());
}

void XcpSetDlg::showXcpSetFromConfig()
{
    qDebug()<<"***Show xcp config from config.";

    ui->le_DaqType->setText(xcpConfig.daqType);

    ui->cb_XcpOnCan->clear();
    QStringList nameList_xcpOnCan = xcpConfig.xcpOnCanNameList;
    ui->cb_XcpOnCan->addItems(nameList_xcpOnCan);

    ui->cb_Event->clear();
    QStringList nameList_event;
    foreach (A2L_Event *event, xcpConfig.eventList) {
        nameList_event.append(event->ShortName);
    }
    ui->cb_Event->addItems(nameList_event);

    ui->cb_EventTime->clear();
    QStringList nameList_eventTime = xcpConfig.eventTimeList;
    ui->cb_EventTime->addItems(nameList_eventTime);

    QStringList nameList_daqEventTime = xcpConfig.eventTimeList;
    daqEventNameList = nameList_daqEventTime;

    ui->cb_DaqList->clear();
    QStringList nameList_daq = xcpConfig.daqListNameList;
    ui->cb_DaqList->addItems(nameList_daq);

    ui->cb_DaqListCanId->clear();
    QStringList nameList_daqCanId = xcpConfig.daqListCanIdList;
    ui->cb_DaqListCanId->addItems(nameList_daqCanId);

    ui->cb_EventCanIdList->clear();
    QStringList nameList_eventCanId;
    foreach (A2L_EventCanId *eventCanId, xcpConfig.eventCanIdList) {
        nameList_eventTime.append(eventCanId->Name);
    }
    ui->cb_EventCanIdList->addItems(nameList_eventCanId);

    ui->sb_CMD_ID->setValue(xcpConfig.id_CMD);
    ui->sb_RES_ID->setValue(xcpConfig.id_RES);

    ui->cb_Baudrate->setCurrentText(QString::number(xcpConfig.baudrate));
    ui->cb_FdBaudrate->setCurrentText(QString::number(xcpConfig.fdBaudrate));
    ui->cb_isCANFD->setChecked(xcpConfig.isCanFD);
    ui->sb_maxDLC->setValue(xcpConfig.maxDLC);

    ui->sb_DAQMBL->setValue(100);

    ui->cb_daqTimeSet->clear();
    ui->cb_daqTimeSet->addItems(nameList_event);

}

QString XcpSetDlg::getXcpSetFileName() const
{
    return xcpSetFileName;
}

QString XcpSetDlg::getXcpSetFilePath() const
{
    return xcpSetFilePath;
}

void XcpSetDlg::readSetting()
{
    QSettings setting(xcpSetFilePath, QSettings::IniFormat);
    xcpConfig.workName = setting.value("Workspace").toString();
    xcpConfig.intfName = setting.value("Interface").toString();
    xcpConfig.a2lProjectName = setting.value("A2LProjectName").toString();
    xcpConfig.a2lFilePath = setting.value("A2LFilePath").toString();
    xcpConfig.hexFilePath = setting.value("HexFilePath").toString();

    xcpConfig.a2lFilePath_ref = setting.value("A2LFilePath_ref").toString();
    xcpConfig.hexFilePath_ref = setting.value("HexFilePath_ref").toString();
    xcpConfig.appDir = setting.value("appDir").toString();

    xcpConfig.useSeedKey = setting.value("UseSeedKey").toBool();
    xcpConfig.seedKeyDllPath = setting.value("SeedKeyDllPath").toString();
    ui->cb_xcpCanDeviceType->setCurrentIndex(setting.value("XcpDeviceType").toInt());
    ui->cb_TSCAN_Device_Index->setCurrentIndex(setting.value("TsDeviceIndex").toInt());
    ui->cb_zlgCanDeviceType->setCurrentIndex(setting.value("ZlgDeviceType").toInt());
    ui->cb_ZLGCAN_Device_Index->setCurrentIndex(setting.value("ZlgDeviceIndex").toInt());

    QString curDir = QCoreApplication::applicationDirPath();
    if(curDir != xcpConfig.appDir)
    {
        qDebug()<<"[A2L PATH]Origin app dir:"<<xcpConfig.appDir;
        qDebug()<<"[A2L PATH]New app dir:"<<curDir;
        if(xcpConfig.appDir == "")
        {
            xcpConfig.a2lFilePath = (xcpConfig.a2lFilePath_ref == "") ? xcpConfig.a2lFilePath : curDir + xcpConfig.a2lFilePath_ref;
            xcpConfig.hexFilePath = (xcpConfig.hexFilePath_ref == "") ? xcpConfig.hexFilePath : curDir + xcpConfig.hexFilePath_ref;
        }
        else
        {
            xcpConfig.a2lFilePath.replace(xcpConfig.appDir, curDir);
            xcpConfig.hexFilePath.replace(xcpConfig.appDir, curDir);
        }
        xcpConfig.appDir = curDir;

        qDebug()<<"[A2L PATH]New a2l path:"<<xcpConfig.a2lFilePath;
        qDebug()<<"[A2L PATH]New hex path:"<<xcpConfig.hexFilePath;
    }

    xcpConfig.index_XcpOnCan = setting.value("Index_XcpOnCan").toUInt();
    xcpConfig.baudrate = setting.value("Baudrate").toUInt();
    xcpConfig.id_CMD = setting.value("ID_CMD").toUInt();
    xcpConfig.id_RES = setting.value("ID_RES").toUInt();
    xcpConfig.daqType = setting.value("DaqType").toString();

    xcpConfig.isCanFD = setting.value("IsCanFD").toBool();
    xcpConfig.fdBaudrate = setting.value("FdBaudrate").toUInt();
    xcpConfig.maxDLC = setting.value("MAX_DLC").toUInt();

    ui->cb_Baudrate->setCurrentText(QString::number(xcpConfig.baudrate));
    ui->cb_FdBaudrate->setCurrentText(QString::number(xcpConfig.fdBaudrate));
    ui->cb_isCANFD->setChecked(xcpConfig.isCanFD);
    ui->sb_maxDLC->setValue(xcpConfig.maxDLC);

    xcpConfig.calCsvFilePath = setting.value("CalCsvFile").toString();
    if(!xcpConfig.calCsvFilePath.isEmpty())
    {
        loadCalInfoList(xcpConfig.calCsvFilePath);
    }

    xcpConfig.useA2lSetFile = setting.value("UseA2lSetFile").toBool();
    xcpConfig.usePamsInExcel = setting.value("UsePamsInExcel").toBool();

    // load poll pams
    QList<XcpPam> pamList;
    QStringList pamNames, pamPollTypes, pamDefaultValues, caliTypes, pamDaqEventTypes;
    QString pamName = setting.value("PollPamNames").toString();
    QString pamPollType = setting.value("PollPamPollTypes").toString();
    QString pamDefaultValue = setting.value("PollPamDefaultValues").toString();

    if (!pamName.isEmpty())
    {
        pamNames = pamName.split("|");
        pamPollTypes = pamPollType.split("|");
        pamDefaultValues = pamDefaultValue.split("|");
    }
    //qDebug()<<"read pams count:"<<pamNames.count()<<", default values count:"<<pamDefaultValues.count();
    for(int j = 0; j < pamNames.count(); j++)
    {
        XcpPam pam;
        pam.pamName = pamNames.at(j);
        pam.pollType = pamPollTypes.at(j);
        QString valueStr = pamDefaultValues.at(j);
        pam.pamDefaultValue = valueStr.toDouble();

        pamList.append(pam);
    }
    xcpConfig.pollPamList = pamList;

    // load cal pams
    pamList.clear();
    pamNames.clear();
    pamPollTypes.clear();
    pamDefaultValues.clear();
    QString caliType;

    pamName = setting.value("CalPamNames").toString();
    pamDefaultValue = setting.value("CalPamDefaultValues").toString();
    caliType = setting.value("CalPamTypes").toString();

    if (!pamName.isEmpty())
    {
        pamNames = pamName.split("|");
        pamDefaultValues = pamDefaultValue.split("|");
        caliTypes = caliType.split("|");
    }
    //qDebug()<<"write pams count:"<<pamNames.count()<<", default values count:"<<pamDefaultValues.count();
    for(int j = 0; j < pamNames.count(); j++)
    {
        XcpPam pam;
        pam.pamName = pamNames.at(j);
        QString valueStr = pamDefaultValues.at(j);
        pam.pamDefaultValue = valueStr.toDouble();

        if(caliTypes.count() > j)
            pam.caliType = caliTypes.at(j);

        pamList.append(pam);
    }
    xcpConfig.calPamList = pamList;

    // load daqReadPams
    pamList.clear();
    pamNames.clear();
    pamDaqEventTypes.clear();
    pamDefaultValues.clear();
    pamName = setting.value("DaqPamNames").toString();
    QString pamDaqEventType = setting.value("DaqPamEventTypes").toString();
    pamDefaultValue = setting.value("DaqPamDefaultValues").toString();

    if (!pamName.isEmpty())
    {
        pamNames = pamName.split("|");
        pamDaqEventTypes = pamDaqEventType.split("|");
        pamDefaultValues = pamDefaultValue.split("|");
    }

    for(int j = 0; j < pamNames.count(); j++)
    {
        XcpPam pam;
        pam.pamName = pamNames.at(j);
        pam.daqEventTime = pamDaqEventTypes.at(j);
        QString valueStr = pamDefaultValues.at(j);
        pam.pamDefaultValue = valueStr.toDouble();
        pamList.append(pam);
    }
    xcpConfig.daqPamList = pamList;

    // load [daq] [daq list] [event list] [event can id list]
    xcpConfig.daqList.clear();
    xcpConfig.eventCanIdList.clear();
    xcpConfig.eventList.clear();

    xcpConfig.a2lDaq.Name = setting.value("DaqName").toString();
    xcpConfig.a2lDaq.DaqType = setting.value("DaqType").toString();
    xcpConfig.a2lDaq.GranularityOdtEntrySize = setting.value("GranularityOdtEntrySize").toString();
    xcpConfig.a2lDaq.IdentificationField = setting.value("IdentificationField").toString();
    xcpConfig.a2lDaq.AddressExtType = setting.value("AddressExtType").toString();
    xcpConfig.a2lDaq.OptimisationType = setting.value("OptimisationType").toString();
    xcpConfig.a2lDaq.Max_Daq = (quint16)setting.value("Max_Daq").toUInt();
    xcpConfig.a2lDaq.Min_Daq = (quint16)setting.value("Min_Daq").toUInt();
    xcpConfig.a2lDaq.Max_Event_Channel = (quint8)setting.value("Max_Event_Channel").toUInt();
    xcpConfig.a2lDaq.Max_Odt_Entry_Size_Daq = (quint8)setting.value("Max_Odt_Entry_Size_Daq").toUInt();

    int daqCount = setting.beginReadArray("XcpDaqList");
    for (int i = 0; i < daqCount; i++)
    {
        setting.setArrayIndex(i);
        A2L_Daq_List *daq = new A2L_Daq_List();
        daq->Name = setting.value("DaqListName").toString();
        daq->DaqListNumber = (quint16)(setting.value("DaqListNumber").toUInt());
        daq->DaqListType = setting.value("DaqListType").toString();
        daq->Max_Odt = (quint8)setting.value("Max_Odt").toUInt();
        daq->Max_Odt_Entries = (quint8)setting.value("Max_Odt_Entries").toUInt();
        daq->First_Pid = (quint8)(setting.value("First_Pid").toUInt());
        daq->Event_Fixed = (quint8)(setting.value("Event_Fixed").toUInt());

        xcpConfig.daqList.append(daq);
    }
    setting.endArray();

    int eventCanIdCount = setting.beginReadArray("XcpEventCanIdList");
    for (int i = 0; i < eventCanIdCount; i++)
    {
        setting.setArrayIndex(i);
        A2L_EventCanId *eventCanId = new A2L_EventCanId();
        eventCanId->Name = setting.value("EventCanIdName").toString();
        eventCanId->EventNumber = (quint8)(setting.value("EventNumber").toUInt());
        eventCanId->Event_CAN_ID = (quint32)(setting.value("EventCanId").toUInt());

        xcpConfig.eventCanIdList.append(eventCanId);
    }
    setting.endArray();

    int eventCount = setting.beginReadArray("XcpEventList");
    for (int i = 0; i < eventCount; i++)
    {
        setting.setArrayIndex(i);
        A2L_Event *event = new A2L_Event();
        event->Name = setting.value("EventName").toString();
        event->ChannelNumber = (quint8)(setting.value("ChannelNumber").toUInt());
        event->EventChannelName = setting.value("EventChannelName").toString();
        event->ShortName = setting.value("ShortName").toString();
        event->EventType = setting.value("EventType").toString();
        event->Priority = (quint8)(setting.value("EventPriority").toUInt());
        event->TimeCycle = (quint8)(setting.value("TimeCycle").toUInt());
        event->TimeUnit = (quint8)(setting.value("TimeUnit").toUInt());
        event->Max_Daq_List = (quint8)(setting.value("MaxDaqList").toUInt());
        event->Rate_ms = setting.value("Rate_ms").toFloat();
        QString eventCanIdName = setting.value("RefEventCanIdName").toString();
        if(!eventCanIdName.isEmpty())
        {

        }

        xcpConfig.eventList.append(event);
    }
    setting.endArray();

    // load xcp on can name list
    int xcpOnCanCount = setting.beginReadArray("XcpOnCanNameList");
    for (int i = 0; i < xcpOnCanCount; i++)
    {
        setting.setArrayIndex(i);
        QString xcpOnCanName = setting.value("XcpOnCanName").toString();
        xcpConfig.xcpOnCanNameList.append(xcpOnCanName);
    }
    setting.endArray();

    // load daq list name list
    int daqListCount = setting.beginReadArray("DaqListNameList");
    for (int i = 0; i < daqListCount; i++)
    {
        setting.setArrayIndex(i);
        QString daqListName = setting.value("DaqListName").toString();
        xcpConfig.daqListNameList.append(daqListName);
    }
    setting.endArray();

    // load daq list can id list
    int daqListCanIdCount = setting.beginReadArray("DaqListCanIdList");
    for (int i = 0; i < daqListCanIdCount; i++)
    {
        setting.setArrayIndex(i);
        QString daqListCanId = setting.value("DaqListCanId").toString();
        xcpConfig.daqListCanIdList.append(daqListCanId);
    }
    setting.endArray();

    // load event time name list
    int eventTimeCount = setting.beginReadArray("EventTimeNameList");
    for (int i = 0; i < eventTimeCount; i++)
    {
        setting.setArrayIndex(i);
        QString eventTimeName = setting.value("EventTimeName").toString();
        xcpConfig.eventTimeList.append(eventTimeName);
    }
    setting.endArray();

}

void XcpSetDlg::writeSetting()
{

    xcpConfig.useA2lSetFile = ui->cb_useA2lSetFile->isChecked();
    xcpConfig.usePamsInExcel = ui->cb_usePamsInExcel->isChecked();

    xcpConfig.useSeedKey = ui->cb_useSeedKey->isChecked();
    xcpConfig.seedKeyDllPath = ui->le_seedKeyDllPath->text();

    xcpConfig.isCanFD = ui->cb_isCANFD->isChecked();
    xcpConfig.fdBaudrate = ui->cb_FdBaudrate->currentText().toUInt();
    xcpConfig.maxDLC = ui->sb_maxDLC->value();

    QSettings setting(xcpSetFilePath, QSettings::IniFormat);

    setting.setValue("Workspace", xcpConfig.workName);
    setting.setValue("Interface", xcpConfig.intfName);
    setting.setValue("A2LProjectName", xcpConfig.a2lProjectName);
    setting.setValue("A2LFilePath", xcpConfig.a2lFilePath);
    setting.setValue("HexFilePath", xcpConfig.hexFilePath);

    setting.setValue("UseSeedKey", xcpConfig.useSeedKey);
    setting.setValue("SeedKeyDllPath", xcpConfig.seedKeyDllPath);

    QString curDir = QCoreApplication::applicationDirPath();
    if(curDir != xcpConfig.appDir)
    {
        xcpConfig.appDir = curDir;
    }
    qDebug()<<"[A2L PATH]Dir saved:"<<xcpConfig.appDir;
    setting.setValue("appDir", xcpConfig.appDir);
    setting.setValue("A2LFilePath_ref", xcpConfig.a2lFilePath_ref);
    setting.setValue("HexFilePath_ref", xcpConfig.hexFilePath_ref);

    setting.setValue("Index_XcpOnCan", QString::number(xcpConfig.index_XcpOnCan));
    setting.setValue("Baudrate", QString::number(xcpConfig.baudrate));
    setting.setValue("ID_CMD", QString::number(xcpConfig.id_CMD));
    setting.setValue("ID_RES", QString::number(xcpConfig.id_RES));
    setting.setValue("DaqType", xcpConfig.daqType);

    setting.setValue("IsCanFD", QString::number(xcpConfig.isCanFD));
    setting.setValue("FdBaudrate", QString::number(xcpConfig.fdBaudrate));
    setting.setValue("MAX_DLC", QString::number(xcpConfig.maxDLC));

    setting.setValue("CalCsvFile", xcpConfig.calCsvFilePath);
    setting.setValue("UseA2lSetFile", xcpConfig.useA2lSetFile);
    setting.setValue("UsePamsInExcel", xcpConfig.usePamsInExcel);

    setting.setValue("XcpDeviceType", ui->cb_xcpCanDeviceType->currentIndex());
    setting.setValue("TsDeviceIndex", ui->cb_TSCAN_Device_Index->currentIndex());
    setting.setValue("ZlgDeviceType", ui->cb_zlgCanDeviceType->currentIndex());
    setting.setValue("ZlgDeviceIndex", ui->cb_ZLGCAN_Device_Index->currentIndex());

    packPamInfoToConfig();

    // poll pams setting
    QList<XcpPam> pamList = xcpConfig.pollPamList;
    QString pamNames, pamPollTypes, pamDefaultValues, pamCaliTypes, pamDaqEventTypes;
    pamNames.clear();
    pamPollTypes.clear();
    pamDefaultValues.clear();

    for (int j = 0; j < pamList.count(); j++)
    {
        if (j == pamList.count()-1)
        {
            pamNames += (pamList.at(j).pamName);
            pamPollTypes += (pamList.at(j).pollType);
            pamDefaultValues += QString::number(pamList.at(j).pamDefaultValue, 'g', 6);
        }
        else
        {
            pamNames += (pamList.at(j).pamName + "|");
            pamPollTypes += (pamList.at(j).pollType + "|");
            pamDefaultValues += QString::number(pamList.at(j).pamDefaultValue, 'g', 6) + "|";
        }
    }
    setting.setValue("PollPamNames", pamNames);
    setting.setValue("PollPamPollTypes", pamPollTypes);
    setting.setValue("PollPamDefaultValues", pamDefaultValues);

    //cali pams setting
    pamList.clear();
    pamList = xcpConfig.calPamList;
    pamNames.clear();
    pamCaliTypes.clear();

    pamDefaultValues.clear();
    for (int j = 0; j < pamList.count(); j++)
    {
        if (j == pamList.count()-1)
        {
            pamNames += (pamList.at(j).pamName);

            pamDefaultValues += QString::number(pamList.at(j).pamDefaultValue, 'g', 6);

            pamCaliTypes += (pamList.at(j).caliType);
        }
        else
        {
            pamNames += (pamList.at(j).pamName + "|");

            pamDefaultValues += QString::number(pamList.at(j).pamDefaultValue, 'g', 6) + "|";

            pamCaliTypes += (pamList.at(j).caliType + "|");
        }
    }
    setting.setValue("CalPamNames", pamNames);
    setting.setValue("CalPamDefaultValues", pamDefaultValues);
    setting.setValue("CalPamTypes", pamCaliTypes);

    // daq pams setting
    pamList = xcpConfig.daqPamList;
    pamNames.clear();
    pamDaqEventTypes.clear();
    pamDefaultValues.clear();

    for (int j = 0; j < pamList.count(); j++)
    {
        if (j == pamList.count()-1)
        {
            pamNames += (pamList.at(j).pamName);
            pamDaqEventTypes += (pamList.at(j).daqEventTime);
            pamDefaultValues += QString::number(pamList.at(j).pamDefaultValue, 'g', 6);
        }
        else
        {
            pamNames += (pamList.at(j).pamName + "|");
            pamDaqEventTypes += (pamList.at(j).daqEventTime + "|");
            pamDefaultValues += QString::number(pamList.at(j).pamDefaultValue, 'g', 6) + "|";
        }
    }
    setting.setValue("DaqPamNames", pamNames);
    setting.setValue("DaqPamEventTypes", pamDaqEventTypes);
    setting.setValue("DaqPamDefaultValues", pamDefaultValues);

    if(curA2LProj)
    {
        // a2l daq info
        A2L_Daq *daqInfo = curA2LProj->getDaqInfo();
        setting.setValue("DaqName", daqInfo->Name);
        setting.setValue("DaqType", daqInfo->DaqType);
        setting.setValue("GranularityOdtEntrySize", daqInfo->GranularityOdtEntrySize);
        setting.setValue("IdentificationField", daqInfo->IdentificationField);
        setting.setValue("AddressExtType", daqInfo->AddressExtType);
        setting.setValue("OptimisationType", daqInfo->OptimisationType);
        setting.setValue("Min_Daq", daqInfo->Min_Daq);
        setting.setValue("Max_Event_Channel", daqInfo->Max_Event_Channel);
        setting.setValue("Max_Odt_Entry_Size_Daq", daqInfo->Max_Odt_Entry_Size_Daq);

        // a2l daq list
        setting.beginWriteArray("XcpDaqList");
        QList<A2L_Daq_List*> daqList = curA2LProj->getDaqListStructList();
        int daqCount = daqList.count();
        for(int i = 0; i < daqCount; i++)
        {
            setting.setArrayIndex(i);

            A2L_Daq_List *daq = daqList.at(i);
            setting.setValue("DaqListName", daq->Name);
            setting.setValue("DaqListNumber", daq->DaqListNumber);
            setting.setValue("DaqListType", daq->DaqListType);
            setting.setValue("Max_Odt", daq->Max_Odt);
            setting.setValue("Max_Odt_Entries", daq->Max_Odt_Entries);
            setting.setValue("First_Pid", daq->First_Pid);
            setting.setValue("Event_Fixed", daq->Event_Fixed);
        }
        setting.endArray();

        // a2l event list
        setting.beginWriteArray("XcpEventList");
        QList<A2L_Event*> eventList = curA2LProj->getEventList();
        int eventCount = eventList.count();
        for(int i = 0; i < eventCount; i++)
        {
            setting.setArrayIndex(i);

            A2L_Event *event = eventList.at(i);
            setting.setValue("EventName", event->Name);
            setting.setValue("ChannelNumber", QString::number(event->ChannelNumber));
            setting.setValue("EventChannelName", event->EventChannelName);
            setting.setValue("ShortName", event->ShortName);
            setting.setValue("EventType", event->EventType);
            setting.setValue("EventPriority", QString::number(event->Priority));
            setting.setValue("TimeCycle", QString::number(event->TimeCycle));
            setting.setValue("TimeUnit", QString::number(event->TimeUnit));
            setting.setValue("MaxDaqList", QString::number(event->Max_Daq_List));

            setting.setValue("Rate_ms", QString::number(event->Rate_ms));
            if(event->eventCanId)
                setting.setValue("RefEventCanIdName", event->eventCanId->Name);
        }
        setting.endArray();

        // a2l event can id list
        setting.beginWriteArray("XcpEventCanIdList");
        QList<A2L_EventCanId*> eventCanIdList = curA2LProj->getA2lEventCanIdList();
        int eventCanIdCount = eventCanIdList.count();
        for(int i = 0; i < eventCanIdCount; i++)
        {
            setting.setArrayIndex(i);

            A2L_EventCanId *eventCanId = eventCanIdList.at(i);
            setting.setValue("EventCanIdName", eventCanId->Name);
            setting.setValue("EventNumber", QString::number(eventCanId->EventNumber));
            setting.setValue("EventCanId", QString::number(eventCanId->Event_CAN_ID));
        }
        setting.endArray();

        // a2l xcp on can name list
        setting.beginWriteArray("XcpOnCanNameList");
        QStringList xcpOnCanNameList = curA2LProj->getXcpOnCanInstanceNameList();
        int xcpOnCanCount = xcpOnCanNameList.count();
        for(int i = 0; i < xcpOnCanCount; i++)
        {
            setting.setArrayIndex(i);

            setting.setValue("XcpOnCanName", xcpOnCanNameList.at(i));
        }
        setting.endArray();

        // a2l daq list name list
        setting.beginWriteArray("DaqListNameList");
        QStringList daqListNameList = curA2LProj->getDaqListList();
        int daqListCount = daqListNameList.count();
        for(int i = 0; i < daqListCount; i++)
        {
            setting.setArrayIndex(i);

            setting.setValue("DaqListName", daqListNameList.at(i));
        }
        setting.endArray();

        // a2l daq list can id name list
        setting.beginWriteArray("DaqListCanIdList");
        QStringList daqListCanIdList = curA2LProj->getDaqListCanIdList();
        int daqListCanIdCount = daqListCanIdList.count();
        for(int i = 0; i < daqListCanIdCount; i++)
        {
            setting.setArrayIndex(i);

            setting.setValue("DaqListCanId", daqListCanIdList.at(i));
        }
        setting.endArray();

        // a2l event time name list
        setting.beginWriteArray("EventTimeNameList");
        QStringList EventTimeNameList = curA2LProj->getEventTimeList();
        int eventTimeCount = EventTimeNameList.count();
        for(int i = 0; i < eventTimeCount; i++)
        {
            setting.setArrayIndex(i);

            setting.setValue("EventTimeName", EventTimeNameList.at(i));
        }
        setting.endArray();
    }

    //export a2l pams to excel

    emit xcpSetUpdated();

}

void XcpSetDlg::showCalListInTable(QList<CalInfo> calList)
{
    ui->tableWidget_calInfoList->clearContents();
    ui->tableWidget_calInfoList->setRowCount(calList.count());
    for(int i = 0; i < calList.count(); i++)
    {
        CalInfo calIns = calList.at(i);
        QTableWidgetItem *nameItem = new QTableWidgetItem(calIns.charName);
        QTableWidgetItem *typeItem = new QTableWidgetItem(calIns.type);
        QTableWidgetItem *unitItem = new QTableWidgetItem(calIns.unit);

        QTableWidgetItem *xAxisItem = new QTableWidgetItem(calIns.xAxisName);
        QTableWidgetItem *yAxisItem = new QTableWidgetItem(calIns.yAxisName);

        QString xAxis = calIns.xAxisStrArray.join(",");
        QString yAxis = calIns.yAxisStrArray.join(",");
        QString zValue = calIns.zValueStrArray.join(",");
        QTableWidgetItem *zValueItem = new QTableWidgetItem(zValue);

        ui->tableWidget_calInfoList->setItem(i, 0, nameItem);
        ui->tableWidget_calInfoList->setItem(i, 1, typeItem);
        ui->tableWidget_calInfoList->setItem(i, 2, unitItem);
        ui->tableWidget_calInfoList->setItem(i, 3, xAxisItem);
        ui->tableWidget_calInfoList->setItem(i, 4, yAxisItem);
        ui->tableWidget_calInfoList->setItem(i, 5, zValueItem);
    }
    ui->tableWidget_calInfoList->resizeColumnsToContents();
}

void XcpSetDlg::loadCalInfoList(QString csvFile)
{
    if(csvFile.isEmpty())
        return;

    ui->le_csvFile->setText(csvFile);

    CaliCsv *caliCsv = new CaliCsv(this, csvFile);

    QList<CalInfo> calInfoList = caliCsv->getCalList();
    //qDebug()<<"cal pam count:"<<calList.count();

    showCalListInTable(calInfoList);
}

QList<A2L_VarChar *> XcpSetDlg::getCharVarListFromCalInfoList(QList<CalInfo> calList)
{
    QList<A2L_VarChar*> charVarList;
    if(curA2LProj == NULL)
        return charVarList;

    QStringList charTypeList = {"VALUE", "VAL_BLK", "CURVE", "MAP"};
    for(int i = 0; i < calList.count(); i++)
    {
        CalInfo calInfo = calList.at(i);

        qDebug()<<"cal index:"<<i;
        if(charTypeList.indexOf(calInfo.type) == -1)
            continue;

        A2L_VarChar *charVar = curA2LProj->getCharVarByName(calInfo.charName);

        if(!charVar)
            continue;

        charVar->smKey = "WP_CSV_" + workName + "_" + funName;

        if(charVar->charType == "VALUE")
        {
            QString valueStr = calInfo.zValueStrArray.at(0);
            bool ok = false;
            double value = valueStr.toDouble(&ok);

            charVar->setValue(ok ? value : 0);

            if(charVar->ConversionType == "TAB_VERB" && !charVar->ConversionVTab.isEmpty())
            {
                QString valuePairStr = charVar->ConversionVTab;
                QStringList valuePairList = valuePairStr.split(",");
                foreach (QString valuePair, valuePairList) {
                    int index = valuePair.indexOf(valueStr);
                    if(index != -1)
                    {
                        QString str = valuePair.split("-").first();
                        value = str.toDouble();
                        charVar->setValue(value);
                    }
                }
            }
        }
        else if(charVar->charType == "VAL_BLK")
        {
            QStringList blkValueStr = calInfo.zValueStrArray;
            bool ok = false;
            for(int i = 0; i < blkValueStr.count(); i++)
            {
                QString valueStr = blkValueStr.at(i);
                double value = valueStr.toDouble(&ok);
                if(i < charVar->number_BLK && ok)
                {
                    charVar->setBlkValue(i, 1, value);
                }
            }

            qDebug()<<"~~~Set Blk value[0]" << charVar->Blk_Values[0] << ", blk count:"<<charVar->Blk_Values.count();
        }
        else if(charVar->charType == "CURVE")
        {
            qDebug()<<"~~~from hex:curve xAxis Value:"<<charVar->hexValue_XList;
            qDebug()<<"~~~from hex:curve zValue:"<<charVar->hexValue_ZList;
            qDebug()<<"~~~from calInfo:curve xAxis Value:"<<calInfo.xAxisStrArray;
            qDebug()<<"~~~from calInfo:curve zValue:"<<calInfo.zValueStrArray;

            QStringList zValueStrList = calInfo.zValueStrArray;
            QStringList xValueStrList = calInfo.xAxisStrArray;
            bool ok1 = false;
            bool ok2 = false;
            for(int i = 0; i < zValueStrList.count(); i++)
            {
                QString valueStr = zValueStrList.at(i);
                QString xAxisStr = xValueStrList.at(i);
                double value = valueStr.toDouble(&ok1);
                if(i < charVar->zCount && ok1)
                {
                    charVar->hexValue_ZList.replace(i, value);
                }
                double xValue = xAxisStr.toDouble(&ok2);
                if(i < charVar->xCount && ok2)
                {
                    charVar->hexValue_XList.replace(i, xValue);
                }
            }

            qDebug()<<"~~~from cal csv:curve xAxis Value:"<<charVar->hexValue_XList;
            qDebug()<<"~~~from cal csv:curve zValue:"<<charVar->hexValue_ZList;
        }
        else if(charVar->charType == "MAP")
        {

        }
        else
        {

        }

        charVarList.append(charVar);
    }
    return charVarList;
}

/*
bool XcpSetDlg::delExcel(QString filePath)
{
    QFileInfo excelFile(filePath);
    if(excelFile.isFile())
    {
        if(!QFile::remove(filePath))
        {
            qDebug()<<"Delete excel file error: "<<filePath;
            return false;
        }
    }

    return excelFile.isFile();
}

void XcpSetDlg::convertToColName(int data, QString &res)
{
    Q_ASSERT(data>0 && data<65535);
    int tempData = data / 26;
    if(tempData > 0)
    {
        int mode = data % 26;
        convertToColName(mode,res);
        convertToColName(tempData,res);
    }
    else
    {
        res=(to26AlphabetString(data)+res);
    }
}

QString XcpSetDlg::to26AlphabetString(int data)
{
    QChar ch = data + 0x40;//A???0x41
    return QString(ch);
}

bool XcpSetDlg::exportMeasToExcel(QList<A2L_VarMeas *> meaVars, QString excelPath)
{
    ExcelOperator *measExcel = new ExcelOperator();
    measExcel->open(excelPath);
    QAxObject *sheet = measExcel->getSheet("Sheet1");
    if(sheet == NULL)
    {
        return false;
    }
    if(meaVars.isEmpty())
        return false;


    QList<QList<QVariant>> measVariants;
    for(int i = 0; i < meaVars.count(); i++)
    {
        QList<QVariant> variant;
        A2L_VarMeas *measVar = meaVars.at(i);

        variant.append(QVariant::fromValue(measVar->Name));
        variant.append(QVariant::fromValue(measVar->ShortName));
        variant.append(QVariant::fromValue(measVar->DataType));
        variant.append(QVariant::fromValue(measVar->ScalingFactor));
        variant.append(QVariant::fromValue(measVar->ScalingOffset));
        variant.append(QVariant::fromValue(measVar->Unit));
        variant.append(QVariant::fromValue(measVar->ECUAddress));
        variant.append(QVariant::fromValue(measVar->DataSizeAG));
        variant.append(QVariant::fromValue(measVar->LowerLimit));
        variant.append(QVariant::fromValue(measVar->UpperLimit));
        variant.append(QVariant::fromValue(measVar->getValue()));
        variant.append(QVariant::fromValue(measVar->ConversionType));
        variant.append(QVariant::fromValue(measVar->COEFFS));
        variant.append(QVariant::fromValue(measVar->StartByte));
        variant.append(QVariant::fromValue(measVar->rate_ms));
        variant.append(QVariant::fromValue(measVar->smKey));

        measVariants.append(variant);
    }

    int row = measVariants.size();
    int col = measVariants.at(0).size();

    QString rangStr;
    convertToColName(col,rangStr);
    rangStr += QString::number(row);
    rangStr = "A1:" + rangStr;
    qDebug()<<rangStr;

    QAxObject *range = sheet->querySubObject("Range(const QString&)",rangStr);
    if(NULL == range || range->isNull())
    {
        return false;
    }
    bool succ = false;

    QVariant var;
    QVariantList vars;
    for(int i = 0; i < row; i++)
    {
        vars.append(QVariant(measVariants[i]));
    }
    var = QVariant(vars);
    succ = range->setProperty("Value", var);
    delete range;

    measExcel->close();

    return true;
}

bool XcpSetDlg::exportCharsToExcel(QList<A2L_VarChar *> charVars, QString excelPath)
{
    ExcelOperator *charExcel = new ExcelOperator();
    charExcel->open(excelPath);
    QAxObject *sheet = charExcel->getSheet("Sheet1");
    if(sheet == NULL)
    {
        return false;
    }

    if(charVars.isEmpty())
        return false;

    QList<QList<QVariant>> charVariants;
    for(int i = 0; i < charVars.count(); i++)
    {
        QList<QVariant> variant;
        A2L_VarChar *charVar = charVars.at(i);

        variant.append(QVariant::fromValue(charVar->Name));
        variant.append(QVariant::fromValue(charVar->charType));
        variant.append(QVariant::fromValue(charVar->DataType));
        variant.append(QVariant::fromValue(charVar->ScalingFactor));
        variant.append(QVariant::fromValue(charVar->ScalingOffset));
        variant.append(QVariant::fromValue(charVar->Unit));
        variant.append(QVariant::fromValue(charVar->ECUAddress));
        variant.append(QVariant::fromValue(charVar->DataSizeAG));
        variant.append(QVariant::fromValue(charVar->LowerLimit));
        variant.append(QVariant::fromValue(charVar->UpperLimit));
        variant.append(QVariant::fromValue(charVar->getValue()));
        variant.append(QVariant::fromValue(charVar->ConversionType));
        variant.append(QVariant::fromValue(charVar->COEFFS));
        variant.append(QVariant::fromValue(charVar->StartByte));
        variant.append(QVariant::fromValue(charVar->smKey));

        charVariants.append(variant);
    }

    int row = charVariants.size();
    int col = charVariants.at(0).size();

    QString rangStr;
    convertToColName(col,rangStr);
    rangStr += QString::number(row);
    rangStr = "A1:" + rangStr;
    qDebug()<<rangStr;

    QAxObject *range = sheet->querySubObject("Range(const QString&)",rangStr);
    if(NULL == range || range->isNull())
    {
        return false;
    }
    bool succ = false;

    QVariant var;
    QVariantList vars;
    for(int i = 0; i < row; i++)
    {
        vars.append(QVariant(charVariants[i]));
    }
    var = QVariant(vars);

    succ = range->setProperty("Value", var);

    delete range;

    charExcel->close();

    return true;
}
*/

bool XcpSetDlg::getA2lPamsAndExportToCsv()
{
    if(!curA2LProj) return false;

    QList<A2L_VarMeas*> measPamList;
    quint64 sizeRead = 0;
    QString smKeyRead = "";

    QList<A2L_VarMeas*> daqMeasPamList;
    quint64 sizeReadDAQ;
    QString smKeyReadDAQ = "";

    QList<A2L_VarChar*>  charPamList;
    quint64 sizeWrite;
    QString smKeyWrite = "";

    QList<A2L_VarChar*>  charMapPamList;

    smKeyRead = "RP_POLL_" + xcpConfig.workName + "_" + funName;
    smKeyWrite = "WP_" + xcpConfig.workName + "_" + funName;
    smKeyReadDAQ = "RP_DAQ_" + xcpConfig.workName + "_" + funName;

    QString smKeyWriteCsv = "WP_CSV_" + xcpConfig.workName + "_" + funName;

    int startBitIndex = 0;
    int startByteIndex = 0;
    startBitIndex += 0;

    qDebug()<<"In xcp fun thread: load a2l pams...";
    qDebug()<<"************getting poll read meas pams from a2l project***************";
    for(int i = 0; i < xcpConfig.pollPamList.count(); i++)
    {
        XcpPam xcpPam = xcpConfig.pollPamList.at(i);

        A2L_VarMeas *measVar = curA2LProj->getMeasVarByName(xcpPam.pamName);

        if(measVar)
        {
            measPamList.append(measVar);

            measVar->StartByte = startByteIndex;
            measVar->smKey = smKeyRead;

            startBitIndex += (measVar->DataSizeAG * 8);
            startByteIndex = startBitIndex/8;

        }
    }

    sizeRead = startBitIndex/8;

    startBitIndex = 0;
    startByteIndex = 0;
    qDebug()<<"************getting cal char pams from a2l project***************";
    for(int i = 0; i < xcpConfig.calPamList.count(); i++)
    {
        XcpPam xcpPam = xcpConfig.calPamList.at(i);

        A2L_VarChar *charVar = curA2LProj->getCharVarByName(xcpPam.pamName);

        if(charVar == NULL)
            continue;

        if(charVar->charType == "VALUE")
        {
            charPamList.append(charVar);

            charVar->StartByte = startByteIndex;
            charVar->smKey = smKeyWrite;
            charVar->setValue(xcpPam.pamDefaultValue);
            //charVar->funIndex = this->funIndex;

            startBitIndex += (charVar->DataSizeAG * 8);
            startByteIndex = startBitIndex/8;

            charVar->pamModule = 1;   // 1 = xcp value, 0 = can pam
        }
        else if(charVar->charType == "CURVE" || charVar->charType == "MAP" || charVar->charType == "VAL_BLK")
        {
            charMapPamList.append(charVar);
        }
    }

    sizeWrite = startBitIndex/8;

    //daq meas pams
    startBitIndex = 0;
    startByteIndex = 0;
    qDebug()<<"************getting daq read meas pams from a2l project***************";
    for(int i = 0; i < xcpConfig.daqPamList.count(); i++)
    {
        XcpPam xcpPam = xcpConfig.daqPamList.at(i);
        //qDebug()<<"xcpPam rate_ms:"<<xcpPam.daqEventTime;
        A2L_VarMeas *measVar = curA2LProj->getMeasVarByName(xcpPam.pamName);

        if(measVar)
        {
            daqMeasPamList.append(measVar);

            measVar->StartByte = startByteIndex;
            measVar->smKey = smKeyReadDAQ;
            measVar->rate_ms = xcpPam.daqEventTime.replace("ms", "").toDouble();
            //qDebug()<<"Meas rate_ms:"<<measVar->rate_ms;

            startBitIndex += (measVar->DataSizeAG * 8);
            startByteIndex = startBitIndex/8;
        }
    }

    sizeReadDAQ = startBitIndex/8;

    qDebug()<<"In xcp setting:" << funName << ", total size of measVars:"<<sizeRead<<", size of charVars for VALUE:"<<sizeWrite<<", size of daqMeasVars:"<<sizeReadDAQ;
    qDebug()<<"Num of charVars for MAP:"<<charMapPamList.count();

    if(measPamList.isEmpty() && daqMeasPamList.isEmpty() && charPamList.isEmpty())
        return false;

    int lastIndex = xcpSetFilePath.lastIndexOf("/");
    QString workPath = xcpSetFilePath.left(lastIndex);

    QString pamsPath = workPath + "/xcp_pams/" + funName;
    QFileInfo dirInfo(pamsPath);
    if(!dirInfo.isDir())
    {
        QDir dir;
        dir.mkpath(pamsPath);
    }

    //polling meas
    QString measPath_poll = pamsPath + "/measurements_poll.csv";
    qDebug()<<"[meas csv poll]:"<<measPath_poll;
    delFile(measPath_poll);
    exportMeasToCsv(measPamList, measPath_poll);

    //daq meas
    QString measPath_daq = pamsPath + "/measurements_daq.csv";
    qDebug()<<"[meas csv daq]:"<<measPath_daq;
    delFile(measPath_daq);
    exportMeasToCsv(daqMeasPamList, measPath_daq);

    //cali chars
    QString charPath = pamsPath + "/characteristics.csv";
    qDebug()<<"[char csv]:"<<charPath;
    delFile(charPath);
    exportCharsToCsv(charPamList, charPath);

    //load cdm csv cal info chars
    CaliCsv *caliCsv = new CaliCsv(this, xcpConfig.calCsvFilePath);
    QList<CalInfo> calInfoList = caliCsv->getCalList();
    QList<A2L_VarChar*> csvCharPamList = getCharVarListFromCalInfoList(calInfoList);

    QString csvCharPath = pamsPath + "/characteristics_csv.csv";
    qDebug()<<"[csv char csv]:"<<csvCharPath;
    delFile(csvCharPath);
    exportCharsToCsv(csvCharPamList, csvCharPath);


    if(!measPamList.isEmpty())
    {
        qDeleteAll(measPamList);
    }
    if(!daqMeasPamList.isEmpty())
    {
        qDeleteAll(daqMeasPamList);
    }
    if(!charPamList.isEmpty())
    {
        qDeleteAll(charPamList);
    }
    if(!csvCharPamList.isEmpty())
    {
        qDeleteAll(csvCharPamList);
        delete caliCsv;
    }

    return true;

}

bool XcpSetDlg::delFile(QString filePath)
{
    QFileInfo file(filePath);
    if(file.isFile())
    {
        if(!QFile::remove(filePath))
        {
            qDebug()<<"Delete file error: "<<filePath;
            return false;
        }
    }

    return file.isFile();
}

bool XcpSetDlg::exportMeasToCsv(QList<A2L_VarMeas *> meaVars, QString csvPath)
{
    QFile *csvFile = new QFile(csvPath);

    if(!csvFile->open(QIODevice::ReadWrite | QIODevice::Append))
    {
        qCritical()<<"csv file open error."<<csvPath;
        qDebug()<<csvFile->errorString();
        delete csvFile;

        return false;
    }

    int varCount = meaVars.count();
    int max = (varCount == 0 ? 5 : varCount*10);
    int min = 0;
    QProgressDialog *progressDlg=new QProgressDialog(tr("Export meas vars......"), tr("Cancel"), min, max, this);
    progressDlg->setWindowModality(Qt::WindowModal);
    progressDlg->setGeometry(QRect(700, 500, 600, 80));
    //如果进度条运行的时间小于1，进度条就不会显示，不设置默认是4S
    progressDlg->setMinimumDuration(4);
    //设置标题，可以不设置默认继承父窗口标题
    progressDlg->setWindowTitle(tr("Wait for exporting completed"));
    progressDlg->show();
    progressDlg->setValue(5);

    QTextStream out;
    out.setDevice(csvFile);
    out.setCodec("GB2312");

    for(int i = 0; i < meaVars.count(); i++)
    {
        A2L_VarMeas *meaVar = meaVars.at(i);
        if(!meaVar) continue;
        QString sepStr = "," , lineEndStr = "\n" ;
        out << meaVar->Name + sepStr;
        out << meaVar->ShortName + sepStr;
        out << meaVar->DataType + sepStr;
        out << QString::number(meaVar->ScalingFactor, 'g', 6) + sepStr;
        out << QString::number(meaVar->ScalingOffset, 'g', 6) + sepStr;
        out << meaVar->Unit + sepStr;
        out << QString::number(meaVar->ECUAddress) + sepStr;
        out << QString::number(meaVar->DataSizeAG) + sepStr;
        out << QString::number(meaVar->LowerLimit, 'g', 6) + sepStr;
        out << QString::number(meaVar->UpperLimit, 'g', 6) + sepStr;
        out << QString::number(meaVar->getValue()) + sepStr;
        out << meaVar->ConversionType + sepStr;
        out << meaVar->COEFFS + sepStr;
        out << QString::number(meaVar->StartByte) + sepStr;
        out << QString::number(meaVar->rate_ms, 'g', 6) + sepStr;
        out << meaVar->smKey + lineEndStr;

        progressDlg->setValue((i+1)*10);
    }

    out.flush();
    csvFile->close();

    progressDlg->setValue(max);

    return true;

}

bool XcpSetDlg::exportCharsToCsv(QList<A2L_VarChar *> charVars, QString csvPath)
{
    QFile *csvFile = new QFile(csvPath);

    if(!csvFile->open(QIODevice::ReadWrite | QIODevice::Append))
    {
        qCritical()<<"csv file open error."<<csvPath;
        qDebug()<<csvFile->errorString();
        delete csvFile;

        return false;
    }

    int varCount = charVars.count();
    int max = (varCount == 0 ? 5 : varCount*10);
    int min = 0;
    QProgressDialog *progressDlg=new QProgressDialog(tr("Export char vars......"), tr("Cancel"), min, max, this);
    progressDlg->setWindowModality(Qt::WindowModal);
    progressDlg->setGeometry(QRect(700, 500, 600, 80));
    //如果进度条运行的时间小于1，进度条就不会显示，不设置默认是4S
    progressDlg->setMinimumDuration(4);
    //设置标题，可以不设置默认继承父窗口标题
    progressDlg->setWindowTitle(tr("Wait for exporting completed"));
    progressDlg->show();
    progressDlg->setValue(5);

    QTextStream out;
    out.setDevice(csvFile);
    out.setCodec("GB2312");

    for(int i = 0; i < charVars.count(); i++)
    {
        A2L_VarChar *charVar = charVars.at(i);
        if(!charVar) continue;
        QString sepStr = "," , lineEndStr = "\n" ;
        out << charVar->Name + sepStr;
        out << charVar->charType + sepStr;
        out << charVar->DataType + sepStr;
        out << QString::number(charVar->ScalingFactor, 'g', 6) + sepStr;
        out << QString::number(charVar->ScalingOffset, 'g', 6) + sepStr;
        out << charVar->Unit + sepStr;
        out << QString::number(charVar->ECUAddress) + sepStr;
        out << QString::number(charVar->DataSizeAG) + sepStr;
        out << QString::number(charVar->LowerLimit, 'g', 6) + sepStr;
        out << QString::number(charVar->UpperLimit, 'g', 6) + sepStr;
        out << QString::number(charVar->getValue()) + sepStr;
        out << charVar->ConversionType + sepStr;
        out << charVar->COEFFS + sepStr;
        out << QString::number(charVar->StartByte) + sepStr;
        out << charVar->smKey + lineEndStr;

        progressDlg->setValue((i+1)*10);
    }

    out.flush();

    csvFile->close();

    progressDlg->setValue(max);

    return true;
}

QStringList XcpSetDlg::importPamNamesFromIncaPamExcel(QString excelPath)
{
    QStringList pamNameList;
    if(excelPath.isEmpty()) return pamNameList;

    ExcelOperator *incaExcel = new ExcelOperator();
    incaExcel->open(excelPath);
    QAxObject *sheet = incaExcel->getSheet("INCA");
    if(sheet == NULL)
    {
        qDebug()<<"import xcp pams excel sheet not found!";
        return pamNameList;
    }

    //ui->le_Log->setText("import xcp pams from inca excel......");
    QAxObject *usedRange = sheet->querySubObject("UsedRange");
    QVariant var = usedRange->dynamicCall("value");
    QVariantList varRowContents = var.toList();
    const int rowCount = varRowContents.size();
    if(rowCount <= 1) return pamNameList;

    QVariantList tmp;
    for(int i = 0; i < rowCount; i++)
    {
        if(i == 0) continue;

        tmp=varRowContents[i].toList();

        QString pamName = tmp[0].toString();
        //qDebug()<<"Import INCA pamName:"<<pamName;

        pamNameList.append(pamName);
    }

    incaExcel->close();

    //ui->le_Log->setText("import xcp pams from inca excel end.");

    return pamNameList;
}

QString XcpSetDlg::checkPamIsMeas(QString pamName)
{
    if(!curA2LProj) return "";
    A2L_VarMeas *measVar = curA2LProj->getMeasVarByName(pamName);

    if(measVar)
    {
        return pamName;
    }
   return "";
}

void XcpSetDlg::on_pb_loadCalCsv_clicked()
{
    QFileDialog* fd = new QFileDialog(this);
    QString appPath = QApplication::applicationDirPath();
    QString filePath = fd->getOpenFileName(this, tr("Open File"),appPath, tr("CSV(*.csv)"));

    xcpConfig.calCsvFilePath = filePath;

    loadCalInfoList(filePath);
}

void XcpSetDlg::on_pb_exportPams_clicked()
{

    if(getA2lPamsAndExportToCsv())
    {
        qDebug()<<"Export a2l pams to csv succeed!";
    }
    else
    {
        qCritical()<<"Export a2l pams to csv fail.";
    }

}

void XcpSetDlg::on_pb_seedKeyDll_released()
{
    QFileDialog* fd = new QFileDialog(this);
    QString appPath = QApplication::applicationDirPath();
    QString filePath = fd->getOpenFileName(this, tr("Open File"),appPath, tr("DLL(*.dll)"));

    ui->le_seedKeyDllPath->setText(filePath);
    xcpConfig.seedKeyDllPath = filePath;

}

void XcpSetDlg::on_pb_setDaqTimeForSelPam_released()
{
    QString curSelDaqTime = ui->cb_daqTimeSet->currentText();

    QList<QTableWidgetItem*> selItems = ui->tableWidget_Read_DAQ->selectedItems();
    if(curSelDaqTime.isEmpty() || selItems.isEmpty()) return;

    QList<int> selRows;
    foreach (QTableWidgetItem *item, selItems) {
        int row = item->row();
        if(selRows.indexOf(row) == -1)
        {
            selRows.append(row);
        }
    }

    //std::sort(selRows.begin(), selRows.end(), std::greater<int>());

    foreach (int row, selRows) {
        QComboBox *cb = (QComboBox*)(ui->tableWidget_Read_DAQ->cellWidget(row, 1));
        cb->setCurrentText(curSelDaqTime);
    }
}

void XcpSetDlg::on_pb_upPamDaq_released()
{
    QList<QTableWidgetItem*> selItems = ui->tableWidget_Read_DAQ->selectedItems();
    if(selItems.isEmpty()) return;

    QList<int> selRows;
    foreach (QTableWidgetItem *item, selItems) {
        if(item->column() != 0) continue;

        int row = item->row();
        if(selRows.indexOf(row) == -1)
        {
            selRows.append(row);
        }
    }

    std::sort(selRows.begin(), selRows.end());

    foreach (int row, selRows) {
        if(row == 0) continue;

        QTableWidgetItem *item0 = ui->tableWidget_Read_DAQ->takeItem(row, 0);
        QTableWidgetItem *item1 = ui->tableWidget_Read_DAQ->takeItem(row, 1);
        QTableWidgetItem *item2 = ui->tableWidget_Read_DAQ->takeItem(row, 2);
        QComboBox *cb = (QComboBox*)ui->tableWidget_Read_DAQ->cellWidget(row, 1);

        ui->tableWidget_Read_DAQ->insertRow(row - 1);
        ui->tableWidget_Read_DAQ->setItem(row - 1, 0, item0);
        ui->tableWidget_Read_DAQ->setItem(row - 1, 1, item1);
        ui->tableWidget_Read_DAQ->setItem(row - 1, 2, item2);
        ui->tableWidget_Read_DAQ->setCellWidget(row - 1, 1, cb);

        ui->tableWidget_Read_DAQ->removeRow(row + 1);

        ui->tableWidget_Read_DAQ->setCurrentCell(row - 1, 0, QItemSelectionModel::Select);
    }

}

void XcpSetDlg::on_pb_downPamDaq_released()
{
    QList<QTableWidgetItem*> selItems = ui->tableWidget_Read_DAQ->selectedItems();
    if(selItems.isEmpty()) return;

    QList<int> selRows;
    foreach (QTableWidgetItem *item, selItems) {
        if(item->column() != 0) continue;

        int row = item->row();
        if(selRows.indexOf(row) == -1)
        {
            selRows.append(row);
        }
    }

    std::sort(selRows.begin(), selRows.end(), std::greater<int>());

    foreach (int row, selRows) {
        if(row == (ui->tableWidget_Read_DAQ->rowCount() - 1)) continue;

        QTableWidgetItem *item0 = ui->tableWidget_Read_DAQ->takeItem(row, 0);
        QTableWidgetItem *item1 = ui->tableWidget_Read_DAQ->takeItem(row, 1);
        QTableWidgetItem *item2 = ui->tableWidget_Read_DAQ->takeItem(row, 2);
        QComboBox *cb = (QComboBox*)ui->tableWidget_Read_DAQ->cellWidget(row, 1);

        ui->tableWidget_Read_DAQ->insertRow(row + 2);
        ui->tableWidget_Read_DAQ->setItem(row + 2, 0, item0);
        ui->tableWidget_Read_DAQ->setItem(row + 2, 1, item1);
        ui->tableWidget_Read_DAQ->setItem(row + 2, 2, item2);
        ui->tableWidget_Read_DAQ->setCellWidget(row + 2, 1, cb);

        ui->tableWidget_Read_DAQ->removeRow(row);

        ui->tableWidget_Read_DAQ->setCurrentCell(row + 1, 0, QItemSelectionModel::Select);
    }
}

void XcpSetDlg::on_pb_upPamWrite_released()
{
    QList<QTableWidgetItem*> selItems = ui->tableWidget_Write->selectedItems();
    if(selItems.isEmpty()) return;

    QList<int> selRows;
    foreach (QTableWidgetItem *item, selItems) {
        if(item->column() != 0) continue;

        int row = item->row();
        if(selRows.indexOf(row) == -1)
        {
            selRows.append(row);
        }
    }

    std::sort(selRows.begin(), selRows.end());

    foreach (int row, selRows) {
        if(row == 0) continue;

        QTableWidgetItem *item0 = ui->tableWidget_Write->takeItem(row, 0);
        QTableWidgetItem *item1 = ui->tableWidget_Write->takeItem(row, 1);
        QTableWidgetItem *item2 = ui->tableWidget_Write->takeItem(row, 2);

        ui->tableWidget_Write->insertRow(row - 1);
        ui->tableWidget_Write->setItem(row - 1, 0, item0);
        ui->tableWidget_Write->setItem(row - 1, 1, item1);
        ui->tableWidget_Write->setItem(row - 1, 2, item2);

        ui->tableWidget_Write->removeRow(row + 1);

        ui->tableWidget_Write->setCurrentCell(row - 1, 0, QItemSelectionModel::Select);
    }
}

void XcpSetDlg::on_pb_downPamWrite_released()
{
    QList<QTableWidgetItem*> selItems = ui->tableWidget_Write->selectedItems();
    if(selItems.isEmpty()) return;

    QList<int> selRows;
    foreach (QTableWidgetItem *item, selItems) {
        if(item->column() != 0) continue;

        int row = item->row();
        if(selRows.indexOf(row) == -1)
        {
            selRows.append(row);
        }
    }

    std::sort(selRows.begin(), selRows.end(), std::greater<int>());

    foreach (int row, selRows) {
        if(row == (ui->tableWidget_Write->rowCount() - 1)) continue;

        QTableWidgetItem *item0 = ui->tableWidget_Write->takeItem(row, 0);
        QTableWidgetItem *item1 = ui->tableWidget_Write->takeItem(row, 1);
        QTableWidgetItem *item2 = ui->tableWidget_Write->takeItem(row, 2);

        ui->tableWidget_Write->insertRow(row + 2);
        ui->tableWidget_Write->setItem(row + 2, 0, item0);
        ui->tableWidget_Write->setItem(row + 2, 1, item1);
        ui->tableWidget_Write->setItem(row + 2, 2, item2);

        ui->tableWidget_Write->removeRow(row);

        ui->tableWidget_Write->setCurrentCell(row + 1, 0, QItemSelectionModel::Select);
    }
}

void XcpSetDlg::on_pb_importIncaPams_released()
{
    QFileDialog* fd = new QFileDialog(this);
    QString appPath = QApplication::applicationDirPath();
    QString filePath = fd->getOpenFileName(this, tr("Open File"),appPath, tr("Excel(*.xls *.xlsx)"));

    if(filePath.isEmpty()) return;

    QStringList pamNameList = importPamNamesFromIncaPamExcel(filePath);

    /*
    QProgressDialog dialog;
    dialog.setFont(QFont("Microsoft YaHei UI"));
    dialog.setLabelText(QString("Reading INCA pam file : progressing using %1 thread(s)...").arg(QThread::idealThreadCount()));

    QFutureWatcher<QString> futureWatcher;
    QObject::connect(&futureWatcher, &QFutureWatcher<void>::finished, &dialog, &QProgressDialog::reset);
    QObject::connect(&futureWatcher, &QFutureWatcher<void>::progressRangeChanged, &dialog, &QProgressDialog::setRange);
    QObject::connect(&futureWatcher, &QFutureWatcher<void>::progressValueChanged, &dialog, &QProgressDialog::setValue);

    futureWatcher.setFuture(QtConcurrent::mapped(pamNameList, std::bind(&XcpSetDlg::checkPamIsMeas, this, std::placeholders::_1)));
    dialog.exec();
    futureWatcher.waitForFinished();


    QStringList measPamNameList = futureWatcher.future().results();
    */

    QStringList measPamNameList, charPamNameList;
    qDebug()<<"************check pam is meas or char***************";
    for(int i = 0; i < pamNameList.count(); i++)
    {

        A2L_VarMeas *measVar = curA2LProj->getMeasVarByName(pamNameList.at(i));

        if(measVar)
        {
            measPamNameList.append(pamNameList.at(i));
            continue;
        }

        A2L_VarChar *charVar = curA2LProj->getCharVarByName(pamNameList.at(i));
        if(charVar)
        {
            charPamNameList.append(pamNameList.at(i));
            continue;
        }
    }

    Slt_MeasPamSelAcceptedDAQ(measPamNameList);

    Slt_CharPamSelAccepted(charPamNameList);
}

void XcpSetDlg::on_pb_calculateBusload_released()
{
    packPamInfoToConfig();

    QList<XcpPam> pamList = xcpConfig.daqPamList;

    quint64 busload_BitSum = 0;
    foreach (XcpPam pam, pamList) {
        QString pamName = pam.pamName;
        QString timeStr = pam.daqEventTime;
        quint32 time = timeStr.replace("ms", "").toUInt();

        A2L_VarMeas *measVar = curA2LProj->getMeasVarByName(pamName);
        if(measVar)
        {
            //qDebug()<<"DataSize:"<<measVar->DataSizeAG;
            busload_BitSum += (8 * measVar->DataSizeAG) * (1000 / time);
        }
    }

    busload_BitSum *= 8/7;
    float busload = (float)busload_BitSum / (float)xcpConfig.baudrate * 100;  // 估算总线负载率%
    qDebug()<< "估算总线负载率为:" << busload << "%";

    ui->dsb_busload->setValue(busload);
}
