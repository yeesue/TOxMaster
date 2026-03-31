#include "settingwin.h"
#include "ui_settingwin.h"
#include <QMenu>
#include <QMessageBox>
#include <QDir>

QStringList typeList = {"Polling_10ms", "Polling_20ms", "Polling_100ms"};

SettingWin::SettingWin(QWidget *parent, A2LProjectWin *a2lProjectWin, int index) :
    QDialog(parent), a2lWin(a2lProjectWin), setIndex(index),
    ui(new Ui::SettingWin)
{
    ui->setupUi(this);
    setWindowTitle("XCP Comm Setting");

    QStringList header_r, header_w;
    header_r << QString::fromLocal8Bit("参数名_读") << QString::fromLocal8Bit("类型") << QString::fromLocal8Bit("默认值");
    ui->tableWidget_Read->setHorizontalHeaderLabels(header_r);
    ui->tableWidget_Read->setColumnWidth(0, 300);
    ui->tableWidget_Read->setColumnWidth(1, 200);
    ui->tableWidget_Read->setColumnWidth(2, 100);
    ui->tableWidget_Read->setContextMenuPolicy(Qt::CustomContextMenu);


    header_w << QString::fromLocal8Bit("参数名_写") << QString::fromLocal8Bit("默认值") << QString::fromLocal8Bit("类型");
    ui->tableWidget_Write->setHorizontalHeaderLabels(header_w);
    ui->tableWidget_Write->setColumnWidth(0, 300);
    ui->tableWidget_Write->setColumnWidth(1, 200);
    ui->tableWidget_Write->setColumnWidth(2, 100);
    ui->tableWidget_Write->setContextMenuPolicy(Qt::CustomContextMenu);

    ui->tableWidget_Read_DAQ->setHorizontalHeaderLabels(header_r);
    ui->tableWidget_Read_DAQ->setColumnWidth(0, 300);
    ui->tableWidget_Read_DAQ->setColumnWidth(1, 200);
    ui->tableWidget_Read_DAQ->setColumnWidth(2, 100);
    ui->tableWidget_Read_DAQ->setContextMenuPolicy(Qt::CustomContextMenu);

    if(a2lWin)
        connect(a2lWin, &A2L_ProjectWin::winClosed, this, &SettingWin::projectWinClosedSlot);


    readSetting();
    loadAllA2LProject();

    connect(ui->cb_SetProject, &QComboBox::currentTextChanged, this, &SettingWin::projComboTextChangedSlot);
    connect(ui->cb_project, QOverload<const QString&>::of(&QComboBox::currentIndexChanged), this, &SettingWin::curProjectChangedSlot);

    connect(ui->tableWidget_Read, &QTableWidget::customContextMenuRequested, this, &SettingWin::showCustomContextMenuInRead);
    connect(ui->tableWidget_Write, &QTableWidget::customContextMenuRequested, this, &SettingWin::showCustomContextMenuInWrite);
    connect(ui->tableWidget_Read_DAQ, &QTableWidget::customContextMenuRequested, this, &SettingWin::showCustomContextMenuInReadDAQ);

    curProj = getProjByName(curProjName);
    if(curProj == NULL)
    {
        qDebug()<<"Current xcp project invalid.";
    }
    else
    {
        qDebug()<<"Current xcp project:"<<curProj->Proj_name;
        setCurrentProj(curProj);
    }
}

SettingWin::~SettingWin()
{

    delete ui;
}

void SettingWin::accept()
{
    on_pushButton_saveProj_clicked();

    writeSetting();

    QDialog::accept();
}

void SettingWin::showCustomContextMenuInRead(QPoint pos)
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

    connect(addAction, &QAction::triggered, this, &SettingWin::addSlotRead);
    connect(insertAction, &QAction::triggered, this, &SettingWin::insertSlotRead);
    connect(removeAction, &QAction::triggered, this, &SettingWin::removeSlotRead);

    menu->move(cursor().pos());
    menu->show();
}

