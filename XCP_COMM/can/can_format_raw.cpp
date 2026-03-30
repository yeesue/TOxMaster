#include "can_format_raw.h"
#include <QDebug>

typedef union {
    quint64 data;
    quint8  buf[8];
}can_data_format;

#define CAN_FORMAT_START_BIT_M2I(sb) ((sb)%8 + (7 - (sb)/8)*8)
#define CAN_FORMAT_START_BIT_I2M(sb) CAN_FORMAT_START_BIT_M2I(sb)
//#define CAN_FORMAT_START_BIT_M2I(sb, lb) sb - (sb%8) - (lb - 8 + sb%8)


int can_format_motorola2userdata(quint64 srcData, quint8 StartBit, quint32 LenBit, quint64 *dstData)
{
    quint8 intel_StartBit = 0;
    quint8 maxLenBit = 0;
    quint64 mask = 0;
    quint64 result = 0;
    can_data_format CAN_Data;

    maxLenBit = (8*(StartBit/8)+(8 - StartBit%8));
    if (LenBit > maxLenBit)
        return 0;

    intel_StartBit = CAN_FORMAT_START_BIT_M2I(StartBit);

    CAN_Data.data = srcData;

    for (int i = 0; i < 4; i++)
    {
        byte_switch(&CAN_Data.buf[i], &CAN_Data.buf[7-i]);
    }

    for (int i = 0; i< LenBit; i++)
    {
        mask |= 1 << i;
    }

    result = (CAN_Data.data >> intel_StartBit) & mask;

    *dstData = result;

    return 1;

}

int can_format_intel2userdata(quint64 srcData, quint8 StartBit, quint32 LenBit, quint64 *dstData)
{
    quint64 mask = 0;
    quint64 result = 0;
    can_data_format CAN_Data;

    if ((StartBit+LenBit) > 64)
        return 0;


    for (int i = 0; i< LenBit; i++)
    {
        mask |= 1 << i;
    }

    result = (srcData >> StartBit) & mask;

    *dstData = result;

    return 1;
}

int can_format_userdata2motorola(quint64 srcData, quint8 StartBit, quint32 LenBit, quint64 *dstData)
{
    quint8 moto_StartBit = 0;
    quint64 mask = 0;
    quint64 result = *dstData;
    can_data_format CAN_Data;
    quint8 maxLenBit = 0;

    maxLenBit = (8*(StartBit/8)+(8 - StartBit%8));
    if (LenBit > maxLenBit)
        return 0;

    moto_StartBit = CAN_FORMAT_START_BIT_I2M(StartBit);

    CAN_Data.data = 0;

    for (int i = 0; i< LenBit; i++)
    {
        mask |= 1 << i;
    }

    CAN_Data.data = (srcData & mask) << moto_StartBit;

    for (int i = 0; i < 4; i++)
    {
        byte_switch(&CAN_Data.buf[i], &CAN_Data.buf[7-i]);
    }

    result |= CAN_Data.data;

    *dstData = result;

    return 1;

}

int can_format_userdata2intel(quint64 srcData, quint8 StartBit, quint32 LenBit, quint64 *dstData)
{
    quint64 mask = 0;
    quint64 result = *dstData;

    if ((StartBit+LenBit) > 64)
        return 0;

    for (int i = 0; i< LenBit; i++)
    {
        mask |= 1 << i;
    }

    result |= (srcData & mask) << StartBit;

    *dstData = result;

    return 1;
}

void byte_switch(quint8 *byte1, quint8 *byte2)
{
    quint8 temp = *byte1;
    *byte1 = *byte2;
    *byte2 = temp;
}

int can_format_userdata2motorola_2(quint64 srcData, quint8 StartBit, quint32 LenBit, quint64 *dstData)
{
    quint8 moto_StartBit = 0;
    quint64 mask = 0;
    quint64 result = *dstData;
    can_data_format CAN_Data;
    quint8 maxLenBit = 0;

    maxLenBit = (8*(StartBit/8)+(8 - StartBit%8));
    if (LenBit > maxLenBit)
        return 0;

    moto_StartBit = CAN_FORMAT_START_BIT_I2M(StartBit);

    CAN_Data.data = 0;

    for (int i = 0; i< LenBit; i++)
    {
        mask |= 1 << i;
    }

    CAN_Data.data = (srcData & mask) << moto_StartBit;

    for (int i = 0; i < 4; i++)
    {
        byte_switch(&CAN_Data.buf[i], &CAN_Data.buf[7-i]);
    }

    result |= CAN_Data.data;

    *dstData = result;

    return 1;
}

