#include "ni_uds_worker.h"
#include <QDebug>
#include <QLibrary>
#include <QCoreApplication>
#include <QSettings>


void displayError(qint32 status, const char *source)
{
    char statusString[1024];
    int size = sizeof(statusString);
    ndStatusToString(status, statusString, &size);
    qDebug()<<"\nError at "<<source<<"\n"<<statusString;
}

NI_UDS_Worker::NI_UDS_Worker(QObject *parent) : QObject(parent)
{

}

NI_UDS_Worker::~NI_UDS_Worker()
{

}

void NI_UDS_Worker::uds_demo()
{
    /*
    udsInfo.canIntf = "CAN4@nixnet";
    udsInfo.baudrate = 500000;
    udsInfo.baudrate_canfd = 2000000;
    udsInfo.canIoMode = 2;
    udsInfo.transportProtocol = 0;
    udsInfo.maxDLC = 64;
    udsInfo.transmitID = 0x7E7;
    udsInfo.receiveID = 0x7EF;
    */

    udsDiagRef = new TD1;

    if(uds_openDiagnostic(udsInfo.canIntf, udsInfo.baudrate, udsInfo.baudrate_canfd,
                          udsInfo.canIoMode, udsInfo.transportProtocol, udsInfo.maxDLC,
                          udsInfo.transmitID, udsInfo.receiveID, udsDiagRef))
    {
        qDebug()<<"[UDS] open diag ok!";
    }
    else
    {
        qDebug()<<"[UDS] open diag error.";
        return;
    }

    accessMode = 0x03;

    uds_diagSessionControl(udsDiagRef, accessMode);

    uds_testPresented(udsDiagRef, true);

    uds_requestSeed(udsDiagRef, 0x01, seed, 4);
    uds_seed2Key_g3_peb(0xD68EE6A2, seed, 4, key, 4);
    qDebug()<<"Seed(0x):"<<QString::number(seed[0], 16)<<QString::number(seed[1], 16)<<QString::number(seed[2], 16)<<QString::number(seed[3], 16);
    qDebug()<<"key(0x):"<<QString::number(key[0], 16)<<QString::number(key[1], 16)<<QString::number(key[2], 16)<<QString::number(key[3], 16);
    uds_sendKey(udsDiagRef, 0x02, key, 4);

    uds_closeDiagnostic(udsDiagRef);
}

bool NI_UDS_Worker::uds_run()
{

    if(!uds_start())
    {
        return false;
    }

    quint8 sessionMode = 0x03;
    if(!uds_manl_diagSessionControl(sessionMode))
    {
        return false;
    }

    quint8 seed[4];
    quint8 key[4];
    //quint32 securityConstant = udsInfo.securityConstant;
    quint8 mode = 0x01;

    if(!uds_manl_saftyPass(mode, seed, 4, key, 4))
    {
        return false;
    }

    /*
    if(!testerTimer)
    {
        testerTimer = new QTimer();
    }
    testerTimer->setInterval(udsInfo.testerCycleTime);
    qDebug()<<"create timer:"<<udsInfo.testerCycleTime;
    connect(testerTimer, SIGNAL(timeout()), this, SLOT(Slt_udsTester()));
    testerTimer->start();
    */

    return true;

}

bool NI_UDS_Worker::uds_exit()
{

    quint8 seed[4];
    quint8 key[4];
    quint8 mode = 0x01;

    uds_manl_eolModeOut(mode, seed, 4, key, 4);

    if(testerTimer)
    {
        testerTimer->stop();
        delete testerTimer;
        testerTimer = NULL;
    }

    return uds_stop();
}

