#include "can_fun_thread.h"
#include <QDebug>
#include <QtCore/qmath.h>

Can_Fun_Thread::Can_Fun_Thread(QObject *parent)
{

}

Can_Fun_Thread::~Can_Fun_Thread()
{
    stopCanComm();
}

void Can_Fun_Thread::setFunInfo(FunStruct *value, int index)
{
    fun = value;
    funIndex = index;
}

void Can_Fun_Thread::run()
{
    init();

    startCanComm();

    exec();
}

void Can_Fun_Thread::Slt_CanStateChanged(int state)
{
    if(state != curState)
    {
        curState = state;
        emit canFunStateChanged(state, funIndex);
    }
}

void Can_Fun_Thread::Slt_ReadFrameRawDataReceived(quint32 id, quint8 *data, quint8 len)
{

    quint8 *buf = new quint8[len-8];
    memcpy(buf, data, 8);
    memcpy(buf+8, data+16, len-16);   

    QString keyName = "RP_" + sel_interface + "_" + QString::number(id);

    emit canDataForRecord(buf, len-8, keyName);

    //qDebug()<<"Frame:"<<id<<", Timestamp:"<<*(quint64*)(buf);

}

void Can_Fun_Thread::Slt_ReadFrameSmUpdated(quint32 id, quint8 *data, quint8 len)
{
    QString keyName = "RP_" + sel_interface + "_" + QString::number(id);
    QSharedMemory *sm = readFrameSmHash.value(keyName);

    //copy frame timestamp and payload
    copyToSharedMemory_2Data(sm, 0, (char*)data, 8, 8, (char*)data+16, len-16);
}

void Can_Fun_Thread::Slt_FrameSigValueUpdated()
{
    for(int i = 0; i < dbc_frames_r.count(); i++)
    {
        dbc_frame *frame = dbc_frames_r.at(i);
        //qDebug()<<"frame:"<<frame->m_name;

        if(!frame) continue;

        QString keyName = "RP_" + sel_interface + "_" + QString::number(frame->m_id);
        QSharedMemory *sm = readFrameSmHash.value(keyName);
        char *buf = new char[8+frame->m_dlc];

        if(!copyFromSharedMemory(sm, 0, buf, 8 + frame->m_dlc))
            continue;

        if(timeStamp_start == 0)
        {
            timeStamp_start = *(quint64*)buf;
        }
        else
        {
            timeStamp_on = *(quint64*)buf - timeStamp_start;
        }


        QList<PARAM*> pams = readPamListHash.value(keyName);

        if(pams.count() > 1)
        {
            qreal time_s = (qreal)(timeStamp_on) / 10000000.0;
            pams.at(0)->setValue(time_s);
        }

        for(int i = 0; i < frame->m_sigs.count(); i++)
        {
            dbc_signal *sig = frame->m_sigs.at(i);

            if(!sig)
                continue;

            float sigValue = 0;
            if(sig->m_valType == 1)
            {
                quint32 sigUIntData = 0;
                if(sig->m_byteOrder == 1)
                {
                    sigUIntData = CanRawMoto2UIntUserData((quint8*)buf + 8, sig->m_startBit, sig->m_bitLen);
                }
                else
                {
                    sigUIntData = CanRawIntel2UIntUserData((quint8*)buf + 8, sig->m_startBit, sig->m_bitLen);
                }
                sigValue = sigUIntData * sig->m_factor + sig->m_offset;
            }
            else if(sig->m_valType == 0)
            {
                qint32 sigIntData = 0;
                if(sig->m_byteOrder == 1)
                {
                    sigIntData = CanRawMoto2IntUserData((quint8*)buf + 8, sig->m_startBit, sig->m_bitLen);
                }
                else
                {
                    sigIntData = CanRawIntel2IntUserData((quint8*)buf + 8, sig->m_startBit, sig->m_bitLen);
                }

                if(sig->m_bitLen <= 32)
                {
                    //if(sig->m_bitLen == 32)
                        //qDebug()<<"sig:"<<sig->m_name<<",sigIntData:"<<sigIntData;

                    int intData = sigIntData & ((1 << (sig->m_bitLen-1))-1);
                    int sign = sigIntData >> (sig->m_bitLen - 1) == 0 ? 1 : -1;
                    sigIntData = sign > 0 ? intData : -(qPow(2, (sig->m_bitLen-1)) - intData);
                }

                sigValue = sigIntData * sig->m_factor + sig->m_offset;

                //qDebug()<<"sig:"<<sig->m_name<<", sigIntData:"<<sigIntData;
            }
            else if(sig->m_valType == 2)
            {
                // type float
                quint32 sigUIntData = 0;
                if(sig->m_byteOrder == 1)
                {
                    sigUIntData = CanRawMoto2UIntUserData((quint8*)buf + 8, sig->m_startBit, sig->m_bitLen);
                }
                else
                {
                    sigUIntData = CanRawIntel2UIntUserData((quint8*)buf + 8, sig->m_startBit, sig->m_bitLen);
                }

                float tempValue = *(float*)&sigUIntData;
                sigValue = tempValue * sig->m_factor + sig->m_offset;
            }

            sig->m_value = sigValue;

            //qDebug()<<"sig:"<<sig->m_name<<", value:"<<sigValue;

            if((pams.count()-1) <= i)
                continue;

            PARAM *pam = pams.at(i+1);
            if(pam)
            {
                pam->setValue(sigValue);
                //qDebug()<<"pam:"<<pam->m_name<<", value:"<<pam->getValue();
            }
        }

        delete[] buf;
    }
}

