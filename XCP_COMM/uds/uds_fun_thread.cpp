#include "uds_fun_thread.h"

Uds_Fun_Thread::Uds_Fun_Thread(QObject *parent):
    QThread(parent)
{

}

Uds_Fun_Thread::~Uds_Fun_Thread()
{
    //stopUDS();

    if(udsWorker)
    {
        if(udsWorker->getIsActive())
        {
            stopUDS();
        }

        delete udsWorker;
        udsWorker = NULL;
        qDebug()<<"[UDS] Delete uds worker in deconstruct, fun:"<<fun->funName;
    }

    qDeleteAll(udsReadPamList);
    udsReadPamList.clear();

    qDeleteAll(udsWritePamList);
    udsWritePamList.clear();

    qDebug()<<"[UDS] uds thread deleted, fun:"<<fun->funName;
}

void Uds_Fun_Thread::preFun()
{
    readSetting();
}

void Uds_Fun_Thread::waitForUdsActive(bool udsActive, quint32 waitTime)
{
    if(!udsWorker) return;

    int stepMs = 200;
    int count = waitTime / stepMs;
    while(count > 0 && udsWorker->getIsActive() != udsActive)
    {
        count--;
        waitMs(stepMs);
    }
}

void Uds_Fun_Thread::setFunInfo(FunStruct *value, int index)
{
    fun = value;
    funIndex = index;
}

void Uds_Fun_Thread::setUdsInfo(const UDSInfo &value)
{
    udsInfo = value;
}

void Uds_Fun_Thread::run()
{
    qRegisterMetaType<Uds_Fun_Thread::UDS_STATE>("Uds_Fun_Thread::UDS_STATE");
    curState = INIT;
    lastState = curState;

    QByteArray temp;
    temp.resize(64);
    temp.fill(0);


    QSharedMemory readSm;
    QString readSmKey = "RP_UDS_" + fun->funName;
    int readSmSize = 64;
    readSm.setKey(readSmKey);
    readSm.create(readSmSize);
    bool attachStatus = readSm.attach();
    //qDebug()<<"attach status:"<<attachStatus;

    readSm.lock();
    memcpy((char*)readSm.data(), (char*)temp.data(), 64);
    readSm.unlock();

    QSharedMemory writeSm;
    QString writeSmKey = "WP_UDS_" + fun->funName;
    int writeSmSize = 64;
    writeSm.setKey(writeSmKey);
    writeSm.create(writeSmSize);
    writeSm.attach();

    *(quint64*)udsReqData = 0;
    *(quint64*)udsResData = 0;

    int count = 0;
    int countLim = udsInfo.testerCycleTime / 10;

    createPams();
    createPamXml();

    while(!isStop)
    {
        msleep(10);

        if(curState != lastState)
        {
            lastState = curState;
        }

        switch (curState) {
        case INIT:
        {
            init();

            emit udsFunStateChanged(1, funIndex);
            curState = STARTING;
        }
            break;
        case STARTING:
        {
            msleep(500);
            bool status = startUDS();
            emit udsFunStateChanged(2, funIndex);
            curState = status ? START_OK : ERR;
        }
            break;
        case START_OK:
        {
            time.restart();

            udsStartFinished = true;
            emit udsFunStateChanged(3, funIndex);
            curState = WORKING;

        }
            break;
        case WORKING:
        {
            if(!udsRun)
            {
                //stopUDS();
                curState = STOP;
            }
            else
            {
                count++;
                if(count >= countLim)
                {
                    udsWorker->uds_manl_testPresented(udsInfo.testerResponseRequired);
                    count = 0;
                }

                packWritePamList(&writeSm, writeSmSize);
                fromWriteSmToReqPams(&writeSm, writeSmSize);

                workUDS();
                updateUdsCmdActiveInSm(&writeSm, udsCmdActive);

                packResPamsToReadSm(&readSm, readSmSize);
                updateReadPamList();

                curState = WORKING;
            }
        }
            break;
        case ERR:
        {
            msleep(500);
            emit udsFunStateChanged(-1, funIndex);
            curState = STOP;
        }
            break;
        case STOP:
        {
            msleep(500);
            bool status = stopUDS();
            udsStartFinished = false;
            emit udsFunStateChanged(0, funIndex);
            curState = udsRun ? STARTING : END;
        }
            break;
        case END:
        {
            msleep(500);

            emit udsFunStateChanged(0, funIndex);
            if(udsWorker)
            {
                if(udsWorker->getIsActive())
                {
                    stopUDS();
                }

                delete udsWorker;
                udsWorker = NULL;
                qDebug()<<"[UDS] Delete uds worker in thread, fun:"<<fun->funName;
            }

            curState = udsRun ? INIT : END;
        }
            break;
        default:
            break;
        }
    }
}

