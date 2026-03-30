#include "common.h"
#include <QDebug>

bool copyToSharedMemory(QSharedMemory *sm, int index, char *srcData, quint16 len)
{
    if(!sm || !srcData) return false;

    if(!sm->isAttached())
    {
        if(!sm->attach())
        {
            //qDebug()<<"unable attach to sharedmemory.";
            return false;
        }
    }

    sm->lock();
    memcpy((char*)sm->data() + index, srcData, len);
    sm->unlock();

    return true;
}

bool copyToSharedMemory_2Data(QSharedMemory *sm, int index1, char *srcData1, quint16 len1, int index2, char *srcData2, quint16 len2)
{
    if(!sm || !srcData1 || !srcData2) return false;

    if(!sm->isAttached())
    {
        if(!sm->attach())
        {
            //qDebug()<<"unable attach to sharedmemory.";
            return false;
        }
    }

    sm->lock();
    memcpy((char*)sm->data() + index1, srcData1, len1);
    memcpy((char*)sm->data() + index2, srcData2, len2);
    sm->unlock();

    return true;
}

bool copyFromSharedMemory(QSharedMemory *sm, int index, char *dstData, quint16 len)
{
    if(!sm || !dstData) return false;

    if(!sm->isAttached())
    {
        if(!sm->attach())
        {
            //qDebug()<<"unable attach to sharedmemory.";
            return false;
        }
    }

    sm->lock();
    memcpy(dstData, (char*)sm->data() + index, len);
    sm->unlock();

    return true;
}

bool copyFromSharedMemory_2Data(QSharedMemory *sm, int index1, char *dstData1, quint16 len1, int index2, char *dstData2, quint16 len2)
{
    if(!sm || !dstData1 || !dstData2) return false;

    if(!sm->isAttached())
    {
        if(!sm->attach())
        {
            //qDebug()<<"unable attach to sharedmemory.";
            return false;
        }
    }

    sm->lock();
    memcpy(dstData1, (char*)sm->data() + index1, len1);
    memcpy(dstData2, (char*)sm->data() + index2, len2);
    sm->unlock();

    return true;
}

quint32 canRawMoto_2_UserData(quint8 *raw, quint16 sb, quint8 lb)
{
    quint8 lsbbit = 0;
    quint8 lsbbyte = 0;
    quint8 msbbyte = 0;
    quint8 index = 0;
    quint32 dataMerge = 0, dataValue = 0;

    lsbbit = sb & 7;
    lsbbyte = sb >> 3;

    msbbyte = lsbbyte - ((lsbbit + lb -1) >> 3);

    for(index = msbbyte; index < (lsbbyte + 1); index++)
    {
        dataMerge += raw[index] << ((lsbbyte - index) << 3);
    }
    dataValue = dataMerge >> lsbbit;
    dataValue = dataValue & ((1 << lb) - 1);

    return dataValue;
}

qint32  canRawMoto_2_UserData_Int(quint8 *raw, quint16 sb, quint8 lb)
{
    quint8 lsbbit = 0;
    quint8 lsbbyte = 0;
    quint8 msbbyte = 0;
    quint8 index = 0;
    qint32 dataMerge = 0, dataValue = 0;

    lsbbit = sb & 7;
    lsbbyte = sb >> 3;

    msbbyte = lsbbyte - ((lsbbit + lb -1) >> 3);

    for(index = msbbyte; index < (lsbbyte + 1); index++)
    {
        dataMerge += raw[index] << ((lsbbyte - index) << 3);
    }
    dataValue = dataMerge >> lsbbit;
    dataValue = dataValue & ((1 << lb) - 1);

    return dataValue;
}

quint32 canRawIntel_2_UserData(quint8 *raw, quint8 sb, quint8 lb)
{
    quint8 lsbbit = 0;
    quint8 lsbbyte = 0;
    quint8 msbbyte = 0;
    quint8 index = 0;
    quint32 dataMerge = 0, dataValue = 0;

    lsbbit = sb & 7;
    lsbbyte = sb >> 3;

    msbbyte = lsbbyte + ((lsbbit + lb -1) >> 3);

    for(index = lsbbyte; index < (msbbyte + 1); index++)
    {
        dataMerge += raw[index] << (index << 3);
    }
    dataValue = dataMerge >> lsbbit;
    dataValue = dataValue & ((1 << lb) - 1);

    return dataValue;
}

