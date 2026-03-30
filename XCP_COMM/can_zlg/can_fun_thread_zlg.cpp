#include "can_fun_thread_zlg.h"
#include <QDebug>
#include <QtCore/qmath.h>
#include <QCoreApplication>

Can_Fun_Thread_ZLG::Can_Fun_Thread_ZLG(QObject *parent)
{

}

Can_Fun_Thread_ZLG::~Can_Fun_Thread_ZLG()
{
    stopCan();
}

void Can_Fun_Thread_ZLG::setFunInfo(FunStruct *value, int index)
{
    fun = value;
    funIndex = index;
}

void Can_Fun_Thread_ZLG::run()
{
    init();

    startCan();

    exec();
}

void Can_Fun_Thread_ZLG::Slt_CanStateChanged(quint32 canIndex, Can_Thread_ZLG::CAN_STATE state)
{
    if(state != curState)
    {
        curState = state;

        int funState = 0;
        if(curState == Can_Thread_ZLG::OPEN_ERR || curState == Can_Thread_ZLG::INIT_ERR || curState == Can_Thread_ZLG::R_W_ERR)
        {
            funState = -1;
        }
        else if(curState == Can_Thread_ZLG::INIT_OK)
        {
            funState = 1;
        }
        else if(curState == Can_Thread_ZLG::START_OK)
        {
            funState = 2;
        }
        else if(curState == Can_Thread_ZLG::R_W_ING)
        {
            funState = 3;
        }
        else
        {
            funState = 0;
        }


        emit canFunStateChanged(funState, funIndex);
    }
}

void Can_Fun_Thread_ZLG::Slt_ReadFrameRawDataReceived(quint32 id, quint8 *data, quint8 len)
{

    quint8 *buf = new quint8[len-8];
    memcpy(buf, data, 8);
    memcpy(buf+8, data+16, len-16);   

    QString keyName = "RP_ZLGCAN" + QString::number(canIndex) + "_" + QString::number(id);

    emit canDataForRecord(buf, len-8, keyName);

    //qDebug()<<"Frame:"<<id<<", Timestamp:"<<*(quint64*)(buf);

}

void Can_Fun_Thread_ZLG::Slt_FrameSigValueUpdated()
{
    for(int i = 0; i < dbc_frames_r.count(); i++)
    {
        BO_ *frame = dbc_frames_r.at(i);
        //qDebug()<<"frame:"<<frame->m_name<<", dlc:"<<frame->m_length;

        if(!frame) continue;

        QString keyName = "RP_ZLGCAN" + QString::number(canIndex) + "_" + QString::number(frame->m_id);
        QSharedMemory *sm = readFrameSmHash.value(keyName);

        char *buf = new char[8+frame->m_length];

        if(!copyFromSharedMemory(sm, 0, buf, 8 + frame->m_length))
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
            qreal time_s = (qreal)(timeStamp_on) / 10000000.0;   //统一按照NI的硬件时间戳处理：精度100ns
            pams.at(0)->setValue(time_s);
        }

        for(int i = 0; i < frame->m_sgList.count(); i++)
        {
            SG_ *sig = frame->m_sgList.at(i);

            if(!sig)
                continue;

            float sigValue = 0;
            if(sig->m_valType == 1)
            {
                quint32 sigUIntData = 0;
                if(sig->m_byteorder == 0)
                {
                    sigUIntData = CanRawMoto2UIntUserData((quint8*)buf + 8, sig->m_startBit, sig->m_bitLen);
                }
                else
                {
                    sigUIntData = CanRawIntel2UIntUserData((quint8*)buf + 8, sig->m_startBit, sig->m_bitLen);
                }
                sigValue = sigUIntData * sig->m_factor + sig->m_offset;

                //qDebug()<<"sig:"<<sig->m_name<<", sb:"<<sig->m_startBit<<",lb:"<<sig->m_bitLen<<", sigUIntData:"<<sigUIntData;
            }
            else if(sig->m_valType == 0)
            {
                qint32 sigIntData = 0;
                if(sig->m_byteorder == 0)
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
                if(sig->m_byteorder == 0)
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
                //qDebug()<<"pam:"<<pam->Name<<", value:"<<pam->getValue();
            }
        }

        delete[] buf;
    }
}

