#include "can_win.h"
#include "ui_can_win.h"

#include <QLineEdit>
#include <QSettings>
#include <QMessageBox>
#include <QTableView>

CAN_WIN::CAN_WIN(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::CAN_WIN)
{
    ui->setupUi(this);

    nixnet_ins = new nixnet_Thread(this);

    setWindowTitle("WIN_CAN");

    resize(900, 600);

    ui->traceTree_Read->setColumnCount(4);
    ui->traceTree_Write->setColumnCount(4);
    QStringList headerLabels;
    headerLabels<<"Message"<<"Data"<<"ID"<<"TimeStamp";
    ui->traceTree_Read->setHeaderLabels(headerLabels);

    ui->traceTree_Write->setHeaderLabels(headerLabels);

    timer_r_show->setInterval(200);
    connect(timer_r_show, SIGNAL(timeout()), this, SLOT(updateCANData_R()));
    connect(timer_r_show, SIGNAL(timeout()), this, SLOT(updateCANRawData_R()));
    timer_w_pack->setInterval(200);
    connect(timer_w_pack, SIGNAL(timeout()), this, SLOT(updateCANRawData_W()));

    //connect(ui->traceTree_Write, SIGNAL(itemDoubleClicked(QTreeWidgetItem*,int)), this, SLOT(treeMouseDoubleClicked(QTreeWidgetItem*,int)));

    connect(&xml_ins, SIGNAL(readyForLoadDBC()), this, SLOT(loadDBC_XNET()));
    connect(nixnet_ins, SIGNAL(stateChanged(bool)), this, SLOT(canStateChangedSlot(bool)));


    DSBDelegateCAN *delegate = new DSBDelegateCAN();
    ui->traceTree_Write->setItemDelegate(delegate);
    ui->traceTree_Write->setEditTriggers(QAbstractItemView::DoubleClicked);
    connect(delegate, SIGNAL(modelDataUpdated(QModelIndex, double)), this, SLOT(writeModelDataUpdatedSlot(QModelIndex, double)));

    //connect(ui->traceTree_Write,SIGNAL(itemDoubleClicked(QTreeWidgetItem*, int)),
            //this,SLOT(openPersistentEditor(QTreeWidgetItem*,int)));
    //connect(ui->traceTree_Write,SIGNAL(itemSelectionChanged()),
            //this,SLOT(slotSelectionChanged()));

}

CAN_WIN::~CAN_WIN()
{
    delete nixnet_ins;
    delete ui;
}

bool CAN_WIN::setSessionInfo()
{
    dbc_frames_r.clear();
    dbc_frames_w.clear();

    dbc_signals_r.clear();
    dbc_signals_w.clear();

    Globals::dbcSignals_r_gloabl.clear();
    Globals::dbcSignals_w_global.clear();


    sel_cluster = "Cluster";

    sel_frames_r = ui->traceTree_Read->getMsgsString();
    sel_sigs_r = ui->traceTree_Read->getSigsString();
    sel_frames_w = ui->traceTree_Write->getMsgsString();
    sel_sigs_w = ui->traceTree_Write->getSigsString();

    sel_interface = ui->ComBox_Interface->currentText();

    sel_baudrate = ui->ComBox_Baudrate->currentText().toInt();
    qDebug()<<"BaudRate="<<sel_baudrate;

    sel_canio_mode = ui->ComBox_CANIOMode->currentIndex();
    qDebug()<<"CANIO_Mode="<<ui->ComBox_CANIOMode->currentText();


    sel_canfd_baudrate = ui->ComBox_CANFD_Baudrate->currentText().toInt();
    qDebug()<<"CAN_FD BaudRate="<<sel_canfd_baudrate;

    if(sel_canio_mode == 0)
    {
        if(sel_baudrate == 0)
            return false;
    }
    else
    {
        if(sel_canfd_baudrate == 0)
            return false;
    }

    sel_termination_on = ui->checkBox_Termination->isChecked();

    sel_CustomBD_on = ui->checkBox_CustomBD->isChecked();
    sel_CustomFDBD_on = ui->checkBox_CustomFDBD->isChecked();
    sel_SamplePoint_BD = ui->doubleSpinBox_SamplePointBD->value();
    sel_SamplePoint_FDBD = ui->doubleSpinBox_SamplePointFDBD->value();


    sel_read_period = 10;
    sel_write_period = 100;

    nixnet_ins->path = sel_dbcpath;
    nixnet_ins->cluster = sel_cluster;

    nixnet_ins->port = sel_interface;
    nixnet_ins->frames_r = sel_frames_r;
    nixnet_ins->frames_w = sel_frames_w;
    nixnet_ins->baudrate = sel_baudrate;
    nixnet_ins->canio_mode = sel_canio_mode;
    nixnet_ins->canfd_baudrate = sel_canfd_baudrate;
    nixnet_ins->thread_Read->sleep_ms = sel_read_period;
    nixnet_ins->thread_Write->sleep_ms = sel_write_period;
    nixnet_ins->canTermination = sel_termination_on;

    nixnet_ins->customBD_on = sel_CustomBD_on;
    nixnet_ins->customFDBD_on = sel_CustomFDBD_on;
    nixnet_ins->samplePoint_BD = sel_SamplePoint_BD;
    nixnet_ins->samplePoint_FDBD = sel_SamplePoint_FDBD;

    nixnet_ins->sigs_r = sel_sigs_r;
    nixnet_ins->sigs_w = sel_sigs_w;


    QStringList frames_r, frames_w;
    frames_r = ui->traceTree_Read->getMsgsStringList();
    frames_w = ui->traceTree_Write->getMsgsStringList();
    nixnet_ins->setFrameNamesForRead(frames_r);
    nixnet_ins->setFrameNamesForWrite(frames_w);

    dbc_frames_r = getFramesByNames(frames_r);
    dbc_frames_w = getFramesByNames(frames_w);

    int frameCount_r = dbc_frames_r.count();
    for(int i = 0; i < frameCount_r; i++)
    {
        dbc_frame *frame = dbc_frames_r.at(i);
        int sigCount = frame->dbc_sigs.count();
        for (int j = 0; j < sigCount; j++)
        {
            dbc_signal *sig = frame->dbc_sigs.at(j);
            dbc_signals_r.append(sig);

            if(sig->Name.startsWith("headerID", Qt::CaseSensitive))
            {
                frame->pduHeaderID_sigs.append(sig);
                frame->headerIDIndexHash.insert(sig->DefaultValue, (sig->StartBit-7)/96);
                //qDebug()<<"insert header id hash:"<<sig->DefaultValue<<", "<<frame->headerIDIndexHash.value(sig->DefaultValue);
            }
        }
    }

    int frameCount_w = dbc_frames_w.count();
    for(int i = 0; i < frameCount_w; i++)
    {
        dbc_frame *frame = dbc_frames_w.at(i);
        int sigCount = frame->dbc_sigs.count();
        for (int j = 0; j < sigCount; j++)
        {
            dbc_signal *sig = frame->dbc_sigs.at(j);
            dbc_signals_w.append(sig);
        }
    }


    if (nixnet_ins->thread_Read->byteOrder_R == MOTOROLA)
    {

    }
    if (nixnet_ins->thread_Write->byteOrder_W == MOTOROLA)
    {

    }

    Globals::dbcSignals_r_gloabl = dbc_signals_r;
    Globals::dbcSignals_w_global = dbc_signals_w;

    //xml file setting
    xml_ins.setdbcPath(sel_dbcpath);
    xml_ins.setBaudrate(sel_baudrate);
    xml_ins.setInterface(sel_interface);
    xml_ins.setCANIOMode(sel_canio_mode);
    xml_ins.setCANFDBaudrate(sel_canfd_baudrate);
    xml_ins.setFramesForRead(dbc_frames_r);
    xml_ins.setFramesForWrite(dbc_frames_w);
    xml_ins.setCANTermination(sel_termination_on == false ? 0 : 1);
    xml_ins.setCustomBD(sel_CustomBD_on == false ? 0 : 1);
    xml_ins.setCustomFDBD(sel_CustomFDBD_on == false ? 0 : 1);
    xml_ins.setSamplePoint_BD(sel_SamplePoint_BD);
    xml_ins.setSamplePoint_FDBD(sel_SamplePoint_FDBD);
    xml_ins.genXML();

    return true;
}

