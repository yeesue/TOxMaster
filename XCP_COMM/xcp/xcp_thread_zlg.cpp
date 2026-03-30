#include "xcp_thread_zlg.h"


XCP_R_Thread_ZLG::XCP_R_Thread_ZLG(QObject *parent)
{

}

XCP_R_Thread_ZLG::~XCP_R_Thread_ZLG()
{
    requestInterruption();
    quit();
    wait();
}

void XCP_R_Thread_ZLG::setIsStop(bool value)
{
    isStop = value;
}


void XCP_R_Thread_ZLG::setRcvBufSize(int value)
{
    rcvBufSize = value;
}

bool XCP_R_Thread_ZLG::getResDataReady() const
{
    return resDataReady;
}

void XCP_R_Thread_ZLG::setResDataReady(bool value)
{
    resDataReady = value;
}

void XCP_R_Thread_ZLG::run()
{

    qDebug()<<"rcv thread: channelHandle:"<<channelHandle<<", canIndex:"<<canIndex<<",id:0x"<<QString::number(id_Xcp_SlaveToMaster, 16);
    while (!isInterruptionRequested())
    {
        usleep(10);

        if(canType == 0)
        {
            BYTE type = TYPE_CAN;
            quint32 rcvNum = ZCAN_GetReceiveNum(channelHandle, type);
            if(rcvNum)
            {
                pCanDataRec = new ZCAN_Receive_Data[rcvNum];
                quint32 rcvNumReturn = ZCAN_Receive(channelHandle, pCanDataRec, rcvNum, 50);

                if(rcvNum == rcvNumReturn)
                {
                    unpackCanFrame(rcvNum);
                }

                delete[] pCanDataRec;
            }
        }
        else if(canType == 1)
        {
            quint32 rcvNum_CAN = ZCAN_GetReceiveNum(channelHandle, TYPE_CAN);
            if(rcvNum_CAN > 0)
            {
                pCanDataRec = new ZCAN_Receive_Data[rcvNum_CAN];
                quint32 rcvNumReturn_CAN = ZCAN_Receive(channelHandle, pCanDataRec, rcvNum_CAN, 50);

                if(rcvNum_CAN == rcvNumReturn_CAN)
                {
                    unpackCanFrame(rcvNum_CAN);
                }

                delete[] pCanDataRec;
            }

            quint32 rcvNum_CANFD = ZCAN_GetReceiveNum(channelHandle, TYPE_CANFD);
            if(rcvNum_CANFD > 0)
            {
                pCanFDDataRec = new ZCAN_ReceiveFD_Data[rcvNum_CANFD];
                quint32 rcvNumReturn_CANFD = ZCAN_ReceiveFD(channelHandle, pCanFDDataRec, rcvNum_CANFD, 50);

                if(rcvNum_CANFD == rcvNumReturn_CANFD)
                {
                    unpackCanFdFrame(rcvNum_CANFD);
                }

                delete[] pCanFDDataRec;
            }
        }

    }

}

void XCP_R_Thread_ZLG::unpackCanFrame(quint32 num)
{
    for(int i = 0; i < num; i++)
    {
        ZCAN_Receive_Data canData = pCanDataRec[i];
        quint32 id = canData.frame.can_id;

        //qDebug()<<"Receive rx msg: 0x"<<QString::number(id, 16)<<", time:" <<canData.timestamp;

        if(id != this->id_Xcp_SlaveToMaster && eventIdList.indexOf(id) == -1) continue;

        frameTimeStamp = canData.timestamp * 10;  //ZLG中的timestamp单位为1us, 为了与NI的时间戳一致，变为100ns(*10)
        if(frameTimeStamp <= frameTimeStamp_last) return;

        quint8 len = canData.frame.can_dlc + 8;
        quint8 *buf = new quint8[len];
        memcpy(buf, (quint8*)&(frameTimeStamp), 8);
        memcpy(buf+8, canData.frame.data, canData.frame.can_dlc);
        quint8 pid = canData.frame.data[0];

        if (pid > 0xFB)
        {
            resDataReady = true;
            emit RESDataReady_ZLG(buf, len);
        }
        else
        {
            emit ODTDataReady_ZLG(buf, len);
        }

        frameTimeStamp_last = frameTimeStamp;
    }
}

void XCP_R_Thread_ZLG::unpackCanFdFrame(quint32 num)
{
    for(int i = 0; i < num; i++)
    {
        ZCAN_ReceiveFD_Data canData = pCanFDDataRec[i];
        quint32 id = canData.frame.can_id;

        //qDebug()<<"Receive rx msg: 0x"<<QString::number(id, 16)<<", time:" <<canData.timestamp;

        if(id != this->id_Xcp_SlaveToMaster && eventIdList.indexOf(id) == -1) continue;

        frameTimeStamp = canData.timestamp * 10;  //ZLG中的timestamp单位为1us, 为了与NI的时间戳一致，变为100ns(*10)
        if(frameTimeStamp <= frameTimeStamp_last) return;

        quint8 len = canData.frame.len + 8;
        quint8 *buf = new quint8[len];
        memcpy(buf, (quint8*)&(frameTimeStamp), 8);
        memcpy(buf+8, canData.frame.data, canData.frame.len);
        quint8 pid = canData.frame.data[0];

        if (pid > 0xFB)
        {
            resDataReady = true;
            emit RESDataReady_ZLG(buf, len);
        }
        else
        {
            emit ODTDataReady_ZLG(buf, len);
        }

        frameTimeStamp_last = frameTimeStamp;
    }
}

