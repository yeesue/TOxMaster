#include "globals.h"

Globals::Globals()
{

}

QList<A2L_VarChar*> Globals::charPamList_global;
QList<A2L_VarMeas*> Globals::measPamList_global;
QList<A2L_VarMeas*> Globals::daqMeasPamList_global;

QList<A2L_VarChar*> Globals::charPamList_2nd_global;
QList<A2L_VarMeas*> Globals::measPamList_2nd_global;
QList<A2L_VarMeas*> Globals::daqMeasPamList_2nd_global;

QList<dbc_signal*> Globals::dbcSignals_r_gloabl;
QList<dbc_signal*> Globals::dbcSignals_w_global;

QHash<QString, QList<A2L_VarMeas*>> Globals::funMeasListHash;
QHash<QString, QList<A2L_VarChar*>> Globals::funCharListHash;

QHash<QString, QList<PARAM*>> Globals::funPamListHash;
QHash<QString, QList<PARAM*>> Globals::funWritePamListHash;

QHash<QString, QList<PARAM*>> Globals::funMapPamListHash;

QHash<QString, PARAM*> Globals::mapPamHash;

QHash<QString, PARAM*> Globals::pamHash;
