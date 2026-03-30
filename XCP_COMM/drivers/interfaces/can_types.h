/**
 * @file can_types.h
 * @brief CAN common data type definitions
 * 
 * Defines unified CAN frame structure and configuration types
 * Supports CAN 2.0 and CAN FD
 */

#ifndef CAN_TYPES_H
#define CAN_TYPES_H

#include <QtGlobal>
#include <QByteArray>
#include <QString>

namespace can {

/**
 * @brief CAN frame type
 */
enum class FrameType {
    Data = 0,       ///< Data frame
    Remote = 1,     ///< Remote frame
    Error = 2,      ///< Error frame
    Overload = 3    ///< Overload frame
};

/**
 * @brief CAN baud rate configuration
 */
struct BaudRateConfig {
    quint32 arbitrationBaud;    ///< Arbitration domain baud rate (bps)
    quint32 dataBaud;           ///< Data domain baud rate (bps), for CAN FD
    bool isCanFd;               ///< Whether in CAN FD mode
    bool isNonIso;              ///< Whether non-ISO CAN FD
    
    BaudRateConfig()
        : arbitrationBaud(500000)
        , dataBaud(2000000)
        , isCanFd(false)
        , isNonIso(false)
    {}
};

/**
 * @brief CAN frame structure
 */
struct CanFrame {
    quint32 id;              ///< CAN ID (standard frame 11-bit, extended frame 29-bit)
    QByteArray data;         ///< Data payload (CAN: 0-8 bytes, CAN FD: 0-64 bytes)
    bool extended;           ///< Whether extended frame
    bool fd;                 ///< Whether CAN FD frame
    bool brs;                ///< Bit Rate Switch (CAN FD)
    bool esi;                ///< Error State Indicator (CAN FD)
    FrameType type;          ///< Frame type
    quint64 timestamp;       ///< Timestamp (microseconds)
    quint8 dlc;              ///< Data Length Code
    
    CanFrame()
        : id(0)
        , extended(false)
        , fd(false)
        , brs(false)
        , esi(false)
        , type(FrameType::Data)
        , timestamp(0)
        , dlc(0)
    {}
    
    /**
     * @brief Check if frame is valid
     */
    bool isValid() const {
        if (fd) {
            // CAN FD: max 64 bytes
            return data.size() <= 64;
        } else {
            // CAN 2.0: max 8 bytes
            return data.size() <= 8;
        }
    }
    
    /**
     * @brief Get data length from DLC
     */
    static quint8 dlcToLength(quint8 dlc) {
        static const quint8 dlcTable[] = {
            0, 1, 2, 3, 4, 5, 6, 7, 8, 12, 16, 20, 24, 32, 48, 64
        };
        return (dlc < 16) ? dlcTable[dlc] : 64;
    }
    
    /**
     * @brief Get DLC from data length
     */
    static quint8 lengthToDlc(quint8 length) {
        if (length <= 8) return length;
        if (length <= 12) return 9;
        if (length <= 16) return 10;
        if (length <= 20) return 11;
        if (length <= 24) return 12;
        if (length <= 32) return 13;
        if (length <= 48) return 14;
        return 15;
    }
};

/**
 * @brief CAN error type
 */
enum class CanError {
    None = 0,
    BusOff,
    ErrorPassive,
    Warning,
    StuffError,
    FormError,
    AckError,
    BitError,
    CrcError
};

/**
 * @brief CAN driver status
 */
struct CanDriverStatus {
    bool isOpen;             ///< Whether device is open
    bool isRunning;          ///< Whether receiving/transmitting
    quint32 txCount;         ///< Transmit frame count
    quint32 rxCount;         ///< Receive frame count
    quint32 errorCount;      ///< Error count
    CanError lastError;      ///< Last error
    
    CanDriverStatus()
        : isOpen(false)
        , isRunning(false)
        , txCount(0)
        , rxCount(0)
        , errorCount(0)
        , lastError(CanError::None)
    {}
};

/**
 * @brief CAN filter configuration
 */
struct CanFilter {
    quint32 id;              ///< Filter ID
    quint32 mask;            ///< Filter mask
    bool extended;           ///< Whether extended frame
    
    CanFilter()
        : id(0)
        , mask(0x7FF)
        , extended(false)
    {}
    
    CanFilter(quint32 id, quint32 mask, bool extended = false)
        : id(id)
        , mask(mask)
        , extended(extended)
    {}
};

} // namespace can

#endif // CAN_TYPES_H