void SettingWin::showCustomContextMenuInWrite(QPoint pos)
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

    connect(addAction, &QAction::triggered, this, &SettingWin::addSlotWrite);
    connect(insertAction, &QAction::triggered, this, &SettingWin::insertSlotWrite);
    connect(removeAction, &QAction::triggered, this, &SettingWin::removeSlotWrite);

    menu->move(cursor().pos());
    menu->show();
}

void SettingWin::showCustomContextMenuInReadDAQ(QPoint pos)
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

    connect(addAction, &QAction::triggered, this, &SettingWin::addSlotReadDAQ);
    connect(insertAction, &QAction::triggered, this, &SettingWin::insertSlotReadDAQ);
    connect(removeAction, &QAction::triggered, this, &SettingWin::removeSlotReadDAQ);

    menu->move(cursor().pos());
    menu->show();
}

void SettingWin::addSlotRead(bool)
{
    if(curA2LProj == NULL)
        return;

    PAMSELDlg *pamselDlg = new PAMSELDlg(this, curA2LProj);
    connect(pamselDlg, &PAMSELDlg::measPamSelAccepted, this, &SettingWin::measPamSelAcceptedSlot);
    pamselDlg->show();

    /*
    int curRow = ui->tableWidget_Read->rowCount();
    ui->tableWidget_Read->insertRow(curRow);

    QComboBox *cb_type = new QComboBox();
    cb_type->addItems(typeList);

    ui->tableWidget_Read->setItem(curRow, 0, new QTableWidgetItem("Param_R_"+QString::number(curRow)));
    ui->tableWidget_Read->setCellWidget(curRow, 1, cb_type);
    ui->tableWidget_Read->setItem(curRow, 2, new QTableWidgetItem(QString::number(0)));
    */
}

void SettingWin::insertSlotRead(bool)
{
    int curRow = ui->tableWidget_Read->currentRow();
    ui->tableWidget_Read->insertRow(curRow);

    QComboBox *cb_type = new QComboBox();
    cb_type->addItems(typeList);

    ui->tableWidget_Read->setItem(curRow, 0, new QTableWidgetItem("Param_R_"+QString::number(curRow)));
    ui->tableWidget_Read->setCellWidget(curRow, 1, cb_type);
    ui->tableWidget_Read->setItem(curRow, 2, new QTableWidgetItem(QString::number(0)));
}

void SettingWin::removeSlotRead(bool)
{
    int curRow = ui->tableWidget_Read->currentRow();
    ui->tableWidget_Read->removeRow(curRow);
}

void SettingWin::addSlotWrite(bool)
{
    if(curA2LProj == NULL)
        return;
    PAMSELDlg *pamselDlg = new PAMSELDlg(this, curA2LProj);
    connect(pamselDlg, &PAMSELDlg::charPamSelAccepted, this, &SettingWin::charPamSelAcceptedSlot);
    pamselDlg->show();

    /*
    int curRow = ui->tableWidget_Write->rowCount();
    ui->tableWidget_Write->insertRow(curRow);

    ui->tableWidget_Write->setItem(curRow, 0, new QTableWidgetItem("Param_W_"+QString::number(curRow)));
    ui->tableWidget_Write->setItem(curRow, 1, new QTableWidgetItem(QString::number(0)));
    */
}

void SettingWin::insertSlotWrite(bool)
{
    int curRow = ui->tableWidget_Write->currentRow();
    ui->tableWidget_Write->insertRow(curRow);

    ui->tableWidget_Write->setItem(curRow, 0, new QTableWidgetItem("Param_W_"+QString::number(curRow)));
    ui->tableWidget_Write->setItem(curRow, 1, new QTableWidgetItem(QString::number(0)));
}

void SettingWin::removeSlotWrite(bool)
{
    int curRow = ui->tableWidget_Write->currentRow();
    ui->tableWidget_Write->removeRow(curRow);
}

void SettingWin::editSlot(bool)
{

}

