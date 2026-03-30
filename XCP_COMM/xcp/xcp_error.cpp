/**
 * @file xcp_error.cpp
 * @brief XCP error code implementation
 */

#include "xcp_error.h"

XCPErrorRegistry& XCPErrorRegistry::instance()
{
    static XCPErrorRegistry instance;
    return instance;
}

XCPErrorRegistry::XCPErrorRegistry()
{
    initializeErrorTable();
}

void XCPErrorRegistry::initializeErrorTable()
{
    // Success response
    m_errorTable[ERR_SUCCESS] = {
        ERR_SUCCESS,
        "SUCCESS",
        "Operation successful",
        "",
        XCPErrorSeverity::None,
        true,
        0
    };
    
    // Protocol defined errors
    m_errorTable[ERR_CMD_UNKNOWN] = {
        ERR_CMD_UNKNOWN,
        "CMD_UNKNOWN",
        "Unknown command: ECU does not support this command code",
        "Check if command code is correct, confirm ECU supports this function",
        XCPErrorSeverity::Error,
        false,
        0
    };
    
    m_errorTable[ERR_CMD_SYNTAX] = {
        ERR_CMD_SYNTAX,
        "CMD_SYNTAX",
        "Command syntax error: parameter format or value invalid",
        "Check if command parameters are correct",
        XCPErrorSeverity::Error,
        false,
        0
    };
    
    m_errorTable[ERR_CMD_BUSY] = {
        ERR_CMD_BUSY,
        "CMD_BUSY",
        "Command busy: ECU is processing other tasks",
        "Wait and retry after a while",
        XCPErrorSeverity::Warning,
        true,
        3
    };
    
    m_errorTable[ERR_DAQ_ACTIVE] = {
        ERR_DAQ_ACTIVE,
        "DAQ_ACTIVE",
        "DAQ is running: need to stop DAQ first",
        "Call START_STOP_SYNCH to stop DAQ before executing this command",
        XCPErrorSeverity::Warning,
        true,
        0
    };
    
    m_errorTable[ERR_PGM_ACTIVE] = {
        ERR_PGM_ACTIVE,
        "PGM_ACTIVE",
        "Programming mode is running",
        "Wait for programming operation to complete",
        XCPErrorSeverity::Warning,
        true,
        0
    };
    
    m_errorTable[ERR_DAQ_OVERLOAD] = {
        ERR_DAQ_OVERLOAD,
        "DAQ_OVERLOAD",
        "DAQ overload: processor cannot process data in time",
        "Reduce DAQ sampling points or lower sampling rate",
        XCPErrorSeverity::Error,
        true,
        1
    };
    
    m_errorTable[ERR_ACCESS_LOCKED] = {
        ERR_ACCESS_LOCKED,
        "ACCESS_LOCKED",
        "Resource locked: need Seed/Key unlock",
        "Call GET_SEED and UNLOCK to unlock resource",
        XCPErrorSeverity::Warning,
        true,
        0
    };
    
    m_errorTable[ERR_ACCESS_DENIED] = {
        ERR_ACCESS_DENIED,
        "ACCESS_DENIED",
        "Access denied: insufficient permissions",
        "Check Seed/Key unlock status, ensure sufficient permissions obtained",
        XCPErrorSeverity::Error,
        false,
        0
    };
    
    m_errorTable[ERR_OUT_OF_RANGE] = {
        ERR_OUT_OF_RANGE,
        "OUT_OF_RANGE",
        "Parameter exceeds valid range",
        "Check if parameter value is within valid range",
        XCPErrorSeverity::Error,
        false,
        0
    };
    
    m_errorTable[ERR_WRITE_PROTECTED] = {
        ERR_WRITE_PROTECTED,
        "WRITE_PROTECTED",
        "Target area write protected",
        "Check memory segment attributes, try switching to RAM page",
        XCPErrorSeverity::Error,
        false,
        0
    };
    
    m_errorTable[ERR_CAL_PAGE_INVALID] = {
        ERR_CAL_PAGE_INVALID,
        "CAL_PAGE_INVALID",
        "Calibration page invalid",
        "Check if page number is within valid range",
        XCPErrorSeverity::Error,
        false,
        0
    };
    
    m_errorTable[ERR_SEGMENT_INVALID] = {
        ERR_SEGMENT_INVALID,
        "SEGMENT_INVALID",
        "Memory segment invalid",
        "Check if memory segment number is correct",
        XCPErrorSeverity::Error,
        false,
        0
    };
    
    m_errorTable[ERR_SEQUENCE] = {
        ERR_SEQUENCE,
        "SEQUENCE",
        "Command sequence error",
        "Send commands in correct order (e.g., CONNECT before DAQ configuration)",
        XCPErrorSeverity::Error,
        true,
        1
    };
    
    m_errorTable[ERR_DAQ_CONFIG] = {
        ERR_DAQ_CONFIG,
        "DAQ_CONFIG",
        "DAQ configuration invalid",
        "Check if DAQ configuration parameters are correct",
        XCPErrorSeverity::Error,
        true,
        0
    };
    
    m_errorTable[ERR_MEMORY_OVERFLOW] = {
        ERR_MEMORY_OVERFLOW,
        "MEMORY_OVERFLOW",
        "Memory overflow",
        "Reduce number of allocated DAQ lists or ODTs",
        XCPErrorSeverity::Error,
        false,
        0
    };
    
    m_errorTable[ERR_GENERIC] = {
        ERR_GENERIC,
        "GENERIC",
        "Generic error",
        "Check ECU log for detailed information",
        XCPErrorSeverity::Error,
        false,
        0
    };
    
    m_errorTable[ERR_VERIFY] = {
        ERR_VERIFY,
        "VERIFY",
        "Verification failed",
        "Check data integrity, may need to retransmit",
        XCPErrorSeverity::Error,
        true,
        1
    };
    
    // Custom communication layer errors
    m_errorTable[ERR_TIMEOUT] = {
        ERR_TIMEOUT,
        "TIMEOUT",
        "Response timeout: ECU did not respond within specified time",
        "Check CAN bus connection, increase timeout, retry command",
        XCPErrorSeverity::Warning,
        true,
        3
    };
    
    m_errorTable[ERR_NOT_CONNECTED] = {
        ERR_NOT_CONNECTED,
        "NOT_CONNECTED",
        "Not connected: XCP connection with ECU not established",
        "Call CONNECT command first to establish connection",
        XCPErrorSeverity::Error,
        true,
        0
    };
    
    m_errorTable[ERR_COMMUNICATION] = {
        ERR_COMMUNICATION,
        "COMMUNICATION",
        "Communication error: CAN bus communication failed",
        "Check CAN hardware connection and baud rate settings",
        XCPErrorSeverity::Critical,
        true,
        2
    };
    
    m_errorTable[ERR_BUFFER_OVERFLOW] = {
        ERR_BUFFER_OVERFLOW,
        "BUFFER_OVERFLOW",
        "Buffer overflow",
        "Reduce data transmission volume or increase buffer size",
        XCPErrorSeverity::Warning,
        true,
        0
    };
    
    m_errorTable[ERR_INVALID_RESPONSE] = {
        ERR_INVALID_RESPONSE,
        "INVALID_RESPONSE",
        "Invalid response: response format does not match protocol specification",
        "Check if ECU XCP implementation is correct",
        XCPErrorSeverity::Error,
        true,
        2
    };
    
    m_errorTable[ERR_INIT_FAILED] = {
        ERR_INIT_FAILED,
        "INIT_FAILED",
        "Initialization failed: CAN device initialization failed",
        "Check CAN device driver and configuration",
        XCPErrorSeverity::Critical,
        true,
        1
    };
    
    m_errorTable[ERR_RESOURCE_UNAVAILABLE] = {
        ERR_RESOURCE_UNAVAILABLE,
        "RESOURCE_UNAVAILABLE",
        "Resource unavailable",
        "Release other resources or wait for resource availability",
        XCPErrorSeverity::Warning,
        true,
        2
    };
}