quint32 canUserData_2_RawMoto(quint8 *raw, quint16 sb, quint8 lb, quint32 data)
{
    quint8 lsbbit = 0;
    quint8 lsbbyte = 0;
    quint8 msbbit = 0;
    quint8 msbbyte = 0;
    quint8 index = 0;
    quint32 dataValue = 0;

    lsbbit = sb & 7;
    lsbbyte = sb >> 3;

    msbbit = (lb + lsbbit) & 7;
    msbbyte = lsbbyte - ((lsbbit + lb -1) >> 3);
    //qDebug()<<"lsbbit:"<<lsbbit<<", lsbbyte:"<<lsbbyte<<", msbbit:"<<msbbit<<", msbbyte:"<<msbbyte;

    dataValue = data;

    quint8 byte_l = raw[lsbbyte];
    byte_l = byte_l & ((1 << lsbbit) - 1);
    dataValue = dataValue << lsbbit;
    dataValue = dataValue | byte_l;

    quint8 byte_b = raw[msbbyte];
    byte_b = byte_b & (~((0x01 << msbbit) - 1));
    dataValue = dataValue | (byte_b << ((lsbbyte - msbbyte) << 3));

    //qDebug()<<"dataValue:"<<dataValue<<", byte_l:"<<byte_l<<", byte_b:"<<byte_b;

    for(index = msbbyte; index < (lsbbyte + 1); index++)
    {
        raw[index] = (quint8)(dataValue >> ((lsbbyte - index) << 3));
        //qDebug()<<"raw index:"<<index<<", data:"<<raw[index];
    }

    return 1;
}

quint32 canUserData_2_RawMoto(quint8 *raw, quint16 sb, quint8 lb, qint32 data)
{
    quint8 lsbbit = 0;
    quint8 lsbbyte = 0;
    quint8 msbbit = 0;
    quint8 msbbyte = 0;
    quint8 index = 0;
    qint32 dataValue = 0;

    lsbbit = sb & 7;
    lsbbyte = sb >> 3;

    msbbit = (lb + lsbbit) & 7;
    msbbyte = lsbbyte - ((lsbbit + lb -1) >> 3);

    dataValue = data;

    quint8 byte_l = raw[lsbbyte];
    byte_l = byte_l & ((1 << lsbbit) - 1);
    dataValue = dataValue << lsbbit;
    dataValue = dataValue | byte_l;

    quint8 byte_b = raw[msbbyte];
    byte_b = byte_b & (~((0x01 << msbbit) - 1));
    dataValue = dataValue | (byte_b << ((lsbbyte - msbbyte) << 3));

    //qDebug()<<"lsbbit:"<<lsbbit<<", lsbbyte:"<<lsbbyte<<", msbbit:"<<msbbit<<", msbbyte:"<<msbbyte;

    for(index = msbbyte; index < (lsbbyte + 1); index++)
    {
        raw[index] = (quint8)(dataValue >> ((lsbbyte - index) << 3));
    }

    return 1;
}

quint32 canUserData_2_RawIntel(quint8 *raw, quint16 sb, quint8 lb, quint32 data)
{
    quint8 lsbbit = 0;
    quint8 lsbbyte = 0;
    quint8 msbbit = 0;
    quint8 msbbyte = 0;
    quint8 index = 0;
    quint32 dataValue = 0;

    lsbbit = sb & 7;
    lsbbyte = sb >> 3;

    msbbit = (lb + lsbbit) & 7;
    msbbyte = lsbbyte + ((lsbbit + lb -1) >> 3);

    dataValue = data;

    quint8 byte_l = raw[lsbbyte];
    byte_l = byte_l & ((1 << lsbbit) - 1);
    dataValue = dataValue << lsbbit;
    dataValue = dataValue | byte_l;

    quint8 byte_b = raw[msbbyte];
    byte_b = byte_b & (~((0x01 << msbbit) - 1));
    dataValue = dataValue | (byte_b << ((msbbyte - lsbbyte) << 3));

    for(index = lsbbyte; index < (msbbyte + 1); index++)
    {
        raw[index] = (quint8)(dataValue >> ((lsbbyte - index) << 3));
    }

    return 1;
}

quint16 canSigMotoSbConvToMdfSb(quint16 sb, quint8 lb)
{
    quint8 lsbbit = 0;
    quint8 lsbbyte = 0;
    quint8 msbbyte = 0;
    quint16 mdfSb = 0;

    lsbbit = sb & 7;
    lsbbyte = sb >> 3;

    msbbyte = lsbbyte - ((lsbbit + lb -1) >> 3);

    mdfSb = msbbyte * 8 + lsbbit;

    return mdfSb;
}
