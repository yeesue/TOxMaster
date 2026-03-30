#ifndef XCPMASTER_H
#define XCPMASTER_H

#include <QObject>
#include "xcp_thread.h"
#include "xcp_thread_ts.h"
#include "xcp_thread_zlg.h"
#include "xcp_common.h"
#include <QReadWriteLock>
#include <QSharedMemory>
#include <QApplication>
#include "a2l_varmeas.h"
#include "a2l_varchar.h"
#include "windows.h"


class XCPMaster : public QObject
{
    Q_OBJECT
public:
    explicit XCPMaster(QObject *parent = 0, QString name = "", int canType = 0);
    ~XCPMaster();


    bool XCPInit();
    void XCPStop();

    // xcpDeviceType = ni_xnet
    bool XCPInit_NI();
    void XCPStop_NI();

    // xcpDeviceType = ts_can
    bool XCPInit_TS();
    void XCPStop_TS();

    // xcpDeviceType = zlg_can
    bool XCPInit_ZLG();
    void XCPStop_ZLG();

    //XCP CMD Sequences
    void XCPDemo();
    bool XCP_Setup_Session();

    void XCP_SetMemSeg();
    bool XCP_SET_CAL_PAGE(quint8 memSeg, quint8 page);

    bool XCP_UnlockEcuResource(quint8 resource);

    bool XCP_GetInfos();
    bool XCP_Cal();
    bool XCP_Poll();
    bool XCP_DAQ_Demo();
    bool XCP_DAQ_CONFIG();  //DAQ configuration
    bool XCP_DAQ_Start_Stop(bool runFlag);

    bool XCP_Poll_Meas(A2L_VarMeas *measVar);
    bool copyPollRawDataToSM(char *data, int size, A2L_VarMeas *measVar);

    bool XCP_Cal_VALUE(A2L_VarChar* varChar);
    bool XCP_Cal_VALUE(A2L_VarChar* varChar, char *data, int size);
    bool XCP_Cal_VALUE(A2L_VarChar* varChar, char *data, int size, quint32 offset);
    bool XCP_Cal_CURVE(A2L_VarChar* curveChar, int index, qreal phys);
    bool updateCalTimeInSM(A2L_VarChar *varChar);

    bool XCP_Cal_VAL_BLK(A2L_VarChar* varChar);
    bool XCP_Cal_CURVE(A2L_VarChar* varChar);

    void setXcpDaq(A2L_Daq* daq);
    void setXcpDaqList(QList<A2L_Daq_List*> daqList);
    void setXcpEventList(QList<A2L_Event*> eventList);
    void setXcpEventCanIdList(const QList<A2L_EventCanId *> &value);

    void initVars();
    void arrangeMeasVars();
    void arrangeMeasVars_Static();

    void arrangeMeasVars_dlc64();

    bool XCP_DAQ_ALLOC_DYN();
    bool XCP_PREPARE_DAQ_LIST_DYN();
    bool XCP_CONFIG_DAQ_LIST_DYN();

    bool XCP_PREPARE_DAQ_LIST_STATIC();
    bool XCP_CONFIG_DAQ_LIST_STATIC();

    bool XCP_DAQ_LIST_CONFIG();
    bool XCP_SET_DAQ_LIST_MODE_FORALL();
    bool XCP_STARTSTOP_DAQ_SELECT(bool Flag);

    void setAddressMTA(quint32 Address, quint8 Address_Ext = 0x00);
    void setDataDOWNLOAD(quint8 *data, quint8 numDataAG);

    void setnumOfDataUPLOAD(quint8 numOfData);
    void getDataUPLOAD(quint8 *data, quint8 numDataAG);

    // config daq meas pams
    void setMeasVarListForDAQ(QList<A2L_VarMeas*> varList);
    void addMeasVarListForDAQ(QList<A2L_VarMeas*> varList);

    void setBaudrate(quint64 baudrate);
    void setIDMasterToSlave(quint32 id);
    void setIDSlaveToMaster(quint32 id);
    void setXcpCanInterface(QString canInterface);
    void setResistanceEnable(int value);

    void setIsCanFd(bool value);
    void setXcpFdBaudrate(const quint64 &value);
    void setMaxDLC(const quint8 &value);

    void setCharList(const QList<A2L_VarChar *> &value);

    QHash<quint16, QList<A2L_VarMeas *> > getDaqListVarHash() const;

    QHash<quint16, QSharedMemory *> getDaqListSMHash() const;

    QHash<quint16, quint32> getDaqListBlockSizeHash() const;

    void setXcpName(const QString &value);

    QHash<QString, QList<A2L_VarMeas *> > getDgNameVarHash();

    void setPollMeasList(const QList<A2L_VarMeas *> &value);

    void setSeedKeyDllPath(const QString &value);

    void setUseSeedKey(bool value);

    void setXcpMemSegList(const QList<A2L_MemorySegment *> &value);

    void setXcpDeviceType(int value);

    void setZlgDeviceType(const quint32 &value);

