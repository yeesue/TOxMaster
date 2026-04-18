#include "can_thread_zlg.h"
#include <QMetaType>

Can_R_Thread_ZLG::Can_R_Thread_ZLG(QObject *parent)
{


}

Can_R_Thread_ZLG::~Can_R_Thread_ZLG()
{

}

#ifdef Q_OS_WIN
void Can_R_Thread_ZLG::setChannelHandle(const CHANNEL_HANDLE &value)
{
    channelHandle = value;
}
#endif

void Can_R_Thread_ZLG::setIsStop(bool value)
{
    isStop = value;
}

void Can_R_Thread_ZLG::run()
{
    while(!isStop)
    {
        msleep(10);
#ifdef Q_OS_WIN
        BYTE type;
        if(canType == 0)
        {

            type = TYPE_CAN;
            quint32 rcvNum = ZCAN_GetReceiveNum(channelHandle, type);
            if(rcvNum > 0)
            {
                pCanDataRec = new ZCAN_Receive_Data[rcvNum];
                quint32 rcvNumReturn = ZCAN_Receive(channelHandle, pCanDataRec, rcvNum, 50);
                if(rcvNum == rcvNumReturn)
                {
                    rcvCanDataHandle(rcvNumReturn);

                }

                delete[] pCanDataRec;
            }
        }
        else
        {
            quint32 rcvNum_CAN = ZCAN_GetReceiveNum(channelHandle, TYPE_CAN);
            if(rcvNum_CAN > 0)
            {
                pCanDataRec = new ZCAN_Receive_Data[rcvNum_CAN];
                quint32 rcvNumReturn_CAN = ZCAN_Receive(channelHandle, pCanDataRec, rcvNum_CAN, 50);

                if(rcvNum_CAN == rcvNumReturn_CAN)
                {
                    rcvCanDataHandle(rcvNumReturn_CAN);
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
                    rcvCanFdDataHandle(rcvNumReturn_CANFD);
                }

                delete[] pCanFDDataRec;
            }
        }
#endif
#ifdef Q_OS_LINUX
        if(canType == 0)
        {
            quint32 bufSize = 20;
            pCanDataRec_ = new ZCAN_20_MSG[bufSize];
            quint32 rcvNumReturn = VCI_Receive(deviceType, deviceIndex, canIndex, pCanDataRec_, bufSize, 50);

            if(rcvNumReturn > 0)
            {
                rcvCanDataHandle(rcvNumReturn);
            }

            delete[] pCanDataRec_;
        }
        else
        {
            quint32 bufSize = 20;
            pCanDataRec_ = new ZCAN_20_MSG[bufSize];
            quint32 rcvNumReturn = VCI_Receive(deviceType, deviceIndex, canIndex, pCanDataRec_, bufSize, 50);

            if(rcvNumReturn > 0)
            {
                rcvCanDataHandle(rcvNumReturn);
            }

            delete[] pCanDataRec_;

            pCanFDDataRec_ = new ZCAN_FD_MSG[bufSize];
            U32 rcvNumReturn_CANFD = VCI_ReceiveFD(deviceType, deviceIndex, canIndex, pCanFDDataRec_, bufSize, 50);

            if(rcvNumReturn_CANFD > 0)
            {
                rcvCanFdDataHandle(rcvNumReturn_CANFD);
            }

            delete[] pCanFDDataRec_;
        }
#endif
    }
}

void Can_R_Thread_ZLG::rcvCanDataHandle(quint32 num)
{
#ifdef Q_OS_WIN
    if(num == 0 || pCanDataRec == NULL)
        return;
    for(int i = 0; i < num; i++)
    {
        ZCAN_Receive_Data zcanData = pCanDataRec[i];

        //qreal absTime = (qreal)zcanData.timestamp / 1000000.0;  //ZLG中的timestamp单位为1us
        quint64 timeStamp = (qreal)zcanData.timestamp * 10;  //ZLG中的timestamp单位为1us,转换成NI板卡的硬件时间100ns，统一处理

        if(mdfRecordOn)
        {
            quint32 bufSize = zcanData.frame.can_dlc + 8;
            QString mdfKeyName = modName + "_0x" + QString::number(zcanData.frame.can_id, 16);
            ByteArrayPtr buf = makeByteArray(bufSize);
            memcpy(buf.data(), (char*)&timeStamp, 8);
            memcpy(buf.data()+8, (char*)&(zcanData.frame.data), zcanData.frame.can_dlc);

            emit canDataForRecord(buf, bufSize, mdfKeyName);
        }

        quint32 id = zcanData.frame.can_id;
        QSharedMemory *sm = smHash.value(id);

        if(sm)
        {
            copyToSharedMemory_2Data(sm, 0, (char*)&timeStamp, 8, 8, (char*)(zcanData.frame.data), zcanData.frame.can_id);
        }

    }
#endif
#ifdef Q_OS_LINUX
    if(num == 0 || pCanDataRec_ == NULL)
        return;
    for(int i = 0; i < num; i++)
    {
        ZCAN_20_MSG zcanData = pCanDataRec_[i];

        qreal absTime = (qreal)zcanData.hdr.ts / 10000.0;  //ZLG Linux中的timestamp单位为0.1ms/100us
        if(mdfRecordOn)
        {
            quint32 bufSize = zcanData.hdr.len + 8;
            QString mdfKeyName = modName + "_0x" + QString::number(zcanData.hdr.id, 16);
            ByteArrayPtr buf = makeByteArray(bufSize);
            memcpy(buf.data(), (char*)&absTime, 8);
            memcpy(buf.data()+8, (char*)(zcanData.dat), zcanData.hdr.len);

            emit canDataForRecord(buf, bufSize, mdfKeyName);
        }

        quint32 id = zcanData.hdr.id;
        QSharedMemory *sm = smHash.value(id);

        if(copyToSharedMemory_2Data(sm, 0, (char*)&absTime, 8
                                 , 8, (char*)(zcanData.dat), zcanData.hdr.len))
            continue;

    }
#endif
}

