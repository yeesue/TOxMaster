#include "wt5000_thread.h"
#include "vc/tmctl.h"
#include <QDebug>
#include <QDateTime>

WT5000_Thread::WT5000_Thread()
{

}

WT5000_Thread::~WT5000_Thread()
{
    emit wtFunStateChanged(0, funIndex);
    wtState = 0;
    endHandle();
}

bool WT5000_Thread::connectDevice()
{
    QString IPConfig=IPAddress;
    QByteArray temp=IPConfig.toLatin1();
    sta=TmcInitialize(TM_CTL_VXI11,temp.data(),&deviceID);
    if(sta!=0)
    {
        qInfo()<<Q_FUNC_INFO<<"WT5000 connection error.";
        return false;
    }
    sta=TmcSetTerm(deviceID,2,1);
    //Sets the terminator. The default value after initialization is “eos = 2, eot = 1.”
    if(sta!=0)
    {
        TmcFinish(deviceID);
        qInfo()<<Q_FUNC_INFO<<"WT5000 set term error.";
        return false;
    }
    sta=TmcSetTimeout(deviceID,3);
    //Sets the communication timeout value The default value after initialization is 30 s.
    if(sta!=0)
    {
        TmcFinish(deviceID);
        qInfo()<<Q_FUNC_INFO<<"WT5000 set time out error.";
        return false;
    }

//    sta=TmcSend(deviceID,(char*)"*RST");
//    if(sta!=0)
//    {
//        qInfo()<<Q_FUNC_INFO<<"Send RST error";
//        TmcFinish(deviceID);
//        return false;
//    }

    sta=TmcSend(deviceID,(char*)"*IDN?");
    if(sta!=0)
    {
        qInfo()<<Q_FUNC_INFO<<"Send IDN error";
        TmcFinish(deviceID);
        return false;
    }

    char buff[65535];
    int length;
    sta=TmcReceive(deviceID,buff,65535,&length);
    if(sta!=0)
    {
        qInfo()<<Q_FUNC_INFO<<"Recvive error.";
        stopFlg=true;
        TmcFinish(deviceID);
        return false;
    }
    //Sets remote mode.
    sta=TmcSetRen(deviceID,1);
    if(sta!=0)
    {
        TmcFinish(deviceID);
        qInfo()<<Q_FUNC_INFO<<"WT5000 set remote mode error.";
        return false;
    }

    stopFlg=false;
    start();

    return true;

}

bool WT5000_Thread::disconnectDevice()
{
    qInfo()<<"WT5000 设备断开连接";
    emit wtFunStateChanged(0, funIndex);
    wtState = 0;

    if(stopFlg) return true;

    stopFlg=true;
    wait(1000);
    //Sets local mode.
    sta=TmcSetRen(deviceID,0);
    if(sta!=0)
    {
        TmcFinish(deviceID);
        qInfo()<<Q_FUNC_INFO<<"WT5000 set remote mode error.";
        return false;
    }
    sta=TmcSend(deviceID,(char*)":COMM:OVER 0");
    if(sta!=0)
    {
        qInfo()<<Q_FUNC_INFO<<"Send COMM OVER error.";
        return false;
    }
    sta=TmcSend(deviceID,(char*)":COMM:HEAD ON");
    if(sta!=0)
    {
        qInfo()<<Q_FUNC_INFO<<"Send COMM HEAD ON error.";
        return false;
    }

    sta=TmcSend(deviceID,(char*)":COMM:VERB OFF");
    if(sta!=0)
    {
        qInfo()<<Q_FUNC_INFO<<"Send COMM VERB OFF error.";
        return false;
    }

    sta=TmcFinish(deviceID);
    if(sta!=0)
    {
        qInfo()<<Q_FUNC_INFO<<"WT5000 disconnect error.";
        return false;
    }
    return true;
}

void WT5000_Thread::setIPAddress(QString address)
{
    IPAddress=address;
}

void WT5000_Thread::setFunInfo(FunStruct *value, int index)
{
    fun = value;
    funIndex = index;
}

void WT5000_Thread::init()
{
    readSetting();

    createSM();

    createPams();

    emit wtFunStateChanged(0, funIndex);
    wtState = 0;

    qInfo()<<"WT5000 初始化";

    if(!connectDevice())
    {
        qDebug()<<"WT5000 连接失败";

        emit wtFunStateChanged(-1, funIndex);

        return;
    }

    connect(this, SIGNAL(wtDataUpdated()), this, SLOT(wtDataUpdatedSlot()));

    emit wtFunStateChanged(1, funIndex);
    qInfo()<<"WT5000 设备连接成功";
    wtState = 1;
}

