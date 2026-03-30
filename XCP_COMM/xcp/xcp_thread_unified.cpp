/**
 * @file xcp_thread_unified.cpp
 * @brief Unified XCP thread implementation based on ICanDriver
 */

#include "xcp_thread_unified.h"
#include "logger.h"
#include <QCoreApplication>
#include <cstring>

namespace xcp {

// ==================== XCP_R_Thread_Unified ====================

XCP_R_Thread_Unified::XCP_R_Thread_Unified(can::ICanDriver* driver, QObject* parent)
    : XCP_R_Thread_Base(parent)
    , m_driver(driver)
{
}

XCP_R_Thread_Unified::~XCP_R_Thread_Unified()
{
    setIsStop(true);
    requestInterruption();
    quit();
    wait();
}

void XCP_R_Thread_Unified::setIsStop(bool value)
{
    m_isStop.store(value);
}

void XCP_R_Thread_Unified::setResDataReady(bool ready)
{
    m_resDataReady.store(ready);
}

bool XCP_R_Thread_Unified::getResDataReady() const
{
    return m_resDataReady.load();
}

void XCP_R_Thread_Unified::setRcvBufSize(int size)
{
    m_rcvBufSize = size;
    if (m_driver) {
        m_driver->setReceiveBufferSize(size);
    }
}

void XCP_R_Thread_Unified::setIdXcpSlaveToMaster(quint32 id)
{
    m_idXcpSlaveToMaster = id;
}

void XCP_R_Thread_Unified::setEventIdList(const QList<quint32>& ids)
{
    m_eventIdList = ids;
}

void XCP_R_Thread_Unified::setIsCanFd(bool isFd)
{
    m_isCanFd = isFd;
}

void XCP_R_Thread_Unified::setMaxDLC(quint8 dlc)
{
    m_maxDLC = dlc;
}

void XCP_R_Thread_Unified::startReceiving()
{
    if (!isRunning()) {
        m_isStop.store(false);
        start();
    }
}

void XCP_R_Thread_Unified::stopReceiving()
{
    m_isStop.store(true);
}

void XCP_R_Thread_Unified::run()
{
    if (!m_driver || !m_driver->isOpen()) {
        LOG_WARN_STREAM() << "XCP_R_Thread_Unified: Driver not open";
        return;
    }

    LOG_DEBUG_STREAM() << "XCP_R_Thread_Unified: Starting receive loop";

    can::CanFrame frame;
    
    while (!m_isStop.load()) {
        if (m_driver->receive(frame, 10)) {
            processFrame(frame);
        }
    }

    LOG_DEBUG_STREAM() << "XCP_R_Thread_Unified: Receive loop ended";
}

void XCP_R_Thread_Unified::processFrame(const can::CanFrame& frame)
{
    // Check frame ID
    quint32 id = frame.id & 0x1FFFFFFF;
    
    // Check if it is XCP response frame
    if (id == m_idXcpSlaveToMaster) {
        // Check timestamp for duplicate removal
        if (frame.timestamp > 0 && frame.timestamp <= m_lastTimestamp) {
            return;
        }
        m_lastTimestamp = frame.timestamp;
        
        // Dispatch by PID
        quint8 pid = frame.data.isEmpty() ? 0 : static_cast<quint8>(frame.data[0]);
        
        if (pid > 0xFB) {
            // Response frame (RES/ERR): PID = 0xFC, 0xFD, 0xFE, 0xFF
            m_resDataReady.store(true);
            
            // Create complete data packet with timestamp
            int totalSize = frame.data.size() + 8;  // data + timestamp
            quint8* data = new quint8[totalSize];
            
            // First 8 bytes for timestamp
            memcpy(data, &frame.timestamp, 8);
            // Following bytes for data
            memcpy(data + 8, frame.data.constData(), frame.data.size());
            
            emit RESDataReady(data, totalSize);
        } else {
            // DAQ/ODT frame: PID = 0x00 ~ 0xFB
            processDaqFrame(frame, pid);
        }
        
        emit curFrameTimeChanged(frame.timestamp);
        return;
    }
    
    // Check if it is event frame (DAQ event channel)
    if (m_eventIdList.contains(id)) {
        quint8 pid = frame.data.isEmpty() ? 0 : static_cast<quint8>(frame.data[0]);
        processDaqFrame(frame, pid);
        emit curFrameTimeChanged(frame.timestamp);
    }
}

void XCP_R_Thread_Unified::processDaqFrame(const can::CanFrame& frame, quint8 pid)
{
    Q_UNUSED(pid);
    
    // Create complete data packet with timestamp (compatible with old interface)
    int totalSize = frame.data.size() + 8;
    quint8* data = new quint8[totalSize];
    
    // First 8 bytes for timestamp
    memcpy(data, &frame.timestamp, 8);
    // Following bytes for data
    memcpy(data + 8, frame.data.constData(), frame.data.size());
    
    emit ODTDataReady(data, totalSize);
}

// ==================== XCP_W_Thread_Unified ====================

XCP_W_Thread_Unified::XCP_W_Thread_Unified(can::ICanDriver* driver, QObject* parent)
    : XCP_W_Thread_Base(parent)
    , m_driver(driver)
{
}

XCP_W_Thread_Unified::~XCP_W_Thread_Unified()
{
    m_isStop.store(true);
    m_payloadCondition.wakeAll();
    requestInterruption();
    quit();
    wait();
}

void XCP_W_Thread_Unified::setIsStop(bool value)
{
    m_isStop.store(value);
    if (value) {
        m_payloadCondition.wakeAll();
    }
}

void XCP_W_Thread_Unified::setWriteOnceEnable(bool enable)
{
    m_writeOnceEnable.store(enable);
    if (enable) {
        m_payloadCondition.wakeAll();
    }
}

bool XCP_W_Thread_Unified::writeOnceSucceed() const
{
    return m_writeSucceed.load();
}

void XCP_W_Thread_Unified::setCMDPayLoad(quint8* data, quint32 numBytes)
{
    QMutexLocker locker(&m_payloadMutex);
    m_payload = QByteArray(reinterpret_cast<char*>(data), numBytes);
}

void XCP_W_Thread_Unified::setIdMasterToSlave(quint32 id)
{
    m_idMasterToSlave = id;
}

void XCP_W_Thread_Unified::setIsCanFd(bool isFd)
{
    m_isCanFd = isFd;
}

void XCP_W_Thread_Unified::setMaxDLC(quint8 dlc)
{
    m_maxDLC = dlc;
}

void XCP_W_Thread_Unified::run()
{
    if (!m_driver || !m_driver->isOpen()) {
        qWarning() << "XCP_W_Thread_Unified: Driver not open";
        return;
    }

    qDebug() << "XCP_W_Thread_Unified: Starting write thread";

    while (!m_isStop.load()) {
        // Wait for data
        {
            QMutexLocker locker(&m_payloadMutex);
            while (m_payload.isEmpty() && !m_isStop.load() && !m_writeOnceEnable.load()) {
                m_payloadCondition.wait(&m_payloadMutex, 100);
            }
        }

        if (m_isStop.load()) {
            break;
        }

        if (!m_payload.isEmpty()) {
            m_writeSucceed.store(packAndSend());
            
            if (m_writeOnceEnable.load()) {
                m_writeOnceEnable.store(false);
            }
            
            // Clear payload
            QMutexLocker locker(&m_payloadMutex);
            m_payload.clear();
        }
    }

    qDebug() << "XCP_W_Thread_Unified: Write thread ended";
}

bool XCP_W_Thread_Unified::packAndSend()
{
    can::CanFrame frame;
    frame.id = m_idMasterToSlave;
    frame.extended = (m_idMasterToSlave > 0x7FF);
    frame.fd = m_isCanFd;
    frame.data = m_payload;
    
    // Set DLC
    quint8 dlc = qMax<quint8>(m_payload.size(), 8);
    if (m_isCanFd && dlc > 8) {
        frame.dlc = can::CanFrame::lengthToDlc(dlc);
    } else {
        frame.dlc = 8;
    }

    return m_driver->send(frame, 100);
}

// ==================== XCP_Thread_Unified ====================

XCP_Thread_Unified::XCP_Thread_Unified(can::CanDriverFactory::DriverType driverType, QObject* parent)
    : XCP_Thread_Base(parent)
{
    m_driver = can::CanDriverFactory::create(driverType, this);
    m_ownsDriver = (m_driver != nullptr);
    
    if (m_driver) {
        m_readThread = new XCP_R_Thread_Unified(m_driver, this);
        m_writeThread = new XCP_W_Thread_Unified(m_driver, this);
    }
}

XCP_Thread_Unified::XCP_Thread_Unified(can::ICanDriver* driver, QObject* parent)
    : XCP_Thread_Base(parent)
    , m_driver(driver)
    , m_ownsDriver(false)
{
    if (m_driver) {
        m_readThread = new XCP_R_Thread_Unified(m_driver, this);
        m_writeThread = new XCP_W_Thread_Unified(m_driver, this);
    }
}

XCP_Thread_Unified::~XCP_Thread_Unified()
{
    XCP_CAN_Stop();
    
    if (m_readThread) {
        delete m_readThread;
        m_readThread = nullptr;
    }
    if (m_writeThread) {
        delete m_writeThread;
        m_writeThread = nullptr;
    }
    if (m_ownsDriver && m_driver) {
        delete m_driver;
        m_driver = nullptr;
    }
}

bool XCP_Thread_Unified::XCP_CAN_Init()
{
    if (!m_driver) {
        qWarning() << "XCP_Thread_Unified: No driver available";
        return false;
    }

    qDebug() << "XCP_Thread_Unified: Initializing CAN";

    // Select interface
    if (!m_interfaceName.isEmpty()) {
        if (!m_driver->selectInterface(m_interfaceName)) {
            qWarning() << "XCP_Thread_Unified: Failed to select interface" << m_interfaceName;
        }
    }

    // Open device
    if (!m_driver->open()) {
        qWarning() << "XCP_Thread_Unified: Failed to open device:" << m_driver->lastError();
        return false;
    }

    // Configure baud rate
    setupBaudRate();

    // Configure ID
    if (m_readThread) {
        m_readThread->setIdXcpSlaveToMaster(m_idSlaveToMaster);
        m_readThread->setEventIdList(m_eventIdList);
    }
    if (m_writeThread) {
        m_writeThread->setIdMasterToSlave(m_idMasterToSlave);
        m_writeThread->setIsCanFd(m_isCanFd);
        m_writeThread->setMaxDLC(m_maxDLC);
    }

    m_initialized = true;
    
    // Start read/write threads
    if (m_readThread) {
        m_readThread->startReceiving();
    }
    if (m_writeThread) {
        m_writeThread->start();
    }

    qDebug() << "XCP_Thread_Unified: Initialization complete";
    return true;
}

void XCP_Thread_Unified::XCP_CAN_Stop()
{
    if (m_readThread) {
        m_readThread->stopReceiving();
    }
    
    if (m_driver && m_driver->isOpen()) {
        m_driver->close();
    }
    
    m_initialized = false;
    qDebug() << "XCP_Thread_Unified: Stopped";
}

bool XCP_Thread_Unified::isInitialized() const
{
    return m_initialized;
}

void XCP_Thread_Unified::setCanInterface(const QString& interfaceName)
{
    m_interfaceName = interfaceName;
}

void XCP_Thread_Unified::setBaudRate(quint64 baudRate)
{
    m_baudRate = baudRate;
}

void XCP_Thread_Unified::setFdBaudRate(quint64 fdBaudRate)
{
    m_fdBaudRate = fdBaudRate;
}

void XCP_Thread_Unified::setIsCanFd(bool isFd)
{
    m_isCanFd = isFd;
}

void XCP_Thread_Unified::setMaxDLC(quint8 dlc)
{
    m_maxDLC = dlc;
}

void XCP_Thread_Unified::setResistanceEnable(int enable)
{
    m_resistanceEnable = enable;
}

void XCP_Thread_Unified::setIdMasterToSlave(quint32 id)
{
    m_idMasterToSlave = id;
    if (m_writeThread) {
        m_writeThread->setIdMasterToSlave(id);
    }
}

void XCP_Thread_Unified::setIdSlaveToMaster(quint32 id)
{
    m_idSlaveToMaster = id;
    if (m_readThread) {
        m_readThread->setIdXcpSlaveToMaster(id);
    }
}

void XCP_Thread_Unified::setEventIdList(const QList<quint32>& ids)
{
    m_eventIdList = ids;
    if (m_readThread) {
        m_readThread->setEventIdList(ids);
    }
}

void XCP_Thread_Unified::setDeviceType(quint32 type)
{
    m_deviceType = type;
}

void XCP_Thread_Unified::setDeviceIndex(quint32 index)
{
    m_deviceIndex = index;
}

void XCP_Thread_Unified::setCanIndex(quint32 index)
{
    m_canIndex = index;
    m_interfaceName = QString("CAN%1").arg(index);
}

XCP_R_Thread_Base* XCP_Thread_Unified::readThread()
{
    return m_readThread;
}

XCP_W_Thread_Base* XCP_Thread_Unified::writeThread()
{
    return m_writeThread;
}

QStringList XCP_Thread_Unified::getCANInterface()
{
    if (m_driver) {
        return m_driver->availableInterfaces();
    }
    return QStringList();
}

void XCP_Thread_Unified::setupBaudRate()
{
    if (!m_driver) return;

    can::BaudRateConfig config;
    config.arbitrationBaud = m_baudRate;
    config.dataBaud = m_isCanFd ? m_fdBaudRate : m_baudRate;
    config.isCanFd = m_isCanFd;

    m_driver->setBaudRate(config);
}

} // namespace xcp
