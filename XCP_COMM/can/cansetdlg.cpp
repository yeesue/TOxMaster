#include "cansetdlg.h"
#include "ui_cansetdlg.h"
#include <QDir>
#include <QDesktopWidget>

CanSetDlg::CanSetDlg(QWidget *parent, QString work_name, QString fun_name, QString intf_name, QString set_file_path) :
    QDialog(parent),
    workName(work_name), funName(fun_name),
    intfName(intf_name), canSetFilePath(set_file_path),
    ui(new Ui::CanSetDlg)
{
    ui->setupUi(this);
    setWindowTitle("CAN/CANFD Setting");

    QDesktopWidget *deskTop =  QApplication::desktop();
    resize(deskTop->width()*3/4, deskTop->height()*3/4);

    ui->traceTree_Read->setColumnCount(6);
    ui->traceTree_Write->setColumnCount(6);
    QStringList headerLabels;
    headerLabels<<"Message"<<"Data"<<"Pam0"<<"Pam1"<<"Pam2"<<"Pam3";

    ui->traceTree_Read->setHeaderLabels(headerLabels);
    ui->traceTree_Write->setHeaderLabels(headerLabels);

    ui->traceTree_Read->setColumnWidth(0, 300);
    ui->traceTree_Write->setColumnWidth(0, 300);

    //canSetFilePath = QApplication::applicationDirPath() + canSetFilePath;
    QString fileName = workName.toLower() + "-" + funName.toLower() + "-setting.xml";
    QString filePath_ref = "/work/" + workName + "/" + fileName;
    QString filePath = QApplication::applicationDirPath() + filePath_ref;

    if(canSetFilePath.isEmpty())
    {

        canSetFileName = fileName;
        canSetFilePath = filePath;
    }
    else
    {
        canSetFilePath = QApplication::applicationDirPath() + canSetFilePath;

        QFileInfo fileInfo(canSetFilePath);
        if(canSetFilePath != filePath && fileInfo.isFile())
        {
            QFile file(canSetFilePath);
            bool ok = file.rename(filePath);
            if(ok)
            {
                qDebug()<<"can/canfd fun setting file rename as :"<<fileName;
                canSetFileName = fileName;
                canSetFilePath = filePath;
            }
        }
    }

    connect(&xml_ins, SIGNAL(readyForLoadDBC(QString)), this, SLOT(loadDbc(QString)));
    connect(&xml_ins, SIGNAL(readyForLoadArxml(QString)), this, SLOT(loadArxml(QString)));

    readSettingXml();
}

CanSetDlg::~CanSetDlg()
{
    delete ui;
}

void CanSetDlg::setDatabaseHeader(QString filePath)
{
    if(filePath.isEmpty()) return;

    QString name;
    QStringList fileList = filePath.split("/", QString::SkipEmptyParts);
    name = fileList.at(fileList.count()-1);
    name = name.split(".", QString::SkipEmptyParts)[0];
    QTreeWidgetItem* header = new QTreeWidgetItem;
    header->setText(0, name);
    header->setText(1, "ID");
    header->setText(2, "DLC");
    ui->dbcTree->setHeaderItem(header);
}

void CanSetDlg::showClusterInfo(dbc_Cluster *cluster)
{
    if(cluster == NULL)
        return;

    ui->dbcTree->clear();

    if(canConfig.isArxml)
    {
        ui->ComBox_Baudrate->setCurrentText(QString::number(cluster->m_can_baudrate));
        ui->ComBox_CANFD_Baudrate->setCurrentText(QString::number(cluster->m_canfd_baudrate));
        ui->ComBox_CANIOMode->setCurrentIndex(cluster->m_can_io_mode);
    }

    canConfig.dbc_frames = cluster->m_frames;

    QTreeWidgetItem* root = new QTreeWidgetItem();
    root->setText(0, cluster->m_clusterName);
    canConfig.sel_cluster = cluster->m_clusterName;

    QList<QTreeWidgetItem *> frameList;

    for(int i = 0; i < cluster->m_frames.count(); i++)
    {
        dbc_frame *frame = cluster->m_frames.at(i);

        QString frameText = frame->m_name;
        QString frameIdText = "0x"+QString::number(frame->m_id, 16).toUpper();
        QString frameDlcText = QString::number(frame->m_dlc);
        QString framePeriodText = QString::number(frame->m_dlc);

        QTreeWidgetItem* frameItem = new QTreeWidgetItem();
        frameItem->setText(0, frameText);
        frameItem->setText(1, frameIdText);
        frameItem->setText(2, frameDlcText);
        frameItem->setText(3, framePeriodText);
        frameList.append(frameItem);

        QList<QTreeWidgetItem*> sigList;

        for(int j = 0; j < frame->m_sigs.count(); j++)
        {
            dbc_signal *sig = frame->m_sigs.at(j);

            QTreeWidgetItem *sigItem = new QTreeWidgetItem();
            sigItem->setText(0, sig->m_name);

            sigList.append(sigItem);

        }

        frameItem->addChildren(sigList);
    }

    root->insertChildren(0, frameList);

    ui->dbcTree->insertTopLevelItem(0, root);
    ui->dbcTree->setExpandsOnDoubleClick(true);
    ui->dbcTree->expandItem(root);
    //ui->dbcTree->resizeColumnToContents(0);
    ui->dbcTree->resizeColumnToContents(1);
    ui->dbcTree->resizeColumnToContents(2);
    ui->dbcTree->resizeColumnToContents(3);
}

void CanSetDlg::clusterChangedSlot(QString clusterName)
{
    if(clusterName.isEmpty())
        return;

    if(canConfig.isArxml)
    {
        canConfig.sel_arCluster = clusterName;
        qDebug()<<"arClusterName:"<<clusterName;
        QString dbcFile = clusterDbcHash.value(clusterName);
        qDebug()<<"dbcfromArxml:"<<dbcFile;
        dbc_Cluster *cluster = loadDbcFromArxml(dbcFile);
        showClusterInfo(cluster);
    }
    else
    {
        qDebug()<<"clusterName:"<<clusterName;
        dbc_Cluster *cluster = clusterHash.value(clusterName);
        showClusterInfo(cluster);
    }
}

void CanSetDlg::loadDatabase(QString dbFilePath)
{
    if(dbFilePath.isEmpty())
        return;
    qDebug()<<"Loading database file path:"<<dbFilePath;

    if(dbFilePath.contains(".arxml") || dbFilePath.contains(".ARXML"))
    {
        loadArxml(dbFilePath);

        canConfig.sel_arCluster = ui->ComBox_Cluster->currentText();
        QString dbcFile = clusterDbcHash.value(canConfig.sel_arCluster);
        dbc_Cluster *cluster = loadDbcFromArxml(dbcFile);
        showClusterInfo(cluster);

    }
    else if (!dbFilePath.isEmpty())
    {
        loadDbc(dbFilePath);

        canConfig.sel_cluster = ui->ComBox_Cluster->currentText();
        dbc_Cluster *cluster = clusterHash.value(canConfig.sel_cluster);
        showClusterInfo(cluster);
    }

    connect(ui->ComBox_Cluster, SIGNAL(currentTextChanged(QString)), this, SLOT(clusterChangedSlot(QString)));
}