QStringList CAN_WIN::getSMKeys()
{
    return nixnet_ins->getSMKeys();
}

void CAN_WIN::setInterface(QString intfName)
{
    ui->ComBox_Interface->setCurrentText(intfName);
    sel_interface = intfName;
    setWindowTitle("WIN_"+sel_interface);
}

void CAN_WIN::runIntf()
{
    if(!intfSettingLoad)
    {
        if(!on_actionLoadXML_triggered())
        {
            return;
        }
    }

    on_action_Connect_triggered();
}

void CAN_WIN::stopIntf()
{
    on_action_Disconnect_triggered();
}

void CAN_WIN::loadDBC_XNET()
{

    qDeleteAll(dbc_frames);
    dbc_frames.clear();

    if (sel_dbcpath.isEmpty())
    {
        sel_dbcpath = xml_ins.getdbcPath();
        if (sel_dbcpath.isEmpty())
            return;
    }

    temp_path = sel_dbcpath.toLocal8Bit();
    dbcPath = temp_path.data();

    QString name;
    QStringList fileList = sel_dbcpath.split("/", QString::SkipEmptyParts);
    qDebug()<<fileList.count();
    name = fileList.at(fileList.count()-1);
    name = name.split(".", QString::SkipEmptyParts)[0];
    temp_name = name.toLocal8Bit();
    dbcName = temp_name.data();


    QTreeWidgetItem* header = new QTreeWidgetItem;
    header->setText(0, name);
    header->setText(1, "ID");

    ui->dbcTree->setHeaderItem(header);

    status_dbc = nxdbAddAlias64(dbcName, dbcPath, sel_baudrate);
    if (status_dbc == nxSuccess)
    {
        qDebug()<<"Add DBC Alias Succeed!"<<dbcName;
    }
    else
    {
        qDebug()<<"Add DBC Alias Error."<<status_dbc<<"\n";
        char statusString[1024];
        nxStatusToString(status_dbc, sizeof(statusString), statusString);
        qDebug()<<statusString;
    }

    status_dbc = nxdbOpenDatabase(dbcName, &databaseRef);
    if (status_dbc == nxSuccess)
    {
        qDebug()<<"Open Database Succeed!";
    }

    status_dbc = nxdbGetPropertySize(databaseRef, nxPropDatabase_ClstRefs, &sizeOfClusterRef);
    numOfClusterRef = sizeOfClusterRef/sizeof(nxDatabaseRef_t);
    qDebug()<<"Num of Cluster from database:"<<numOfClusterRef;

    clusterRef = new nxDatabaseRef_t[numOfClusterRef];
    status_dbc = nxdbGetProperty(databaseRef, nxPropDatabase_ClstRefs, sizeOfClusterRef, clusterRef);
    if (status_dbc == nxSuccess)
    {
        //qDebug()<<"Get Cluster Ref Succeed! ClusterRef = "<<clusterRef[i];
    }

    //ui->dbcTree->clear();
    ui->ComBox_Cluster->clear();
    clusterHash.clear();

    for(int i = 0; i < numOfClusterRef; i++)
    {
        qDebug()<<"Get Cluster Ref Succeed! ClusterRef["<<i<<"]:"<<clusterRef[i];

        dbc_Cluster *cluster = new dbc_Cluster;


        status_dbc = nxdbGetPropertySize(clusterRef[i], nxPropClst_Name, &sizeOfClusterName);
        clusterName = new char[sizeOfClusterName];
        status_dbc = nxdbGetProperty(clusterRef[i], nxPropClst_Name, sizeOfClusterName, clusterName);
        if (status_dbc == nxSuccess)
        {
            qDebug()<<"Get Cluster Name Succeed! ClusterName = "<<clusterName;
            sel_cluster = QString(QLatin1String(clusterName));

            ui->ComBox_Cluster->addItem(clusterName);
            cluster->clusterName = sel_cluster;
        }
        else
        {
            qDebug()<<"Get Cluster Name Error."<<status_dbc<<"\n";
            char statusString[1024];
            nxStatusToString(status_dbc, sizeof(statusString), statusString);
            qDebug()<<statusString;
        }

        u64 Baudrate64 = 0;
        status_dbc = nxdbGetProperty(clusterRef[i], nxPropClst_BaudRate64, sizeof(u64), &Baudrate64);
        if (status_dbc == nxSuccess)
        {
            qDebug()<<"Get Cluster CAN Standard Baudrate Succeed! Value = "<<Baudrate64;
            //ui->ComBox_Baudrate->setCurrentText(QString::number(Baudrate64, 10));

            cluster->Baudrate = Baudrate64;
        }
        else
        {
            qDebug()<<"Get CAN Standard Baudrate Error."<<status_dbc<<"\n";
            char statusString[1024];
            nxStatusToString(status_dbc, sizeof(statusString), statusString);
            qDebug()<<statusString;
        }

        u32 CANIOMode = 1;
        /*
        status_dbc = nxdbSetProperty(clusterRef[i], nxPropClst_CanIoMode, sizeof(u32), &CANIOMode);
        if (status_dbc == nxSuccess)
        {
            qDebug()<<"Set Cluster CAN I/O Mode Succeed!";

        }
        else
        {
            qDebug()<<"Set CAN I/O Mode Error."<<status_dbc<<"\n";
            char statusString[1024];
            nxStatusToString(status_dbc, sizeof(statusString), statusString);
            qDebug()<<statusString;
        }
    */
        CANIOMode = 0;
        status_dbc = nxdbGetProperty(clusterRef[i], nxPropClst_CanIoMode, sizeof(u32), &CANIOMode);
        if (status_dbc == nxSuccess)
        {
            qDebug()<<"Get Cluster CAN I/O Mode Succeed! Value = "<<CANIOMode;
            //ui->ComBox_CANIOMode->setCurrentIndex(CANIOMode);

            cluster->CANIO_Mode = CANIOMode;
        }
        else
        {
            qDebug()<<"Get CAN I/O Mode Error."<<status_dbc<<"\n";
            char statusString[1024];
            nxStatusToString(status_dbc, sizeof(statusString), statusString);
            qDebug()<<statusString;
        }

        u64 CANFDBaudrate64 = 4000000;
    /*
        status_dbc = nxdbSetProperty(clusterRef[i], nxPropClst_CanFdBaudRate64, sizeof(u64), &CANFDBaudrate64);
        if (status_dbc == nxSuccess)
        {
            qDebug()<<"Set Cluster CAN FD Baudrate Succeed! ";

        }
        else
        {
            qDebug()<<"Set CAN FD Baudrate Error."<<status_dbc<<"\n";
            char statusString[1024];
            nxStatusToString(status_dbc, sizeof(statusString), statusString);
            qDebug()<<statusString;
        }

        status_dbc = nxdbSaveDatabase(databaseRef, dbcPath);
        if (status_dbc == nxSuccess)
        {
            qDebug()<<"Save database succeed! ";
        }
        else
        {
            qDebug()<<"Save database Error."<<status_dbc<<"\n";
            char statusString[1024];
            nxStatusToString(status_dbc, sizeof(statusString), statusString);
            qDebug()<<statusString;
        }
    */
        CANFDBaudrate64 = 0;
        status_dbc = nxdbGetProperty(clusterRef[i], nxPropClst_CanFdBaudRate64, sizeof(u64), &CANFDBaudrate64);
        if (status_dbc == nxSuccess)
        {
            qDebug()<<"Get Cluster CAN FD Baudrate Succeed! Value = "<<CANFDBaudrate64;
            //ui->ComBox_CANFD_Baudrate->setCurrentText(QString::number(CANFDBaudrate64, 10));

            cluster->CANFD_Baudrate = CANFDBaudrate64;
        }
        else
        {
            qDebug()<<"Get CAN FD Baudrate Error."<<status_dbc<<"\n";
            char statusString[1024];
            nxStatusToString(status_dbc, sizeof(statusString), statusString);
            qDebug()<<statusString;
        }


        //QTreeWidgetItem* root = new QTreeWidgetItem;
        ///root->setText(0, sel_cluster);
        //ui->dbcTree->insertTopLevelItem(0, root);

        status_dbc = nxdbGetPropertySize(clusterRef[i], nxPropClst_FrmRefs, &sizeOfFrameRef);
        qDebug()<<"Size of Frame Ref = "<<sizeOfFrameRef;
        numOfFrameRef = sizeOfFrameRef/sizeof(nxDatabaseRef_t);
        frameRef = new nxDatabaseRef_t[numOfFrameRef];

        status_dbc = nxdbGetProperty(clusterRef[i], nxPropClst_FrmRefs, sizeOfFrameRef, frameRef);
        if (status_dbc == nxSuccess)
        {
            qDebug()<<"Get Frame Ref Succeed!";
        }

        //QList<QTreeWidgetItem *> frames;
        QList<dbc_frame*> dbcFrames;
        for (int i = 0; i < numOfFrameRef; i++)
        {
            QString frameText, frameIdText;
            u32 payLoadLength;
            boolean idExtented;
            u32 timingType;
            u32 canioMode;
            double transmitTime;
            dbc_frame *cur_frame = new dbc_frame;

            status_dbc = nxdbGetPropertySize(frameRef[i], nxPropFrm_Name, &sizeOfFrameName);

            frameName = new char[sizeOfFrameName];
            status_dbc = nxdbGetProperty(frameRef[i], nxPropFrm_Name, sizeOfFrameName, frameName);
            status_dbc = nxdbGetProperty(frameRef[i], nxPropFrm_ID, sizeof(u32), &frameId);
            status_dbc = nxdbGetProperty(frameRef[i], nxPropFrm_CANExtID, sizeof(boolean), &idExtented);
            status_dbc = nxdbGetProperty(frameRef[i], nxPropFrm_CANTimingType, sizeof(u32), &timingType);
            status_dbc = nxdbGetProperty(frameRef[i], nxPropFrm_CANTxTime, sizeof(double), &transmitTime);
            status_dbc = nxdbGetProperty(frameRef[i], nxPropFrm_PayloadLen, sizeof(u32), &payLoadLength);
            status_dbc = nxdbGetProperty(frameRef[i], nxPropFrm_CANioMode, sizeof(u32), &canioMode);
            if (status_dbc == nxSuccess)
            {
                qDebug()<<"No."<<i<<"Get Frame Name : "<<frameName;
                frameText = QString(QLatin1String(frameName));
                frameList << frameText;
                frameIdText = "0x"+QString::number(frameId, 16).toUpper();

                cur_frame->Name = frameText;
                cur_frame->DLC = payLoadLength;
                cur_frame->ID = frameId;
                cur_frame->isExtented = idExtented;
                cur_frame->TimingType = timingType;
                cur_frame->CyclicTime = transmitTime*1000;
                cur_frame->canioMode = canioMode;
            }




            u32 sizeOfSigRef;
            status_dbc = nxdbGetPropertySize(frameRef[i], nxPropFrm_SigRefs, &sizeOfSigRef);
            u32 numOfSigRef = sizeOfSigRef/sizeof(nxDatabaseRef_t);
            nxDatabaseRef_t *sigRef = new nxDatabaseRef_t[numOfSigRef];
            status_dbc = nxdbGetProperty(frameRef[i], nxPropFrm_SigRefs, sizeOfSigRef, sigRef);
            if (status_dbc == nxSuccess)
            {
                qDebug()<<QString("Get Sigs Ref of Frame: %1 succeed!").arg(frameText);
            }
            //QList<QTreeWidgetItem *> sigs;
            for (int j = 0; j < numOfSigRef; j++)
            {
                dbc_signal *cur_sig = new dbc_signal;
                u32 byteOrder, sig_dataType, startbit, lengthbit;

                double defaultValue, maxValue, minValue, factor, offset;
                u32 sizeOfSigName, sizeOfSigUnit;
                status_dbc = nxdbGetPropertySize(sigRef[j], nxPropSig_Name, &sizeOfSigName);
                status_dbc = nxdbGetPropertySize(sigRef[j], nxPropSig_Unit, &sizeOfSigUnit);
                char *sigName = new char[sizeOfSigName];
                char *sigUnit = new char[sizeOfSigUnit];
                QString sigNameText, sigUnitText;

                status_dbc = nxdbGetProperty(sigRef[j], nxPropSig_Name, sizeOfSigName, sigName);
                status_dbc = nxdbGetProperty(sigRef[j], nxPropSig_Unit, sizeOfSigUnit, sigUnit);
                status_dbc = nxdbGetProperty(sigRef[j], nxPropSig_ByteOrdr, sizeof(u32), &byteOrder);
                status_dbc = nxdbGetProperty(sigRef[j], nxPropSig_DataType, sizeof(u32), &sig_dataType);
                status_dbc = nxdbGetProperty(sigRef[j], nxPropSig_StartBit, sizeof(u32), &startbit);
                status_dbc = nxdbGetProperty(sigRef[j], nxPropSig_NumBits, sizeof(u32), &lengthbit);
                status_dbc = nxdbGetProperty(sigRef[j], nxPropSig_Default, sizeof(double), &defaultValue);
                status_dbc = nxdbGetProperty(sigRef[j], nxPropSig_Max, sizeof(double), &maxValue);
                status_dbc = nxdbGetProperty(sigRef[j], nxPropSig_Min, sizeof(double), &minValue);
                status_dbc = nxdbGetProperty(sigRef[j], nxPropSig_ScaleFac, sizeof(double), &factor);
                status_dbc = nxdbGetProperty(sigRef[j], nxPropSig_ScaleOff, sizeof(double), &offset);
                if (status_dbc == nxSuccess)
                {
                    qDebug()<<"Get Signal Name : "<<sigName;
                    sigNameText = QString(QLatin1String(sigName));
                    sigUnitText = QString(QLatin1String(sigUnit));
                    cur_sig->Name = sigNameText;
                    cur_sig->Unit = sigUnitText;
                    cur_sig->ByteOrder = byteOrder;
                    cur_sig->valType = sig_dataType;
                    cur_sig->StartBit = startbit;
                    qDebug()<<"Signal startbit : "<<cur_sig->StartBit;
                    quint16 sb_oth;
                    sb_oth = startbit;
                    can_format_motorola2intel_sb_lb(&sb_oth, (quint16*)&lengthbit);
                    cur_sig->StartBit_oth = sb_oth;
                    cur_sig->LengthBit = lengthbit;
                    cur_sig->DefaultValue = defaultValue;
                    cur_sig->Maximum = maxValue;
                    cur_sig->Minimum = minValue;
                    cur_sig->ScalingFactor = factor;
                    cur_sig->ScalingOffset = offset;
                }

                //QTreeWidgetItem *sig = new QTreeWidgetItem;
                //sig->setText(0, sigNameText);
                //sigs.append(sig);

                cur_frame->dbc_sigs.append(cur_sig);
            }
            //frame->addChildren(sigs);

            //dbc_frames.append(cur_frame);
            dbc_frames.append(cur_frame);
            nixnet_ins->addFrame(cur_frame);

            dbcFrames.append(cur_frame);
        }
        //root->insertChildren(0, frames);
        //ui->dbcTree->clear();
        //ui->dbcTree->insertTopLevelItem(0,root);

        cluster->frames = dbcFrames;

        clusterHash.insert(clusterName, cluster);
    }

    connect(ui->ComBox_Cluster, SIGNAL(currentIndexChanged(QString)), this, SLOT(clusterChangedSlot(QString)));

    dbc_Cluster *cluster = clusterHash.value(ui->ComBox_Cluster->currentText());

    showClusterInfo(cluster);

}

