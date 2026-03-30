#include "can_thread_ts.h"
#include <QMetaType>

/*
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
*/

///////////////////////////////////////////////////////
Can_R_Thread_TS::Can_R_Thread_TS(QObject *parent)
{

}

Can_R_Thread_TS::~Can_R_Thread_TS()
{
    delReadBuf();
}


void Can_R_Thread_TS::setIsStop(bool value)
{
    isStop = value;
}

void Can_R_Thread_TS::run()
{

    initReadBuf();

    int status = tsfifo_clear_canfd_receive_buffers(deviceHandle, canIndex);
    qDebug()<<"Can rcv thread, clear read buffer status:"<<status;

    while(!isStop)
    {
        msleep(10);

#ifdef Q_OS_WIN
        rcvBufSize = 10;

        if(canType == 0)
        {
            int retValue = tsfifo_receive_can_msgs(deviceHandle, pCanDataRec, &rcvBufSize, canIndex, TX_RX_MESSAGES);

            if(retValue == 0)
            {
                rcvCanDataHandle(rcvBufSize);
            }

        }
        else
        {
            int retValue = tsfifo_receive_canfd_msgs(deviceHandle, pCanFDDataRec, &rcvBufSize, canIndex, TX_RX_MESSAGES);

            if(retValue == 0)
            {
                rcvCanFdDataHandle(rcvBufSize);
            }
        }

#endif

#ifdef Q_OS_LINUX

#endif
    }

    delReadBuf();
}