void CanSetDlg::loadDbc(QString filePath)
{
    if (filePath.isEmpty())
    {
        filePath = xml_ins.getDbcPath();
        if (filePath.isEmpty())
            return;
    }

    temp_path = filePath.toLocal8Bit();
    dbcPath = temp_path.data();

    QString name;
    QStringList fileList = filePath.split("/", QString::SkipEmptyParts);
    name = fileList.at(fileList.count()-1);
    name = name.split(".", QString::SkipEmptyParts)[0];
    temp_name = name.toLocal8Bit();
    dbcName = temp_name.data();

    u32 baudrate_default = 500000;
    u64 fdBaudrate_default = 2000000;

    nxStatus_t status_dbc;

    /*
    status_dbc = nxdbRemoveAlias(dbcName);
    if (status_dbc == nxSuccess)
    {
        qDebug()<<"Remove DBC Alias Succeed!"<<dbcName;
    }
    */

    //多次加载同一个xnet alias会报错，但不影响使用
    status_dbc = nxdbAddAlias64(dbcName, dbcPath, baudrate_default);
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

    nxDatabaseRef_t databaseRef;
    status_dbc = nxdbOpenDatabase(dbcName, &databaseRef);
    if (status_dbc == nxSuccess)
    {
        qDebug()<<"Open Database Succeed!";
    }

    u32 sizeOfClusterRef = 0;
    status_dbc = nxdbGetPropertySize(databaseRef, nxPropDatabase_ClstRefs, &sizeOfClusterRef);
    u32 numOfClusterRef = sizeOfClusterRef/sizeof(nxDatabaseRef_t);
    qDebug()<<"Num of Cluster from database:"<<numOfClusterRef;

    nxDatabaseRef_t *clusterRef = new nxDatabaseRef_t[numOfClusterRef];
    status_dbc = nxdbGetProperty(databaseRef, nxPropDatabase_ClstRefs, sizeOfClusterRef, clusterRef);
    if (status_dbc == nxSuccess)
    {
        //qDebug()<<"Get Cluster Ref Succeed! ClusterRef = "<<clusterRef[i];
    }

    ui->ComBox_Cluster->clear();
    clusterHash.clear();

    for(int i = 0; i < numOfClusterRef; i++)
    {
        qDebug()<<"Get Cluster Ref Succeed! ClusterRef["<<i<<"]:"<<clusterRef[i];

        dbc_Cluster *cluster = new dbc_Cluster;

        u32 sizeOfClusterName = 0;
        // Get Cluster Name
        status_dbc = nxdbGetPropertySize(clusterRef[i], nxPropClst_Name, &sizeOfClusterName);
        char *clusterName = new char[sizeOfClusterName];
        status_dbc = nxdbGetProperty(clusterRef[i], nxPropClst_Name, sizeOfClusterName, clusterName);
        if (status_dbc == nxSuccess)
        {
            qDebug()<<"Get Cluster Name Succeed! ClusterName = "<<clusterName;
            ui->ComBox_Cluster->addItem(clusterName);
            cluster->m_clusterName = QString(QLatin1String(clusterName));;
        }
        else
        {
            qDebug()<<"Get Cluster Name Error."<<status_dbc<<"\n";
            char statusString[1024];
            nxStatusToString(status_dbc, sizeof(statusString), statusString);
            qDebug()<<statusString;
        }

        // Get Baudrate
        u64 Baudrate64 = 0;
        status_dbc = nxdbGetProperty(clusterRef[i], nxPropClst_BaudRate64, sizeof(u64), &Baudrate64);
        if (status_dbc == nxSuccess)
        {
            qDebug()<<"Get Cluster CAN Standard Baudrate Succeed! Value = "<<Baudrate64;
            //ui->ComBox_Baudrate->setCurrentText(QString::number(Baudrate64, 10));

            cluster->m_can_baudrate = Baudrate64;
        }
        else
        {
            qDebug()<<"Get CAN Standard Baudrate Error."<<status_dbc<<"\n";
            char statusString[1024];
            nxStatusToString(status_dbc, sizeof(statusString), statusString);
            qDebug()<<statusString;
        }

        u32 CANIOMode = 1;

        //Get CANIO_Mode
        CANIOMode = 0;
        status_dbc = nxdbGetProperty(clusterRef[i], nxPropClst_CanIoMode, sizeof(u32), &CANIOMode);
        if (status_dbc == nxSuccess)
        {
            qDebug()<<"Get Cluster CAN I/O Mode Succeed! Value = "<<CANIOMode;
            //ui->ComBox_CANIOMode->setCurrentIndex(CANIOMode);

            cluster->m_can_io_mode = CANIOMode;
        }
        else
        {
            qDebug()<<"Get CAN I/O Mode Error."<<status_dbc<<"\n";
            char statusString[1024];
            nxStatusToString(status_dbc, sizeof(statusString), statusString);
            qDebug()<<statusString;
        }

        u64 CANFDBaudrate64 = 0;

        //Get FD Baudrate
        CANFDBaudrate64 = 0;
        status_dbc = nxdbGetProperty(clusterRef[i], nxPropClst_CanFdBaudRate64, sizeof(u64), &CANFDBaudrate64);
        if (status_dbc == nxSuccess)
        {
            qDebug()<<"Get Cluster CAN FD Baudrate Succeed! Value = "<<CANFDBaudrate64;

            cluster->m_canfd_baudrate = CANFDBaudrate64;
        }
        else
        {
            qDebug()<<"Get CAN FD Baudrate Error."<<status_dbc<<"\n";
            char statusString[1024];
            nxStatusToString(status_dbc, sizeof(statusString), statusString);
            qDebug()<<statusString;
        }

        u32 sizeOfFrameRef = 0;
        //Get Frames Size
        status_dbc = nxdbGetPropertySize(clusterRef[i], nxPropClst_FrmRefs, &sizeOfFrameRef);
        qDebug()<<"Size of Frame Ref = "<<sizeOfFrameRef;
        u32 numOfFrameRef = sizeOfFrameRef/sizeof(nxDatabaseRef_t);
        nxDatabaseRef_t *frameRef = new nxDatabaseRef_t[numOfFrameRef];

        status_dbc = nxdbGetProperty(clusterRef[i], nxPropClst_FrmRefs, sizeOfFrameRef, frameRef);
        if (status_dbc == nxSuccess)
        {
            qDebug()<<"Get Frame Ref Succeed!";
        }

        QList<dbc_frame*> dbcFrames;
        for (int i = 0; i < numOfFrameRef; i++)
        {
            QString frameText, frameIdText;
            u32 payLoadLength;
            bool idExtented;
            u32 timingType;
            u32 canioMode;
            double transmitTime;
            dbc_frame *cur_frame = new dbc_frame;

            u32 sizeOfFrameName = 0;
            status_dbc = nxdbGetPropertySize(frameRef[i], nxPropFrm_Name, &sizeOfFrameName);

            u32 frameId = 0;
            char *frameName = new char[sizeOfFrameName];
            status_dbc = nxdbGetProperty(frameRef[i], nxPropFrm_Name, sizeOfFrameName, frameName);
            status_dbc = nxdbGetProperty(frameRef[i], nxPropFrm_ID, sizeof(u32), &frameId);
            status_dbc = nxdbGetProperty(frameRef[i], nxPropFrm_CANExtID, sizeof(boolean), &idExtented);
            status_dbc = nxdbGetProperty(frameRef[i], nxPropFrm_CANTimingType, sizeof(u32), &timingType);
            status_dbc = nxdbGetProperty(frameRef[i], nxPropFrm_CANTxTime, sizeof(double), &transmitTime);
            status_dbc = nxdbGetProperty(frameRef[i], nxPropFrm_PayloadLen, sizeof(u32), &payLoadLength);
            status_dbc = nxdbGetProperty(frameRef[i], nxPropFrm_CANioMode, sizeof(u32), &canioMode);
            if (status_dbc == nxSuccess)
            {
                //qDebug()<<"No."<<i<<"Get Frame Name : "<<frameName;
                frameText = QString(QLatin1String(frameName));
                frameIdText = "0x"+QString::number(frameId, 16).toUpper();

                cur_frame->m_name = frameText;
                cur_frame->m_dlc = payLoadLength;
                cur_frame->m_id = frameId;
                cur_frame->m_extented = idExtented;
                cur_frame->m_timingType = timingType;
                cur_frame->m_cyclicTime = transmitTime*1000;
                cur_frame->m_can_io_mode = canioMode;
            }

            u32 sizeOfSigRef;
            status_dbc = nxdbGetPropertySize(frameRef[i], nxPropFrm_SigRefs, &sizeOfSigRef);
            u32 numOfSigRef = sizeOfSigRef/sizeof(nxDatabaseRef_t);
            nxDatabaseRef_t *sigRef = new nxDatabaseRef_t[numOfSigRef];
            status_dbc = nxdbGetProperty(frameRef[i], nxPropFrm_SigRefs, sizeOfSigRef, sigRef);
            if (status_dbc == nxSuccess)
            {
                //qDebug()<<QString("Get Sigs Ref of Frame: %1 succeed!").arg(frameText);
            }

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
                    //qDebug()<<"Get Signal Name : "<<sigName;
                    sigNameText = QString(QLatin1String(sigName));
                    sigUnitText = QString(QLatin1String(sigUnit));
                    cur_sig->m_name = sigNameText;
                    cur_sig->m_unit = sigUnitText;
                    cur_sig->m_byteOrder = byteOrder;
                    cur_sig->m_valType = sig_dataType;
                    cur_sig->m_startBit = startbit;

                    quint16 sb_oth;
                    sb_oth = startbit;
                    can_format_motorola2mdf_sb_lb(sb_oth, (quint16)lengthbit);
                    cur_sig->m_startBit_oth = sb_oth;
                    cur_sig->m_bitLen = lengthbit;
                    cur_sig->m_value_default = defaultValue;
                    cur_sig->m_maximum = maxValue;
                    cur_sig->m_minimum = minValue;
                    cur_sig->m_factor = factor;
                    cur_sig->m_offset = offset;

                    //qDebug()<<"Signal: "<<sigName<<",sb:"<<cur_sig->m_startBit<<",sb_oth:"<<cur_sig->m_startBit_oth;
                }

                cur_frame->m_sigs.append(cur_sig);
            }
            canConfig.dbc_frames.append(cur_frame);

            dbcFrames.append(cur_frame);
        }

        cluster->m_frames = dbcFrames;

        clusterHash.insert(clusterName, cluster);
    }

    status_dbc = nxdbCloseDatabase(databaseRef, 1);
    if (status_dbc == nxSuccess)
    {
        qDebug()<<"Close Database Succeed!";
    }

    connect(ui->ComBox_Cluster, SIGNAL(currentIndexChanged(QString)), this, SLOT(clusterChangedSlot(QString)));

    QString filePath_ref = filePath;
    filePath_ref = filePath_ref.replace(QCoreApplication::applicationDirPath(), "");

    canConfig.sel_dbcPath = filePath;
    canConfig.sel_dbcPath_ref = filePath_ref;

    canConfig.isArxml = false;
    xml_ins.setIsArxml(false);
}