void CAN_WIN::loadMsgToTree()
{
    if (dbc_frames_r.isEmpty() && dbc_frames_w.isEmpty())
        return;
    for (int i = 0; i < dbc_frames_r.count(); i++)
    {
        dbc_frame *frame = dbc_frames_r[i];
        QTreeWidgetItem *msgItem = new QTreeWidgetItem;
        msgItem->setText(0, frame->Name);
        for(int k = 0; k < 4; k++)
            msgItem->setBackgroundColor(k, QColor(0xFFEC8B));
        for (int j = 0; j < frame->dbc_sigs.count(); j++)
        {
            dbc_signal *signal = frame->dbc_sigs[j];
            QTreeWidgetItem *sigItem = new QTreeWidgetItem();
            sigItem->setData(1, Qt::UserRole, QVariant::fromValue(signal));

            sigItem->setText(0, signal->Name);
            sigItem->setText(1, QString::number(signal->DefaultValue, 'g', 6));
            msgItem->addChild(sigItem);
        }

        ui->traceTree_Read->addTopLevelItem(msgItem);
    }

    for (int i = 0; i < dbc_frames_w.count(); i++)
    {
        dbc_frame *frame = dbc_frames_w[i];
        QTreeWidgetItem *msgItem = new QTreeWidgetItem;
        msgItem->setText(0, frame->Name);
        for(int k = 0; k < 4; k++)
            msgItem->setBackgroundColor(k, QColor(0xC1FFC1));
        for (int j = 0; j < frame->dbc_sigs.count(); j++)
        {
            dbc_signal *signal = frame->dbc_sigs[j];
            QTreeWidgetItem *sigItem = new QTreeWidgetItem();
            sigItem->setData(1, Qt::UserRole, QVariant::fromValue(signal));
            sigItem->setText(0, signal->Name);
            sigItem->setText(1, QString::number(signal->DefaultValue, 'g', 6));
            msgItem->addChild(sigItem);
        }

        ui->traceTree_Write->addTopLevelItem(msgItem);
    }
}