bool Uds_Fun_Thread::readSetting()
{
    if(fun->setFilePath.isEmpty() || fun->intfName.isEmpty())
        return false;

    qDebug()<<"In uds fun thread: read setting file: "<<fun->setFilePath;
    QSettings setting(fun->setFilePath, QSettings::IniFormat);

    setting.beginGroup("UDSSetting");

    udsInfo.canIntf = setting.value("CanIntf").toString() + "@nixnet";
    udsInfo.canIoMode = setting.value("CanIoMode").toUInt();

    udsInfo.baudrate = setting.value("Baudrate").toUInt();
    udsInfo.baudrate_canfd = setting.value("Baudrate_CANFD").toUInt();

    bool ok = false;
    udsInfo.transmitID = setting.value("TransmitID").toString().toUInt(&ok, 16);
    udsInfo.receiveID = setting.value("ReceiveID").toString().toUInt(&ok, 16);

    udsInfo.maxDLC = setting.value("maxDLC").toUInt();
    udsInfo.securityConstant = setting.value("SecurityConstant").toString().toUInt(&ok, 16);

    udsInfo.testerActive = setting.value("TesterActive").toBool();
    udsInfo.testerCycleTime = setting.value("TesterCycleTime").toUInt();
    udsInfo.testerResponseRequired = setting.value("TesterResponseRequired").toBool();

    udsInfo.useDll = setting.value("DllActive").toBool();
    udsInfo.dllPath = setting.value("DllPath").toString();
    udsInfo.seedKeyType = setting.value("SeedKeyMethodIndex").toUInt();

    udsInfo.byteOrder = setting.value("ByteOrder").toUInt();

    udsInfo.autoFreeze = setting.value("AutoFreeze").toBool();
    udsInfo.timeBetweenCtrlAndFreeze = setting.value("TimeBetweenCtrlAndFreeze").toUInt();


    setting.endGroup();
}