void CanSetDlg::loadArxml(QString filePath)
{
    ArxmlPraser arxmlDatabase(this, filePath);
    if(arxmlDatabase.getPraserOk())
    {
        arXmlPackages = arxmlDatabase.getArXmlPackages();
        canClusters = arxmlDatabase.getCanClusters();
        genDbcFileList = arxmlDatabase.getGenDbcFileList();
        clusterDbcHash = arxmlDatabase.getClusterDbcHash();

        ui->ComBox_Cluster->clear();
        clusterHash.clear();

        foreach (dbc_Cluster* cluster, canClusters) {
            clusterHash.insert(cluster->m_clusterName, cluster);
            ui->ComBox_Cluster->addItem(cluster->m_clusterName);
        }
    }

    canConfig.sel_arxmlPath = filePath;
    canConfig.isArxml = true;
}

dbc_Cluster *CanSetDlg::loadDbcFromArxml(QString filePath)
{
    if(filePath.isEmpty())
        return NULL;

    qDebug()<<"===========Load dbc cluster from in arxml===========";
    qDebug()<<"Select arCluster name:"<<canConfig.sel_arCluster<<", arxml dbc file path:"<<filePath;

    canConfig.sel_dbcPath = filePath;
    temp_path = canConfig.sel_dbcPath.toLocal8Bit();
    dbcPath = temp_path.data();

    QString name;
    QStringList fileList = canConfig.sel_dbcPath.split("/", QString::SkipEmptyParts);
    name = fileList.at(fileList.count()-1);
    name = name.split(".", QString::SkipEmptyParts)[0];
    temp_name = name.toLocal8Bit();
    dbcName = temp_name.data();

    u32 baudrate_default = 500000;
    u64 fdBaudrate_default = 2000000;

    nxStatus_t status_dbc;

    /*
    status_dbc = nxdbRemoveAlias(dbcName);
    if (status_dbc == nxSuccess)
    {
        qDebug()<<"Remove DBC Alias Succeed!"<<dbcName;
    }
    */

    //多次加载同一个xnet alias会报错，但不影响使用
    status_dbc = nxdbAddAlias64(dbcName, dbcPath, baudrate_default);
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

    nxDatabaseRef_t databaseRef;
    status_dbc = nxdbOpenDatabase(dbcName, &databaseRef);
    if (status_dbc == nxSuccess)
    {
        qDebug()<<"Open Database Succeed!";
    }

    u32 sizeOfClusterRef = 0;
    status_dbc = nxdbGetPropertySize(databaseRef, nxPropDatabase_ClstRefs, &sizeOfClusterRef);
    u32 numOfClusterRef = sizeOfClusterRef/sizeof(nxDatabaseRef_t);
    qDebug()<<"Num of Cluster from database:"<<numOfClusterRef;

    nxDatabaseRef_t *clusterRef = new nxDatabaseRef_t[numOfClusterRef];
    status_dbc = nxdbGetProperty(databaseRef, nxPropDatabase_ClstRefs, sizeOfClusterRef, clusterRef);
    if (status_dbc == nxSuccess)
    {
        //qDebug()<<"Get Cluster Ref Succeed! ClusterRef = "<<clusterRef[i];
    }

    if(numOfClusterRef < 1)
        return NULL;

    qDebug()<<"Get Cluster Ref Succeed! ClusterRef["<<0<<"]:"<<clusterRef[0];

    dbc_Cluster *cluster = new dbc_Cluster;

    u32 sizeOfClusterName = 0;
    // Get Cluster Name
    status_dbc = nxdbGetPropertySize(clusterRef[0], nxPropClst_Name, &sizeOfClusterName);
    char *clusterName = new char[sizeOfClusterName];
    status_dbc = nxdbGetProperty(clusterRef[0], nxPropClst_Name, sizeOfClusterName, clusterName);
    if (status_dbc == nxSuccess)
    {
        qDebug()<<"Get Cluster Name Succeed! ClusterName = "<<clusterName;

        cluster->m_clusterName = QString(clusterName);

    }
    else
    {
        qDebug()<<"Get Cluster Name Error."<<status_dbc<<"\n";
        char statusString[1024];
        nxStatusToString(status_dbc, sizeof(statusString), statusString);
        qDebug()<<statusString;
    }

    // Get Baudrate
    u64 Baudrate64 = 0;
    status_dbc = nxdbGetProperty(clusterRef[0], nxPropClst_BaudRate64, sizeof(u64), &Baudrate64);
    if (status_dbc == nxSuccess)
    {
        qDebug()<<"Get Cluster CAN Standard Baudrate Succeed! Value = "<<Baudrate64;
        cluster->m_can_baudrate = Baudrate64;
    }
    else
    {
        qDebug()<<"Get CAN Standard Baudrate Error."<<status_dbc<<"\n";
        char statusString[1024];
        nxStatusToString(status_dbc, sizeof(statusString), statusString);
        qDebug()<<statusString;
    }

    u32 CANIOMode = 1;
    //Get CANIO_Mode
    CANIOMode = 0;
    status_dbc = nxdbGetProperty(clusterRef[0], nxPropClst_CanIoMode, sizeof(u32), &CANIOMode);
    if (status_dbc == nxSuccess)
    {
        qDebug()<<"Get Cluster CAN I/O Mode Succeed! Value = "<<CANIOMode;
        //ui->ComBox_CANIOMode->setCurrentIndex(CANIOMode);

        cluster->m_can_io_mode = CANIOMode;
    }
    else
    {
        qDebug()<<"Get CAN I/O Mode Error."<<status_dbc<<"\n";
        char statusString[1024];
        nxStatusToString(status_dbc, sizeof(statusString), statusString);
        qDebug()<<statusString;
    }

    u64 CANFDBaudrate64 = 4000000;
    //Get FD Baudrate
    CANFDBaudrate64 = 0;
    status_dbc = nxdbGetProperty(clusterRef[0], nxPropClst_CanFdBaudRate64, sizeof(u64), &CANFDBaudrate64);
    if (status_dbc == nxSuccess)
    {
        qDebug()<<"Get Cluster CAN FD Baudrate Succeed! Value = "<<CANFDBaudrate64;

        cluster->m_canfd_baudrate = CANFDBaudrate64;
    }
    else
    {
        qDebug()<<"Get CAN FD Baudrate Error."<<status_dbc<<"\n";
        char statusString[1024];
        nxStatusToString(status_dbc, sizeof(statusString), statusString);
        qDebug()<<statusString;
    }

    u32 sizeOfFrameRef = 0;
    //Get Frames Size
    status_dbc = nxdbGetPropertySize(clusterRef[0], nxPropClst_FrmRefs, &sizeOfFrameRef);
    qDebug()<<"Size of Frame Ref = "<<sizeOfFrameRef;
    u32 numOfFrameRef = sizeOfFrameRef/sizeof(nxDatabaseRef_t);
    nxDatabaseRef_t *frameRef = new nxDatabaseRef_t[numOfFrameRef];

    status_dbc = nxdbGetProperty(clusterRef[0], nxPropClst_FrmRefs, sizeOfFrameRef, frameRef);
    if (status_dbc == nxSuccess)
    {
        qDebug()<<"Get Frame Ref Succeed!";
    }

    QList<dbc_frame*> dbcFrames;
    for (int i = 0; i < numOfFrameRef; i++)
    {
        QString frameText, frameIdText;
        u32 payLoadLength;
        bool idExtented;
        u32 timingType;
        u32 canioMode;
        double transmitTime;
        dbc_frame *cur_frame = new dbc_frame;

        u32 sizeOfFrameName = 0;
        status_dbc = nxdbGetPropertySize(frameRef[i], nxPropFrm_Name, &sizeOfFrameName);

        u32 frameId = 0;
        char *frameName = new char[sizeOfFrameName];
        status_dbc = nxdbGetProperty(frameRef[i], nxPropFrm_Name, sizeOfFrameName, frameName);
        status_dbc = nxdbGetProperty(frameRef[i], nxPropFrm_ID, sizeof(u32), &frameId);
        status_dbc = nxdbGetProperty(frameRef[i], nxPropFrm_CANExtID, sizeof(boolean), &idExtented);
        status_dbc = nxdbGetProperty(frameRef[i], nxPropFrm_CANTimingType, sizeof(u32), &timingType);
        status_dbc = nxdbGetProperty(frameRef[i], nxPropFrm_CANTxTime, sizeof(double), &transmitTime);
        status_dbc = nxdbGetProperty(frameRef[i], nxPropFrm_PayloadLen, sizeof(u32), &payLoadLength);
        status_dbc = nxdbGetProperty(frameRef[i], nxPropFrm_CANioMode, sizeof(u32), &canioMode);
        if (status_dbc == nxSuccess)
        {
            //qDebug()<<"No."<<i<<"Get Frame Name : "<<frameName;
            frameText = QString(QLatin1String(frameName));
            //frameList << frameText;
            frameIdText = "0x"+QString::number(frameId, 16).toUpper();

            cur_frame->m_name = frameText;
            cur_frame->m_dlc = payLoadLength;
            cur_frame->m_id = frameId;
            cur_frame->m_extented = idExtented;
            cur_frame->m_timingType = timingType;
            cur_frame->m_cyclicTime = transmitTime*1000;
            cur_frame->m_can_io_mode = canioMode;
        }

        u32 sizeOfSigRef;
        status_dbc = nxdbGetPropertySize(frameRef[i], nxPropFrm_SigRefs, &sizeOfSigRef);
        u32 numOfSigRef = sizeOfSigRef/sizeof(nxDatabaseRef_t);
        nxDatabaseRef_t *sigRef = new nxDatabaseRef_t[numOfSigRef];
        status_dbc = nxdbGetProperty(frameRef[i], nxPropFrm_SigRefs, sizeOfSigRef, sigRef);
        if (status_dbc == nxSuccess)
        {
            //qDebug()<<QString("Get Sigs Ref of Frame: %1 succeed!").arg(frameText);
        }
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
                //qDebug()<<"Get Signal Name : "<<sigName;
                sigNameText = QString(QLatin1String(sigName));
                sigUnitText = QString(QLatin1String(sigUnit));
                cur_sig->m_name = sigNameText;
                cur_sig->m_unit = sigUnitText;
                cur_sig->m_byteOrder = byteOrder;
                cur_sig->m_valType = sig_dataType;
                cur_sig->m_startBit = startbit;
                //qDebug()<<"Signal startbit : "<<cur_sig->m_startBit;
                quint16 sb_oth;
                sb_oth = startbit;
                can_format_motorola2intel_sb_lb(&sb_oth, (quint16*)&lengthbit);
                cur_sig->m_startBit_oth = sb_oth;
                cur_sig->m_bitLen = lengthbit;
                cur_sig->m_value_default = defaultValue;
                cur_sig->m_maximum = maxValue;
                cur_sig->m_minimum = minValue;
                cur_sig->m_factor = factor;
                cur_sig->m_offset = offset;
            }

            cur_frame->m_sigs.append(cur_sig);
        }

        canConfig.dbc_frames.append(cur_frame);

        dbcFrames.append(cur_frame);
    }

    status_dbc = nxdbCloseDatabase(databaseRef, 1);
    if (status_dbc == nxSuccess)
    {
        qDebug()<<"Close Database Succeed!";
    }

    cluster->m_frames = dbcFrames;

    qDebug()<<"===========Load dbc cluster from in arxml done===========";

    return cluster;
}

