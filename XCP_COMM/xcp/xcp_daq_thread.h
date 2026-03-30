#ifndef XCP_DAQ_THREAD_H
#define XCP_DAQ_THREAD_H

#include <QObject>
#include "xcpmaster.h"
#include "a2l_varmeas.h"

class XCP_DAQ_Thread : public QObject
{
    Q_OBJECT
public:
    explicit XCP_DAQ_Thread(QObject *parent = nullptr, XCPMaster *master = nullptr);

    void setDaqMeasPamList(const QList<A2L_VarMeas *> &value);

    void arrangeMeasVars();
    void arrangeMeasVars_Static();

signals:

private:
    A2L_Daq_List *getDaqListByEventRate(float rate_ms);

    void xcpDaqConfigPamReset();

private:
    QList<A2L_VarMeas*> daqMeasPamList;     //daq meas pams

    XCPMaster *xcpMaster = NULL;

    bool daqRunFlag = false;

    //DAQ XCP infos
    A2L_Daq *xcpDaq = NULL;
    QList<A2L_Daq_List*> xcpDaqList;
    QList<A2L_Event*> xcpEventList;

    QList<A2L_VarMeas*> measVarList;
    QHash<float, QList<A2L_VarMeas*>> rateVarHash;  // variable list in every rate
    QHash<quint16, QList<A2L_VarMeas*>> daqListVarHash; //variable list in every daqList
    QHash<quint8, DataBuffer> pidHash;
    QHash<quint8, PID_Attr>pidAttrHash;
    QHash<A2L_VarMeas*, var_Attr> varAttrHash;
    QHash<A2L_VarMeas*, float> varValueHash;
    QHash<quint16, DataBuffer> daqListDataHash;
    QHash<quint16, QSharedMemory*> daqListSMHash;
    QList<XCP_DAQ_PTR> daqPTRList;
    QList<XCP_DAQ_ALLOC> daqAlloc;
    bool varsDAQConfigured = false;
    bool DAQRunning = false;

    quint32 sizeDataBuffer_DAQ = 0;
    quint16 DAQ_COUNT = 0;
    quint16 MIN_DAQ = 0;


};

#endif // XCP_DAQ_THREAD_H
