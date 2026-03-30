/**
 * @file xcp_pgm.h
 * @brief XCP PGM (Program) command set - Flash programming functionality
 */

#ifndef XCP_PGM_H
#define XCP_PGM_H

#include <QObject>
#include <QByteArray>
#include "xcp_error.h"

// Forward declaration
class XCPMaster;

/**
 * @brief PGM processor capability flags
 */
struct PGM_Processor_Info
{
    quint8 maxSector;
    quint16 maxCtoPayload;
    quint32 maxBlocksize;
    quint8 minSt;
    quint16 maxSt;
    quint8 compressionSupported;
    quint8 encryptionSupported;
    quint8 programmingMethod;
};

/**
 * @brief Sector information
 */
struct PGM_Sector_Info
{
    quint8 sectorNumber;
    QString sectorName;
    quint32 sectorAddress;
    quint32 sectorSize;
    quint8 clearSequenceNumber;
    quint8 programmingSequenceNumber;
    quint8 programmingMethod;
};

/**
 * @brief Programming progress callback
 */
struct PGM_Progress
{
    quint32 totalBytes;
    quint32 programmedBytes;
    quint32 currentSector;
    quint8 currentStep;
    QString statusMessage;
};

/**
 * @class XCP_PGM
 * @brief XCP PGM command handler
 */
class XCP_PGM : public QObject
{
    Q_OBJECT

public:
    enum ProgrammingMethod {
        METHOD_ABSOLUTE = 0x00,
        METHOD_FUNCTIONAL = 0x01
    };

    enum ProgrammingStep {
        STEP_IDLE = 0,
        STEP_STARTING,
        STEP_CLEARING,
        STEP_PREPARING,
        STEP_PROGRAMMING,
        STEP_VERIFYING,
        STEP_RESETTING,
        STEP_DONE,
        STEP_ERROR
    };

    explicit XCP_PGM(QObject *parent = nullptr);
    ~XCP_PGM();

    void setXCPMaster(XCPMaster* master);

    // PGM info retrieval
    bool getProcessorInfo(PGM_Processor_Info& info);
    bool getSectorInfo(quint8 sectorNumber, PGM_Sector_Info& info);
    bool getAllSectorInfo(QList<PGM_Sector_Info>& sectorList);

    // Programming operations
    bool programStart(quint8 clearType = 0x00, quint8 commModePgm = 0x00);
    bool programClear(quint32 address, quint32 size, quint8 addressExtension = 0x00);
    bool programClearSector(quint8 sectorNumber);
    bool programPrepare(quint32 codeSize, quint32 address, quint8 addressExtension = 0x00);
    bool programData(const quint8* data, quint16 size);
    bool programNext(const quint8* data, quint16 size);
    bool programMax(const quint8* data, quint16 size);
    bool programVerify(const quint8* data, quint16 size);
    bool programFormat(quint8 sectorNumber, quint8 compressionMethod = 0x00, quint8 encryptionMethod = 0x00);
    bool programReset();

    // High-level API
    bool programBlock(quint32 address, const QByteArray& data, bool verify = true);
    bool programHexFile(const QString& hexFilePath, bool verify = true);
    PGM_Progress getProgress() const;
    void cancelProgramming();
    bool isProgramming() const;
    QString lastError() const;

signals:
    void progressUpdated(const PGM_Progress& progress);
    void programmingFinished(bool success, const QString& message);
    void sectorProgrammed(quint8 sectorNumber, bool success);

private:
    bool sendCommand(quint8 cmd, const quint8* data = nullptr, quint8 dataLen = 0);
    bool waitForResponse(quint8 expectedCmd, quint32 timeoutMs = 5000);
    void updateProgress(ProgrammingStep step, quint32 programmed, quint32 total);
    void setError(const QString& error);
    void clearError();

private:
    XCPMaster* m_master = nullptr;
    PGM_Processor_Info m_processorInfo;
    bool m_processorInfoValid = false;
    QList<PGM_Sector_Info> m_sectorList;
    bool m_programming = false;
    bool m_cancelRequested = false;
    ProgrammingStep m_currentStep = STEP_IDLE;
    PGM_Progress m_progress;
    QString m_lastError;
    XCPErrorCode m_lastErrorCode = ERR_SUCCESS;
    quint8 m_cmdBuffer[256];
    quint8 m_respBuffer[256];
};

#endif // XCP_PGM_H