void Uds_Fun_Thread::createPams()
{
    udsReadPamList.clear();
    udsWritePamList.clear();

    QString readSmKey = "RP_UDS_" + fun->funName;
    QString writeSmKey = "WP_UDS_" + fun->funName;

    PARAM *timePam_r = new PARAM();
    timePam_r->Unit = "s";
    timePam_r->DefaultValue = 0;
    timePam_r->LowerLimit = DBL_MIN;
    timePam_r->UpperLimit = DBL_MAX;
    timePam_r->ScalingFactor = 1;
    timePam_r->ScalingOffset = 0;
    timePam_r->LengthBit = 64;
    timePam_r->StartBit = 0;
    timePam_r->ByteOrder = 0;
    timePam_r->Type = 3;
    timePam_r->Name = "Time";
    timePam_r->smKey = readSmKey;

    udsReadPamList.append(timePam_r);

    PARAM *pam_udsActive = new PARAM();
    pam_udsActive->Unit = "";
    pam_udsActive->DefaultValue = 0;
    pam_udsActive->LowerLimit = 0;
    pam_udsActive->UpperLimit = 255;
    pam_udsActive->ScalingFactor = 1;
    pam_udsActive->ScalingOffset = 0;
    pam_udsActive->LengthBit = 8;
    pam_udsActive->StartBit = 64;
    pam_udsActive->ByteOrder = 0;
    pam_udsActive->Type = 0;
    pam_udsActive->Name = "udsActiveAct";
    pam_udsActive->smKey = readSmKey;

    udsReadPamList.append(pam_udsActive);

    PARAM *pam_udsCmdActive = new PARAM();
    pam_udsCmdActive->Unit = "";
    pam_udsCmdActive->DefaultValue = 0;
    pam_udsCmdActive->LowerLimit = 0;
    pam_udsCmdActive->UpperLimit = 255;
    pam_udsCmdActive->ScalingFactor = 1;
    pam_udsCmdActive->ScalingOffset = 0;
    pam_udsCmdActive->LengthBit = 8;
    pam_udsCmdActive->StartBit = 64 + 8;
    pam_udsCmdActive->ByteOrder = 0;
    pam_udsCmdActive->Type = 0;
    pam_udsCmdActive->Name = "udsCmdActive";
    pam_udsCmdActive->smKey = readSmKey;

    udsReadPamList.append(pam_udsCmdActive);

    PARAM *pam_udsCmd = new PARAM();
    pam_udsCmd->Unit = "";
    pam_udsCmd->DefaultValue = 0;
    pam_udsCmd->LowerLimit = 0;
    pam_udsCmd->UpperLimit = 255;
    pam_udsCmd->ScalingFactor = 1;
    pam_udsCmd->ScalingOffset = 0;
    pam_udsCmd->LengthBit = 8;
    pam_udsCmd->StartBit = 64 + 8*2;
    pam_udsCmd->ByteOrder = 0;
    pam_udsCmd->Type = 0;
    pam_udsCmd->Name = "udsCmdAct";
    pam_udsCmd->smKey = readSmKey;

    udsReadPamList.append(pam_udsCmd);

    PARAM *pam_resDataLen = new PARAM();
    pam_resDataLen->Unit = "";
    pam_resDataLen->DefaultValue = 0;
    pam_resDataLen->LowerLimit = 0;
    pam_resDataLen->UpperLimit = 255;
    pam_resDataLen->ScalingFactor = 1;
    pam_resDataLen->ScalingOffset = 0;
    pam_resDataLen->LengthBit = 8;
    pam_resDataLen->StartBit = 64 + 8*3;
    pam_resDataLen->ByteOrder = 0;
    pam_resDataLen->Type = 0;
    pam_resDataLen->Name = "udsResDataLen";
    pam_resDataLen->smKey = readSmKey;

    udsReadPamList.append(pam_resDataLen);

    for(int i = 0; i < (64-12); i++)
    {
        PARAM *pam_resData = new PARAM();
        pam_resData->Unit = "";
        pam_resData->DefaultValue = 0;
        pam_resData->LowerLimit = 0;
        pam_resData->UpperLimit = 255;
        pam_resData->ScalingFactor = 1;
        pam_resData->ScalingOffset = 0;
        pam_resData->LengthBit = 8;
        pam_resData->StartBit = 64 + 8*(4+i);
        pam_resData->ByteOrder = 0;
        pam_resData->Type = 0;
        pam_resData->Name = "udsResData[" + QString::number(i) + "]";
        pam_resData->smKey = readSmKey;

        udsReadPamList.append(pam_resData);

    }

    ///////////////////////////////////////////////////////////

    PARAM *timePam_w = new PARAM();
    timePam_w->Unit = "s";
    timePam_w->DefaultValue = 0;
    timePam_w->LowerLimit = DBL_MIN;
    timePam_w->UpperLimit = DBL_MAX;
    timePam_w->ScalingFactor = 1;
    timePam_w->ScalingOffset = 0;
    timePam_w->LengthBit = 64;
    timePam_w->StartBit = 0;
    timePam_w->ByteOrder = 0;
    timePam_w->Type = 3;
    timePam_w->Name = "Time";
    timePam_w->smKey = writeSmKey;

    udsWritePamList.append(timePam_w);

    PARAM *pam_udsActiveReq = new PARAM();
    pam_udsActiveReq->Unit = "";
    pam_udsActiveReq->DefaultValue = 0;
    pam_udsActiveReq->LowerLimit = 0;
    pam_udsActiveReq->UpperLimit = 255;
    pam_udsActiveReq->ScalingFactor = 1;
    pam_udsActiveReq->ScalingOffset = 0;
    pam_udsActiveReq->LengthBit = 8;
    pam_udsActiveReq->StartBit = 64;
    pam_udsActiveReq->ByteOrder = 0;
    pam_udsActiveReq->Type = 0;
    pam_udsActiveReq->Name = "udsActiveReq";
    pam_udsActiveReq->smKey = writeSmKey;

    udsWritePamList.append(pam_udsActiveReq);

    PARAM *pam_udsCmdActiveReq = new PARAM();
    pam_udsCmdActiveReq->Unit = "";
    pam_udsCmdActiveReq->DefaultValue = 0;
    pam_udsCmdActiveReq->LowerLimit = 0;
    pam_udsCmdActiveReq->UpperLimit = 255;
    pam_udsCmdActiveReq->ScalingFactor = 1;
    pam_udsCmdActiveReq->ScalingOffset = 0;
    pam_udsCmdActiveReq->LengthBit = 8;
    pam_udsCmdActiveReq->StartBit = 64 + 8;
    pam_udsCmdActiveReq->ByteOrder = 0;
    pam_udsCmdActiveReq->Type = 0;
    pam_udsCmdActiveReq->Name = "udsCmdActive";
    pam_udsCmdActiveReq->smKey = writeSmKey;

    udsWritePamList.append(pam_udsCmdActiveReq);

    PARAM *pam_udsCmdReq = new PARAM();
    pam_udsCmdReq->Unit = "";
    pam_udsCmdReq->DefaultValue = 0;
    pam_udsCmdReq->LowerLimit = 0;
    pam_udsCmdReq->UpperLimit = 255;
    pam_udsCmdReq->ScalingFactor = 1;
    pam_udsCmdReq->ScalingOffset = 0;
    pam_udsCmdReq->LengthBit = 8;
    pam_udsCmdReq->StartBit = 64 + 8*2;
    pam_udsCmdReq->ByteOrder = 0;
    pam_udsCmdReq->Type = 0;
    pam_udsCmdReq->Name = "udsCmd";
    pam_udsCmdReq->smKey = writeSmKey;

    udsWritePamList.append(pam_udsCmdReq);

    PARAM *pam_reqDataLen = new PARAM();
    pam_reqDataLen->Unit = "";
    pam_reqDataLen->DefaultValue = 0;
    pam_reqDataLen->LowerLimit = 0;
    pam_reqDataLen->UpperLimit = 255;
    pam_reqDataLen->ScalingFactor = 1;
    pam_reqDataLen->ScalingOffset = 0;
    pam_reqDataLen->LengthBit = 8;
    pam_reqDataLen->StartBit = 64 + 8*3;
    pam_reqDataLen->ByteOrder = 0;
    pam_reqDataLen->Type = 0;
    pam_reqDataLen->Name = "udsReqDataLen";
    pam_reqDataLen->smKey = writeSmKey;

    udsWritePamList.append(pam_reqDataLen);

    for(int i = 0; i < (64-12); i++)
    {
        PARAM *pam_reqData = new PARAM();
        pam_reqData->Unit = "";
        pam_reqData->DefaultValue = 0;
        pam_reqData->LowerLimit = 0;
        pam_reqData->UpperLimit = 255;
        pam_reqData->ScalingFactor = 1;
        pam_reqData->ScalingOffset = 0;
        pam_reqData->LengthBit = 8;
        pam_reqData->StartBit = 64 + 8*(4+i);
        pam_reqData->ByteOrder = 0;
        pam_reqData->Type = 0;
        pam_reqData->Name = "udsReqData[" + QString::number(i) + "]";
        pam_reqData->smKey = writeSmKey;

        udsWritePamList.append(pam_reqData);

    }

}