void CAN_WIN::loadMsgToModel()
{
    if (dbc_frames_r.isEmpty() && dbc_frames_w.isEmpty())
        return;
    for (int i = 0; i < dbc_frames_r.count(); i++)
    {
        dbc_frame *frame = dbc_frames_r[i];
        QTreeWidgetItem *msgItem = new QTreeWidgetItem();
        msgItem->setData(0, Qt::UserRole, QVariant::fromValue(frame));
        msgItem->setText(0, frame->Name);
        for(int k = 0; k < 4; k++)
            msgItem->setBackgroundColor(k, QColor(0xFFEC8B));
        for (int j = 0; j < frame->dbc_sigs.count(); j++)
        {
            dbc_signal *signal = frame->dbc_sigs[j];
            QTreeWidgetItem *sigItem = new QTreeWidgetItem();
            sigItem->setData(1, Qt::UserRole, QVariant::fromValue(signal));

            sigItem->setText(0, signal->Name);
            sigItem->setText(1, QString::number(signal->DefaultValue, 'g', 6));
            msgItem->addChild(sigItem);

        }

        ui->traceTree_Read->addTopLevelItem(msgItem);
    }

    for (int i = 0; i < dbc_frames_w.count(); i++)
    {
        dbc_frame *frame = dbc_frames_w[i];
        QTreeWidgetItem *msgItem = new QTreeWidgetItem();
        msgItem->setData(0, Qt::UserRole, QVariant::fromValue(frame));
        msgItem->setText(0, frame->Name);
        for(int k = 0; k < 4; k++)
            msgItem->setBackgroundColor(k, QColor(0xC1FFC1));
        for (int j = 0; j < frame->dbc_sigs.count(); j++)
        {
            dbc_signal *signal = frame->dbc_sigs[j];
            QTreeWidgetItem *sigItem = new QTreeWidgetItem();
            sigItem->setData(1, Qt::UserRole, QVariant::fromValue(signal));
            sigItem->setText(0, signal->Name);
            sigItem->setText(1, QString::number(signal->DefaultValue, 'g', 6));
            msgItem->addChild(sigItem);

            sigItem->setFlags(sigItem->flags() | Qt::ItemIsEditable);
        }

        ui->traceTree_Write->addTopLevelItem(msgItem);
    }

}


