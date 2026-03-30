/**
 * @file zlg_can_driver.h
 * @brief ZLG CAN driver implementation
 */

#ifndef ZLG_CAN_DRIVER_H
#define ZLG_CAN_DRIVER_H

#include "../interfaces/can_driver.h"
#include <QMutex>

// ZLG type forward declarations
#define INVALID_DEVICE_HANDLE 0
#define INVALID_CHANNEL_HANDLE 0
#define CAN_EFF_FLAG 0x80000000U
#define CAN_EFF_MASK 0x1FFFFFFFU
#define CANFD_BRS 0x01
#define CANFD_ESI 0x02
#define TYPE_CAN 0
#define TYPE_CANFD 1

typedef quint32 DEVICE_HANDLE;
typedef quint32 CHANNEL_HANDLE;

// ZLG frame structure definitions
struct ZCAN_Receive_Data {
    struct {
        quint32 can_id;
        quint8 can_dlc;
        quint8 data[8];
    } frame;
    quint64 timestamp;
};

struct ZCAN_Transmit_Data {
    struct {
        quint32 can_id;
        quint8 can_dlc;
        quint8 data[8];
    } frame;
    quint8 transmit_type;
};

struct ZCAN_ReceiveFD_Data {
    struct {
        quint32 can_id;
        quint8 flags;
        quint8 len;
        quint8 data[64];
    } frame;
    quint64 timestamp;
};

struct ZCAN_TransmitFD_Data {
    struct {
        quint32 can_id;
        quint8 flags;
        quint8 len;
        quint8 data[64];
    } frame;
    quint8 transmit_type;
};

struct ZCAN_DEVICE_INFO {
    quint16 hw_Version;
    quint16 fw_Version;
    quint16 dr_Version;
    quint16 in_Version;
    quint16 ir_Version;
    quint8 can_Num;
};

namespace can {

/**
 * @brief ZLG CAN driver implementation
 */
class ZlgCanDriver : public ICanDriver
{
    Q_OBJECT

public:
    explicit ZlgCanDriver(QObject* parent = nullptr);
    ~ZlgCanDriver() override;
    
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
    // ZLG specific conversion functions
    CanFrame convertFromZlg(const ZCAN_Receive_Data& zlgFrame);
    void convertToZlg(const CanFrame& frame, ZCAN_Transmit_Data& zlgFrame);
    CanFrame convertFromZlgFd(const ZCAN_ReceiveFD_Data& zlgFrame);
    void convertToZlgFd(const CanFrame& frame, ZCAN_TransmitFD_Data& zlgFrame);
    
    // ZLG device type mapping
    quint32 getDeviceTypeCode(const QString& modelName);
    
private:
    DEVICE_HANDLE m_deviceHandle;       ///< Device handle
    CHANNEL_HANDLE m_channelHandle;     ///< Channel handle
    
    DriverState m_state;
    QString m_lastError;
    QString m_currentInterface;
    
    quint32 m_deviceType;
    quint32 m_deviceIndex;
    quint32 m_channelIndex;
    
    bool m_isCanFd;
    BaudRateConfig m_baudConfig;
    
    ZCAN_Receive_Data* m_rxBuffer;      ///< Receive buffer
    ZCAN_ReceiveFD_Data* m_rxFdBuffer;  ///< CAN FD receive buffer
    int m_rxBufSize;
    
    ZCAN_DEVICE_INFO m_deviceInfo;      ///< Device information
    
    mutable QMutex m_mutex;
};

} // namespace can

#endif // ZLG_CAN_DRIVER_H
