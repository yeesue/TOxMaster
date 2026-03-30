#include "param.h"
#include <QDebug>

PARAM::PARAM(QObject *parent) : QObject(parent)
{

}

PARAM::~PARAM()
{

}

void PARAM::setValue(qreal value)
{
    this->Value = value;
    //qDebug()<<"In PARAM: Param_Name:"<<Name<<", set value:"<<value;

    if(pamModule == 1)
    {
        emit valueUpdated(this, this->Value);
    }
}

qreal PARAM::getValue()
{
    return this->Value;
}

void PARAM::setMapValue(int index_x, int index_y, qreal value)
{
    this->Value = value;
    qDebug()<<"In  Map PARAM: Param_Name:"<<Name<<", set value:"<<value;

    if(pamModule == 1)
    {
        emit mapValueUpdated(this, index_x, index_y, this->Value);
    }
}

void PARAM::setXcpCharValue(qreal value)
{
    if(pamModule != 1)
        return;

    this->Value = value;

    emit valueUpdated(this, this->Value);
}

void PARAM::updatePamValueInSM(qreal value)
{
    QSharedMemory sharedMemory;
    sharedMemory.setKey(smKey);
    if (!sharedMemory.isAttached())
    {
        if (!sharedMemory.attach())
        {
            qDebug()<<"Unable to attach SharedMemory";
            return;
        }
    }

    qreal pamValue = (value - ScalingOffset)/ScalingFactor;

    char *data = (char*)sharedMemory.data();
    sharedMemory.lock();
    value2IntelData(data, pamValue);
    sharedMemory.unlock();
}

qreal PARAM::getPamValueInSM()
{
    QSharedMemory sharedMemory;
    sharedMemory.setKey(smKey);
    if (!sharedMemory.isAttached())
    {
        if (!sharedMemory.attach())
        {
            qDebug()<<"Unable to attach SharedMemory";
            return 0;
        }
    }

    //qreal pamValue = (value - ScalingOffset)/ScalingFactor;

    char *data = (char*)sharedMemory.data();
    int lenByte = LengthBit / 8;
    char *valData = new char[lenByte];

    sharedMemory.lock();
    memcpy(valData, data, lenByte);
    sharedMemory.unlock();

    qreal pamValue = 0;

    return pamValue;

}


bool PARAM::isNedPam()
{
    return this->nedPam;
}

void PARAM::value2IntelData(char *data, qreal value)
{

    int startByte = StartBit / 8;
    int offset = StartBit % 8;

    bool val = (value > 0 ? true : false);
    quint8 val8 = val;
    if (val)
        val8 = val << offset;
    else
        val8 = rotateLeft(0xFE, offset);

    char *to = data;
    switch (LengthBit) {
    case 1:
        if (val)
            *(quint8*)(to + startByte) |= val8;
        else
            *(quint8*)(to + startByte) &= val8;
        break;
    case 8:
    {
        *(quint8*)(to + startByte) = (quint8)value;
        break;
    }
    case 16:
    {
        *(quint16*)(to + startByte) = (quint16)value;
        break;
    }
    case 32:
    {
        *(float*)(to + startByte) = (float)value;
        break;
    }
    case 64:
    {
        *(qreal*)(to + startByte) = (qreal)value;
        break;
    }
    default:
        break;
    }
}

quint8 PARAM::rotateLeft(quint8 val, quint8 n)
{
    n %= 8;
    if (n ==0)
        return val;
    return (val << n) | (val >> (8-n));
}


