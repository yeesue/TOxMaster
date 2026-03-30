#ifndef COMMON_H
#define COMMON_H

#include <QString>
#include "can_format_raw.h"
#include "config.h"
#include "xcp_common.h"
#include "dbcparser.h"

#ifdef Q_OS_WIN
#pragma comment(lib, "user32.lib")
#include <qt_windows.h>
#endif

#define INTEL 0
#define MOTOROLA 1


#define READ_SM_REFRESH_REMIND WM_USER+101
#define WRITE_SM_REFRESH_REMIND WM_USER+102

#define RECORD_REMIND WM_USER+110

#define INDEX_READ_SM_TCP1  1
#define INDEX_WRITE_SM_TCP1  2
#define INDEX_READ_SM_CAN    10
#define INDEX_WRITE_SM_CAN   20


#define WIN_TITLE "ParameterExplorer"
#define WIN_TITLE_2 "MDF_Factory"


struct PAM_INFO
{
    QString Param_Name;
    int Type;
    int StartBit;
    int StartBit_Oth;
    int LengthBit;
    int ByteOrder;
    qreal DefaultValue;
    qreal ScalingFactor;
    qreal ScalingOffset;
    qreal Maximum;
    qreal Minimum;
    QString Unit;
    QString Comment;
};

struct XcpPam {
    QString pamName = "";
    QString pollType = "";
    QString caliType = "";
    QString daqEventTime = "";
    int     daqEventIndex = 0;
    qreal   pamDefaultValue = 0;
    quint16 sizeByte = 0;
};

struct XcpConfig {
    QString workName = "";
    QString intfName = "";
    QString a2lProjectName = "";
    QString a2lFilePath = "";
    QString hexFilePath = "";
    QString appDir = "";
    QString a2lFilePath_ref = "";
    QString hexFilePath_ref = "";
    QString calCsvFilePath = "";

    int index_XcpOnCan = 0;
    quint64 baudrate = 1000000;
    quint32 id_CMD = 0;
    quint32 id_RES = 0;
    QString daqType = "";

    bool isCanFD = false;
    quint64 fdBaudrate = 2000000;
    quint8 maxDLC = 8;

    bool useSeedKey = false;
    QString seedKeyDllPath = "";

    bool useA2lSetFile = false;
    A2L_Daq a2lDaq;
    QList<A2L_Daq_List*> daqList;
    QList<A2L_Event*> eventList;
    QList<A2L_EventCanId*> eventCanIdList;
    QStringList xcpOnCanNameList;
    QStringList eventTimeList;
    QStringList daqListNameList;
    QStringList daqListCanIdList;

    bool usePamsInExcel = false;

    QList<XcpPam> calPamList;
    QList<XcpPam> daqPamList;
    QList<XcpPam> pollPamList;

    int xcpDeviceType = 0;  // 0 = ni_xnet, 1 = ts_can, 2 = zlg_can
    int tsDeviceIndex = 0;
    int zlgDeviceType = 0;  // 41 = USB_CANFD_200U, 42 = USB_CANFD_100U, 43 = USB_CANFD_MINI,
    int zlgDeviceIndex = 0;
};


struct CanConfig {
    QString workName = "";

    bool isArxml = false;
    QString sel_arxmlPath = "";
    QString sel_arCluster = "";
    QString sel_dbcPath = "";
    QString sel_cluster = "";

    QString appDir = "";

    QString sel_arxmlPath_ref = "";
    QString sel_dbcPath_ref = "";

    QString sel_interface = "";
    quint64 sel_baudrate = 0;
    quint32 sel_canio_mode = 0;
    quint64 sel_canfd_baudrate = 0;

    bool sel_termination_on = false;

    bool sel_CustomBD_on = false;
    bool sel_CustomFDBD_on = false;
    qreal sel_SamplePoint_BD = 87.5;    //87.5%
    qreal sel_SamplePoint_FDBD = 87.5;  // 87.5%

    QString sel_frames_r = "";
    QString sel_frames_w = "";
    QString sel_sigs_r = "";
    QString sel_sigs_w = "";

    QList<dbc_frame*> dbc_frames;
    QList<dbc_frame*> dbc_frames_r;
    QList<dbc_frame*> dbc_frames_w;
};

