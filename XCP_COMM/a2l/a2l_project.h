#ifndef A2L_PROJECT_H
#define A2L_PROJECT_H

#include <QObject>
#include "xcp_common.h"
#include "a2l_praser_interface.h"
#include "xcpmaster.h"
#include <QMap>
#include "a2l_varmeas.h"
#include "a2l_varchar.h"


class A2L_Project : public QObject
{
    Q_OBJECT
public:
    A2L_Project(QObject* parent = 0);
    A2L_Project(QString fullFileName, A2L_PRASER_INTERFACE* a2lPraserRef);

    void setFullFileName(QString fullFileName);
    QString getA2LFileName();
    void setA2LPraser(A2L_PRASER_INTERFACE *a2lPraserRef);
    bool getPrasedStatus();
    QStringList getPrasedInfo();

    bool getHexPrasedStatus();
    QString getA2LRefHexFileName();

    QStringList getEventNameList();
    QStringList getEventTimeList();

    QStringList getA2LAllMeasVarNameList();
    QStringList getA2LAllCharVarNameList();

    void addSelMeasVarList(QStringList measVarNameList, QStringList measVarRateList);
    void addSelCharVarList(QStringList charVarNameList);

    void removeMeasVarFromSelMeasVarList(A2L_VarMeas *measVar);

    void updateEventRelatedMeasSize(A2L_VarMeas *measVar, quint8 addOrDel); // add = 1, del = 0
    QHash<QString, quint32> getEventCurSizeHash();
    QHash<QString, quint32> getEventMaxSizeHash();
    QHash<QString, qreal> getEventRateHash();

    QList<A2L_VarMeas*> getSelectedMeasVarList();
    QList<A2L_VarMeas*> getAddMeasVarListByNameAndRateList(QStringList measVarNameList, QStringList measVarRateList);
    A2L_VarMeas* getMeasVarByNameAndRate(QString measVarName, QString measVarRate);
    A2L_VarMeas* findMeasVarInSelMeasVarList(QString measVarName, QString measVarRate);

    A2L_VarMeas* getMeasVarByName(QString measVarName);
    void getScalingNumFromCoefs(QList<qreal> coefList, qreal &scalingFactor, qreal &scalingOffset);


    QList<A2L_VarChar*> getAddCharVarListByNameList(QStringList charVarNameList);
    A2L_VarChar* getCharVarByName(QString charVarName);
    A2L_VarChar* findCharVarInSelCharVarList(QString charVarName);
    A2L_VarChar* findValueCharVarInSelCharVarList(QString charVarName);
    A2L_VarChar* findCurveCharVarInSelCharVarList(QString charVarName);
    A2L_VarChar* findMapCharVarInSelCharVarList(QString charVarName);

    QList<A2L_VarChar*> getAddValueCharVarListByNameList(QStringList charVarNameList);
    QList<A2L_VarChar*> getAddCurveCharVarListByNameList(QStringList charVarNameList);
    QList<A2L_VarChar*> getAddMapCharVarListByNameList(QStringList charVarNameList);

    void removeCharVarFromSelCharVarList(A2L_VarChar *charVar);

    A2L_Daq *getDaqInfo();
    A2L_Xcp_On_Can *getXcpOnCanInfo(int index);
    void setCurUseXcpOnCanIndex(int index);
    A2L_Xcp_On_Can *getCurUseXcpOnCan();
    QStringList getXcpOnCanInstanceNameList();
    QStringList getDaqListCanIdList();
    QStringList getEventCanIdList();
    QStringList getDaqListList();
    QList<A2L_Daq_List*> getDaqListStructList();

    A2L_Daq_Memory_Consumption *getDaqMemoryConsumption();


    bool addHexFile(QString fullHexName);

    void setXcpMaster(XCPMaster* xcpMaster);

    bool getCurProXcpCanInitStatus();
    bool getCurProXcpSessionConnectedStatus();
    bool getCurProXcpDaqRunningStatus();

    bool xcpDaqDynConfig();
    bool setXcpDaqStartStop(bool status);

    bool xcpDaqConfig();

    bool xcpDaqStatConfig();

    A2L_Event* findEventByShortName(QString shortName);
    A2L_Event* findEventByChannelNumber(quint8 channelNumber);
    QList<A2L_Event*> getEventList();

    QList<A2L_EventCanId *> getA2lEventCanIdList() const;

    QList<A2L_MemorySegment *> getA2lMemSegList() const;

private:
    A2L_PRASER_INTERFACE *a2lPraser = NULL;
    QString a2lFullFileName;
    QString a2lFullHexName;
    QStringList infoList;
    bool prasedStatus = false;
    bool hexPrasedStatus = false;

    QStringList allMeasVarNameList;
    QStringList allCharVarNameList;

    QStringList selMeasVarNameList;
    QStringList selCharVarNameList;


    QHash<A2L_Event*, QList<A2L_VarMeas*>> eventSelMeasVarHash;
    QHash<A2L_Event*, quint32> eventSelMeasSizeSumHash;
    QHash<QString, quint32> eventNameSelMeasSizeSumHash;
    QHash<QString, quint32> eventNameSizeMaxHash;
    QHash<QString, qreal> eventNameRateHash;

    QList<A2L_VarMeas*> selMeasVarList;
    A2L_Daq *a2lDaq = NULL;
    A2L_Daq_Memory_Consumption *a2lDaqMem = NULL;
    QList<A2L_Event*> a2lEventList;
    QStringList eventNameList;
    QList<A2L_Event*> a2lEventInXcpOnCanList;
    QStringList eventInXcpOnCanNameList;
    QList<A2L_Xcp_On_Can*> a2lXcpOnCanList;
    QStringList xcpOnCanInstanceNameList;
    QList<A2L_EventCanId*> a2lEventCanIdList;
    QStringList eventCanIdNameList;
    QList<A2L_Daq_List*> a2lDaqListList;
    QStringList daqListNameList;
    QList<A2L_DaqListCanId*> a2lDaqListCanIdList;
    QStringList daqListCanIdNameList;
    QList<A2L_MemorySegment*> a2lMemSegList;

    int curIndexXcpOnCan = 0;
    A2L_Xcp_On_Can *curXcpOnCan = NULL;

    quint32 hexOrDecStrToU32Num(QString str);
    quint32 getSizeFromDataType(QString dataType);


    QList<A2L_VarChar*> selCharVarList;

    QList<A2L_VarChar*> selValueCharVarList;
    QList<A2L_VarChar*> selCurveCharVarList;
    QList<A2L_VarChar*> selMapCharVarList;

    void getA2LDaqInfo(int indexXcpOnCan);
    void getA2LDaqMemInfo(int indexXcpOnCan);
    void getA2LEventListInfo(int indexXcpOnCan);
    void getA2LEventCanIdListInfo(int indexXcpOnCan);

    void getA2LDaqInfo();
    void getA2LDaqMemInfo();
    void getA2LEventListInfo();
    void getA2LXcpOnCanListInfo();
    void getA2LEventCanIdListInfo();
    void getA2LDaqListListInfo();
    void getA2LDaqListCanIdListInfo();

    void getA2LMemSegListInfo();

    XCPMaster *xcpMaster = NULL;

};

#endif // A2L_PROJECT_H