void WT5000_Thread::endHandle()
{
    disconnect(this, SIGNAL(wtDataUpdated()), this, SLOT(wtDataUpdatedSlot()));

    deleteSM();
}

void WT5000_Thread::createSM()
{
    smKey = "RP_WT5000";

    if(!sm_wt5000)
    {
        sm_wt5000 = new QSharedMemory(smKey);
    }
    sm_wt5000->create(200);

}

void WT5000_Thread::deleteSM()
{
    if(sm_wt5000)
    {
        sm_wt5000->detach();

        delete sm_wt5000;
        sm_wt5000 = NULL;
    }
}

void WT5000_Thread::writeSetting()
{

}

void WT5000_Thread::readSetting()
{
    //QString appPath = QCoreApplication::applicationDirPath();
    //QString setFilePath = appPath + "/setting/wt5000_setting.ini";

    //QSettings setting(setFilePath, QSettings::IniFormat);

    if(fun->setFilePath.isEmpty() || fun->intfName.isEmpty())
        return;

    QString setFilePath = QCoreApplication::applicationDirPath() + fun->setFilePath;
    qDebug()<<"In wt5000 fun thread: read setting: "<<setFilePath;
    QSettings setting(setFilePath, QSettings::IniFormat);

    IPAddress = setting.value("IP_Address").toString();
}

void WT5000_Thread::createPams()
{
    pamNameList << "I_1" << "U_1" << "I_2" << "U_2" << "I_3" << "U_3"
                << "P_1" << "Q_1" << "P_2" << "Q_2" << "P_3" << "Q_3"
                << "ETA_1" << "ETA_2" << "ETA_3" << "P_Sigma" << "I_4" << "U_4"
                << "P_4" << "Q_4" << "S_1" << "S_2" << "S_3" << "S_4"
                << "PF_1" << "PF_2" << "PF_3" << "PF_4" << "PHI_1" << "PHI_2"
                << "PHI_3" << "PHI_4" << "SPE" << "TORQ" << "S_4" << "S_4"
                << "S_4" << "S_4" << "S_4" << "S_4" << "S_4" << "S_4"
                << "S_4" << "S_4" << "S_4" << "S_4" << "S_4" << "S_4";

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
    timePam->smKey = smKey;
    timePam->smSize = 200;

    readPamList.append(timePam);

    for(int i = 0; i < pamNameList.count(); i++)
    {
        QString pamName = pamNameList.at(i);

        PARAM *pam = new PARAM();

        pam->Name = pamName;
        pam->ShortName = pamName;
        pam->LongIdentifier = pamName;

        pam->ScalingFactor = 1;
        pam->ScalingOffset = 0;

        pam->Type = 11;  // 11 = float(moto)
        pam->StartBit = 64 + 32 * i;
        pam->LengthBit = 32;
        pam->ByteOrder = 1;  // 1 = moto

        pam->StartByte = 8 + 4 * i;

        pam->smKey = smKey;
        pam->smSize = 200;

        readPamList.append(pam);
    }

}

QString WT5000_Thread::getSmKey() const
{
    return smKey;
}

QList<PARAM *> WT5000_Thread::getReadPamList() const
{
    return readPamList;
}

int WT5000_Thread::getWtState() const
{
    return wtState;
}

void WT5000_Thread::setStopFlg(bool value)
{
    stopFlg = value;
}