void can_format_motorola2intel_rawdata(quint64 srcData, quint64 *dstData)
{
    can_data_format CAN_Data;
    CAN_Data.data = srcData;
    for (int i = 0; i < 4; i++)
    {
        byte_switch(&CAN_Data.buf[i], &CAN_Data.buf[7-i]);
    }
    *dstData = CAN_Data.data;
}

void can_format_motorola2intel_sb_lb(quint16 *StartBit, quint16 *LenBit)
{
    quint8 intel_StartBit = 0;
    quint8 maxLenBit = 0;

    maxLenBit = (8*(*StartBit/8)+(8 - *StartBit%8));
    if (*LenBit > maxLenBit)
        return;

    intel_StartBit = CAN_FORMAT_START_BIT_M2I(*StartBit);
    *StartBit = intel_StartBit;
}

void can_format_motorola2mdf_sb_lb(quint16 &StartBit, quint16 LenBit)
{
    quint8 maxLenBit = 0;
    maxLenBit = (8*(StartBit/8)+(8 - StartBit%8));
    if (LenBit > maxLenBit)
        return;

    quint16 lsbBit = StartBit & 7;
    quint16 bitsGap = (((lsbBit + LenBit - 1) >> 3) * 8);
    //qDebug()<<"bitsGap:"<<bitsGap;

    StartBit -= bitsGap;
}

quint32 CanRawMoto2UIntUserData(quint8 *raw, quint16 sb, quint16 lb)
{
    quint8 lsbbit = 0;
    quint8 lsbbyte = 0;
    quint8 msbbyte = 0;

    quint8 index = 0;
    quint32 dataMerge = 0, dataValue = 0;

    lsbbit = sb & 7;
    lsbbyte = sb >> 3;

    msbbyte = lsbbyte - ((lsbbit + lb - 1) >> 3);

    for(index = msbbyte; index < (lsbbyte + 1); index++)
    {
        dataMerge += raw[index] << ((lsbbyte - index) << 3);
    }

    dataValue = dataMerge >> lsbbit;
    dataValue = dataValue & (((quint64)1 << lb) - 1);

    return dataValue;
}

qint32 CanRawMoto2IntUserData(quint8 *raw, quint16 sb, quint16 lb)
{
    quint8 lsbbit = 0;
    quint8 lsbbyte = 0;
    quint8 msbbyte = 0;

    quint8 index = 0;
    qint32 dataMerge = 0, dataValue = 0;

    lsbbit = sb & 7;
    lsbbyte = sb >> 3;

    msbbyte = lsbbyte - ((lsbbit + lb - 1) >> 3);

    for(index = msbbyte; index < (lsbbyte + 1); index++)
    {
        dataMerge += raw[index] << ((lsbbyte - index) << 3);


    }

    //if(lb == 32)
        //qDebug()<<"dataMerge1:"<<dataMerge;

    dataValue = dataMerge >> lsbbit;
    //if(lb == 32)
        //qDebug()<<"dataValue1:"<<dataValue;

    dataValue = dataValue & (((quint64)1 << lb) - 1);

    //if(lb == 32)
        //qDebug()<<"dataValue2:"<<dataValue;

    return dataValue;
}

quint32 CanRawIntel2UIntUserData(quint8 *raw, quint16 sb, quint16 lb)
{
    quint8 lsbbit = 0;
    quint8 lsbbyte = 0;
    quint8 msbbyte = 0;

     quint8 index = 0;
    quint32 dataMerge = 0, dataValue = 0;

    lsbbit = sb & 7;
    lsbbyte = sb >> 3;

    msbbyte = lsbbyte + ((lsbbit + lb - 1) >> 3);

    for(index = lsbbyte; index < (msbbyte + 1); index++)
    {
        dataMerge += raw[index] << (index << 3);
    }

    dataValue = dataMerge >> lsbbit;
    dataValue = dataValue & ((1 << lb) - 1);

    return dataValue;
}

qint32 CanRawIntel2IntUserData(quint8 *raw, quint16 sb, quint16 lb)
{
    quint8 lsbbit = 0;
    quint8 lsbbyte = 0;
    quint8 msbbyte = 0;

     quint8 index = 0;
    qint32 dataMerge = 0, dataValue = 0;

    lsbbit = sb & 7;
    lsbbyte = sb >> 3;

    msbbyte = lsbbyte + ((lsbbit + lb - 1) >> 3);

    for(index = lsbbyte; index < (msbbyte + 1); index++)
    {
        dataMerge += raw[index] << (index << 3);
    }

    dataValue = dataMerge >> lsbbit;
    dataValue = dataValue & ((1 << lb) - 1);

    return dataValue;
}


