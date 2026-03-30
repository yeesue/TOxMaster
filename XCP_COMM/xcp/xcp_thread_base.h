/**
 * @file xcp_thread_base.h
 * @brief XCP thread abstract base classes
 */

#ifndef XCP_THREAD_BASE_H
#define XCP_THREAD_BASE_H

#include <QObject>
#include <QThread>
#include <QList>
#include <QTimer>
#include "xcp_common.h"

namespace xcp {

/**
 * @brief XCP read thread abstract base class
 */
class XCP_R_Thread_Base : public QThread
{
    Q_OBJECT

public:
    explicit XCP_R_Thread_Base(QObject* parent = nullptr) : QThread(parent) {}
    virtual ~XCP_R_Thread_Base() = default;

    // Control interface
    virtual void setIsStop(bool value) = 0;
    virtual void setResDataReady(bool ready) = 0;
    virtual bool getResDataReady() const = 0;
    
    // Configuration interface
    virtual void setRcvBufSize(int size) = 0;
    virtual void setIdXcpSlaveToMaster(quint32 id) = 0;
    virtual void setEventIdList(const QList<quint32>& ids) = 0;

signals:
    void RESDataReady(quint8* data, quint32 numBytes);
    void ODTDataReady(quint8* data, quint32 numBytes);
    void curFrameTimeChanged(quint64 timeStamp);
};

/**
 * @brief XCP write thread abstract base class
 */
class XCP_W_Thread_Base : public QThread
{
    Q_OBJECT

public:
    explicit XCP_W_Thread_Base(QObject* parent = nullptr) : QThread(parent) {}
    virtual ~XCP_W_Thread_Base() = default;

    // Control interface
    virtual void setWriteOnceEnable(bool enable) = 0;
    virtual bool writeOnceSucceed() const = 0;
    
    // Data interface
    virtual void setCMDPayLoad(quint8* data, quint32 numBytes) = 0;

signals:
    void CMDDataSend(quint8* data, quint32 numBytes);
};

/**
 * @brief XCP thread manager base class
 */
class XCP_Thread_Base : public QObject
{
    Q_OBJECT

public:
    explicit XCP_Thread_Base(QObject* parent = nullptr) : QObject(parent) {}
    virtual ~XCP_Thread_Base() = default;

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

    // Device configuration (for ZLG etc.)
    virtual void setDeviceType(quint32 type) { Q_UNUSED(type); }
    virtual void setDeviceIndex(quint32 index) { Q_UNUSED(index); }
    virtual void setCanIndex(quint32 index) { Q_UNUSED(index); }

    // Access sub-threads
    virtual XCP_R_Thread_Base* readThread() = 0;
    virtual XCP_W_Thread_Base* writeThread() = 0;

    // Info query
    virtual QStringList getCANInterface() = 0;
    
    // Property access
    bool isCanFd() const { return m_isCanFd; }
    quint8 maxDLC() const { return m_maxDLC; }

protected:
    bool m_isCanFd = false;
    quint8 m_maxDLC = 8;
    bool m_initialized = false;
};

} // namespace xcp

#endif // XCP_THREAD_BASE_H