void Can_Fun_Thread::Slt_WriteFrameSigValueUpdated()
{
    for(int i = 0; i < dbc_frames_w.count(); i++)
    {
        dbc_frame *frame = dbc_frames_w.at(i);
        //qDebug()<<"frame:"<<frame->m_name;

        if(!frame) continue;

        QString keyName = "WP_" + sel_interface + "_" + QString::number(frame->m_id);
        QList<PARAM*> pams = writePamListHash.value(keyName);

        for(int i = 0; i < frame->m_sigs.count(); i++)
        {
            dbc_signal *sig = frame->m_sigs.at(i);

            if(!sig) continue;
            if(sig->m_name.contains("_RC") || sig->m_name.contains("_CRC")) continue;
            if(sig->assignMode > 1 || (pams.count()-1) <= i) continue;

            PARAM *pam = pams.at(i+1);
            if(pam)
            {
                sig->m_value = pam->getValue();
            }
        }
    }

}

void Can_Fun_Thread::init()
{
    if(nixnetIns == NULL)
    {
        nixnetIns = new NIXNET_Thread();
    }

    connect(nixnetIns, SIGNAL(stateChanged(int)), this, SLOT(Slt_CanStateChanged(int)));
    connect(nixnetIns, SIGNAL(readFrameRawDataReceived(quint32, quint8*, quint8)), this, SLOT(Slt_ReadFrameRawDataReceived(quint32, quint8*, quint8)));
    connect(nixnetIns, SIGNAL(readFrameRawDataReceived(quint32, quint8*, quint8)), this, SLOT(Slt_ReadFrameSmUpdated(quint32,quint8*,quint8)));

    readSettingXml();

    qDebug()<<"read setting xml end=====";

    packFramesIntoPamsHash();

    qDebug()<<"pack frames to pamHash end=====";

    setCanCommInfo();

    qDebug()<<"set can comm info end=====";
}

bool Can_Fun_Thread::startCanComm()
{

    if (!nixnetIns->deviceConnect())
    {
        qDebug()<<"=====start can comm fail=====";
        emit canFunStateChanged(-1, funIndex);
        return false;
    }
    canStartFinished = true;

    this->readFrameSmHash = nixnetIns->getReadFrameSmHash();
    this->writeFrameSmHash = nixnetIns->getWriteFrameSmHash();

    QTimer *timer = new QTimer();
    timer->setInterval(10);
    connect(timer, SIGNAL(timeout()), this, SLOT(Slt_FrameSigValueUpdated()));
    connect(timer, SIGNAL(timeout()), this, SLOT(Slt_WriteFrameSigValueUpdated()));
    timer->start();

    return true;
}

void Can_Fun_Thread::stopCanComm()
{
    canStartFinished = false;

    timeStamp_on = 0;
    timeStamp_start = 0;

    if(nixnetIns)
    {
        disconnect(nixnetIns, SIGNAL(readFrameRawDataReceived(quint32, quint8*, quint8)), this, SLOT(Slt_ReadFrameRawDataReceived(quint32, quint8*, quint8)));

        nixnetIns->deviceDisconnect();

        delete nixnetIns;
        nixnetIns = NULL;

        qDebug()<<"=====del nixnetIns=====";
    }

    if(curCluster)
    {
        delete curCluster;
        curCluster = NULL;
    }

    qDeleteAll(dbc_frames);
    dbc_frames.clear();
    qDeleteAll(dbc_frames_r);
    dbc_frames_r.clear();
    qDeleteAll(dbc_frames_w);
    dbc_frames_w.clear();

    if(arXmlPackages)
    {
        delete arXmlPackages;
        arXmlPackages = NULL;
    }

    qDeleteAll(readPamList);
    readPamList.clear();
    readPamListHash.clear();

    qDeleteAll(writePamList);
    writePamList.clear();
    writePamListHash.clear();

    qDebug()<<"=====stop can comm=====";

}

