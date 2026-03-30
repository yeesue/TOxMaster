#include "wt5000thread.h"
//#include "vc/tmctl.h"
#include "tmctl.h"
//#include "fn_datashare.h"
//#include "datatransfer.h"
#include <QDebug>
#include <QDateTime>


WT5000Thread::WT5000Thread()
{

}

WT5000Thread::~WT5000Thread()
{
    disconnectDevice();
}

bool WT5000Thread::connectDevice()
{
//    QString IPConfig=IPAddress+",anonymous,";
//    QByteArray temp=IPConfig.toLatin1();
//    sta=TmcInitialize(TM_CTL_ETHER,temp.data(),&deviceID);
    //GPIB
    //sta=TmcInitialize(TM_CTL_GPIB,"1",&deviceID);
    //WT5000 使用VTI11

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

bool WT5000Thread::disconnectDevice()
{
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

void WT5000Thread::setIPAddress(QString address)
{
    IPAddress=address;
}

void WT5000Thread::run()
{
    char buff[65535];
    int length;
    qint64 currentTime;
    char sigBuff[8];

    sta=TmcSend(deviceID,(char*)":INPUT:MODULE?");
    if(sta!=0)
    {
        qInfo()<<Q_FUNC_INFO<<"Send input module error";
        stopFlg=true;
        TmcFinish(deviceID);
        return;
    }
    sta=TmcReceive(deviceID,buff,65535,&length);
    if(sta!=0)
    {
        qInfo()<<Q_FUNC_INFO<<"Receive error";
        stopFlg=true;
        TmcFinish(deviceID);
        return;
    }
    sta=TmcSend(deviceID,(char*)":NUM:FORM FLO");
    if(sta!=0)
    {
        qInfo()<<Q_FUNC_INFO<<"Send format require error.";
        stopFlg=true;
        TmcFinish(deviceID);
        return;
    }
    sta=TmcSend(deviceID,(char*)":NUM:NUM 48");
    if(sta!=0)
    {
        qInfo()<<Q_FUNC_INFO<<"Send numbers require error.";
        stopFlg=true;
        TmcFinish(deviceID);
        return;
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
//    sta=TmcSend(deviceID,(char*)":NUM:NORM:ITEM33S,4,TOT");
//    if(sta!=0)
//    {
//        qInfo()<<Q_FUNC_INFO<<"Send item33 require error.";
//        stopFlg=true;
//    }
//    sta=TmcSend(deviceID,(char*)":NUM:NORM:ITEM34S,4,TOT");
//    if(sta!=0)
//    {
//        qInfo()<<Q_FUNC_INFO<<"Send item34 require error.";
//        stopFlg=true;
//    }
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


    while(!stopFlg)
    {
        msleep(m_periodms);

        currentTime=QDateTime::currentSecsSinceEpoch();
        copyToSharedMemory(sm_wt5000, 0, (char*)&currentTime, 8);

        //uint64ToCharData(currentTime,sigBuff,0);
        //FN_UpdateChannelData(sharedMemorySendToConsole,0,sigBuff,8);

        sta=TmcSend(deviceID,(char*)":NUM:VAL?");
        if(sta!=0)
        {
//            qInfo()<<Q_FUNC_INFO<<"Send format require error,disconnect from WT3000.";
            //stopFlg=true;
            //emit reconnect();
        }

        sta=TmcReceive(deviceID,buff,65535,&length);
        if(sta!=0)
        {
//            qInfo()<<Q_FUNC_INFO<<"Receive error,disconnect from WT3000.";
            //stopFlg=true;
            //emit reconnect();
        }
        else
        {
            //FN_UpdateChannelData(sharedMemorySendToConsole,8,buff+6,4*48);
            //所有float数据更新至共享内存中。
            copyToSharedMemory(sm_wt5000, 8, buff + 6, 4 * 48);
            //emit rcvUpdated();
        }

    }
}