///////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////


XCP_W_Thread_ZLG::XCP_W_Thread_ZLG(QObject *parent)
{

}

XCP_W_Thread_ZLG::~XCP_W_Thread_ZLG()
{
    requestInterruption();
    quit();
    wait();
}

void XCP_W_Thread_ZLG::setWriteOnceEnable(bool enable)
{
    writeOnceEnable = enable;
    writeSucceed = false;
}

bool XCP_W_Thread_ZLG::writeOnceSucceed()
{
    return writeSucceed;
}

void XCP_W_Thread_ZLG::setCMDPayLoad(quint8 *data, quint32 numByte)
{
    if(canType == 0)
    {
        quint8 *dst = pCanDataSnd.frame.data;
        memcpy(dst, data, numByte);
        pCanDataSnd.frame.can_dlc = numByte;
    }
    else if(canType == 1)
    {
        quint8 *dst = pCanFDDataSnd.frame.data;
        memcpy(dst, data, numByte);
        pCanFDDataSnd.frame.len = numByte;
    }

}

void XCP_W_Thread_ZLG::run()
{
    qDebug()<<"snd thread: channelHandle:"<<channelHandle<<", canIndex:"<<canIndex<<",id:0x"<<QString::number(id_Xcp_MasterToSlave, 16);
    while (!isInterruptionRequested())
    {
        usleep(10);

        if(!packTransmitData()) continue;

        if (writeOnceEnable)
        {

            if(canType == 0)
            {
                quint32 sndNum = ZCAN_Transmit(channelHandle, &pCanDataSnd, 1);
                if(sndNum != 1)
                {
                    qDebug()<<"Transmit error:"<<pCanDataSnd.frame.can_id;
                    //continue;
                }
                else
                {
                    //qDebug()<<"transmit, id:0x"<<QString::number(pCanDataSnd.frame.can_id, 16);
                    writeSucceed = true;
                }
            }
            else
            {               
                quint32 sndNum = ZCAN_TransmitFD(channelHandle, &pCanFDDataSnd, 1);
                if(sndNum != 1)
                {
                    qDebug()<<"TransmitFD error:"<<pCanFDDataSnd.frame.can_id;
                    //continue;
                }
                else
                {
                    writeSucceed = true;
                }
            }

            writeOnceEnable = false;
        }
    }
}

bool XCP_W_Thread_ZLG::packTransmitData()
{
    if(canType == 0)
    {
        pCanDataSnd.transmit_type = 0;
        pCanDataSnd.frame.can_id = id_Xcp_MasterToSlave;
        pCanDataSnd.frame.can_dlc = 8;
    }
    else
    {
        pCanFDDataSnd.transmit_type = 0;
        pCanFDDataSnd.frame.can_id = id_Xcp_MasterToSlave;
        pCanFDDataSnd.frame.len = 64;
    }

    return true;

}

//////////////////////////////////////////////////////////////////////
XCP_Thread_ZLG::XCP_Thread_ZLG(QObject *parent) : QObject(parent)
{
    thread_RES = new XCP_R_Thread_ZLG(this);
    thread_CMD = new XCP_W_Thread_ZLG(this);
    //thread_EVENT = new XCP_R_Event_Thread(this);
}

XCP_Thread_ZLG::~XCP_Thread_ZLG()
{
    //delete thread_EVENT;
    delete thread_RES;
    delete thread_CMD;
}

QStringList XCP_Thread_ZLG::getCANInterface()
{
    QStringList interfaceList;

    quint32 chnCount = 0;

    for(int i = 0; i < chnCount; i++)
    {
        QString chnName = "CAN" + QString::number(i+1);
        interfaceList.append(chnName);
    }

    return interfaceList;
}