    void setZlgDeviceIndex(const quint32 &value);

signals:
    void ODTDataUpdated(quint16);
    void ODTDataForRecord(quint8*, quint32, quint16);
    void ODTDataForRecord(quint8*, quint32, QString);
    void pollDataForRecord(quint8*, quint32, int);
    void varValueUpdated(A2L_VarMeas*, float);

public slots:
    void getCTORESData(quint8 *data, quint32 numBytes);
    void getDAQODTData(quint8 *data, quint32 numData);

    void getCTORESData_TS(quint8 *data, quint32 numBytes);
    void getDAQODTData_TS(quint8 *data, quint32 numData);

    void getCTORESData_ZLG(quint8 *data, quint32 numBytes);
    void getDAQODTData_ZLG(quint8 *data, quint32 numData);

    void ODTDataUpdatedSlot(quint16 DAQList);

    qreal RatFuncConvFromIntToPhys(qreal valueInt, QList<qreal> coefList);
    qreal RatFuncConvFromPhysToInt(qreal valuePhys, QList<qreal> coefList);

    void charVarValueChangedSlot(A2L_VarChar* charVar);

    void curveCharZValueChangedSlot(A2L_VarChar* curveChar, int index, qreal data);

    bool getXcpCanInitStatus();
    bool getXcpSessionConnectedStatus();
    bool getXcpDaqRunningStatus();

    void downloadCals();
    void uploadCals();

    void sltDaqRunStatusActive(bool status);

private:
    int getPollMeasIndexInList(A2L_VarMeas *measVar);

    bool copyFileToPath(QString sourceDir ,QString toDir, bool coverFileIfExist);

private:

    XCP_Thread *xcpCanThread = NULL;
    XCP_Thread_TS *xcpTsCanThread = NULL;
    XCP_Thread_ZLG *xcpZlgCanThread = NULL;

    QString xcpName = "";
    int xcpDeviceType = 0; // 0 = ni_xent, 1 = ts_can, 2 = zlg_can

    QString xcpdbc_Path;
    QString xcp_Cluster;
    QString FrameText_CMD;
    QString FrameText_RES;
    QString FrameText_EVENT;    
    QString xcpcan_Interface;

    quint64 xcpBaudrate;
    quint32 ID_Master_to_Slave;
    quint32 ID_Slave_to_Master;

    int resistanceEnable = 0;
    bool isCanFd = false;
    quint64 xcpFdBaudrate;
    quint8 maxDLC = 8;

    APP_CHANNEL tsCanIndex = CHN1;

    quint32 zlgDeviceType = 0;
    quint32 zlgDeviceIndex = 0;
    quint32 zlgCanIndex = 0;

    quint8 CTO_CMDPacket[64];
    quint8 CTO_RESPacket[64];
    quint8 CTO_RESTime[8];

    quint8 DAQ_ODTPacket[64];  // CANFD 容量为64，兼容CAN 容量 8

    quint8 *DataBuffer_DAQ;
    quint32 sizeDataBuffer_DAQ;

    bool CTORESDataUpdated = false;

    void xcpdbcUpdate();
    void xcpdbcRemove();
    bool xcpCanInitFlag = false;

    void reverse(char* data, int size);

    bool CTO_CMD_RES_Comm();
    void CTO_ClearData();
    void showDataInHex(quint8 *data, quint16 size);

    bool XCPConnected = false;
    bool XCPStatus = false;

    //XCP CTO STD CMD
    bool CTO_CONNECT();
    quint8 ByteOrder_XCPAddr = 0;  //0=Intel, 1=Motorola for Address position
    bool CTO_DISCONNECT();
    bool CTO_GET_STATUS();
    bool CAL_PAG_LOCK_STATUS;
    bool DAQ_LOCK_STATUS;
    bool STIM_LOCK_STATUS;
    bool PGM_LOCK_STATUS;
    bool DAQ_Running_GETSTATUS;

    bool CTO_GET_SEED();
    quint8 SEED_MODE = 0x00; //0=(first part)seed, 1=remaining part of seed
    quint8 SEED_RESOURCE = 0x00; //0x01=CAL/PAG, 0x04=DAQ, 0x08=STIM, 0x10=PGM
    quint8 LEN_SEED = 0x00;
    quint8 data_SEED[16]; //SEED data buffer

    bool useSeedKey = false;
    QString seedKeyDllPath = "";

    bool CTO_UNLOCK();
    quint8 LEN_KEY = 0x00;
    quint8 data_KEY[16]; //KEY data buffer
    quint8 PROTECT_STATUS = 0x00;  // current protection status
    bool CAL_PAG_PRO_STATUS;
    bool DAQ_PRO_STATUS;
    bool STIM_PRO_STATUS;
    bool PGM_PRO_STATUS;

    qint16 XCP_Seed2Key(QString dllPath, quint8 privilege, quint8 lenOfSeed, quint8 *seed, quint8 *lenOfKey, quint8 *key);

    bool CTO_SYNCH();
    bool CTO_GET_COMM_MODE_INFO();

    bool CTO_GET_ID();
    quint32 length_GET_ID;