void SettingWin::addSlotReadDAQ(bool)
{
    if(curA2LProj == NULL)
        return;

    PAMSELDlg *pamselDlg = new PAMSELDlg(this, curA2LProj);
    connect(pamselDlg, &PAMSELDlg::measPamSelAccepted, this, &SettingWin::measPamSelAcceptedSlotDAQ);
    pamselDlg->show();
}

void SettingWin::insertSlotReadDAQ(bool)
{

}

void SettingWin::removeSlotReadDAQ(bool)
{
    int curRow = ui->tableWidget_Read_DAQ->currentRow();
    ui->tableWidget_Read_DAQ->removeRow(curRow);
}

void SettingWin::readSetting()
{
    QString appPath = QApplication::applicationDirPath();
    QSettings setting(appPath + "//setting//" + SET_FILENAME, QSettings::IniFormat);

    QString curProjName = setting.value("CurSetProject").toString();
    QString curProjName_2nd = setting.value("CurSetProject_2nd").toString();

    if(setIndex == 1)
    {
        this->curProjName = curProjName;
    }
    else if(setIndex == 2)
    {
        this->curProjName = curProjName_2nd;
    }

    //this->curProjName = curProjName;

    projList.clear();
    int size = setting.beginReadArray("SetProjectList");

    qDebug()<<"size="<<size;
    qDebug()<<setting.fileName();

    for (int i = 0; i < size; i++)
    {
        setting.setArrayIndex(i);

        Proj *proj = new Proj;
        proj->Proj_name = setting.value("SetProjectName").toString();
        proj->intfName = setting.value("Interface").toString();
        proj->a2lProjectName = setting.value("A2LProjectName").toString();
        proj->a2lFilePath = setting.value("A2LFilePath").toString();
        proj->hexFilePath = setting.value("HexFilePath").toString();

        proj->index_XcpOnCan = setting.value("Index_XcpOnCan").toInt();
        proj->baudrate = setting.value("Baudrate").toInt();
        proj->id_CMD = setting.value("ID_CMD").toInt();
        proj->id_RES = setting.value("ID_RES").toInt();

        ui->cb_SetProject->addItem(proj->Proj_name);

        // load readPams
        QList<XCP_PAM> pamList;
        QStringList pamNames, pamPollTypes, pamDefaultValues, caliTypes, pamDaqEventTypes;
        QString pamName = setting.value("readPamNames").toString();
        QString pamPollType = setting.value("readPamPollTypes").toString();
        QString pamDefaultValue = setting.value("readPamDefaultValues").toString();

        if (!pamName.isEmpty())
        {
            pamNames = pamName.split("|");
            pamPollTypes = pamPollType.split("|");
            pamDefaultValues = pamDefaultValue.split("|");
        }
        //qDebug()<<"read pams count:"<<pamNames.count()<<", default values count:"<<pamDefaultValues.count();
        for(int j = 0; j < pamNames.count(); j++)
        {
            XCP_PAM pam;
            pam.pamName = pamNames.at(j);
            pam.pollType = pamPollTypes.at(j);
            QString valueStr = pamDefaultValues.at(j);
            pam.pamDefaultValue = valueStr.toDouble();
            pamList.append(pam);
        }
        proj->readPamList = pamList;

        // load writePams
        pamList.clear();
        pamNames.clear();
        pamPollTypes.clear();
        pamDefaultValues.clear();
        QString caliType;

        pamName = setting.value("writePamNames").toString();
        pamDefaultValue = setting.value("writePamDefaultValues").toString();
        caliType = setting.value("writePamTypes").toString();

        if (!pamName.isEmpty())
        {
            pamNames = pamName.split("|");
            pamDefaultValues = pamDefaultValue.split("|");
            caliTypes = caliType.split("|");
        }
        //qDebug()<<"write pams count:"<<pamNames.count()<<", default values count:"<<pamDefaultValues.count();
        for(int j = 0; j < pamNames.count(); j++)
        {
            XCP_PAM pam;
            pam.pamName = pamNames.at(j);
            QString valueStr = pamDefaultValues.at(j);
            pam.pamDefaultValue = valueStr.toDouble();

            if(caliTypes.count() > j)
                pam.caliType = caliTypes.at(j);

            pamList.append(pam);
        }
        proj->writePamList = pamList;

        // load daqReadPams
        pamList.clear();
        pamNames.clear();
        pamDaqEventTypes.clear();
        pamDefaultValues.clear();
        pamName = setting.value("daqReadPamNames").toString();
        QString pamDaqEventType = setting.value("daqReadPamEventTypes").toString();
        pamDefaultValue = setting.value("daqReadPamDefaultValues").toString();

        if (!pamName.isEmpty())
        {
            pamNames = pamName.split("|");
            pamDaqEventTypes = pamDaqEventType.split("|");
            pamDefaultValues = pamDefaultValue.split("|");
        }

        for(int j = 0; j < pamNames.count(); j++)
        {
            XCP_PAM pam;
            pam.pamName = pamNames.at(j);
            pam.daqEventTime = pamDaqEventTypes.at(j);
            QString valueStr = pamDefaultValues.at(j);
            pam.pamDefaultValue = valueStr.toDouble();
            pamList.append(pam);
        }
        proj->daqReadPamList = pamList;

        projNameList.append(proj->Proj_name);
        projList.append(proj);

    }
    setting.endArray();

}

