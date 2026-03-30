/**
 * @file ts_can_driver.h
 * @brief TSCAN CAN driver implementation
 */

#ifndef TS_CAN_DRIVER_H
#define TS_CAN_DRIVER_H

#include "../interfaces/can_driver.h"
#include <QMutex>

// TSCAN type forward declarations
typedef enum {
    CHN1, CHN2, CHN3, CHN4, CHN5, CHN6, CHN7, CHN8
} APP_CHANNEL;

// TSCAN frame structure definitions
typedef struct _TLIBCAN {
    quint8 FIdxChn;
    quint8 FDLC;
    quint8 FReserved;
    qint32 FIdentifier;
    quint64 FTimeUS;
    quint8 FData[8];
    struct {
        quint8 value;
        struct {
            quint8 istx : 1;
            quint8 remoteframe : 1;
            quint8 extframe : 1;
            quint8 tbd : 4;
            quint8 iserrorframe : 1;
        } bits;
    } FProperties;
} TLIBCAN;

typedef struct _TLIBCANFD {
    quint8 FIdxChn;
    quint8 FDLC;
    qint32 FIdentifier;
    quint64 FTimeUS;
    quint8 FData[64];
    struct {
        quint8 value;
        struct {
            quint8 istx : 1;
            quint8 remoteframe : 1;
            quint8 extframe : 1;
            quint8 tbd : 4;
            quint8 iserrorframe : 1;
        } bits;
    } FProperties;
    struct {
        quint8 value;
        struct {
            quint8 EDL : 1;
            quint8 BRS : 1;
            quint8 ESI : 1;
            quint8 tbd : 5;
        } bits;
    } FFDProperties;
} TLIBCANFD;

namespace can {

/**
 * @brief TSCAN CAN driver implementation
 */
class TsCanDriver : public ICanDriver
{
    Q_OBJECT

public:
    explicit TsCanDriver(QObject* parent = nullptr);
    ~TsCanDriver() override;
    
    // ICanDriver interface implementation
    bool open() override;
    void close() override;
    bool isOpen() const override;
    
    bool setBaudRate(const BaudRateConfig& config) override;
    bool setFilters(const QList<CanFilter>& filters) override;
    void setReceiveBufferSize(int size) override;
    
    bool send(const CanFrame& frame, int timeoutMs = 100) override;
    bool receive(CanFrame& frame, int timeoutMs = 100) override;
    int sendBatch(const QList<CanFrame>& frames) override;
    
    DriverState state() const override;
    QString lastError() const override;
    CanDeviceInfo deviceInfo() const override;
    QStringList availableInterfaces() const override;
    bool selectInterface(const QString& interfaceName) override;
    QString currentInterface() const override;
    void clearReceiveBuffer() override;

private:
    // TSCAN specific conversion functions
    CanFrame convertFromTscan(const TLIBCAN& tscanFrame);
    void convertToTscan(const CanFrame& frame, TLIBCAN& tscanFrame);
    CanFrame convertFromTscanFd(const TLIBCANFD& tscanFrame);
    void convertToTscanFd(const CanFrame& frame, TLIBCANFD& tscanFrame);
    
private:
    size_t m_deviceHandle;          ///< Device handle
    APP_CHANNEL m_channel;          ///< CAN channel
    
    DriverState m_state;
    QString m_lastError;
    QString m_currentInterface;
    
    quint32 m_deviceType;
    quint32 m_deviceIndex;
    quint8 m_channelIndex;
    
    bool m_isCanFd;
    BaudRateConfig m_baudConfig;
    
    TLIBCAN* m_rxBuffer;            ///< Receive buffer
    TLIBCANFD* m_rxFdBuffer;        ///< CAN FD receive buffer
    int m_rxBufSize;
    
    mutable QMutex m_mutex;
};

} // namespace can

#endif // TS_CAN_DRIVER_H
