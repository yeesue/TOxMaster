#ifndef XCP_COMMON_H
#define XCP_COMMON_H

#include <QtCore>
#include "a2l_varmeas.h"
#include "a2l_varchar.h"

//CMD TABLE
#define CONNECT                       0xFF
#define DISCONNECT                    0xFE
#define GET_STATUS                    0xFD
#define SYNCH                         0xFC

#define GET_COMM_MODE_INFO            0xFB
#define GET_ID                        0xFA
#define SET_REQUEST                   0xF9
#define GET_SEED                      0xF8
#define UNLOCK                        0xF7
#define SET_MTA                       0xF6
#define UPLOAD                        0xF5
#define SHORT_UPLOAD                  0xF4
#define BUILD_CHECKSUM                0xF3

#define TRANSPORT_LAYER_CMD           0xF2
#define USER_CMD                      0xF1

#define DOWNLOAD                      0xF0

#define DOWNLOAD_NEXT                 0xEF
#define DOWNLOAD_MAX                  0xEE
#define SHORT_DOWNLOAD                0xED
#define MODIFY_BITS                   0xEC

#define SET_CAL_PAGE                  0xEB
#define GET_CAL_PAGE                  0xEA

#define GET_PAG_PROCESSOR_INFO        0xE9
#define GET_SEGMENT_INFO              0xE8
#define GET_PAGE_INFO                 0xE7
#define SET_SEGMENT_MODE              0xE6
#define GET_SEGMENT_MODE              0xE5
#define COPY_CAL_PAGE                 0xE4

#define CLEAR_DAQ_LIST                0xE3
#define SET_DAQ_PTR                   0xE2
#define WRITE_DAQ                     0xE1
#define SET_DAQ_LIST_MODE             0xE0
#define GET_DAQ_LIST_MODE             0xDF
#define START_STOP_DAQ_LIST           0xDE
#define START_STOP_SYNCH              0xDD

#define GET_DAQ_CLOCK                 0xDC
#define READ_DAQ                      0xDB
#define GET_DAQ_PROCESSOR_INFO        0xDA
#define GET_DAQ_RESOLUTION_INFO       0xD9
#define GET_DAQ_LIST_INFO             0xD8
#define GET_DAQ_EVENT_INFO            0xD7

#define FREE_DAQ                      0xD6
#define ALLOC_DAQ                     0xD5
#define ALLOC_ODT                     0xD4
#define ALLOC_ODT_ENTRY               0xD3

#define INTEL                         0
#define MOTOROLA                      1

class A2L_VarMeas;
class A2L_VarChar;

enum A2L_TimeUnit
{
    UNIT_1NS = 0,
    UNIT_10NS = 1,
    UNIT_100NS = 2,
    UNIT_1US = 3,
    UNIT_10US = 4,
    UNIT_100US = 5,
    UNIT_1MS = 6,
    UNIT_10MS = 7,
    UNIT_100MS = 8,
    UNIT_1S = 9,
    UNIT_1PS = 10,
    UNIT_10PS = 11,
    UNIT_100PS = 12
              };

struct A2L_Daq {
    QString Name;
    QString DaqType;
    QString GranularityOdtEntrySize;
    QString IdentificationField;
    QString AddressExtType;
    QString OptimisationType;
    quint16 Max_Daq;
    quint16 Min_Daq;
    quint8 Max_Event_Channel;
    quint8 Max_Odt_Entry_Size_Daq;

};

struct A2L_Daq_List {
    QString Name;
    quint16 DaqListNumber;
    QString DaqListType;
    quint8 Max_Odt;
    quint8 Max_Odt_Entries;
    quint8 First_Pid;
    quint8 Event_Fixed;

};

struct A2L_Daq_Memory_Consumption {
    QString Name;
    quint32 Daq_Memory_Limit;
    quint8 Daq_Size;
    quint8 Odt_Size;
    quint8 Odt_Entry_Size;
    quint8 Odt_Daq_Buffer_Element_Size;
    quint8 Odt_Stim_Buffer_Element_Size;
};