void Can_R_Thread_ZLG::rcvCanFdDataHandle(quint32 num)
{
#ifdef Q_OS_WIN
    if(num == 0 || pCanFDDataRec == NULL)
        return;
    for(int i = 0; i < num; i++)
    {
        ZCAN_ReceiveFD_Data zcanFDData = pCanFDDataRec[i];

        //qreal absTime = (qreal)zcanFDData.timestamp / 1000000.0;  //ZLG中的timestamp单位为1us
        quint64 timeStamp = (qreal)zcanFDData.timestamp * 10;  //ZLG中的timestamp单位为1us,转换成NI板卡的硬件时间100ns，统一处理

        if(mdfRecordOn)
        {
            quint32 bufSize = zcanFDData.frame.len + 8;
            QString mdfKeyName = "RP_ZLGCAN" + QString::number(canIndex) + "_" + QString::number(zcanFDData.frame.can_id);
            ByteArrayPtr buf = makeByteArray(bufSize);
            memcpy(buf.data(), (char*)&timeStamp, 8);
            memcpy(buf.data()+8, (char*)&(zcanFDData.frame.data), zcanFDData.frame.len);

            emit canDataForRecord(buf, bufSize, mdfKeyName);
        }

        quint32 id = zcanFDData.frame.can_id;
        QSharedMemory *sm = smHash.value(id);

        if(sm)
        {
            copyToSharedMemory_2Data(sm, 0, (char*)&timeStamp, 8, 8, (char*)(zcanFDData.frame.data), zcanFDData.frame.len);
        }

    }
#endif
#ifdef Q_OS_LINUX
    if(num == 0 || pCanFDDataRec_ == NULL)
        return;
    for(int i = 0; i < num; i++)
    {
        ZCAN_FD_MSG zcanFDData = pCanFDDataRec_[i];

        qreal absTime = (qreal)zcanFDData.hdr.ts / 10000.0;  //ZLG Linux中的timestamp单位为 0.1ms/100us

        if(mdfRecordOn)
        {
            quint32 bufSize = zcanFDData.hdr.len + 8;
            QString mdfKeyName = modName + "_0x" + QString::number(zcanFDData.hdr.id, 16);
            ByteArrayPtr buf = makeByteArray(bufSize);
            memcpy(buf.data(), (char*)&absTime, 8);
            memcpy(buf.data()+8, (char*)&(zcanFDData.dat), zcanFDData.hdr.len);

            emit canDataForRecord(buf, bufSize, mdfKeyName);
        }

        quint32 id = zcanFDData.hdr.id;
        QSharedMemory *sm = smHash.value(id);
        if(copyToSharedMemory_2Data(sm, 0, (char*)&absTime, 8
                                 , 8, (char*)(zcanFDData.dat), zcanFDData.hdr.len))
            continue;
    }
#endif
}

void Can_R_Thread_ZLG::setCanIndex(const quint32 &value)
{
    canIndex = value;
}

void Can_R_Thread_ZLG::setDeviceIndex(const quint32 &value)
{
    deviceIndex = value;
}

void Can_R_Thread_ZLG::setDeviceType(const quint32 &value)
{
    deviceType = value;
}

void Can_R_Thread_ZLG::setMdfRecordOn(bool value)
{
    mdfRecordOn = value;
}


void Can_R_Thread_ZLG::setSmHash(const QHash<quint32, QSharedMemory *> &value)
{
    smHash = value;
}

void Can_R_Thread_ZLG::setModName(const QString &value)
{
    modName = value;
}

void Can_R_Thread_ZLG::setReadFrameList(const QList<BO_ *> &value)
{
    readFrameList = value;
}

void Can_R_Thread_ZLG::setCanType(const quint32 &value)
{
    canType = value;
}


/////////////////////////////////////////////////////////////////
Can_W_Thread_ZLG::Can_W_Thread_ZLG(QObject *parent)
{

}
#ifdef Q_OS_WIN
void Can_W_Thread_ZLG::setChannelHandle(const CHANNEL_HANDLE &value)
{
    channelHandle = value;
}
#endif
void Can_W_Thread_ZLG::setIsStop(bool value)
{
    isStop = value;
}


void Can_W_Thread_ZLG::run()
{
    //qDebug()<<"can snd thread num:"<<QThread::currentThreadId();
    time.start();
    quint64 startTime = time.elapsed();
    quint64 passTime = 0;

    init();

    while(!isStop)
    {

        //passTime = (time.elapsed() - startTime);
        //if(passTime > 10) passTime = 2;
        //msleep((transRate - passTime));
        //startTime = time.elapsed();                        // last cycle time stamp(ms)

        /*
        if(!remoteCtrl)
        {
            if(!packTransmitData()) continue;                // for remote control
        }

        if(!copySndDataFromSmToBuf()) continue;
        */

        usleep(transRate * 1000);
        packTransmitData_v2();

#ifdef Q_OS_WIN
        if(canType == 0)
        {
            quint32 sndNum = ZCAN_Transmit(channelHandle, &pCanDataSnd, 1);
            if(sndNum != 1)
            {
                //qDebug()<<"Transmit error:"<<writeFrame->m_id;
                continue;
            }
        }
        else
        {
            if(writeFrame->m_length <= 8)
            {
                quint32 sndNum = ZCAN_Transmit(channelHandle, &pCanDataSnd, 1);
                if(sndNum != 1)
                {
                    //qDebug()<<"Transmit error:"<<writeFrame->m_id;
                    continue;
                }
            }
            else
            {

                quint32 sndNum = ZCAN_TransmitFD(channelHandle, &pCanFDDataSnd, 1);
                if(sndNum != 1)
                {
                    //qDebug()<<"TransmitFD error:"<<writeFrame->m_id;
                    continue;
                }
                else
                {

                }
            }

        }
#endif
#ifdef Q_OS_LINUX
        if(canType == 0)
        {
            quint32 sndNum = VCI_Transmit(deviceType, deviceIndex, canIndex, &pCanDataSnd_, 1);
            if(sndNum != 1)
            {
                qDebug()<<"Transmit error:"<<writeFrame->m_id;
                continue;
            }
        }
        else
        {
            if(writeFrame->m_length <= 8)
            {
                quint32 sndNum = VCI_Transmit(deviceType, deviceIndex, canIndex, &pCanDataSnd_, 1);
                if(sndNum != 1)
                {
                    //qDebug()<<"Transmit error:"<<writeFrame->m_id;
                    continue;
                }
            }
            else
            {

                quint32 sndNum = VCI_TransmitFD(deviceType, deviceIndex, canIndex, &pCanFDDataSnd_, 1);
                if(sndNum != 1)
                {
                    //qDebug()<<"TransmitFD error:"<<writeFrame->m_id;
                    continue;
                }
                else
                {

                }
            }

        }
#endif

        updateSndTimeStamp();
    }
}

