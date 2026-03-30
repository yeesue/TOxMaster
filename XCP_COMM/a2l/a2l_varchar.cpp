#include "a2l_varchar.h"

A2L_VarChar::A2L_VarChar()
{

}

A2L_VarChar::~A2L_VarChar()
{

}

QList<qreal> A2L_VarChar::getBlkValueList()
{
    return this->Blk_Values;
}


void A2L_VarChar::setBlkValue(int startIndex, int count, qreal value)
{
    if (count == 0 || startIndex >= number_BLK)
        return;
    for (int i = 0; i < count; i++)
    {
        Blk_Values.replace(startIndex+i, value);
    }
}

void A2L_VarChar::curveCharValueChangedSlot(int startIndex, int count, qreal data)
{
    if (count == 0 || startIndex >= zCount)
        return;
    for (int i = 0; i < count; i++)
    {
        hexValue_ZList.replace(startIndex+i, data);
        emit curveCharZValueChanged(this, startIndex+i, data);
    }
}

/*
qreal A2L_VarChar::getValue()
{
    return this->Value;
}
*/

QList<qreal> A2L_VarChar::getZValueList()
{
    return hexValue_ZList;
}

QList<qreal> A2L_VarChar::getXValueList()
{
    return hexValue_XList;
}

QList<qreal> A2L_VarChar::getYValueList()
{
    return hexValue_YList;
}