QList<dbc_frame *> CanSetDlg::getFramesByNames(QStringList frameNameList)
{
    QList<dbc_frame*> dbc_frames_temp;
    //qDebug()<<"frameName num="<<frameNameList.count();
    for (int i = 0; i < frameNameList.count(); i++)
    {
        QString frameString = frameNameList[i];
        //qDebug()<<"frameString = "<<frameString;
        int j = 0;
        dbc_frame *frame_temp = canConfig.dbc_frames[j];
        while ((j < canConfig.dbc_frames.count()) && (frame_temp->m_name != frameString))
        {
            j++;
            frame_temp = canConfig.dbc_frames[j];
        }
        //qDebug()<<"frame_temp Name ="<<frame_temp.Name;
        dbc_frames_temp.append(frame_temp);
    }
    return dbc_frames_temp;
}

void CanSetDlg::loadMsgToModel()
{
    if (canConfig.dbc_frames_r.isEmpty() && canConfig.dbc_frames_w.isEmpty())
        return;

    qDebug()<<"show read frames to tree";
    for (int i = 0; i < canConfig.dbc_frames_r.count(); i++)
    {
        dbc_frame *frame = canConfig.dbc_frames_r[i];
        QTreeWidgetItem *msgItem = new QTreeWidgetItem();
        msgItem->setData(0, Qt::UserRole, QVariant::fromValue(frame));
        msgItem->setText(0, frame->m_name);
        for(int k = 0; k < 6; k++)
            msgItem->setBackgroundColor(k, QColor(0xFFEC8B));
        for (int j = 0; j < frame->m_sigs.count(); j++)
        {
            dbc_signal *signal = frame->m_sigs[j];
            QTreeWidgetItem *sigItem = new QTreeWidgetItem();
            sigItem->setData(1, Qt::UserRole, QVariant::fromValue(signal));

            sigItem->setText(0, signal->m_name);
            sigItem->setText(1, QString::number(signal->m_value_default, 'g', 6));
            msgItem->addChild(sigItem);
        }

        ui->traceTree_Read->addTopLevelItem(msgItem);
    }

    qDebug()<<"show write frames to tree";
    for (int i = 0; i < canConfig.dbc_frames_w.count(); i++)
    {
        dbc_frame *frame = canConfig.dbc_frames_w[i];
        QTreeWidgetItem *msgItem = new QTreeWidgetItem();
        msgItem->setData(0, Qt::UserRole, QVariant::fromValue(frame));
        msgItem->setText(0, frame->m_name);
        for(int k = 0; k < 6; k++)
            msgItem->setBackgroundColor(k, QColor(0xC1FFC1));
        for (int j = 0; j < frame->m_sigs.count(); j++)
        {
            dbc_signal *signal = frame->m_sigs[j];
            QTreeWidgetItem *sigItem = new QTreeWidgetItem();
            sigItem->setData(1, Qt::UserRole, QVariant::fromValue(signal));
            sigItem->setText(0, signal->m_name);
            QString valueStr;
            if(signal->assignMode == 0)
            {
                valueStr = QString::number(signal->m_value, 'g', 6);
            }
            else if(signal->assignMode == 2)
            {
                valueStr = "RC";

                if(signal->sigAssignModePams.count() >= 3)
                {
                    sigItem->setText(2, signal->sigAssignModePams.at(0));
                    sigItem->setText(3, signal->sigAssignModePams.at(1));
                    sigItem->setText(4, signal->sigAssignModePams.at(2));
                    sigItem->setText(5, signal->sigAssignModePams.at(3));
                }

            }
            else if(signal->assignMode == 3)
            {
                valueStr = "CRC";

                if(signal->checkSumInfos.count() >= 3)
                {
                    sigItem->setText(2, signal->checkSumInfos.at(0));
                    sigItem->setText(3, signal->checkSumInfos.at(1));
                    sigItem->setText(4, signal->checkSumInfos.at(2));
                    sigItem->setText(5, signal->checkSumInfos.at(3));
                }
            }
            sigItem->setText(1, valueStr);

            msgItem->addChild(sigItem);

            sigItem->setFlags(sigItem->flags() | Qt::ItemIsEditable);


        }

        ui->traceTree_Write->addTopLevelItem(msgItem);
    }
}

