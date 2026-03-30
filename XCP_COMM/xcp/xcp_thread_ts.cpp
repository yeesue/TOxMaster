#include "xcp_thread_ts.h"

quint8 fromDlcToFDlc(quint8 dlc)
{
    if(dlc <= 8) return dlc;
    quint8 fdlc = 0;
    switch (dlc) {
    case 12:
        fdlc = 9;
        break;
    case 16:
        fdlc = 10;
        break;
    case 20:
        fdlc = 11;
        break;
    case 24:
        fdlc = 12;
        break;
    case 32:
        fdlc = 13;
        break;
    case 48:
        fdlc = 14;
        break;
    case 64:
        fdlc = 15;
        break;
    default:
        break;
    }

    return fdlc;
}

quint8 fromFDlcToDlc(quint8 fdlc)
{
    if(fdlc <= 8) return fdlc;
    quint8 dlc = 0;
    switch (fdlc) {
    case 9:
        dlc = 12;
        break;
    case 10:
        dlc = 16;
        break;
    case 11:
        dlc = 20;
        break;
    case 12:
        dlc = 24;
        break;
    case 13:
        dlc = 32;
        break;
    case 14:
        dlc = 48;
        break;
    case 15:
        dlc = 64;
        break;
    default:
        break;
    }

    return dlc;
}

//////////////////////////////////////////////////////////
XCP_R_Thread_TS::XCP_R_Thread_TS(QObject *parent)
{

}

XCP_R_Thread_TS::~XCP_R_Thread_TS()
{
    requestInterruption();
    quit();
    wait();
}

void XCP_R_Thread_TS::setIsStop(bool value)
{
    isStop = value;
}


void XCP_R_Thread_TS::setRcvBufSize(int value)
{
    rcvBufSize = value;
}

bool XCP_R_Thread_TS::getResDataReady() const
{
    return resDataReady;
}

void XCP_R_Thread_TS::setResDataReady(bool value)
{
    resDataReady = value;
}

void XCP_R_Thread_TS::startRcv()
{
    initReadBuf();

    int status = tsfifo_clear_canfd_receive_buffers(deviceHandle, canIndex);
    qDebug()<<"Can rcv thread, clear read buffer status:"<<status;
}

void XCP_R_Thread_TS::stopRcv()
{
    delReadBuf();
}

void XCP_R_Thread_TS::run()
{
    initReadBuf();

    qDebug()<<"deviceHandle:"<<deviceHandle<<", canIndex:"<<canIndex<<", retBufSize:"<<returnBufSize;
    while (!isInterruptionRequested())
    {
        usleep(100);

        if(canType == 0)
        {
            returnBufSize = 100;
            u32 retValue = tsfifo_receive_can_msgs(deviceHandle, pCanDataRec, &returnBufSize, canIndex, TX_RX_MESSAGES);
            if(retValue == 0x00)
            {
                //qDebug()<<"receive can msg ok, retSize: "<<returnBufSize;

                unpackCanFrame();
            }
            else
            {
                //qDebug()<<"receive can msg error: "<<retValue;
            }
        }
        else if(canType == 1)
        {
            returnBufSize = 100;
            u32 retValue = tsfifo_receive_canfd_msgs(deviceHandle, pCanFDDataRec, &returnBufSize, canIndex, TX_RX_MESSAGES);
            if(retValue == 0x00)
            {
                //qDebug()<<"receive canfd msg ok, retSize: "<<returnBufSize;

                unpackCanFdFrame();
            }
            else
            {
                //qDebug()<<"receive canfd msg error: "<<retValue;
            }
        }

    }

    delReadBuf();
}

void XCP_R_Thread_TS::unpackCanFrame()
{
    for(int i = 0; i < returnBufSize; i++)
    {
        TLIBCAN canData = pCanDataRec[i];
        qint32 id = canData.FIdentifier;

        if(canData.FProperties.bits.istx == 1)
        {
            //qDebug()<<"Receive tx msg: 0x"<<QString::number(id, 16)<<", time:" <<canData.FTimeUS;
            continue;
        }

        if((quint32)id != this->id_Xcp_SlaveToMaster && eventIdList.indexOf((quint32)id) == -1) continue;

        //qDebug()<<"Receive rx msg: 0x"<<QString::number(id, 16)<<", time:" <<canData.FTimeUS<<", data:"<<QString::number(canData.FData[0], 16);

        frameTimeStamp = canData.FTimeUS * 10;  //TS中的timestamp单位为1us, 为了与NI的时间戳一致，变为100ns(*10)
        if(frameTimeStamp <= frameTimeStamp_last) return;

        quint8 len = canData.FDLC + 8;
        quint8 *buf = new quint8[len];
        memcpy(buf, (quint8*)&(frameTimeStamp), 8);
        memcpy(buf+8, canData.FData, canData.FDLC);
        quint8 pid = canData.FData[0];

        if (pid > 0xFB)
        {
            resDataReady = true;
            emit RESDataReady_TS(buf, len);
        }
        else
        {
            emit ODTDataReady_TS(buf, len);
        }

        frameTimeStamp_last = frameTimeStamp;
    }
}