bool Uds_Fun_Thread::createPamXml()
{
    QString appPath = QCoreApplication::applicationDirPath();

    QString FileName = "UDS_COMM_" + fun->funName + ".xml";
    QString xmlPath = appPath + "/pamFactory/" + FileName;
    qDebug()<<"UDS pam file path:"<<xmlPath;

    if (xmlPath.isEmpty())
        return false;

    QFile xmlFile(xmlPath);
    if (!xmlFile.open(QFile::WriteOnly | QFile::Text))
    {
        qDebug()<<tr("Cannot write file %1:\n%2.").arg(xmlPath).arg(xmlFile.errorString());
        return false;
    }

    QDomDocument xml("UDS_" + fun->funName);

    QDomElement root = xml.createElement("Root");

    xml.appendChild(root);

    QDomElement ReadSigElem = xml.createElement("RP_UDS_" + fun->funName);
    root.appendChild(ReadSigElem);

    // add time channel
    QDomElement timeElem_r = xml.createElement("param_"+QString::number(0));
    ReadSigElem.appendChild(timeElem_r);
    timeElem_r.setAttribute("unit", "s");
    timeElem_r.setAttribute("value_default", QString::number(0,10));
    timeElem_r.setAttribute("minimum", QString::number(DBL_MIN, 'g', 6));
    timeElem_r.setAttribute("maximum", QString::number(DBL_MAX, 'g', 6));
    timeElem_r.setAttribute("scalingoffset", QString::number(0));
    timeElem_r.setAttribute("scalingfactor", QString::number(1));
    timeElem_r.setAttribute("lengthbit", QString::number(64, 10));
    timeElem_r.setAttribute("startbit", QString::number(0, 10));
    timeElem_r.setAttribute("StartBit_Oth", QString::number(0, 10));
    timeElem_r.setAttribute("byteorder", QString::number(0,10));
    timeElem_r.setAttribute("type", QString::number(3, 10));
    timeElem_r.setAttribute("param_name", "Time");


    QDomElement sigElem1 = xml.createElement("param_"+QString::number(1));
    ReadSigElem.appendChild(sigElem1);
    sigElem1.setAttribute("unit", "");
    sigElem1.setAttribute("value_default", "0");
    sigElem1.setAttribute("minimum", QString::number(0, 10));
    sigElem1.setAttribute("maximum", QString::number(255, 10));
    sigElem1.setAttribute("scalingoffset", "0");
    sigElem1.setAttribute("scalingfactor", "1");
    sigElem1.setAttribute("lengthbit", "8");
    sigElem1.setAttribute("startbit", QString::number(64, 10));
    sigElem1.setAttribute("byteorder", QString::number(0,10));
    sigElem1.setAttribute("type", QString::number(0, 10));
    sigElem1.setAttribute("param_name", "udsActiveAct");

    QDomElement sigElem2 = xml.createElement("param_"+QString::number(2));
    ReadSigElem.appendChild(sigElem2);
    sigElem2.setAttribute("unit", "");
    sigElem2.setAttribute("value_default", "0");
    sigElem2.setAttribute("minimum", QString::number(0, 10));
    sigElem2.setAttribute("maximum", QString::number(255, 10));
    sigElem2.setAttribute("scalingoffset", "0");
    sigElem2.setAttribute("scalingfactor", "1");
    sigElem2.setAttribute("lengthbit", "8");
    sigElem2.setAttribute("startbit", QString::number(64 + 8, 10));
    sigElem2.setAttribute("byteorder", QString::number(0,10));
    sigElem2.setAttribute("type", QString::number(0, 10));
    sigElem2.setAttribute("param_name", "udsCmdActive");

    QDomElement sigElem3 = xml.createElement("param_"+QString::number(3));
    ReadSigElem.appendChild(sigElem3);
    sigElem3.setAttribute("unit", "");
    sigElem3.setAttribute("value_default", "0");
    sigElem3.setAttribute("minimum", QString::number(0, 10));
    sigElem3.setAttribute("maximum", QString::number(255, 10));
    sigElem3.setAttribute("scalingoffset", "0");
    sigElem3.setAttribute("scalingfactor", "1");
    sigElem3.setAttribute("lengthbit", "8");
    sigElem3.setAttribute("startbit", QString::number(80, 10));
    sigElem3.setAttribute("byteorder", QString::number(0,10));
    sigElem3.setAttribute("type", QString::number(0, 10));
    sigElem3.setAttribute("param_name", "udsCmdAct");

    QDomElement sigElem4 = xml.createElement("param_"+QString::number(4));
    ReadSigElem.appendChild(sigElem4);
    sigElem4.setAttribute("unit", "");
    sigElem4.setAttribute("value_default", "0");
    sigElem4.setAttribute("minimum", QString::number(0, 10));
    sigElem4.setAttribute("maximum", QString::number(255, 10));
    sigElem4.setAttribute("scalingoffset", "0");
    sigElem4.setAttribute("scalingfactor", "1");
    sigElem4.setAttribute("lengthbit", "8");
    sigElem4.setAttribute("startbit", QString::number(88, 10));
    sigElem4.setAttribute("byteorder", QString::number(0,10));
    sigElem4.setAttribute("type", QString::number(0, 10));
    sigElem4.setAttribute("param_name", "udsResDataLen");

    for(int i = 0; i < (64-12); i++)
    {
        QDomElement sigElem5 = xml.createElement("param_"+QString::number(4 + i));
        ReadSigElem.appendChild(sigElem5);
        sigElem5.setAttribute("unit", "");
        sigElem5.setAttribute("value_default", "0");
        sigElem5.setAttribute("minimum", QString::number(0, 10));
        sigElem5.setAttribute("maximum", QString::number(255, 10));
        sigElem5.setAttribute("scalingoffset", "0");
        sigElem5.setAttribute("scalingfactor", "1");
        sigElem5.setAttribute("lengthbit", "8");
        sigElem5.setAttribute("startbit", QString::number(96 + 8*i, 10));
        sigElem5.setAttribute("byteorder", QString::number(0,10));
        sigElem5.setAttribute("type", QString::number(0, 10));
        sigElem5.setAttribute("param_name", "udsResData[" + QString::number(i) + "]");
    }


    ReadSigElem.setAttribute("size_bytes", 64);
    ReadSigElem.setAttribute("sample_rate", QString::number(10,10));
    ReadSigElem.setAttribute("is_read", QString::number(1,10));


    QDomElement WriteSigElem = xml.createElement("WP_UDS_" + fun->funName);
    root.appendChild(WriteSigElem);

    QDomElement timeElem_w = xml.createElement("param_"+QString::number(0));
    WriteSigElem.appendChild(timeElem_w);
    timeElem_w.setAttribute("unit", "s");
    timeElem_w.setAttribute("value_default", QString::number(0, 10));
    timeElem_w.setAttribute("minimum", QString::number(-DBL_MAX, 'g', 6));
    timeElem_w.setAttribute("maximum", QString::number(DBL_MAX, 'g', 6));
    timeElem_w.setAttribute("scalingoffset", QString::number(0));
    timeElem_w.setAttribute("scalingfactor", QString::number(1));
    timeElem_w.setAttribute("lengthbit", QString::number(64, 10));
    timeElem_w.setAttribute("startbit", QString::number(0, 10));
    timeElem_w.setAttribute("byteorder", QString::number(0,10));
    timeElem_w.setAttribute("type", QString::number(3, 10));
    timeElem_w.setAttribute("param_name", "Time");


    QDomElement sigElem1_w = xml.createElement("param_"+QString::number(1));
    WriteSigElem.appendChild(sigElem1_w);
    sigElem1_w.setAttribute("unit", "");
    sigElem1_w.setAttribute("value_default", QString::number(0));
    sigElem1_w.setAttribute("minimum", QString::number(0, 10));
    sigElem1_w.setAttribute("maximum", QString::number(255, 10));
    sigElem1_w.setAttribute("scalingoffset", QString::number(0));
    sigElem1_w.setAttribute("scalingfactor", QString::number(1));
    sigElem1_w.setAttribute("lengthbit", QString::number(8, 10));
    sigElem1_w.setAttribute("startbit", QString::number(64, 10));
    sigElem1_w.setAttribute("StartBit_Oth", QString::number(0, 10));
    sigElem1_w.setAttribute("byteorder", QString::number(0,10));
    sigElem1_w.setAttribute("type", QString::number(0, 10));
    sigElem1_w.setAttribute("param_name", "udsActive");

    QDomElement sigElem2_w = xml.createElement("param_"+QString::number(2));
    WriteSigElem.appendChild(sigElem2_w);
    sigElem2_w.setAttribute("unit", "");
    sigElem2_w.setAttribute("value_default", QString::number(0));
    sigElem2_w.setAttribute("minimum", QString::number(0, 10));
    sigElem2_w.setAttribute("maximum", QString::number(255, 10));
    sigElem2_w.setAttribute("scalingoffset", QString::number(0));
    sigElem2_w.setAttribute("scalingfactor", QString::number(1));
    sigElem2_w.setAttribute("lengthbit", QString::number(8, 10));
    sigElem2_w.setAttribute("startbit", QString::number(72, 10));
    sigElem2_w.setAttribute("StartBit_Oth", QString::number(72, 10));
    sigElem2_w.setAttribute("byteorder", QString::number(0,10));
    sigElem2_w.setAttribute("type", QString::number(0, 10));
    sigElem2_w.setAttribute("param_name", "udsCmdActive");

    QDomElement sigElem3_w = xml.createElement("param_"+QString::number(3));
    WriteSigElem.appendChild(sigElem3_w);
    sigElem3_w.setAttribute("unit", "");
    sigElem3_w.setAttribute("value_default", QString::number(0));
    sigElem3_w.setAttribute("minimum", QString::number(0, 10));
    sigElem3_w.setAttribute("maximum", QString::number(255, 10));
    sigElem3_w.setAttribute("scalingoffset", QString::number(0));
    sigElem3_w.setAttribute("scalingfactor", QString::number(1));
    sigElem3_w.setAttribute("lengthbit", QString::number(8, 10));
    sigElem3_w.setAttribute("startbit", QString::number(80, 10));
    sigElem3_w.setAttribute("StartBit_Oth", QString::number(80, 10));
    sigElem3_w.setAttribute("byteorder", QString::number(0,10));
    sigElem3_w.setAttribute("type", QString::number(0, 10));
    sigElem3_w.setAttribute("param_name", "udsCmd");

    QDomElement sigElem4_w = xml.createElement("param_"+QString::number(4));
    WriteSigElem.appendChild(sigElem4_w);
    sigElem4_w.setAttribute("unit", "");
    sigElem4_w.setAttribute("value_default", QString::number(0));
    sigElem4_w.setAttribute("minimum", QString::number(0, 10));
    sigElem4_w.setAttribute("maximum", QString::number(255, 10));
    sigElem4_w.setAttribute("scalingoffset", QString::number(0));
    sigElem4_w.setAttribute("scalingfactor", QString::number(1));
    sigElem4_w.setAttribute("lengthbit", QString::number(8, 10));
    sigElem4_w.setAttribute("startbit", QString::number(88, 10));
    sigElem4_w.setAttribute("StartBit_Oth", QString::number(88, 10));
    sigElem4_w.setAttribute("byteorder", QString::number(0,10));
    sigElem4_w.setAttribute("type", QString::number(0, 10));
    sigElem4_w.setAttribute("param_name", "udsReqDataLen");

    for(int i = 0; i < (64-12); i++)
    {
        QDomElement sigElem5_w = xml.createElement("param_"+QString::number(4 + i));
        WriteSigElem.appendChild(sigElem5_w);
        sigElem5_w.setAttribute("unit", "");
        sigElem5_w.setAttribute("value_default", QString::number(0));
        sigElem5_w.setAttribute("minimum", QString::number(0, 10));
        sigElem5_w.setAttribute("maximum", QString::number(255, 10));
        sigElem5_w.setAttribute("scalingoffset", QString::number(0));
        sigElem5_w.setAttribute("scalingfactor", QString::number(1));
        sigElem5_w.setAttribute("lengthbit", QString::number(8, 10));
        sigElem5_w.setAttribute("startbit", QString::number(96 + 8*i, 10));
        sigElem5_w.setAttribute("StartBit_Oth", QString::number(96 + 8*i, 10));
        sigElem5_w.setAttribute("byteorder", QString::number(0,10));
        sigElem5_w.setAttribute("type", QString::number(0, 10));
        sigElem5_w.setAttribute("param_name", "udsReqData[" + QString::number(i) + "]");
    }


    WriteSigElem.setAttribute("size_bytes", 64);
    WriteSigElem.setAttribute("sample_rate", QString::number(10,10));
    WriteSigElem.setAttribute("is_read", QString::number(0,10));


    QString xmlString = xml.toString();
    QTextStream out(&xmlFile);
    out << xmlString;

    xmlFile.close();

    return true;
}