void SettingWin::writeSetting()
{
    QString appPath = QApplication::applicationDirPath();
    QSettings setting(appPath + "\\setting\\" + SET_FILENAME, QSettings::IniFormat);

    qDebug()<<"Write xcp comm setting:" + setting.fileName();
    qDebug()<<"Current proj:"<<ui->cb_SetProject->currentText();

    if(setIndex == 1)
    {
        setting.setValue("CurSetProject", ui->cb_SetProject->currentText());
    }
    else if(setIndex == 2)
    {
        setting.setValue("CurSetProject_2nd", ui->cb_SetProject->currentText());
    }
    //setting.setValue("CurSetProject", ui->cb_SetProject->currentText());

    if(projList.isEmpty())
        return;

    setting.beginWriteArray("SetProjectList");
    int projCount = projList.count();
    for (int i = 0; i < projCount; i++)
    {
        setting.setArrayIndex(i);

        Proj *proj = projList.at(i);

        setting.setValue("SetProjectName", proj->Proj_name);
        setting.setValue("Interface", proj->intfName);
        setting.setValue("A2LProjectName", proj->a2lProjectName);
        setting.setValue("A2LFilePath", proj->a2lFilePath);
        setting.setValue("HexFilePath", proj->hexFilePath);

        setting.setValue("Index_XcpOnCan", QString::number(proj->index_XcpOnCan));
        setting.setValue("Baudrate", QString::number(proj->baudrate));
        setting.setValue("ID_CMD", QString::number(proj->id_CMD));
        setting.setValue("ID_RES", QString::number(proj->id_RES));

        // read pams setting
        QList<XCP_PAM> pamList = proj->readPamList;
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
        setting.setValue("readPamNames", pamNames);
        setting.setValue("readPamPollTypes", pamPollTypes);
        setting.setValue("readPamDefaultValues", pamDefaultValues);

        //write pams setting
        pamList.clear();        
        pamList = proj->writePamList;
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
        setting.setValue("writePamNames", pamNames);
        setting.setValue("writePamDefaultValues", pamDefaultValues);
        setting.setValue("writePamTypes", pamCaliTypes);

        // daq read pams setting
        pamList = proj->daqReadPamList;
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
        setting.setValue("daqReadPamNames", pamNames);
        setting.setValue("daqReadPamEventTypes", pamDaqEventTypes);
        setting.setValue("daqReadPamDefaultValues", pamDefaultValues);

    }
    setting.endArray();

    qDebug()<<"Write xcp comm setting end.";

}

void SettingWin::projComboTextChangedSlot(QString text)
{
    qDebug()<<"project changed:"<<text;
    Proj *proj = getProjByName(text);
    setCurrentProj(proj);
}

