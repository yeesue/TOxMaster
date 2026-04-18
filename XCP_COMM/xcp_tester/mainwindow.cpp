#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QInputDialog>
#include <QDir>
#include <QMessageBox>
#include <QCloseEvent>
#include "xcp_error.h"
#include "../common/memory_manager.h"
#include "../win/plot3dwin.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    setWindowTitle("XCP_COMM");

    importExportService = new ImportExportService(this);
    uiStatePresenter = new UiStatePresenter(this);
    recordService = new RecordService(this);

    mainWindowContext.project = &curProj;
    mainWindowContext.currentA2lProject = curA2LProject;
    mainWindowContext.logWin = logWin;
    mainWindowContext.mdfFileName = &mdfFileName;
    mainWindowContext.recordOn = &recordOn;
    mainWindowContext.recordRateMs = &recordRate_ms;

    recordService->setContext(&mainWindowContext);
    recordService->setMeasToPamConverter(
        [this](const QList<A2L_VarMeas*> &measPamList, quint32 &blockSize) {
            return fromMeasToPams(measPamList, blockSize);
        });
    connect(recordService, &RecordService::recordTimeUpdated, this, &MainWindow::showRecordTimeInTimeEdit);

    //log
    logWin = new LogWin(this, false);
    mainWindowContext.logWin = logWin;

    ui->le_Log->setText("Initialization");

    //led indicator
    setLED(ui->led_CAN, 0, LED_SIZE);
    setLED(ui->led_XCP, 0, LED_SIZE);
    setLED(ui->led_Polling, 0, LED_SIZE);
    setLED(ui->led_Cali, 0, LED_SIZE);
    setLED(ui->led_Daq, 0, LED_SIZE);
    setLED(ui->led_Ext, 0, LED_SIZE);

    ui->le_Log_2->setText("Initialization");

    //led indicator 2
    setLED(ui->led_CAN_2, 0, LED_SIZE);
    setLED(ui->led_XCP_2, 0, LED_SIZE);
    setLED(ui->led_Polling_2, 0, LED_SIZE);
    setLED(ui->led_Cali_2, 0, LED_SIZE);
    setLED(ui->led_Daq_2, 0, LED_SIZE);
    setLED(ui->led_Ext_2, 0, LED_SIZE);

    //progressbar
    progBar = new QProgressBar(this);
    statusBar()->addWidget(progBar, 1);
    statusBar()->hide();

    // 初始化工作线程
    workerThread = new WorkerThread(this);
    connect(workerThread, &WorkerThread::taskStarted, this, &MainWindow::onWorkerTaskStarted);
    connect(workerThread, &WorkerThread::taskFinished, this, &MainWindow::onWorkerTaskFinished);
    connect(workerThread, &WorkerThread::progressUpdated, this, &MainWindow::onWorkerProgressUpdated);

    // 初始化数据更新器
    dataUpdater = new DataUpdater(this);
    dataUpdater_2nd = new DataUpdater(this);
    
    // 连接数据更新器的信号到槽函数
    connect(dataUpdater, &DataUpdater::pollReadTimeUpdated, this, &MainWindow::onPollReadTimeUpdated);
    connect(dataUpdater, &DataUpdater::daqReadTimeUpdated, this, &MainWindow::onDaqReadTimeUpdated);
    connect(dataUpdater, &DataUpdater::caliWriteTimeUpdated, this, &MainWindow::onCaliWriteTimeUpdated);
    
    // 添加3D数据可视化菜单项
    QMenu *viewMenu = menuBar()->addMenu("View");
    QAction *plot3DAction = viewMenu->addAction("3D Data Visualization");
    connect(plot3DAction, &QAction::triggered, this, &MainWindow::show3DDataVisualization);
    connect(dataUpdater, &DataUpdater::measVarsUpdated, this, &MainWindow::onMeasVarsUpdated);
    connect(dataUpdater, &DataUpdater::daqMeasVarsUpdated, this, &MainWindow::onDaqMeasVarsUpdated);
    
    connect(dataUpdater_2nd, &DataUpdater::pollReadTimeUpdated, this, &MainWindow::onPollReadTimeUpdated_2nd);
    connect(dataUpdater_2nd, &DataUpdater::daqReadTimeUpdated, this, &MainWindow::onDaqReadTimeUpdated_2nd);
    connect(dataUpdater_2nd, &DataUpdater::caliWriteTimeUpdated, this, &MainWindow::onCaliWriteTimeUpdated_2nd);
    connect(dataUpdater_2nd, &DataUpdater::measVarsUpdated, this, &MainWindow::onMeasVarsUpdated_2nd);
    connect(dataUpdater_2nd, &DataUpdater::daqMeasVarsUpdated, this, &MainWindow::onDaqMeasVarsUpdated_2nd);

    QStringList header_r, header_w, header_r_daq;
    header_r << QString::fromLocal8Bit("Pam_Polling") << QString::fromLocal8Bit("Value");
    ui->tableWidget_Read->setHorizontalHeaderLabels(header_r);
    ui->tableWidget_Read->setColumnWidth(0, 300);
    ui->tableWidget_Read->setColumnWidth(1, 150);

    ui->tableWidget_Read_2->setHorizontalHeaderLabels(header_r);
    ui->tableWidget_Read_2->setColumnWidth(0, 300);
    ui->tableWidget_Read_2->setColumnWidth(1, 150);

    header_r_daq << QString::fromLocal8Bit("Pam_DAQ") << QString::fromLocal8Bit("Value");
    ui->tableWidget_Read_DAQ->setHorizontalHeaderLabels(header_r_daq);
    ui->tableWidget_Read_DAQ->setColumnWidth(0, 300);
    ui->tableWidget_Read_DAQ->setColumnWidth(1, 150);

    ui->tableWidget_Read_DAQ_2->setHorizontalHeaderLabels(header_r_daq);
    ui->tableWidget_Read_DAQ_2->setColumnWidth(0, 300);
    ui->tableWidget_Read_DAQ_2->setColumnWidth(1, 150);

    header_w << QString::fromLocal8Bit("Pam_Calibrate") << QString::fromLocal8Bit("Value");
    ui->tableWidget_Write->setHorizontalHeaderLabels(header_w);
    ui->tableWidget_Write->setColumnWidth(0, 300);
    ui->tableWidget_Write->setColumnWidth(1, 150);

    ui->tableWidget_Write_2->setHorizontalHeaderLabels(header_w);
    ui->tableWidget_Write_2->setColumnWidth(0, 300);
    ui->tableWidget_Write_2->setColumnWidth(1, 150);

    DoubleSpinBoxDelegate *dsbDelegate = new DoubleSpinBoxDelegate(this);
    ui->tableWidget_Write->setItemDelegate(dsbDelegate);
    //connect(dsbDelegate, SIGNAL(modelDataUpdated(int,double)), this, SLOT(modelDataUpdatedSlot(int,double)));
    connect(dsbDelegate, &DoubleSpinBoxDelegate::modelDataUpdated, this, &MainWindow::sltModelDataUpdated);

    ui->tableWidget_Write->setEditTriggers(QAbstractItemView::DoubleClicked);

    DoubleSpinBoxDelegate *dsbDelegate_2nd = new DoubleSpinBoxDelegate(this);
    ui->tableWidget_Write_2->setItemDelegate(dsbDelegate_2nd);
    connect(dsbDelegate_2nd, &DoubleSpinBoxDelegate::modelDataUpdated, this, &MainWindow::sltModelDataUpdated_2nd);

    ui->tableWidget_Write_2->setEditTriggers(QAbstractItemView::DoubleClicked);

    // a2l Map window
    mapWin = new MapWin(this);
    mapWin_2nd = new MapWin(this);

    //a2l project window
    a2lWin = new A2LProjectWin(this);

    ui->actionLoad->setVisible(showActions);
    ui->actionConnect->setVisible(showActions);
    ui->actionDisconnect->setVisible(showActions);
    ui->actionMap->setVisible(showActions);
    ui->actionRaw->setVisible(showActions);
    ui->actionRecord->setVisible(showActions);
    ui->actionSetting->setVisible(showActions);
    ui->actionStop->setVisible(showActions);
    ui->actionVisual->setVisible(showActions);
    ui->actionA2l->setVisible(showActions);
    ui->actionSaveToExcel->setVisible(showActions);
    ui->actionFromExcel->setVisible(showActions);
    ui->actiontoDB->setVisible(showActions);

    ui->tableWidget_Read->setVisible(true);
    ui->tableWidget_Write->setVisible(true);
    ui->tableWidget_Read_DAQ->setVisible(true);

    ui->tableWidget_Read_2->setVisible(true);
    ui->tableWidget_Write_2->setVisible(true);
    ui->tableWidget_Read_DAQ_2->setVisible(true);

    updateTimer = new QTimer(this);

    setMinimumHeight(200);
    resize(QSize(1500, 900));

    if(autoStart)
    {
        startXCP();
    }

    on_actionShow_triggered();
}

MainWindow::~MainWindow()
{
    if (recordService) {
        recordService->dispose();
    }

    if(process)
    {
        process->terminate();
        process->kill();
        delete process;
    }
    killModrive();

    // 不再需要手动删除smRead和smWrite，因为使用了MemoryManager
    // delete smRead;
    // delete smWrite;

    // 清理工作线程
    if (workerThread) {
        workerThread->quit();
        workerThread->wait();
        delete workerThread;
    }
    
    // 清理数据更新器
    delete dataUpdater;
    delete dataUpdater_2nd;

    delete ui;
}

void MainWindow::loadCurrentSetting()
{
    readCurSetting();
    mainWindowContext.project = &curProj;

    setWindowTitle(curProj.Proj_name);

    curA2LProject = a2lWin->getProjectByName(curProj.a2lProjectName);
    mainWindowContext.currentA2lProject = curA2LProject;
    getA2LPamsByNames();
    showMeasAndCharsInTable();
}

void MainWindow::loadCurrentSetting_2nd()
{
    readCurSetting_2nd();

    curA2LProject_2nd = a2lWin->getProjectByName(curProj_2nd.a2lProjectName);
    getA2LPamsByNames_2nd();
    showMeasAndCharsInTable_2nd();
}

/*
void MainWindow::configDevice()
{
    readCurSetting();

    setWindowTitle(curProj.Proj_name);

    curA2LProject = a2lWin->getProjectByName(curProj.a2lProjectName);

    getA2LPamsByNames();

    showMeasAndCharsInTable();

    xcpMaster = new XCPMaster(this);
    xcpPollThread = new XCP_Polling_Thread(this, xcpMaster);
    charPamCheckThread = new CharPamCheck(this, xcpMaster);
    mapCharPamCheckThread = new MapCharPamCheckThread(this, xcpMaster);

    if(mdfRecordIns == NULL)
    {
        mdfRecordIns = new MDF_Record_Thread();
        mdfRecordIns->setPollPams(transferMeasToPams(measPamList));
        mdfRecordIns->setPollDataBlockSize(sizeRead+8);
        mdfRecordIns->setRecordFileName(curProj.Proj_name);

        QThread *recordThread = new QThread();
        mdfRecordIns->moveToThread(recordThread);
        recordThread->start();
    }

    //connect(xcpPollThread, SIGNAL(measPamsValueUpdated()), this, SLOT(fromReadSMToMeasVars()));
    connect(xcpPollThread, &XCP_Polling_Thread::pollingStatusChanged, this, &MainWindow::pollingStatusChangedSlot);
    connect(xcpPollThread, &XCP_Polling_Thread::caliStatusChanged, this, &MainWindow::caliStatusChangedSlot);

    //bool conFlag = connect(this, SIGNAL(mdfRecordDataUpdated(char*,int)), mdfRecordIns, SLOT(mdf_record_writeRawData(char*, int)));

    bool conFlag = connect(mdfRecordIns, &MDF_Record_Thread::recordTime, this, &MainWindow::showRecordTimeInTimeEdit);

}

void MainWindow::configDevice_2nd()
{
    if(a2lWin == NULL)
        a2lWin = new A2LProjectWin(this);

    readSetting_Simple();

    setWindowTitle("XCP_Trans");

    if(xcpMaster == NULL)
        xcpMaster = new XCPMaster(this);

    if(xcpPollThread == NULL)
        xcpPollThread = new XCP_Polling_Thread(this, xcpMaster);
    if(charPamCheckThread == NULL)
        charPamCheckThread = new CharPamCheck(this, xcpMaster);

    connect(xcpPollThread, &XCP_Polling_Thread::measPamsValueUpdated, this, &MainWindow::fromReadSMToMeasVars);
    connect(xcpPollThread, &XCP_Polling_Thread::pollingStatusChanged, this, &MainWindow::pollingStatusChangedSlot);
    connect(xcpPollThread, &XCP_Polling_Thread::caliStatusChanged, this, &MainWindow::caliStatusChangedSlot);

}
*/

void MainWindow::runDevice()
{
    on_actionConnect_triggered();
}

void MainWindow::stopDevice()
{
    on_actionDisconnect_triggered();
}

void MainWindow::startXCP()
{
    on_actionXcpOff_triggered();
    on_actionFromExcel_triggered();
    on_actionXcpOn_triggered();
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    int result = QMessageBox::warning(this,
                         QString::fromLocal8Bit("Shut Down"),
                         QString::fromLocal8Bit("Confirm to shut down?\n "
                            "Make sure no xcp session is running"),
                         QMessageBox::Ok | QMessageBox::Cancel, QMessageBox::NoButton
                         );
    if (result == QMessageBox::Cancel)
    {
        event->ignore();
    }
    else
    {
        event->accept();

        on_actionXcpOff_triggered();

        QMainWindow::closeEvent(event);
    }
}

void MainWindow::sltXcpMsg(QString msg)
{
    ui->le_Log->setText(msg);
}

void MainWindow::sltXcpCanInitStatus(int status)
{
    setLED(ui->led_CAN, status, LED_SIZE);
}

void MainWindow::sltXcpConnStatus(int status)
{
    setLED(ui->led_XCP, status, LED_SIZE);
}

void MainWindow::sltXcpCaliStatus(int status)
{
    setLED(ui->led_Cali, status, LED_SIZE);
}

void MainWindow::sltXcpPollingStatus(int status)
{
    setLED(ui->led_Polling, status, LED_SIZE);
}

void MainWindow::sltXcpDaqRunStatus(int status)
{
    setLED(ui->led_Daq, status, LED_SIZE);
}

void MainWindow::sltXcpMsg_2nd(QString msg)
{
    ui->le_Log_2->setText(msg);
}

void MainWindow::sltXcpCanInitStatus_2nd(int status)
{
    setLED(ui->led_CAN_2, status, LED_SIZE);
}

void MainWindow::sltXcpConnStatus_2nd(int status)
{
    setLED(ui->led_XCP_2, status, LED_SIZE);
}

void MainWindow::sltXcpCaliStatus_2nd(int status)
{
    setLED(ui->led_Cali_2, status, LED_SIZE);
}

void MainWindow::sltXcpPollingStatus_2nd(int status)
{
    setLED(ui->led_Polling_2, status, LED_SIZE);
}

void MainWindow::sltXcpDaqRunStatus_2nd(int status)
{
    setLED(ui->led_Daq_2, status, LED_SIZE);
}

void MainWindow::updateCharValueInSM(A2L_VarChar *charVar, qreal value)
{
    int startByte = charVar->StartByte;
    quint32 dataSize = charVar->DataSizeAG;
    QString type = charVar->DataType;

    qreal intValue = 0;
    if (charVar->ConversionType == "RAT_FUNC")
    {
         intValue = xcpMaster->RatFuncConvFromPhysToInt(value, charVar->CoefList);
    }
    else if (charVar->ConversionType == "TAB_VERB")
    {
        intValue = value;
    }
    else
    {
        intValue = value;
    }

    CharArrayPtr data = makeCharArray(dataSize);
    switch (dataSize) {
    case 1:
    {
        if(type == "UBYTE")
            *(quint8*)(data.data()) = (quint8)intValue;
        else if(type == "SBYTE")
            *(qint8*)(data.data()) = (qint8)intValue;
        break;
    }
    case 2:
    {
        if(type == "UWORD")
            *(quint16*)(data.data()) = (quint16)intValue;
        else if(type == "SWORD")
            *(qint16*)(data.data()) = (qint16)intValue;

        break;
    }
    case 4:
    {
        if(type == "ULONG")
            *(quint32*)(data.data()) = (quint32)intValue;
        else if(type == "SLONG")
            *(qint32*)(data.data()) = (qint32)intValue;
        else if(type == "FLOAT32_IEEE")
            *(float*)(data.data()) = (float)intValue;
        break;
    }
    case 8:
    {
        if(type == "A_UINT64")
            *(quint64*)(data.data()) = (quint64)intValue;
        else if(type == "A_INT64")
            *(qint64*)(data.data()) = (qint64)intValue;
        else if(type == "FLOAT64_IEEE")
            *(qreal*)(data.data()) = (qreal)intValue;

        break;
    }
    default:
        break;
    }

    // 使用MemoryManager替代QSharedMemory
    QString key = "WP_" + curProj.Proj_name;
    void* memory = MemoryManager::instance()->getMemory(key);
    if(!memory)
        return;

    // 直接写入内存数据，不需要锁定，因为MemoryManager内部已经处理了线程安全
    memcpy((char*)memory + 8 + startByte, data.data(), dataSize);

}

