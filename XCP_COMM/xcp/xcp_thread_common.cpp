/**
 * @file xcp_thread_common.cpp
 * @brief Common XCP thread base classes implementation
 */

#include "xcp_thread_common.h"

namespace xcp {

/**
 * @brief XCP_R_Thread_Common implementation
 */
XCP_R_Thread_Common::XCP_R_Thread_Common(QObject* parent)
    : QThread(parent)
{}

XCP_R_Thread_Common::~XCP_R_Thread_Common()
{
    // Ensure thread is stopped
    setIsStop(true);
    wait();
}

void XCP_R_Thread_Common::setIsStop(bool value)
{
    m_isStop.store(value);
    if (value) {
        m_condition.wakeAll();
    }
}

void XCP_R_Thread_Common::setResDataReady(bool ready)
{
    m_resDataReady.store(ready);
}

bool XCP_R_Thread_Common::getResDataReady() const
{
    return m_resDataReady.load();
}

void XCP_R_Thread_Common::setRcvBufSize(int size)
{
    m_rcvBufSize = size;
}

void XCP_R_Thread_Common::setIdXcpSlaveToMaster(quint32 id)
{
    m_idXcpSlaveToMaster = id;
}

void XCP_R_Thread_Common::setEventIdList(const QList<quint32>& ids)
{
    m_eventIdList = ids;
}

void XCP_R_Thread_Common::setIsCanFd(bool isFd)
{
    m_isCanFd = isFd;
}

void XCP_R_Thread_Common::setMaxDLC(quint8 dlc)
{
    m_maxDLC = dlc;
}

/**
 * @brief XCP_W_Thread_Common implementation
 */
XCP_W_Thread_Common::XCP_W_Thread_Common(QObject* parent)
    : QThread(parent)
{}

XCP_W_Thread_Common::~XCP_W_Thread_Common()
{
    // Ensure thread is stopped
    m_isStop.store(true);
    m_condition.wakeAll();
    wait();
}

void XCP_W_Thread_Common::setWriteOnceEnable(bool enable)
{
    m_writeOnceEnable.store(enable);
    if (enable) {
        m_condition.wakeOne();
    }
}

bool XCP_W_Thread_Common::writeOnceSucceed() const
{
    return m_writeSucceed.load();
}

void XCP_W_Thread_Common::setCMDPayLoad(quint8* data, quint32 numBytes)
{
    QMutexLocker locker(&m_mutex);
    m_payload.clear();
    m_payload.append(reinterpret_cast<const char*>(data), numBytes);
}

void XCP_W_Thread_Common::setIdMasterToSlave(quint32 id)
{
    m_idMasterToSlave = id;
}

void XCP_W_Thread_Common::setIsCanFd(bool isFd)
{
    m_isCanFd = isFd;
}

void XCP_W_Thread_Common::setMaxDLC(quint8 dlc)
{
    m_maxDLC = dlc;
}

/**
 * @brief XCP_Thread_Manager_Common implementation
 */
XCP_Thread_Manager_Common::XCP_Thread_Manager_Common(QObject* parent)
    : QObject(parent)
{}

XCP_Thread_Manager_Common::~XCP_Thread_Manager_Common()
{
    // Ensure resources are released
    // XCP_CAN_Stop() is a pure virtual function, should be implemented by subclass
}

} // namespace xcp