bool CanSetDlg::readSettingXml()
{
    disconnect(ui->ComBox_Cluster, SIGNAL(currentTextChanged(QString)), this, SLOT(clusterChangedSlot(QString)));
    ui->ComBox_Cluster->clear();


    canConfig.sel_frames_r.clear();
    canConfig.sel_frames_w.clear();
    canConfig.dbc_frames_r.clear();
    canConfig.dbc_frames_w.clear();

    ui->traceTree_Read->clear();
    ui->traceTree_Write->clear();
    readTreeModel.clear();
    writeTreeModel.clear();

    xml_ins.setXmlFilePath(this->canSetFilePath);

    if (!xml_ins.readXML())
    {
        qDebug()<<"xml Read Error.";
        return false;
    }
    canConfig.isArxml = xml_ins.getIsArxml();
    canConfig.sel_arxmlPath = xml_ins.getArxmlPath();
    canConfig.sel_arxmlPath_ref = xml_ins.getArxmlRefPath();
    canConfig.sel_arCluster = xml_ins.getArCluster();
    canConfig.sel_dbcPath = xml_ins.getDbcPath();
    canConfig.sel_dbcPath_ref = xml_ins.getDbcRefPath();
    canConfig.sel_cluster = xml_ins.getCluster();
    canConfig.sel_interface = xml_ins.getInterface();
    canConfig.sel_baudrate = xml_ins.getBaudrate();
    canConfig.sel_canio_mode = xml_ins.getCANIOMode();
    canConfig.sel_canfd_baudrate = xml_ins.getCANFDBaudrate();
    canConfig.dbc_frames_r = xml_ins.getFramesForRead();
    canConfig.dbc_frames_w = xml_ins.getFramesForWrite();
    canConfig.sel_termination_on = (xml_ins.getCANTermination() > 0);
    canConfig.sel_CustomBD_on = (xml_ins.getCustomBD() > 0);
    canConfig.sel_CustomFDBD_on = (xml_ins.getCustomFDBD() > 0);
    canConfig.sel_SamplePoint_BD = xml_ins.getSamplePoint_BD();
    canConfig.sel_SamplePoint_FDBD = xml_ins.getSamplePoint_FDBD();

    // xml配置文件中保存的信息复原
    ui->ComBox_Interface->setCurrentText(canConfig.sel_interface);
    ui->ComBox_Baudrate->setCurrentText(QString::number(canConfig.sel_baudrate));
    ui->ComBox_CANIOMode->setCurrentIndex(canConfig.sel_canio_mode);
    ui->ComBox_CANFD_Baudrate->setCurrentText(QString::number(canConfig.sel_canfd_baudrate));
    ui->checkBox_Termination->setChecked(canConfig.sel_termination_on);
    ui->checkBox_CustomBD->setChecked(canConfig.sel_CustomBD_on);
    ui->checkBox_CustomFDBD->setChecked(canConfig.sel_CustomFDBD_on);
    ui->doubleSpinBox_SamplePointBD->setValue(canConfig.sel_SamplePoint_BD);
    ui->doubleSpinBox_SamplePointFDBD->setValue(canConfig.sel_SamplePoint_FDBD);

    if(canConfig.isArxml)
    {
        setDatabaseHeader(canConfig.sel_arxmlPath);
        if(!canConfig.sel_arxmlPath.isEmpty())
        {
            qDebug()<<"In xml sel [arCluster]:"<<canConfig.sel_arCluster;

            loadArxml(canConfig.sel_arxmlPath);

            QString filePath = clusterDbcHash.value(canConfig.sel_arCluster);
            dbc_Cluster *cluster = loadDbcFromArxml(filePath);
            showClusterInfo(cluster);
            ui->ComBox_Cluster->setCurrentText(canConfig.sel_arCluster);
            ui->ComBox_Baudrate->setCurrentText(QString::number(canConfig.sel_baudrate));
            ui->ComBox_CANIOMode->setCurrentIndex(canConfig.sel_canio_mode);
            ui->ComBox_CANFD_Baudrate->setCurrentText(QString::number(canConfig.sel_canfd_baudrate));
        }
    }
    else
    {
        setDatabaseHeader(canConfig.sel_dbcPath);
        if(!canConfig.sel_dbcPath.isEmpty())
        {
            loadDbc(canConfig.sel_dbcPath);
            qDebug()<<"sel Cluster:"<<canConfig.sel_cluster;
            dbc_Cluster *cluster = clusterHash.value(canConfig.sel_cluster);
            showClusterInfo(cluster);
        }
    }

    qDebug()<<"load msg to model...";
    loadMsgToModel();
    qDebug()<<"load msg to model done.";

    connect(ui->ComBox_Cluster, SIGNAL(currentTextChanged(QString)), this, SLOT(clusterChangedSlot(QString)));

    intfSettingLoad = true;

    return true;
}

