/**
 * @file mock_can_driver.h
 * @brief Mock CAN 驱动实现（用于单元测试）
 * 
 * 适配 can::ICanDriver 新版接口
 */

#ifndef MOCK_CAN_DRIVER_H
#define MOCK_CAN_DRIVER_H

#include "can_driver.h"
#include <QList>
#include <QMutex>

namespace can {

/**
 * @brief Mock CAN 驱动
 * 
 * 用于单元测试，模拟 CAN 卡行为：
 * - 可预设发送/接收数据
 * - 记录所有调用
 */
class MockCanDriver : public ICanDriver
{
    Q_OBJECT

public:
    explicit MockCanDriver(QObject* parent = nullptr)
        : ICanDriver(parent)
        , m_isOpen(false)
        , m_state(DriverState::Closed)
    {}

    // ====== ICanDriver 接口实现 ======

    bool open() override {
        m_isOpen = true;
        m_state = DriverState::Open;
        emit stateChanged(m_state);
        return true;
    }

    void close() override {
        m_isOpen = false;
        m_state = DriverState::Closed;
        emit stateChanged(m_state);
    }

    bool isOpen() const override {
        return m_isOpen;
    }

    bool setBaudRate(const BaudRateConfig& config) override {
        m_baudConfig = config;
        return true;
    }

    bool setFilters(const QList<CanFilter>& filters) override {
        m_filters = filters;
        return true;
    }

    void setReceiveBufferSize(int size) override {
        m_receiveBufferSize = size;
    }

    bool send(const CanFrame& frame, int timeoutMs = 100) override {
        Q_UNUSED(timeoutMs);
        QMutexLocker locker(&m_mutex);
        m_sentFrames.append(frame);
        return true;
    }

    bool receive(CanFrame& frame, int timeoutMs = 100) override {
        Q_UNUSED(timeoutMs);
        QMutexLocker locker(&m_mutex);

        if (m_receiveQueue.isEmpty()) {
            return false;
        }

        frame = m_receiveQueue.takeFirst();
        return true;
    }

    int sendBatch(const QList<CanFrame>& frames) override {
        QMutexLocker locker(&m_mutex);
        m_sentFrames.append(frames);
        return frames.size();
    }

    DriverState state() const override {
        return m_state;
    }

    QString lastError() const override {
        return m_lastError;
    }

    CanDeviceInfo deviceInfo() const override {
        CanDeviceInfo info;
        info.vendor = "Mock";
        info.model = "MockCAN";
        info.channelCount = 2;
        return info;
    }

    QStringList availableInterfaces() const override {
        return QStringList() << "CAN0" << "CAN1";
    }

    bool selectInterface(const QString& interfaceName) override {
        m_currentInterface = interfaceName;
        return true;
    }

    QString currentInterface() const override {
        return m_currentInterface;
    }

    void clearReceiveBuffer() override {
        QMutexLocker locker(&m_mutex);
        m_receiveQueue.clear();
    }

    // ====== Mock 专用接口 ======

    void enqueueReceiveFrame(const CanFrame& frame) {
        QMutexLocker locker(&m_mutex);
        m_receiveQueue.append(frame);
    }

    CanFrame getLastSentFrame() {
        QMutexLocker locker(&m_mutex);
        if (m_sentFrames.isEmpty()) {
            return CanFrame();
        }
        return m_sentFrames.last();
    }

    QList<CanFrame> getAllSentFrames() {
        QMutexLocker locker(&m_mutex);
        return m_sentFrames;
    }

    void clearSentFrames() {
        QMutexLocker locker(&m_mutex);
        m_sentFrames.clear();
    }

    void setLastError(const QString& error) {
        m_lastError = error;
    }

    BaudRateConfig getBaudConfig() const {
        return m_baudConfig;
    }

    QList<CanFilter> getFilters() const {
        return m_filters;
    }

private:
    bool m_isOpen;
    DriverState m_state;
    BaudRateConfig m_baudConfig;
    QList<CanFilter> m_filters;
    int m_receiveBufferSize;
    QString m_lastError;
    QString m_currentInterface;

    QList<CanFrame> m_sentFrames;
    QList<CanFrame> m_receiveQueue;
    mutable QMutex m_mutex;
};

} // namespace can

#endif // MOCK_CAN_DRIVER_H