    bool CTO_SET_MTA();
    quint32 MTA_Address = 0x00000000;
    quint8 MTA_Address_Extension = 0x00;
    void fromIntelToMoto(quint32 *address);

    bool CTO_UPLOAD();
    quint8 numOfDataAG_UPLOAD;
    quint8 data_UPLOAD[7];

    bool CTO_SHORT_UPLOAD();
    bool CTO_BUILD_CHECKSUM();
    //XCP CTO CAL CMD
    bool CTO_DOWNLOAD();
    quint8 numOfDataAG_DOWNLOAD = 0;
    quint8 data_DOWNLOAD[6];
    void ClearDataDownload();

    //XCP CTO PAG CMD
    bool CTO_SET_CAL_PAGE();
    bool CTO_GET_CAL_PAGE();
    quint8 curSegment;
    quint8 curPage;

    bool CTO_GET_PAG_PROCESSOR_INFO();
    quint8 MAX_SEGMENT = 0x00;

    //XCP DAQ

    //XCP DAQ Static
    bool CTO_CLEAR_DAQ_LIST();
    quint16 DAQLISTNUMBER_CLEARDAQLIST = 0;

    bool CTO_SET_DAQ_PTR();
    quint16 DAQLISTNUMBER_SETPTR = 0;
    quint8 ODTNUMBER_SETPTR = 0;
    quint8 ODTENTRYNUMBER_SETPTR = 0;

    bool CTO_WRITE_DAQ();
    quint8 SIZE_WRITEDAQ;
    quint8 ADDRESSEXT_WRITEDAQ;
    quint32 ADDRESS_WRITEDAQ;

    bool CTO_SET_DAQ_LIST_MODE();
    quint16 DAQLISTNUMBER_SETDAQLISTMODE = 0;
    quint16 EVENTCHANNEL_SETDAQLISTMODE = 0;
    quint8 TransmissionRatePrescaler = 0x01;
    quint8 DAQListPriority = 0;  //0xFF Highest

    bool CTO_GET_DAQ_LIST_MODE();
    quint16 DAQLISTNUMBER_GETDAQLISTMODE = 0;

    bool CTO_START_STOP_DAQ_LIST();
    quint8 MODE_STARTSTOPDAQLIST = 0;   //0x00-stop, 0x01-start, 0x02-select
    quint16 DAQLISTNUMBER_STARTSTOP = 0;

    bool CTO_START_STOP_SYNCH();
    quint8 MODE_STARTSTOPSYNCH = 0;

    //XCP DAQ Dynamic
    bool CTO_FREE_DAQ();
    bool CTO_ALLOC_DAQ();
    quint16 DAQ_COUNT = 0;
    quint16 MIN_DAQ = 0;

    bool CTO_ALLOC_ODT();
    quint16 DAQ_LIST_NUMBER;
    quint8 ODT_COUNT = 0;

    bool CTO_ALLOC_ODT_ENTRY();
    quint8 ODT_NUMBER;
    quint8 ODT_ENTRIES_COUNT;

    QHash<A2L_VarMeas*, QSharedMemory*> measSMHash;
    QList<A2L_VarMeas*> pollMeasList;

    //QList<varInfo*> varsForDAQ;
    QList<A2L_VarMeas*> daqMeasVarList;
    QHash<float, QList<A2L_VarMeas*>> rateVarHash;  // variable list in every rate
    QHash<quint16, QList<A2L_VarMeas*>> daqListVarHash; //variable list in every daqList

    QHash<quint8, DataBuffer> pidHash;
    QHash<quint8, PID_Attr>pidAttrHash;
    QHash<A2L_VarMeas*, var_Attr> varAttrHash;
    QHash<A2L_VarMeas*, float> varValueHash;
    QHash<quint16, DataBuffer> daqListDataHash;
    QHash<quint16, QSharedMemory*> daqListSMHash;
    QHash<quint16, quint32> daqListBlockSizeHash;
    QList<XCP_DAQ_PTR> daqPTRList;
    QList<XCP_DAQ_ALLOC> daqAlloc;
    bool varsDAQConfigured = false;
    bool DAQRunning = false;

    QHash<QString, QList<A2L_VarMeas*>> dgNameVarHash;
    QHash<QString, QSharedMemory*> dgNameSMHash;
    QHash<QString, quint32> dgNameBlockSizeHash;

    void xcpDaqConfigPamReset();

    //DAQ XCP infos
    A2L_Daq *xcpDaq = NULL;
    QList<A2L_Daq_List*> xcpDaqList;
    QList<A2L_Event*> xcpEventList;
    QList<A2L_EventCanId*> xcpEventCanIdList;
    QList<quint32> idList;

    A2L_EventCanId *findEventCanIdByEventNumber(quint8 eventNumber);
    A2L_Event *getEventChannelByEventRate(float rate_ms);
    A2L_Daq_List *getDaqListByEventRate(float rate_ms);

    //Memory infos
    QList<A2L_MemorySegment*> xcpMemSegList;


    QList<A2L_VarChar*> charVarList;
    A2L_VarChar* curCharVar;

    QList<A2L_VarChar*> charList;

};

#endif // XCPMASTER_H
