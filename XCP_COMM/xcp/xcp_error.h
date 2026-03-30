/**
 * @file xcp_error.h
 * @brief XCP protocol error code definitions and handling
 * 
 * Defines all error codes from XCP protocol specification, plus custom communication layer error codes.
 * Provides error code to description mapping and recovery suggestions.
 * 
 * Error code categories:
 * - 0xFF: Success response (ERR_SUCCESS)
 * - 0x00-0x1F: Protocol defined error codes
 * - 0xF0-0xFE: Custom error codes (communication layer)
 */

#ifndef XCP_ERROR_H
#define XCP_ERROR_H

#include <QString>
#include <QHash>

/**
 * @brief XCP error code enumeration
 * 
 * Based on XCP Protocol Layer Specification (Part 2) - Version 1.0
 */
enum XCPErrorCode
{
    // Success response
    ERR_SUCCESS = 0xFF,             ///< Success response (normal response packet)
    
    // Protocol defined error codes (0x00-0x1F)
    ERR_CMD_UNKNOWN = 0x00,         ///< Unknown command, ECU does not support this command
    ERR_CMD_SYNTAX = 0x01,          ///< Command syntax error, invalid parameters
    ERR_CMD_BUSY = 0x02,            ///< Command busy, internal resources occupied
    ERR_DAQ_ACTIVE = 0x03,          ///< DAQ is running, need to stop DAQ first
    ERR_PGM_ACTIVE = 0x04,          ///< PGM is running
    ERR_UNKNOWN = 0x05,             ///< Unknown error (reserved)
    ERR_DAQ_OVERLOAD = 0x06,        ///< DAQ overload, processor cannot handle data
    // 0x07-0x0F reserved
    ERR_ACCESS_LOCKED = 0x10,       ///< Resource locked, needs unlock
    ERR_ACCESS_DENIED = 0x11,       ///< Access denied, insufficient permissions
    ERR_OUT_OF_RANGE = 0x12,        ///< Parameter out of range
    ERR_WRITE_PROTECTED = 0x13,     ///< Write protected, target area forbidden to write
    ERR_CAL_PAGE_INVALID = 0x14,    ///< Calibration page invalid
    ERR_SEGMENT_INVALID = 0x15,     ///< Memory segment invalid
    ERR_SEQUENCE = 0x16,            ///< Sequence error, command order incorrect
    ERR_DAQ_CONFIG = 0x17,          ///< DAQ configuration invalid
    ERR_MEMORY_OVERFLOW = 0x18,     ///< Memory overflow
    ERR_GENERIC = 0x19,             ///< Generic error
    ERR_VERIFY = 0x1A,              ///< Verification failed
    
    // Custom communication layer error codes (0xF0-0xFE)
    ERR_TIMEOUT = 0xF0,             ///< Response timeout
    ERR_NOT_CONNECTED = 0xF1,       ///< Not connected to ECU
    ERR_COMMUNICATION = 0xF2,       ///< Communication error
    ERR_BUFFER_OVERFLOW = 0xF3,     ///< Buffer overflow
    ERR_INVALID_RESPONSE = 0xF4,    ///< Invalid response
    ERR_INIT_FAILED = 0xF5,         ///< Initialization failed
    ERR_RESOURCE_UNAVAILABLE = 0xF6 ///< Resource unavailable
};

/**
 * @brief Error severity level
 */
enum class XCPErrorSeverity
{
    None,       ///< No error
    Info,       ///< Information notice
    Warning,    ///< Warning, can continue operation
    Error,      ///< Error, needs handling
    Critical    ///< Critical error, needs reconnection or restart
};

/**
 * @brief XCP error information structure
 */
struct XCPErrorInfo
{
    XCPErrorCode code;              ///< Error code
    QString name;                   ///< Error name (English)
    QString description;            ///< Error description
    QString suggestion;             ///< Recovery suggestion
    XCPErrorSeverity severity;      ///< Severity level
    bool recoverable;               ///< Whether auto recoverable
    int retryCount;                 ///< Suggested retry count (0 means no retry)
    
    XCPErrorInfo() 
        : code(ERR_SUCCESS)
        , severity(XCPErrorSeverity::None)
        , recoverable(true)
        , retryCount(0) 
    {}
    
    XCPErrorInfo(XCPErrorCode c, const QString& n, const QString& d, const QString& s,
                 XCPErrorSeverity sev, bool rec, int retry)
        : code(c), name(n), description(d), suggestion(s)
        , severity(sev), recoverable(rec), retryCount(retry)
    {}
};

/**
 * @class XCPErrorRegistry
 * @brief XCP error code registry
 * 
 * Provides error code to error information mapping and error handling suggestions.
 */
class XCPErrorRegistry
{
public:
    /**
     * @brief Get singleton instance
     */
    static XCPErrorRegistry& instance();
    
    /**
     * @brief Get error information by error code
     * @param code Error code
     * @return Error information structure
     */
    XCPErrorInfo getErrorInfo(XCPErrorCode code) const;
    
    /**
     * @brief Get error information by error code value
     * @param codeValue Error code value
     * @return Error information structure
     */
    XCPErrorInfo getErrorInfo(quint8 codeValue) const;
    
    /**
     * @brief Get error description
     * @param code Error code
     * @return Error description string
     */
    QString getDescription(XCPErrorCode code) const;
    
    /**
     * @brief Get error description
     * @param codeValue Error code value
     * @return Error description string
     */
    QString getDescription(quint8 codeValue) const;
    
    /**
     * @brief Get error recovery suggestion
     * @param code Error code
     * @return Recovery suggestion string
     */
    QString getSuggestion(XCPErrorCode code) const;
    
    /**
     * @brief Check if error is recoverable
     * @param code Error code
     * @return true if auto recoverable
     */
    bool isRecoverable(XCPErrorCode code) const;
    
    /**
     * @brief Check if error can be retried
     * @param code Error code
     * @return true if can retry
     */
    bool canRetry(XCPErrorCode code) const;
    
    /**
     * @brief Get suggested retry count
     * @param code Error code
     * @return Retry count
     */
    int getRetryCount(XCPErrorCode code) const;
    
    /**
     * @brief Get error severity
     * @param code Error code
     * @return Severity level
     */
    XCPErrorSeverity getSeverity(XCPErrorCode code) const;
    
    /**
     * @brief Check if response is success
     * @param codeValue Response code value
     * @return true if success
     */
    bool isSuccess(quint8 codeValue) const;
    
    /**
     * @brief Check if response is error
     * @param codeValue Response code value
     * @return true if error
     */
    bool isError(quint8 codeValue) const;
    
    /**
     * @brief Format error message for log output
     * @param code Error code
     * @param context Context message (optional)
     * @return Formatted error message
     */
    QString formatError(XCPErrorCode code, const QString& context = QString()) const;
    
    /**
     * @brief Format error message for log output
     * @param codeValue Error code value
     * @param context Context message (optional)
     * @return Formatted error message
     */
    QString formatError(quint8 codeValue, const QString& context = QString()) const;

private:
    XCPErrorRegistry();
    
    void initializeErrorTable();
    
    QHash<quint8, XCPErrorInfo> m_errorTable;
};

#endif // XCP_ERROR_H