struct TsCanConfig {
    QString workName = "";

    bool isArxml = false;
    QString sel_arxmlPath = "";
    QString sel_arCluster = "";
    QString sel_dbcPath = "";
    QString sel_cluster = "";

    QString appDir = "";

    QString sel_arxmlPath_ref = "";
    QString sel_dbcPath_ref = "";

    QString sel_interface = "";
    quint64 sel_baudrate = 0;
    quint32 sel_canio_mode = 0;
    quint64 sel_canfd_baudrate = 0;

    bool sel_termination_on = false;

    bool sel_CustomBD_on = false;
    bool sel_CustomFDBD_on = false;
    qreal sel_SamplePoint_BD = 87.5;    //87.5%
    qreal sel_SamplePoint_FDBD = 87.5;  // 87.5%

    QString sel_frames_r = "";
    QString sel_frames_w = "";
    QString sel_sigs_r = "";
    QString sel_sigs_w = "";

    QList<BO_*> dbc_frames;
    QList<BO_*> dbc_frames_r;
    QList<BO_*> dbc_frames_w;
};

struct ZlgCanConfig {
    QString workName = "";

    bool isArxml = false;
    QString sel_arxmlPath = "";
    QString sel_arCluster = "";
    QString sel_dbcPath = "";
    QString sel_cluster = "";

    QString appDir = "";

    QString sel_arxmlPath_ref = "";
    QString sel_dbcPath_ref = "";

    QString sel_interface = "";
    quint64 sel_baudrate = 0;
    quint32 sel_canio_mode = 0;
    quint64 sel_canfd_baudrate = 0;

    bool sel_termination_on = false;

    bool sel_CustomBD_on = false;
    bool sel_CustomFDBD_on = false;
    qreal sel_SamplePoint_BD = 87.5;    //87.5%
    qreal sel_SamplePoint_FDBD = 87.5;  // 87.5%

    QString sel_frames_r = "";
    QString sel_frames_w = "";
    QString sel_sigs_r = "";
    QString sel_sigs_w = "";

    QList<BO_*> dbc_frames;
    QList<BO_*> dbc_frames_r;
    QList<BO_*> dbc_frames_w;
};

struct FunStruct {
    QString funName;
    QString intfName;
    QString devType;  // 0 = NI-XNET, 1 = TS-CAN, 2 = ZLG-CAN
    QString funType;   // 0 = XCP, 1 = CAN/CANFD, 2 = UDS, 3 = WT3000, 4 = WT5000
    QString setFilePath;
};

struct PamInfo {
    QString modName;
    QString pamName;
    double  pamSetValue;

    double showMin;
    double showMax;
    bool autoScale;
    quint8 decimal = 2;
};

struct CRC_INFO{
    int crcIndex;
    int startIndex;
    int endIndex;
};

//extern void can_format_motorola2intel_rawdata(char *rawData);
//extern void can_format_motorola2intel_sb_lb(quint16 *StartBit, quint16 *LenBit);

bool copyToSharedMemory(QSharedMemory *sm, int index, char *srcData, quint16 len);

bool copyToSharedMemory_2Data(QSharedMemory *sm, int index1, char *srcData1, quint16 len1, int index2, char *srcData2, quint16 len2);

bool copyFromSharedMemory(QSharedMemory *sm, int index, char *dstData, quint16 len);

bool copyFromSharedMemory_2Data(QSharedMemory *sm, int index1, char *dstData1, quint16 len1, int index2, char *dstData2, quint16 len2);

quint32 canRawMoto_2_UserData(quint8 *raw, quint16 sb, quint8 lb);
qint32  canRawMoto_2_UserData_Int(quint8 *raw, quint16 sb, quint8 lb);
quint32 canRawIntel_2_UserData(quint8 *raw, quint16 sb, quint8 lb);

quint32 canUserData_2_RawMoto(quint8 *raw, quint16 sb, quint8 lb, quint32 data);
quint32 canUserData_2_RawMoto(quint8 *raw, quint16 sb, quint8 lb, qint32 data);
quint32 canUserData_2_RawIntel(quint8 *raw, quint16 sb, quint8 lb, quint32 data);

quint16 canSigMotoSbConvToMdfSb(quint16 sb, quint8 lb);

#endif // COMMON_H
