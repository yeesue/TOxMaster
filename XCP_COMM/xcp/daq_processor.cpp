/**
 * @file daq_processor.cpp
 * @brief DAQ/ODT data processor implementation
 */

#include "daq_processor.h"
#include "logger.h"
#include <QDebug>
#include <cstring>

namespace xcp {

DaqProcessor::DaqProcessor(QObject* parent)
    : QObject(parent)
    , m_odtPacket(64, 0)
{
}

DaqProcessor::~DaqProcessor()
{
    clearDaqSharedMemories();
}

void DaqProcessor::setPidAttribute(quint8 pid, const PidAttribute& attr)
{
    m_pidAttrHash[pid] = attr;
}

void DaqProcessor::setDaqSharedMemory(quint16 daqList, QSharedMemory* sm, quint32 size)
{
    m_daqListSmHash[daqList] = sm;
    m_daqListSizeHash[daqList] = size;
}

void DaqProcessor::setIsCanFd(bool isFd)
{
    m_isCanFd = isFd;
}

void DaqProcessor::setMaxDLC(quint8 dlc)
{
    m_maxDLC = dlc;
}

void DaqProcessor::setXcpName(const QString& name)
{
    m_xcpName = name;
}

void DaqProcessor::clearPidAttributes()
{
    m_pidAttrHash.clear();
}

void DaqProcessor::clearDaqSharedMemories()
{
    // Do not delete SharedMemory, managed by caller
    m_daqListSmHash.clear();
    m_daqListSizeHash.clear();
}

void DaqProcessor::processDaqData(quint8 pid, const QByteArray& payload, quint64 timestamp)
{
    // Find PID attribute
    if (!m_pidAttrHash.contains(pid)) {
        return;
    }
    
    PidAttribute attr = m_pidAttrHash.value(pid);
    quint16 daqList = attr.daqList;
    
    // Write to shared memory
    writeToSharedMemory(daqList, attr.odtOffset, 
                        reinterpret_cast<const quint8*>(payload.constData() + 1),
                        payload.size() - 1, timestamp);
    
    // If biggest PID, trigger update notification
    if (attr.biggestPid) {
        quint32 smSize = m_daqListSizeHash.value(daqList, 0);
        notifyDataReady(daqList, smSize);
    }
}

void DaqProcessor::processFromCanFrame(const quint8* frameData, quint32 frameSize,
                                        const quint8* payload, quint32 payloadSize,
                                        quint64 timestamp)
{
    Q_UNUSED(frameData);
    Q_UNUSED(frameSize);
    
    if (payloadSize == 0) {
        return;
    }
    
    // Copy payload to ODT packet
    m_odtPacket.resize(payloadSize);
    memcpy(m_odtPacket.data(), payload, payloadSize);
    
    // Get PID
    quint8 pid = m_odtPacket[0];
    
    // Find PID attribute
    if (!m_pidAttrHash.contains(pid)) {
        return;
    }
    
    PidAttribute attr = m_pidAttrHash.value(pid);
    quint16 daqList = attr.daqList;
    
    // Write to shared memory
    quint32 dataSize = payloadSize - 1;
    writeToSharedMemory(daqList, attr.odtOffset,
                        reinterpret_cast<const quint8*>(m_odtPacket.constData() + 1),
                        dataSize, timestamp);
    
    // If biggest PID, trigger update notification
    if (attr.biggestPid) {
        quint32 smSize = m_daqListSizeHash.value(daqList, 0);
        notifyDataReady(daqList, smSize);
    }
}

void DaqProcessor::writeToSharedMemory(quint16 daqList, quint16 odtOffset,
                                        const quint8* data, quint32 size,
                                        quint64 timestamp)
{
    QSharedMemory* sm = m_daqListSmHash.value(daqList);
    if (!sm) {
        return;
    }
    
    if (!sm->isAttached()) {
        if (!sm->attach()) {
            LOG_WARN_STREAM() << "DaqProcessor: Failed to attach shared memory for DAQ list" << daqList;
            return;
        }
    }
    
    // Write timestamp and data
    sm->lock();
    quint8* to = static_cast<quint8*>(sm->data());
    
    // Write timestamp (first 8 bytes)
    memcpy(to, &timestamp, 8);
    
    // Write ODT data (after 8 byte offset)
    memcpy(to + 8 + odtOffset, data, size);
    
    sm->unlock();
}

void DaqProcessor::notifyDataReady(quint16 daqList, quint32 smSize)
{
    // Send update signal
    emit odtDataUpdated(daqList);
    
    // Prepare data copy for recording
    if (smSize > 0) {
        QSharedMemory* sm = m_daqListSmHash.value(daqList);
        if (sm && sm->isAttached()) {
            ByteArrayPtr buf = makeByteArray(smSize);
            sm->lock();
            memcpy(buf.data(), sm->data(), smSize);
            sm->unlock();
            
            emit odtDataForRecord(buf, smSize, daqList);
            emit odtDataForRecord(buf, smSize, 
                "RP_DAQ_" + m_xcpName + "_" + QString::number(daqList));
        }
    }
}

} // namespace xcp