void XCP_R_Thread_TS::unpackCanFdFrame()
{
    for(int i = 0; i < returnBufSize; i++)
    {
        TLIBCANFD canData = pCanFDDataRec[i];
        qint32 id = canData.FIdentifier;

        if(canData.FProperties.bits.istx == 1)
        {
            //qDebug()<<"Receive tx msg: 0x"<<QString::number(id, 16)<<", time:" <<canData.FTimeUS;
            continue;
        }

        if((quint32)id != this->id_Xcp_SlaveToMaster && eventIdList.indexOf((quint32)id) == -1) continue;

        //qDebug()<<"Receive rx msg: 0x"<<QString::number(id, 16)<<", time:" <<canData.FTimeUS<<", dlc:"<<QString::number(canData.FDLC);

        frameTimeStamp = canData.FTimeUS * 10;  //TS中的timestamp单位为1us, 为了与NI的时间戳一致，变为100ns(*10)
        if(frameTimeStamp <= frameTimeStamp_last) return;

        //同星fd报文dlc转换成实际dlc
        quint8 fdlc = fromFDlcToDlc(canData.FDLC);

        quint8 len = fdlc + 8;
        quint8 *buf = new quint8[len];
        memcpy(buf, (quint8*)&(frameTimeStamp), 8);
        memcpy(buf+8, canData.FData, fdlc);
        quint8 pid = canData.FData[0];
        //qDebug()<<"pid:"<<pid;

        if (pid > 0xFB)
        {
            resDataReady = true;
            emit RESDataReady_TS(buf, len);
        }
        else
        {
            emit ODTDataReady_TS(buf, len);
        }

        frameTimeStamp_last = frameTimeStamp;
    }
}

void XCP_R_Thread_TS::initReadBuf()
{
    if(canType == 0)
    {
        pCanDataRec = new TLIBCAN[rcvBufSize];

        //connect(this, SIGNAL(canDataRcvd(TLibCAN)), this, SLOT(rcvCanDataHandle(TLibCAN)));

        //register event can
        //TCANQueueEvent_Win32_t fun = (TCANQueueEvent_Win32_t)&rcvCanMsgHandle;
        //tscan_register_event_can(deviceHandle, fun);
    }
    else
    {
        pCanFDDataRec = new TLIBCANFD[rcvBufSize];

        //connect(this, SIGNAL(rcvDataUpdated(quint32,quint8*,quint8)), this, SLOT(Slt_rcvDataUpdated(quint32,quint8*,quint8)));

        //register event can
        //TCANFDQueueEvent_Win32_t fun = (TCANFDQueueEvent_Win32_t)&rcvCanFdMsgHandle;
        //tscan_register_event_canfd(deviceHandle, fun);
    }
}

void XCP_R_Thread_TS::delReadBuf()
{
    if(canType == 0)
    {
        //register event can
        //TCANQueueEvent_Win32_t fun = (TCANQueueEvent_Win32_t)&rcvCanMsgHandle;
        //tscan_unregister_event_can(deviceHandle, fun);
    }
    else
    {
        //register event can
        //TCANFDQueueEvent_Win32_t fun = (TCANFDQueueEvent_Win32_t)&rcvCanFdMsgHandle;
        //tscan_unregister_event_canfd(deviceHandle, fun);
    }

    if(pCanDataRec)
    {
        delete[] pCanDataRec;
    }
    if(pCanFDDataRec)
    {
        delete[] pCanFDDataRec;
    }
}
///////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////


XCP_W_Thread_TS::XCP_W_Thread_TS(QObject *parent)
{

}

XCP_W_Thread_TS::~XCP_W_Thread_TS()
{
    requestInterruption();
    quit();
    wait();
}