void SettingWin::measPamSelAcceptedSlot(QStringList measPamNames)
{
    if(measPamNames.isEmpty())
        return;
    for(int i = 0; i < measPamNames.count(); i++)
    {
        int curRow = ui->tableWidget_Read->rowCount();
        ui->tableWidget_Read->insertRow(curRow);

        QComboBox *cb_type = new QComboBox();
        cb_type->addItems(typeList);

        ui->tableWidget_Read->setItem(curRow, 0, new QTableWidgetItem(measPamNames.at(i)));
        ui->tableWidget_Read->setCellWidget(curRow, 1, cb_type);
        ui->tableWidget_Read->setItem(curRow, 2, new QTableWidgetItem(QString::number(0)));
    }

}

void SettingWin::charPamSelAcceptedSlot(QStringList charPamNames)
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

        ui->tableWidget_Write->setItem(curRow, 0, new QTableWidgetItem(charPamNames.at(i)));
        ui->tableWidget_Write->setItem(curRow, 1, new QTableWidgetItem(QString::number(charVar->getValue(), 'g', 6)));
        ui->tableWidget_Write->setItem(curRow, 2, new QTableWidgetItem(charVar->Type));
    }
}

void SettingWin::measPamSelAcceptedSlotDAQ(QStringList measPamNames)
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

        ui->tableWidget_Read_DAQ->setItem(curRow, 0, new QTableWidgetItem(measPamNames.at(i)));
        ui->tableWidget_Read_DAQ->setCellWidget(curRow, 1, cb_type);
        ui->tableWidget_Read_DAQ->setItem(curRow, 2, new QTableWidgetItem(QString::number(0)));
    }
}

void SettingWin::loadAllA2LProject()
{
    if(projList.isEmpty() || a2lWin == NULL)
        return;

    foreach (Proj *proj, projList) {
        a2lWin->addA2LProject(proj->a2lFilePath, proj->hexFilePath);
    }

    ui->cb_project->clear();
    ui->cb_project->addItem("");
    ui->cb_project->addItems(a2lWin->getProjectNameList());

}

void SettingWin::setCurrentProj(Proj *proj)
{
    if (proj == NULL)
        return;

    on_pb_findDev_clicked();

    curProj = proj;

    ui->cb_SetProject->setCurrentText(proj->Proj_name);
    ui->lineEdit_projNew->setText(proj->Proj_name);

    ui->cb_Intf->setCurrentText(proj->intfName);

    ui->cb_project->setCurrentText(proj->a2lProjectName);
    ui->cb_XcpOnCan->setCurrentIndex(curProj->index_XcpOnCan);

    ui->tableWidget_Read->clearContents();
    ui->tableWidget_Write->clearContents();
    ui->tableWidget_Read_DAQ->clearContents();

    QList<XCP_PAM> pamList = curProj->readPamList;
    ui->tableWidget_Read->setRowCount(pamList.count());
    for(int i = 0; i < pamList.count(); i++)
    {
        XCP_PAM pam = pamList.at(i);
        QTableWidgetItem *nameItem = new QTableWidgetItem(pam.pamName);
        QComboBox *cb_type = new QComboBox();
        cb_type->addItems(typeList);
        cb_type->setCurrentText(pam.pollType);
        QTableWidgetItem *valueItem = new QTableWidgetItem(QString::number(pam.pamDefaultValue, 'g', 6));

        ui->tableWidget_Read->setItem(i, 0, nameItem);
        ui->tableWidget_Read->setCellWidget(i, 1, cb_type);
        ui->tableWidget_Read->setItem(i, 2, valueItem);
    }

    pamList = curProj->writePamList;
    ui->tableWidget_Write->setRowCount(pamList.count());
    for(int i = 0; i < pamList.count(); i++)
    {
        XCP_PAM pam = pamList.at(i);

        QTableWidgetItem *nameItem = new QTableWidgetItem(pam.pamName);
        QTableWidgetItem *valueItem = new QTableWidgetItem(QString::number(pam.pamDefaultValue, 'g', 6));
        QTableWidgetItem *typeItem = new QTableWidgetItem(pam.caliType);

        ui->tableWidget_Write->setItem(i, 0, nameItem);
        ui->tableWidget_Write->setItem(i, 1, valueItem);
        ui->tableWidget_Write->setItem(i, 2, typeItem);
    }

    pamList = curProj->daqReadPamList;
    ui->tableWidget_Read_DAQ->setRowCount(pamList.count());
    for(int i = 0; i < pamList.count(); i++)
    {
        XCP_PAM pam = pamList.at(i);
        QTableWidgetItem *nameItem = new QTableWidgetItem(pam.pamName);
        QComboBox *cb_type = new QComboBox();
        cb_type->addItems(daqEventList);
        cb_type->setCurrentText(pam.daqEventTime);
        QTableWidgetItem *valueItem = new QTableWidgetItem(QString::number(pam.pamDefaultValue, 'g', 6));

        ui->tableWidget_Read_DAQ->setItem(i, 0, nameItem);
        ui->tableWidget_Read_DAQ->setCellWidget(i, 1, cb_type);
        ui->tableWidget_Read_DAQ->setItem(i, 2, valueItem);
    }
}