XCPErrorInfo XCPErrorRegistry::getErrorInfo(XCPErrorCode code) const
{
    return getErrorInfo(static_cast<quint8>(code));
}

XCPErrorInfo XCPErrorRegistry::getErrorInfo(quint8 codeValue) const
{
    if (m_errorTable.contains(codeValue)) {
        return m_errorTable[codeValue];
    }
    
    // Unknown error code
    return {
        static_cast<XCPErrorCode>(codeValue),
        "UNKNOWN",
        QString("Unknown error code: 0x%1").arg(codeValue, 2, 16, QChar('0')),
        "Check XCP protocol documentation or ECU supplier documentation",
        XCPErrorSeverity::Error,
        false,
        0
    };
}

QString XCPErrorRegistry::getDescription(XCPErrorCode code) const
{
    return getDescription(static_cast<quint8>(code));
}

QString XCPErrorRegistry::getDescription(quint8 codeValue) const
{
    return getErrorInfo(codeValue).description;
}

QString XCPErrorRegistry::getSuggestion(XCPErrorCode code) const
{
    return getErrorInfo(code).suggestion;
}

bool XCPErrorRegistry::isRecoverable(XCPErrorCode code) const
{
    return getErrorInfo(code).recoverable;
}

bool XCPErrorRegistry::canRetry(XCPErrorCode code) const
{
    return getErrorInfo(code).retryCount > 0;
}

int XCPErrorRegistry::getRetryCount(XCPErrorCode code) const
{
    return getErrorInfo(code).retryCount;
}

XCPErrorSeverity XCPErrorRegistry::getSeverity(XCPErrorCode code) const
{
    return getErrorInfo(code).severity;
}

bool XCPErrorRegistry::isSuccess(quint8 codeValue) const
{
    return codeValue == ERR_SUCCESS;
}

bool XCPErrorRegistry::isError(quint8 codeValue) const
{
    return codeValue != ERR_SUCCESS;
}

QString XCPErrorRegistry::formatError(XCPErrorCode code, const QString& context) const
{
    return formatError(static_cast<quint8>(code), context);
}

QString XCPErrorRegistry::formatError(quint8 codeValue, const QString& context) const
{
    XCPErrorInfo info = getErrorInfo(codeValue);
    
    QString result = QString("[XCP Error 0x%1 %2] %3")
                     .arg(codeValue, 2, 16, QChar('0'))
                     .arg(info.name)
                     .arg(info.description);
    
    if (!info.suggestion.isEmpty()) {
        result += QString(" | Suggestion: %1").arg(info.suggestion);
    }
    
    if (!context.isEmpty()) {
        result += QString(" | Context: %1").arg(context);
    }
    
    return result;
}