void Can_R_Thread_TS::initReadBuf()
{
    if(canType == 0)
    {
        pCanDataRec = new TLIBCAN[rcvBufSize];

        //connect(this, SIGNAL(canDataRcvd(TLIBCAN)), this, SLOT(rcvCanDataHandle(TLIBCAN)));

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

void Can_R_Thread_TS::delReadBuf()
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

void Can_R_Thread_TS::Slt_rcvDataUpdated(quint32 id, quint8 *buf, quint8 len)
{
    qDebug()<<"slot for rcv data updated.";
    QSharedMemory *sm = smHash.value(id);
    copyToSharedMemory(sm, 0, (char*)buf, len);

    delete[] buf;
}


void Can_R_Thread_TS::rcvCanDataHandle(qint32 num)
{
    //qDebug()<<"act rcv buf size:"<<num;
    if(num == 0 || pCanDataRec == NULL)
        return;

    for(int i = 0; i < num; i++)
    {
        TLIBCAN canData = pCanDataRec[i];

        //qreal absTime = (qreal)canData.FTimeUS / 1000000.0;  //TS中的timestamp单位为1us
        quint64 timeStamp = (qreal)canData.FTimeUS * 10;  //TS中的timestamp单位为1us, 转换成NI板卡的硬件时间100ns，统一处理

        if(mdfRecordOn)
        {
            quint32 bufSize = canData.FDLC + 8;
            QString mdfKeyName = "RP_TSCAN" + QString::number(canIndex) + "_" + QString::number(canData.FIdentifier);
            char *buf = new char[bufSize];
            memcpy(buf, (char*)&timeStamp, 8);
            memcpy(buf+8, (char*)(canData.FData), canData.FDLC);

            emit canDataForRecord((quint8*)buf, bufSize, mdfKeyName);
        }

        quint32 id = canData.FIdentifier;
        QSharedMemory *sm = smHash.value(id);

        if(sm)
        {
            copyToSharedMemory_2Data(sm, 0, (char*)&timeStamp, 8, 8, (char*)(canData.FData), canData.FDLC);
        }
    }
}

void Can_R_Thread_TS::rcvCanFdDataHandle(quint32 num)
{
    //qDebug()<<"act rcv fd buf size:"<<num;
    if(num == 0 || pCanFDDataRec == NULL)
        return;

    for(int i = 0; i < num; i++)
    {
        TLIBCANFD canFDData = pCanFDDataRec[i];

        //qreal absTime = (qreal)canFDData.FTimeUS / 1000000.0;  //ZLG中的timestamp单位为1us
        quint64 timeStamp = (qreal)canFDData.FTimeUS * 10;  //TS中的timestamp单位为1us, 转换成NI板卡的硬件时间100ns，统一处理
        quint16 dlc = fromFDlcToDlc(canFDData.FDLC);

        if(mdfRecordOn)
        {
            quint32 bufSize = dlc + 8;
            QString mdfKeyName = "RP_TSCAN" + QString::number(canIndex) + "_" + QString::number(canFDData.FIdentifier);
            char *buf = new char[bufSize];
            memcpy(buf, (char*)&timeStamp, 8);
            memcpy(buf+8, (char*)(canFDData.FData), dlc);

            emit canDataForRecord((quint8*)buf, bufSize, mdfKeyName);
        }

        quint32 id = canFDData.FIdentifier;
        QSharedMemory *sm = smHash.value(id);


        if(sm)
        {
            copyToSharedMemory_2Data(sm, 0, (char*)&timeStamp, 8, 8, (char*)(canFDData.FData), dlc);
        }
    }
}

bool Can_R_Thread_TS::isReadFrame(quint32 id)
{
    foreach (BO_* frame, readFrameList) {
        if(frame->m_id == id)
            return true;
    }
    return false;
}

void Can_R_Thread_TS::setFilterFrames(QList<BO_ *> frames)
{
    if(frames.isEmpty()) return;

    foreach (BO_* frame, frames) {
        quint32 id = frame->m_id;
    }
}

void Can_R_Thread_TS::setDeviceHandle(const size_t &value)
{
    deviceHandle = value;
}

void Can_R_Thread_TS::setCanIndex(const APP_CHANNEL &value)
{
    canIndex = value;
    //qDebug()<<"value:"<<(quint8)value<<", can index:"<<(quint8)canIndex;
}


#ifdef Q_OS_LINUX

#endif

void Can_R_Thread_TS::setMdfRecordOn(bool value)
{
    mdfRecordOn = value;
}


void Can_R_Thread_TS::setSmHash(const QHash<quint32, QSharedMemory *> &value)
{
    smHash = value;
}

void Can_R_Thread_TS::setModName(const QString &value)
{
    modName = value;
}

void Can_R_Thread_TS::setReadFrameList(const QList<BO_ *> &value)
{
    readFrameList = value;
}

void Can_R_Thread_TS::setCanType(const quint32 &value)
{
    canType = value;
}


/////////////////////////////////////////////////////////////////
Can_W_Thread_TS::Can_W_Thread_TS(QObject *parent)
{

}

void Can_W_Thread_TS::setIsStop(bool value)
{
    isStop = value;
}

void Can_W_Thread_TS::run()
{
    qDebug()<<"can snd thread num:"<<QThread::currentThreadId();
    time.start();

    quint64 startTime = time.elapsed();
    quint64 passTime = 0;

    init();

    while(!isStop)
    {
        passTime = (time.elapsed() - startTime);
        if(passTime > 10) passTime = 2;
        msleep((transRate - passTime));

        startTime = time.elapsed();                        // last cycle time stamp(ms)

        if(!packTransmitData()) continue;

#ifdef Q_OS_WIN
        if(!copySndDataFromSmToBuf()) continue;

        if(canType == 0)
        {
            int retValue = tscan_transmit_can_async(deviceHandle, &pCanDataSnd);
            if(retValue != 0)
            {
                //qDebug()<<"Transmit error:"<<writeFrame->m_id;
                continue;
            }
        }
        else
        {
            if(writeFrame->m_length <= 8)
            {
                int retValue = tscan_transmit_can_async(deviceHandle, &pCanDataSnd);
                if(retValue != 0)
                {
                    //qDebug()<<"Transmit error:"<<writeFrame->m_id;
                    continue;
                }
            }
            else
            {

                int retValue = tscan_transmit_canfd_async(deviceHandle, &pCanFDDataSnd);
                if(retValue != 0)
                {
                    //qDebug()<<"TransmitFD error:"<<writeFrame->m_id;
                    continue;
                }
            }

        }

#endif
#ifdef Q_OS_LINUX

#endif

        updateSndTimeStamp();
    }
}

void Can_W_Thread_TS::init()
{
    if(writeFrame == NULL || sm == NULL)
        return;

    char* buf = new char[writeFrame->m_length + 8];

    copyFromSharedMemory(sm, 0, buf, writeFrame->m_length + 8);

    for(int i = 0; i < writeFrame->m_sgList.count(); i++)
    {
        SG_ *sig = writeFrame->m_sgList.at(i);

        float sigValue = sig->m_value;
        if(sig->m_valType == 1)
        {
            quint32 sigData = (quint32)((sigValue - sig->m_offset) / sig->m_factor);
            canUserData_2_RawMoto((quint8*)(buf + 8), sig->m_startBit, sig->m_bitLen, sigData);
        }
        else if(sig->m_valType == 0)
        {
            qint32 sigData = (qint32)((sigValue - sig->m_offset) / sig->m_factor);
            canUserData_2_RawMoto((quint8*)(buf + 8), sig->m_startBit, sig->m_bitLen, sigData);
        }
    }

    copyToSharedMemory(sm, 0, buf, writeFrame->m_length + 8);

    delete[] buf;
}

bool Can_W_Thread_TS::packTransmitData()
{
    if(writeFrame == NULL || sm == NULL)
        return false;

    char* buf = new char[writeFrame->m_length + 8];

    copyFromSharedMemory(sm, 0, buf, writeFrame->m_length + 8);

    frameFromRawDataToValue(buf + 8, writeFrame);

    for(int i = 0; i < writeFrame->m_sgList.count(); i++)
    {
        SG_ *sig = writeFrame->m_sgList.at(i);

        // RC handle
        if(sig->m_name.contains("_RC"))
        {
            sigRC(sig, 0, 15, 1);
        }

        float sigValue = sig->m_value;
        if(sig->m_valType == 1)
        {
            quint32 sigData = (quint32)((sigValue - sig->m_offset) / sig->m_factor);
            canUserData_2_RawMoto((quint8*)(buf + 8), sig->m_startBit, sig->m_bitLen, sigData);
        }
        else if(sig->m_valType == 0)
        {
            qint32 sigData = (qint32)((sigValue - sig->m_offset) / sig->m_factor);
            canUserData_2_RawIntel((quint8*)(buf + 8), sig->m_startBit, sig->m_bitLen, sigData);
        }
    }

    // crc checksum
    for(int i = 0; i < crcInfoList.count(); i++)
    {
        CRC_INFO crcInfo = crcInfoList.at(i);
        dataCRC((buf + 8), crcInfo.startIndex, crcInfo.endIndex, crcInfo.crcIndex);
    }

    copyToSharedMemory(sm, 0, buf, writeFrame->m_length + 8);

    delete[] buf;

    return true;
}

void Can_W_Thread_TS::frameFromRawDataToValue(char *data, BO_ *frame)
{
    for(int j = 0; j < frame->m_sgList.count(); j++)
    {
        SG_ *sig = frame->m_sgList.at(j);
        float sigValue = 0;

        if(sig->m_valType == 0)
        {
            qint32 sigData = canRawMoto_2_UserData_Int((quint8*)data, sig->m_startBit, sig->m_bitLen);
            sigValue = sigData * sig->m_factor + sig->m_offset;
        }
        else if(sig->m_valType == 1)
        {
            quint32 sigData = canRawMoto_2_UserData((quint8*)data, sig->m_startBit, sig->m_bitLen);
            sigValue = sigData * sig->m_factor + sig->m_offset;
        }
        else if(sig->m_valType == 2)
        {

        }
        else if(sig->m_valType == 3)
        {

        }

        sig->m_value = sigValue;
    }
}

void Can_W_Thread_TS::sigRC(SG_ *sg, int min, int max, int step)
{
    sg->m_value += step;

    if(sg->m_value > max ) sg->m_value = min;
}

void Can_W_Thread_TS::dataCRC(char *data, int startIndex, int endIndex, int crcIndex)
{
    quint8 crcResult = 0;
    for(int i = startIndex; i < endIndex; i++)
    {
        if(i == crcIndex)
            continue;

        crcResult = crcResult ^ data[i];
    }

    data[crcIndex] = crcResult;
}

#ifdef Q_OS_WIN
bool Can_W_Thread_TS::copySndDataFromSmToBuf()
{
    if(writeFrame == NULL || sm == NULL)
        return false;

    if(canType == 0)
    {
        pCanDataSnd.FProperties.bits.extframe = 0;
        pCanDataSnd.FProperties.bits.iserrorframe = 0;
        pCanDataSnd.FProperties.bits.istx = 1;
        pCanDataSnd.FProperties.bits.remoteframe = 0;

        pCanDataSnd.FIdxChn = canIndex;

        pCanDataSnd.FIdentifier = writeFrame->m_id;
        pCanDataSnd.FDLC = writeFrame->m_length;

        if(!copyFromSharedMemory(sm, 8, (char*)(pCanDataSnd.FData), writeFrame->m_length))
            return false;
    }
    else
    {
        if(writeFrame->m_length <= 8)
        {
            pCanDataSnd.FProperties.bits.extframe = 0;
            pCanDataSnd.FProperties.bits.iserrorframe = 0;
            pCanDataSnd.FProperties.bits.istx = 1;
            pCanDataSnd.FProperties.bits.remoteframe = 0;

            pCanDataSnd.FIdxChn = canIndex;
            pCanDataSnd.FIdentifier = writeFrame->m_id;
            pCanDataSnd.FDLC = writeFrame->m_length;

            if(!copyFromSharedMemory(sm, 8, (char*)(pCanDataSnd.FData), writeFrame->m_length))
                return false;
        }
        else
        {
            pCanFDDataSnd.FProperties.value = 0x00;
            pCanFDDataSnd.FProperties.bits.extframe = 0;
            pCanFDDataSnd.FProperties.bits.iserrorframe = 0;
            pCanFDDataSnd.FProperties.bits.istx = 1;
            pCanFDDataSnd.FProperties.bits.remoteframe = 0;

            pCanFDDataSnd.FFDProperties.value = 0x00;
            pCanFDDataSnd.FFDProperties.bits.BRS = (canType == 2) ? 1 : 0;
            pCanFDDataSnd.FFDProperties.bits.EDL = 1;

            pCanFDDataSnd.FIdxChn = canIndex;
            pCanFDDataSnd.FIdentifier = writeFrame->m_id;
            //pCanFDDataSnd.FDLC = writeFrame->m_length;
            pCanFDDataSnd.FDLC = fromDlcToFDlc(writeFrame->m_length);

            if(!copyFromSharedMemory(sm, 8, (char*)(pCanFDDataSnd.FData), writeFrame->m_length))
                return false;
        }
    }
    return true;
}
#endif

void Can_W_Thread_TS::updateSndTimeStamp()
{
    int timeStamp = time.elapsed();  // timeStamp(ms)
    sndAbsTime = (qreal)timeStamp / 1000.0;    // us

    copyToSharedMemory(sm, 0, (char*)&sndAbsTime, 8);
}


void Can_W_Thread_TS::endHandle()
{
    /*
    if(pCanDataSnd)
    {
        delete pCanDataSnd;
        pCanDataSnd = NULL;
    }
    if(pCanFDDataSnd)
    {
        delete pCanFDDataSnd;
        pCanFDDataSnd = NULL;
    }
    */
}

void Can_W_Thread_TS::setDeviceHandle(const size_t &value)
{
    deviceHandle = value;
}

#ifdef Q_OS_LINUX

#endif

void Can_W_Thread_TS::setCanIndex(const APP_CHANNEL &value)
{
    canIndex = value;
}

void Can_W_Thread_TS::setSm(QSharedMemory *value)
{
    sm = value;
}

void Can_W_Thread_TS::setWriteFrame(BO_ *value)
{
    writeFrame = value;

    transRate = writeFrame->m_period;
    if(transRate == 0) transRate = 100;

    crcInfoList.clear();
    for(int j = 0; j < writeFrame->m_sgList.count(); j++)
    {
        SG_ *sig = writeFrame->m_sgList.at(j);

        if(sig->m_name.contains("_CRC"))
        {
            CRC_INFO crcInfo;
            crcInfo.crcIndex = sig->m_startBit / 8;
            crcInfo.startIndex = (crcInfo.crcIndex / 12) * 12 + 4;
            crcInfo.endIndex = crcInfo.startIndex + 7;
            //qDebug()<<"crcIndex:"<<crcInfo.crcIndex<<", start:"<<crcInfo.startIndex<<", end:"<<crcInfo.endIndex;

            crcInfoList.append(crcInfo);
        }

    }
}

void Can_W_Thread_TS::setSmHash(const QHash<quint32, QSharedMemory *> &value)
{
    smHash = value;
}

void Can_W_Thread_TS::setModName(const QString &value)
{
    modName = value;
}

void Can_W_Thread_TS::setWriteFrameList(const QList<BO_ *> &value)
{
    writeFrameList = value;
}

void Can_W_Thread_TS::setCanType(const quint32 &value)
{
    canType = value;
}


/////////////////////////////////////////////////////////////////
Can_Thread_TS::Can_Thread_TS(QObject *parent)
{

}

Can_Thread_TS::~Can_Thread_TS()
{
    if(canRcvThread)
    {
        canRcvThread->setIsStop(true);
        canRcvThread->quit();
        canRcvThread->wait();

        delete canRcvThread;
        canRcvThread = NULL;
        qDebug()<<"can rcv thread deleted.";
    }
    if(canSndThread)
    {
        canSndThread->setIsStop(true);
        canSndThread->quit();
        canSndThread->wait();

        delete canSndThread;
        canSndThread = NULL;
        qDebug()<<"can snd thread deleted.";
    }

    for(int i = 0; i < canSndThreadList.count(); i++)
    {
        Can_W_Thread_TS *thread = canSndThreadList.at(i);
        if(thread)
        {
            thread->setIsStop(true);
            thread->terminate();
            thread->wait();

            delete thread;
            thread = NULL;
            qDebug()<<"can snd thread list deleted."<<i;
        }
    }
    canSndThreadList.clear();

    if(deviceHandle)
    {
#ifdef Q_OS_WIN
        if(deviceHandle)
        {
            tscan_disconnect_by_handle(deviceHandle);
        }
#endif
#ifdef Q_OS_LINUX

#endif
    }
}

void Can_Thread_TS::run()
{
    qRegisterMetaType<Can_Thread_TS::CAN_STATE>("Can_Thread_TS::CAN_STATE");

    initTsApiObj();

    initConnection();

    curState = UNCONN;
    lastState = curState;

    while(!isStop)
    {
        msleep(100);

        if(curState != lastState)
        {
            emit stateChanged(canIndex, curState);
            lastState = curState;
        }

        switch (curState) {
        case UNCONN:
        {
#ifdef Q_OS_WIN
            qDebug()<<"Before connect, deviceHandle="<<deviceHandle;

            //connect device (can device)
            uint32_t retValue = tscan_connect("", &deviceHandle);
            if(retValue == 0 || retValue == 5)
            {
                qDebug()<<"connect device ok. handle:"<<deviceHandle;
            }
            else
            {
                qDebug()<<"connect device handle error,return value:"<<retValue;
            }

            curState = deviceHandle > 0 ? CONN_OK : CONN_ERR;
#endif

#ifdef Q_OS_LINUX

#endif
        }
            break;
        case CONN_OK:
        {
#ifdef Q_OS_WIN

#endif
#ifdef Q_OS_LINUX

#endif

#ifdef Q_OS_WIN
            //set config value
            quint32 ret = 0;

            canfd_abitBaudrate /= 1000;
            canfd_dbitBaudrate /= 1000;

            qDebug()<<"canType:"<<canType<<",a baudrate(kbps):"<<canfd_abitBaudrate<<"d baudrate(kbps):"<<canfd_dbitBaudrate<<", canIndex:"<<canIndex;
            if(canType > 0)
            {
                ret = tscan_config_canfd_by_baudrate(deviceHandle, canIndex, canfd_abitBaudrate, canfd_dbitBaudrate, (TLIBCANFDControllerType)canfd_standard, lfdmNormal, resistanceEnable);
                if(ret == 0)
                {
                    qDebug()<<"set canfd baudrate succeed.";
                }
                else
                {
                    qDebug()<<"set canfd baudrate fail, return value:"<<ret;
                }
            }
            else
            {
                ret = tscan_config_canfd_by_baudrate(deviceHandle, canIndex, canfd_abitBaudrate, canfd_abitBaudrate, lfdtCAN, lfdmNormal, resistanceEnable);
                if(ret == 0)
                {
                    qDebug()<<"set can baudrate succeed.";
                }
                else
                {
                    qDebug()<<"set can baudrate fail, return value:"<<ret;
                }
            }

            curState = ret == 0 ? SETVALUE_OK : SETVALUE_ERR;
#endif

#ifdef Q_OS_LINUX

#endif

        }
            break;
        case CONN_ERR:
        {
            qDebug()<<"Open error. [canIndex: "<<canIndex<<"]";
            curState = ERR_HANDLE;
        }
            break;
        case SETVALUE_OK:
        {
#ifdef Q_OS_WIN

            curState = START_OK;
#endif
#ifdef Q_OS_LINUX

#endif

        }
            break;
        case SETVALUE_ERR:
        {
            qDebug()<<"SetValue error, [canIndex: "<<canIndex<<"]";
            curState = ERR_HANDLE;
        }
            break;
        case START_OK:
        {
#ifdef Q_OS_WIN

#endif
#ifdef Q_OS_LINUX

#endif
            //action after start can ok and before read and write frames
            if(canRcvThread == NULL)
            {
                canRcvThread = new Can_R_Thread_TS(this);
            }

            canRcvThread->setDeviceHandle(this->deviceHandle);
            //qDebug()<<"can Index in can thread:"<<(quint8)canIndex;
            canRcvThread->setCanIndex(this->canIndex);

            canRcvThread->setCanType(this->canType);
            canRcvThread->setModName(this->modName);
            canRcvThread->setReadFrameList(this->readFrameList);
            canRcvThread->setSmHash(this->readSmHash);
            connect(canRcvThread, SIGNAL(canDataForRecord(quint8*,quint32,QString)), this, SIGNAL(canDataForRecord(quint8*,quint32,QString)));

            canRcvThread->start();

            for(int i = 0; i < writeFrameList.count(); i++)
            {
                BO_ *frame = writeFrameList.at(i);
                if(frame == NULL) continue;

                Can_W_Thread_TS *canSndThread = new Can_W_Thread_TS(this);

                canSndThread->setDeviceHandle(this->deviceHandle);
                canSndThread->setCanIndex(this->canIndex);

                canSndThread->setCanType(this->canType);
                canSndThread->setModName(this->modName);
                canSndThread->setWriteFrame(frame);
                canSndThread->setSm(this->writeSmHash.value(frame->m_id));

                canSndThread->start(QThread::HighPriority);

                canSndThreadList.append(canSndThread);
            }


            curState = R_W_ING;
        }
            break;
        case START_ERR:
        {
            qDebug()<<"Start can error. [canIndex: "<<canIndex<<"]";
            curState = ERR_HANDLE;
        }
            break;
        case R_W_ING:
        {
            if(canStop)
            {
                curState = STOP;
                break;
            }

            curState = R_W_ING;
        }
            break;
        case R_W_ERR:
        {
            qDebug()<<"Can read or write error. [canIndex: "<<canIndex<<"]";
            curState = ERR_HANDLE;
        }
            break;

        case ERR_HANDLE:
        {
            msleep(500);
            qDebug()<<"Error handle: redo open device";
            curState = STOP;
        }
            break;
        case STOP:
        {
            qDebug()<<"Can Thread in STOP";
            if(canRcvThread)
            {
                canRcvThread->setIsStop(true);
                canRcvThread->terminate();
                canRcvThread->wait();

                delete canRcvThread;
                canRcvThread = NULL;
            }
            if(canSndThread)
            {
                canSndThread->setIsStop(true);
                canSndThread->terminate();
                canSndThread->wait();

                delete canSndThread;
                canSndThread = NULL;
            }

            for(int i = 0; i < canSndThreadList.count(); i++)
            {
                Can_W_Thread_TS *thread = canSndThreadList.at(i);
                if(thread)
                {
                    thread->setIsStop(true);
                    thread->quit();
                    thread->wait();

                    delete thread;
                    thread = NULL;
                    qDebug()<<"can snd thread list deleted."<<i;
                }
            }
            canSndThreadList.clear();

            int retValue = 0;
            if(!stopWithoutClose)
            {
#ifdef Q_OS_WIN

#endif
#ifdef Q_OS_LINUX

#endif
            }
            if(deviceHandle != 0)
            {
                retValue = tscan_disconnect_by_handle(deviceHandle);
            }

            curState = retValue == 0 ? DISCONN_OK : DISCONN_ERR;

            deviceHandle = 0;

        }
            break;
        case DISCONN_OK:
        {
            qDebug()<<"Close device ok. [canIndex: "<<canIndex<<"]";
            curState = canStop ? END : UNCONN;
        }
            break;
        case DISCONN_ERR:
        {
            qDebug()<<"Close device error. [canIndex: "<<canIndex<<"]";

            curState = canStop ? END : ERR_HANDLE;
        }
            break;
        case END:
        {
            qDebug()<<"Can Thread in END";

            return;
            //msleep(500);
            //curState = canStop ? END : UNCONN;
        }
            break;
        default:
            break;
        }
    }

}


void Can_Thread_TS::initTsApiObj()
{
    initialize_lib_tscan(true, false, false);

    //扫描获取可用同星设备
    uint32_t deviceCount = 0;
    if(tscan_scan_devices(&deviceCount) == 0)
    {
        qDebug()<<"同星设备数量:"<<deviceCount;
    }

}

void Can_Thread_TS::initConnection()
{
    //create sharedmemory hash
    for(int i = 0; i < readFrameList.count(); i++)
    {
        BO_ *frame = readFrameList.at(i);
        if(!frame) continue;

        QSharedMemory *sm = new QSharedMemory();
        QString keyName = "RP_TSCAN" + QString::number(canIndex) + "_" + QString::number(frame->m_id);
        sm->setKey(keyName);
        sm->create((frame->m_length < 8 ? 8 : frame->m_length) + 8);

        frame->smKey = keyName;
        readFrameSmHash.insert(keyName, sm);

        readSmHash.insert(frame->m_id, sm);

    }

    for(int i = 0; i < writeFrameList.count(); i++)
    {
        BO_ *frame = writeFrameList.at(i);
        if(!frame) continue;

        QSharedMemory *sm = new QSharedMemory();
        QString keyName = "WP_TSCAN" + QString::number(canIndex) + "_" + QString::number(frame->m_id);
        sm->setKey(keyName);
        sm->create((frame->m_length < 8 ? 8 : frame->m_length) + 8);

        frame->smKey = keyName;
        writeFrameSmHash.insert(keyName, sm);

        writeSmHash.insert(frame->m_id, sm);
    }
}

#ifdef Q_OS_WIN
void Can_Thread_TS::setFiltersForFrames(QList<BO_ *> frames)
{
    for(int i = 0; i < frames.count(); i++)
    {
        BO_* frame = frames.at(i);
    }

}

QHash<QString, QSharedMemory *> Can_Thread_TS::getWriteFrameSmHash() const
{
    return writeFrameSmHash;
}

QHash<QString, QSharedMemory *> Can_Thread_TS::getReadFrameSmHash() const
{
    return readFrameSmHash;
}

size_t Can_Thread_TS::getDeviceHandle() const
{
    return deviceHandle;
}

void Can_Thread_TS::setDeviceHandle(const size_t &value)
{
    deviceHandle = value;
}
#endif

#ifdef Q_OS_LINUX


#endif

void Can_Thread_TS::setStopWithoutClose(bool value)
{
    stopWithoutClose = value;
}


Can_Thread_TS::CAN_STATE Can_Thread_TS::getCurState() const
{
    return curState;
}

void Can_Thread_TS::setWriteSmHash(const QHash<quint32, QSharedMemory *> &value)
{
    writeSmHash = value;
}

void Can_Thread_TS::setMdfRcdOn(bool flag)
{
    if(canRcvThread)
    {
        canRcvThread->setMdfRecordOn(flag);
    }
}

void Can_Thread_TS::setReadSmHash(const QHash<quint32, QSharedMemory *> &value)
{
    readSmHash = value;
}

void Can_Thread_TS::setModName(const QString &value)
{
    modName = value;
}

void Can_Thread_TS::setWriteFrameList(const QList<BO_ *> &value)
{
    writeFrameList = value;
}

void Can_Thread_TS::setReadFrameList(const QList<BO_ *> &value)
{
    readFrameList = value;
}

void Can_Thread_TS::setIsCustomBaud(bool value)
{
    isCustomBaud = value;
}

void Can_Thread_TS::setResistanceEnable(const quint32 &value)
{
    resistanceEnable = value;
}


void Can_Thread_TS::setCanfd_dbitBaudrate(const quint64 &value)
{
    canfd_dbitBaudrate = value;
}

void Can_Thread_TS::setCanfd_abitBaudrate(const quint32 &value)
{
    canfd_abitBaudrate = value;
}

void Can_Thread_TS::setCanfd_standard(const quint32 &value)
{
    canfd_standard = value;
}

void Can_Thread_TS::setCanType(const quint32 &value)
{
    canType = value;
    isCANFD = canType > 0 ? true : false;
}

void Can_Thread_TS::setCanIndex(const APP_CHANNEL &value)
{
    canIndex = value;
}


void Can_Thread_TS::setCanStop(bool value)
{
    canStop = value;
}

void Can_Thread_TS::setIsStop(bool value)
{
    isStop = value;
}

