#include "a2l_varmeas.h"


A2L_VarMeas::A2L_VarMeas()
{

}

A2L_VarMeas::~A2L_VarMeas()
{

}

int A2L_VarMeas::getPrecision()
{
    QString preciionStr = Format.replace("\"", "").split(".").at(1);
    int precision = preciionStr.toInt();
    return precision;
}

void A2L_VarMeas::valueUpdatedTimerSlot()
{
    //emit valueUpdated(Value);
}