bool NI_UDS_Worker::uds_start()
{
    udsDiagRef = new TD1;

    if(uds_openDiagnostic(udsInfo.canIntf, udsInfo.baudrate, udsInfo.baudrate_canfd,
                          udsInfo.canIoMode, udsInfo.transportProtocol, udsInfo.maxDLC,
                          udsInfo.transmitID, udsInfo.receiveID, udsDiagRef))
    {
        qDebug()<<"[UDS] open diag ok!";
        return true;
    }
    else
    {
        qDebug()<<"[UDS] open diag error.";
        return false;
    }
}

bool NI_UDS_Worker::uds_stop()
{
    if(!udsDiagRef) return true;

    if(uds_closeDiagnostic(udsDiagRef))
    {
        qDebug()<<"[UDS] close diag ok!";

        return true;
    }
    else
    {
        qDebug()<<"[UDS] close diag error.";
        return false;
    }
}

void NI_UDS_Worker::setUdsInfo(const UDSInfo &value)
{
    udsInfo = value;
}

bool NI_UDS_Worker::getIsActive() const
{
    return isActive;
}

bool NI_UDS_Worker::uds_manl_diagSessionControl(quint8 sessionMode)
{
    return uds_diagSessionControl(udsDiagRef, sessionMode);
}

bool NI_UDS_Worker::uds_manl_saftyPass(quint8 accessMode, quint8 *seed, quint16 lenOfSeed, quint8 *key, quint16 lenOfKey)
{

    if(!seed || !key) return false;

    if(!uds_requestSeed(udsDiagRef, accessMode, seed, lenOfSeed))
    {
        qDebug()<<"request seed error.";
        return false;
    }

    qDebug()<<"Seed(0x):"<<QString::number(seed[0], 16)<<QString::number(seed[1], 16)<<QString::number(seed[2], 16)<<QString::number(seed[3], 16);

    switch (udsInfo.seedKeyType) {
    case 0:
        uds_seed2Key_g3_peb(0xD68EE6A2, seed, lenOfSeed, key, lenOfKey);
        break;
    case 1:
        uds_seed2Key_g3_pcu(seed, lenOfSeed, key, &lenOfKey);
        break;
    case 2:
        uds_seed2Key_g3_peb(udsInfo.securityConstant, seed, lenOfSeed, key, lenOfKey);
        break;
    default:
        break;
    }

    qDebug()<<"Key(0x):"<<QString::number(key[0], 16)<<QString::number(key[1], 16)<<QString::number(key[2], 16)<<QString::number(key[3], 16);

    if(!uds_sendKey(udsDiagRef, accessMode + 1, key, lenOfKey))
    {
        qDebug()<<"send key error.";
        return false;
    }

    if(udsInfo.seedKeyType == 0)
    {
        quint8 dataIn[1];
        dataIn[0] = 0x01;
        quint8 dataOut[4];
        int lenOut = 4;

        uds_manl_routineControl(0xAE08, 0x01, dataIn, 1, dataOut, lenOut);
        uds_manl_routineControl(0xAE09, 0x01, dataIn, 1, dataOut, lenOut);
    }
    else if(udsInfo.seedKeyType == 1)
    {
        quint8 dataIn[1];
        dataIn[0] = 0x01;
        quint8 dataOut[4];
        int lenOut = 4;
        uds_manl_ioControlByIdentifier(0xE001, 0x03, dataIn, 1, dataOut, lenOut);
        uds_manl_ioControlByIdentifier(0xE001, 0x02, dataIn, 0, dataOut, lenOut);
    }

    return true;
}