Proj *SettingWin::getProjByName(QString name)
{
    if (projList.isEmpty())
        return NULL;

    foreach (Proj *proj, projList) {
        if (proj->Proj_name == name)
            return proj;
    }
    return NULL;
}

void SettingWin::projectWinClosedSlot()
{
    qDebug()<<"project win closed slot active.";
    ui->cb_project->clear();
    ui->cb_project->addItem("");
    ui->cb_project->addItems(a2lWin->getProjectNameList());

    ui->cb_project->setCurrentText("");

}

void SettingWin::curProjectChangedSlot(QString projectName)
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

    if(a2lProject == NULL)
        return;

    A2L_Daq *xcpDaqInfo = a2lProject->getDaqInfo();
    if (xcpDaqInfo == NULL)
        return;
    ui->le_DaqType->setText(xcpDaqInfo->DaqType);

    ui->cb_XcpOnCan->clear();
    ui->cb_XcpOnCan->addItems(a2lProject->getXcpOnCanInstanceNameList());
    ui->cb_Event->clear();
    ui->cb_Event->addItems(a2lProject->getEventNameList());

    ui->cb_EventTime->clear();
    ui->cb_EventTime->addItems(a2lProject->getEventTimeList());


    ui->cb_DaqList->clear();
    ui->cb_DaqList->addItems(a2lProject->getDaqListList());
    ui->cb_DaqListCanId->clear();
    ui->cb_DaqListCanId->addItems(a2lProject->getDaqListCanIdList());
    ui->cb_EventCanIdList->clear();
    ui->cb_EventCanIdList->addItems(a2lProject->getEventCanIdList());

    this->daqEventList = a2lProject->getEventTimeList();

    A2L_Xcp_On_Can *xcpOnCanInfo = a2lProject->getXcpOnCanInfo(ui->cb_XcpOnCan->currentIndex());
    if (xcpOnCanInfo == NULL)
        return;
    ui->sb_CMD_ID->setValue(xcpOnCanInfo->CAN_ID_MASTER);
    ui->sb_RES_ID->setValue(xcpOnCanInfo->CAN_ID_SLAVE);
    if (xcpOnCanInfo->BAUDRATE == 500000)
        ui->cb_Baudrate->setCurrentText("500K");
    else if (xcpOnCanInfo->BAUDRATE == 1000000)
        ui->cb_Baudrate->setCurrentText("1M");
    ui->sb_DAQMBL->setValue(xcpOnCanInfo->MAX_BUS_LOAD);

    A2L_Daq_Memory_Consumption *daqMemory = a2lProject->getDaqMemoryConsumption();
    if (daqMemory != NULL)
    {
        ui->sb_DAQMEM->setValue(daqMemory->Daq_Memory_Limit);
    }

}

