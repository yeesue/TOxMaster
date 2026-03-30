#ifndef NI_UDS_WORKER_H
#define NI_UDS_WORKER_H

#include <QObject>
#include <QTimer>
#include "NIDiagCS.h"
#include "uds_config.h"


void displayError(qint32 status, const char *source);


class NI_UDS_Worker : public QObject
{
    Q_OBJECT
public:
    explicit NI_UDS_Worker(QObject *parent = nullptr);
    ~NI_UDS_Worker();

    void uds_demo();

    bool uds_run();
    bool uds_exit();

    bool uds_start();
    bool uds_stop();

    void setUdsInfo(const UDSInfo &value);

    bool getIsActive() const;

    bool uds_manl_diagSessionControl(quint8 sessionMode);

    bool uds_manl_saftyPass(quint8 accessMode, quint8 *seed, quint16 lenOfSeed, quint8 *key, quint16 lenOfKey);

    bool uds_manl_eolModeIn(quint8 accessMode, quint8 *seed, quint16 lenOfSeed, quint8 *key, quint16 lenOfKey);
    bool uds_manl_eolModeOut(quint8 accessMode, quint8 *seed, quint16 lenOfSeed, quint8 *key, quint16 lenOfKey);

    bool uds_manl_readDataByIdentifier(quint16 did, quint8 *data, qint32 *len);
    bool uds_manl_writeDataByIdentifier(quint16 did, quint8 *data, quint32 len);
    bool uds_manl_ioControlByIdentifier(quint16 did, quint8 mode, quint8 *dataIn, quint32 lenIn, quint8 *dataOut, quint32 lenOut);
    bool uds_manl_routineControl(quint16 did, quint8 mode, quint8 *dataIn, quint32 lenIn, quint8 *dataOut, quint32 lenOut);

    bool uds_manl_testPresented(bool responseRequired);

    bool uds_manl_clearDiagInfo(quint32 groupOfDTC);

    bool uds_manl_clearDiagInfo_V2(quint32 groupOfDTC, quint32 timeOutRspPending);

    bool uds_manl_readDiagInfo(quint8 mask, quint16 groupOfDTC, quint32 *dtcLen, quint32 *dtcBuf);

    bool uds_manl_diagFrameSendAndRcv( quint8 *dataIn, qint32 lenIn, quint8 *dataOut, qint32 *lenOut);

public slots:
    void Slt_udsTester();

signals:

private:
    void readSetting();

    bool uds_openDiagnostic(QString intf, quint64 baudrate, quint64 baudrate_canfd,
                            quint32 ioMode, quint16 transportProtocol, quint16 maxDLC,
                            quint32 transmitID, quint32 receiveID, TD1 *diagRef);

    bool uds_closeDiagnostic(TD1 *diagRef);

    bool uds_diagSessionControl(TD1 *diagRef, quint8 sessionMode);

    bool uds_testPresented(TD1 *diagRef, bool reponseRequired);

    bool uds_clearDiagInfo(TD1 *diagRef, quint32 groupOfDTC);

    bool uds_readDiagInfo(TD1 *diagRef, quint8 mask, quint16 groupOfDTC, TD4 *DTCs, qint32 *len);

    bool uds_requestSeed(TD1 *diagRef, quint8 accessMode, quint8 *seed, qint32 len);
    bool uds_sendKey(TD1 *diagRef, quint8 accessMode, quint8 *key, qint32 len);

    void uds_seed2Key_g3_peb(quint32 securityConstant, quint8 *seed, quint8 lenOfSeed, quint8 *key, quint8 lenOfKey);
    void uds_seed2Key_g3_pcu(quint8 *seed, quint16 lenOfSeed, quint8 *key, quint16 *lenOfKeys);


    bool uds_readDataByIdentifier(TD1 *diagRef, quint16 id, quint8 *data, qint32 *len);
    bool uds_writeDataByIdentifier(TD1 *diagRef, quint16 id, quint8 *data, qint32 len);

    bool uds_IOControlByIdentifier(TD1 *diagRef, quint16 id, quint8 mode, quint8 *dataIn, qint32 lenIn, quint8 *dataOut, qint32 lenOut);

    bool uds_routineControl(TD1 *diagRef, quint16 id, quint8 mode, quint8 *dataIn, qint32 lenIn, quint8 *dataOut, qint32 lenOut);

    qint32 Seed2Key_g3_edu(QString dllPath, quint32 VenderCode, quint8 *seed, quint16 sizeSeed, quint8 *key, quint16 *sizeKey, quint16 algorithmNumber);

    bool uds_diagFrameSend(TD1 *diagRef, quint8 *dataIn, qint32 len);
    bool uds_diagFrameRecv(TD1 *diagRef, quint8 *dataOut, qint32 *len);
private:

    UDSInfo udsInfo;
    TD1 *udsDiagRef = NULL;

    bool isActive = false;
    bool synFlag = false;

    quint8 accessMode = 0;  // 0 = default mode, 1 = ECU programming session, 2 = ECU extented diagnostic session

    quint8 saftyLevel = 0; // 0 = lv1, 1 = lv2, 2 = lv3
    quint8 seed[4];
    quint8 key[4];

    QTimer *testerTimer = NULL;
};

#endif // NI_UDS_WORKER_H