void CAN_WIN::updateCANData_R()
{
    if (nixnet_ins->thread_Read->isRunning())
    {

        u32 indexOfSig = 0;

        nxFrameVar_t *Frame = (nxFrameVar_t *)nixnet_ins->thread_Read->CANFD_FrameBuffer_R;
        //int i = 0;

        for(int i = 0; i < nixnet_ins->thread_Read->numOfFrame; i++)
        {
            u32 numOfChildren = ui->traceTree_Read->topLevelItem(i)->childCount();
            for (int j = 0; j < numOfChildren; j++)
            {
                QString sigValueText;
                f64 sigValue = nixnet_ins->thread_Read->sigValueBuffer[indexOfSig];
                sigValueText = QString::number(sigValue, 'g', 6);
                ui->traceTree_Read->topLevelItem(i)->child(j)->setText(1, sigValueText);
                indexOfSig++;
            }
        }

    }
}

void CAN_WIN::updateCANRawData_R()
{
    if (nixnet_ins->thread_Read->isRunning())
    {

        QString dataText = "";
        QString timeText = "";
        QString idText = "";
        u8 *Frame = nixnet_ins->thread_Read->CANFD_FrameBuffer_R;
        for (int i = 0; i < nixnet_ins->thread_Read->numOfFrame; i++)
        {            
            dbc_frame *curFrame = dbc_frames_r.at(i);
            u32 dlc = curFrame->DLC;
            u32 sizeFrame = nxFrameSize(dlc);
            dataText = "";
            for (int j = 0; j < dlc; j++)
            {
                if (j % 8 == 0 && j != 0)
                    dataText += "\n";
                QString tempstr = QString(tr("%1")).arg(*(Frame+16+j), 2, 16, QChar('0'));
                dataText += tempstr;
                if (j != (dlc-1))
                    dataText += " ";
            }
            ui->traceTree_Read->topLevelItem(i)->setText(1, dataText.toUpper());
            idText = "0x" + QString::number(*(u32*)(Frame+8), 16).toUpper();
            ui->traceTree_Read->topLevelItem(i)->setText(2, idText);

            f64 *absTime = (nixnet_ins->thread_Read->absTime + i);

            timeText = QString::number(*absTime, 'g', 6);
            ui->traceTree_Read->topLevelItem(i)->setText(3, timeText);

            Frame += sizeFrame;

        }


    }
}

void CAN_WIN::updateCANData_W()
{
    if (nixnet_ins->thread_Write->isRunning())
    {
        int numOfFrames_W = nixnet_ins->thread_Write->numOfFrame;
        int indexOfSig = 0;
        for (int i = 0; i < numOfFrames_W; i++)
        {
            int numOfSigs = ui->traceTree_Write->topLevelItem(i)->childCount();
            QString msgName = ui->traceTree_Write->topLevelItem(i)->text(0);
            for (int j = 0; j < numOfSigs; j++)
            {                
                //QLineEdit *lineEditItem = ui->traceTree_Write->allLineEdit[msgName][j];
                //QString sigValueText = lineEditItem->text();
                QString sigValueText =ui->traceTree_Write->topLevelItem(i)->child(j)->text(1);
                f64 sigValue = sigValueText.toDouble();

                nixnet_ins->thread_Write->sigValueBuffer[indexOfSig] = sigValue;
                indexOfSig++;
            }
        }
        //qDebug()<<"sigValue_W[0]"<<nixnet_ins->thread_Write->sigValueBuffer[0];
        nixnet_ins->thread_Write->setWriteOnceEnable(true);

    }
}

void CAN_WIN::updateCANRawData_W()
{
    if (nixnet_ins->thread_Write->isRunning())
    {

        QString dataText = "";
        QString idText = "", timeText;
        u8 *Frame = nixnet_ins->thread_Write->CANFD_FrameBuffer_W;
        for(int i = 0; i < nixnet_ins->thread_Write->numOfFrame; i++)
        {
            //nxFrameCANFD_t *Frame = nixnet_ins->thread_Write->CANFD_FrameBuffer_W + i;
            dbc_frame *curFrame = dbc_frames_w.at(i);
            u32 dlc = curFrame->DLC;
            u32 sizeFrame = nxFrameSize(dlc);
            //u32 sizeFrame = dlc+16;
            dataText = "";
            for (int k = 0; k < dlc; k++)
                {
                    if (k % 8 == 0 && k != 0)
                        dataText += "\n";
                    QString tempstr = QString(tr("%1")).arg(*(Frame+16+k), 2, 16, QChar('0'));
                    dataText += tempstr;
                    if (k != (dlc-1))
                        dataText += " ";
                }
            ui->traceTree_Write->topLevelItem(i)->setText(1, dataText.toUpper());
            idText = "0x" + QString::number(*(u32*)(Frame+8), 16).toUpper();
            ui->traceTree_Write->topLevelItem(i)->setText(2, idText);
            f64 absTime = (*(f64*)Frame);
            timeText = QString::number(absTime, 'g', 6);
            ui->traceTree_Write->topLevelItem(i)->setText(3, timeText);

            Frame += sizeFrame;
        }

    }
}