bool NI_UDS_Worker::uds_manl_eolModeIn(quint8 accessMode, quint8 *seed, quint16 lenOfSeed, quint8 *key, quint16 lenOfKey)
{
    quint8 sessionMode = 0x03;
    if(!uds_manl_diagSessionControl(sessionMode))
    {
        return false;
    }

    if(!seed || !key) return false;

    if(!uds_requestSeed(udsDiagRef, accessMode, seed, lenOfSeed))
    {
        qDebug()<<"request seed error.";
        return false;
    }

    qDebug()<<"Seed(0x):"<<QString::number(seed[0], 16)<<QString::number(seed[1], 16)<<QString::number(seed[2], 16)<<QString::number(seed[3], 16);

    switch (udsInfo.seedKeyType) {
    case 0:
        uds_seed2Key_g3_peb(0xD68EE6A2, seed, lenOfSeed, key, lenOfKey);
        break;
    case 1:
        uds_seed2Key_g3_pcu(seed, lenOfSeed, key, &lenOfKey);
        break;
    case 2:
        uds_seed2Key_g3_peb(udsInfo.securityConstant, seed, lenOfSeed, key, lenOfKey);
        break;
    default:
        break;
    }

    qDebug()<<"Key(0x):"<<QString::number(key[0], 16)<<QString::number(key[1], 16)<<QString::number(key[2], 16)<<QString::number(key[3], 16);

    if(!uds_sendKey(udsDiagRef, accessMode + 1, key, lenOfKey))
    {
        qDebug()<<"send key error.";
        return false;
    }

    if(udsInfo.seedKeyType == 0)
    {
        quint8 dataIn[1];
        dataIn[0] = 0x01;
        quint8 dataOut[4];
        int lenOut = 4;

        uds_manl_routineControl(0xAE08, 0x01, dataIn, 1, dataOut, lenOut);
        uds_manl_routineControl(0xAE09, 0x01, dataIn, 1, dataOut, lenOut);
    }
    else if(udsInfo.seedKeyType == 1)
    {
        quint8 dataIn[1];
        dataIn[0] = 0x01;
        quint8 dataOut[4];
        int lenOut = 4;
        uds_manl_ioControlByIdentifier(0xE001, 0x03, dataIn, 1, dataOut, lenOut);
        uds_manl_ioControlByIdentifier(0xE001, 0x02, dataIn, 0, dataOut, lenOut);
    }

    return true;
}

bool NI_UDS_Worker::uds_manl_eolModeOut(quint8 accessMode, quint8 *seed, quint16 lenOfSeed, quint8 *key, quint16 lenOfKey)
{
    quint8 sessionMode = 0x03;
    if(!uds_manl_diagSessionControl(sessionMode))
    {
        return false;
    }

    if(!seed || !key) return false;

    if(!uds_requestSeed(udsDiagRef, accessMode, seed, lenOfSeed))
    {
        qDebug()<<"request seed error.";
        return false;
    }

    qDebug()<<"Seed(0x):"<<QString::number(seed[0], 16)<<QString::number(seed[1], 16)<<QString::number(seed[2], 16)<<QString::number(seed[3], 16);

    switch (udsInfo.seedKeyType) {
    case 0:
        uds_seed2Key_g3_peb(0xD68EE6A2, seed, lenOfSeed, key, lenOfKey);
        break;
    case 1:
        uds_seed2Key_g3_pcu(seed, lenOfSeed, key, &lenOfKey);
        break;
    case 2:
        uds_seed2Key_g3_peb(udsInfo.securityConstant, seed, lenOfSeed, key, lenOfKey);
        break;
    default:
        break;
    }

    qDebug()<<"Key(0x):"<<QString::number(key[0], 16)<<QString::number(key[1], 16)<<QString::number(key[2], 16)<<QString::number(key[3], 16);

    if(!uds_sendKey(udsDiagRef, accessMode + 1, key, lenOfKey))
    {
        qDebug()<<"send key error.";
        return false;
    }

    if(udsInfo.seedKeyType == 0)
    {
        quint8 dataIn[1];
        dataIn[0] = 0x00;
        quint8 dataOut[4];
        int lenOut = 4;

        uds_manl_routineControl(0xAE08, 0x01, dataIn, 1, dataOut, lenOut);

    }
    else if(udsInfo.seedKeyType == 1)
    {
        quint8 dataIn[1];
        dataIn[0] = 0x00;
        quint8 dataOut[4];
        int lenOut = 4;
        uds_manl_ioControlByIdentifier(0xE001, 0x01, dataIn, 0, dataOut, lenOut);
    }

    return true;
}