bool Can_Fun_Thread::readSettingXml()
{
    if(fun->setFilePath.isEmpty())
        return false;

    sel_frames_r.clear();
    sel_frames_w.clear();
    sel_sigs_r.clear();
    sel_sigs_w.clear();

    dbc_frames.clear();

    QString filePath = QCoreApplication::applicationDirPath() + fun->setFilePath;
    xml_ins.setXmlFilePath(filePath);

    if (!xml_ins.readXML())
    {
        qDebug()<<"CAN function setting xml read error!";
        return false;
    }


    // xml配置文件中保存的信息复原

    isArxml = xml_ins.getIsArxml();
    sel_arxmlPath = xml_ins.getArxmlPath();
    sel_arCluster = xml_ins.getArCluster();
    sel_dbcPath = xml_ins.getDbcPath();
    sel_cluster = xml_ins.getCluster();
    sel_interface = xml_ins.getInterface();
    sel_baudrate = xml_ins.getBaudrate();
    sel_canio_mode = xml_ins.getCANIOMode();
    sel_canfd_baudrate = xml_ins.getCANFDBaudrate();
    sel_termination_on = (xml_ins.getCANTermination() > 0);
    sel_CustomBD_on = (xml_ins.getCustomBD() > 0);
    sel_CustomFDBD_on = (xml_ins.getCustomFDBD() > 0);
    sel_SamplePoint_BD = xml_ins.getSamplePoint_BD();
    sel_SamplePoint_FDBD = xml_ins.getSamplePoint_FDBD();

    sel_frames_r = xml_ins.getSel_frames_r();
    sel_frames_w = xml_ins.getSel_frames_w();
    sel_sigs_r = xml_ins.getSel_sigs_r();
    sel_sigs_w = xml_ins.getSel_sigs_w();

    dbc_frames_r = xml_ins.getFramesForRead();
    dbc_frames_w = xml_ins.getFramesForWrite();

    foreach (dbc_frame *frame, dbc_frames_r) {
        idSelFrameHash.insert(frame->m_id, frame);

    }
    foreach (dbc_frame *frame, dbc_frames_w) {
        idSelFrameHash.insert(frame->m_id, frame);
    }

    if(isArxml)
    {
        if(!sel_arxmlPath.isEmpty())
        {
            qDebug()<<"In xml sel [arCluster]:"<<sel_arCluster;

            loadArxml(sel_arxmlPath);

            QString filePath = clusterDbcHash.value(sel_arCluster);
            dbc_Cluster *cluster = loadDbcFromArxml(filePath);

            curCluster = cluster;
        }
    }
    else
    {
        if(!sel_dbcPath.isEmpty())
        {
            loadDbc(sel_dbcPath);
            dbc_Cluster *cluster = clusterHash.value(sel_cluster);
            qDebug()<<"sel cluster:["<<sel_cluster<<"]";

            curCluster = cluster;

        }
    }
    qDebug()<<"read database file end=====";
    if(!curCluster)
        return false;

    this->dbc_frames = curCluster->m_frames;

    if(nixnetIns)
    {
        nixnetIns->setFrames(curCluster->m_frames);
        nixnetIns->setFrames_r(this->dbc_frames_r);
        nixnetIns->setFrames_w(this->dbc_frames_w);
    }

    canSettingLoad = true;

    return true;
}

void Can_Fun_Thread::setCanCommInfo()
{
    if(!nixnetIns)
        return;

    nixnetIns->path = sel_dbcPath;
    nixnetIns->cluster = sel_cluster;

    nixnetIns->port = sel_interface;
    nixnetIns->baudrate = sel_baudrate;
    nixnetIns->canio_mode = sel_canio_mode;
    nixnetIns->canfd_baudrate = sel_canfd_baudrate;
    //nixnetIns->thread_Read->sleep_ms = sel_read_period;
    //nixnetIns->thread_Write->sleep_ms = sel_write_period;
    nixnetIns->canTermination = sel_termination_on;

    nixnetIns->customBD_on = sel_CustomBD_on;
    nixnetIns->customFDBD_on = sel_CustomFDBD_on;
    nixnetIns->samplePoint_BD = sel_SamplePoint_BD;
    nixnetIns->samplePoint_FDBD = sel_SamplePoint_FDBD;

    nixnetIns->frames_r = sel_frames_r;
    nixnetIns->frames_w = sel_frames_w;
    nixnetIns->sigs_r = sel_sigs_r;
    nixnetIns->sigs_w = sel_sigs_w;


}