void CAN_WIN::updateCANSigValueInSM_W(int index, qreal value)
{
    if (index == -1)
        return;
    QSharedMemory sharedMemory_Sig_W;
    sharedMemory_Sig_W.setKey("WP_"+sel_interface);
    if (!sharedMemory_Sig_W.isAttached())
    {
        if(!sharedMemory_Sig_W.attach())
        {
            qDebug()<<"Unable to attach writeSignals SharedMemory.";
        }
    }
    //qDebug()<<"index="<<index<<", value"<<value;

    char *to = (char*)(sharedMemory_Sig_W.data()) + index*sizeof(f64);
    char *from = (char*)&value;

    sharedMemory_Sig_W.lock();    
    memcpy(to+8, from, sizeof(qreal));
    sharedMemory_Sig_W.unlock();

    nixnet_ins->thread_Write->setWriteOnceEnable(true);

}

void CAN_WIN::itemValueCheck(QTreeWidgetItem *item, int column)
{
    if (column == 1 && item->parent())
    {
        qDebug()<<item->text(0)<<" Value Change to "<<item->text(1);
        QString valueText = item->text(1);

    }
}

void CAN_WIN::treeMouseDoubleClicked(QTreeWidgetItem *item, int column)
{

    curItem = item;

    if (column == 1 && (item->parent() != NULL) && (item->child(0) == NULL))
    {
        QString sigName = item->text(0);
        indexOfEditSig = getWriteSigIndexByName(sigName);

        input = new QLineEdit;
        input->setText(item->text(1));

        //edit_SM.setKey(item->parent()->text(0));

        ui->traceTree_Write->setItemWidget(item, 1, input);
        input->setVisible(true);

        connect(input, SIGNAL(editingFinished()), this, SLOT(inputSlot()));
    }

}

void CAN_WIN::inputSlot()
{
    QString valueText = input->text();
    curItem->setText(1, valueText);
    ui->traceTree_Write->removeItemWidget(curItem, 1);
    disconnect(input, SIGNAL(editingFinished()), this, SLOT(inputSlot()));
    input = NULL;
    //updateCANData_W();
    updateCANSigValueInSM_W(indexOfEditSig, valueText.toDouble());

}

bool CAN_WIN::on_action_Connect_triggered()
{
    if(!setSessionInfo())
        return false;

    if (!nixnet_ins->deviceConnect())
        return false;

    uploadWriteSigValue();

    ui->traceTree_Read->setAcceptDrops(false);
    ui->traceTree_Read->setDragEnabled(false);
    ui->traceTree_Write->setAcceptDrops(false);
    ui->traceTree_Write->setDragEnabled(false);
    ui->action_Load_DBC->setEnabled(false);
    ui->dbcTree->setAcceptDrops(false);
    ui->ComBox_Baudrate->setEnabled(false);
    ui->ComBox_Interface->setEnabled(false);
    ui->ComBox_CANFD_Baudrate->setEnabled(false);
    ui->ComBox_CANIOMode->setEnabled(false);
    ui->actionClearFrame->setEnabled(false);
    ui->actionClearFrameR->setEnabled(false);
    ui->actionClearFrameW->setEnabled(false);
    ui->actionLoadXML->setEnabled(false);
    ui->actionSaveXML->setEnabled(false);
    ui->action_Connect->setEnabled(false);
    ui->checkBox_Termination->setEnabled(false);
    ui->checkBox_CustomBD->setEnabled(false);
    ui->checkBox_CustomFDBD->setEnabled(false);
    ui->doubleSpinBox_SamplePointBD->setEnabled(false);
    ui->doubleSpinBox_SamplePointFDBD->setEnabled(false);

    timer_r_show->start();
    timer_w_pack->start();

    return true;

}

void CAN_WIN::on_action_Disconnect_triggered()
{
    timer_r_show->stop();
    timer_w_pack->stop();

    nixnet_ins->deviceDisconnect();

    ui->traceTree_Read->setAcceptDrops(true);
    ui->traceTree_Read->setDragEnabled(true);
    ui->traceTree_Write->setAcceptDrops(true);
    ui->traceTree_Write->setDragEnabled(true);
    ui->action_Load_DBC->setEnabled(true);
    ui->dbcTree->setAcceptDrops(true);
    ui->ComBox_Baudrate->setEnabled(true);
    ui->ComBox_Interface->setEnabled(true);
    ui->ComBox_CANFD_Baudrate->setEnabled(true);
    ui->ComBox_CANIOMode->setEnabled(true);
    ui->actionClearFrame->setEnabled(true);
    ui->actionClearFrameR->setEnabled(true);
    ui->actionClearFrameW->setEnabled(true);
    ui->actionLoadXML->setEnabled(true);
    ui->actionSaveXML->setEnabled(true);
    ui->action_Connect->setEnabled(true);
    ui->checkBox_Termination->setEnabled(true);
    ui->checkBox_CustomBD->setEnabled(true);
    ui->checkBox_CustomFDBD->setEnabled(true);
    ui->doubleSpinBox_SamplePointBD->setEnabled(true);
    ui->doubleSpinBox_SamplePointFDBD->setEnabled(true);
}

void CAN_WIN::on_action_Load_DBC_triggered()
{
    sel_dbcpath = QFileDialog::getOpenFileName(this,tr("Load dbc file..."),"D:", tr("dbc file(*.dbc *.DBC *.xml *.arxml)"));
    qDebug()<<"Selected DBC File Path = "<<sel_dbcpath;
    if (!sel_dbcpath.isEmpty())
        loadDBC_XNET();
}


void CAN_WIN::on_actionLoad_triggered()
{
    sel_dbcpath = QFileDialog::getOpenFileName(this,tr("Load dbc file..."),"D:", tr("dbc file(*.dbc *.DBC *.xml *.arxml)"));
    qDebug()<<"Selected DBC File Path = "<<sel_dbcpath;
    if (!sel_dbcpath.isEmpty())
        loadDBC_XNET();
}

QList<dbc_frame*> CAN_WIN::getFramesByNames(QStringList frameNameList)
{
    QList<dbc_frame*> dbc_frames_temp;
    //qDebug()<<"frameName num="<<frameNameList.count();
    for (int i = 0; i < frameNameList.count(); i++)
    {
        QString frameString = frameNameList[i];
        //qDebug()<<"frameString = "<<frameString;
        int j = 0;
        dbc_frame *frame_temp = dbc_frames[j];
        while ((j < dbc_frames.count()) && (frame_temp->Name != frameString))
        {
            j++;
            frame_temp = dbc_frames[j];
        }
        //qDebug()<<"frame_temp Name ="<<frame_temp.Name;
        dbc_frames_temp.append(frame_temp);        
    }
    return dbc_frames_temp;
}

void CAN_WIN::uploadWriteSigValue()
{
    //qDebug()<<"dbc_frames_w num ="<<dbc_frames_w.count();
    for (int i = 0; i < dbc_frames_w.count(); i++)
    {
        dbc_frame *frame = dbc_frames_w[i];
        for (int j = 0; j < frame->dbc_sigs.count(); j++)
        {
            dbc_signal *sig = frame->dbc_sigs[j];
            QString valueText = QString::number(sig->DefaultValue, 'g', 6);
            ui->traceTree_Write->topLevelItem(i)->child(j)->setText(1, valueText);
        }
    }

}

