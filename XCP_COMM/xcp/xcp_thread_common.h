/**
 * @file xcp_thread_common.h
 * @brief Common XCP thread base classes
 * 
 * Provides common functionality for XCP threads
 */

#ifndef XCP_THREAD_COMMON_H
#define XCP_THREAD_COMMON_H

#include <QObject>
#include <QThread>
#include <QMutex>
#include <QWaitCondition>
#include <atomic>
#include "xcp_common.h"
#include "common/smart_ptr.h"

namespace xcp {

/**
 * @brief Common XCP read thread base class
 * 
 * Provides common functionality for all XCP read threads
 */
class XCP_R_Thread_Common : public QThread
{
    Q_OBJECT

public:
    explicit XCP_R_Thread_Common(QObject* parent = nullptr);
    virtual ~XCP_R_Thread_Common();

    /**
     * @brief Set stop flag
     */
    void setIsStop(bool value);

    /**
     * @brief Set response data ready flag
     */
    void setResDataReady(bool ready);

    /**
     * @brief Get response data ready flag
     */
    bool getResDataReady() const;

    /**
     * @brief Set receive buffer size
     */
    void setRcvBufSize(int size);

    /**
     * @brief Set slave to master ID
     */
    void setIdXcpSlaveToMaster(quint32 id);

    /**
     * @brief Set event ID list
     */
    void setEventIdList(const QList<quint32>& ids);

    /**
     * @brief Set CAN FD mode
     */
    void setIsCanFd(bool isFd);

    /**
     * @brief Set maximum DLC
     */
    void setMaxDLC(quint8 dlc);

    /**
     * @brief Start receiving
     */
    virtual void startReceiving() = 0;

    /**
     * @brief Stop receiving
     */
    virtual void stopReceiving() = 0;

protected:
    // Thread synchronization
    QMutex m_mutex;
    QWaitCondition m_condition;
    std::atomic<bool> m_isStop{false};
    std::atomic<bool> m_resDataReady{false};
    int m_rcvBufSize = 100;
    quint32 m_idXcpSlaveToMaster = 0;
    QList<quint32> m_eventIdList;
    bool m_isCanFd = false;
    quint8 m_maxDLC = 8;
    quint64 m_lastTimestamp = 0;

signals:
    /**
     * @brief Emitted when response data is ready
     */
    void RESDataReady(ByteArrayPtr data, quint32 numBytes);

    /**
     * @brief Emitted when ODT data is ready
     */
    void ODTDataReady(ByteArrayPtr data, quint32 numBytes);

    /**
     * @brief Emitted when frame timestamp changes
     */
    void curFrameTimeChanged(quint64 timeStamp);
};

/**
 * @brief Common XCP write thread base class
 * 
 * Provides common functionality for all XCP write threads
 */
class XCP_W_Thread_Common : public QThread
{
    Q_OBJECT

public:
    explicit XCP_W_Thread_Common(QObject* parent = nullptr);
    virtual ~XCP_W_Thread_Common();

    /**
     * @brief Set write once enable
     */
    void setWriteOnceEnable(bool enable);

    /**
     * @brief Check if write was successful
     */
    bool writeOnceSucceed() const;

    /**
     * @brief Set command payload
     */
    void setCMDPayLoad(quint8* data, quint32 numBytes);

    /**
     * @brief Set master to slave ID
     */
    void setIdMasterToSlave(quint32 id);

    /**
     * @brief Set CAN FD mode
     */
    void setIsCanFd(bool isFd);

    /**
     * @brief Set maximum DLC
     */
    void setMaxDLC(quint8 dlc);

protected:
    /**
     * @brief Pack and send data
     */
    virtual bool packAndSend() = 0;

    // Thread synchronization
    QMutex m_mutex;
    QWaitCondition m_condition;
    std::atomic<bool> m_isStop{false};
    std::atomic<bool> m_writeOnceEnable{false};
    std::atomic<bool> m_writeSucceed{false};
    quint32 m_idMasterToSlave = 0;
    bool m_isCanFd = false;
    quint8 m_maxDLC = 8;
    QByteArray m_payload;

signals:
    /**
     * @brief Emitted when command data is sent
     */
    void CMDDataSend(ByteArrayPtr data, quint32 numBytes);
};

/**
 * @brief Common XCP thread manager base class
 * 
 * Provides common functionality for all XCP thread managers
 */
class XCP_Thread_Manager_Common : public QObject
{
    Q_OBJECT

public:
    explicit XCP_Thread_Manager_Common(QObject* parent = nullptr);
    virtual ~XCP_Thread_Manager_Common();

    // Lifecycle management
    virtual bool XCP_CAN_Init() = 0;
    virtual void XCP_CAN_Stop() = 0;
    virtual bool isInitialized() const = 0;

    // Configuration interface
    virtual void setCanInterface(const QString& interfaceName) = 0;
    virtual void setBaudRate(quint64 baudRate) = 0;
    virtual void setFdBaudRate(quint64 fdBaudRate) = 0;
    virtual void setIsCanFd(bool isFd) = 0;
    virtual void setMaxDLC(quint8 dlc) = 0;
    virtual void setResistanceEnable(int enable) = 0;

    // ID configuration
    virtual void setIdMasterToSlave(quint32 id) = 0;
    virtual void setIdSlaveToMaster(quint32 id) = 0;
    virtual void setEventIdList(const QList<quint32>& ids) = 0;

    // Access sub-threads
    virtual XCP_R_Thread_Common* readThread() = 0;
    virtual XCP_W_Thread_Common* writeThread() = 0;

    // Info query
    virtual QStringList getCANInterface() = 0;

protected:
    QString m_interfaceName;
    quint64 m_baudRate = 500000;
    quint64 m_fdBaudRate = 2000000;
    int m_resistanceEnable = 1;
    bool m_isCanFd = false;
    quint8 m_maxDLC = 8;
    quint32 m_idMasterToSlave = 0;
    quint32 m_idSlaveToMaster = 0;
    QList<quint32> m_eventIdList;
    bool m_initialized = false;
};

} // namespace xcp

#endif // XCP_THREAD_COMMON_H