struct A2L_Xcp_On_Can {
    QString Name;
    quint32 CAN_ID_MASTER;
    quint32 CAN_ID_SLAVE;
    quint64 BAUDRATE;
    quint8 SAMPLE_POINT;
    quint8 BTL_CYCLES;
    quint8 SJW;
    quint8 MAX_BUS_LOAD;
    QString SAMPLE_RATE;
    QString SYNC_EDGE;
    QString TRANSPORT_LAYER_INSTANCE;

    bool IsCANFD = false;
    quint64 FD_BAUDRATE;
    quint8 MAX_DLC = 8;
    quint8 FD_SAMPLE_POINT;
    quint8 FD_SECOND_SAMPLE_POINT;
    quint8 FD_SJW;
    quint8 FD_BTL_CYCLES;
};

struct A2L_EventCanId {
    QString Name;
    quint8 EventNumber;
    quint32 Event_CAN_ID;
};


struct A2L_Event {
    QString Name;
    quint8 ChannelNumber;
    QString EventChannelName;
    QString ShortName;
    QString EventType;
    quint8 Priority;
    quint8 TimeCycle;
    quint8 TimeUnit;
    quint8 Max_Daq_List;

    float Rate_ms = 0;
    A2L_EventCanId *eventCanId = NULL;
};

struct A2L_DaqListCanId {
    QString Name;
    quint16 DaqListNumber;
    quint32 DaqList_CAN_ID;
};

struct A2L_Checksum {
    QString checksumType;
    quint32 maxBlockSize;
};

struct A2L_Page {
    quint8 pageIndex;
    QString ecuAccessType;
    QString xcpReadAccessType;
    QString xcpWriteAccessType;
};

struct A2L_MemorySegment {
    QString memName;
    QString description;
    QString prgType;
    QString memType;
    QString memAttr;
    quint32 memAddress;
    quint32 memSize;
    qint16 offset1;
    qint16 offset2;
    qint16 offset3;
    qint16 offset4;
    qint16 offset5;

    quint8 segIndex;
    quint8 pageNum;
    quint8 addExtension;
    quint8 compressMethod;
    quint8 encryptionMethod;

    QString checksumType;
    quint32 maxBlockSize;

    //A2L_Checksum *a2lChecksum = NULL;
    QList<A2L_Page*> a2lPageList;
};

/*
struct A2L_VarMeas {
    QString Name;
    QString LongIdentifier;
    QString DataType;
    QString Conversion;
    QString ConversionType;
    QString COEFFS;
    QString ConversionTable;

    QString Unit;
    QString Format;

    quint32 Resolution; //smallest possible change in bits
    qreal Accuracy;     //possible variation from exact value in %

    quint32 DataSizeAG;
    quint32 ECUAddress;

    qreal Factor = 1;
    qreal Offset = 0;
    qreal LowerLimit;
    qreal UpperLimit;
    QList<qreal> CoefList;   // INT = f(PHYS) = (aX^2 + bX + c)/(dX^2 + eX + f)

    float rate_ms;            // ms
    A2L_Event *daqEventRef = NULL;

};
*/

struct varInfo{
    QString varName;
    quint32 varAddress;
    quint8  varSizeAG;
    float  rate_ms;

};

struct XCP_DAQ_ALLOC{
    quint16 DAQ_LIST_NUMBER;
    quint8 ODT_COUNT;
    QList<quint8> ODT_ENTRY_COUNT;
};

struct XCP_DAQ_PTR{
    quint16 DAQ_LIST_INDEX;
    quint8  DAQ_ODT_INDEX;
    quint8  DAQ_ODT_ENTRY_INDEX;
    quint32 PTR_Address;
    quint8  PTR_Size;
    quint32 offsetDataBuffer;
    //varInfo *PTR_var;
    A2L_VarMeas *PTR_var;
    quint8 DAQ_PID;
};

struct DataBuffer{
    quint8 *data;
    quint32 numData;
};

struct PID_Attr{
    quint16 DAQ_List;
    quint32 ODT_Offset;
    bool biggestPID;
};

struct var_Attr{
    quint16 DAQ_List;
    quint32 var_Offset;
};


#endif // XCP_COMMON_H