bool CAN_WIN::on_actionLoadXML_triggered()
{
    /*
    int ret = QMessageBox::warning(this, tr("Clear frames and import"),
                                     tr("Are you confirmed to clear all frames and import the frames?"),
                                     QMessageBox::Ok | QMessageBox::Cancel);
    if (ret == QMessageBox::Cancel)
        return false;
    */
    sel_frames_r.clear();
    sel_frames_w.clear();
    dbc_frames_r.clear();
    dbc_frames_w.clear();
    ui->traceTree_Read->clear();
    ui->traceTree_Write->clear();

    readTreeModel.clear();
    writeTreeModel.clear();

    sel_interface = ui->ComBox_Interface->currentText();
    xml_ins.setInterface(sel_interface);

    if (!xml_ins.readXML())
    {
        qDebug()<<"xml Read Error.";
        return false;
    }
    sel_dbcpath = xml_ins.getdbcPath();
    sel_interface = xml_ins.getInterface();
    sel_baudrate = xml_ins.getBaudrate();
    sel_canio_mode = xml_ins.getCANIOMode();
    sel_canfd_baudrate = xml_ins.getCANFDBaudrate();
    dbc_frames_r = xml_ins.getFramesForRead();
    dbc_frames_w = xml_ins.getFramesForWrite();
    sel_termination_on = (xml_ins.getCANTermination() > 0);
    sel_CustomBD_on = (xml_ins.getCustomBD() > 0);
    sel_CustomFDBD_on = (xml_ins.getCustomFDBD() > 0);
    sel_SamplePoint_BD = xml_ins.getSamplePoint_BD();
    sel_SamplePoint_FDBD = xml_ins.getSamplePoint_FDBD();

    ui->ComBox_Interface->setCurrentText(sel_interface);
    ui->ComBox_Baudrate->setCurrentText(QString::number(sel_baudrate));
    ui->ComBox_CANIOMode->setCurrentIndex(sel_canio_mode);
    ui->ComBox_CANFD_Baudrate->setCurrentText(QString::number(sel_canfd_baudrate));
    ui->checkBox_Termination->setChecked(sel_termination_on);
    ui->checkBox_CustomBD->setChecked(sel_CustomBD_on);
    ui->checkBox_CustomFDBD->setChecked(sel_CustomFDBD_on);
    ui->doubleSpinBox_SamplePointBD->setValue(sel_SamplePoint_BD);
    ui->doubleSpinBox_SamplePointFDBD->setValue(sel_SamplePoint_FDBD);

    //loadMsgToTree();
    loadMsgToModel();

    intfSettingLoad = true;

    return true;
}

QList<dbc_signal *> CAN_WIN::getReadSignals()
{
    return dbc_signals_r;
}

QList<dbc_signal *> CAN_WIN::getWriteSignals()
{
    return dbc_signals_w;
}

QString CAN_WIN::getSelIntf()
{
    return sel_interface;
}

qreal *CAN_WIN::getSigValueBufferWritePtr()
{
    return nixnet_ins->thread_Write->sigValueBuffer;
}

bool CAN_WIN::getReadThreadRunningStatus()
{
    return nixnet_ins->thread_Read->isRunning();
}

int CAN_WIN::getWriteSigIndexByName(QString sigName)
{
    if (dbc_signals_w.isEmpty())
        return -1;
    int index = 0;
    dbc_signal *sig = dbc_signals_w.at(index);
    while ((sig->Name != sigName) || (index >= dbc_signals_w.count()))
    {
        index++;
        sig = dbc_signals_w.at(index);
    }

    if (index >= dbc_signals_w.count())
        return -1;
    else
        return index;

}

bool CAN_WIN::getState()
{
    bool run_Read = nixnet_ins->thread_Read->isRunning();
    bool run_Write = nixnet_ins->thread_Write->isRunning();


    if (run_Read || run_Write)
        return true;
    else
        return false;
}

void CAN_WIN::canStateChangedSlot(bool state)
{
    emit canStateChanged(sel_interface, state);
}

void CAN_WIN::winClosedSlot(QString str)
{
    if (nixnet_ins->thread_Read->isRunning() || nixnet_ins->thread_Write->isRunning())
    {
        qDebug()<<"nixnet thread runninng";
        this->hide();
    }
    else
    {
        qDebug()<<"nixnet thread not runninng";
        //emit winClosed(str);
    }
}

void CAN_WIN::writeModelDataUpdatedSlot(QModelIndex index, double value)
{
    ui->traceTree_Write->setCurrentIndex(index);
    QTreeWidgetItem *item =  ui->traceTree_Write->currentItem();

    dbc_signal *signal = item->data(1,Qt::UserRole).value<dbc_signal*>();
    indexOfEditSig = getWriteSigIndexByName(signal->Name);

    //qDebug()<<"signal:"<<signal->Name<<", update value:"<<value;

    updateCANSigValueInSM_W(indexOfEditSig, value);
}



qreal *CAN_WIN::getSigValueBufferReadPtr()
{
    return nixnet_ins->thread_Read->sigValueBuffer;
}

bool CAN_WIN::nativeEvent(const QByteArray &eventType, void *message, long *result)
{

    MSG *param = static_cast<MSG *>(message);

    int userIndex = param->message;
    if (userIndex == RECORD_REMIND)
    {
        int record_flag = param->wParam;
        qDebug()<<"Record Remind! record flag = "<<record_flag;
        //鎺ュ埌娑堟伅鍚庤Е鍙戞暟鎹褰
        if (record_flag)
            nixnet_ins->createRecordFile();
        else
            nixnet_ins->closeRecordFile();


        return true;
    }

    return QWidget::nativeEvent(eventType, message, result);
}

void CAN_WIN::closeEvent(QCloseEvent *event)
{
    emit winClosed(windowTitle());
}


void CAN_WIN::on_actionClearFrame_triggered()
{
    int ret = QMessageBox::warning(this, tr("Clear frames"),
                                     tr("Are you confirmed to clear all selected frames?"),
                                     QMessageBox::Ok | QMessageBox::Cancel);
    if (ret == QMessageBox::Cancel)
        return;

    sel_frames_r.clear();
    sel_frames_w.clear();
    dbc_frames_r.clear();
    dbc_frames_w.clear();
    ui->traceTree_Read->clear();
    ui->traceTree_Write->clear();

    intfSettingLoad = false;
}