void SettingWin::on_pushButton_add_clicked()
{
    QString newProjName = ui->lineEdit_projNew->text();
    if (newProjName.isEmpty())
        return;
    if (projNameList.indexOf(newProjName) != -1)
    {
        ui->cb_SetProject->setCurrentText(newProjName);
        return;
    }

    Proj* proj = new Proj;
    proj->Proj_name = newProjName;
    proj->a2lProjectName = "";
    proj->a2lFilePath = "";
    proj->hexFilePath = "";

    ui->cb_SetProject->addItem(proj->Proj_name);
    ui->cb_SetProject->setCurrentText(proj->Proj_name);

    curProj = proj;

    projNameList.append(newProjName);
    projList.append(proj);

    setCurrentProj(proj);

}


void SettingWin::on_pushButton_saveProj_clicked()
{
    if(curProj == NULL || curA2LProj == NULL)
        return;

    curProj->intfName = ui->cb_Intf->currentText();
    curProj->a2lProjectName = ui->cb_project->currentText();
    curProj->a2lFilePath = curA2LProj->getA2LFileName();
    curProj->hexFilePath = curA2LProj->getA2LRefHexFileName();

    curProj->index_XcpOnCan = ui->cb_XcpOnCan->currentIndex();
    A2L_Xcp_On_Can *xcpOnCanInfo = curA2LProj->getXcpOnCanInfo(ui->cb_XcpOnCan->currentIndex());
    if (xcpOnCanInfo != NULL)
    {
        curProj->baudrate = xcpOnCanInfo->BAUDRATE;
        curProj->id_CMD = xcpOnCanInfo->CAN_ID_MASTER;
        curProj->id_RES = xcpOnCanInfo->CAN_ID_SLAVE;
    }

    QList<XCP_PAM> pamList;
    int rowCount_R = ui->tableWidget_Read->rowCount();
    for (int i = 0; i < rowCount_R; i++)
    {
        XCP_PAM pam;
        pam.pamName = ui->tableWidget_Read->item(i,0)->text();
        pam.pollType = ((QComboBox*)ui->tableWidget_Read->cellWidget(i,1))->currentText();
        pam.pamDefaultValue = ui->tableWidget_Read->item(i, 2)->text().toDouble();

        pamList.append(pam);
    }
    curProj->readPamList = pamList;

    pamList.clear();
    int rowCount_W = ui->tableWidget_Write->rowCount();
    for (int i = 0; i < rowCount_W; i++)
    {
        XCP_PAM pam;
        pam.pamName = ui->tableWidget_Write->item(i,0)->text();
        pam.pamDefaultValue = ui->tableWidget_Write->item(i, 1)->text().toDouble();
        pam.caliType = ui->tableWidget_Write->item(i, 2)->text();
        pamList.append(pam);
    }
    curProj->writePamList = pamList;

    pamList.clear();
    int rowCount_R_DAQ = ui->tableWidget_Read_DAQ->rowCount();
    for (int i = 0; i < rowCount_R_DAQ; i++)
    {
        XCP_PAM pam;
        pam.pamName = ui->tableWidget_Read_DAQ->item(i,0)->text();
        pam.daqEventTime = ((QComboBox*)ui->tableWidget_Read_DAQ->cellWidget(i,1))->currentText();
        pam.daqEventIndex = ((QComboBox*)ui->tableWidget_Read_DAQ->cellWidget(i,1))->currentIndex();
        pam.pamDefaultValue = ui->tableWidget_Read_DAQ->item(i, 2)->text().toDouble();

        pamList.append(pam);
    }
    curProj->daqReadPamList = pamList;
}

void SettingWin::on_pb_findDev_clicked()
{
    XCP_Thread *xcpThread = new XCP_Thread;
    QStringList intfList = xcpThread->getCANInterface();

    ui->cb_Intf->clear();
    ui->cb_Intf->addItems(intfList);

}

void SettingWin::on_pb_projectManage_clicked()
{
    if(a2lWin == NULL)
    {
        a2lWin = new A2LProjectWin(this);
        connect(a2lWin, &A2L_ProjectWin::winClosed, this, &SettingWin::projectWinClosedSlot);
    }

    a2lWin->show();
}