bool NI_UDS_Worker::uds_manl_readDataByIdentifier(quint16 did, quint8 *data, qint32 *len)
{
    return uds_readDataByIdentifier(udsDiagRef, did, data, len);
}

bool NI_UDS_Worker::uds_manl_writeDataByIdentifier(quint16 did, quint8 *data, quint32 len)
{
    return uds_writeDataByIdentifier(udsDiagRef, did, data, len);
}

bool NI_UDS_Worker::uds_manl_ioControlByIdentifier(quint16 did, quint8 mode, quint8 *dataIn, quint32 lenIn, quint8 *dataOut, quint32 lenOut)
{
    return uds_IOControlByIdentifier(udsDiagRef, did, mode, dataIn, lenIn, dataOut, lenOut);
}

bool NI_UDS_Worker::uds_manl_routineControl(quint16 did, quint8 mode, quint8 *dataIn, quint32 lenIn, quint8 *dataOut, quint32 lenOut)
{
    return uds_routineControl(udsDiagRef, did, mode, dataIn, lenIn, dataOut, lenOut);
}

bool NI_UDS_Worker::uds_manl_testPresented(bool responseRequired)
{
    return uds_testPresented(udsDiagRef, responseRequired);
}

bool NI_UDS_Worker::uds_manl_clearDiagInfo(quint32 groupOfDTC)
{
    return uds_clearDiagInfo(udsDiagRef, groupOfDTC);
}

bool NI_UDS_Worker::uds_manl_clearDiagInfo_V2(quint32 groupOfDTC, quint32 timeOutRspPending)
{
    ndDiagSetProperty(udsDiagRef, 16, timeOutRspPending);

    uds_clearDiagInfo(udsDiagRef, groupOfDTC);

    ndDiagSetProperty(udsDiagRef, 16, 1000);   // 1000ms为默认值
}

bool NI_UDS_Worker::uds_manl_readDiagInfo(quint8 mask, quint16 groupOfDTC, quint32 *dtcLen, quint32 *dtcBuf)
{
    TD4 DTCs[24];
    bool status = uds_readDiagInfo(udsDiagRef, mask, groupOfDTC, DTCs, (qint32*)dtcLen);

    if(status)
    {
        int len = *dtcLen > 12 ? 12 : *dtcLen;
        for(int i = 0; i < len; i++)
        {
            dtcBuf[i] = DTCs[i].DTC;
        }
    }

    return status;
}

bool NI_UDS_Worker::uds_manl_diagFrameSendAndRcv(quint8 *dataIn, qint32 lenIn, quint8 *dataOut, qint32 *lenOut)
{
    if(!uds_diagFrameSend(udsDiagRef, dataIn, lenIn))
    {
        qDebug()<<"diagFrameSend fail.";
        return false;
    }

    if(!uds_diagFrameRecv(udsDiagRef, dataOut, lenOut))
    {
        qDebug()<<"diagFrameRecv fail.";
        return false;
    }

    return true;

}

void NI_UDS_Worker::Slt_udsTester()
{
    uds_manl_testPresented(udsInfo.testerResponseRequired);
}