void Uds_Fun_Thread::init()
{
    if(!udsWorker)
    {
        udsWorker = new NI_UDS_Worker();
    }

    udsWorker->setUdsInfo(udsInfo);
}

bool Uds_Fun_Thread::startUDS()
{
    if(udsWorker->uds_run())
    {
        qDebug()<<"[UDS] uds start ok!";
        udsActiveAct = 0x01;
        return true;
    }
    else
    {
        qDebug()<<"[UDS] uds start error.";
        udsActiveAct = 0x00;
        return false;
    }
}

bool Uds_Fun_Thread::stopUDS()
{
    if(udsWorker->uds_exit())
    {
        qDebug()<<"[UDS] uds stop ok!";
        udsActiveAct = 0x00;
        return true;
    }
    else
    {
        qDebug()<<"[UDS] uds stop error.";
        udsActiveAct = 0x00;
        return false;
    }
}

void Uds_Fun_Thread::workUDS()
{
    if(udsCmdActive == 0) return;

    qreal timeReq = time.elapsed()/1000;
    qDebug()<<QString("[UDS %1] Len %2 ReqData: %3 %4 %5 %6 %7 %8 %9 %10 %11")
              .arg(timeReq, 6, 'f', 2, QLatin1Char(' '))
              .arg(udsReqDataLen, 2, 10, QLatin1Char('0'))
              .arg(udsCmd, 2, 16, QLatin1Char('0'))
              .arg(udsReqData[0], 2, 16, QLatin1Char('0'))
              .arg(udsReqData[1], 2, 16, QLatin1Char('0'))
              .arg(udsReqData[2], 2, 16, QLatin1Char('0'))
              .arg(udsReqData[3], 2, 16, QLatin1Char('0'))
              .arg(udsReqData[4], 2, 16, QLatin1Char('0'))
              .arg(udsReqData[5], 2, 16, QLatin1Char('0'))
              .arg(udsReqData[6], 2, 16, QLatin1Char('0'))
              .arg(udsReqData[7], 2, 16, QLatin1Char('0')).toUpper();

    quint8 cmd = udsCmd;
    bool cmdExeSucceed = false;

    switch (cmd) {
    case 0x10:
    {
        quint8 sessionMode = 0x00;
        sessionMode = udsReqData[0];
        cmdExeSucceed = udsWorker->uds_manl_diagSessionControl(sessionMode);
        break;
    }
    case 0x11:
    {
        quint8 resetType = 0x00;
        resetType = udsReqData[0];

        break;
    }
    case 0x14:
    {
        quint32 groupOfDTC = 0xFFFFFF;
        cmdExeSucceed = udsWorker->uds_manl_clearDiagInfo(groupOfDTC);
        break;
    }
    case 0x19:
    {
        quint32 groupOfDTC = 0xFFFFFF;
        quint8 mask = 0x01;  // mode:01=当前故障 / 08=历史故障 / 09=两者
        mask = udsReqData[1];
        quint32 dtcLen = 0;
        quint32 *dtcBuf = (quint32*)udsResData;
        cmdExeSucceed = udsWorker->uds_manl_readDiagInfo(mask, groupOfDTC, &dtcLen, dtcBuf);
        udsResDataLen = dtcLen;

        break;
    }
    case 0x22:
    {
        quint16 did = 0x0000;
        did = *(quint16*)(udsReqData);
        qDebug()<<"[UDS] 0x22 Active, DID(0x):"<<QString::number(did, 16).toUpper();

        if(udsInfo.byteOrder == 1)
        {
            reverseBytes((quint8*)&did, 2);
        }

        quint8 *dataOut = udsResData;
        qint32 lenOut = 20;
        cmdExeSucceed = udsWorker->uds_manl_readDataByIdentifier(did, dataOut, &lenOut);
        udsResDataLen = lenOut;

        break;
    }
    case 0x27:
    {
        quint8 mode = 0x00;
        mode = udsReqData[0];
        quint8 *seed = udsReqData + 1;
        quint16 lenOfSeed = 4;

        quint8 *key = udsResData;
        quint16 lenOfKey = 4;
        cmdExeSucceed = udsWorker->uds_manl_saftyPass(mode, seed, lenOfSeed, key, lenOfKey);

        break;
    }
    case 0x2E:
    {
        quint16 did = 0x0000;
        did = *(quint16*)(udsReqData);

        if(udsInfo.byteOrder == 1)
        {
            reverseBytes((quint8*)&did, 2);
        }

        quint8 *dataIn = udsReqData + 2;
        quint8 lenIn = udsReqDataLen - 3;

        cmdExeSucceed = udsWorker->uds_manl_writeDataByIdentifier(did, dataIn, lenIn);
        break;
    }
    case 0x2F:
    {
        quint16 did = 0x0000;
        quint8 mode = 0x00;
        did = *(quint16*)(udsReqData);
        mode = udsReqData[2];
        qDebug()<<"[UDS] 0x2F Active, DID(0x):"<<QString::number(did, 16).toUpper();

        if(udsInfo.byteOrder == 1)
        {
            reverseBytes((quint8*)&did, 2);
        }

        quint8 *dataIn = udsReqData + 3;
        quint32 lenIn = udsReqDataLen - 3;

        if(mode == 0x02 || mode == 0x01)
        {
            lenIn = 0;
        }

        quint8 *dataOut = udsResData;
        quint32 lenOut = 4;
        cmdExeSucceed = udsWorker->uds_manl_ioControlByIdentifier(did, mode, dataIn, lenIn, dataOut, lenOut);

        if(udsInfo.autoFreeze)
        {
            if(udsInfo.timeBetweenCtrlAndFreeze != 0)
            {
                waitMs(udsInfo.timeBetweenCtrlAndFreeze);
            }
            cmdExeSucceed = udsWorker->uds_manl_ioControlByIdentifier(did, 0x02, dataIn, 0, dataOut, lenOut);
        }

        break;
    }
    case 0x31:
    {
        quint8 mode = 0x00;
        mode = udsReqData[0];
        quint16 did = 0x0000;
        did = *(quint16*)(udsReqData + 1);

        if(udsInfo.byteOrder == 1)
        {
            reverseBytes((quint8*)&did, 2);
        }

        quint8 *dataIn = udsReqData + 3;
        quint32 lenIn = udsReqDataLen - 3;

        if(mode == 0x01)
        {
            //lenIn = 1;
            //dataIn[0] = 0x01;
        }
        else if(mode == 0x02)
        {
            //lenIn = 1;
            //dataIn[0] = 0x00;
        }
        else if(mode == 0x03)
        {
            //lenIn = 0;
        }

        quint8 *dataOut = udsResData;
        quint32 lenOut = 4;

        cmdExeSucceed = udsWorker->uds_manl_routineControl(did, mode, dataIn, lenIn, dataOut, lenOut);

        break;
    }
    case 0x3E:
    {
        quint8 mode = 0x00;
        mode = udsReqData[0];
        bool responseRequired = true;
        if(mode == 0x80) responseRequired = false;

        cmdExeSucceed = udsWorker->uds_manl_testPresented(responseRequired);
        break;
    }
    case 0x00:
    {
        //send raw data by diagostic
        quint8 *dataIn = udsReqData;
        qint32 lenIn = udsReqDataLen;
        quint8 *dataOut = udsResData;
        qint32 lenOut = 0;
        qDebug()<<"0x00, Diag raw frame send dataLen: "<<lenIn<<". data[0]"<<dataIn[0];
        cmdExeSucceed = udsWorker->uds_manl_diagFrameSendAndRcv(dataIn, lenIn, dataOut, &lenOut);
        qDebug()<<"0x00, Diag raw frame recv dataLen: "<<lenOut;
        udsResDataLen = lenOut;

        break;
    }
    default:
        break;
    }

    qDebug()<<"[UDS] Exe Status:"<<cmdExeSucceed;

    qreal timeRes = time.elapsed() / 1000;
    qDebug()<<QString("[UDS %1] Len %2 ResData: %3 %4 %5 %6 %7 %8 %9 %10 %11")
              .arg(timeRes, 6, 'f', 2, QLatin1Char(' '))
              .arg(udsResDataLen, 2, 10, QLatin1Char('0'))
              .arg(udsCmd, 2, 16, QLatin1Char('0'))
              .arg(udsResData[0], 2, 16, QLatin1Char('0'))
              .arg(udsResData[1], 2, 16, QLatin1Char('0'))
              .arg(udsResData[2], 2, 16, QLatin1Char('0'))
              .arg(udsResData[3], 2, 16, QLatin1Char('0'))
              .arg(udsResData[4], 2, 16, QLatin1Char('0'))
              .arg(udsResData[5], 2, 16, QLatin1Char('0'))
              .arg(udsResData[6], 2, 16, QLatin1Char('0'))
              .arg(udsResData[7], 2, 16, QLatin1Char('0')).toUpper();


    udsCmdActive = 0x00;
}