void MainWindow::modelDataUpdatedSlot(int rowIndex, double value)
{
    //qDebug()<<"data update active: index="<<rowIndex<<", value="<<value;

    if(extCal)
        return;

    A2L_VarChar *charVar = charPamList.at(rowIndex);
    if(charVar != NULL)
    {
        charVar->setValue(value);
        updateCharValueInSM(charVar, value);
    }
}

void MainWindow::sltModelDataUpdated(int rowIndex, double value)
{
    if(!xcpMainThread)
        return;

    A2L_VarChar *charVar = charPamList.at(rowIndex);
    if(charVar != NULL)
    {
        charVar->setValue(value);
        xcpMainThread->updateCharValueInSM(charVar, value);
    }
}

void MainWindow::modelDataUpdatedSlot_2nd(int rowIndex, double value)
{

}

void MainWindow::sltModelDataUpdated_2nd(int rowIndex, double value)
{
    if(!xcpMainThread_2nd)
        return;

    A2L_VarChar *charVar = charPamList_2nd.at(rowIndex);
    if(charVar != NULL)
    {
        charVar->setValue(value);
        xcpMainThread_2nd->updateCharValueInSM(charVar, value);
    }
}

void MainWindow::readSetting_Simple()
{
    QString appPath = QApplication::applicationDirPath();
    QSettings setting(appPath + "/setting/" + SET_FILENAME_S, QSettings::IniFormat);

    curProj.Proj_name = setting.value("SetProjectName").toString();
    curProj.intfName = setting.value("Interface").toString();
    curProj.index_XcpOnCan = setting.value("Index_XcpOnCan").toInt();
    curProj.baudrate = setting.value("Baudrate").toInt();
    curProj.id_CMD = setting.value("ID_CMD").toInt();
    curProj.id_RES = setting.value("ID_RES").toInt();

    qDeleteAll(eventCanIdList);
    eventCanIdList.clear();
    qDeleteAll(eventList);
    eventList.clear();

    int eventCanIdCount = setting.beginReadArray("XcpEventCanIdList");
    for (int i = 0; i < eventCanIdCount; i++)
    {
        setting.setArrayIndex(i);
        A2L_EventCanId *eventCanId = new A2L_EventCanId();
        eventCanId->Name = setting.value("EventCanIdName").toString();
        eventCanId->EventNumber = (quint8)(setting.value("EventNumber").toUInt());
        eventCanId->Event_CAN_ID = (quint32)(setting.value("EventCanId").toUInt());

        eventCanIdList.append(eventCanId);
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

        eventList.append(event);
    }
    setting.endArray();

    smKeyRead = "RP_POLL_" + curProj.Proj_name;
    smKeyWrite = "WP_" + curProj.Proj_name;

}

void MainWindow::writeSetting_Simple()
{
    if(!curA2LProject) return;

    QString appPath = QApplication::applicationDirPath();
    QSettings setting(appPath + "/setting/" + SET_FILENAME_S, QSettings::IniFormat);

    qDebug()<<"*****Write xcp setting:" + setting.fileName();

    qDebug()<<"Current proj:"<<curProj.Proj_name;

    setting.setValue("SetProjectName", curProj.Proj_name);
    setting.setValue("Interface", curProj.intfName);
    setting.setValue("A2LProjectName", curProj.a2lProjectName);

    setting.setValue("Index_XcpOnCan", QString::number(curProj.index_XcpOnCan));
    setting.setValue("Baudrate", QString::number(curProj.baudrate));
    setting.setValue("ID_CMD", QString::number(curProj.id_CMD));
    setting.setValue("ID_RES", QString::number(curProj.id_RES));

    setting.beginWriteArray("XcpEventList");
    QList<A2L_Event*> eventList = curA2LProject->getEventList();
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

    setting.beginWriteArray("XcpEventCanIdList");
    QList<A2L_EventCanId*> eventCanIdList = curA2LProject->getA2lEventCanIdList();
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

    qDebug()<<"Write xcp setting end.*****";
}

void MainWindow::readSetting_Simple_2nd()
{
    QString appPath = QApplication::applicationDirPath();
    QSettings setting(appPath + "/setting/" + SET_FILENAME_S_2ND, QSettings::IniFormat);

    curProj_2nd.Proj_name = setting.value("SetProjectName").toString();
    curProj_2nd.intfName = setting.value("Interface").toString();
    curProj_2nd.index_XcpOnCan = setting.value("Index_XcpOnCan").toInt();
    curProj_2nd.baudrate = setting.value("Baudrate").toInt();
    curProj_2nd.id_CMD = setting.value("ID_CMD").toInt();
    curProj_2nd.id_RES = setting.value("ID_RES").toInt();

    qDeleteAll(eventCanIdList_2nd);
    eventCanIdList_2nd.clear();
    qDeleteAll(eventList_2nd);
    eventList_2nd.clear();

    int eventCanIdCount = setting.beginReadArray("XcpEventCanIdList");
    for (int i = 0; i < eventCanIdCount; i++)
    {
        setting.setArrayIndex(i);
        A2L_EventCanId *eventCanId = new A2L_EventCanId();
        eventCanId->Name = setting.value("EventCanIdName").toString();
        eventCanId->EventNumber = (quint8)(setting.value("EventNumber").toUInt());
        eventCanId->Event_CAN_ID = (quint32)(setting.value("EventCanId").toUInt());

        eventCanIdList_2nd.append(eventCanId);
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

        eventList_2nd.append(event);
    }
    setting.endArray();

    smKeyRead_2nd = "RP_POLL_" + curProj_2nd.Proj_name;
    smKeyWrite_2nd = "WP_" + curProj_2nd.Proj_name;
}

void MainWindow::writeSetting_Simple_2nd()
{
    if(!curA2LProject_2nd) return;

    QString appPath = QApplication::applicationDirPath();
    QSettings setting(appPath + "\\setting\\" + SET_FILENAME_S_2ND, QSettings::IniFormat);

    qDebug()<<"*****Write 2nd xcp setting:" + setting.fileName();

    qDebug()<<"Current proj:"<<curProj_2nd.Proj_name;

    setting.setValue("SetProjectName", curProj_2nd.Proj_name);
    setting.setValue("Interface", curProj_2nd.intfName);
    setting.setValue("A2LProjectName", curProj_2nd.a2lProjectName);

    setting.setValue("Index_XcpOnCan", QString::number(curProj_2nd.index_XcpOnCan));
    setting.setValue("Baudrate", QString::number(curProj_2nd.baudrate));
    setting.setValue("ID_CMD", QString::number(curProj_2nd.id_CMD));
    setting.setValue("ID_RES", QString::number(curProj_2nd.id_RES));

    setting.beginWriteArray("XcpEventList");
    QList<A2L_Event*> eventList = curA2LProject_2nd->getEventList();
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

    setting.beginWriteArray("XcpEventCanIdList");
    QList<A2L_EventCanId*> eventCanIdList = curA2LProject_2nd->getA2lEventCanIdList();
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

    qDebug()<<"Write 2nd xcp setting end.*****";
}

void MainWindow::readCurSetting()
{
    QString appPath = QApplication::applicationDirPath();
    QSettings setting(appPath + "//setting//" + SET_FILENAME, QSettings::IniFormat);


    QString curProjName = setting.value("CurSetProject").toString();

    int sizeBitRead = 0;
    int sizeBitWrite = 0;

    int size = setting.beginReadArray("SetProjectList");
    for (int i = 0; i < size; i++)
    {
        setting.setArrayIndex(i);
        Proj proj;
        proj.Proj_name = setting.value("SetProjectName").toString();
        proj.intfName = setting.value("Interface").toString();
        proj.a2lProjectName = setting.value("A2LProjectName").toString();
        proj.a2lFilePath = setting.value("A2LFilePath").toString();
        proj.hexFilePath = setting.value("HexFilePath").toString();
        proj.index_XcpOnCan = setting.value("Index_XcpOnCan").toInt();
        proj.baudrate = setting.value("Baudrate").toInt();
        proj.id_CMD = setting.value("ID_CMD").toInt();
        proj.id_RES = setting.value("ID_RES").toInt();

        sizeBitRead = 0;
        sizeBitWrite = 0;

        // load readPams
        QList<XCP_PAM> pamList;

        QStringList pamNames, pamPollTypes, pamDefaultValues, pamCaliTypes, pamDaqEventTypes;
        QString pamName = setting.value("readPamNames").toString();
        QString pamPollType = setting.value("readPamPollTypes").toString();
        QString pamDefaultValue = setting.value("readPamDefaultValues").toString();

        if (!pamName.isEmpty())
        {
            pamNames = pamName.split("|");
            pamPollTypes = pamPollType.split("|");
            pamDefaultValues = pamDefaultValue.split("|");
        }
        for(int j = 0; j < pamNames.count(); j++)
        {
            XCP_PAM pam;
            pam.pamName = pamNames.at(j);
            pam.pollType = pamPollTypes.at(j);
            QString valueStr = pamDefaultValues.at(j);
            pam.pamDefaultValue = valueStr.toDouble();
            pamList.append(pam);

            int lenBit = 0;
            sizeBitRead += lenBit;
        }
        proj.readPamList = pamList;


        // load writePams
        pamList.clear();
        pamNames.clear();
        pamDefaultValues.clear();

        QString pamCaliType;

        pamName = setting.value("writePamNames").toString();
        pamDefaultValue = setting.value("writePamDefaultValues").toString();
        pamCaliType = setting.value("writePamTypes").toString();

        if (!pamName.isEmpty())
        {
            pamNames = pamName.split("|");
            pamDefaultValues = pamDefaultValue.split("|");
            pamCaliTypes = pamCaliType.split("|");
        }
        for(int j = 0; j < pamNames.count(); j++)
        {
            XCP_PAM pam;
            pam.pamName = pamNames.at(j);
            QString valueStr = pamDefaultValues.at(j);
            pam.pamDefaultValue = valueStr.toDouble();
            pam.caliType = pamCaliTypes.at(j);

            pamList.append(pam);

            int lenBit = 0;
            sizeBitWrite += lenBit;
        }
        proj.writePamList = pamList;

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
        proj.daqReadPamList = pamList;

        if (curProjName == proj.Proj_name)
        {
            curProj = proj;
        }

        if(!a2lWin->getProjectByName(proj.a2lProjectName))
        {
            a2lWin->addA2LProject(proj.a2lFilePath, proj.hexFilePath);
        }
    }
    setting.endArray();

    smKeyRead = "RP_POLL_" + curProj.Proj_name;
    smKeyWrite = "WP_" + curProj.Proj_name;

    smKeyReadDAQ = "RP_DAQ_" + curProj.Proj_name;

    //sizeRead = sizeBitRead/8 + (sizeBitRead%8 == 0 ? 0 : 1);
    //sizeWrite = sizeBitWrite/8 + (sizeBitWrite%8 == 0 ? 0 : 1);

    qDebug()<<"CurSetProj"<<curProj.Proj_name<<", numRead:"<<curProj.readPamList.count()<<", numWrite:"<<curProj.writePamList.count()
           <<", numReadDAQ:"<<curProj.daqReadPamList.count();

}

void MainWindow::readCurSetting_2nd()
{
    QString appPath = QApplication::applicationDirPath();
    QSettings setting(appPath + "//setting//" + SET_FILENAME, QSettings::IniFormat);


    QString curProjName = setting.value("CurSetProject_2nd").toString();

    int sizeBitRead = 0;
    int sizeBitWrite = 0;

    int size = setting.beginReadArray("SetProjectList");
    for (int i = 0; i < size; i++)
    {
        setting.setArrayIndex(i);
        Proj proj;
        proj.Proj_name = setting.value("SetProjectName").toString();
        proj.intfName = setting.value("Interface").toString();
        proj.a2lProjectName = setting.value("A2LProjectName").toString();
        proj.a2lFilePath = setting.value("A2LFilePath").toString();
        proj.hexFilePath = setting.value("HexFilePath").toString();
        proj.index_XcpOnCan = setting.value("Index_XcpOnCan").toInt();
        proj.baudrate = setting.value("Baudrate").toInt();
        proj.id_CMD = setting.value("ID_CMD").toInt();
        proj.id_RES = setting.value("ID_RES").toInt();

        sizeBitRead = 0;
        sizeBitWrite = 0;

        // load readPams
        QList<XCP_PAM> pamList;

        QStringList pamNames, pamPollTypes, pamDefaultValues, pamCaliTypes, pamDaqEventTypes;
        QString pamName = setting.value("readPamNames").toString();
        QString pamPollType = setting.value("readPamPollTypes").toString();
        QString pamDefaultValue = setting.value("readPamDefaultValues").toString();

        if (!pamName.isEmpty())
        {
            pamNames = pamName.split("|");
            pamPollTypes = pamPollType.split("|");
            pamDefaultValues = pamDefaultValue.split("|");
        }
        for(int j = 0; j < pamNames.count(); j++)
        {
            XCP_PAM pam;
            pam.pamName = pamNames.at(j);
            pam.pollType = pamPollTypes.at(j);
            QString valueStr = pamDefaultValues.at(j);
            pam.pamDefaultValue = valueStr.toDouble();
            pamList.append(pam);

            int lenBit = 0;
            sizeBitRead += lenBit;
        }
        proj.readPamList = pamList;


        // load writePams
        pamList.clear();
        pamNames.clear();
        pamDefaultValues.clear();

        QString pamCaliType;

        pamName = setting.value("writePamNames").toString();
        pamDefaultValue = setting.value("writePamDefaultValues").toString();
        pamCaliType = setting.value("writePamTypes").toString();

        if (!pamName.isEmpty())
        {
            pamNames = pamName.split("|");
            pamDefaultValues = pamDefaultValue.split("|");
            pamCaliTypes = pamCaliType.split("|");
        }
        for(int j = 0; j < pamNames.count(); j++)
        {
            XCP_PAM pam;
            pam.pamName = pamNames.at(j);
            QString valueStr = pamDefaultValues.at(j);
            pam.pamDefaultValue = valueStr.toDouble();
            pam.caliType = pamCaliTypes.at(j);

            pamList.append(pam);

            int lenBit = 0;
            sizeBitWrite += lenBit;
        }
        proj.writePamList = pamList;

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
        proj.daqReadPamList = pamList;

        if (curProjName == proj.Proj_name)
        {
            curProj_2nd = proj;
        }

        if(!a2lWin->getProjectByName(proj.a2lProjectName))
        {
            a2lWin->addA2LProject(proj.a2lFilePath, proj.hexFilePath);
        }
    }
    setting.endArray();

    smKeyRead_2nd = "RP_POLL_" + curProj_2nd.Proj_name;
    smKeyWrite_2nd = "WP_" + curProj_2nd.Proj_name;

    smKeyReadDAQ_2nd = "RP_DAQ_" + curProj_2nd.Proj_name;

    //sizeRead = sizeBitRead/8 + (sizeBitRead%8 == 0 ? 0 : 1);
    //sizeWrite = sizeBitWrite/8 + (sizeBitWrite%8 == 0 ? 0 : 1);

    qDebug()<<"CurSetProj"<<curProj.Proj_name<<", numRead:"<<curProj.readPamList.count()<<", numWrite:"<<curProj.writePamList.count()
           <<", numReadDAQ:"<<curProj.daqReadPamList.count();
}

TypeInfo MainWindow::getPamInfoFromType(QString type)
{
    TypeInfo info;
    if(type == "BOOL")
    {
        info.LengthBit = 1;
        info.Type = 0;
    }
    else if(type == "UINT8")
    {
        info.LengthBit = 8;
        info.Type = 1;
    }
    else if(type == "INT8")
    {
        info.LengthBit = 8;
        info.Type = 2;
    }
    else if(type == "UINT16")
    {
        info.LengthBit = 16;
        info.Type = 3;
    }
    else if(type == "INT16")
    {
        info.LengthBit = 16;
        info.Type = 4;
    }
    else if(type == "UINT32")
    {
        info.LengthBit = 32;
        info.Type = 5;
    }
    else if(type == "INT32")
    {
        info.LengthBit = 32;
        info.Type = 6;
    }
    else if(type == "UINT64")
    {
        info.LengthBit = 64;
        info.Type = 7;
    }
    else if(type == "INT64")
    {
        info.LengthBit = 64;
        info.Type = 8;
    }
    else if(type == "FLOAT32")
    {
        info.LengthBit = 32;
        info.Type = 9;
    }
    else if(type == "DOUBLE64")
    {
        info.LengthBit = 64;
        info.Type = 10;
    }

    return info;
}