void Can_Fun_Thread_ZLG::Slt_WriteFrameSigValueUpdated()
{
    for(int i = 0; i < dbc_frames_w.count(); i++)
    {
        BO_ *frame = dbc_frames_w.at(i);
        //qDebug()<<"frame:"<<frame->m_name;

        if(!frame) continue;

        QString keyName = "WP_ZLGCAN" + QString::number(canIndex) + "_" + QString::number(frame->m_id);
        QList<PARAM*> pams = writePamListHash.value(keyName);

        for(int i = 0; i < frame->m_sgList.count(); i++)
        {
            SG_ *sig = frame->m_sgList.at(i);

            if(!sig) continue;
            if(sig->m_name.contains("_RC") || sig->m_name.contains("_CRC")) continue;
            //if(sig->assignMode > 1 || (pams.count()-1) <= i) continue;

            PARAM *pam = pams.at(i+1);
            if(pam)
            {
                sig->m_value = pam->getValue();
            }
        }
    }

}

void Can_Fun_Thread_ZLG::init()
{
    if(zlgcanIns == NULL)
    {
        zlgcanIns = new Can_Thread_ZLG();
    }

    connect(zlgcanIns, SIGNAL(stateChanged(quint32, Can_Thread_ZLG::CAN_STATE)), this, SLOT(Slt_CanStateChanged(quint32, Can_Thread_ZLG::CAN_STATE)));
    //connect(zlgcanIns, SIGNAL(readFrameRawDataReceived(quint32, quint8*, quint8)), this, SLOT(Slt_ReadFrameRawDataReceived(quint32, quint8*, quint8)));
    //connect(zlgcanIns, SIGNAL(readFrameRawDataReceived(quint32, quint8*, quint8)), this, SLOT(Slt_ReadFrameSmUpdated(quint32,quint8*,quint8)));
    connect(zlgcanIns, SIGNAL(canDataForRecord(quint8*,quint32,QString)), this, SIGNAL(canDataForRecord(quint8*,quint32,QString)));

    initDeviceTypeNumHash();

    loadXml();

    qDebug()<<"=== tscan read setting xml end =====";

    packFramesIntoPamsHash();

    //qDebug()<<"pack frames to pamHash end=====";

    setCanCommInfo();

    qDebug()<<"set tscan comm info end=====";
}

bool Can_Fun_Thread_ZLG::startCan()
{

    if(!zlgcanIns) return false;

    zlgcanIns->start();

    //wait for can state changed
    waitForCanThreadStateReady(zlgcanIns, 3000);

    canStartFinished = true;

    this->readFrameSmHash = zlgcanIns->getReadFrameSmHash();
    this->writeFrameSmHash = zlgcanIns->getWriteFrameSmHash();

    QTimer *timer = new QTimer();
    timer->setInterval(10);
    connect(timer, SIGNAL(timeout()), this, SLOT(Slt_FrameSigValueUpdated()));
    connect(timer, SIGNAL(timeout()), this, SLOT(Slt_WriteFrameSigValueUpdated()));
    timer->start();

    return true;
}

void Can_Fun_Thread_ZLG::stopCan()
{
    canStartFinished = false;

    timeStamp_on = 0;
    timeStamp_start = 0;

    if(zlgcanIns)
    {
        //disconnect(zlgcanIns, SIGNAL(readFrameRawDataReceived(quint32, quint8*, quint8)), this, SLOT(Slt_ReadFrameRawDataReceived(quint32, quint8*, quint8)));

        zlgcanIns->setIsStop(true);
        zlgcanIns->quit();
        zlgcanIns->wait();


        delete zlgcanIns;
        zlgcanIns = NULL;

        qDebug()<<"=====del zlgcanIns=====";

        emit canFunStateChanged(0, funIndex);
    }

    qDeleteAll(dbc_frames_r);
    dbc_frames_r.clear();
    qDeleteAll(dbc_frames_w);
    dbc_frames_w.clear();

    qDeleteAll(readPamList);
    readPamList.clear();
    readPamListHash.clear();

    qDeleteAll(writePamList);
    writePamList.clear();
    writePamListHash.clear();

    qDebug()<<"=====stop tscan comm=====";

}

void Can_Fun_Thread_ZLG::waitForCanThreadStateReady(Can_Thread_ZLG *thread, quint32 waitTime_ms)
{
    if(!thread)
        return;
    int count = waitTime_ms / 500;
    int index = 0;
    while(!(thread->getCurState() == Can_Thread_ZLG::R_W_ING) && index <= count)
    {
        wait_ms(500);

        index++;
    }
    return;
}

void Can_Fun_Thread_ZLG::wait_ms(int msec)
{
    QTime dieTime = QTime::currentTime().addMSecs(msec);
    while( QTime::currentTime() < dieTime )
        QCoreApplication::processEvents(QEventLoop::AllEvents, 100);
}