bool CanSetDlg::writeSettingXml()
{

    canConfig.sel_cluster = ui->ComBox_Cluster->currentText();
    if(canConfig.sel_cluster.isEmpty())
        canConfig.sel_cluster = "Cluster";
    canConfig.sel_interface = ui->ComBox_Interface->currentText();

    canConfig.sel_canio_mode = ui->ComBox_CANIOMode->currentIndex();
    canConfig.sel_baudrate = ui->ComBox_Baudrate->currentText().toInt();
    canConfig.sel_canfd_baudrate = ui->ComBox_CANFD_Baudrate->currentText().toInt();
    canConfig.sel_termination_on = ui->checkBox_Termination->isChecked();
    canConfig.sel_CustomBD_on = ui->checkBox_CustomBD->isChecked();
    canConfig.sel_CustomFDBD_on = ui->checkBox_CustomFDBD->isChecked();
    canConfig.sel_SamplePoint_BD = ui->doubleSpinBox_SamplePointBD->value();
    canConfig.sel_SamplePoint_FDBD = ui->doubleSpinBox_SamplePointFDBD->value();

    canConfig.sel_frames_r = ui->traceTree_Read->getMsgsString();
    canConfig.sel_sigs_r = ui->traceTree_Read->getSigsString();
    canConfig.sel_frames_w = ui->traceTree_Write->getMsgsString();
    canConfig.sel_sigs_w = ui->traceTree_Write->getSigsString();

    QStringList sigValList = ui->traceTree_Write->getSigSetValueStringList();
    QHash<QString, QStringList> sigPamHash =  ui->traceTree_Write->getFrameSigPamString();

    QStringList frames_r, frames_w;
    frames_r = ui->traceTree_Read->getMsgsStringList();
    frames_w = ui->traceTree_Write->getMsgsStringList();

    qDebug()<<"frames read:"<<frames_r;
    qDebug()<<"frames write:"<<frames_w;
    qDebug()<<"sel frames r:"<<canConfig.sel_frames_r;
    qDebug()<<"sel frames w:"<<canConfig.sel_frames_w;

    canConfig.dbc_frames_r = getFramesByNames(frames_r);
    canConfig.dbc_frames_w = getFramesByNames(frames_w);

    int sigIndex = 0;
    foreach (dbc_frame *frame, canConfig.dbc_frames_w) {
        foreach (dbc_signal *sig, frame->m_sigs) {
            QString setValueStr = sigValList.at(sigIndex);
            bool ok = true;
            double setValue = setValueStr.toDouble(&ok);
            sig->m_value = ok ? setValue : 0;

            QStringList sigPams = sigPamHash.value(sig->m_name);

            if(setValueStr.toUpper().contains("CRC") || setValueStr.toUpper().contains("CHECKSUM"))
            {
                sig->assignMode = CRC;

                sig->checkSumInfos = sigPams;

                frame->flagChecksum = true;

                CheckSumInfo checkSumInfo;
                checkSumInfo.startIndex_CS_data = QString(sig->checkSumInfos.at(0)).toInt();
                checkSumInfo.endIndex_CS_data = QString(sig->checkSumInfos.at(1)).toInt();
                checkSumInfo.index_CS = QString(sig->checkSumInfos.at(2)).toInt();
                frame->checkSumInfoList.append(checkSumInfo);
            }
            else if(setValueStr.toUpper().contains("RC") || setValueStr.toUpper().contains("ROLLINGCOUNTER"))
            {
                sig->assignMode = RC;
                sig->sigAssignModePams = sigPams;
            }
            else
            {
                sig->assignMode = Constant;
                sig->sigAssignModePams = sigPams;
            }

            qDebug()<<"sig:"<<sig->m_name<<",setValue:"<<setValueStr;
            sigIndex++;
        }
    }

    //xml file setting
    xml_ins.setArxmlPath(canConfig.sel_arxmlPath);
    xml_ins.setArxmlRefPath(canConfig.sel_arxmlPath_ref);
    xml_ins.setArCluster(canConfig.sel_arCluster);
    xml_ins.setIsArxml(canConfig.isArxml);
    xml_ins.setDbcPath(canConfig.sel_dbcPath);
    xml_ins.setDbcRefPath(canConfig.sel_dbcPath_ref);
    xml_ins.setCluster(canConfig.sel_cluster);
    xml_ins.setBaudrate(canConfig.sel_baudrate);
    xml_ins.setInterface(canConfig.sel_interface);
    xml_ins.setCANIOMode(canConfig.sel_canio_mode);
    xml_ins.setCANFDBaudrate(canConfig.sel_canfd_baudrate);
    xml_ins.setFramesForRead(canConfig.dbc_frames_r);
    xml_ins.setFramesForWrite(canConfig.dbc_frames_w);
    xml_ins.setCANTermination(canConfig.sel_termination_on == false ? 0 : 1);
    xml_ins.setCustomBD(canConfig.sel_CustomBD_on == false ? 0 : 1);
    xml_ins.setCustomFDBD(canConfig.sel_CustomFDBD_on == false ? 0 : 1);
    xml_ins.setSamplePoint_BD(canConfig.sel_SamplePoint_BD);
    xml_ins.setSamplePoint_FDBD(canConfig.sel_SamplePoint_FDBD);
    xml_ins.setSel_frames_r(canConfig.sel_frames_r);
    xml_ins.setSel_frames_w(canConfig.sel_frames_w);
    xml_ins.setSel_sigs_r(canConfig.sel_sigs_r);
    xml_ins.setSel_sigs_w(canConfig.sel_sigs_w);

    xml_ins.setXmlFilePath(this->canSetFilePath);
    xml_ins.genXML();

    return true;
}