void Uds_Fun_Thread::fromWritePamToReqPams()
{
    udsActiveReq = udsWritePamList.at(1)->getValue();

    quint8 cmdTrigger = udsWritePamList.at(2)->getValue();
    udsCmd = udsWritePamList.at(3)->getValue();
    udsReqDataLen = udsWritePamList.at(4)->getValue();

    for(int i = 0; i < (64-12); i++)
    {
        udsReqData[i] = udsReadPamList.at(5+i)->getValue();
    }

}

void Uds_Fun_Thread::packWritePamList(QSharedMemory *writeSm, int writeSmSize)
{
    quint8 buf[writeSmSize];

    qreal *time = (qreal*)&buf;

    *time = udsWritePamList.at(0)->getValue();

    for(int i = 0; i < (64-8); i++)
    {
        buf[8 + i] = udsReadPamList.at(1+i)->getValue();
    }

    copyToSharedMemory(writeSm, 0, (char*)buf, writeSmSize);
}

void Uds_Fun_Thread::fromWriteSmToReqPams(QSharedMemory *writeSm, int writeSmSize)
{
    quint8 buf[writeSmSize];
    if(!copyFromSharedMemory(writeSm, 0, (char*)buf, writeSmSize))
        return;

    //qreal timeStamp = *(qreal*)buf;

    udsActiveReq = buf[8];

    quint8 cmdTrigger = buf[9];
    udsCmd = buf[10];
    udsReqDataLen = buf[11];

    memcpy(udsReqData, buf + 12, 8);

    if(udsCmdActive == 0 && cmdTrigger > 0)
    {
        udsCmdActive = 0x01;
        qDebug()<<"cmd active: 0x"<<QString::number(udsCmd, 16).toUpper();

    }
}