void CAN_WIN::on_actionClearFrameR_triggered()
{
    int ret = QMessageBox::warning(this, tr("Clear read frames"),
                                     tr("Are you confirmed to clear all selected read frames?"),
                                     QMessageBox::Ok | QMessageBox::Cancel);
    if (ret == QMessageBox::Cancel)
        return;

    sel_frames_r.clear();
    dbc_frames_r.clear();
    ui->traceTree_Read->clear();

}

void CAN_WIN::on_actionClearFrameW_triggered()
{
    int ret = QMessageBox::warning(this, tr("Clear write frames"),
                                     tr("Are you confirmed to clear all selected write frames?"),
                                     QMessageBox::Ok | QMessageBox::Cancel);
    if (ret == QMessageBox::Cancel)
        return;

    sel_frames_w.clear();
    dbc_frames_w.clear();
    ui->traceTree_Write->clear();
}

void CAN_WIN::on_actionReadSIG_triggered()
{
    //QStandardItemModel *model = new QStandardItemModel();
    model_r = new QStandardItemModel();
    QStringList labels = QObject::trUtf8("SIG, Value, Unit").simplified().split(",");
    model_r->setHorizontalHeaderLabels(labels);

    QStandardItem *item = 0;

    int sigCount = dbc_signals_r.count();
    for(int i = 0; i < sigCount; i++)
    {
        dbc_signal *sig = dbc_signals_r.at(i);
        qreal sigValue = *(nixnet_ins->thread_Read->sigValueBuffer + i);
        item = new QStandardItem(sig->Name+"_"+sel_interface);
        model_r->setItem(i, 0, item);
        item = new QStandardItem(sig->Unit);
        model_r->setItem(i, 2, item);
        item = new QStandardItem(QString::number(sigValue, 'g', 6));
        model_r->setItem(i, 1, item);
    }

    QTimer *timer_r = new QTimer;
    connect(timer_r, SIGNAL(timeout()), this, SLOT(sigValueReadUpdate()));
    timer_r->setInterval(500);
    timer_r->start();

    QTableView *view = new QTableView;
    view->setModel(model_r);
    view->setWindowTitle("Sigs_Read_"+sel_interface);
    view->show();
}

void CAN_WIN::on_actionWriteSIG_triggered()
{

    //QStandardItemModel *model = new QStandardItemModel();
    model_w = new QStandardItemModel();
    QStringList labels = QObject::trUtf8("SIG, Value, Unit").simplified().split(",");
    model_w->setHorizontalHeaderLabels(labels);

    QStandardItem *item = 0;

    int sigCount = dbc_signals_w.count();
    for(int i = 0; i < sigCount; i++)
    {
        dbc_signal *sig = dbc_signals_w.at(i);
        qreal sigValue = *(nixnet_ins->thread_Write->sigValueBuffer + i);
        item = new QStandardItem(sig->Name+"_"+sel_interface);
        model_w->setItem(i, 0, item);
        item = new QStandardItem(sig->Unit);
        model_w->setItem(i, 2, item);
        item = new QStandardItem(QString::number(sigValue, 'g', 6));
        model_w->setItem(i, 1, item);
    }

    QTimer *timer_w = new QTimer;
    connect(timer_w, SIGNAL(timeout()), this, SLOT(sigValueWriteUpdate()));
    timer_w->setInterval(500);
    timer_w->start();

    QTableView *view = new QTableView;
    view->setModel(model_w);
    view->setWindowTitle("Sigs_Write_"+sel_interface);
    view->show();
}

void CAN_WIN::sigValueWriteUpdate()
{
    QStandardItem *item = 0;

    int sigCount = dbc_signals_w.count();
    for(int i = 0; i < sigCount; i++)
    {
        qreal sigValue = *(nixnet_ins->thread_Write->sigValueBuffer + i);
        item = model_w->item(i, 1);
        item->setText(QString::number(sigValue, 'g', 6));
    }
}

void CAN_WIN::sigValueReadUpdate()
{
    QStandardItem *item = 0;

    int sigCount = dbc_signals_r.count();
    for(int i = 0; i < sigCount; i++)
    {
        qreal sigValue = *(nixnet_ins->thread_Read->sigValueBuffer + i);
        item = model_r->item(i, 1);
        item->setText(QString::number(sigValue, 'g', 6));
    }
}

void CAN_WIN::clusterChangedSlot(QString clusterName)
{
    if(clusterName.isEmpty())
        return;
    dbc_Cluster *cluster = clusterHash.value(clusterName);

    showClusterInfo(cluster);
}

void CAN_WIN::showClusterInfo(dbc_Cluster *cluster)
{
    if(cluster == NULL)
        return;

    ui->dbcTree->clear();

    ui->ComBox_Baudrate->setCurrentText(QString::number(cluster->Baudrate));
    ui->ComBox_CANFD_Baudrate->setCurrentText(QString::number(cluster->CANFD_Baudrate));
    ui->ComBox_CANIOMode->setCurrentIndex(cluster->CANIO_Mode);

    QTreeWidgetItem* root = new QTreeWidgetItem();
    root->setText(0, cluster->clusterName);

    QList<QTreeWidgetItem *> frameList;

    for(int i = 0; i < cluster->frames.count(); i++)
    {
        dbc_frame *frame = cluster->frames.at(i);

        QString frameText = frame->Name;
        QString frameIdText = "0x"+QString::number(frame->ID, 16).toUpper();

        QTreeWidgetItem* frameItem = new QTreeWidgetItem();
        frameItem->setText(0, frameText);
        frameItem->setText(1, frameIdText);
        frameList.append(frameItem);

        QList<QTreeWidgetItem*> sigList;

        for(int j = 0; j < frame->dbc_sigs.count(); j++)
        {
            dbc_signal *sig = frame->dbc_sigs.at(j);

            QTreeWidgetItem *sigItem = new QTreeWidgetItem();
            sigItem->setText(0, sig->Name);
            sigList.append(sigItem);
        }

        frameItem->addChildren(sigList);
    }

    root->insertChildren(0, frameList);

    ui->dbcTree->insertTopLevelItem(0, root);

    ui->dbcTree->setExpandsOnDoubleClick(true);
}

void CAN_WIN::on_actionSaveXML_triggered()
{
    int result = QMessageBox::warning(this,
                         QString::fromLocal8Bit("保存配置"),
                         QString::fromLocal8Bit("保存当前信号配置为默认? \n "),
                         QMessageBox::Ok | QMessageBox::Cancel, QMessageBox::NoButton
                         );
    if (result == QMessageBox::Cancel)
    {

    }
    else
    {
        if(!setSessionInfo())
        {
            qDebug()<<"Save xml file error.";
        }
    }
}

void CAN_WIN::on_actionFD_Rebuild_triggered()
{
    if(!nixnet_ins)
        return;

    fdReBuildFlag = !fdReBuildFlag;
    nixnet_ins->setFDRebuildFlag(fdReBuildFlag);

}