void Can_Fun_Thread::loadDatabase(QString dbFilePath)
{
    if(dbFilePath.isEmpty())
        return;
    qDebug()<<"Loading database file path:"<<dbFilePath;

    if(dbFilePath.contains(".arxml") || dbFilePath.contains(".ARXML"))
    {
        loadArxml(dbFilePath);

        //sel_arCluster = ui->ComBox_Cluster->currentText();
        QString dbcFile = clusterDbcHash.value(sel_arCluster);
        dbc_Cluster *cluster = loadDbcFromArxml(dbcFile);

    }
    else if (!dbFilePath.isEmpty())
    {
        loadDbc(dbFilePath);

        //sel_cluster = ui->ComBox_Cluster->currentText();
        dbc_Cluster *cluster = clusterHash.value(sel_cluster);

    }
}

void Can_Fun_Thread::loadDbc(QString filePath)
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
                    can_format_motorola2intel_sb_lb(&sb_oth, (quint16*)&lengthbit);
                    cur_sig->m_startBit_oth = sb_oth;
                    cur_sig->m_bitLen = lengthbit;
                    cur_sig->m_value_default = defaultValue;
                    if(maxValue == 0)
                    {
                        maxValue = qPow(2, lengthbit) - 1;
                    }
                    cur_sig->m_maximum = maxValue;
                    cur_sig->m_minimum = minValue;
                    cur_sig->m_factor = factor;
                    cur_sig->m_offset = offset;
                    //qDebug()<<"signal:"<<cur_sig->m_name<<",type="<<cur_sig->m_valType<<", byteOrder:"<<cur_sig->m_byteOrder
                    //       <<", startbit: "<<cur_sig->m_startBit<<", oth startbit:"<<cur_sig->m_startBit_oth
                    //      <<", len:"<<cur_sig->m_bitLen;
                }

                cur_frame->m_sigs.append(cur_sig);
            }

            dbcFrames.append(cur_frame);

            idFrameHash.insert(cur_frame->m_id, cur_frame);
        }

        cluster->m_frames = dbcFrames;

        clusterHash.insert(clusterName, cluster);

        qDebug()<<"cluster name:["<<clusterName<<"]";
    }

    status_dbc = nxdbCloseDatabase(databaseRef, 1);
    if (status_dbc == nxSuccess)
    {
        qDebug()<<"Close Database Succeed!";
    }

    sel_dbcPath = filePath;
    isArxml = false;
    xml_ins.setIsArxml(false);
}

void Can_Fun_Thread::loadArxml(QString filePath)
{
    ArxmlPraser arxmlDatabase(this, filePath);
    if(arxmlDatabase.getPraserOk())
    {
        arXmlPackages = arxmlDatabase.getArXmlPackages();
        canClusters = arxmlDatabase.getCanClusters();
        genDbcFileList = arxmlDatabase.getGenDbcFileList();
        clusterDbcHash = arxmlDatabase.getClusterDbcHash();
        clusterHash.clear();

        foreach (dbc_Cluster* cluster, canClusters) {
            clusterHash.insert(cluster->m_clusterName, cluster);
        }
    }

    sel_arxmlPath = filePath;
    isArxml = true;
}

dbc_Cluster *Can_Fun_Thread::loadDbcFromArxml(QString filePath)
{
    if(filePath.isEmpty())
        return NULL;

    qDebug()<<"===========Load dbc cluster from in arxml===========";
    qDebug()<<"Select arCluster name:"<<sel_arCluster<<", arxml dbc file path:"<<filePath;

    sel_dbcPath = filePath;
    temp_path = sel_dbcPath.toLocal8Bit();
    dbcPath = temp_path.data();

    QString name;
    QStringList fileList = sel_dbcPath.split("/", QString::SkipEmptyParts);
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
                can_format_motorola2intel_sb_lb(&sb_oth, (quint16*)&lengthbit);
                cur_sig->m_startBit_oth = sb_oth;
                cur_sig->m_bitLen = lengthbit;
                cur_sig->m_value_default = defaultValue;
                cur_sig->m_maximum = maxValue;
                cur_sig->m_minimum = minValue;
                cur_sig->m_factor = factor;
                cur_sig->m_offset = offset;
                qDebug()<<"signal startbit: "<<cur_sig->m_startBit
                       <<", oth startbit:"<<cur_sig->m_startBit_oth
                      <<", len:"<<cur_sig->m_bitLen;
            }

            cur_frame->m_sigs.append(cur_sig);
        }

        dbcFrames.append(cur_frame);
        idFrameHash.insert(cur_frame->m_id, cur_frame);
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