void Uds_Fun_Thread::updateUdsCmdActiveInSm(QSharedMemory *writeSm, quint8 value)
{
    copyToSharedMemory(writeSm, 9, (char*)&value, 1);
}

void Uds_Fun_Thread::packResPamsToReadSm(QSharedMemory *readSm, int readSmSize)
{
    quint8 buf[readSmSize];

    qreal absTime = (qreal)time.elapsed() / 1000.0;
    *(qreal*)buf = absTime;

    buf[8] = udsActiveAct;
    buf[9] = udsCmdActive;
    buf[10] = udsCmd;
    buf[11] = udsResDataLen;

    memcpy(buf + 12, udsResData, 50);

    copyToSharedMemory(readSm, 0, (char*)buf, readSmSize);

    updateReadPamList();
}

void Uds_Fun_Thread::updateReadPamList()
{

    qreal absTime = (qreal)time.elapsed() / 1000.0;
    udsReadPamList.at(0)->setValue(absTime);
    udsReadPamList.at(1)->setValue(udsActiveAct);
    udsReadPamList.at(2)->setValue(udsCmdActive);
    udsReadPamList.at(3)->setValue(udsCmd);
    udsReadPamList.at(4)->setValue(udsResDataLen);

    for(int i = 0; i < (64-12); i++)
    {
        udsReadPamList.at(4+i)->setValue(udsResData[i]);
    }
}

