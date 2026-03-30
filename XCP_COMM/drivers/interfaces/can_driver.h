/**
 * @file can_driver.h
 * @brief CAN driver abstract interface
 * 
 * Defines unified CAN driver interface, supports multiple hardware vendors
 * Implementations: NI-XNET, TSCAN, ZLG
 */

#ifndef CAN_DRIVER_H
#define CAN_DRIVER_H

#include "can_types.h"
#include <QObject>
#include <QList>
#include <functional>

namespace can {

/**
 * @brief Driver state
 */
enum class DriverState {
    Closed,         ///< Device closed
    Opening,        ///< Opening device
    Open,           ///< Device open
    Running,        ///< Receiving/Transmitting
    Error           ///< Error state
};

/**
 * @brief Device information
 */
struct CanDeviceInfo {
    QString vendor;         ///< Vendor name
    QString model;          ///< Device model
    QString serialNumber;   ///< Serial number
    QString firmwareVersion;///< Firmware version
    int channelCount;       ///< Number of channels
    
    CanDeviceInfo()
        : channelCount(0)
    {}
};

/**
 * @brief CAN driver abstract interface
 * 
 * Provides unified CAN communication interface
 * All concrete driver implementations must inherit this interface
 */
class ICanDriver : public QObject
{
    Q_OBJECT

public:
    explicit ICanDriver(QObject* parent = nullptr) : QObject(parent) {}
    virtual ~ICanDriver() = default;
    
    // ==================== Lifecycle management ====================
    
    /**
     * @brief Open CAN device
     * @return true on success
     */
    virtual bool open() = 0;
    
    /**
     * @brief Close CAN device
     */
    virtual void close() = 0;
    
    /**
     * @brief Check if device is open
     */
    virtual bool isOpen() const = 0;
    
    // ==================== Configuration ====================
    
    /**
     * @brief Set baud rate
     * @param config Baud rate configuration
     * @return true on success
     */
    virtual bool setBaudRate(const BaudRateConfig& config) = 0;
    
    /**
     * @brief Set CAN filters
     * @param filters Filter list
     * @return true on success
     */
    virtual bool setFilters(const QList<CanFilter>& filters) = 0;
    
    /**
     * @brief Set receive buffer size
     * @param size Buffer size (number of frames)
     */
    virtual void setReceiveBufferSize(int size) = 0;
    
    // ==================== Transmit/Receive operations ====================
    
    /**
     * @brief Send single frame
     * @param frame CAN frame
     * @param timeoutMs Timeout (milliseconds), -1 for infinite wait
     * @return true on success
     */
    virtual bool send(const CanFrame& frame, int timeoutMs = 100) = 0;
    
    /**
     * @brief Receive single frame
     * @param frame Output frame
     * @param timeoutMs Timeout (milliseconds), -1 for infinite wait
     * @return true on success, false on timeout
     */
    virtual bool receive(CanFrame& frame, int timeoutMs = 100) = 0;
    
    /**
     * @brief Batch send frames
     * @param frames Frame list
     * @return Number of successfully sent frames
     */
    virtual int sendBatch(const QList<CanFrame>& frames) = 0;
    
    // ==================== Status query ====================
    
    /**
     * @brief Get driver state
     */
    virtual DriverState state() const = 0;
    
    /**
     * @brief Get last error message
     */
    virtual QString lastError() const = 0;
    
    /**
     * @brief Get device information
     */
    virtual CanDeviceInfo deviceInfo() const = 0;
    
    /**
     * @brief Get list of supported interfaces
     * @return Interface name list
     */
    virtual QStringList availableInterfaces() const = 0;
    
    /**
     * @brief Select communication interface
     * @param interfaceName Interface name
     * @return true on success
     */
    virtual bool selectInterface(const QString& interfaceName) = 0;
    
    /**
     * @brief Get currently selected interface
     */
    virtual QString currentInterface() const = 0;
    
    /**
     * @brief Clear receive buffer
     */
    virtual void clearReceiveBuffer() = 0;

signals:
    /**
     * @brief Frame received signal
     */
    void frameReceived(const CanFrame& frame);
    
    /**
     * @brief Error occurred signal
     */
    void errorOccurred(CanError error, const QString& message);
    
    /**
     * @brief State changed signal
     */
    void stateChanged(DriverState state);
};

/**
 * @brief CAN driver factory
 */
class CanDriverFactory
{
public:
    /**
     * @brief Driver type
     */
    enum class DriverType {
        NI_XNET,    ///< NI-XNET driver
        TS_CAN,     ///< TSCAN driver
        ZLG_CAN     ///< ZLG driver
    };
    
    /**
     * @brief Create CAN driver instance
     * @param type Driver type
     * @param parent Parent object
     * @return Driver instance pointer, caller responsible for deletion
     */
    static ICanDriver* create(DriverType type, QObject* parent = nullptr);
    
    /**
     * @brief Get driver type name
     */
    static QString driverTypeName(DriverType type);
    
    /**
     * @brief Detect available driver types
     * @return List of available driver types
     */
    static QList<DriverType> detectAvailableDrivers();
};

} // namespace can

#endif // CAN_DRIVER_H