bool XCP_Thread_ZLG::XCP_CAN_Init()
{
    qDebug()<<"In ZLG XCP CAN Init";
    deviceHandle = ZCAN_OpenDevice(deviceType, deviceIndex, 0);
    if(deviceHandle != INVALID_DEVICE_HANDLE)
    {

        qDebug()<<"打开ZLG设备成功, DeviceHandle:"<<deviceHandle;
    }
    else
    {
        qDebug()<<"打开ZLG设备失败";
    }


    if(canType == 0)
    {
        quint32 status1 = ZCAN_SetAbitBaud(deviceHandle, canIndex, canfd_abitBaudrate);
        //qDebug()<<"set canfd abit baudrate status:"<<status2;

        quint32 status2 = ZCAN_SetDbitBaud(deviceHandle, canIndex, canfd_dbitBaudrate);
        //qDebug()<<"set canfd dbit baudrate status:"<<status3;

        if(status1 && status2)
        {
            qDebug()<<"设置CAN波特率成功, arbit_baud:"<<canfd_abitBaudrate<<", data_baud:"<<canfd_dbitBaudrate;
        }
        else
        {
            qDebug()<<"设置CAN波特率失败";
            //return false;
        }
    }
    else
    {
        quint32 status1 = ZCAN_SetCANFDStandard(deviceHandle, canIndex, canfd_standard);
        //qDebug()<<"set canfd standard status:"<<status1;
    }

    //init can
    canPortInitConfig.can_type = canType > 0 ? TYPE_CANFD : TYPE_CANFD;   // even if is can type, still set as canfd type

    canPortInitConfig.can.acc_code = 0;
    canPortInitConfig.can.acc_mask = 0xFFFFFFFF; // SJA1000帧过滤屏蔽码，设置成1代表“无关位”
    canPortInitConfig.can.filter = 0;
    canPortInitConfig.can.mode = 0;
    canPortInitConfig.can.timing0 = 0;
    canPortInitConfig.can.timing1 = 0;
    canPortInitConfig.can.reserved = 0;

    canPortInitConfig.canfd.acc_code = 0;
    canPortInitConfig.canfd.acc_mask = 0;
    canPortInitConfig.canfd.abit_timing = 0;
    canPortInitConfig.canfd.dbit_timing = 0;
    canPortInitConfig.canfd.brp = 0;
    canPortInitConfig.canfd.filter = 0;
    canPortInitConfig.canfd.mode = 0;         // 0=normal, 1=only listen
    canPortInitConfig.canfd.pad = 0;
    canPortInitConfig.canfd.reserved = 0;

    channelHandle =  ZCAN_InitCAN(deviceHandle, canIndex, &canPortInitConfig);
    if(channelHandle != INVALID_CHANNEL_HANDLE)
    {
        qDebug()<<"初始化CAN通道成功,ChannelHandle:"<<channelHandle;
    }
    else
    {
        qDebug()<<"初始化CAN通道失败";
    }

    quint32 status = ZCAN_SetResistanceEnable(channelHandle, canIndex, resistanceEnable);
    //qDebug()<<"set resistanceEnable status:"<<status;

    status = ZCAN_StartCAN(channelHandle);
    if(status == STATUS_OK)
    {
        qDebug()<<"启动CAN通道成功,状态:"<<status;
    }
    else
    {
        qDebug()<<"启动CAN通道失败";
    }

    //创建读和写线程
    if(thread_RES == NULL)
    {
        thread_RES = new XCP_R_Thread_ZLG(this);
    }
    thread_RES->channelHandle = this->channelHandle;
    thread_RES->canIndex = this->canIndex;
    thread_RES->canType = this->canType;
    thread_RES->id_Xcp_SlaveToMaster = this->id_RES;
    // extFrame的31位为1
    if(thread_RES->id_Xcp_SlaveToMaster > 0x8FF)
    {
        thread_RES->id_Xcp_SlaveToMaster += 0x80000000;
    }

    thread_RES->start();

    if(thread_CMD == NULL)
    {
        thread_CMD = new XCP_W_Thread_ZLG(this);
    }
    thread_CMD->channelHandle = this->channelHandle;
    thread_CMD->canIndex = this->canIndex;
    thread_CMD->canType = this->canType;
    thread_CMD->id_Xcp_MasterToSlave = this->id_CMD;
    // extFrame的31位为1
    if(thread_CMD->id_Xcp_MasterToSlave > 0x8FF)
    {
        thread_CMD->id_Xcp_MasterToSlave += 0x80000000;
    }

    thread_CMD->start();

    return true;
}

void XCP_Thread_ZLG::XCP_CAN_Stop()
{
    thread_RES->requestInterruption();
    thread_RES->quit();
    thread_RES->wait();

    thread_CMD->requestInterruption();
    thread_CMD->quit();
    thread_CMD->wait();

    if(channelHandle != INVALID_CHANNEL_HANDLE)
    {
        ZCAN_ClearBuffer(channelHandle);
        ZCAN_SetFilterClear(channelHandle, canIndex);

        ZCAN_ResetCAN(channelHandle);

        channelHandle = INVALID_CHANNEL_HANDLE;

        // close device
        quint32 status = ZCAN_CloseDevice(deviceHandle);
        if(status == STATUS_OK)
        {
            qDebug()<<"关闭设备成功";
        }
        else if(status == STATUS_ERR)
        {
            qDebug()<<"关闭设备错误";
        }

        deviceHandle = INVALID_DEVICE_HANDLE;
    }
}