void XCP_W_Thread_TS::setWriteOnceEnable(bool enable)
{
    writeOnceEnable = enable;
    writeSucceed = false;
}

bool XCP_W_Thread_TS::writeOnceSucceed()
{
    return writeSucceed;
}

void XCP_W_Thread_TS::setCMDPayLoad(quint8 *data, u32 numByte)
{
    if(canType == 0)
    {
        quint8 *dst = pCanDataSnd.FData;
        memcpy(dst, data, numByte);
        pCanDataSnd.FDLC = numByte;
    }
    else
    {
        quint8 *dst = pCanFDDataSnd.FData;
        memcpy(dst, data, numByte);
        u32 dlc = fromDlcToFDlc(numByte);
        pCanFDDataSnd.FDLC = dlc;
    }

}

void XCP_W_Thread_TS::run()
{
    while (!isInterruptionRequested())
    {
        usleep(10);

        if(!packTransmitData()) continue;

        if (writeOnceEnable)
        {
            if(canType == 0)
            {
                int retValue = tscan_transmit_can_async(deviceHandle, &pCanDataSnd);
                if(retValue != 0)
                {
                    qDebug()<<"TS Transmit error:"<< pCanDataSnd.FIdentifier;
                    //continue;
                }
                else
                {
                    writeSucceed = true;
                }
            }
            else
            {
                int retValue = tscan_transmit_canfd_async(deviceHandle, &pCanFDDataSnd);
                if(retValue != 0)
                {
                    qDebug()<<"TransmitFD error:"<<pCanFDDataSnd.FIdentifier;
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

bool XCP_W_Thread_TS::packTransmitData()
{
    if(canType == 0)
    {
        pCanDataSnd.FProperties.bits.extframe = id_Xcp_MasterToSlave > 0x8FF ? 1 : 0;
        pCanDataSnd.FProperties.bits.iserrorframe = 0;
        pCanDataSnd.FProperties.bits.istx = 1;
        pCanDataSnd.FProperties.bits.remoteframe = 0;

        pCanDataSnd.FIdxChn = canIndex;

        pCanDataSnd.FIdentifier = id_Xcp_MasterToSlave;
        pCanDataSnd.FDLC = 8;
    }
    else
    {
        pCanFDDataSnd.FIdxChn = canIndex;
        pCanFDDataSnd.FIdentifier = id_Xcp_MasterToSlave;
        //pCanFDDataSnd.FDLC = fromDlcToFDlc(64);

        pCanFDDataSnd.FProperties.value = 0x00;
        pCanFDDataSnd.FProperties.bits.extframe = id_Xcp_MasterToSlave > 0x8FF ? 1 : 0;
        pCanFDDataSnd.FProperties.bits.iserrorframe = 0;
        pCanFDDataSnd.FProperties.bits.istx = 1;
        pCanFDDataSnd.FProperties.bits.remoteframe = 0;

        pCanFDDataSnd.FFDProperties.value = 0x80;
        pCanFDDataSnd.FFDProperties.bits.BRS = (canType == 2) ? 1 : 0;
        pCanFDDataSnd.FFDProperties.bits.EDL = 1;
        pCanFDDataSnd.FFDProperties.bits.ESI = 1;
        //pCanFDDataSnd.FFDProperties.value = 0x80;
    }

    //qDebug()<<"fd property:"<<pCanFDDataSnd.FFDProperties.value;

    return true;

}

//////////////////////////////////////////////////////////////////////
XCP_Thread_TS::XCP_Thread_TS(QObject *parent) : QObject(parent)
{
    thread_RES = new XCP_R_Thread_TS(this);
    thread_CMD = new XCP_W_Thread_TS(this);
    //thread_EVENT = new XCP_R_Event_Thread(this);
}

XCP_Thread_TS::~XCP_Thread_TS()
{
    //delete thread_EVENT;
    delete thread_RES;
    delete thread_CMD;
}

QStringList XCP_Thread_TS::getCANInterface()
{
    QStringList interfaceList;

    int tsDeviceCount = initTsApiObj();

    for(int i = 0; i < tsDeviceCount; i++)
    {
        QString deviceName = "CAN" + QString::number(i+1);
        interfaceList.append(deviceName);
    }

    return interfaceList;
}

bool XCP_Thread_TS::XCP_CAN_Init()
{
    qDebug()<<"In TS XCP CAN Init";
    quint32 deviceCount = initTsApiObj();
    //qDebug()<<"TS Device Count:"<<deviceCount;

    //connect ts device (ts can device)
    uint32_t retValue = tscan_connect("", &deviceHandle);
    if(retValue == 0 || retValue == 5)
    {
        qDebug()<<"连接同星设备成功,句柄:"<<deviceHandle;
    }
    else
    {
        qDebug()<<"连接同星设备失败";
        return false;
    }

    /*
    //添加了之后程序启动就异常退出，不知原因
    s32 chnCount = 0;
    retValue = tscan_get_can_channel_count(deviceHandle, &chnCount);
    if(retValue == 0)
    {
        qDebug()<<"设备CAN通道数:"<<chnCount;
    }
    */

    canfd_abitBaudrate /= 1000;
    canfd_dbitBaudrate /= 1000;

    qDebug()<<"canType:"<<canType<<",a baudrate(kbps):"<<canfd_abitBaudrate<<"d baudrate(kbps):"<<canfd_dbitBaudrate<<", canIndex:"<<canIndex;
    if(canType > 0)
    {
        canfd_standard = 1;  // TLIBCANFDControllerType:  0 = lfdtCAN = 0, lfdtISOCAN = 1, lfdtNonISOCAN = 2
        retValue = tscan_config_canfd_by_baudrate(deviceHandle, canIndex, canfd_abitBaudrate, canfd_dbitBaudrate, (TLIBCANFDControllerType)canfd_standard, lfdmNormal, resistanceEnable);
        if(retValue == 0)
        {
            qDebug()<<"设置CANFD波特率成功, arbit_baud:"<<canfd_abitBaudrate<<", data_baud:"<<canfd_dbitBaudrate;
        }
        else
        {
            qDebug()<<"设置CANFD波特率失败, 返回值:"<<retValue;
            return false;
        }
    }
    else
    {
        retValue = tscan_config_canfd_by_baudrate(deviceHandle, canIndex, canfd_abitBaudrate, canfd_dbitBaudrate, lfdtCAN, lfdmNormal, resistanceEnable);
        if(retValue == 0)
        {
            qDebug()<<"设置CAN波特率成功, arbit_baud:"<<canfd_abitBaudrate<<", data_baud:"<<canfd_dbitBaudrate;
        }
        else
        {
            qDebug()<<"设置CAN波特率失败, 返回值:"<<retValue;
            return false;
        }
    }

    //创建读和写线程
    if(thread_RES == NULL)
    {
        thread_RES = new XCP_R_Thread_TS(this);
    }
    thread_RES->deviceHandle = this->deviceHandle;
    thread_RES->canIndex = this->canIndex;
    thread_RES->canType = this->canType;
    thread_RES->id_Xcp_SlaveToMaster = this->id_RES;

    thread_RES->start();

    if(thread_CMD == NULL)
    {
        thread_CMD = new XCP_W_Thread_TS(this);
    }
    thread_CMD->deviceHandle = this->deviceHandle;
    thread_CMD->canIndex = this->canIndex;
    thread_CMD->canType = this->canType;
    thread_CMD->id_Xcp_MasterToSlave = this->id_CMD;

    thread_CMD->start();

    return true;
}

void XCP_Thread_TS::XCP_CAN_Stop()
{
    thread_RES->requestInterruption();
    thread_RES->quit();
    thread_RES->wait();

    thread_CMD->requestInterruption();
    thread_CMD->quit();
    thread_CMD->wait();

    if(deviceHandle)
    {
        int status = tsfifo_clear_canfd_receive_buffers(deviceHandle, canIndex);
        qDebug()<<"Clear read buffer status:"<<status;
        tscan_disconnect_by_handle(deviceHandle);
    }

    finalize_lib_tscan();


}

quint32 XCP_Thread_TS::initTsApiObj()
{
    /*
    if(tsApiObj == NULL)
    {
        tsApiObj = new TSCANLINApi();
        qDebug()<<"初始化同星接口";
    }

    tsApiObj->InitTSCANAPI(true, false, false);
    */

    initialize_lib_tscan(true, false, false);

    //扫描获取可用同星设备
    uint32_t deviceCount = 0;
    if(tscan_scan_devices(&deviceCount) == 0)
    {
        qDebug()<<"同星设备数量:"<<deviceCount;
    }

    return deviceCount;
}