void Can_Fun_Thread_ZLG::initDeviceTypeNumHash()
{
#ifdef Q_OS_WIN
    deviceTypeNumHash.insert("ZCAN_USBCANFD_100U", 42);
    deviceTypeNumHash.insert("ZCAN_USBCANFD_200U", 41);
    deviceTypeNumHash.insert("ZCAN_USBCANFD_MINI", 43);
#endif
}

BO_ *Can_Fun_Thread_ZLG::getBoById(quint32 id)
{
    if(m_cluster == NULL)
        return NULL;
    if(m_cluster->m_boList.isEmpty())
        return NULL;
    foreach (BO_* bo, m_cluster->m_boList) {
        if(!bo)
            continue;
        if(bo->m_id == id)
            return bo;
    }
    return NULL;
}

SG_ *Can_Fun_Thread_ZLG::getSgByName(BO_ *bo, QString sgName)
{
    if(!bo)
        return NULL;
    if(bo->m_sgList.isEmpty())
        return NULL;
    foreach (SG_* sg, bo->m_sgList) {
        if(!sg)
            continue;
        if(sg->m_name == sgName)
            return sg;
    }

    return NULL;
}

bool Can_Fun_Thread_ZLG::loadXml()
{
    dbc_frames_r.clear();
    dbc_frames_w.clear();

    if(fun->setFilePath.isEmpty())
        return false;

    QString xmlPath = QCoreApplication::applicationDirPath() + fun->setFilePath;
    qDebug()<<"Load xml file:"<<xmlPath;

    QFile xmlFile(xmlPath);
    if (!xmlFile.open(QFile::ReadOnly | QFile::Text))
    {
        qDebug()<<tr("Cannot read file %1:\n%2.").arg(xmlPath).arg(xmlFile.errorString());
        return false;
    }

    QDomDocument xml("TS_CAN");

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
        loadDbc(QCoreApplication::applicationDirPath() + dbcPath_ref);
    }
    deviceType = root.attribute("device_type");
    deviceTypeIndex = deviceTypeNumHash.value(deviceType);

    deviceIndex = root.attribute("device_index").toInt();
    canIndex = root.attribute("can_index").toInt();
    canIoMode = (root.attribute("can_io_mode").toUInt());
    canfdStandard = (root.attribute("canfd_standard").toUInt());

    canBaudrate = (root.attribute("can_baudrate").toULongLong());
    canfdBaudrate = (root.attribute("canfd_baudrate").toULongLong()); // toLongLong

    terminationEnable = (root.attribute("termination_enable").toInt() > 0 ? true : false);
    enableCustomBD = (root.attribute("custom_abit").toInt() > 0 ? true : false);
    enableCustomFDBD = (root.attribute("custom_dbit").toInt() > 0 ? true : false);
    samplePointBD = (root.attribute("samplepoint_abit").toDouble());
    ssamplePointFDBD = (root.attribute("samplepoint_dbit").toDouble());

    QString modName = "ZLGCAN" + QString::number(canIndex);

    QList<BO_*> readFrames;
    QList<BO_*> writeFrames;

    QDomElement msgElem = root.firstChildElement();
    while (!msgElem.isNull())
    {

        QString boName = msgElem.tagName();

        quint32 id = msgElem.attribute("id").toInt(0,16);
        BO_* frame = getBoById(id);

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
            SG_* signal = getSgByName(frame, sigName);
            if(!signal)
            {
                sigElem = sigElem.nextSiblingElement();
                continue;
            }

            signal->m_value = sigElem.attribute("set_value").toDouble();

            //qDebug()<<"sigName="<<signal->m_name<<", setValue="<<signal->m_value;

            //if (signal->m_name != "Time")
            //    frame->m_sgList.append(signal);

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

    dbc_frames_r = readFrames;
    dbc_frames_w = writeFrames;

    return true;
}