/*
bool CanSetDlg::saveXml()
{
    QStringList readFrameList = ui->traceTree_Read->getMsgsStringList();
    QStringList writeFrameList = ui->traceTree_Write->getMsgsStringList();

    QString readFrames = ui->traceTree_Read->getMsgsString();
    QString writeFrames = ui->traceTree_Write->getMsgsString();

    QString intfName = ui->ComBox_Interface->currentText();

    QString modName = "NIXNET_" + intfName;

    quint8 can_io_mode = ui->ComBox_CANIOMode->currentIndex();
    quint8 canfd_standard = ui->ComBox_CanFDStandard->currentIndex();

    quint64 can_baudrate = (quint64)(ui->ComBox_Baudrate->currentText().toInt());
    quint64 canfd_baudrate = (quint64)(ui->ComBox_CANFD_Baudrate->currentText().toLongLong());

    quint8 termination_enable = ui->checkBox_Termination->isChecked() ? 1 : 0;
    quint8 isCanFD = ui->checkBox_isCANFD->isChecked() ? 1 : 0;

    double samplePoint_abit = ui->doubleSpinBox_SamplePointBD->value();
    double samplePoint_dbit = ui->doubleSpinBox_SamplePointFDBD->value();
    bool sp_abit_enable = ui->checkBox_CustomBD->isChecked();
    bool sp_dbit_enable = ui->checkBox_CustomFDBD->isChecked();

    if(can_io_mode == 0)
    {
        if(can_baudrate == 0)
            return false;
    }
    else
    {
        if(canfd_baudrate == 0)
            return false;
    }

    QString appPath = QApplication::applicationDirPath();
    QString xmlPath_ref = "/pamFactory/NIXNET_" +intfName + ".xml";
    QString xmlPath = appPath + xmlPath_ref;
    qDebug()<<"Save xml file:"<<xmlPath;

    QFile xmlFile(xmlPath);
    if (!xmlFile.open(QFile::WriteOnly | QFile::Text))
    {
        qDebug()<<tr("Cannot write file %1:\n%2.").arg(xmlPath)
                  .arg(xmlFile.errorString());
        return false;
    }

    QDomDocument xml("NIXNET_CAN");

    QDomElement root = xml.createElement("Root");
    root.setAttribute("dbcfile", dbcFilePath);
    root.setAttribute("dbcfile_ref", dbcFilePath_ref);
    root.setAttribute("cluster_name", m_cluster->m_clusterName);
    root.setAttribute("interface", intfName);
    root.setAttribute("can_io_mode", can_io_mode);
    root.setAttribute("canfd_standard", canfd_standard);
    root.setAttribute("can_baudrate", can_baudrate);
    root.setAttribute("canfd_baudrate", canfd_baudrate);
    root.setAttribute("termination_enable", termination_enable);
    root.setAttribute("custom_abit", sp_abit_enable);
    root.setAttribute("custom_dbit", sp_dbit_enable);
    root.setAttribute("samplepoint_abit", samplePoint_abit);
    root.setAttribute("samplepoint_dbit", samplePoint_dbit);
    xml.appendChild(root);

    QList<dbc_frame*> readBOList = getFrameListByNames(readFrameList);

    for(int i=0; i < readBOList.count(); i++)
    {
        dbc_frame* frame = readBOList[i];
        qDebug()<<"frame:"<<frame->m_name;
        quint32 sizeFrame = frame->m_dlc + 8;
        qDebug()<<"size of frame:"<<sizeFrame;
        QDomElement ReadElem = xml.createElement("RP_" + frame->m_name + "_" + moduleName);
        root.appendChild(ReadElem);
        ReadElem.setAttribute("size_bytes", sizeFrame);
        ReadElem.setAttribute("is_read", 1);
        ReadElem.setAttribute("id", "0x"+QString::number(frame->m_id, 16));
        ReadElem.setAttribute("extented", frame->m_extented);
        ReadElem.setAttribute("dlc", frame->m_dlc);
        ReadElem.setAttribute("sample_rate", frame->m_cyclicTime);

        QDomElement timeElem = xml.createElement("param_"+QString::number(0));
        ReadElem.appendChild(timeElem);
        timeElem.setAttribute("unit", "s");
        timeElem.setAttribute("value_default", "0");
        timeElem.setAttribute("minimum", "0");
        timeElem.setAttribute("maximum", "0");
        timeElem.setAttribute("scalingoffset", "0");
        timeElem.setAttribute("scalingfactor", "1");
        timeElem.setAttribute("lengthbit", "64");
        timeElem.setAttribute("startbit", "0");
        timeElem.setAttribute("startbit_oth", "0");
        timeElem.setAttribute("byteorder", "0");
        timeElem.setAttribute("type", "3");
        timeElem.setAttribute("param_name", "Time");

        for (int j=1; j < frame->m_sigs.count()+1; j++)
        {
            //qDebug()<<"signal read inde = "<<j;
            dbc_signal* sig = frame->m_sigs[j-1];
            QDomElement Elem = xml.createElement("param_"+QString::number(j));
            ReadElem.appendChild(Elem);

            Elem.setAttribute("unit", sig->m_unit);
            Elem.setAttribute("value_default", QString::number(sig->m_value_default));
            Elem.setAttribute("minimum", QString::number(sig->m_minimum));
            Elem.setAttribute("maximum", QString::number(sig->m_maximum));
            Elem.setAttribute("scalingoffset", QString::number(sig->m_offset));
            Elem.setAttribute("scalingfactor", QString::number(sig->m_factor));
            Elem.setAttribute("lengthbit", QString::number(sig->m_bitLen, 10));
            Elem.setAttribute("startbit", QString::number(sig->m_startBit+64, 10));
            Elem.setAttribute("startbit_oth", QString::number(sig->m_startBit+64, 10));
            Elem.setAttribute("byteorder", QString::number(sig->m_byteOrder,10));
            Elem.setAttribute("type", QString::number(sig->m_valType, 10));
            Elem.setAttribute("param_name", sig->m_name);

        }
    }

    QList<dbc_frame*> writeBOList = getFrameListByNames(writeFrameList);

    for(int i=0; i < writeBOList.count(); i++)
    {
        dbc_frame* frame = writeBOList[i];
        qDebug()<<"frame:"<<frame->m_name;
        quint32 sizeFrame = frame->m_dlc + 8;
        qDebug()<<"size of frame:"<<sizeFrame;
        QDomElement WriteElem = xml.createElement("WP_" + frame->m_name + "_" + moduleName);
        root.appendChild(WriteElem);
        WriteElem.setAttribute("size_bytes", sizeFrame);
        WriteElem.setAttribute("is_read", 0);
        WriteElem.setAttribute("id", "0x"+QString::number(frame->m_id, 16));
        WriteElem.setAttribute("extented", frame->m_extented);
        WriteElem.setAttribute("dlc", frame->m_dlc);
        WriteElem.setAttribute("sample_rate", frame->m_cyclicTime);

        QDomElement timeElem = xml.createElement("param_"+QString::number(0));
        WriteElem.appendChild(timeElem);
        timeElem.setAttribute("unit", "s");
        timeElem.setAttribute("value_default", "0");
        timeElem.setAttribute("minimum", "0");
        timeElem.setAttribute("maximum", "0");
        timeElem.setAttribute("scalingoffset", "0");
        timeElem.setAttribute("scalingfactor", "1");
        timeElem.setAttribute("lengthbit", "64");
        timeElem.setAttribute("startbit", "0");
        timeElem.setAttribute("startbit_oth", "0");
        timeElem.setAttribute("byteorder", "0");
        timeElem.setAttribute("type", "3");
        timeElem.setAttribute("param_name", "Time");

        for (int j=1; j < frame->m_sigs.count()+1; j++)
        {
            dbc_signal* sig = frame->m_sigs[j-1];
            //qDebug()<<"sig in gen xml name:"<<sig->m_name<<",startbit:"<<sig->m_startBit;
            QDomElement Elem = xml.createElement("param_"+QString::number(j));
            WriteElem.appendChild(Elem);

            Elem.setAttribute("unit", sig->m_unit);
            Elem.setAttribute("value_default", QString::number(sig->m_value_default));
            Elem.setAttribute("minimum", QString::number(sig->m_minimum));
            Elem.setAttribute("maximum", QString::number(sig->m_maximum));
            Elem.setAttribute("scalingoffset", QString::number(sig->m_offset));
            Elem.setAttribute("ScalingFactor", QString::number(sig->m_factor));
            Elem.setAttribute("lengthbit", QString::number(sig->m_bitLen, 10));
            Elem.setAttribute("startbit", QString::number(sig->m_startBit+64, 10));
            Elem.setAttribute("startbit_oth", QString::number(sig->m_startBit+64, 10));
            Elem.setAttribute("byteorder", QString::number(sig->m_byteOrder,10));
            Elem.setAttribute("type", QString::number(sig->m_valType, 10));
            Elem.setAttribute("param_name", sig->m_name);
            Elem.setAttribute("set_value", QString::number(sig->m_value));

        }
    }

    QString xmlString = xml.toString();
    QTextStream out(&xmlFile);
    out << xmlString;

    xmlFile.close();

    return true;
}

bool CanSetDlg::loadXml()
{
    QString appPath = QApplication::applicationDirPath();

    QString intfName = ui->ComBox_Interface->currentText();

    QString fileName = "NIXNET_" +intfName + ".xml";
    QString xmlPath = appPath + "/pamFactory/" + fileName;
    qDebug()<<"Load xml file:"<<xmlPath;

    if (xmlPath.isEmpty())
        return false;

    QFile xmlFile(xmlPath);
    if (!xmlFile.open(QFile::ReadOnly | QFile::Text))
    {
        qDebug()<<tr("Cannot read file %1:\n%2.").arg(xmlPath).arg(xmlFile.errorString());
        return false;
    }

    QDomDocument xml("NIXNET_CAN");

    if (!xml.setContent(&xmlFile))
    {
        xmlFile.close();
        return false;
    }
    xmlFile.close();

    QDomElement root = xml.documentElement();

    QString dbcPath = root.attribute("dbcfile");
    QString dbcPath_ref = root.attribute("dbcfile_ref");
    QString clusterName = root.attribute("cluster_name");

    if(!dbcPath_ref.isEmpty())
    {
        load_dbc_xnet(QApplication::applicationDirPath() + dbcPath_ref);

    }
    //QString intfName = root.attribute("interface");
    //ui->ComBox_Interface->setCurrentText(intfName);

    ui->ComBox_CANIOMode->setCurrentIndex(root.attribute("can_io_mode").toInt());
    ui->ComBox_CanFDStandard->setCurrentIndex(root.attribute("canfd_standard").toInt());

    ui->ComBox_Baudrate->setCurrentText(root.attribute("can_baudrate"));
    ui->ComBox_CANFD_Baudrate->setCurrentText(root.attribute("canfd_baudrate")); // toLongLong

    ui->checkBox_Termination->setChecked(root.attribute("termination_enable").toInt() > 0 ? true : false);
    ui->checkBox_CustomBD->setChecked(root.attribute("custom_abit").toInt() > 0 ? true : false);
    ui->checkBox_CustomFDBD->setChecked(root.attribute("custom_dbit").toInt() > 0 ? true : false);
    ui->doubleSpinBox_SamplePointBD->setValue(root.attribute("samplepoint_abit").toDouble());
    ui->doubleSpinBox_SamplePointFDBD->setValue(root.attribute("samplepoint_dbit").toDouble());

    QString modName = "NIXNET_" + intfName;

    QList<dbc_frame*> readFrames;
    QList<dbc_frame*> writeFrames;

    QDomElement msgElem = root.firstChildElement();
    while (!msgElem.isNull())
    {

        QString boName = msgElem.tagName();
        //dbc_frame* frame = getBoByName(boName);

        quint32 id = msgElem.attribute("id").toInt(0,16);
        dbc_frame* frame = getFrameById(id);

        if(!frame)
        {
            msgElem = msgElem.nextSiblingElement();
            continue;
        }

        qDebug()<<"frame Name ="<<frame->m_name<<",id="<<frame->m_id;

        QDomElement sigElem = msgElem.firstChildElement();
        while (!sigElem.isNull())
        {
            QString sigName = sigElem.attribute("param_name");
            dbc_signal* signal = getSgByName(frame, sigName);
            if(!signal)
            {
                sigElem = sigElem.nextSiblingElement();
                continue;
            }

            signal->m_value = sigElem.attribute("set_value").toDouble();

            sigElem = sigElem.nextSiblingElement();
        }


        if (frame->m_id != 0)
        {
            int isRead = msgElem.attribute("is_read").toInt();
            if (isRead)
            {
                readFrames.append(frame);
            }
            else
            {
                writeFrames.append(frame);
            }
        }

        msgElem = msgElem.nextSiblingElement();

    }

    loadFramesToReadTree(readFrames);
    loadFramesToWriteTree(writeFrames);

    return true;
}
*/