void NI_UDS_Worker::readSetting()
{
    QString appPath = QCoreApplication::applicationDirPath();
    QSettings setting(appPath + "/setting/uds_setting.ini", QSettings::IniFormat);

    setting.beginGroup("UDSSetting");
    udsInfo.canIntf = setting.value("CanIntf").toString() + "@nixnet";

    QStringList canIoModeList;
    canIoModeList << "CAN" << "CAN FD" << "CAN FD BRS";
    udsInfo.canIoMode = canIoModeList.indexOf(setting.value("CanIoMode").toString());

    udsInfo.baudrate = setting.value("Baudrate").toUInt();
    udsInfo.baudrate_canfd = setting.value("Baudrate_CANFD").toUInt();

    bool ok = false;
    udsInfo.transmitID = setting.value("TransmitID").toString().toUInt(&ok, 16);
    udsInfo.receiveID = setting.value("ReceiveID").toString().toUInt(&ok, 16);

    udsInfo.maxDLC = setting.value("maxDLC").toUInt();
    udsInfo.securityConstant = setting.value("SecurityConstant").toString().toUInt(&ok, 16);

    udsInfo.testerActive = setting.value("TesterActive").toBool();
    udsInfo.testerCycleTime = setting.value("TesterCycleTime").toUInt();
    udsInfo.testerResponseRequired = setting.value("TesterResponseRequired").toBool();

    QStringList seedKeyTypeList;
    seedKeyTypeList << "G3_PEB" << "G3_PCU" << "EP33L_BMS";
    udsInfo.seedKeyType = seedKeyTypeList.indexOf(setting.value("SeedKeyMethod").toString());

    udsInfo.byteOrder = setting.value("ByteOrder").toUInt();

    setting.endGroup();

    qDebug()<<"[UDS] read setting done";
}


bool NI_UDS_Worker::uds_openDiagnostic(QString intf, quint64 baudrate, quint64 baudrate_canfd, quint32 ioMode, quint16 transportProtocol, quint16 maxDLC, quint32 transmitID, quint32 receiveID, TD1 *diagRef)
{
    QByteArray tempIntf = intf.toLocal8Bit();
    char* canIntf = tempIntf.data();

    qint32 status = ndOpenDiagnosticOnCANFD(canIntf, baudrate, baudrate_canfd, ioMode, transportProtocol, maxDLC, transmitID, receiveID, diagRef);

    if(status == 0 && diagRef != NULL)
    {
        isActive = true;
        return true;
    }
    else
    {
        displayError(status, "open diag");
        isActive = false;
        return false;
    }
}

bool NI_UDS_Worker::uds_closeDiagnostic(TD1 *diagRef)
{
    if(!diagRef) return false;

    qint32 status = ndCloseDiagnostic(diagRef);

    isActive = false;

    return status == 0 ? true : false;
}

bool NI_UDS_Worker::uds_diagSessionControl(TD1 *diagRef, quint8 sessionMode)
{
    if(!diagRef) return false;

    LVBoolean succeed = false;
    qint32 status = ndUDSDiagnosticSessionControl(diagRef, sessionMode, &succeed);

    if(status == 0 && succeed)
    {
        qDebug()<<"[UDS] diag session control ok!";
        return true;
    }
    else
    {
        displayError(status, "uds diag session control");
        return false;
    }
}

bool NI_UDS_Worker::uds_testPresented(TD1 *diagRef, bool reponseRequired)
{    
    if(!diagRef) return false;
    LVBoolean testResponseRequired = reponseRequired;
    LVBoolean testSucceed = false;
    qint32 status = ndUDSTesterPresent(diagRef, &testResponseRequired, &testSucceed);
    if(status == 0 && testSucceed)
    {
        return true;
    }
    else
    {
        displayError(status, "test presented");
        return false;
    }
}

bool NI_UDS_Worker::uds_clearDiagInfo(TD1 *diagRef, quint32 groupOfDTC)
{
    if(!diagRef) return false;
    LVBoolean testSucceed = false;

    TD3 *dtcDescrp = new TD3;
    dtcDescrp->AddDataByteLength = 0;
    dtcDescrp->ByteOrder = 0;   // 0 = moto, 1 = intel
    dtcDescrp->DTCByteLength = 3;
    dtcDescrp->StatusByteLength = 1;

    qint32 status = ndUDSClearDiagnosticInformation(diagRef, groupOfDTC, dtcDescrp, &testSucceed);
    if(status == 0 && testSucceed)
    {
        return true;
    }
    else
    {
        displayError(status, "clear diagnostic information");
        return false;
    }
}

