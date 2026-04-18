/**
 * @file xcp_thread_unified.h
 * @brief Unified XCP thread implementation based on ICanDriver
 */

#ifndef XCP_THREAD_UNIFIED_H
#define XCP_THREAD_UNIFIED_H

#include "xcp_thread_base.h"
#include "daq_processor.h"
#include "drivers/interfaces/can_driver.h"
#include <QMutex>
#include <QWaitCondition>
#include <atomic>

namespace xcp {

/**
 * @brief Unified read thread implementation
 */
class XCP_R_Thread_Unified : public XCP_R_Thread_Base
{
    Q_OBJECT

public:
    explicit XCP_R_Thread_Unified(can::ICanDriver* driver, QObject* parent = nullptr);
    ~XCP_R_Thread_Unified();

    void setIsStop(bool value) override;
    void setResDataReady(bool ready) override;
    bool getResDataReady() const override;
    void setRcvBufSize(int size) override;
    void setIdXcpSlaveToMaster(quint32 id) override;
    void setEventIdList(const QList<quint32>& ids) override;
    
    void setIsCanFd(bool isFd);
    void setMaxDLC(quint8 dlc);

    void startReceiving();
    void stopReceiving();

protected:
    void run() override;

private slots:
    void onFrameReceived(const can::CanFrame& frame);

private:
    void processFrame(const can::CanFrame& frame);
    void processDaqFrame(const can::CanFrame& frame, quint8 pid);
    quint8* createTimestampedData(const can::CanFrame& frame, int& totalSize);

    can::ICanDriver* m_driver;
    std::atomic<bool> m_isStop{false};
    std::atomic<bool> m_resDataReady{false};
    int m_rcvBufSize = 100;
    quint32 m_idXcpSlaveToMaster = 0;
    QList<quint32> m_eventIdList;
    quint64 m_lastTimestamp = 0;
    bool m_isCanFd = false;
    quint8 m_maxDLC = 8;
    QFuture<void> m_receiveFuture;
};

/**
 * @brief Unified write thread implementation
 */
class XCP_W_Thread_Unified : public XCP_W_Thread_Base
{
    Q_OBJECT

public:
    explicit XCP_W_Thread_Unified(can::ICanDriver* driver, QObject* parent = nullptr);
    ~XCP_W_Thread_Unified();

    void setIsStop(bool value);
    void setWriteOnceEnable(bool enable) override;
    bool writeOnceSucceed() const override;
    void setCMDPayLoad(quint8* data, quint32 numBytes) override;
    
    void setIdMasterToSlave(quint32 id);
    void setIsCanFd(bool isFd);
    void setMaxDLC(quint8 dlc);

protected:
    void run() override;

private slots:
    void sendPayload();

private:
    bool packAndSend();

    can::ICanDriver* m_driver;
    std::atomic<bool> m_isStop{false};
    std::atomic<bool> m_writeOnceEnable{false};
    std::atomic<bool> m_writeSucceed{false};
    
    quint32 m_idMasterToSlave = 0;
    bool m_isCanFd = false;
    quint8 m_maxDLC = 8;
    
    QByteArray m_payload;
    QMutex m_payloadMutex;
    QWaitCondition m_payloadCondition;
    QFuture<void> m_sendFuture;
};

/**
 * @brief Unified XCP thread manager
 */
class XCP_Thread_Unified : public XCP_Thread_Base
{
    Q_OBJECT

public:
    explicit XCP_Thread_Unified(can::CanDriverFactory::DriverType driverType, QObject* parent = nullptr);
    explicit XCP_Thread_Unified(can::ICanDriver* driver, QObject* parent = nullptr);
    ~XCP_Thread_Unified();

    // Lifecycle management
    bool XCP_CAN_Init() override;
    void XCP_CAN_Stop() override;
    bool isInitialized() const override;

    // Configuration interface
    void setCanInterface(const QString& interfaceName) override;
    void setBaudRate(quint64 baudRate) override;
    void setFdBaudRate(quint64 fdBaudRate) override;
    void setIsCanFd(bool isFd) override;
    void setMaxDLC(quint8 dlc) override;
    void setResistanceEnable(int enable) override;
    
    // ID configuration
    void setIdMasterToSlave(quint32 id) override;
    void setIdSlaveToMaster(quint32 id) override;
    void setEventIdList(const QList<quint32>& ids) override;

    // Device configuration (for ZLG etc.)
    void setDeviceType(quint32 type) override;
    void setDeviceIndex(quint32 index) override;
    void setCanIndex(quint32 index) override;

    // Access sub-threads
    XCP_R_Thread_Base* readThread() override;
    XCP_W_Thread_Base* writeThread() override;

    // Info query
    QStringList getCANInterface() override;

    // Get underlying driver (for advanced operations)
    can::ICanDriver* driver() const { return m_driver; }

private:
    bool initDriver();
    void setupBaudRate();

    can::ICanDriver* m_driver = nullptr;
    bool m_ownsDriver = false;
    
    XCP_R_Thread_Unified* m_readThread = nullptr;
    XCP_W_Thread_Unified* m_writeThread = nullptr;
    
    QString m_interfaceName;
    quint64 m_baudRate = 500000;
    quint64 m_fdBaudRate = 2000000;
    int m_resistanceEnable = 1;
    
    quint32 m_idMasterToSlave = 0;
    quint32 m_idSlaveToMaster = 0;
    QList<quint32> m_eventIdList;
    
    // Device specific configuration
    quint32 m_deviceType = 0;
    quint32 m_deviceIndex = 0;
    quint32 m_canIndex = 0;
};

} // namespace xcp

#endif // XCP_THREAD_UNIFIED_H