void Uds_Fun_Thread::createSM()
{

}

void Uds_Fun_Thread::deleteSM()
{

}

void Uds_Fun_Thread::reverseBytes(quint8 *data, int len)
{
    if(!data) return;
    for(int i = 0; i < len/2; i++)
    {
        quint8 temp = data[i];
        data[i] = data[len-1-i];
        data[len-1-i] = temp;
    }
}

void Uds_Fun_Thread::waitMs(quint32 mSec)
{
    QEventLoop loop;//定义一个新的事件循环
    QTimer::singleShot(mSec, &loop, SLOT(quit()));//创建单次定时器，槽函数为事件循环的退出函数
    loop.exec();//事件循环开始执行，程序会卡在这里，直到定时时间到，本循环被退出
}

QList<PARAM *> Uds_Fun_Thread::getUdsWritePamList() const
{
    return udsWritePamList;
}

QList<PARAM *> Uds_Fun_Thread::getUdsReadPamList() const
{
    return udsReadPamList;
}

void Uds_Fun_Thread::setUdsRun(bool value)
{
    udsRun = value;
}

void Uds_Fun_Thread::setIsStop(bool value)
{
    isStop = value;
}

bool Uds_Fun_Thread::getUdsStartFinished() const
{
    return udsStartFinished;
}