void Can_W_Thread_ZLG::init()
{
    if(writeFrame == NULL || sm == NULL)
        return;

    char* buf = new char[writeFrame->m_length + 8];

    copyFromSharedMemory(sm, 0, buf, writeFrame->m_length + 8);

    for(int i = 0; i < writeFrame->m_sgList.count(); i++)
    {
        SG_ *sig = writeFrame->m_sgList.at(i);

        float sigValue = sig->m_value;
        if(sig->m_valType == 1)  // unsigned
        {
            quint32 sigData = static_cast<quint32>((sigValue - sig->m_offset) / sig->m_factor);
            canUserData_2_RawMoto((quint8*)(buf + 8), sig->m_startBit, sig->m_bitLen, sigData);
        }
        else if(sig->m_valType == 0)  // signed
        {
            qint32 sigData = static_cast<qint32>((sigValue - sig->m_offset) / sig->m_factor);
            canUserData_2_RawMoto((quint8*)(buf + 8), sig->m_startBit, sig->m_bitLen, sigData);
        }
    }

    copyToSharedMemory(sm, 0, buf, writeFrame->m_length + 8);

    delete[] buf;
}

bool Can_W_Thread_ZLG::packTransmitData()
{
    if(writeFrame == NULL || sm == NULL)
        return false;

    char* buf = new char[writeFrame->m_length + 8];

    copyFromSharedMemory(sm, 0, buf, writeFrame->m_length + 8);

    for(int i = 0; i < writeFrame->m_sgList.count(); i++)
    {
        SG_ *sig = writeFrame->m_sgList.at(i);

        // RC handle
        if(sig->m_name.contains("_RC"))
        {
            sigRC(sig, 0, 15, 1);
        }

        float sigValue = sig->m_value;

        if(sig->m_valType == 0)
        {
            quint32 sigData = (quint32)((sigValue - sig->m_offset) / sig->m_factor);
            canUserData_2_RawMoto((quint8*)(buf + 8), sig->m_startBit, sig->m_bitLen, sigData);
        }
        else if(sig->m_valType == 1)
        {
            qint32 sigData = (qint32)((sigValue - sig->m_offset) / sig->m_factor);
            canUserData_2_RawMoto((quint8*)(buf + 8), sig->m_startBit, sig->m_bitLen, sigData);
        }

    }

    // crc checksum
    for(int i = 0; i < crcInfoList.count(); i++)
    {
        CRC_INFO crcInfo = crcInfoList.at(i);
        tableXorCRC((buf+8), crcInfo.startIndex, crcInfo.endIndex, crcInfo.crcIndex);
    }

    copyToSharedMemory(sm, 0, buf, writeFrame->m_length + 8);

    delete[] buf;

    return true;
}

void Can_W_Thread_ZLG::sigRC(SG_ *sg, int min, int max, int step)
{
    sg->m_value += step;

    if(sg->m_value > max ) sg->m_value = min;
}

void Can_W_Thread_ZLG::dataCRC(char *data, int startIndex, int endIndex, int crcIndex)
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

void Can_W_Thread_ZLG::tableXorCRC(char *data, int startIndex, int endIndex, int crcIndex)
{
    quint8 crcResult = 0x00;
    for(int i = startIndex; i <= endIndex; i++)
    {
        if(i == crcIndex)
            continue;

        crcResult = crcResult ^ data[i];
        crcResult = crc_Table[crcResult];
        //if(crcIndex == 4)
            //qDebug()<<i<<",crc:"<<crcResult;
    }

    data[crcIndex] = crcResult;
}

bool Can_W_Thread_ZLG::copySndDataFromSmToBuf()
{
    if(writeFrame == NULL || sm == NULL)
        return false;
#ifdef Q_OS_WIN
    if(canType == 0)
    {
        pCanDataSnd.transmit_type = 0;
        pCanDataSnd.frame.can_id = writeFrame->m_id;
        pCanDataSnd.frame.can_dlc = writeFrame->m_length;

        if(!copyFromSharedMemory(sm, 8, (char*)(pCanDataSnd.frame.data), pCanDataSnd.frame.can_dlc))
            return false;
    }
    else
    {
        if(writeFrame->m_length <= 8)
        {
            pCanDataSnd.transmit_type = 0;
            pCanDataSnd.frame.can_id = writeFrame->m_id;
            pCanDataSnd.frame.can_dlc = writeFrame->m_length;

            if(!copyFromSharedMemory(sm, 8, (char*)(pCanDataSnd.frame.data), pCanDataSnd.frame.can_dlc))
                return false;
        }
        else
        {
            pCanFDDataSnd.transmit_type = 0;
            pCanFDDataSnd.frame.can_id = writeFrame->m_id;
            pCanFDDataSnd.frame.len = writeFrame->m_length;

            if(!copyFromSharedMemory(sm, 8, (char*)(pCanFDDataSnd.frame.data), pCanFDDataSnd.frame.len))
                return false;
        }
    }
#endif
    return true;
}

void Can_W_Thread_ZLG::updateSndTimeStamp()
{
    int timeStamp = time.elapsed();  // timeStamp(ms)
    sndAbsTime = (qreal)timeStamp / 1000.0;

    copyToSharedMemory(sm, 0, (char*)&sndAbsTime, 8);
}


