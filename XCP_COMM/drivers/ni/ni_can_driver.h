/**
 * @file ni_can_driver.h
 * @brief NI-XNET CAN driver implementation
 */

#ifndef NI_CAN_DRIVER_H
#define NI_CAN_DRIVER_H

#include "../interfaces/can_driver.h"
#include <QMutex>

// NI-XNET type forward declarations (actual use requires including nixnet.h)
typedef quint32 nxStatus_t;
typedef quint32 nxSessionRef_t;
typedef quint32 nxDatabaseRef_t;

// NI-XNET frame structure definitions
struct nxFrameCAN_t {
    quint64 Timestamp;
    quint32 Identifier;
    quint8 Type;
    quint8 Flags;
    quint8 Payload[8];
};

struct nxFrameCANFD_t {
    quint64 Timestamp;
    quint32 Identifier;
    quint8 Type;
    quint8 Flags;
    quint8 Payload[64];
};

namespace can {

/**
 * @brief NI-XNET CAN driver implementation
 */
class NiCanDriver : public ICanDriver
{
    Q_OBJECT

public:
    explicit NiCanDriver(QObject* parent = nullptr);
    ~NiCanDriver() override;
    
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
    // NI-XNET specific conversion functions
    nxStatus_t createSession();
    void destroySession();
    CanFrame convertFromNxFrame(const nxFrameCAN_t* nxFrame);
    void convertToNxFrame(const CanFrame& frame, nxFrameCAN_t* nxFrame);
    CanFrame convertFromNxFdFrame(const nxFrameCANFD_t* nxFrame);
    void convertToNxFdFrame(const CanFrame& frame, nxFrameCANFD_t* nxFrame);
    
private:
    nxSessionRef_t m_sessionRx;     ///< Receive session
    nxSessionRef_t m_sessionTx;     ///< Transmit session
    nxDatabaseRef_t m_databaseRef;  ///< Database reference
    
    DriverState m_state;
    QString m_lastError;
    QString m_currentInterface;
    QString m_databasePath;
    QString m_clusterName;
    
    bool m_isCanFd;
    BaudRateConfig m_baudConfig;
    
    nxFrameCAN_t* m_rxBuffer;       ///< Receive buffer
    nxFrameCANFD_t* m_rxFdBuffer;   ///< CAN FD receive buffer
    int m_rxBufSize;
    
    mutable QMutex m_mutex;
};

} // namespace can

#endif // NI_CAN_DRIVER_H