void MainWindow::getA2LPamsByNames()
{
    if(curA2LProject == NULL)
        return;

    qDeleteAll(measPamList);
    measPamList.clear();
    Globals::measPamList_global.clear();

    qDeleteAll(charPamList);
    charPamList.clear();
    Globals::charPamList_global.clear();

    qDeleteAll(charMapPamList);
    charMapPamList.clear();

    qDeleteAll(daqMeasPamList);
    daqMeasPamList.clear();
    Globals::daqMeasPamList_global.clear();

    int startBitIndex = 0;
    int startByteIndex = 0;

    startBitIndex += 0;

    qDebug()<<"************getting read meas pams from a2l project***************";
    for(int i = 0; i < curProj.readPamList.count(); i++)
    {
        XCP_PAM xcpPam = curProj.readPamList.at(i);

        A2L_VarMeas *measVar = curA2LProject->getMeasVarByName(xcpPam.pamName);

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
    qDebug()<<"************getting write meas pams from a2l project***************";
    for(int i = 0; i < curProj.writePamList.count(); i++)
    {
        XCP_PAM xcpPam = curProj.writePamList.at(i);

        A2L_VarChar *charVar = curA2LProject->getCharVarByName(xcpPam.pamName);

        if(charVar == NULL)
            continue;

        if(charVar->charType == "VALUE")
        {
            charPamList.append(charVar);

            charVar->StartByte = startByteIndex;
            charVar->smKey = smKeyWrite;
            charVar->setValue(xcpPam.pamDefaultValue);

            startBitIndex += (charVar->DataSizeAG * 8);
            startByteIndex = startBitIndex/8;
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
    for(int i = 0; i < curProj.daqReadPamList.count(); i++)
    {
        XCP_PAM xcpPam = curProj.daqReadPamList.at(i);
        //qDebug()<<"xcpPam rate_ms:"<<xcpPam.daqEventTime;
        A2L_VarMeas *measVar = curA2LProject->getMeasVarByName(xcpPam.pamName);

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

    Globals::measPamList_global = measPamList;
    Globals::charPamList_global = charPamList;
    Globals::daqMeasPamList_global = daqMeasPamList;

    qDebug()<<"Total size of measVars:"<<sizeRead<<", size of charVars for VALUE:"<<sizeWrite<<", size of daqMeasVars:"<<sizeReadDAQ;
    qDebug()<<"Num of charVars for MAP:"<<charMapPamList.count();


}

void MainWindow::getA2LPamsByNames_2nd()
{
    if(curA2LProject_2nd == NULL)
        return;

    qDeleteAll(measPamList_2nd);
    measPamList_2nd.clear();
    Globals::measPamList_2nd_global.clear();

    qDeleteAll(charPamList_2nd);
    charPamList_2nd.clear();
    Globals::charPamList_2nd_global.clear();

    qDeleteAll(charMapPamList_2nd);
    charMapPamList_2nd.clear();

    qDeleteAll(daqMeasPamList_2nd);
    daqMeasPamList_2nd.clear();
    Globals::daqMeasPamList_2nd_global.clear();

    int startBitIndex = 0;
    int startByteIndex = 0;

    startBitIndex += 0;

    qDebug()<<"************getting 2nd read meas pams from a2l project***************";
    for(int i = 0; i < curProj_2nd.readPamList.count(); i++)
    {
        XCP_PAM xcpPam = curProj_2nd.readPamList.at(i);

        A2L_VarMeas *measVar = curA2LProject_2nd->getMeasVarByName(xcpPam.pamName);

        if(measVar)
        {
            measPamList_2nd.append(measVar);

            measVar->StartByte = startByteIndex;
            measVar->smKey = smKeyRead_2nd;

            startBitIndex += (measVar->DataSizeAG * 8);
            startByteIndex = startBitIndex/8;

        }
    }

    sizeRead_2nd = startBitIndex/8;

    startBitIndex = 0;
    startByteIndex = 0;
    qDebug()<<"************getting 2nd write meas pams from a2l project***************";
    for(int i = 0; i < curProj_2nd.writePamList.count(); i++)
    {
        XCP_PAM xcpPam = curProj_2nd.writePamList.at(i);

        A2L_VarChar *charVar = curA2LProject_2nd->getCharVarByName(xcpPam.pamName);

        if(charVar == NULL)
            continue;

        if(charVar->charType == "VALUE")
        {
            charPamList_2nd.append(charVar);

            charVar->StartByte = startByteIndex;
            charVar->smKey = smKeyWrite_2nd;
            charVar->setValue(xcpPam.pamDefaultValue);

            startBitIndex += (charVar->DataSizeAG * 8);
            startByteIndex = startBitIndex/8;
        }
        else if(charVar->charType == "CURVE" || charVar->charType == "MAP" || charVar->charType == "VAL_BLK")
        {
            charMapPamList_2nd.append(charVar);
        }
    }

    sizeWrite_2nd = startBitIndex/8;

    //daq meas pams
    startBitIndex = 0;
    startByteIndex = 0;
    qDebug()<<"************getting 2nd daq read meas pams from a2l project***************";
    for(int i = 0; i < curProj_2nd.daqReadPamList.count(); i++)
    {
        XCP_PAM xcpPam = curProj_2nd.daqReadPamList.at(i);
        //qDebug()<<"xcpPam rate_ms:"<<xcpPam.daqEventTime;
        A2L_VarMeas *measVar = curA2LProject_2nd->getMeasVarByName(xcpPam.pamName);

        if(measVar)
        {
            daqMeasPamList_2nd.append(measVar);

            measVar->StartByte = startByteIndex;
            measVar->smKey = smKeyReadDAQ_2nd;
            measVar->rate_ms = xcpPam.daqEventTime.replace("ms", "").toDouble();
            //qDebug()<<"Meas rate_ms:"<<measVar->rate_ms;

            startBitIndex += (measVar->DataSizeAG * 8);
            startByteIndex = startBitIndex/8;
        }
    }

    sizeReadDAQ_2nd = startBitIndex/8;

    Globals::measPamList_2nd_global = measPamList_2nd;
    Globals::charPamList_2nd_global = charPamList_2nd;
    Globals::daqMeasPamList_2nd_global = daqMeasPamList_2nd;

    qDebug()<<"Total size of measVars:"<<sizeRead_2nd<<", size of charVars for VALUE:"<<sizeWrite_2nd<<", size of daqMeasVars:"<<sizeReadDAQ_2nd;
    qDebug()<<"Num of charVars for MAP:"<<charMapPamList_2nd.count();
}

void MainWindow::showMeasAndCharsInTable()
{
    ui->tableWidget_Read->clearContents();
    ui->tableWidget_Write->clearContents();
    ui->tableWidget_Read_DAQ->clearContents();

    ui->tableWidget_Read->setRowCount(measPamList.count());
    ui->tableWidget_Write->setRowCount(charPamList.count());
    ui->tableWidget_Read_DAQ->setRowCount(daqMeasPamList.count());

    for(int i = 0; i < measPamList.count(); i++)
    {
        A2L_VarMeas *measVar = measPamList.at(i);        

        ui->tableWidget_Read->setItem(i, 0, new QTableWidgetItem(measVar->Name));

        QTableWidgetItem *valueItem = new QTableWidgetItem(QString::number(measVar->getValue()));
        valueItem->setTextAlignment(Qt::AlignCenter);
        ui->tableWidget_Read->setItem(i, 1, valueItem);

    }

    for(int i = 0; i < charPamList.count(); i++)
    {
        A2L_VarChar *charVar = charPamList.at(i);

        QTableWidgetItem *varItem = new QTableWidgetItem();
        varItem->setData(Qt::UserRole,QVariant::fromValue(charVar));
        varItem->setText(charVar->Name);

        QTableWidgetItem *varValueItem = new QTableWidgetItem();
        varValueItem->setData(Qt::UserRole,QVariant::fromValue(charVar));
        varValueItem->setText(QString::number(charVar->getValue()));
        varValueItem->setTextAlignment(Qt::AlignCenter);

        ui->tableWidget_Write->setItem(i, 0, varItem);
        ui->tableWidget_Write->setItem(i, 1, varValueItem);
    }

    for(int i = 0; i < daqMeasPamList.count(); i++)
    {
        A2L_VarMeas *measVar = daqMeasPamList.at(i);

        ui->tableWidget_Read_DAQ->setItem(i, 0, new QTableWidgetItem(measVar->Name));

        QTableWidgetItem *valueItem = new QTableWidgetItem(QString::number(measVar->getValue()));
        valueItem->setTextAlignment(Qt::AlignCenter);
        ui->tableWidget_Read_DAQ->setItem(i, 1, valueItem);
    }

    // 配置并启动数据更新器
    dataUpdater->setMeasVars(measPamList);
    dataUpdater->setDaqMeasVars(daqMeasPamList);
    dataUpdater->setCharVars(charPamList);
    dataUpdater->setUpdateRate(updateRate_ms);
    dataUpdater->start();

}

void MainWindow::showMeasAndCharsInTable_2nd()
{
    ui->tableWidget_Read_2->clearContents();
    ui->tableWidget_Write_2->clearContents();
    ui->tableWidget_Read_DAQ_2->clearContents();

    ui->tableWidget_Read_2->setRowCount(measPamList_2nd.count());
    ui->tableWidget_Write_2->setRowCount(charPamList_2nd.count());
    ui->tableWidget_Read_DAQ_2->setRowCount(daqMeasPamList_2nd.count());

    for(int i = 0; i < measPamList_2nd.count(); i++)
    {
        A2L_VarMeas *measVar = measPamList_2nd.at(i);

        ui->tableWidget_Read_2->setItem(i, 0, new QTableWidgetItem(measVar->Name));

        QTableWidgetItem *valueItem = new QTableWidgetItem(QString::number(measVar->getValue()));
        valueItem->setTextAlignment(Qt::AlignCenter);
        ui->tableWidget_Read_2->setItem(i, 1, valueItem);

    }

    for(int i = 0; i < charPamList_2nd.count(); i++)
    {
        A2L_VarChar *charVar = charPamList_2nd.at(i);

        QTableWidgetItem *varItem = new QTableWidgetItem();
        varItem->setData(Qt::UserRole,QVariant::fromValue(charVar));
        varItem->setText(charVar->Name);

        QTableWidgetItem *varValueItem = new QTableWidgetItem();
        varValueItem->setData(Qt::UserRole,QVariant::fromValue(charVar));
        varValueItem->setText(QString::number(charVar->getValue()));
        varValueItem->setTextAlignment(Qt::AlignCenter);

        ui->tableWidget_Write_2->setItem(i, 0, varItem);
        ui->tableWidget_Write_2->setItem(i, 1, varValueItem);
    }

    for(int i = 0; i < daqMeasPamList_2nd.count(); i++)
    {
        A2L_VarMeas *measVar = daqMeasPamList_2nd.at(i);

        ui->tableWidget_Read_DAQ_2->setItem(i, 0, new QTableWidgetItem(measVar->Name));

        QTableWidgetItem *valueItem = new QTableWidgetItem(QString::number(measVar->getValue()));
        valueItem->setTextAlignment(Qt::AlignCenter);
        ui->tableWidget_Read_DAQ_2->setItem(i, 1, valueItem);
    }

    // 配置并启动数据更新器
    dataUpdater_2nd->setMeasVars(measPamList_2nd);
    dataUpdater_2nd->setDaqMeasVars(daqMeasPamList_2nd);
    dataUpdater_2nd->setCharVars(charPamList_2nd);
    dataUpdater_2nd->setUpdateRate(updateRate_ms);
    dataUpdater_2nd->start();
}

void MainWindow::showMeasAndCharsInTableView()
{
    readModel.clear();
    writeModel.clear();

    readModel.setRowCount(measPamList.count());
    writeModel.setRowCount(charPamList.count());
    readModel.setColumnCount(2);
    writeModel.setColumnCount(2);

    for(int i = 0; i < measPamList.count(); i++)
    {
        A2L_VarMeas *measVar = measPamList.at(i);

        QModelIndex pamNameIndex = readModel.index(i, 0, QModelIndex());
        QModelIndex pamValueIndex = readModel.index(i, 1, QModelIndex());
        readModel.setData(pamNameIndex, QVariant(measVar->Name));
        readModel.setData(pamValueIndex, QVariant(measVar->getValue()));

    }

    for(int i = 0; i < charPamList.count(); i++)
    {
        A2L_VarChar *charVar = charPamList.at(i);

        QModelIndex pamNameIndex = writeModel.index(i, 0, QModelIndex());
        QModelIndex pamValueIndex = writeModel.index(i, 1, QModelIndex());
        writeModel.setData(pamNameIndex, QVariant(charVar->Name));
        writeModel.setData(pamValueIndex, QVariant(charVar->getValue()));
    }

    readTableView.resizeColumnsToContents();
    writeTableView.resizeColumnsToContents();


}

void MainWindow::showCharMapsInMapWin()
{
    if(mapWin == NULL)
        return;
    if(charMapPamList.isEmpty())
        return;

    mapWin->updateMapList(charMapPamList);
    // 使用MemoryManager替代QSharedMemory，不需要设置smMapWriteHash
    mapWin->show();
}

void MainWindow::showCharMapsInMapWin_2nd()
{
    if(mapWin_2nd == NULL)
        return;
    if(charMapPamList_2nd.isEmpty())
        return;

    mapWin_2nd->updateMapList(charMapPamList_2nd);
    // 使用MemoryManager替代QSharedMemory，不需要设置smMapWriteHash_2nd
    mapWin_2nd->show();
}

void MainWindow::show3DDataVisualization()
{
    Plot3DWin *plot3DWin = new Plot3DWin(this);
    plot3DWin->setWinName("3D Data Visualization");
    plot3DWin->show();
}

void MainWindow::SltDataUpdateInTable()
{

    ui->le_readTime->setText(getPollReadTime());
    for(int i = 0; i < measPamList.count(); i++)
    {
        A2L_VarMeas *measVar = measPamList.at(i);

        QTableWidgetItem *valueItem = ui->tableWidget_Read->item(i, 1);
        valueItem->setText(QString::number(measVar->getValue(), 'g', 8));
    }

    ui->le_readTime_DAQ->setText(getDaqReadTime());
    for(int i = 0; i < daqMeasPamList.count(); i++)
    {
        A2L_VarMeas *measVar = daqMeasPamList.at(i);

        QTableWidgetItem *valueItem = ui->tableWidget_Read_DAQ->item(i, 1);
        valueItem->setText(QString::number(measVar->getValue(), 'g', 8));
    }

    ui->le_writeTime->setText(getCaliWriteTime());
}

void MainWindow::SltDataUpdateInTable_2nd()
{
    ui->le_readTime_Polling_2->setText(getPollReadTime_2nd());
    for(int i = 0; i < measPamList_2nd.count(); i++)
    {
        A2L_VarMeas *measVar = measPamList_2nd.at(i);

        QTableWidgetItem *valueItem = ui->tableWidget_Read_2->item(i, 1);
        valueItem->setText(QString::number(measVar->getValue(), 'g', 8));
    }

    ui->le_readTime_DAQ_2->setText(getDaqReadTime_2nd());
    for(int i = 0; i < daqMeasPamList_2nd.count(); i++)
    {
        A2L_VarMeas *measVar = daqMeasPamList_2nd.at(i);

        QTableWidgetItem *valueItem = ui->tableWidget_Read_DAQ_2->item(i, 1);
        valueItem->setText(QString::number(measVar->getValue(), 'g', 8));
    }

    ui->le_writeTime_2->setText(getCaliWriteTime_2nd());
}

QString MainWindow::getHexDataString(char *data, quint64 size)
{
    QString HexString = "";

    for(int i = 0; i < size; i++)
    {
        HexString += (QString("%1").arg((quint8)(data[i]), 2, 16, QLatin1Char('0')).toUpper() + " ");
    }
    return HexString;
}


void MainWindow::fromReadRawDataToPamValue(char *data, quint64 size)
{
    CharArrayPtr buffer = makeCharArray(size);
    memcpy(buffer.data(), data, size);

    for(int i = 0; i < readPamList.count(); i++)
    {
        PARAM *pam = readPamList.at(i);

        int sb = pam->StartBit-64;
        int lb = pam->LengthBit;
        int type = pam->Type;

        std::unique_ptr<quint64> result(new quint64);
        qreal pamValue = 0;

        from_intel2userdata(buffer.data(), size, sb, lb, result.get());

        if(type == 0 || type == 1 || type == 3 || type == 5 || type == 7)
        {
            pamValue = *result;
        }
        else if(type == 2)
        {
            pamValue = *(qint8*)result.get();
        }
        else if(type == 4)
        {
            pamValue = *(qint16*)result.get();
        }
        else if(type == 6)
        {
            pamValue = *(qint32*)result.get();
        }
        else if(type == 8)
        {
            pamValue = *(qint64*)result.get();
        }
        else if(type == 9)
        {
            pamValue = *(float*)result.get();
        }
        else if(type == 10)
        {
            pamValue = *(qreal*)result.get();
        }

        pam->setValue(pamValue);

        //ui->tableWidget_Read->item(i, 1)->setText(QString::number(pamValue, 'g', 6));
    }
}

void MainWindow::fromReadSMToPamValue()
{
    // 使用MemoryManager替代QSharedMemory
    QString key = "RP_" + curProj.Proj_name;
    void* memory = MemoryManager::instance()->getMemory(key);
    if(!memory)
        return;
    CharArrayPtr buffer = makeCharArray(sizeRead);

    // 直接读取内存数据，不需要锁定，因为MemoryManager内部已经处理了线程安全
    qreal readTime = 0;
    memcpy((char*)&readTime, (char*)memory, 8);
    memcpy(buffer.data(), (char*)memory + 8, sizeRead);

    ui->le_readTime->setText(QString::number(readTime, 'f', 3));
    fromReadRawDataToPamValue(buffer.data(), sizeRead);
    if(showRawData)
    {
        //ui->tb_Read->setText(getHexDataString(buffer.data(), sizeRead));
    }
}

void MainWindow::fromPamsToWriteRawData(char *data, quint64 size)
{
    CharArrayPtr buffer = makeCharArray(size);

    for(int i = 0; i < writePamList.count(); i++)
    {
        PARAM *pam = writePamList.at(i);

        value2IntelData(buffer.data(), pam);
    }

    memcpy(data, buffer.data(), size);
}

void MainWindow::fromPamsToWriteSM()
{
    // 使用MemoryManager替代QSharedMemory
    QString key = "WP_" + curProj.Proj_name;
    void* memory = MemoryManager::instance()->getMemory(key);
    if(!memory)
        return;
    CharArrayPtr buffer = makeCharArray(sizeWrite+8);

    fromPamsToWriteRawData(buffer.data(), sizeWrite);

    // 直接写入内存数据，不需要锁定，因为MemoryManager内部已经处理了线程安全
    memcpy((char*)memory, buffer.data(), sizeWrite+8);

    emit writePamValueUpdated();

}

void MainWindow::fromReadSMToMeasVars()
{
    if(!showData)
        return;

    // 使用MemoryManager替代QSharedMemory
    QString key = "RP_" + curProj.Proj_name;
    void* memory = MemoryManager::instance()->getMemory(key);
    if(!memory)
        return;

    CharArrayPtr buffer = makeCharArray(sizeRead);

    // 直接读取内存数据，不需要锁定，因为MemoryManager内部已经处理了线程安全
    qreal readTime = 0;
    memcpy((char*)&readTime, (char*)memory, 8);
    memcpy(buffer.data(), (char*)memory + 8, sizeRead);

    ui->le_readTime->setText(QString::number(readTime, 'f', 3));
    fromReadRawDataToMeasVars(buffer.data(), sizeRead);
    if(showRawData)
    {
        //ui->tb_Read->setText(getHexDataString(buffer.data(), sizeRead));
    }

    //emit mdfRecordDataUpdated(buffer.data(), sizeRead);
    if(recordOn)
    {
        //mdfRecordIns->mdf_record_writeRawData(buffer.data(), sizeRead+8);
    }
}

void MainWindow::fromReadRawDataToMeasVars(char *data, quint64 size)
{

    for(int i = 0; i < measPamList.count(); i++)
    {
        A2L_VarMeas *measVar = measPamList.at(i);

        int startByte = measVar->StartByte;
        int dataSize = measVar->DataSizeAG;
        QString type = measVar->DataType;

        if(startByte + dataSize > size)
            continue;

        CharArrayPtr temp = makeCharArray(dataSize);
        memcpy(temp.data(), data+startByte, dataSize);

        qreal measValue = 0;
        switch (dataSize) {
        case 1:
        {
            if(type == "UBYTE")
                measValue = *(quint8*)temp.data();
            else if(type == "SBYTE")
                measValue = *(qint8*)temp.data();
            break;
        }
        case 2:
        {
            if(type == "UWORD")
                measValue = *(quint16*)temp.data();
            else if(type == "SWORD")
                measValue = *(qint16*)temp.data();
            break;
        }
        case 4:
        {
            if(type == "ULONG")
                measValue = *(quint32*)temp.data();
            else if(type == "SLONG")
                measValue = *(qint32*)temp.data();
            else if(type == "FLOAT32_IEEE")
                measValue = *(float*)temp.data();
            break;
        }
        case 8:
        {
            if(type == "A_UINT64")
                measValue = *(quint64*)temp.data();
            else if(type == "A_INT64")
                measValue = *(qint64*)temp.data();
            else if(type == "FLOAT64_IEEE")
                measValue = *(qreal*)temp.data();
            break;
        }
        default:
            break;
        }

        qreal measPhysValue = 0;
        if (measVar->ConversionType == "RAT_FUNC")
        {
             measPhysValue = xcpMaster->RatFuncConvFromIntToPhys(measValue, measVar->CoefList);
        }
        else if (measVar->ConversionType == "TAB_VERB")
        {
            measPhysValue = measValue;
        }
        else
        {
            measPhysValue = measValue;
        }

        measVar->setValue(measPhysValue);
        ui->tableWidget_Read->item(i, 1)->setText(QString::number(measPhysValue, 'g', 6));
        //QModelIndex index = readModel.index(i, 1, QModelIndex());
        //readModel.setData(index, QVariant(measPhysValue));
    }

}

void MainWindow::fromCharVarsToWriteSM()
{
    // 使用MemoryManager替代QSharedMemory
    QString key = "WP_" + curProj.Proj_name;
    void* memory = MemoryManager::instance()->getMemory(key);
    if(!memory)
        return;
    CharArrayPtr buffer = makeCharArray(sizeWrite+8);

    fromCharVarsToWriteRawData(buffer.data(), sizeWrite);

    // 直接写入内存数据，不需要锁定，因为MemoryManager内部已经处理了线程安全
    qreal initTime = 0;
    memcpy((char*)memory, (char*)&initTime, 8);
    memcpy((char*)memory + 8, buffer.data(), sizeWrite);

    emit writePamValueUpdated();
}

void MainWindow::fromCharVarsToWriteRawData(char *data, quint64 size)
{
    CharArrayPtr buffer = makeCharArray(size);

    for(int i = 0; i < charPamList.count(); i++)
    {
        A2L_VarChar *charVar = charPamList.at(i);

        value2IntelData(buffer.data(), charVar);
    }

    memcpy(data, buffer.data(), size);
}

void MainWindow::fromMapCharVarsToMapWriteSM()
{
    for(int i = 0; i < charMapPamList.count(); i++)
    {
        A2L_VarChar *charVar = charMapPamList.at(i);

        int size = (charVar->zCount * charVar->DataSizeAG + 8);

        // 使用MemoryManager替代QSharedMemory
        QString key = QString("MAP_%1").arg(charVar->Name);
        void* mem = MemoryManager::instance()->getMemory(key);
        if (!mem) {
            qDebug()<<"Unable to get map write memory.";
            continue;
        }

        int offset = 8;
        for(int j = 0; j < charVar->zCount; j++)
        {
            qreal value = charVar->hexValue_ZList.at(j);
            CharArrayPtr data = makeCharArray(charVar->DataSizeAG);
            transferPhyValueToRawData(charVar, value, data.data());

            memcpy((char*)mem+offset, data.data(), charVar->DataSizeAG);

            offset += charVar->DataSizeAG;
        }

    }
}

bool MainWindow::charVars_Download(QList<A2L_VarChar *> charVars)
{
    if(!xcpMaster)
        return false;
    if(!xcpMaster->getXcpSessionConnectedStatus())
        return false;

    statusBar()->show();
    progBar->reset();
    progBar->setMaximum(charVars.count());

    for(int i = 0; i < charVars.count(); i++)
    {
        A2L_VarChar *charVar = charVars.at(i);

        if(!xcpMaster->XCP_Cal_VALUE(charVar))
        {
            qDebug()<<XCPErrorRegistry::instance().formatError(ERR_GENERIC, QString("Charac: %1, Calibrate error").arg(charVar->Name));
            return false;
        }
        else
        {
            qDebug()<<"Charac:"<<charVar->Name<<", download ok:"<<charVar->getValue();
        }
        progBar->setValue(i);
    }

    statusBar()->hide();
    progBar->reset();

    return true;
}

int MainWindow::from_intel2userdata(char *data, quint64 size, int startbit, int lenbit, quint64 *userdata)
{
    if((startbit + lenbit) > (size*8))
        return 0;

    quint64 mask = 0;
    quint64 result = 0;

    for(int i = 0; i < lenbit; i++)
    {
        mask |= ((quint64)1 << i);
    }

    result = (*(quint64*)(data+startbit/8) >> (startbit%8)) & mask;

    *userdata = result;

    return 1;
}

void MainWindow::value2IntelData(char *data, PARAM *pam)
{
    int startBit = pam->StartBit;
    int startByte = startBit / 8;
    int offset = startBit % 8;
    quint32 lengthBit = pam->LengthBit;
    int type = pam->Type;
    qreal value = pam->getValue();

    bool val = (value > 0 ? true : false);
    quint8 val8 = val;
    if (val)
        val8 = val << offset;
    else
        val8 = rotateLeft(0xFE, offset);

    char *to = data;
    switch (type) {
    case 0:
        if (val)
            *(quint8*)(to + startByte) |= val8;
        else
            *(quint8*)(to + startByte) &= val8;
        break;
    case 1:
    {
        *(quint8*)(to + startByte) = (quint8)value;
        break;
    }
    case 2:
    {
        *(qint8*)(to + startByte) = (qint8)value;
        break;
    }
    case 3:
    {
        *(quint16*)(to + startByte) = (quint16)value;
        break;
    }
    case 4:
    {
        *(qint16*)(to + startByte) = (qint16)value;
        break;
    }
    case 5:
    {
        *(quint32*)(to + startByte) = (quint32)value;
        break;
    }
    case 6:
    {
        *(qint32*)(to + startByte) = (qint32)value;
        break;
    }
    case 7:
    {
        *(quint64*)(to + startByte) = (quint64)value;
        break;
    }
    case 8:
    {
        *(qint64*)(to + startByte) = (qint64)value;
        break;
    }
    case 9:
    {
        *(float*)(to + startByte) = (float)value;
        break;
    }
    case 10:
    {
        *(qreal*)(to + startByte) = (qreal)value;
        break;
    }
    default:
        break;
    }


}

quint8 MainWindow::rotateLeft(quint8 val, quint8 n)
{
    n %= 8;
    if (n ==0)
        return val;
    return (val << n) | (val >> (8-n));
}

void MainWindow::value2IntelData(char *data, A2L_VarChar *charVar)
{
    int startByte = charVar->StartByte;
    quint32 LenBit = charVar->DataSizeAG*8;
    QString type = charVar->DataType;

    qreal value = 0;
    if (charVar->ConversionType == "RAT_FUNC")
    {
         value = xcpMaster->RatFuncConvFromPhysToInt(charVar->getValue(), charVar->CoefList);
    }
    else if (charVar->ConversionType == "TAB_VERB")
    {
        value = charVar->getValue();
    }
    else
    {
        value = charVar->getValue();
    }

    char *to = data;
    switch (LenBit) {
    case 8:
    {
        if(type == "UBYTE")
            *(quint8*)(to + startByte) = (quint8)value;
        else if(type == "SBYTE")
            *(qint8*)(to + startByte) = (qint8)value;
        break;
    }
    case 16:
    {
        if(type == "UWORD")
            *(quint16*)(to + startByte) = (quint16)value;
        else if(type == "SWORD")
            *(qint16*)(to + startByte) = (qint16)value;

        break;
    }
    case 32:
    {
        if(type == "ULONG")
            *(quint32*)(to + startByte) = (quint32)value;
        else if(type == "SLONG")
            *(qint32*)(to + startByte) = (qint32)value;
        else if(type == "FLOAT32_IEEE")
            *(float*)(to + startByte) = (float)value;
        break;
    }
    case 64:
    {
        if(type == "A_UINT64")
            *(quint64*)(to + startByte) = (quint64)value;
        else if(type == "A_INT64")
            *(qint64*)(to + startByte) = (qint64)value;
        else if(type == "FLOAT64_IEEE")
            *(qreal*)(to + startByte) = (qreal)value;

        break;
    }
    default:
        break;
    }
}

PARAM *MainWindow::findPamByName(QList<PARAM *> pamList, QString pamName)
{
    if(pamList.isEmpty())
        return NULL;
    foreach (PARAM *pam, pamList) {
        if(pam->Name == pamName)
            return pam;
    }
    return NULL;
}

A2L_VarChar *MainWindow::findCharVarByName(QList<A2L_VarChar *> charPamList, QString charName)
{
    if(charPamList.isEmpty())
        return NULL;
    foreach (A2L_VarChar *charVar, charPamList) {
        if(charVar->Name == charName)
            return charVar;
    }
    return NULL;
}

QString MainWindow::getPollReadTime()
{
    QString timeStr = "no poll";

    // 使用MemoryManager替代QSharedMemory
    void* memory = MemoryManager::instance()->getMemory(this->smKeyRead);
    if(!memory)
    {
        qDebug()<<"Unable to get memory.";
        return QString("0.000");
    }
    qreal readTime = 0;
    // 直接读取内存数据，不需要锁定，因为MemoryManager内部已经处理了线程安全
    memcpy((char*)&readTime, (char*)memory, 8);

    if(pollStartTime == 0)
    {
        pollStartTime = readTime;
        pollTime = 0;
    }
    else
    {
        pollTime = (qreal)(readTime - pollStartTime) / 10000000.0;
    }

    timeStr = QString::number(pollTime, 'f', 2);

    return timeStr;

}

QString MainWindow::getDaqReadTime()
{
    QString timeStr = "no daq";

    QSharedMemory sm;
    QString smKeyDaq = "RP_DAQ_"+curProj.Proj_name + "_0";
    sm.setKey(smKeyDaq);

    if(!sm.isAttached())
    {
        if(!sm.attach())
        {
            timeStr = "no daq";
            return timeStr;
        }
    }
    quint64 readTime = 0;
    sm.lock();
    memcpy((char*)&readTime, (char*)sm.data(), 8);
    sm.unlock();

    if(daqStartTime == 0)
    {
        daqStartTime = readTime;
        daqTime = 0;
    }
    else
    {
        daqTime = (qreal)(readTime - daqStartTime) / 10000000.0;
    }

    timeStr = QString::number(daqTime, 'f', 2);

    return timeStr;
}

QString MainWindow::getCaliWriteTime()
{
    QString timeStr = "no cali";

    // 使用MemoryManager替代QSharedMemory
    void* memory = MemoryManager::instance()->getMemory(this->smKeyWrite);
    if(!memory)
    {
        qDebug()<<"Unable to get memory.";
        return QString("0.000");
    }
    qreal writeTime = 0;
    // 直接读取内存数据，不需要锁定，因为MemoryManager内部已经处理了线程安全
    memcpy((char*)&writeTime, (char*)memory, 8);

    if(caliStartTime == 0)
    {
        caliStartTime = writeTime;
        caliTime = 0;
    }
    else
    {
        caliTime = (qreal)(writeTime - caliStartTime) / 10000000.0;
    }

    timeStr = QString::number(caliTime, 'f', 2);

    return timeStr;
}

QString MainWindow::getPollReadTime_2nd()
{
    QString timeStr = "no poll";

    // 使用MemoryManager替代QSharedMemory
    void* memory = MemoryManager::instance()->getMemory(this->smKeyRead_2nd);
    if(!memory)
    {
        qDebug()<<"Unable to get memory.";
        return QString("0.000");
    }
    qreal readTime = 0;
    // 直接读取内存数据，不需要锁定，因为MemoryManager内部已经处理了线程安全
    memcpy((char*)&readTime, (char*)memory, 8);

    if(pollStartTime_2nd == 0)
    {
        pollStartTime_2nd = readTime;
        pollTime_2nd = 0;
    }
    else
    {
        pollTime_2nd = (qreal)(readTime - pollStartTime_2nd) / 10000000.0;
    }

    timeStr = QString::number(pollTime_2nd, 'f', 2);

    return timeStr;
}

QString MainWindow::getDaqReadTime_2nd()
{
    QString timeStr = "no daq";

    QSharedMemory sm;
    QString smKeyDaq = "RP_DAQ_"+curProj_2nd.Proj_name + "_0";
    sm.setKey(smKeyDaq);

    if(!sm.isAttached())
    {
        if(!sm.attach())
        {
            timeStr = "no daq";
            return timeStr;
        }
    }
    quint64 readTime = 0;
    sm.lock();
    memcpy((char*)&readTime, (char*)sm.data(), 8);
    sm.unlock();

    if(daqStartTime_2nd == 0)
    {
        daqStartTime_2nd = readTime;
        daqTime_2nd = 0;
    }
    else
    {
        daqTime_2nd = (qreal)(readTime - daqStartTime_2nd) / 10000000.0;
    }

    timeStr = QString::number(daqTime_2nd, 'f', 2);

    return timeStr;
}

QString MainWindow::getCaliWriteTime_2nd()
{
    QString timeStr = "no cali";

    // 使用MemoryManager替代QSharedMemory
    void* memory = MemoryManager::instance()->getMemory(this->smKeyWrite_2nd);
    if(!memory)
    {
        qDebug()<<"Unable to get memory.";
        return QString("0.000");
    }
    qreal writeTime = 0;
    // 直接读取内存数据，不需要锁定，因为MemoryManager内部已经处理了线程安全
    memcpy((char*)&writeTime, (char*)memory, 8);

    if(caliStartTime_2nd == 0)
    {
        caliStartTime_2nd = writeTime;
        caliTime_2nd = 0;
    }
    else
    {
        caliTime_2nd = (qreal)(writeTime - caliStartTime_2nd) / 10000000.0;
    }

    timeStr = QString::number(caliTime_2nd, 'f', 2);

    return timeStr;
}

bool MainWindow::genXML()
{
    QString appPath = QApplication::applicationDirPath();
    QString FileName = "XCP_COMM.xml";
    QString xmlPath = appPath + "/HQ_Testing/pamFactory/" + FileName;

    if (xmlPath.isEmpty())
        return false;

    QFile xmlFile(xmlPath);
    if (!xmlFile.open(QFile::WriteOnly | QFile::Text))
    {
        qDebug()<<XCPErrorRegistry::instance().formatError(ERR_COMMUNICATION, tr("Cannot write file %1:\n%2.").arg(xmlPath).arg(xmlFile.errorString()));
        return false;
    }

    QDomDocument xml("XCP");

    QDomElement root = xml.createElement("Root");

    xml.appendChild(root);

    QList<A2L_VarMeas*> pamList_r = measPamList;

    int sigIndex_r = 0;
    QDomElement ReadSigElem = xml.createElement(smKeyRead);
    root.appendChild(ReadSigElem);

    // add time channel
    QDomElement timeElem_r = xml.createElement("Param_"+QString::number(sigIndex_r));
    ReadSigElem.appendChild(timeElem_r);
    timeElem_r.setAttribute("Unit", "s");
    timeElem_r.setAttribute("DefaultValue", QString::number(0,10));
    timeElem_r.setAttribute("Minimum", QString::number(0, 10));
    timeElem_r.setAttribute("Maximum", QString::number(0, 10));
    timeElem_r.setAttribute("ScalingOffset", QString::number(0));
    timeElem_r.setAttribute("ScalingFactor", QString::number(1));
    timeElem_r.setAttribute("LengthBit", QString::number(64, 10));
    timeElem_r.setAttribute("StartBit", QString::number(0, 10));
    timeElem_r.setAttribute("StartBit_Oth", QString::number(0, 10));
    timeElem_r.setAttribute("ByteOrder", QString::number(0,10));
    timeElem_r.setAttribute("Type", QString::number(3, 10));
    timeElem_r.setAttribute("Param_Name", "Time");

    sigIndex_r++;


    for(int i=0; i < pamList_r.count(); i++)
    {
        A2L_VarMeas *measVar = pamList_r.at(i);
        QDomElement sigElem = xml.createElement("Param_"+QString::number(sigIndex_r));
        ReadSigElem.appendChild(sigElem);


        QString dataType = measVar->DataType;
        int type = 1;
        if(dataType == "UBYTE" || dataType == "UWORD" || dataType == "ULONG" || dataType == "A_UINT64")
            type = 1;
        else if(dataType == "SBYTE" || dataType == "SWORD" || dataType == "SLONG" || dataType == "A_INT64")
            type = 0;
        else if(dataType == "FLOAT32_IEEE")
            type = 2;
        else if(dataType == "FLOAT64_IEEE")
            type = 3;

        sigElem.setAttribute("Unit", measVar->Unit);
        sigElem.setAttribute("DefaultValue", QString::number(0, 'g', 6));
        sigElem.setAttribute("Minimum", QString::number(measVar->LowerLimit, 'g', 6));
        sigElem.setAttribute("Maximum", QString::number(measVar->UpperLimit, 'g', 6));
        sigElem.setAttribute("ScalingOffset", QString::number(measVar->ScalingOffset, 'g', 6));
        sigElem.setAttribute("ScalingFactor", QString::number(measVar->ScalingFactor, 'g', 6));
        sigElem.setAttribute("LengthBit", QString::number(measVar->DataSizeAG*8, 10));
        sigElem.setAttribute("StartBit", QString::number(measVar->StartByte*8+64, 10));
        sigElem.setAttribute("ByteOrder", QString::number(0,10));
        sigElem.setAttribute("Type", QString::number(type, 10));
        sigElem.setAttribute("Param_Name", measVar->Name);

        sigIndex_r++;
    }
    ReadSigElem.setAttribute("Size_Bytes", 8+sizeRead);
    ReadSigElem.setAttribute("Sample_Rate", QString::number(100,10));
    ReadSigElem.setAttribute("isRead", QString::number(1,10));



    QList<A2L_VarChar*> pamList_w = charPamList;

    int sigIndex_w = 0;
    QDomElement WriteSigElem = xml.createElement(smKeyWrite);
    root.appendChild(WriteSigElem);

    // add time channel
    QDomElement timeElem_w = xml.createElement("Param_"+QString::number(sigIndex_w));
    WriteSigElem.appendChild(timeElem_w);
    timeElem_w.setAttribute("Unit", "s");
    timeElem_w.setAttribute("DefaultValue", QString::number(0,10));
    timeElem_w.setAttribute("Minimum", QString::number(0, 10));
    timeElem_w.setAttribute("Maximum", QString::number(0, 10));
    timeElem_w.setAttribute("ScalingOffset", QString::number(0));
    timeElem_w.setAttribute("ScalingFactor", QString::number(1));
    timeElem_w.setAttribute("LengthBit", QString::number(64, 10));
    timeElem_w.setAttribute("StartBit", QString::number(0, 10));
    timeElem_w.setAttribute("StartBit_Oth", QString::number(0, 10));
    timeElem_w.setAttribute("ByteOrder", QString::number(0,10));
    timeElem_w.setAttribute("Type", QString::number(3, 10));
    timeElem_w.setAttribute("Param_Name", "Time");

    sigIndex_w++;

    for(int i=0; i < pamList_w.count(); i++)
    {
        A2L_VarChar *charVar = pamList_w.at(i);
        QDomElement sigElem = xml.createElement("Param_"+QString::number(sigIndex_w));
        WriteSigElem.appendChild(sigElem);

        QString dataType = charVar->DataType;
        int type = 1;
        if(dataType == "UBYTE" || dataType == "UWORD" || dataType == "ULONG" || dataType == "A_UINT64")
            type = 1;
        else if(dataType == "SBYTE" || dataType == "SWORD" || dataType == "SLONG" || dataType == "A_INT64")
            type = 0;
        else if(dataType == "FLOAT32_IEEE")
            type = 2;
        else if(dataType == "FLOAT64_IEEE")
            type = 3;

        sigElem.setAttribute("Unit", charVar->Unit);
        sigElem.setAttribute("DefaultValue", QString::number(charVar->getValue(), 'g', 6));
        sigElem.setAttribute("Minimum", QString::number(charVar->LowerLimit, 'g', 6));
        sigElem.setAttribute("Maximum", QString::number(charVar->UpperLimit, 'g', 6));
        sigElem.setAttribute("ScalingOffset", QString::number(charVar->ScalingOffset, 'g', 6));
        sigElem.setAttribute("ScalingFactor", QString::number(charVar->ScalingFactor, 'g', 6));
        sigElem.setAttribute("LengthBit", QString::number(charVar->DataSizeAG*8, 10));
        sigElem.setAttribute("StartBit", QString::number(charVar->StartByte*8+64, 10));
        sigElem.setAttribute("ByteOrder", QString::number(0,10));
        sigElem.setAttribute("Type", QString::number(type, 10));
        sigElem.setAttribute("Param_Name", charVar->Name);

        sigIndex_w++;
    }
    WriteSigElem.setAttribute("Size_Bytes", 8+sizeWrite);
    WriteSigElem.setAttribute("Sample_Rate", QString::number(100,10));
    WriteSigElem.setAttribute("isRead", QString::number(0,10));


    QString xmlString = xml.toString();
    QTextStream out(&xmlFile);
    out << xmlString;

    xmlFile.close();

    return true;
}

QList<PARAM *> MainWindow::transferMeasToPams(QList<A2L_VarMeas *> measPamList)
{
    QList<PARAM*> pamList;

    if(measPamList.isEmpty())
        return pamList;

    PARAM *timePam = new PARAM();
    timePam->Unit = "s";
    timePam->DefaultValue = 0;
    timePam->LowerLimit = 0;
    timePam->UpperLimit = 1000000;
    timePam->ScalingFactor = 1;
    timePam->ScalingOffset = 0;
    timePam->LengthBit = 64;
    timePam->StartBit = 0;
    timePam->ByteOrder = 0;
    timePam->Type = 3;
    timePam->Name = "Time";

    pamList.append(timePam);

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
        varPam->StartBit = measVar->StartByte*8+64;
        varPam->ByteOrder = 0;
        varPam->Type = type;
        varPam->Name = measVar->Name;

        pamList.append(varPam);
    }

    return pamList;

}

QList<PARAM *> MainWindow::fromMeasToPams(QList<A2L_VarMeas *> measPamList, quint32 &blockSize)
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

QHash<quint16, QList<PARAM *> > MainWindow::getPamHashFromMeasHash(QHash<quint16, QList<A2L_VarMeas *> > measHash)
{
    QHash<quint16, QList<PARAM *>> daqPamsHash;
    QList<quint16> keys = measHash.uniqueKeys();
    if(keys.count() == 0)
        return daqPamsHash;

    for(int i = 0; i < keys.count(); i++)
    {
        quint16 daqList = keys.at(i);
        QList<A2L_VarMeas*> meas = measHash.value(daqList);
        if(meas.isEmpty())
            continue;
        QList<PARAM*> pams = transferMeasToPams(meas);
        daqPamsHash.insert(daqList, pams);
    }

    return daqPamsHash;
}

QHash<QString, QList<PARAM *> > MainWindow::getdgPamHashFromdgMeasHash(QHash<QString, QList<A2L_VarMeas *> > measHash)
{
    QHash<QString, QList<PARAM *>> dgNamePamsHash;
    QStringList keys = measHash.uniqueKeys();
    if(keys.count() == 0)
        return dgNamePamsHash;

    for(int i = 0; i < keys.count(); i++)
    {
        QString dgName = keys.at(i);
        QList<A2L_VarMeas*> meas = measHash.value(dgName);
        if(meas.isEmpty())
            continue;
        QList<PARAM*> pams = transferMeasToPams(meas);
        dgNamePamsHash.insert(dgName, pams);
    }

    return dgNamePamsHash;
}

void MainWindow::showRecordTimeInTimeEdit(QString timeStr)
{
    ui->timeEdit->setTime(QTime::fromString(timeStr, "hh:mm:ss"));

}

void MainWindow::createSM()
{
    // 使用MemoryManager替代QSharedMemory
    QString readKey = "RP_" + curProj.Proj_name;
    MemoryManager::instance()->createMemory(readKey, 8 + sizeRead);
    
    QString writeKey = "WP_" + curProj.Proj_name;
    MemoryManager::instance()->createMemory(writeKey, 8 + sizeWrite);

    //qDebug()<<"smRead size:"<<smRead->size();
    //qDebug()<<"smWrite size:"<<smWrite->size();
}

void MainWindow::deleteSM()
{
    // 使用MemoryManager替代QSharedMemory后，不需要手动删除内存
    // MemoryManager会自动管理内存生命周期
    QString readKey = "RP_" + curProj.Proj_name;
    MemoryManager::instance()->releaseMemory(readKey);
    
    QString writeKey = "WP_" + curProj.Proj_name;
    MemoryManager::instance()->releaseMemory(writeKey);
}

void MainWindow::createSMInMAP()
{

    for(int i = 0; i < charMapPamList.count(); i++)
    {
        A2L_VarChar *mapChar = charMapPamList.at(i);

        // 使用MemoryManager替代QSharedMemory
        QString key = "WP_XCP_MAP_" + mapChar->Name;
        int size = mapChar->zCount * mapChar->DataSizeAG;
        void* memory = MemoryManager::instance()->createMemory(key, size + 8);
        if(!memory)
        {
            qDebug()<<XCPErrorRegistry::instance().formatError(ERR_MEMORY_OVERFLOW, QString("create map memory error, %1").arg(mapChar->Name));
        }

        //qDebug()<<"caculate map memory size:"<<size;
    }

}

void MainWindow::deleteSMInMAP()
{
    // 使用MemoryManager替代QSharedMemory后，不需要手动删除内存
    // 遍历charMapPamList，释放每个map的内存
    for(int i = 0; i < charMapPamList.count(); i++)
    {
        A2L_VarChar *mapChar = charMapPamList.at(i);
        QString key = "WP_XCP_MAP_" + mapChar->Name;
        MemoryManager::instance()->releaseMemory(key);
    }
    
    // 使用MemoryManager替代QSharedMemory，不需要清除QSharedMemory

}

void MainWindow::killModrive()
{
    QProcess p;
    QString c = "taskkill /im MODrive.exe /f";
    p.execute(c);
    p.close();
}

void MainWindow::setAcceptSlot()
{
    readCurSetting();

    setWindowTitle(curProj.Proj_name);

    curA2LProject = a2lWin->getProjectByName(curProj.a2lProjectName);

    getA2LPamsByNames();

}

void MainWindow::setLED(QLabel *label, int color, int size)
{
    if (uiStatePresenter) {
        uiStatePresenter->applyLedState(label, color, size);
        return;
    }
    // ??label?��?????????
    label->setText("");
    // ?????t??��?��
    // ????ui?????????label??��????��???????��?????��??????????��???????��????
    // ????ui?????????label??��????��???????????��?????????????????????????
    QString min_width = QString("min-width: %1px;").arg(size);              // ??��?????size
    QString min_height = QString("min-height: %1px;").arg(size);            // ??��????size
    QString max_width = QString("max-width: %1px;").arg(size);              // ??��?????size
    QString max_height = QString("max-height: %1px;").arg(size);            // ??��????size
    // ????????????????
    QString border_radius = QString("border-radius: %1px;").arg(size/8);    // ????????????size/2
    QString border = QString("border:0px solid black;");                    // ????1px???
    // ??????????????
    QString background = "background-color:";
    switch (color) {
    case 0:
        // ???
        background += "rgb(190,190,190)";
        break;
    case 1:
        // ???
        background += "rgb(255,0,0)";
        break;
    case 2:
        // ???
        background += "rgb(0,255,0)";
        break;
    case 3:
        // ???
        background += "rgb(255,255,0)";
        break;
    default:
        break;
    }

    const QString SheetStyle = min_width + min_height + max_width + max_height + border_radius + border + background;
    label->setStyleSheet(SheetStyle);
}

void MainWindow::pollingStatusChangedSlot(bool pollingStatus)
{
    int color = pollingStatus ? 2 : 1;
    setLED(ui->led_Polling, color, LED_SIZE);
}

void MainWindow::caliStatusChangedSlot(bool caliStatus)
{
    int color = caliStatus ? 2 : 1;
    setLED(ui->led_Cali, color, LED_SIZE);
}

void MainWindow::pollingStatusChangedSlot_2nd(bool pollingStatus)
{
    int color = pollingStatus ? 2 : 1;
    setLED(ui->led_Polling_2, color, LED_SIZE);
}

void MainWindow::caliStatusChangedSlot_2nd(bool caliStatus)
{
    int color = caliStatus ? 2 : 1;
    setLED(ui->led_Cali_2, color, LED_SIZE);
}

void MainWindow::on_actionLoad_triggered()
{
    ui->le_Log->setText("loading setting......");
    loadCurrentSetting();
    ui->le_Log->setText("load setting completed.");
}

void MainWindow::on_actionSetting_triggered()
{
    if(setWin == NULL)
    {
        setWin = new SettingWin(this, a2lWin, 1);
    }

    setWin->show();
}

/*
void MainWindow::on_actionConnect_triggered()
{
    if(a2lWin == NULL || xcpMaster == NULL)
        return;
    if(measPamList.isEmpty() && charPamList.isEmpty() && charMapPamList.isEmpty() && daqMeasPamList.isEmpty())
        return;


    //??????????��??????????xml
    createSM();
    createSMInMAP();

    //????????????????
    fromCharVarsToWriteSM();

    //?????MAP???????
    fromMapCharVarsToMapWriteSM();

    //??????????xml
    genXML();

    showMeasAndCharsInTable();
    showCharMapsInMapWin();

    xcpMaster->setXcpCanInterface(curProj.intfName);
    xcpMaster->setBaudrate(curProj.baudrate);
    xcpMaster->setIDMasterToSlave(curProj.id_CMD);
    xcpMaster->setIDSlaveToMaster(curProj.id_RES);

    if(!xcpMaster->XCPInit())
    {
        ui->le_Log->setText("XCP CAN init fail.");
        return;
    }
    xcpCanInitOk = true;
    ui->le_Log->setText("XCP CAN init ok.");
    setLED(ui->led_CAN, 2, LED_SIZE);


    if(!xcpMaster->XCP_Setup_Session())
    {
        ui->le_Log->setText("XCP setup fail.");
        return;
    }
    xcpSetupOk = true;
    ui->le_Log->setText("XCP ON! Ready to Communication.");
    setLED(ui->led_XCP, 2, LED_SIZE);


    ui->actionSetting->setEnabled(false);
    ui->actionConnect->setEnabled(false);
    ui->actionDisconnect->setEnabled(true);
    ui->actionLoad->setEnabled(false);


    //??????????
    charVars_Download(charPamList);

    //MAP?????????
    mapCharPamCheckThread->setMapCharPamList(charMapPamList);
    // 使用MemoryManager替代QSharedMemory，不需要设置smMapWriteHash
    mapCharPamCheckThread->setMapCharCheckRunFlag(true);
    mapCharPamCheckThread->start();

    //?????????????
    charPamCheckThread->setCharPamList(charPamList);
    charPamCheckThread->setSMKey(smKeyWrite);
    charPamCheckThread->setCharCheckRunFlag(true);
    charPamCheckThread->start();

    //polling??????????????
    xcpPollThread->setMeasPamList(measPamList);
    xcpPollThread->setSMKey(smKeyRead);
    xcpPollThread->setPollRunFlag(true);
    xcpPollThread->start();


    setLED(ui->led_Polling, 2, LED_SIZE);
    setLED(ui->led_Cali, 2, LED_SIZE);

    connect(charPamCheckThread, &CharPamCheck::addCaliAction, xcpPollThread, &XCP_Polling_Thread::addCaliActionSlot);
    connect(mapCharPamCheckThread, &MapCharPamCheckThread::addMapCaliAction, xcpPollThread, &XCP_Polling_Thread::addMapCaliActionSlot);

    mapWin->setXcpConnected(true);

}

void MainWindow::on_actionDisconnect_triggered()
{
    if(mapWin)
        mapWin->setXcpConnected(false);

    if(xcpPollThread == NULL || charPamCheckThread == NULL)
        return;
    if(a2lWin == NULL || xcpMaster == NULL)
        return;

    if(mdfRecordIns)
    {
        on_actionStop_triggered();
    }

    disconnect(charPamCheckThread, &CharPamCheck::addCaliAction, xcpPollThread, &XCP_Polling_Thread::addCaliActionSlot);
    disconnect(mapCharPamCheckThread, &MapCharPamCheckThread::addMapCaliAction, xcpPollThread, &XCP_Polling_Thread::addMapCaliActionSlot);

    charPamCheckThread->setCharCheckRunFlag(false);
    charPamCheckThread->terminate();
    charPamCheckThread->wait();

    mapCharPamCheckThread->setMapCharCheckRunFlag(false);
    mapCharPamCheckThread->terminate();
    mapCharPamCheckThread->wait();

    xcpPollThread->setPollRunFlag(false);
    xcpPollThread->terminate();
    xcpPollThread->wait();

    setLED(ui->led_Polling, 0, LED_SIZE);
    setLED(ui->led_Cali, 0, LED_SIZE);

    xcpMaster->XCPStop();
    //statusLabel->setText("XCP STOP and OFF.");
    ui->le_Log->setText("XCP STOP and OFF.");

    setLED(ui->led_XCP, 0, LED_SIZE);
    setLED(ui->led_CAN, 0, LED_SIZE);

    ui->actionSetting->setEnabled(true);
    ui->actionConnect->setEnabled(true);
    ui->actionDisconnect->setEnabled(true);
    ui->actionLoad->setEnabled(true);

    deleteSM();
    deleteSMInMAP();

}
*/

void MainWindow::on_actionRaw_triggered()
{
    showRawData = !showRawData;
}

void MainWindow::on_actionA2l_triggered()
{
    if(a2lWin == NULL)
    {
        a2lWin = new A2LProjectWin(this);
    }
    a2lWin->show();
}

void MainWindow::on_actionVisual_triggered()
{
    if(xcpMainThread == NULL)
        return;

    if(!xcpMainThread->getXcpSetupOk())
        return;

    xcpMainThread->setPollingRun(true);
    xcpMainThread->setDaqRun(true);
    xcpMainThread->setMdfRecordStatus(false);

    if (uiStatePresenter) {
        uiStatePresenter->applyRecordIndicator(ui->led_Record, "visulize but not record", "idle");
    }

}

void MainWindow::on_actionStop_triggered()
{
    if(xcpMainThread == NULL)
        return;
    if(!xcpMainThread->getXcpSetupOk())
        return;

    xcpMainThread->setPollingRun(false);
    xcpMainThread->setDaqRun(false);
    xcpMainThread->setMdfRecordStatus(false);

    if (uiStatePresenter) {
        uiStatePresenter->applyRecordIndicator(ui->led_Record, "stop visualization", "idle");
    }
}

void MainWindow::on_actionRecord_triggered()
{    

    if(xcpMainThread == NULL)
        return;
    if(!xcpMainThread->getXcpSetupOk())
        return;

    xcpMainThread->setPollingRun(true);
    xcpMainThread->setDaqRun(true);
    xcpMainThread->setMdfRecordStatus(true);

    if (uiStatePresenter) {
        uiStatePresenter->applyRecordIndicator(ui->led_Record, "Recording...", "recording");
    }

}

void MainWindow::on_actionMap_triggered()
{
    if(mapWin == NULL)
    {
        mapWin = new MapWin();
    }

    showCharMapsInMapWin();

    mapWin->show();
}

void MainWindow::transferPhyValueToRawData(A2L_VarChar *charVar, double value, char *data)
{
    if(charVar == NULL)
        return;
    QString dataType = charVar->DataType;
    int dataSize = charVar->DataSizeAG;

    int dataTypeIndex = dataTypeEnum.indexOf(dataType);

    switch (dataTypeIndex) {
    case 0:
    {
        quint8 val = (quint8)value;
        memcpy(data, (char*)&val, dataSize);
        break;
    }
    case 1:
    {
        qint8 val = (qint8)value;
        memcpy(data, (char*)&val, dataSize);
        break;
    }
    case 2:
    {
        quint16 val = (quint16)value;
        memcpy(data, (char*)&val, dataSize);
        break;
    }
    case 3:
    {
        qint16 val = (qint16)value;
        memcpy(data, (char*)&val, dataSize);
        break;
    }
    case 4:
    {
        quint32 val = (quint32)value;
        memcpy(data, (char*)&val, dataSize);
        break;
    }
    case 5:
    {
        qint32 val = (qint32)value;
        memcpy(data, (char*)&val, dataSize);
        break;
    }
    case 6:
    {
        quint64 val = (quint64)value;
        memcpy(data, (char*)&val, dataSize);
        break;
    }
    case 7:
    {
        qint64 val = (qint64)value;
        memcpy(data, (char*)&val, dataSize);
        break;
    }
    case 8:
    {
        float val = (float)value;
        memcpy(data, (char*)&val, dataSize);
        break;
    }
    case 9:
    {
        double val = (double)value;
        memcpy(data, (char*)&val, dataSize);
        break;
    }
    default:
        break;
    }
}

bool MainWindow::delExcel(QString filePath)
{
    return importExportService ? importExportService->delExcel(filePath) : false;
}

void MainWindow::convertToColName(int data, QString &res)
{
    if (importExportService) {
        importExportService->convertToColName(data, res);
    }

}

QString MainWindow::to26AlphabetString(int data)
{
    return importExportService ? importExportService->to26AlphabetString(data) : QString();
}

bool MainWindow::exportMeasToExcel(QList<A2L_VarMeas *> meaVars, QString excelPath)
{
    return importExportService ? importExportService->exportMeasToExcel(meaVars, excelPath) : false;
}

bool MainWindow::exportCharsToExcel(QList<A2L_VarChar *> charVars, QString excelPath)
{
    return importExportService ? importExportService->exportCharsToExcel(charVars, excelPath) : false;
}

bool MainWindow::exportXcpPamsToExcel()
{
    writeSetting_Simple();
    if (importExportService) {
        importExportService->exportPamSetToExcel(buildPamSet(false));
    }

    // xcp 2nd
    writeSetting_Simple_2nd();
    if (importExportService) {
        importExportService->exportPamSetToExcel(buildPamSet(true));
    }
    return true;
}

void MainWindow::on_actionSaveToExcel_triggered()
{
    exportXcpPamsToExcel();
}

void MainWindow::getA2LPamsFromExcel()
{
    if(xcpMainThread)
        return;
    ImportExportService::PamSet pamSet;
    pamSet.projectName = curProj.Proj_name;
    pamSet.secondChannel = false;
    if (importExportService) {
        importExportService->loadPamSetFromExcel(pamSet);
        applyPamSet(pamSet);
    }

    QString logStr = "poll meas count=" + QString::number(measPamList.count()) +
            ", daq meas count=" + QString::number(daqMeasPamList.count()) +
            ", char count=" + QString::number(charPamList.count());
    ui->le_Log->setText(logStr);

    qDebug()<<logStr;
    qDebug()<<"Total size of poll measVars:"<<sizeRead<<", size of daq measVars:"<<sizeReadDAQ<<", size of charVars:"<<sizeWrite;
}

void MainWindow::getA2LPamsFromExcel_2nd()
{
    if(xcpMainThread_2nd)
        return;
    ImportExportService::PamSet pamSet;
    pamSet.projectName = curProj_2nd.Proj_name;
    pamSet.secondChannel = true;
    if (importExportService) {
        importExportService->loadPamSetFromExcel(pamSet);
        applyPamSet(pamSet);
    }

    QString logStr = "2nd poll meas count=" + QString::number(measPamList_2nd.count()) +
            ", 2nd daq meas count=" + QString::number(daqMeasPamList_2nd.count()) +
            ", 2nd char count=" + QString::number(charPamList_2nd.count());
    ui->le_Log_2->setText(logStr);

    qDebug()<<logStr;
    qDebug()<<"Total size of 2nd poll measVars:"<<sizeRead_2nd<<", size of 2nd daq measVars:"<<sizeReadDAQ_2nd<<", size of 2nd charVars:"<<sizeWrite_2nd;
}

void MainWindow::importMeasFromExcel(QString excelPath, QList<A2L_VarMeas*> &measList, quint64 &byteSize)
{
    if (importExportService) {
        importExportService->importMeasFromExcel(excelPath, measList, byteSize);
        return;
    }

    ExcelOperator *measExcel = new ExcelOperator();
    measExcel->open(excelPath);
    QAxObject *sheet = measExcel->getSheet("Sheet1");
    if(sheet == NULL)
    {
        qDebug()<<"import meas: pams excel sheet not found!";
        return;
    }

    //ui->le_Log->setText("import xcp meas pams from excel......");

    //??????��??????QVariant??
    QAxObject *usedRange=sheet->querySubObject("UsedRange");
    QVariant var=usedRange->dynamicCall("value");

    int startBitIndex = 0;
    int startByteIndex = 0;

    startBitIndex += 0;

    QVariantList varRowContents=var.toList();
    const int rowCount=varRowContents.size();
    QVariantList tmp;
    for(int i=0; i<rowCount; ++i)
    {
        tmp=varRowContents[i].toList();

        A2L_VarMeas *measVar = new A2L_VarMeas();
        measVar->Name = tmp[0].toString();
        measVar->ShortName = tmp[1].toString();
        measVar->DataType = tmp[2].toString();
        measVar->ScalingFactor = tmp[3].toDouble();
        measVar->ScalingOffset = tmp[4].toDouble();
        measVar->Unit = tmp[5].toString();
        measVar->ECUAddress = (quint32)(tmp[6].toInt());
        measVar->DataSizeAG = (quint32)(tmp[7].toInt());
        measVar->LowerLimit = tmp[8].toDouble();
        measVar->UpperLimit = tmp[9].toDouble();
        measVar->ConversionType = tmp[11].toString();
        measVar->COEFFS = tmp[12].toString();
        measVar->StartByte = tmp[13].toInt();
        measVar->rate_ms = tmp[14].toFloat();
        measVar->smKey = tmp[15].toString();

        QStringList coefStrList = measVar->COEFFS.split(",");
        for (int i = 0; i < coefStrList.count(); i++)
        {
            QString str = coefStrList.at(i);
            measVar->CoefList.append(str.toDouble());
        }

        //measVar->StartByte = startByteIndex;
        //measVar->smKey = key;

        startBitIndex += (measVar->DataSizeAG * 8);
        startByteIndex = startBitIndex/8;

        measList.append(measVar);
    }

    byteSize = startBitIndex/8;

    measExcel->close();
    delete measExcel;

    //ui->le_Log->setText("import xcp meas pams from excel end.");
}

void MainWindow::importCharsFromExcel(QString excelPath, QList<A2L_VarChar *> &charList, quint64 &byteSize)
{
    if (importExportService) {
        importExportService->importCharsFromExcel(excelPath, charList, byteSize);
        return;
    }

    ExcelOperator *charExcel = new ExcelOperator();
    charExcel->open(excelPath);
    QAxObject *sheet = charExcel->getSheet("Sheet1");
    if(sheet == NULL)
    {
        qDebug()<<"import char: pams excel sheet not found!";
        return;
    }

    //ui->le_Log_2->setText("import char pams from excel......");

    //??????��??????QVariant??
    QAxObject *usedRange=sheet->querySubObject("UsedRange");
    QVariant var=usedRange->dynamicCall("value");

    int startBitIndex = 0;
    int startByteIndex = 0;

    startBitIndex += 0;

    QVariantList varRowContents=var.toList();
    const int rowCount=varRowContents.size();
    QVariantList tmp;
    for(int i=0; i<rowCount; ++i)
    {
        tmp=varRowContents[i].toList();

        A2L_VarChar *charVar = new A2L_VarChar();
        charVar->Name = tmp[0].toString();
        charVar->charType = tmp[1].toString();
        charVar->DataType = tmp[2].toString();
        charVar->ScalingFactor = tmp[3].toDouble();
        charVar->ScalingOffset = tmp[4].toDouble();
        charVar->Unit = tmp[5].toString();
        charVar->ECUAddress = (quint32)(tmp[6].toInt());
        charVar->DataSizeAG = (quint32)(tmp[7].toInt());
        charVar->LowerLimit = tmp[8].toDouble();
        charVar->UpperLimit = tmp[9].toDouble();
        charVar->setValue(tmp[10].toDouble());
        charVar->ConversionType = tmp[11].toString();
        charVar->COEFFS = tmp[12].toString();
        charVar->StartByte = tmp[13].toInt();
        charVar->smKey = tmp[14].toString();

        QStringList coefStrList = charVar->COEFFS.split(",");
        for (int i = 0; i < coefStrList.count(); i++)
        {
            QString str = coefStrList.at(i);
            charVar->CoefList.append(str.toDouble());
        }

        //charVar->StartByte = startByteIndex;
        //charVar->smKey = key;

        startBitIndex += (charVar->DataSizeAG * 8);
        startByteIndex = startBitIndex/8;

        charList.append(charVar);
    }

    byteSize = startBitIndex/8;

    charExcel->close();
    delete charExcel;

    //ui->le_Log->setText("import char pams from excel end.");
}

void MainWindow::on_actionFromExcel_triggered()
{
    if(xcpMainThread)
        return;

    ui->le_Log->setText("loading settings and pams from excel ......");

    readSetting_Simple();
    getA2LPamsFromExcel();

    ui->le_Log->setText("loading from excel completed!");

    showMeasAndCharsInTable();

    if(xcpMainThread_2nd)
        return;

    ui->le_Log_2->setText("loading 2nd settings and pams from excel ......");

    readSetting_Simple_2nd();
    getA2LPamsFromExcel_2nd();

    ui->le_Log_2->setText("loading 2nd from excel completed!");

    showMeasAndCharsInTable_2nd();
}

void MainWindow::on_actionXcpOn_triggered()
{
    if(a2lWin == NULL || xcpMaster == NULL)
        return;
    if(measPamList.isEmpty() && charPamList.isEmpty())
        return;

    // 生成XML配置文件
    genXML();

    // 显示测量和校准变量
    showMeasAndCharsInTable();

    // 配置XCP参数
    xcpMaster->setXcpCanInterface(curProj.intfName);
    xcpMaster->setBaudrate(curProj.baudrate);
    xcpMaster->setIDMasterToSlave(curProj.id_CMD);
    xcpMaster->setIDSlaveToMaster(curProj.id_RES);

    // 使用工作线程执行XCP初始化
    workerThread->setXcpMaster(xcpMaster);
    workerThread->setCharVars(charPamList);
    workerThread->setTask(WorkerThread::INIT_XCP);
    workerThread->start();

    // 禁用相关操作
    ui->actionSetting->setEnabled(false);
    ui->actionXcpOn->setEnabled(false);
    ui->actionFromExcel->setEnabled(false);

    //qDebug()<<"create sharedmemory ok.";

    //??????????
    if(!charVars_Download(charPamList))
    {
        ui->le_Log->setText("Char vars download error.");

        xcpMaster->XCPStop();
        ui->le_Log->setText("XCP STOP and OFF.");

        setLED(ui->led_XCP, 0, LED_SIZE);
        setLED(ui->led_CAN, 0, LED_SIZE);

        //ui->actionSetting->setEnabled(true);
        //ui->actionXcpOn->setEnabled(true);
        //ui->actionXcpOff->setEnabled(false);
        //ui->actionFromExcel->setEnabled(true);

        deleteSM();

    }
    //qDebug()<<"charVars download ok.";

    //?????????????
    charPamCheckThread->setCharPamList(charPamList);
    charPamCheckThread->setSMKey(smKeyWrite);
    charPamCheckThread->setCharCheckRunFlag(true);
    charPamCheckThread->start();

    //qDebug()<<"char check thread start.";

    //polling??????????????
    xcpPollThread->setMeasPamList(measPamList);
    xcpPollThread->setSMKey(smKeyRead);
    xcpPollThread->setPollRunFlag(true);
    xcpPollThread->start();

    //qDebug()<<"poll thread start.";

    setLED(ui->led_Polling, 2, LED_SIZE);
    setLED(ui->led_Cali, 2, LED_SIZE);

    connect(charPamCheckThread, &CharPamCheck::addCaliAction, xcpPollThread, &XCP_Polling_Thread::addCaliActionSlot);

}

void MainWindow::on_actionXcpOff_triggered()
{
    if(xcpPollThread == NULL || charPamCheckThread == NULL)
        return;
    if(a2lWin == NULL || xcpMaster == NULL)
        return;


    disconnect(charPamCheckThread, &CharPamCheck::addCaliAction, xcpPollThread, &XCP_Polling_Thread::addCaliActionSlot);

    charPamCheckThread->setCharCheckRunFlag(false);
    charPamCheckThread->terminate();
    charPamCheckThread->wait();

    xcpPollThread->setPollRunFlag(false);
    xcpPollThread->terminate();
    xcpPollThread->wait();

    setLED(ui->led_Polling, 0, LED_SIZE);
    setLED(ui->led_Cali, 0, LED_SIZE);

    xcpMaster->XCPStop();
    ui->le_Log->setText("XCP STOP and OFF.");

    setLED(ui->led_XCP, 0, LED_SIZE);
    setLED(ui->led_CAN, 0, LED_SIZE);

    ui->actionSetting->setEnabled(true);
    ui->actionXcpOn->setEnabled(true);
    ui->actionXcpOff->setEnabled(false);
    ui->actionFromExcel->setEnabled(true);

    deleteSM();
}

void MainWindow::on_actionUnlock_triggered()
{
    if(!showActions)
    {
        QString dlgTitle="Unlock password";
        QString txtLabel="Input the password";
        QString defaultInput="";
        QLineEdit::EchoMode echoMode=QLineEdit::Password;
        bool ok=true;
        QString text = QInputDialog::getText(this, dlgTitle,txtLabel, echoMode,defaultInput, &ok);
        if (text != verifyPassword)
            return;
    }
    showActions = !showActions;

    ui->actionLoad->setVisible(showActions);
    ui->actionConnect->setVisible(showActions);
    ui->actionDisconnect->setVisible(showActions);
    ui->actionMap->setVisible(showActions);
    ui->actionRaw->setVisible(showActions);
    ui->actionRecord->setVisible(showActions);
    ui->actionSetting->setVisible(showActions);
    ui->actionStop->setVisible(showActions);
    ui->actionVisual->setVisible(showActions);
    ui->actionA2l->setVisible(showActions);
    //ui->actionSaveToExcel->setVisible(showActions);
    //ui->actionFromExcel->setVisible(showActions);
    ui->actiontoDB->setVisible(showActions);
    ui->actionfromDB->setVisible(showActions);
}

void MainWindow::on_actionCan_triggered()
{

}

void MainWindow::on_actionExt_triggered()
{
    extCal = !extCal;
    if(extCal)
        setLED(ui->led_Ext, 2, LED_SIZE);
    else
        setLED(ui->led_Ext, 0, LED_SIZE);
}

void MainWindow::on_actionShow_triggered()
{
    showData = !showData;
    if(showData)
    {
        ui->tableWidget_Read->setVisible(true);
        ui->tableWidget_Write->setVisible(true);
        ui->tableWidget_Read_DAQ->setVisible(true);

        ui->tableWidget_Read_2->setVisible(true);
        ui->tableWidget_Write_2->setVisible(true);
        ui->tableWidget_Read_DAQ_2->setVisible(true);

        resize(QSize(1500, 900));
    }
    else
    {
        ui->tableWidget_Read->setVisible(false);
        ui->tableWidget_Write->setVisible(false);
        ui->tableWidget_Read_DAQ->setVisible(false);

        ui->tableWidget_Read_2->setVisible(false);
        ui->tableWidget_Write_2->setVisible(false);
        ui->tableWidget_Read_DAQ_2->setVisible(false);

        resize(QSize(1500, 200));
    }
}

void MainWindow::on_actionCanBind_triggered()
{
    if(process)
    {
        process->terminate();
        process->kill();
        delete process;
        process = NULL;
    }

    process = new QProcess(this);

    process->setProcessChannelMode(QProcess::MergedChannels);

    QString appPath = QApplication::applicationDirPath();

    QString program = appPath + "/HQ_Testing/HeadQuarter.exe";
    QString program_backup = appPath + "/HeadQuarter.exe";

    if(!QFileInfo::exists(program) && QFileInfo::exists(program_backup))
    {
        program = program_backup;
    }

    QStringList args;
    args.append(program);
    args.append("1");   // 1 means auto start the can module and script in headquarter

    process->start(program, args);

    if(!process->waitForStarted())
    {
        qInfo()<<"Fail to start "<<program;
        qInfo()<<"Error :"<<process->errorString();
        return;
    }
    else
    {
        qInfo()<<"CAN bind start success:"<<program;
    }
}

void MainWindow::getA2LPamsFromDB()
{
    ImportExportService::PamSet pamSet;
    pamSet.projectName = curProj.Proj_name;
    pamSet.secondChannel = false;
    pamSet.smKeyRead = smKeyRead;
    pamSet.smKeyWrite = smKeyWrite;
    if (importExportService) {
        importExportService->loadPamSetFromDb(pamSet, db);
        applyPamSet(pamSet);
    }
}

void MainWindow::getMeasPamsFromDB()
{
    getA2LPamsFromDB();
}

void MainWindow::getCharPamsFromDB()
{
    getA2LPamsFromDB();
}

void MainWindow::on_actionfromDB_triggered()
{
    ui->le_Log->setText("loading settings and pams from db......");

    //configDevice_2nd();

    getA2LPamsFromDB();

    ui->le_Log->setText("loading from db completed!");
}

bool MainWindow::saveMeasToDB()
{
    return importExportService ? importExportService->savePamSetToDb(buildPamSet(false), db) : false;
}

bool MainWindow::saveCalisToDB()
{
    return saveMeasToDB();
}

void MainWindow::on_actiontoDB_triggered()
{
    saveMeasToDB();
}

void MainWindow::on_actionCalCsv_triggered()
{
    if(calMagWin == NULL)
    {
        calMagWin = new CalManageWin(this, this->xcpMaster, this->curA2LProject);
    }
    calMagWin->setXcpMaster(this->xcpMaster);
    calMagWin->setCurA2lProject(this->curA2LProject);
    calMagWin->setXcpMainThread(this->xcpMainThread);

    calMagWin->show();
}



void MainWindow::on_actionLog_triggered()
{
    if(logWin == NULL)
    {
        logWin = new LogWin(this, true);
    }
    logWin->show();
}

void MainWindow::on_actionConfigWin_triggered()
{
    if(configWin == NULL)
    {
        configWin = new ConfigWin(this);
    }
    configWin->show();
}

void MainWindow::on_actionConnect_triggered()
{
    updateTimer->setInterval(updateRate_ms);
    connect(updateTimer, &QTimer::timeout, this, &MainWindow::SltDataUpdateInTable);
    //updateTimer->start();

    if(xcpMainThread == NULL)
    {
        xcpMainThread = new XCP_Main_Thread(this, a2lWin);

        xcpMainThread->setCurProj(this->curProj);
        //xcpMainThread->setCurA2LProject(this->curA2LProject);
        xcpMainThread->setMeasPamList(this->measPamList);
        xcpMainThread->setDaqMeasPamList(this->daqMeasPamList);
        xcpMainThread->setCharPamList(this->charPamList);
        xcpMainThread->setCharMapPamList(this->charMapPamList);
        xcpMainThread->setSmKeyRead(this->smKeyRead);
        xcpMainThread->setSizeRead(this->sizeRead);
        xcpMainThread->setSmKeyReadDAQ(this->smKeyReadDAQ);
        xcpMainThread->setSizeReadDAQ(this->sizeReadDAQ);
        xcpMainThread->setSmKeyWrite(this->smKeyWrite);
        xcpMainThread->setSizeWrite(this->sizeWrite);

        xcpMainThread->setA2lEventList(this->eventList);
        xcpMainThread->setA2lEventCanIdList(this->eventCanIdList);

        connect(xcpMainThread, &XCP_Main_Thread::xcpMsg, this, &MainWindow::sltXcpMsg);
        connect(xcpMainThread, &XCP_Main_Thread::xcpCanInitStatus, this, &MainWindow::sltXcpCanInitStatus);
        connect(xcpMainThread, &XCP_Main_Thread::xcpConnStatus, this, &MainWindow::sltXcpConnStatus);
        connect(xcpMainThread, &XCP_Main_Thread::xcpCaliStatus, this, &MainWindow::sltXcpCaliStatus);
        connect(xcpMainThread, &XCP_Main_Thread::xcpPollingStatus, this, &MainWindow::sltXcpPollingStatus);
        connect(xcpMainThread, &XCP_Main_Thread::xcpDaqRunStatus, this, &MainWindow::sltXcpDaqRunStatus);

        connect(xcpMainThread, &XCP_Main_Thread::pollingStatusChanged, this, &MainWindow::pollingStatusChangedSlot);
        connect(xcpMainThread, &XCP_Main_Thread::caliStatusChanged, this, &MainWindow::caliStatusChangedSlot);

        //connect(xcpMainThread, SIGNAL(recordTimeUpdated(QString)), this, SLOT(showRecordTimeInTimeEdit(QString)));

        pollStartTime = 0;
        pollTime = 0;
        daqStartTime = 0;
        daqTime = 0;
        caliStartTime = 0;
        caliTime = 0;

        xcpMainThread->start();
    }


    //updateTimer->setInterval(updateRate_ms);
    connect(updateTimer, &QTimer::timeout, this, &MainWindow::SltDataUpdateInTable_2nd);
    updateTimer->start();
    if(xcpMainThread_2nd == NULL)
    {
        xcpMainThread_2nd = new XCP_Main_Thread(this, a2lWin);

        xcpMainThread_2nd->setCurProj(this->curProj_2nd);
        //xcpMainThread_2nd->setCurA2LProject(this->curA2LProject);
        xcpMainThread_2nd->setMeasPamList(this->measPamList_2nd);
        xcpMainThread_2nd->setDaqMeasPamList(this->daqMeasPamList_2nd);
        xcpMainThread_2nd->setCharPamList(this->charPamList_2nd);
        xcpMainThread_2nd->setCharMapPamList(this->charMapPamList_2nd);
        xcpMainThread_2nd->setSmKeyRead(this->smKeyRead_2nd);
        xcpMainThread_2nd->setSizeRead(this->sizeRead_2nd);
        xcpMainThread_2nd->setSmKeyReadDAQ(this->smKeyReadDAQ_2nd);
        xcpMainThread_2nd->setSizeReadDAQ(this->sizeReadDAQ_2nd);
        xcpMainThread_2nd->setSmKeyWrite(this->smKeyWrite_2nd);
        xcpMainThread_2nd->setSizeWrite(this->sizeWrite_2nd);

        xcpMainThread_2nd->setA2lEventList(this->eventList_2nd);
        xcpMainThread_2nd->setA2lEventCanIdList(this->eventCanIdList_2nd);

        connect(xcpMainThread_2nd, &XCP_Main_Thread::xcpMsg, this, &MainWindow::sltXcpMsg_2nd);
        connect(xcpMainThread_2nd, &XCP_Main_Thread::xcpCanInitStatus, this, &MainWindow::sltXcpCanInitStatus_2nd);
        connect(xcpMainThread_2nd, &XCP_Main_Thread::xcpConnStatus, this, &MainWindow::sltXcpConnStatus_2nd);
        connect(xcpMainThread_2nd, &XCP_Main_Thread::xcpCaliStatus, this, &MainWindow::sltXcpCaliStatus_2nd);
        connect(xcpMainThread_2nd, &XCP_Main_Thread::xcpPollingStatus, this, &MainWindow::sltXcpPollingStatus_2nd);
        connect(xcpMainThread_2nd, &XCP_Main_Thread::xcpDaqRunStatus, this, &MainWindow::sltXcpDaqRunStatus_2nd);

        connect(xcpMainThread_2nd, &XCP_Main_Thread::pollingStatusChanged, this, &MainWindow::pollingStatusChangedSlot_2nd);
        connect(xcpMainThread_2nd, &XCP_Main_Thread::caliStatusChanged, this, &MainWindow::caliStatusChangedSlot_2nd);

        //connect(xcpMainThread_2nd, SIGNAL(recordTimeUpdated(QString)), this, SLOT(showRecordTimeInTimeEdit(QString)));

        pollStartTime_2nd = 0;
        pollTime_2nd = 0;
        daqStartTime_2nd = 0;
        daqTime_2nd = 0;
        caliStartTime_2nd = 0;
        caliTime_2nd = 0;

        xcpMainThread_2nd->start();
    }

    ui->actionSetting->setEnabled(false);
    ui->actionConnect->setEnabled(false);
    ui->actionDisconnect->setEnabled(true);
    ui->actionLoad->setEnabled(false);


}

void MainWindow::on_actionDisconnect_triggered()
{
    updateTimer->stop();

    if (recordService && recordService->isInitialized()) {
        recordService->stopRecording();
    }

    if(xcpMainThread)
    {
        xcpMainThread->quit();
        xcpMainThread->wait();

        delete xcpMainThread;
        xcpMainThread = NULL;
    }

    if(xcpMainThread_2nd)
    {
        xcpMainThread_2nd->quit();
        xcpMainThread_2nd->wait();

        delete xcpMainThread_2nd;
        xcpMainThread_2nd = NULL;
    }

    ui->actionSetting->setEnabled(true);
    ui->actionConnect->setEnabled(true);
    ui->actionDisconnect->setEnabled(true);
    ui->actionLoad->setEnabled(true);

}

void MainWindow::on_actionChart_triggered()
{
    PlotWin *plotWin = new PlotWin(this);
    plotWin->show();
}

void MainWindow::on_actionGauge_triggered()
{
    //Meter_1 *meter = new Meter_1(this);
    //meter->show();
}

void MainWindow::on_actionXCP_On_1st_triggered()
{
    updateTimer->setInterval(updateRate_ms);
    connect(updateTimer, &QTimer::timeout, this, &MainWindow::SltDataUpdateInTable);
    updateTimer->start();

    if(xcpMainThread == NULL)
    {
        xcpMainThread = new XCP_Main_Thread(this, a2lWin);

        xcpMainThread->setCurProj(this->curProj);
        xcpMainThread->setCurA2LProject(this->curA2LProject);
        xcpMainThread->setMeasPamList(this->measPamList);
        xcpMainThread->setDaqMeasPamList(this->daqMeasPamList);
        xcpMainThread->setCharPamList(this->charPamList);
        xcpMainThread->setCharMapPamList(this->charMapPamList);
        xcpMainThread->setSmKeyRead(this->smKeyRead);
        xcpMainThread->setSizeRead(this->sizeRead);
        xcpMainThread->setSmKeyReadDAQ(this->smKeyReadDAQ);
        xcpMainThread->setSizeReadDAQ(this->sizeReadDAQ);
        xcpMainThread->setSmKeyWrite(this->smKeyWrite);
        xcpMainThread->setSizeWrite(this->sizeWrite);

        connect(xcpMainThread, &XCP_Main_Thread::xcpMsg, this, &MainWindow::sltXcpMsg);
        connect(xcpMainThread, &XCP_Main_Thread::xcpCanInitStatus, this, &MainWindow::sltXcpCanInitStatus);
        connect(xcpMainThread, &XCP_Main_Thread::xcpConnStatus, this, &MainWindow::sltXcpConnStatus);
        connect(xcpMainThread, &XCP_Main_Thread::xcpCaliStatus, this, &MainWindow::sltXcpCaliStatus);
        connect(xcpMainThread, &XCP_Main_Thread::xcpPollingStatus, this, &MainWindow::sltXcpPollingStatus);
        connect(xcpMainThread, &XCP_Main_Thread::xcpDaqRunStatus, this, &MainWindow::sltXcpDaqRunStatus);

        connect(xcpMainThread, &XCP_Main_Thread::pollingStatusChanged, this, &MainWindow::pollingStatusChangedSlot);
        connect(xcpMainThread, &XCP_Main_Thread::caliStatusChanged, this, &MainWindow::caliStatusChangedSlot);

        connect(xcpMainThread, &XCP_Main_Thread::recordTimeUpdated, this, &MainWindow::showRecordTimeInTimeEdit);

        pollStartTime = 0;
        pollTime = 0;
        daqStartTime = 0;
        daqTime = 0;
        caliStartTime = 0;
        caliTime = 0;

        xcpMainThread->start();
    }

    ui->actionXCP_Set_1st->setEnabled(false);
    ui->actionXCP_On_1st->setEnabled(false);
    ui->actionXCP_Off_1st->setEnabled(true);
    ui->actionXCP_Load_1st->setEnabled(false);
}

void MainWindow::on_actionXCP_Off_1st_triggered()
{
    //updateTimer->stop();

    if(xcpMainThread)
    {
        xcpMainThread->quit();
        xcpMainThread->wait();

        delete xcpMainThread;
        xcpMainThread = NULL;
    }

    ui->actionXCP_Set_1st->setEnabled(true);
    ui->actionXCP_On_1st->setEnabled(true);
    ui->actionXCP_Off_1st->setEnabled(true);
    ui->actionXCP_Load_1st->setEnabled(true);
}

void MainWindow::on_actionXCP_Load_1st_triggered()
{
    ui->le_Log->setText("loading setting......");
    loadCurrentSetting();
    ui->le_Log->setText("load setting completed.");
}

void MainWindow::on_actionXCP_Set_1st_triggered()
{
    if(setWin == NULL)
    {
        setWin = new SettingWin(this, a2lWin, 1);
    }

    setWin->show();
}


void MainWindow::on_actionXCP_On_2nd_triggered()
{
    updateTimer->setInterval(updateRate_ms);
    connect(updateTimer, &QTimer::timeout, this, &MainWindow::SltDataUpdateInTable_2nd);
    updateTimer->start();

    if(xcpMainThread_2nd == NULL)
    {
        xcpMainThread_2nd = new XCP_Main_Thread(this, a2lWin);

        xcpMainThread_2nd->setCurProj(this->curProj_2nd);
        xcpMainThread_2nd->setCurA2LProject(this->curA2LProject_2nd);
        xcpMainThread_2nd->setMeasPamList(this->measPamList_2nd);
        xcpMainThread_2nd->setDaqMeasPamList(this->daqMeasPamList_2nd);
        xcpMainThread_2nd->setCharPamList(this->charPamList_2nd);
        xcpMainThread_2nd->setCharMapPamList(this->charMapPamList_2nd);
        xcpMainThread_2nd->setSmKeyRead(this->smKeyRead_2nd);
        xcpMainThread_2nd->setSizeRead(this->sizeRead_2nd);
        xcpMainThread_2nd->setSmKeyReadDAQ(this->smKeyReadDAQ_2nd);
        xcpMainThread_2nd->setSizeReadDAQ(this->sizeReadDAQ_2nd);
        xcpMainThread_2nd->setSmKeyWrite(this->smKeyWrite_2nd);
        xcpMainThread_2nd->setSizeWrite(this->sizeWrite_2nd);

        connect(xcpMainThread_2nd, &XCP_Main_Thread::xcpMsg, this, &MainWindow::sltXcpMsg_2nd);
        connect(xcpMainThread_2nd, &XCP_Main_Thread::xcpCanInitStatus, this, &MainWindow::sltXcpCanInitStatus_2nd);
        connect(xcpMainThread_2nd, &XCP_Main_Thread::xcpConnStatus, this, &MainWindow::sltXcpConnStatus_2nd);
        connect(xcpMainThread_2nd, &XCP_Main_Thread::xcpCaliStatus, this, &MainWindow::sltXcpCaliStatus_2nd);
        connect(xcpMainThread_2nd, &XCP_Main_Thread::xcpPollingStatus, this, &MainWindow::sltXcpPollingStatus_2nd);
        connect(xcpMainThread_2nd, &XCP_Main_Thread::xcpDaqRunStatus, this, &MainWindow::sltXcpDaqRunStatus_2nd);

        connect(xcpMainThread_2nd, &XCP_Main_Thread::pollingStatusChanged, this, &MainWindow::pollingStatusChangedSlot_2nd);
        connect(xcpMainThread_2nd, &XCP_Main_Thread::caliStatusChanged, this, &MainWindow::caliStatusChangedSlot_2nd);

        pollStartTime_2nd = 0;
        pollTime_2nd = 0;
        daqStartTime_2nd = 0;
        daqTime_2nd = 0;
        caliStartTime_2nd = 0;
        caliTime_2nd = 0;

        xcpMainThread_2nd->start();
    }

    ui->actionXCP_Set_2nd->setEnabled(false);
    ui->actionXCP_On_2nd->setEnabled(false);
    ui->actionXCP_Off_2nd->setEnabled(true);
    ui->actionXCP_Load_2nd->setEnabled(false);
}

void MainWindow::on_actionXCP_Off_2nd_triggered()
{
    //updateTimer->stop();

    if(xcpMainThread_2nd)
    {
        xcpMainThread_2nd->quit();
        xcpMainThread_2nd->wait();

        delete xcpMainThread_2nd;
        xcpMainThread_2nd = NULL;
    }

    ui->actionXCP_Set_2nd->setEnabled(true);
    ui->actionXCP_On_2nd->setEnabled(true);
    ui->actionXCP_Off_2nd->setEnabled(true);
    ui->actionXCP_Load_2nd->setEnabled(true);
}

void MainWindow::on_actionXCP_Load_2nd_triggered()
{
    ui->le_Log_2->setText("loading setting......");
    loadCurrentSetting_2nd();
    ui->le_Log_2->setText("load setting completed.");
}

void MainWindow::on_actionXCP_Set_2nd_triggered()
{
    if(setWin_2nd == NULL)
    {
        setWin_2nd = new SettingWin(this, a2lWin, 2);
    }

    setWin_2nd->show();
}

void MainWindow::on_actionRcdOn_triggered()
{
    if(!xcpMainThread && !xcpMainThread_2nd)
        return;

    if(xcpMainThread)
    {
        xcpMainThread->setPollingRun(true);
        xcpMainThread->setDaqRun(true);
    }
    if(xcpMainThread_2nd)
    {
        xcpMainThread_2nd->setPollingRun(true);
        xcpMainThread_2nd->setDaqRun(true);
    }


    initMdfRecord();

    ui->actionRcdOn->setEnabled(false);
    ui->actionRcdOff->setEnabled(true);
    ui->actionRcdSet->setEnabled(false);

    if (uiStatePresenter) {
        uiStatePresenter->applyRecordIndicator(ui->led_Record, "Recording...", "recording");
    }
}

void MainWindow::on_actionRcdOff_triggered()
{
    if(!xcpMainThread && !xcpMainThread_2nd)
        return;

    endMdfRecord();

    ui->actionRcdOn->setEnabled(true);
    ui->actionRcdOff->setEnabled(true);
    ui->actionRcdSet->setEnabled(true);

    if (uiStatePresenter) {
        uiStatePresenter->applyRecordIndicator(ui->led_Record, "visulize but not record", "idle");
    }

}

void MainWindow::on_actionRcdSet_triggered()
{

}

void MainWindow::initMdfRecord()
{
    if (!recordService)
        return;

    recordService->setRecordFileName(mdfFileName);
    recordService->startRecording(activeXcpThreads());
    qDebug() << "==mdf record init in main thread.==";
}

void MainWindow::endMdfRecord()
{
    if (recordService) {
        recordService->stopRecording();
    }
    qDebug() << "==mdf record end in main thread.==";
}

ImportExportService::PamSet MainWindow::buildPamSet(bool secondChannel) const
{
    ImportExportService::PamSet pamSet;
    pamSet.secondChannel = secondChannel;

    if (secondChannel) {
        pamSet.projectName = curProj_2nd.Proj_name;
        pamSet.measPamList = measPamList_2nd;
        pamSet.daqMeasPamList = daqMeasPamList_2nd;
        pamSet.charPamList = charPamList_2nd;
        pamSet.sizeRead = sizeRead_2nd;
        pamSet.sizeReadDAQ = sizeReadDAQ_2nd;
        pamSet.sizeWrite = sizeWrite_2nd;
        pamSet.smKeyRead = smKeyRead_2nd;
        pamSet.smKeyReadDAQ = smKeyReadDAQ_2nd;
        pamSet.smKeyWrite = smKeyWrite_2nd;
    } else {
        pamSet.projectName = curProj.Proj_name;
        pamSet.measPamList = measPamList;
        pamSet.daqMeasPamList = daqMeasPamList;
        pamSet.charPamList = charPamList;
        pamSet.sizeRead = sizeRead;
        pamSet.sizeReadDAQ = sizeReadDAQ;
        pamSet.sizeWrite = sizeWrite;
        pamSet.smKeyRead = smKeyRead;
        pamSet.smKeyReadDAQ = smKeyReadDAQ;
        pamSet.smKeyWrite = smKeyWrite;
    }

    return pamSet;
}

void MainWindow::applyPamSet(const ImportExportService::PamSet &set)
{
    if (set.secondChannel) {
        qDeleteAll(measPamList_2nd);
        qDeleteAll(daqMeasPamList_2nd);
        qDeleteAll(charPamList_2nd);

        measPamList_2nd = set.measPamList;
        daqMeasPamList_2nd = set.daqMeasPamList;
        charPamList_2nd = set.charPamList;
        sizeRead_2nd = set.sizeRead;
        sizeReadDAQ_2nd = set.sizeReadDAQ;
        sizeWrite_2nd = set.sizeWrite;
        smKeyRead_2nd = set.smKeyRead;
        smKeyReadDAQ_2nd = set.smKeyReadDAQ;
        smKeyWrite_2nd = set.smKeyWrite;

        Globals::measPamList_2nd_global = measPamList_2nd;
        Globals::daqMeasPamList_2nd_global = daqMeasPamList_2nd;
        Globals::charPamList_2nd_global = charPamList_2nd;
    } else {
        qDeleteAll(measPamList);
        qDeleteAll(daqMeasPamList);
        qDeleteAll(charPamList);

        measPamList = set.measPamList;
        daqMeasPamList = set.daqMeasPamList;
        charPamList = set.charPamList;
        sizeRead = set.sizeRead;
        sizeReadDAQ = set.sizeReadDAQ;
        sizeWrite = set.sizeWrite;
        smKeyRead = set.smKeyRead;
        smKeyReadDAQ = set.smKeyReadDAQ;
        smKeyWrite = set.smKeyWrite;

        Globals::measPamList_global = measPamList;
        Globals::daqMeasPamList_global = daqMeasPamList;
        Globals::charPamList_global = charPamList;
    }
}

QList<XCP_Main_Thread*> MainWindow::activeXcpThreads() const
{
    QList<XCP_Main_Thread*> threads;
    if (xcpMainThread) {
        threads.append(xcpMainThread);
    }
    if (xcpMainThread_2nd) {
        threads.append(xcpMainThread_2nd);
    }
    return threads;
}

void MainWindow::on_actionVisual_2nd_triggered()
{
    if(xcpMainThread_2nd == NULL)
        return;

    if(!xcpMainThread_2nd->getXcpSetupOk())
        return;

    xcpMainThread_2nd->setPollingRun(true);
    xcpMainThread_2nd->setDaqRun(true);
}

void MainWindow::on_actionStop_2nd_triggered()
{
    if(xcpMainThread_2nd == NULL)
        return;
    if(!xcpMainThread_2nd->getXcpSetupOk())
        return;

    xcpMainThread_2nd->setPollingRun(false);
    xcpMainThread_2nd->setDaqRun(false);

}

void MainWindow::on_actionCalCsv_2nd_triggered()
{
    if(calMagWin_2nd == NULL)
    {
        calMagWin_2nd = new CalManageWin(this, this->xcpMaster, this->curA2LProject_2nd);
    }
    calMagWin_2nd->setXcpMaster(this->xcpMaster);
    calMagWin_2nd->setCurA2lProject(this->curA2LProject_2nd);
    calMagWin_2nd->setXcpMainThread(this->xcpMainThread_2nd);

    calMagWin_2nd->show();
}

void MainWindow::on_actionMap_2nd_triggered()
{
    if(mapWin_2nd == NULL)
    {
        mapWin_2nd = new MapWin();
    }

    showCharMapsInMapWin_2nd();

    mapWin_2nd->show();
}

void MainWindow::on_actionXCP1_On_triggered()
{

}

void MainWindow::on_actionXCP1_Off_triggered()
{

}


void MainWindow::on_actionWorkMagWin_triggered()
{

}

// Worker thread slots
void MainWindow::onWorkerTaskStarted(const QString &message)
{
    ui->le_Log->setText(message);
    statusBar()->show();
    progBar->setRange(0, 100);
    progBar->setValue(0);
}

void MainWindow::onWorkerTaskFinished(bool success, const QString &message)
{
    ui->le_Log->setText(message);
    if (success) {
        setLED(ui->led_XCP, 2, LED_SIZE);
        setLED(ui->led_CAN, 2, LED_SIZE);
        
        // XCP初始化成功后，创建共享内存并写入数据
        createSM();
        fromCharVarsToWriteSM();
        
        // 启用XCP Off按钮
        ui->actionXcpOff->setEnabled(true);
    } else {
        setLED(ui->led_XCP, 1, LED_SIZE);
        setLED(ui->led_CAN, 0, LED_SIZE);
        
        // 初始化失败，重新启用XCP On按钮
        ui->actionXcpOn->setEnabled(true);
        ui->actionSetting->setEnabled(true);
        ui->actionFromExcel->setEnabled(true);
    }
    statusBar()->hide();
}

void MainWindow::onWorkerProgressUpdated(int progress)
{
    progBar->setValue(progress);
}

// Data updater slots
void MainWindow::onPollReadTimeUpdated(const QString &time)
{
    ui->le_readTime->setText(time);
}

void MainWindow::onDaqReadTimeUpdated(const QString &time)
{
    ui->le_readTime_DAQ->setText(time);
}

void MainWindow::onCaliWriteTimeUpdated(const QString &time)
{
    ui->le_writeTime->setText(time);
}

void MainWindow::onMeasVarsUpdated(const QList<double> &values)
{
    // 批量更新测量变量，减少界面重绘次数
    ui->tableWidget_Read->blockSignals(true);
    for (int i = 0; i < values.size() && i < measPamList.size(); i++) {
        QTableWidgetItem *valueItem = ui->tableWidget_Read->item(i, 1);
        if (valueItem) {
            valueItem->setText(QString::number(values[i], 'g', 8));
        }
    }
    ui->tableWidget_Read->blockSignals(false);
}

void MainWindow::onDaqMeasVarsUpdated(const QList<double> &values)
{
    // 批量更新DAQ测量变量，减少界面重绘次数
    ui->tableWidget_Read_DAQ->blockSignals(true);
    for (int i = 0; i < values.size() && i < daqMeasPamList.size(); i++) {
        QTableWidgetItem *valueItem = ui->tableWidget_Read_DAQ->item(i, 1);
        if (valueItem) {
            valueItem->setText(QString::number(values[i], 'g', 8));
        }
    }
    ui->tableWidget_Read_DAQ->blockSignals(false);
}

void MainWindow::onPollReadTimeUpdated_2nd(const QString &time)
{
    ui->le_readTime_Polling_2->setText(time);
}

void MainWindow::onDaqReadTimeUpdated_2nd(const QString &time)
{
    ui->le_readTime_DAQ_2->setText(time);
}

void MainWindow::onCaliWriteTimeUpdated_2nd(const QString &time)
{
    ui->le_writeTime_2->setText(time);
}

void MainWindow::onMeasVarsUpdated_2nd(const QList<double> &values)
{
    // 批量更新测量变量，减少界面重绘次数
    ui->tableWidget_Read_2->blockSignals(true);
    for (int i = 0; i < values.size() && i < measPamList_2nd.size(); i++) {
        QTableWidgetItem *valueItem = ui->tableWidget_Read_2->item(i, 1);
        if (valueItem) {
            valueItem->setText(QString::number(values[i], 'g', 8));
        }
    }
    ui->tableWidget_Read_2->blockSignals(false);
}

void MainWindow::onDaqMeasVarsUpdated_2nd(const QList<double> &values)
{
    // 批量更新DAQ测量变量，减少界面重绘次数
    ui->tableWidget_Read_DAQ_2->blockSignals(true);
    for (int i = 0; i < values.size() && i < daqMeasPamList_2nd.size(); i++) {
        QTableWidgetItem *valueItem = ui->tableWidget_Read_DAQ_2->item(i, 1);
        if (valueItem) {
            valueItem->setText(QString::number(values[i], 'g', 8));
        }
    }
    ui->tableWidget_Read_DAQ_2->blockSignals(false);
}
