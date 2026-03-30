/**
 * @file daq_processor.h
 * @brief DAQ/ODT data processor
 */

#ifndef DAQ_PROCESSOR_H
#define DAQ_PROCESSOR_H

#include <QObject>
#include <QHash>
#include <QSharedMemory>
#include <QByteArray>
#include <QVector>
#include "xcp_common.h"

namespace xcp {

/**
 * @brief PID attribute structure
 */
struct PidAttribute {
    quint16 daqList;
    quint16 odtOffset;
    bool biggestPid;
    
    PidAttribute()
        : daqList(0)
        , odtOffset(0)
        , biggestPid(false)
    {}
};

/**
 * @brief DAQ data processor
 */
class DaqProcessor : public QObject
{
    Q_OBJECT

public:
    explicit DaqProcessor(QObject* parent = nullptr);
    ~DaqProcessor();

    // Configuration interface
    void setPidAttribute(quint8 pid, const PidAttribute& attr);
    void setDaqSharedMemory(quint16 daqList, QSharedMemory* sm, quint32 size);
    void setIsCanFd(bool isFd);
    void setMaxDLC(quint8 dlc);
    void setXcpName(const QString& name);
    
    // Clear
    void clearPidAttributes();
    void clearDaqSharedMemories();

    // Processing interface
    void processDaqData(quint8 pid, const QByteArray& payload, quint64 timestamp);
    
    // Process from CAN frame (compatibility)
    void processFromCanFrame(const quint8* frameData, quint32 frameSize, 
                              const quint8* payload, quint32 payloadSize,
                              quint64 timestamp);

signals:
    void odtDataUpdated(quint16 daqList);
    void odtDataForRecord(quint8* data, quint32 size, quint16 daqList);
    void odtDataForRecord(quint8* data, quint32 size, const QString& name);

private:
    void writeToSharedMemory(quint16 daqList, quint16 odtOffset, 
                             const quint8* data, quint32 size,
                             quint64 timestamp);
    void notifyDataReady(quint16 daqList, quint32 smSize);

    QHash<quint8, PidAttribute> m_pidAttrHash;
    QHash<quint16, QSharedMemory*> m_daqListSmHash;
    QHash<quint16, quint32> m_daqListSizeHash;
    
    QVector<quint8> m_odtPacket;       ///< ODT packet buffer
    bool m_isCanFd = false;
    quint8 m_maxDLC = 8;
    QString m_xcpName;
};

} // namespace xcp

#endif // DAQ_PROCESSOR_H