bool NI_UDS_Worker::uds_readDiagInfo(TD1 *diagRef, quint8 mask, quint16 groupOfDTC, TD4 *DTCs, qint32 *len)
{
    if(!diagRef) return false;
    LVBoolean testSucceed = false;

    TD3 *dtcDescrp = new TD3;
    dtcDescrp->AddDataByteLength = 0;
    dtcDescrp->ByteOrder = 0;   // 0 = moto, 1 = intel
    dtcDescrp->DTCByteLength = 3;
    dtcDescrp->StatusByteLength = 1;

    quint8 availMask = 0x00;

    // mask:01=当前故障 / 08=历史故障 / 09=两者
    qint32 status = ndUDSReportDTCByStatusMask(diagRef, mask, dtcDescrp, &availMask, DTCs, len, &testSucceed);
    if(status == 0 && testSucceed)
    {
        qDebug()<<"Mask:"<<mask<<",Read DTC num:"<<*len<<", availMask:"<<availMask
               <<", DTCs[0]:"<<DTCs[0].DTC<<", DTCs[1]:"<<DTCs[1].DTC<<", DTCs[2]:"<<DTCs[2].DTC<<", DTCs[3]:"<<DTCs[3].DTC;
        for(int i = 0; i < (*len); i++)
        {
            qDebug()<<"DTC["<<i<<"]: 0x"<<QString::number(DTCs[i].DTC, 16).toUpper();
        }
        return true;
    }
    else
    {
        displayError(status, "read diagnostic information");
        return false;
    }
}

bool NI_UDS_Worker::uds_requestSeed(TD1 *diagRef, quint8 accessMode, quint8 *seed, qint32 len)
{
    if(!diagRef) return false;

    LVBoolean succeed = false;

    qint32 status = ndUDSRequestSeed(diagRef, accessMode, seed, &len, &succeed);
    if(status == 0 && succeed)
    {
        qDebug()<<"[UDS] request seed ok!";
        return true;
    }
    else
    {
        displayError(status, "reques seed");
        return false;
    }
}

bool NI_UDS_Worker::uds_sendKey(TD1 *diagRef, quint8 accessMode, quint8 *key, qint32 len)
{
    if(!diagRef) return false;

    LVBoolean succeed = false;

    qint32 status = ndUDSSendKey(diagRef, accessMode, key, len, &succeed);
    if(status == 0 && succeed)
    {
        qDebug()<<"[UDS] send key ok!";
        return true;
    }
    else
    {
        displayError(status, "send key");
        return false;
    }
}