bool Can_W_Thread_ZLG::packTransmitData_v2()
{
    if(writeFrame == NULL || sm == NULL)
        return false;

    char* buf = new char[writeFrame->m_length + 8];

    copyFromSharedMemory(sm, 0, buf, writeFrame->m_length + 8);

    for(int i = 0; i < writeFrame->m_sgList.count(); i++)
    {
        SG_ *sig = writeFrame->m_sgList.at(i);

        // RC handle
        if(sig->m_name.contains("_RC"))
        {
            sigRC(sig, 0, 15, 1);

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

    }

    // crc checksum
    for(int i = 0; i < crcInfoList.count(); i++)
    {
        CRC_INFO crcInfo = crcInfoList.at(i);
        tableXorCRC((buf + 8), crcInfo.startIndex, crcInfo.endIndex, crcInfo.crcIndex);
    }

#ifdef Q_OS_WIN
    if(canType == 0)
    {
        pCanDataSnd.transmit_type = 0;
        pCanDataSnd.frame.can_id = writeFrame->m_id;
        pCanDataSnd.frame.can_dlc = writeFrame->m_length;

        memcpy((char*)pCanDataSnd.frame.data, buf+8,pCanDataSnd.frame.can_dlc);
    }
    else
    {
        if(writeFrame->m_length <= 8)
        {
            pCanDataSnd.transmit_type = 0;
            pCanDataSnd.frame.can_id = writeFrame->m_id;
            pCanDataSnd.frame.can_dlc = writeFrame->m_length;

            memcpy((char*)pCanDataSnd.frame.data, buf+8,pCanDataSnd.frame.can_dlc);
        }
        else
        {
            pCanFDDataSnd.transmit_type = 0;
            pCanFDDataSnd.frame.can_id = writeFrame->m_id;
            pCanFDDataSnd.frame.len = writeFrame->m_length;

            memcpy((char*)pCanFDDataSnd.frame.data, buf+8,pCanFDDataSnd.frame.len);
        }
    }
#endif
#ifdef Q_OS_LINUX
    if(canType == 0)
    {
        pCanDataSnd_.hdr.id = writeFrame->m_id;
        pCanDataSnd_.hdr.len = writeFrame->m_length;
        pCanDataSnd_.hdr.inf.brs = 0;
        pCanDataSnd_.hdr.inf.fmt = 0;
        pCanDataSnd_.hdr.inf.sef = writeFrame->m_extented > 0 ? 1 : 0;
        pCanDataSnd_.hdr.inf.sdf = 0;

        memcpy((char*)(pCanDataSnd_.dat), buf+8, pCanDataSnd_.hdr.len);
    }
    else
    {
        if(writeFrame->m_length <= 8)
        {
            pCanDataSnd_.hdr.id = writeFrame->m_id;
            pCanDataSnd_.hdr.len = writeFrame->m_length;
            pCanDataSnd_.hdr.inf.brs = 0;
            pCanDataSnd_.hdr.inf.fmt = 0;
            pCanDataSnd_.hdr.inf.sef = writeFrame->m_extented > 0 ? 1 : 0;
            pCanDataSnd_.hdr.inf.sdf = 0;

            memcpy((char*)(pCanDataSnd_.dat), buf+8, pCanDataSnd_.hdr.len);
        }
        else
        {
            pCanFDDataSnd_.hdr.id = writeFrame->m_id;
            pCanFDDataSnd_.hdr.len = writeFrame->m_length;
            pCanFDDataSnd_.hdr.inf.brs = 0;
            pCanFDDataSnd_.hdr.inf.fmt = 1;
            pCanFDDataSnd_.hdr.inf.sef = writeFrame->m_extented > 0 ? 1 : 0;
            pCanFDDataSnd_.hdr.inf.sdf = 0;

            memcpy((char*)pCanFDDataSnd_.dat, buf+8,pCanFDDataSnd_.hdr.len);
        }
    }
#endif
    delete[] buf;

    return true;
}

void Can_W_Thread_ZLG::setDeviceIndex(const quint32 &value)
{
    deviceIndex = value;
}

void Can_W_Thread_ZLG::setDeviceType(const quint32 &value)
{
    deviceType = value;
}

void Can_W_Thread_ZLG::setCanIndex(const quint32 &value)
{
    canIndex = value;
    //qDebug()<<"canType:"<<canType;
}

void Can_W_Thread_ZLG::setSm(QSharedMemory *value)
{
    sm = value;
}

void Can_W_Thread_ZLG::setWriteFrame(BO_ *value)
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

void Can_W_Thread_ZLG::setSmHash(const QHash<quint32, QSharedMemory *> &value)
{
    smHash = value;
}

void Can_W_Thread_ZLG::setModName(const QString &value)
{
    modName = value;
}

void Can_W_Thread_ZLG::setWriteFrameList(const QList<BO_ *> &value)
{
    writeFrameList = value;
}

void Can_W_Thread_ZLG::setCanType(const quint32 &value)
{
    canType = value;
}


/////////////////////////////////////////////////////////////////
Can_Thread_ZLG::Can_Thread_ZLG(QObject *parent)
{

}

Can_Thread_ZLG::~Can_Thread_ZLG()
{
    if(canRcvThread)
    {
        canRcvThread->setIsStop(true);
        canRcvThread->quit();
        canRcvThread->wait();

        delete canRcvThread;
        canRcvThread = NULL;
        //qDebug()<<"can rcv thread deleted.";
    }

    /*
    if(canSndThread)
    {
        canSndThread->setIsStop(true);
        canSndThread->quit();
        canSndThread->wait();

        delete canSndThread;
        canSndThread = NULL;
        //qDebug()<<"can snd thread deleted.";
    }
    */


    for(int i = 0; i < canSndThreadList.count(); i++)
    {
        Can_W_Thread_ZLG *thread = canSndThreadList.at(i);
        if(thread)
        {
            thread->setIsStop(true);
            thread->terminate();
            thread->wait();

            delete thread;
            thread = NULL;
            //qDebug()<<"can snd thread list deleted."<<i;
        }
    }
    canSndThreadList.clear();
#ifdef Q_OS_WIN
    if(deviceHandle)
    {
        ZCAN_CloseDevice(deviceHandle);
    }
#endif
}

void Can_Thread_ZLG::run()
{
    qRegisterMetaType<Can_Thread_ZLG::CAN_STATE>("Can_Thread_ZLG::CAN_STATE");
    qRegisterMetaType<quint32>("quint32");

    curState = UNOPEN;
    lastState = curState;

    while(!isStop)
    {
        msleep(100);

        if(curState != lastState)
        {
            emit stateChanged(deviceIndex, canIndex, curState);
            lastState = curState;
        }

        switch (curState) {
        case UNOPEN:
        {
            //qDebug()<<"Before Open, deviceHandle="<<deviceHandle;
            //open device (can device)
#ifdef Q_OS_WIN
            if(deviceHandle == INVALID_DEVICE_HANDLE)
            {

                deviceHandle = ZCAN_OpenDevice(deviceType, deviceIndex, 0);
                stopWithoutClose = false;
                qDebug()<<"open device handle:"<<deviceHandle;
            }
            else
            {
                stopWithoutClose = true;
                qDebug()<<"use exist device handle:"<<deviceHandle;
            }

            //qDebug()<<"open device handle:"<<deviceHandle;
            curState = (deviceHandle != INVALID_DEVICE_HANDLE) ? OPEN_OK : OPEN_ERR;
#endif

#ifdef Q_OS_LINUX
                U32 state = VCI_OpenDevice(deviceType, deviceIndex, 0);
                if(!state)
                {
                    qDebug()<<"VCI_OpenDevice fail, deviceType:"<<deviceType<<", deviceIndex:"<<deviceIndex;
                }
                else
                {
                    qDebug()<<"VCI_OpenDevice succeed, deviceType:"<<deviceType<<", deviceIndex:"<<deviceIndex;
                }
                curState = state ? OPEN_OK : OPEN_ERR;
#endif
        }
            break;
        case OPEN_OK:
        {
#ifdef Q_OS_WIN
            quint32 status = ZCAN_GetDeviceInf(deviceHandle, &deviceInfo);
            if(status)
            {
                qDebug()<<"Device Info:[============";
                qDebug()<<"Device info - can num:"<<deviceInfo.can_Num;
                qDebug()<<"Device info - hw type:"<<deviceInfo.str_hw_Type;
                qDebug()<<"Device info - serial num:"<<deviceInfo.str_Serial_Num;
                qDebug()<<"Device info - in version:"<<QString::number(deviceInfo.in_Version, 16);
                qDebug()<<"=========================]";
            }

            //set config value
            quint32 status1 = 1, status2 = 0, status3 = 0;

            if(isCustomBaud)
            {
                char *customBaud = "500Kbps(87%),2.0Mbps(80%),(60,03800005,01400002)";
                status1 = ZCAN_SetCustomBaudRate(deviceHandle, canIndex, customBaud);
                //qDebug()<<"set custom baudrate status:"<<status1;

                curState = SETVALUE_OK;
            }
            else
            {
                if(canType > 0)
                {
                    status1 = ZCAN_SetCANFDStandard(deviceHandle, canIndex, canfd_standard);
                    //qDebug()<<"set canfd standard status:"<<status1;
                }

                status2 = ZCAN_SetAbitBaud(deviceHandle, canIndex, canfd_abitBaudrate);
                //qDebug()<<"set canfd abit baudrate status:"<<status2;

                status3 = ZCAN_SetDbitBaud(deviceHandle, canIndex, canfd_dbitBaudrate);
                //qDebug()<<"set canfd dbit baudrate status:"<<status3;

                //curState = (status2 && status3) ? SETVALUE_OK : SETVALUE_ERR;
                curState = SETVALUE_OK;
            }
#endif
#ifdef Q_OS_LINUX
            ZCAN_DEV_INF info;
            U32 state  = VCI_ReadBoardInfo(deviceType, deviceIndex, &info);
            if(state)
            {
                qDebug()<<"Device Info:[============";
                qDebug()<<"Device info - hw version:0x"<<QString::number(info.hwv, 16);
                qDebug()<<"Device info - fw version:0x"<<QString::number(info.fwv, 16);
                qDebug()<<"Device info - drv version:0x"<<QString::number(info.drv, 16);
                qDebug()<<"Device info - in version:0x"<<QString::number(info.api, 16);
                qDebug()<<"Device info - irq version:"<<QString::number(info.irq);
                qDebug()<<"Device info - chn number:"<<QString::number(info.chn, 16);
                char sn[20];
                memcpy(sn, info.sn, 20);
                char id[40];
                memcpy(id, info.id, 40);
                qDebug()<<"Device info - id:"<<QString(id);
                qDebug()<<"=========================]";
            }

            curState = SETVALUE_OK;
#endif

        }
            break;
        case OPEN_ERR:
        {
            qDebug()<<"Open error. [deviceType:" <<deviceType<<"],[devIndex:"<<deviceIndex<<"], [canIndex:"<<canIndex<<"]";
            curState = ERR;
        }
            break;
        case SETVALUE_OK:
        {
#ifdef Q_OS_WIN
            //init can
            channelHandle =  ZCAN_InitCAN(deviceHandle, canIndex, &canPortInitConfig);
            qDebug()<<"Init can handle:"<<channelHandle;

            curState = (channelHandle != INVALID_CHANNEL_HANDLE) ? INIT_OK : INIT_ERR;
#endif
#ifdef Q_OS_LINUX
            //init can
            ZCAN_INIT zcanInit;
            zcanInit.clk = 60000000; // clk = 60M
            zcanInit.mode = 1; // 0 = listen only, 1 = ISO/Bosch

            setBaudrateAttr(&zcanInit, canfd_abitBaudrate, 0.8, canfd_dbitBaudrate, 0.8);

            U32 state =  VCI_InitCAN(deviceType, deviceIndex, canIndex, &zcanInit);
            if(!state)
            {
                qDebug()<<"VCI_InitCAN fail"<<canIndex;
            }
            else
            {
                qDebug()<<"VCI_InitCAN succeed!"<<canIndex;
            }
            curState = state ? INIT_OK : INIT_ERR;
#endif
        }
            break;
        case SETVALUE_ERR:
        {
            qDebug()<<"SetValue error. [devIndex: "<<deviceIndex<<"], [canIndex: "<<canIndex<<"]";
            curState = ERR;
        }
            break;
        case INIT_OK:
        {
#ifdef Q_OS_WIN
            qDebug()<<"Init CAN OK. [devIndex: "<<deviceIndex<<"], [canIndex: "<<canIndex<<"]";

            //set config value
            quint32 status = 0, status_filter = 0;
            status = ZCAN_SetResistanceEnable(channelHandle, canIndex, resistanceEnable);
            //qDebug()<<"set resistanceEnable status:"<<status;

            //setFiltersForFrames(readFrameList);

            //curState = status > 0 ? SETRES_OK : SETRES_ERR;
            curState = SETRES_OK;
#endif
#ifdef Q_OS_LINUX
            //set config value
            // 0x18 resistance
            U32 state = VCI_SetReference(deviceType, deviceIndex, canIndex, 0x18, &resistanceEnable);
            if(!state)
            {
                qDebug()<<"VCI_SetReference resistance fail"<<resistanceEnable;
            }
            else
            {
                qDebug()<<"VCI_SetReference resistance succeed! rsEnable:"<<resistanceEnable;
            }

            curState = SETRES_OK;
#endif
        }
            break;
        case INIT_ERR:
        {
            qDebug()<<"InitCAN error. [devIndex: "<<deviceIndex<<"], [canIndex: "<<canIndex<<"]";
            curState = ERR;
        }
            break;
        case SETRES_OK:
        {
#ifdef Q_OS_WIN
            //start can
            quint32 status = 0;
            status = ZCAN_StartCAN(channelHandle);
            //qDebug()<<"start can status:"<<status;

            curState = status > 0 ? START_OK : START_ERR;
#endif
#ifdef Q_OS_LINUX
            //start can
            U32 state = VCI_StartCAN(deviceType, deviceIndex, canIndex);
            if(!state)
            {
                qDebug()<<"VCI_StartCAN fail"<<canIndex;
            }
            else
            {
                qDebug()<<"VCI_StartCAN succeed!"<<canIndex;
            }
            curState = state > 0 ? START_OK : START_ERR;
#endif
        }
            break;
        case SETRES_ERR:
        {
            qDebug()<<"Set resistance error. [devIndex: "<<deviceIndex<<"], [canIndex: "<<canIndex<<"]";
            curState = ERR;
        }
            break;
        case START_OK:
        {
#ifdef Q_OS_WIN
            qDebug()<<"Start can ok! [devIndex: "<<deviceIndex<<"], [canIndex: "<<canIndex<<"]";
            ZCAN_ClearBuffer(channelHandle);

            //action after start can ok and before read and write frames
            if(canRcvThread == NULL)
            {
                canRcvThread = new Can_R_Thread_ZLG(this);
            }
            canRcvThread->setChannelHandle(channelHandle);
            canRcvThread->setCanType(canType);
            canRcvThread->setModName(this->modName);
            canRcvThread->setReadFrameList(this->readFrameList);
            canRcvThread->setSmHash(this->readSmHash);
            connect(canRcvThread, &Can_R_Thread_ZLG::canDataForRecord, this, &Can_Thread_ZLG::canDataForRecord);

            canRcvThread->start();

            for(int i = 0; i < writeFrameList.count(); i++)
            {
                BO_ *frame = writeFrameList.at(i);
                if(frame == NULL) continue;

                Can_W_Thread_ZLG *canSndThread = new Can_W_Thread_ZLG();

                canSndThread->setChannelHandle(this->channelHandle);
                canSndThread->setCanIndex(this->canIndex);
                quint8 canSndType = (canType > 0) ? 1 : 0;
                canSndThread->setCanType(canSndType);
                canSndThread->setModName(this->modName);
                canSndThread->setWriteFrame(frame);
                canSndThread->setSm(this->writeSmHash.value(frame->m_id));

                canSndThread->start(QThread::HighPriority);
                canSndThreadList.append(canSndThread);
            }

            curState = R_W_ING;
#endif
#ifdef Q_OS_LINUX
            qDebug()<<"Start can ok! [devIndex: "<<deviceIndex<<"], [canIndex: "<<canIndex<<"]";
            VCI_ClearBuffer(deviceType, deviceIndex, canIndex);

            //action after start can ok and before read and write frames
            if(canRcvThread == NULL)
            {
                canRcvThread = new Can_R_Thread_ZLG(this);
            }
            canRcvThread->setDeviceType(deviceType);
            canRcvThread->setDeviceIndex(deviceIndex);
            canRcvThread->setCanIndex(canIndex);

            canRcvThread->setCanType(canType);
            canRcvThread->setModName(this->modName);
            canRcvThread->setReadFrameList(this->readFrameList);
            canRcvThread->setSmHash(this->readSmHash);
            connect(canRcvThread, &Can_R_Thread_ZLG::canDataForRecord, this, &Can_Thread_ZLG::canDataForRecord);

            canRcvThread->start();


            for(int i = 0; i < writeFrameList.count(); i++)
            {
                BO_ *frame = writeFrameList.at(i);
                if(frame == NULL) continue;

                Can_W_Thread_ZLG *canSndThread = new Can_W_Thread_ZLG();

                canSndThread->setDeviceType(deviceType);
                canSndThread->setDeviceIndex(deviceIndex);
                canSndThread->setCanIndex(this->canIndex);

                quint8 canSndType = (canType > 0) ? 1 : 0;
                canSndThread->setCanType(canSndType);
                canSndThread->setModName(this->modName);
                canSndThread->setWriteFrame(frame);
                canSndThread->setSm(this->writeSmHash.value(frame->m_id));

                canSndThread->start(QThread::HighPriority);
                canSndThreadList.append(canSndThread);
            }

            curState = R_W_ING;
#endif
        }
            break;
        case START_ERR:
        {
#ifdef Q_OS_WIN
            qDebug()<<"Start can error. [devIndex: "<<deviceIndex<<"], [canIndex: "<<canIndex<<"]";
            readChannelErrInfo(channelHandle);
            curState = ERR;
#endif
#ifdef Q_OS_LINUX
            qDebug()<<"Start can error. [devIndex: "<<deviceIndex<<"], [canIndex: "<<canIndex<<"]";
            ZCAN_ERR_MSG errMsg;
            quint32 state = VCI_ReadErrInfo(deviceType, deviceIndex, canIndex, &errMsg);
            if(state)
            {
                qDebug()<<"errCode:"<<errMsg.hdr.id;
            }
            curState = ERR;
#endif
        }
            break;
        case R_W_ING:
        {
            if(canStop)
            {
                curState = STOP;
                break;
            }

            //read and write frames

            //write data

            //read data


            curState = R_W_ING;
        }
            break;
        case R_W_ERR:
        {
            qDebug()<<"Can read or write error. [devIndex: "<<deviceIndex<<"], [canIndex: "<<canIndex<<"]";
            curState = ERR;
        }
            break;

        case ERR:
        {
            msleep(500);
            qDebug()<<"Error handle: redo open device";
            curState = STOP;
        }
            break;
        case STOP:
        {
            //qDebug()<<"Can Thread in STOP";
            if(canRcvThread)
            {
                canRcvThread->setIsStop(true);
                canRcvThread->quit();
                canRcvThread->wait();

                delete canRcvThread;
                canRcvThread = NULL;
            }

            for(int i = 0; i < canSndThreadList.count(); i++)
            {
                Can_W_Thread_ZLG *thread = canSndThreadList.at(i);
                if(thread)
                {
                    thread->setIsStop(true);
                    thread->quit();
                    thread->wait();

                    delete thread;
                    thread = NULL;
                    //qDebug()<<"can snd thread list deleted."<<i;
                }
            }
            canSndThreadList.clear();

#ifdef Q_OS_WIN
            // Reset CAN or err handle
            if(channelHandle != INVALID_CHANNEL_HANDLE)
            {
                ZCAN_ClearBuffer(channelHandle);
                ZCAN_SetFilterClear(channelHandle, canIndex);

                quint32 status = ZCAN_ResetCAN(channelHandle);
                if(status == STATUS_OK)
                {
                    //qDebug()<<"Reset CAN OK";
                }
                else if(status == STATUS_ERR)
                {
                    qDebug()<<"Reset CAN Error";
                }

                channelHandle = INVALID_CHANNEL_HANDLE;
            }

            if(!stopWithoutClose && deviceHandle)
            {
                // close device
                quint32 status = ZCAN_CloseDevice(deviceHandle);
                if(status == STATUS_OK)
                {
                    //qDebug()<<"Close Device OK";
                    curState = CLOSE_OK;
                }
                else if(status == STATUS_ERR)
                {
                    qDebug()<<"Close Device Error";
                    curState = CLOSE_ERR;
                }

                deviceHandle = INVALID_DEVICE_HANDLE;
            }
            else
            {

            }
#endif
#ifdef Q_OS_LINUX
            U32 state = VCI_ResetCAN(deviceType, deviceIndex, canIndex);
            if(state)
            {
                //qDebug()<<"Reset CAN OK";
            }
            else
            {
                qDebug()<<"Reset CAN Error";
            }
            state = VCI_ClearBuffer(deviceType, deviceIndex, canIndex);

            if(!stopWithoutClose)
            {
                // close device
                state = VCI_CloseDevice(deviceType, deviceIndex);
                if(state)
                {
                    //qDebug()<<"Close Device OK";
                    curState = CLOSE_OK;
                }
                else
                {
                    qDebug()<<"Close Device Error";
                    curState = CLOSE_ERR;
                }
            }
            else
            {

            }
#endif

            curState = CLOSE_OK;

        }
            break;
        case CLOSE_OK:
        {
            qDebug()<<"Close device ok. [devIndex: "<<deviceIndex<<"], [canIndex: "<<canIndex<<"]";
            curState = canStop ? END : UNOPEN;
        }
            break;
        case CLOSE_ERR:
        {
            qDebug()<<"Close device error. [devIndex: "<<deviceIndex<<"], [canIndex: "<<canIndex<<"]";
            msleep(200);
            curState = canStop ? END : ERR;
        }
            break;
        case END:
        {
            //qDebug()<<"Can Thread in END";
            msleep(200);
            curState = canStop ? END : UNOPEN;
        }
            break;
        default:
            break;
        }
    }

}

void Can_Thread_ZLG::initConnection()
{
    //create sharedmemory hash
    for(int i = 0; i < readFrameList.count(); i++)
    {
        BO_ *frame = readFrameList.at(i);
        if(!frame) continue;

        QSharedMemory *sm = new QSharedMemory();
        QString keyName = "RP_ZLGCAN" + QString::number(canIndex) + "_" + QString::number(frame->m_id);
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
        QString keyName = "WP_ZLGCAN" + QString::number(canIndex) + "_" + QString::number(frame->m_id);
        sm->setKey(keyName);
        sm->create((frame->m_length < 8 ? 8 : frame->m_length) + 8);

        frame->smKey = keyName;
        writeFrameSmHash.insert(keyName, sm);

        writeSmHash.insert(frame->m_id, sm);
    }
}


void Can_Thread_ZLG::setFiltersForFrames(QList<BO_ *> frames)
{
#ifdef Q_OS_WIN
    for(int i = 0; i < frames.count(); i++)
    {
        BO_* frame = frames.at(i);

        ZCAN_SetFilter(channelHandle, canIndex, 0, frame->m_id, frame->m_id);
    }

    ZCAN_SetFilterAck(channelHandle, canIndex);
#endif
#ifdef Q_OS_LINUX

#endif
}

#ifdef Q_OS_WIN
void Can_Thread_ZLG::readChannelErrInfo(CHANNEL_HANDLE channelHandle)
{
    ZCAN_CHANNEL_ERR_INFO errInfo;

    ZCAN_ReadChannelErrInfo(channelHandle, &errInfo);

    qDebug()<<"Err Code:"<<errInfo.error_code;

}

QHash<QString, QSharedMemory *> Can_Thread_ZLG::getWriteFrameSmHash() const
{
    return writeFrameSmHash;
}

QHash<QString, QSharedMemory *> Can_Thread_ZLG::getReadFrameSmHash() const
{
    return readFrameSmHash;
}

#endif

#ifdef Q_OS_LINUX
void Can_Thread_ZLG::setBaudrateAttr(ZCAN_INIT *init, quint32 abit, float sp_abit, quint32 dbit, float sp_dbit)
{
    if(!init) return;

    quint8 tseg1_abit = 0, tseg2_abit = 0, sjw_abit = 0, smp_abit = 0;
    quint32 brp_abit = 0;

    quint8 tseg1_dbit = 0, tseg2_dbit = 0, sjw_dbit = 0, smp_dbit = 0;
    quint32 brp_dbit = 0;

    init->clk = 60000000; // USBCANFD-200U clock 60MHz
    init->mode = 1;  // 0 = listen only, 1 = ISO/Bosch

    brp_abit = 0;
    brp_dbit = 1;
    quint32 bitRateCount_abit = init->clk / (abit * (brp_abit+1));
    quint32 bitRateCount_dbit = init->clk / (dbit * (brp_dbit+1));

    sjw_abit = 2;
    sjw_dbit = 2;

    quint32 end_abit = bitRateCount_abit * 0.65;
    float sp_last = 1, sp_cur = 1;
    for(tseg2_abit = sjw_abit; tseg2_abit < end_abit; tseg2_abit++)
    {
        tseg1_abit = bitRateCount_abit - tseg2_abit - 3;
        sp_cur = (tseg1_abit + 2) / (tseg1_abit + tseg2_abit + 3);
        if(sp_abit >= sp_cur && sp_abit < sp_last)
        {
            break;
        }
        sp_last = sp_cur;
    }

    quint32 end_dbit = bitRateCount_dbit * 0.65;
    sp_last = 1, sp_cur = 1;
    for(tseg2_dbit = sjw_dbit; tseg2_dbit < end_dbit; tseg2_dbit++)
    {
        tseg1_dbit = bitRateCount_dbit - tseg2_dbit - 3;
        sp_cur = (tseg1_dbit + 2) / (tseg1_dbit + tseg2_dbit + 3);
        if(sp_dbit >= sp_cur && sp_dbit < sp_last)
        {
            break;
        }
        sp_last = sp_cur;
    }

    qDebug()<<"brp_abit="<<brp_abit<<", sjw_abit="<<sjw_abit<<", tseg1_abit="<<tseg1_abit<<", tseg2_abit="<<tseg2_abit;
    qDebug()<<"brp_dbit="<<brp_dbit<<", sjw_dbit="<<sjw_dbit<<", tseg1_dbit="<<tseg1_dbit<<", tseg2_dbit="<<tseg2_dbit;

    // 设置原则
    // 波特率 > 800K：75%　　波特率 > 500K：80%　　波特率 <= 500K：87.5%

    /*
    //baudrate setting convertion
    if(abit == 500000)
    {
        // 85% samplePoint
        brp_abit = 0;
        sjw_abit = 2;
        tseg1_abit = 94;
        tseg2_abit = 23;
    }

    if(dbit == 2000000)
    {
        // 80% samplePoint
        brp_dbit = 1;
        sjw_dbit = 2;
        tseg1_dbit = 10;
        tseg2_dbit = 2;
    }
    */

    init->aset.tseg1 = tseg1_abit;
    init->aset.tseg2 = tseg2_abit;
    init->aset.sjw = sjw_abit;
    init->aset.smp = smp_abit;
    init->aset.brp = brp_abit;

    init->dset.tseg1 = tseg1_dbit;
    init->dset.tseg2 = tseg2_dbit;
    init->dset.sjw = sjw_dbit;
    init->dset.smp = smp_dbit;
    init->dset.brp = brp_dbit;

}
#endif

void Can_Thread_ZLG::setStopWithoutClose(bool value)
{
    stopWithoutClose = value;
}

#ifdef Q_OS_WIN
DEVICE_HANDLE Can_Thread_ZLG::getDeviceHandle() const
{
    return deviceHandle;
}

void Can_Thread_ZLG::setDeviceHandle(const DEVICE_HANDLE &value)
{
    deviceHandle = value;
}
#endif

Can_Thread_ZLG::CAN_STATE Can_Thread_ZLG::getCurState() const
{
    return curState;
}

void Can_Thread_ZLG::setWriteSmHash(const QHash<quint32, QSharedMemory *> &value)
{
    writeSmHash = value;
}

void Can_Thread_ZLG::setMdfRcdOn(bool flag)
{
    if(canRcvThread)
    {
        canRcvThread->setMdfRecordOn(flag);
    }
}

void Can_Thread_ZLG::setReadSmHash(const QHash<quint32, QSharedMemory *> &value)
{
    readSmHash = value;
}

void Can_Thread_ZLG::setModName(const QString &value)
{
    modName = value;
}

void Can_Thread_ZLG::setWriteFrameList(const QList<BO_ *> &value)
{
    writeFrameList = value;
}

void Can_Thread_ZLG::setReadFrameList(const QList<BO_ *> &value)
{
    readFrameList = value;
}

void Can_Thread_ZLG::setIsCustomBaud(bool value)
{
    isCustomBaud = value;
}

void Can_Thread_ZLG::setResistanceEnable(const quint32 &value)
{
    resistanceEnable = value;
}

#ifdef Q_OS_WIN
void Can_Thread_ZLG::setCanPortInitConfig(const ZCAN_CHANNEL_INIT_CONFIG &value)
{
    canPortInitConfig = value;
}
#endif
void Can_Thread_ZLG::setCanfd_dbitBaudrate(const quint32 &value)
{
    canfd_dbitBaudrate = value;
}

void Can_Thread_ZLG::setCanfd_abitBaudrate(const quint32 &value)
{
    canfd_abitBaudrate = value;
}

void Can_Thread_ZLG::setCanfd_standard(const quint32 &value)
{
    canfd_standard = value;
}

void Can_Thread_ZLG::setCanType(const quint32 &value)
{
    canType = value;
    isCANFD = canType > 0 ? true : false;
}

void Can_Thread_ZLG::setCanIndex(const quint32 &value)
{
    canIndex = value;
}

void Can_Thread_ZLG::setDeviceIndex(const quint32 &value)
{
    deviceIndex = value;
}

void Can_Thread_ZLG::setDeviceType(const quint32 &value)
{
    deviceType = value;
}

void Can_Thread_ZLG::setCanStop(bool value)
{
    canStop = value;
}

void Can_Thread_ZLG::setIsStop(bool value)
{
    isStop = value;
}