QString CanSetDlg::getCanSetFilePath() const
{
    return canSetFilePath;
}

void CanSetDlg::accept()
{
    writeSettingXml();
    QDialog::accept();
}

QString CanSetDlg::getCanSetFileName() const
{
    return canSetFileName;
}

void CanSetDlg::on_pb_loadDatabase_clicked()
{
    QString filePath = QFileDialog::getOpenFileName(this,tr("Load dbc file..."),"D:", tr("dbc file(*.dbc *.DBC *.xml *.arxml *.ARXML)"));
    if(filePath.isEmpty())
        return;
    qDebug()<<"Selected database file path = "<<filePath;

    QString filePath_ref = filePath;
    filePath_ref.replace(QCoreApplication::applicationDirPath(), "");
    qDebug()<<"Selected database file ref path = "<<filePath_ref;

    setDatabaseHeader(filePath);

    if(filePath.contains(".arxml") || filePath.contains(".ARXML"))
    {
        loadArxml(filePath);

        canConfig.sel_arCluster = ui->ComBox_Cluster->currentText();
        QString dbcFile = clusterDbcHash.value(canConfig.sel_arCluster);
        dbc_Cluster *cluster = loadDbcFromArxml(dbcFile);
        showClusterInfo(cluster);
    }
    else if (!filePath.isEmpty())
    {
        loadDbc(filePath);

        canConfig.sel_cluster = ui->ComBox_Cluster->currentText();
        dbc_Cluster *cluster = clusterHash.value(canConfig.sel_cluster);
        showClusterInfo(cluster);
    }

    connect(ui->ComBox_Cluster, SIGNAL(currentTextChanged(QString)), this, SLOT(clusterChangedSlot(QString)));
}

void CanSetDlg::on_pb_reset_clicked()
{
    int result = QMessageBox::warning(this,
                         QString::fromLocal8Bit("Reset"),
                         QString::fromLocal8Bit("Confirm to clear selection? \n "),
                         QMessageBox::Ok | QMessageBox::Cancel, QMessageBox::NoButton
                         );
    if (result == QMessageBox::Cancel)
    {

    }
    else
    {
        ui->traceTree_Read->clear();
        ui->traceTree_Write->clear();

        ui->traceTree_Read->setColumnCount(6);
        ui->traceTree_Write->setColumnCount(6);
        QStringList headerLabels;
        headerLabels<<"Message"<<"Data"<<"Pam0"<<"Pam1"<<"Pam2"<<"Pam3";

        ui->traceTree_Read->setHeaderLabels(headerLabels);
        ui->traceTree_Write->setHeaderLabels(headerLabels);

        ui->traceTree_Read->setColumnWidth(0, 300);
        ui->traceTree_Write->setColumnWidth(0, 300);
    }
}