void NI_UDS_Worker::uds_seed2Key_g3_peb(quint32 securityConstant, quint8 *seed, quint8 lenOfSeed, quint8 *key, quint8 lenOfKey)
{
    if(!seed || !key || lenOfSeed != 4 || lenOfKey != 4) return;

    //quint32 SECURITYCONSTANT_EXT = 0xD18E0306;   // bms constant
    //quint32 SECURITYCONSTANT_EXT = 0xD68EE6A2;   // g3 peb constant (lv1:0xD68EE6A2 ; lv2:0x518C34FB ; lv3:0x568C0177)

    quint32 SECURITYCONSTANT_EXT = securityConstant;

    quint8  iterations = 0;
    quint32 wLastSeed = 0;
    quint32 wTemp = 0;
    quint32 wLSBit = 0;
    quint32 wTop31Bits = 0;
    quint8 jj = 0, SB1 = 0, SB2 = 0, SB3 = 0;
    int temp = 0;

    quint32 wSeed = 0;
    wSeed |= seed[0] << 24;
    wSeed |= seed[1] << 16;
    wSeed |= seed[2] << 8;
    wSeed |= seed[3];


    wLastSeed = wSeed;

    temp = (int)((SECURITYCONSTANT_EXT & 0x00000800) >> 10) | ((SECURITYCONSTANT_EXT & 0x00200000) >> 21);
    //qDebug()<<"temp:"<<temp;

    switch (temp) {
    case 0:
        wTemp = (quint8)((wSeed & 0xFF000000) >> 24);
        break;
    case 1:
        wTemp = (quint8)((wSeed & 0x00FF0000) >> 16);
        break;
    case 2:
        wTemp = (quint8)((wSeed & 0x0000FF00) >> 8);
        break;
    case 3:
        wTemp = (quint8)(wSeed & 0x000000FF);
        break;
    default:
        break;
    }

    //qDebug()<<"wTemp:"<<wTemp;

    SB1 = (quint8)((SECURITYCONSTANT_EXT & 0x000003FC) >> 2);
    SB2 = (quint8)(((SECURITYCONSTANT_EXT & 0x7F800000) >> 23) ^ 0x000000A5);
    SB3 = (quint8)(((SECURITYCONSTANT_EXT & 0x001FE000) >> 13) ^ 0x0000005A);

    //qDebug()<<"sb1:"<<SB1<<", sb2:"<<SB2<<", sb3:"<<SB3;

    iterations = (quint8)(((wTemp ^ SB1) & SB2) + SB3);
    //qDebug()<<"iter:"<<iterations;

    for(jj = 0; jj < iterations; jj++)
    {
        wTemp = ((wLastSeed & 0x40000000) / 0x40000000) ^ ((wLastSeed & 0x01000000) / 0x01000000)
                ^ ((wLastSeed & 0x1000) / 0x1000)       ^ ((wLastSeed & 0x04) / 0x04);

        wLSBit = (wTemp & 0x00000001);

        wLastSeed  = (quint32)(wLastSeed << 1);
        wTop31Bits = (quint32)(wLastSeed & 0xFFFFFFFE);
        wLastSeed  = (quint32)(wTop31Bits | wLSBit);
    }

    //qDebug()<<"wLastSeed:"<<wLastSeed<<", wTop31:"<<wTop31Bits;

    if(SECURITYCONSTANT_EXT & 0x00000001)
    {
        wTop31Bits = ((wLastSeed & 0x00FF0000) >> 16) |  // key[0] = Last_Seed[1]
                     ((wLastSeed & 0xFF000000) >> 8)  |  // key[1] = Last_Seed[0]
                     ((wLastSeed & 0x000000FF) << 8)  |  // key[2] = Last_Seed[3]
                     ((wLastSeed & 0x0000FF00) << 16) ;  // key[3] = Last_Seed[2]
    }
    else
    {
        wTop31Bits = wLastSeed;
    }

    wTop31Bits = wTop31Bits ^ SECURITYCONSTANT_EXT;

    //return wTop31Bits;

    key[0] = (wTop31Bits & 0xFF000000) >> 24;
    key[1] = (wTop31Bits & 0x00FF0000) >> 16;
    key[2] = (wTop31Bits & 0x0000FF00) >> 8;
    key[3] = (wTop31Bits & 0x000000FF);

}

void NI_UDS_Worker::uds_seed2Key_g3_pcu(quint8 *seed, quint16 lenOfSeed, quint8 *key, quint16 *lenOfKeys)
{
    QString appPath = QCoreApplication::applicationDirPath();
    QString dllPath = appPath + "/seedKey/Seed2Key_EDU.dll";
    qDebug()<<"g3 pcu seed2Key dll file:"<<dllPath;

    Seed2Key_g3_edu(dllPath, 0x2B503C61, seed, lenOfSeed, key, lenOfKeys, 0x34);
}

bool NI_UDS_Worker::uds_readDataByIdentifier(TD1 *diagRef, quint16 id, quint8 *data, qint32 *len)
{
    if(!diagRef) return false;

    LVBoolean succeed = false;

    qint32 status = ndUDSReadDataByIdentifier(diagRef, id, data, len, &succeed);
    if(status == 0 && succeed)
    {
        //qDebug()<<"0x22 Response Data Len:"<<*len;
        return true;
    }
    else
    {
        displayError(status, "read data by identifier");
        return false;
    }
}