void WT5000_Thread::run()
{

    init();

    char buff[65535];
    int length;
    uint currentTime;
    qint64 currentTime_ms;
    char sigBuff[8];


    sta=TmcSend(deviceID,(char*)":INPUT:MODULE?");
    if(sta!=0)
    {
        qInfo()<<Q_FUNC_INFO<<"Send input module error";
        stopFlg=true;
    }
    sta=TmcReceive(deviceID,buff,65535,&length);
    if(sta!=0)
    {
        qInfo()<<Q_FUNC_INFO<<"Receive error";
        stopFlg=true;
    }

    sta=TmcSend(deviceID,(char*)":NUM:FORM FLO");
    if(sta!=0)
    {
        qInfo()<<Q_FUNC_INFO<<"Send format require error.";
        stopFlg=true;
    }
    sta=TmcSend(deviceID,(char*)":NUM:NUM 48");
    if(sta!=0)
    {
        qInfo()<<Q_FUNC_INFO<<"Send numbers require error.";
        stopFlg=true;
    }

    sta=TmcSend(deviceID,(char*)":NUM:NORM:ITEM1 IRMS,1,TOT");
    if(sta!=0)
    {
      qInfo()<<Q_FUNC_INFO<<"Send item1 require error.";
      stopFlg=true;
      TmcFinish(deviceID);
      return;
    }
    sta=TmcSend(deviceID,(char*)":NUM:NORM:ITEM2 URMS,1,TOT");
    if(sta!=0)
    {
      qInfo()<<Q_FUNC_INFO<<"Send item2 require error.";
      stopFlg=true;
      TmcFinish(deviceID);
      return;
    }
    sta=TmcSend(deviceID,(char*)":NUM:NORM:ITEM3 IRMS,2,TOT");
    if(sta!=0)
    {
      qInfo()<<Q_FUNC_INFO<<"Send item3 require error.";
      stopFlg=true;
      TmcFinish(deviceID);
      return;
    }
    sta=TmcSend(deviceID,(char*)":NUM:NORM:ITEM4 URMS,2,TOT");
    if(sta!=0)
    {
      qInfo()<<Q_FUNC_INFO<<"Send item4 require error.";
      stopFlg=true;
      TmcFinish(deviceID);
      return;
    }
    sta=TmcSend(deviceID,(char*)":NUM:NORM:ITEM5 IRMS,3,TOT");
    if(sta!=0)
    {
      qInfo()<<Q_FUNC_INFO<<"Send item5 require error.";
      stopFlg=true;
      TmcFinish(deviceID);
      return;
    }
    sta=TmcSend(deviceID,(char*)":NUM:NORM:ITEM6 URMS,3,TOT");
    if(sta!=0)
    {
      qInfo()<<Q_FUNC_INFO<<"Send item6 require error.";
      stopFlg=true;
      TmcFinish(deviceID);
      return;
    }
    sta=TmcSend(deviceID,(char*)":NUM:NORM:ITEM7 P,1,TOT");
    if(sta!=0)
    {
      qInfo()<<Q_FUNC_INFO<<"Send item7 require error.";
      stopFlg=true;
      TmcFinish(deviceID);
      return;
    }
    sta=TmcSend(deviceID,(char*)":NUM:NORM:ITEM8 Q,1,TOT");
    if(sta!=0)
    {
      qInfo()<<Q_FUNC_INFO<<"Send item8 require error.";
      stopFlg=true;
      TmcFinish(deviceID);
      return;
    }
    sta=TmcSend(deviceID,(char*)":NUM:NORM:ITEM9 P,2,TOT");
    if(sta!=0)
    {
      qInfo()<<Q_FUNC_INFO<<"Send item9 require error.";
      stopFlg=true;
      TmcFinish(deviceID);
      return;
    }
    sta=TmcSend(deviceID,(char*)":NUM:NORM:ITEM10 Q,2,TOT");
    if(sta!=0)
    {
      qInfo()<<Q_FUNC_INFO<<"Send item10 require error.";
      stopFlg=true;
      TmcFinish(deviceID);
      return;
    }
    sta=TmcSend(deviceID,(char*)":NUM:NORM:ITEM11 P,3,TOT");
    if(sta!=0)
    {
      qInfo()<<Q_FUNC_INFO<<"Send item11 require error.";
      stopFlg=true;
      TmcFinish(deviceID);
      return;
    }
    sta=TmcSend(deviceID,(char*)":NUM:NORM:ITEM12 Q,3,TOT");
    if(sta!=0)
    {
      qInfo()<<Q_FUNC_INFO<<"Send item12 require error.";
      stopFlg=true;
      TmcFinish(deviceID);
      return;
    }
    sta=TmcSend(deviceID,(char*)":NUM:NORM:ITEM13 ETA1,1,TOT");
    if(sta!=0)
    {
      qInfo()<<Q_FUNC_INFO<<"Send item13 require error.";
      stopFlg=true;
      TmcFinish(deviceID);
      return;
    }
    sta=TmcSend(deviceID,(char*)":NUM:NORM:ITEM14 ETA2,1,TOT");
    if(sta!=0)
    {
      qInfo()<<Q_FUNC_INFO<<"Send item14 require error.";
      stopFlg=true;
      TmcFinish(deviceID);
      return;
    }
    sta=TmcSend(deviceID,(char*)":NUM:NORM:ITEM15 ETA3,1,TOT");
    if(sta!=0)
    {
      qInfo()<<Q_FUNC_INFO<<"Send item15 require error.";
      stopFlg=true;
      TmcFinish(deviceID);
      return;
    }
    sta=TmcSend(deviceID,(char*)":NUM:NORM:ITEM16 P,SIGM,TOT");
    if(sta!=0)
    {
      qInfo()<<Q_FUNC_INFO<<"Send item16 require error.";
      stopFlg=true;
      TmcFinish(deviceID);
      return;
    }
    sta=TmcSend(deviceID,(char*)":NUM:NORM:ITEM17 IRMS,4,TOT");
    if(sta!=0)
    {
      qInfo()<<Q_FUNC_INFO<<"Send item17 require error.";
      stopFlg=true;
      TmcFinish(deviceID);
      return;
    }
    sta=TmcSend(deviceID,(char*)":NUM:NORM:ITEM18 URMS,4,TOT");
    if(sta!=0)
    {
      qInfo()<<Q_FUNC_INFO<<"Send item18 require error.";
      stopFlg=true;
      TmcFinish(deviceID);
      return;
    }
    sta=TmcSend(deviceID,(char*)":NUM:NORM:ITEM19 P,4,TOT");
    if(sta!=0)
    {
      qInfo()<<Q_FUNC_INFO<<"Send item19 require error.";
      stopFlg=true;
      TmcFinish(deviceID);
      return;
    }
    sta=TmcSend(deviceID,(char*)":NUM:NORM:ITEM20 Q,4,TOT");
    if(sta!=0)
    {
      qInfo()<<Q_FUNC_INFO<<"Send item20 require error.";
      stopFlg=true;
      TmcFinish(deviceID);
      return;
    }
    sta=TmcSend(deviceID,(char*)":NUM:NORM:ITEM21 S,1,TOT");
    if(sta!=0)
    {
      qInfo()<<Q_FUNC_INFO<<"Send item21 require error.";
      stopFlg=true;
      TmcFinish(deviceID);
      return;
    }
    sta=TmcSend(deviceID,(char*)":NUM:NORM:ITEM22 S,2,TOT");
    if(sta!=0)
    {
      qInfo()<<Q_FUNC_INFO<<"Send item22 require error.";
      stopFlg=true;
      TmcFinish(deviceID);
      return;
    }
    sta=TmcSend(deviceID,(char*)":NUM:NORM:ITEM23 S,3,TOT");
    if(sta!=0)
    {
      qInfo()<<Q_FUNC_INFO<<"Send item23 require error.";
      stopFlg=true;
      TmcFinish(deviceID);
      return;
    }
    sta=TmcSend(deviceID,(char*)":NUM:NORM:ITEM24 S,4,TOT");
    if(sta!=0)
    {
      qInfo()<<Q_FUNC_INFO<<"Send item24 require error.";
      stopFlg=true;
      TmcFinish(deviceID);
      return;
    }

    sta=TmcSend(deviceID,(char*)":NUM:NORM:ITEM25 PF,1,TOT");
    if(sta!=0)
    {
      qInfo()<<Q_FUNC_INFO<<"Send item25 require error.";
      stopFlg=true;
      TmcFinish(deviceID);
      return;
    }
    sta=TmcSend(deviceID,(char*)":NUM:NORM:ITEM26 PF,2,TOT");
    if(sta!=0)
    {
      qInfo()<<Q_FUNC_INFO<<"Send item26 require error.";
      stopFlg=true;
      TmcFinish(deviceID);
      return;
    }
    sta=TmcSend(deviceID,(char*)":NUM:NORM:ITEM27 PF,3,TOT");
    if(sta!=0)
    {
      qInfo()<<Q_FUNC_INFO<<"Send item27 require error.";
      stopFlg=true;
      TmcFinish(deviceID);
      return;
    }
    sta=TmcSend(deviceID,(char*)":NUM:NORM:ITEM28 PF,4,TOT");
    if(sta!=0)
    {
      qInfo()<<Q_FUNC_INFO<<"Send item28 require error.";
      stopFlg=true;
      TmcFinish(deviceID);
      return;
    }
    sta=TmcSend(deviceID,(char*)":NUM:NORM:ITEM29 PHI,1,TOT");
    if(sta!=0)
    {
      qInfo()<<Q_FUNC_INFO<<"Send item29 require error.";
      stopFlg=true;
      TmcFinish(deviceID);
      return;
    }
    sta=TmcSend(deviceID,(char*)":NUM:NORM:ITEM30 PHI,2,TOT");
    if(sta!=0)
    {
      qInfo()<<Q_FUNC_INFO<<"Send item30 require error.";
      stopFlg=true;
      TmcFinish(deviceID);
      return;
    }
    sta=TmcSend(deviceID,(char*)":NUM:NORM:ITEM31 PHI,3,TOT");
    if(sta!=0)
    {
      qInfo()<<Q_FUNC_INFO<<"Send item31 require error.";
      stopFlg=true;
      TmcFinish(deviceID);
      return;
    }
    sta=TmcSend(deviceID,(char*)":NUM:NORM:ITEM32 PHI,4,TOT");
    if(sta!=0)
    {
      qInfo()<<Q_FUNC_INFO<<"Send item32 require error.";
      stopFlg=true;
      TmcFinish(deviceID);
      return;
    }
    sta=TmcSend(deviceID,(char*)":NUM:NORM:ITEM33 SPE,1,TOT");
    if(sta!=0)
    {
      qInfo()<<Q_FUNC_INFO<<"Send item33 require error.";
      stopFlg=true;
      TmcFinish(deviceID);
      return;
    }
    sta=TmcSend(deviceID,(char*)":NUM:NORM:ITEM34 TORQ,1,TOT");
    if(sta!=0)
    {
      qInfo()<<Q_FUNC_INFO<<"Send item34 require error.";
      stopFlg=true;
      TmcFinish(deviceID);
      return;
    }
  //    sta=TmcSend(deviceID,(char*)":NUM:NORM:ITEM35S,4,TOT");
  //    if(sta!=0)
  //    {
  //        qInfo()<<Q_FUNC_INFO<<"Send item35 require error.";
  //        stopFlg=true;
  //    }
  //    sta=TmcSend(deviceID,(char*)":NUM:NORM:ITEM36S,4,TOT");
  //    if(sta!=0)
  //    {
  //        qInfo()<<Q_FUNC_INFO<<"Send item36 require error.";
  //        stopFlg=true;
  //    }
  //    sta=TmcSend(deviceID,(char*)":NUM:NORM:ITEM37S,4,TOT");
  //    if(sta!=0)
  //    {
  //        qInfo()<<Q_FUNC_INFO<<"Send item37 require error.";
  //        stopFlg=true;
  //    }
  //    sta=TmcSend(deviceID,(char*)":NUM:NORM:ITEM38S,4,TOT");
  //    if(sta!=0)
  //    {
  //        qInfo()<<Q_FUNC_INFO<<"Send item38 require error.";
  //        stopFlg=true;
  //    }
  //    sta=TmcSend(deviceID,(char*)":NUM:NORM:ITEM39S,4,TOT");
  //    if(sta!=0)
  //    {
  //        qInfo()<<Q_FUNC_INFO<<"Send item39 require error.";
  //        stopFlg=true;
  //    }
  //    sta=TmcSend(deviceID,(char*)":NUM:NORM:ITEM40S,4,TOT");
  //    if(sta!=0)
  //    {
  //        qInfo()<<Q_FUNC_INFO<<"Send item40 require error.";
  //        stopFlg=true;
  //    }
  //    sta=TmcSend(deviceID,(char*)":NUM:NORM:ITEM41S,4,TOT");
  //    if(sta!=0)
  //    {
  //        qInfo()<<Q_FUNC_INFO<<"Send item41 require error.";
  //        stopFlg=true;
  //    }
  //    sta=TmcSend(deviceID,(char*)":NUM:NORM:ITEM42S,4,TOT");
  //    if(sta!=0)
  //    {
  //        qInfo()<<Q_FUNC_INFO<<"Send item42 require error.";
  //        stopFlg=true;
  //    }
  //    sta=TmcSend(deviceID,(char*)":NUM:NORM:ITEM43S,4,TOT");
  //    if(sta!=0)
  //    {
  //        qInfo()<<Q_FUNC_INFO<<"Send item43 require error.";
  //        stopFlg=true;
  //    }
  //    sta=TmcSend(deviceID,(char*)":NUM:NORM:ITEM44S,4,TOT");
  //    if(sta!=0)
  //    {
  //        qInfo()<<Q_FUNC_INFO<<"Send item44 require error.";
  //        stopFlg=true;
  //    }
  //    sta=TmcSend(deviceID,(char*)":NUM:NORM:ITEM45S,4,TOT");
  //    if(sta!=0)
  //    {
  //        qInfo()<<Q_FUNC_INFO<<"Send item45 require error.";
  //        stopFlg=true;
  //    }
  //    sta=TmcSend(deviceID,(char*)":NUM:NORM:ITEM46S,4,TOT");
  //    if(sta!=0)
  //    {
  //        qInfo()<<Q_FUNC_INFO<<"Send item46 require error.";
  //        stopFlg=true;
  //    }
  //    sta=TmcSend(deviceID,(char*)":NUM:NORM:ITEM47S,4,TOT");
  //    if(sta!=0)
  //    {
  //        qInfo()<<Q_FUNC_INFO<<"Send item47 require error.";
  //        stopFlg=true;
  //    }
  //    sta=TmcSend(deviceID,(char*)":NUM:NORM:ITEM48S,4,TOT");
  //    if(sta!=0)
  //    {
  //        qInfo()<<Q_FUNC_INFO<<"Send item48 require error.";
  //        stopFlg=true;
  //    }

    emit wtFunStateChanged(3, funIndex);
    qInfo()<<"WT5000 设备通讯运行";
    wtState = 3;

    while(!stopFlg)
    {
        msleep(m_periodms);

        currentTime = QDateTime::currentSecsSinceEpoch();

        currentTime_ms = QDateTime::currentMSecsSinceEpoch();
        qint64 time_100ns = currentTime_ms * 10000;   // ms 转化成 100ns

        //copyToSharedMemory(sm_wt5000, 0, (char*)&currentTime, 8);
        copyToSharedMemory(sm_wt5000, 0, (char*)&time_100ns, 8);

        //uint64ToCharData(currentTime,sigBuff,0);
        //FN_UpdateChannelData(sharedMemorySendToConsole,0,sigBuff,8);

        sta=TmcSend(deviceID,(char*)":NUM:VAL?");
        if(sta!=0)
        {
//            qInfo()<<Q_FUNC_INFO<<"Send format require error,disconnect from WT5000.";
            //stopFlg=true;
            //emit reconnect();
        }

        sta=TmcReceive(deviceID,buff,65535,&length);
        if(sta!=0)
        {
//            qInfo()<<Q_FUNC_INFO<<"Receive error,disconnect from WT5000.";
            //stopFlg=true;
            //emit reconnect();
        }
        else
        {
            //FN_UpdateChannelData(sharedMemorySendToConsole,8,buff+6,4*48);
            //所有float数据更新至共享内存中。
            copyToSharedMemory(sm_wt5000, 8, buff + 6, 4 * 48);
            //qDebug()<<"rcv len:"<<length;

            emit wtDataUpdated();


            quint8 buf_rcd[200];
            //memcpy(buf_rcd, (quint8*)time_100ns, 8);
            //memcpy(buf_rcd + 8, (quint8*)buff + 6, 4 * 48);
            copyFromSharedMemory(sm_wt5000, 0, (char*)buf_rcd, 200);

            //数据提交记录
            emit wtDataForRecord(buf_rcd, 200, smKey);

        }

        sta=TmcSend(deviceID,(char*)":COMM:WAIT 1");
        if(sta!=0)
        {
//            qInfo()<<Q_FUNC_INFO<<"Send wait require error,disconnect from WT5000.";
            //stopFlg=true;
            //emit reconnect();
        }
    }

    disconnectDevice();
}

void WT5000_Thread::wtDataUpdatedSlot()
{
    if(!sm_wt5000) return;

    char buf[200];

    copyFromSharedMemory(sm_wt5000, 0, buf, 200);

    for(int i = 0; i < readPamList.count(); i++)
    {
        PARAM *pam = readPamList.at(i);

        if(i == 0)
        {
            qint64 value = *(qint64*)buf;

            pam->setValue((qreal)value);
        }
        else
        {
            char dataIntel[4];
            reverseData_4bytes(buf + 8 + (i-1) * 4, dataIntel);

            float value = *(float*)dataIntel;

            pam->setValue(value);
        }

    }
}


void WT5000_Thread::reverseData_4bytes(char *src, char *dst)
{
    if(!src || !dst) return;

    dst[0] = src[3];
    dst[1] = src[2];
    dst[2] = src[1];
    dst[3] = src[0];
}

