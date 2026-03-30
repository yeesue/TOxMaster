#ifndef UDS_CONFIG_H
#define UDS_CONFIG_H

#include <QString>


struct UDSInfo{
    QString canIntf = "";
    quint64 baudrate = 500000;
    quint64 baudrate_canfd = 2000000;
    quint32 canIoMode = 0;
    quint16 transportProtocol = 0;
    quint16 maxDLC = 8;
    quint32 transmitID = 0x00;
    quint32 receiveID = 0x00;

    quint32 securityConstant = 0x00000000;

    bool testerActive = false;
    quint16 testerCycleTime = 1000;   //ms
    bool testerResponseRequired = false;


    quint16 seedKeyType = 0;
    bool useDll = false;
    QString dllPath = "";

    quint8 byteOrder = 0;     // 0 = intel, 1 = motorola

    bool autoFreeze = false;
    quint16 timeBetweenCtrlAndFreeze = 0;
};

struct CmdAttr{
    QString attrName;
    quint8 attrDataSize = 0;
    quint8 attrDataIndex = 0;
};

struct UDS_CMD_REQ{
    quint8 cmd = 0x00;
    quint8 data[64];
    quint8 dataLen = 0;

    QString name = "";
    QString preStr = "";

    QList<CmdAttr> attrList;
};

/*
struct UDS_CMD_SESSION_CONTROL{
    quint8 cmd = 0x10;
    quint8 sessionType = 0x00;    // 0x01 = Default; 0x02 = Programming; 0x03 = Extended

    QString name = "诊断会话控制";  //Session_Ctrl
};

struct UDS_CMD_TEST_PRESENTED{
    quint8 cmd = 0x3E;
    quint8 requireAns = 0x00;   // 0x00 = not require; 0x01 = require;

    QString name = "诊断仪在线";  //TestPresented
};

struct UDS_CMD_SECURITY_ACCESS{
    quint8 cmd = 0x27;
    quint8 accessMode = 0x00;    // 0x01-02; 0x03-04; 0x05-06; 0x07-08; 0x09-0A

    quint8 seedOrKey = 0x01;    // 0x01 = request seed; 0x02 = send key;
    quint8 data[4];

    QString name = "安全访问";   //Security_Access
};

struct UDS_CMD_READ_DATA_BY_ID{
    quint8 cmd = 0x22;
    quint16 did = 0x0000;

    QString name = "根据标识符读取数据";   //ReadDataById
};

struct UDS_CMD_WRITE_DATA_BY_ID{
    quint8 cmd = 0x2E;
    quint16 did = 0x0000;

    QString name = "根据标识符写入数据";   //WriteDataById
};

struct UDS_CMD_IO_CTRL_BY_ID{
    quint8 cmd = 0x2F;
    quint16 did = 0x0000;
    quint8 mode = 0x00;    // 0x00 = Return Control to ECU; 0x01 = Reset to default; 0x02 = Freeze Current State; 0x03 = Short Term Control

    quint8 data[4];
    quint8 len = 0;

    QString name = "根据标识符输入输出控制";   //IOCtrlById
};

struct UDS_CMD_ROUTINE_CTRL{
    quint8 cmd = 0x31;
    quint8 mode = 0x00;    // 0x01 = Start Routine; 0x02 = Stop Routine; 0x03 = Request Routine Result;
    quint16 did = 0x0000;

    quint8 data[4];
    quint8 len = 0;

    QString name = "例程控制";   //RoutineCtrl
};
*/

struct UDS_CMD_ITEM_IN_FLOW{
    QString name = "service";
    UDS_CMD_REQ *cmdIns = NULL;
    bool enable = true;
    quint8 type = 0;              // 0 = normal cmd, 1 = seed key cmd pack, 2 = tester presented cmd
    quint16 preTime = 0;
    quint16 postTime = 50;       // post time 50ms
    quint16 retryTime = 0;
    quint8 errHandle = 0;         // 0 = stop, 1 = continue
};

struct UDS_FLOW{
    QString flowName = "udsFlow";
    QList<UDS_CMD_ITEM_IN_FLOW*> cmdItemList;
};

#endif // UDS_CONFIG_H