void Can_Fun_Thread_ZLG::setCanCommInfo()
{
    if(!zlgcanIns)
        return;

    zlgcanIns->setDeviceType(deviceTypeIndex);
    zlgcanIns->setDeviceIndex(deviceIndex);
    zlgcanIns->setCanIndex(canIndex);
    zlgcanIns->setCanfd_abitBaudrate(canBaudrate);
    zlgcanIns->setCanfd_dbitBaudrate(canfdBaudrate);
    zlgcanIns->setResistanceEnable(terminationEnable);
    zlgcanIns->setCanType(canIoMode > 0 ? 1 : 0);
    zlgcanIns->setCanfd_standard(canfdStandard);
    zlgcanIns->setIsCustomBaud(enableCustomBD);
    zlgcanIns->setModName(modName);

    zlgcanIns->setReadFrameList(this->dbc_frames_r);
    zlgcanIns->setWriteFrameList(this->dbc_frames_w);

    zlgcanIns->setReadSmHash(this->readSmHash);
    zlgcanIns->setWriteSmHash(this->writeSmHash);

    ZCAN_CHANNEL_INIT_CONFIG canPortConfig;
    canPortConfig.can_type = canIoMode > 0 ? TYPE_CANFD : TYPE_CANFD;   // even if is can type, still set as canfd type

    canPortConfig.can.acc_code = 0;
    canPortConfig.can.acc_mask = 0xFFFFFFFF; // SJA1000帧过滤屏蔽码，设置成1代表“无关位”;
    canPortConfig.can.filter = 0;
    canPortConfig.can.mode = 0;
    canPortConfig.can.timing0 = 0;
    canPortConfig.can.timing1 = 0;
    canPortConfig.can.reserved = 0;

    canPortConfig.canfd.acc_code = 0;
    canPortConfig.canfd.acc_mask = 0;
    canPortConfig.canfd.abit_timing = 0;
    canPortConfig.canfd.dbit_timing = 0;
    canPortConfig.canfd.brp = 0;
    canPortConfig.canfd.filter = 0;
    canPortConfig.canfd.mode = 0;         // 0=normal, 1=only listen
    canPortConfig.canfd.pad = 0;
    canPortConfig.canfd.reserved = 0;

    zlgcanIns->setCanPortInitConfig(canPortConfig);

}


void Can_Fun_Thread_ZLG::loadDbc(QString dbcFile)
{
    if(dbcFile.isEmpty())
        return;

    DbcParser *dbcParser = new DbcParser();
    if(dbcParser->doParsering(dbcFile))
    {

        CLUSTER_ *cluster =  dbcParser->getCluster();
        if(cluster)
        {
            m_cluster = cluster;
        }
    }
    else
    {
        qDebug()<<"dbc parsing error.";
    }
}


QList<PARAM *> Can_Fun_Thread_ZLG::fromDbcFrameSigsToPams(BO_ *frame, QString key)
{
    QList<PARAM*> pamList;
    if(!frame)
        return pamList;

    int sigCount = frame->m_sgList.count();
    int smSize = frame->m_length + 8;
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
        SG_ *sig = frame->m_sgList.at(i);
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
        pam->LowerLimit = sig->m_min;
        pam->UpperLimit = sig->m_max;
        pam->ScalingFactor = sig->m_factor;
        pam->ScalingOffset = sig->m_offset;
        pam->LengthBit = sig->m_bitLen;
        pam->StartBit = sig->m_startBit + startBitIndex;
        pam->StartByte = pam->StartBit/8;

        pam->ByteOrder = sig->m_byteorder;
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

void Can_Fun_Thread_ZLG::packFramesIntoPamsHash()
{
    for(int i = 0; i < dbc_frames_r.count(); i++)
    {
        BO_ *frame = dbc_frames_r.at(i);

        if(!frame) continue;

        QString keyName = "RP_ZLGCAN" + QString::number(canIndex) + "_" + QString::number(frame->m_id);
        QList<PARAM*> pamList = fromDbcFrameSigsToPams(frame, keyName);

        //qDebug()<<"keyName:"<<keyName;

        readPamListHash.insert(keyName, pamList);
    }

    for(int i = 0; i < dbc_frames_w.count(); i++)
    {
        BO_ *frame = dbc_frames_w.at(i);

        if(!frame) continue;

        QString keyName = "WP_ZLGCAN" + QString::number(canIndex) + "_" + QString::number(frame->m_id);
        QList<PARAM*> pamList = fromDbcFrameSigsToPams(frame, keyName);

        //qDebug()<<"keyName:"<<keyName;

        writePamListHash.insert(keyName, pamList);
    }

}

QHash<QString, QList<PARAM *> > Can_Fun_Thread_ZLG::getWritePamListHash() const
{
    return writePamListHash;
}

bool Can_Fun_Thread_ZLG::getCanStartFinished() const
{
    return canStartFinished;
}

QHash<QString, QList<PARAM *> > Can_Fun_Thread_ZLG::getReadPamListHash() const
{
    return readPamListHash;
}