QList<dbc_frame *> Can_Fun_Thread::getFramesByNames(QStringList frameNameList)
{
    QList<dbc_frame*> dbc_frames_temp;
    for (int i = 0; i < frameNameList.count(); i++)
    {
        QString frameString = frameNameList[i];
        int j = 0;
        dbc_frame *frame_temp = dbc_frames[j];
        while ((j < dbc_frames.count()) && (frame_temp->m_name != frameString))
        {
            j++;
            frame_temp = dbc_frames[j];
        }
        dbc_frames_temp.append(frame_temp);
    }
    return dbc_frames_temp;
}

QList<PARAM *> Can_Fun_Thread::fromDbcFrameSigsToPams(dbc_frame *frame, QString key)
{
    QList<PARAM*> pamList;
    if(!frame)
        return pamList;

    int sigCount = frame->m_sigs.count();
    int smSize = frame->m_dlc + 8;
    //qDebug()<<"sm size:"<<smSize;

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
    timePam->smSize = smSize;

    pamList.append(timePam);

    startBitIndex = 64;

    for(int i = 0; i < sigCount; i++)
    {
        dbc_signal *sig = frame->m_sigs.at(i);
        if(!sig)
            continue;
        PARAM *pam = new PARAM();

        pam->Name = sig->m_name;
        pam->Type = 0;
        if(sig->m_valType == 0)
        {
            pam->Type = 1;
        }
        else if(sig->m_valType == 1)
        {
            pam->Type = 0;
        }
        else
        {
            pam->Type = sig->m_valType;
        }

        pam->Unit = sig->m_unit;
        pam->DefaultValue = sig->m_value_default;
        pam->LowerLimit = sig->m_minimum;
        pam->UpperLimit = sig->m_maximum;
        pam->ScalingFactor = sig->m_factor;
        pam->ScalingOffset = sig->m_offset;
        pam->LengthBit = sig->m_bitLen;
        pam->StartBit = sig->m_startBit + startBitIndex;
        pam->StartByte = pam->StartBit/8;

        pam->ByteOrder = sig->m_byteOrder;
        if(pam->ByteOrder == 1)               //motorola  for mdf cn type  (9=unsigned, 10=signed, 11=float, 12=double)
        {
            pam->StartBit = sig->m_startBit_oth + startBitIndex;
            pam->Type += 9;
            pam->StartByte = pam->StartBit/8;
        }

        pam->smKey = key;
        pam->smSize = smSize;

        pamList.append(pam);
    }

    return pamList;
}

void Can_Fun_Thread::packFramesIntoPamsHash()
{
    for(int i = 0; i < dbc_frames_r.count(); i++)
    {
        dbc_frame *frame = dbc_frames_r.at(i);

        if(!frame) continue;

        QString keyName = "RP_" + sel_interface + "_" + QString::number(frame->m_id);
        QList<PARAM*> pamList = fromDbcFrameSigsToPams(frame, keyName);

        //qDebug()<<"keyName:"<<keyName;

        readPamListHash.insert(keyName, pamList);
    }

    for(int i = 0; i < dbc_frames_w.count(); i++)
    {
        dbc_frame *frame = dbc_frames_w.at(i);

        if(!frame) continue;

        QString keyName = "WP_" + sel_interface + "_" + QString::number(frame->m_id);
        QList<PARAM*> pamList = fromDbcFrameSigsToPams(frame, keyName);

        //qDebug()<<"keyName:"<<keyName;

        writePamListHash.insert(keyName, pamList);
    }

}

QHash<QString, QList<PARAM *> > Can_Fun_Thread::getWritePamListHash() const
{
    return writePamListHash;
}

bool Can_Fun_Thread::getCanStartFinished() const
{
    return canStartFinished;
}

QHash<QString, QList<PARAM *> > Can_Fun_Thread::getReadPamListHash() const
{
    return readPamListHash;
}
