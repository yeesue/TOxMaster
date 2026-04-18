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
    clearDaqMemories();
}

void DaqProcessor::setPidAttribute(quint8 pid, const PidAttribute& attr)
{
    m_pidAttrHash[pid] = attr;
}

void DaqProcessor::setDaqMemory(quint16 daqList, const QString& key, quint32 size)
{
    m_daqListKeyHash[daqList] = key;
    m_daqListSizeHash[daqList] = size;
    
    // 确保内存已创建
    MemoryManager::instance()->createMemory(key, size);
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

void DaqProcessor::clearDaqMemories()
{
    // 清理内存映射
    for (auto it = m_daqListKeyHash.constBegin(); it != m_daqListKeyHash.constEnd(); ++it) {
        MemoryManager::instance()->releaseMemory(it.value());
    }
    m_daqListKeyHash.clear();
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
    
    // Write to memory
    writeToMemory(daqList, attr.odtOffset, 
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
    
    // Write to memory
    quint32 dataSize = payloadSize - 1;
    writeToMemory(daqList, attr.odtOffset,
                  reinterpret_cast<const quint8*>(m_odtPacket.constData() + 1),
                  dataSize, timestamp);
    
    // If biggest PID, trigger update notification
    if (attr.biggestPid) {
        quint32 smSize = m_daqListSizeHash.value(daqList, 0);
        notifyDataReady(daqList, smSize);
    }
}

void DaqProcessor::writeToMemory(quint16 daqList, quint16 odtOffset,
                                  const quint8* data, quint32 size,
                                  quint64 timestamp)
{
    QString key = m_daqListKeyHash.value(daqList);
    if (key.isEmpty()) {
        return;
    }
    
    // Get memory from MemoryManager
    void* mem = MemoryManager::instance()->getMemory(key);
    if (!mem) {
        LOG_WARN_STREAM() << "DaqProcessor: Failed to get memory for DAQ list" << daqList;
        return;
    }
    
    // Write timestamp and data
    quint8* to = static_cast<quint8*>(mem);
    
    // Write timestamp (first 8 bytes)
    memcpy(to, &timestamp, 8);
    
    // Write ODT data (after 8 byte offset)
    memcpy(to + 8 + odtOffset, data, size);
}

void DaqProcessor::notifyDataReady(quint16 daqList, quint32 size)
{
    // Send update signal
    emit odtDataUpdated(daqList);
    
    // Prepare data copy for recording
    if (size > 0) {
        QString key = m_daqListKeyHash.value(daqList);
        if (!key.isEmpty()) {
            void* mem = MemoryManager::instance()->getMemory(key);
            if (mem) {
                ByteArrayPtr buf = makeByteArray(size);
                memcpy(buf.data(), mem, size);
                
                emit odtDataForRecord(buf, size, daqList);
                emit odtDataForRecord(buf, size, 
                    "RP_DAQ_" + m_xcpName + "_" + QString::number(daqList));
            }
        }
    }
}

} // namespace xcp