bool NI_UDS_Worker::uds_writeDataByIdentifier(TD1 *diagRef, quint16 id, quint8 *data, qint32 len)
{
    if(!diagRef) return false;

    LVBoolean succeed = false;

    qint32 status = ndUDSWriteDataByIdentifier(diagRef, id, data, len, &succeed);
    if(status == 0 && succeed)
    {
        return true;
    }
    else
    {
        displayError(status, "write data by identifier");
        return false;
    }
}

bool NI_UDS_Worker::uds_IOControlByIdentifier(TD1 *diagRef, quint16 id, quint8 mode,
                                              quint8 *dataIn, qint32 lenIn, quint8 *dataOut, qint32 lenOut)
{
    if(!diagRef) return false;

    LVBoolean succeed = false;

    qint32 status = ndUDSInputOutputControlByIdentifier(diagRef, id, mode, dataIn, lenIn, dataOut, &lenOut, &succeed);
    if(status == 0 && succeed)
    {
        return true;
    }
    else
    {
        displayError(status, "input output control by identifier");
        return false;
    }
}

bool NI_UDS_Worker::uds_routineControl(TD1 *diagRef, quint16 id, quint8 mode,
                                       quint8 *dataIn, qint32 lenIn, quint8 *dataOut, qint32 lenOut)
{
    if(!diagRef) return false;

    LVBoolean succeed = false;

    qint32 status = ndUDSRoutineControl(diagRef, id, mode, dataIn, lenIn, dataOut, &lenOut, &succeed);
    if(status == 0 && succeed)
    {
        return true;
    }
    else
    {
        displayError(status, "routine control");
        return false;
    }
}

bool NI_UDS_Worker::uds_diagFrameSend(TD1 *diagRef, quint8 *dataIn, qint32 len)
{
    if(!diagRef) return false;

    qint32 status = ndDiagFrameSend(diagRef, dataIn, len);
    if(status == 0)
    {
        return true;
    }
    else
    {
        displayError(status, "raw frame send");
        return false;
    }
}

bool NI_UDS_Worker::uds_diagFrameRecv(TD1 *diagRef, quint8 *dataOut, qint32 *len)
{
    if(!diagRef) return false;

    qint32 status = ndDiagFrameRecv(diagRef, 500, dataOut, len);
    if(status == 0)
    {
        return true;
    }
    else
    {
        displayError(status, "raw frame recv");
        return false;
    }
}

typedef qint32 (*SAIC_ComputeKeyFromSeed)(quint32, quint8*, quint16, quint8*, quint16*, quint16);

qint32 NI_UDS_Worker::Seed2Key_g3_edu(QString dllPath,
                                    quint32 VenderCode,
                                    quint8 *seed, quint16 sizeSeed,
                                    quint8 *key, quint16 *sizeKey,
                                    quint16 algorithmNumber)
{
    if(dllPath.isEmpty()) return -1;
    if(!seed || !key || !sizeKey) return -1;

    QLibrary lib(dllPath);

    if(lib.load())
    {
        qDebug()<<"load SeedKey dll ok.";

        SAIC_ComputeKeyFromSeed seed2Key = (SAIC_ComputeKeyFromSeed)lib.resolve("SAIC_ComputeKeyFromSeed");

        if(seed2Key)
        {
            qDebug()<<"resolve SeedKey ok";

            return seed2Key(VenderCode, seed, sizeSeed, key, sizeKey, algorithmNumber);
        }
        else
        {
            qDebug()<<"resolve SeedKey error.";
            qDebug()<<lib.errorString();
        }
    }
    else
    {
        qDebug()<<"load SeedKey error:" + lib.errorString();
    }
}


