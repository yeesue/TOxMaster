#ifndef GLOBALS_H
#define GLOBALS_H

#include <QHash>
#include "a2l_varchar.h"
#include "a2l_varmeas.h"
#include "param.h"
#include "config.h"

class Globals
{
public:
    Globals();

    static QList<A2L_VarChar*> charPamList_global;
    static QList<A2L_VarMeas*> measPamList_global;
    static QList<A2L_VarMeas*> daqMeasPamList_global;

    static QList<A2L_VarChar*> charPamList_2nd_global;
    static QList<A2L_VarMeas*> measPamList_2nd_global;
    static QList<A2L_VarMeas*> daqMeasPamList_2nd_global;

    static QList<dbc_signal*> dbcSignals_r_gloabl;
    static QList<dbc_signal*> dbcSignals_w_global;

    static QHash<QString, QList<A2L_VarMeas*>> funMeasListHash;
    static QHash<QString, QList<A2L_VarChar*>> funCharListHash;

    static QHash<QString, QList<PARAM*>> funPamListHash;

    static QHash<QString, QList<PARAM*>> funWritePamListHash;

    static QHash<QString, QList<PARAM*>> funMapPamListHash;

    static QHash<QString, PARAM*> mapPamHash;

    static QHash<QString, PARAM*> pamHash;

};

#endif // GLOBALS_H
