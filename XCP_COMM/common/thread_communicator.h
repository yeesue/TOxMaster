/**
 * @file thread_communicator.h
 * @brief Thread communication interface
 * 
 * Provides a standardized interface for thread communication
 * Supports signal-slot mechanism, message passing, and thread-safe data transfer
 */

#ifndef THREAD_COMMUNICATOR_H
#define THREAD_COMMUNICATOR_H

#include <QObject>
#include <QSharedPointer>
#include <QVariant>
#include <QMutex>
#include <QWaitCondition>
#include <QQueue>

namespace common {

/**
 * @brief Thread-safe message class
 * 
 * Encapsulates data to be passed between threads
 */
class ThreadMessage
{
public:
    enum MessageType {
        DataMessage,
        CommandMessage,
        StatusMessage,
        ErrorMessage
    };

    explicit ThreadMessage(MessageType type = DataMessage)
        : m_type(type)
    {}

    /**
     * @brief Set message type
     */
    void setType(MessageType type) { m_type = type; }

    /**
     * @brief Get message type
     */
    MessageType type() const { return m_type; }

    /**
     * @brief Set message data
     */
    void setData(const QVariant& data) { m_data = data; }

    /**
     * @brief Get message data
     */
    QVariant data() const { return m_data; }

    /**
     * @brief Set message sender
     */
    void setSender(QObject* sender) { m_sender = sender; }

    /**
     * @brief Get message sender
     */
    QObject* sender() const { return m_sender; }

private:
    MessageType m_type;
    QVariant m_data;
    QObject* m_sender = nullptr;
};

/**
 * @brief Thread communication interface
 * 
 * Provides standardized thread communication methods
 */
class ThreadCommunicator : public QObject
{
    Q_OBJECT

public:
    explicit ThreadCommunicator(QObject* parent = nullptr);
    ~ThreadCommunicator();

    /**
     * @brief Send message to another thread
     * @param message Message to send
     */
    void sendMessage(const ThreadMessage& message);

    /**
     * @brief Send data to another thread
     * @param data Data to send
     */
    void sendData(const QVariant& data);

    /**
     * @brief Send command to another thread
     * @param command Command to send
     */
    void sendCommand(const QString& command, const QVariant& data = QVariant());

    /**
     * @brief Send status to another thread
     * @param status Status to send
     */
    void sendStatus(const QString& status, int code = 0);

    /**
     * @brief Send error to another thread
     * @param error Error message to send
     */
    void sendError(const QString& error, int code = 0);

    /**
     * @brief Receive message from another thread
     * @param timeoutMs Timeout in milliseconds, -1 for infinite
     * @return Received message, or null if timeout
     */
    QSharedPointer<ThreadMessage> receiveMessage(int timeoutMs = -1);

    /**
     * @brief Check if there are pending messages
     */
    bool hasPendingMessages() const;

    /**
     * @brief Clear all pending messages
     */
    void clearPendingMessages();

signals:
    /**
     * @brief Emitted when a message is received
     */
    void messageReceived(const ThreadMessage& message);

    /**
     * @brief Emitted when data is received
     */
    void dataReceived(const QVariant& data);

    /**
     * @brief Emitted when a command is received
     */
    void commandReceived(const QString& command, const QVariant& data);

    /**
     * @brief Emitted when a status is received
     */
    void statusReceived(const QString& status, int code);

    /**
     * @brief Emitted when an error is received
     */
    void errorReceived(const QString& error, int code);

private slots:
    void onMessageReceived(const ThreadMessage& message);

private:
    mutable QMutex m_mutex;
    QWaitCondition m_condition;
    QQueue<QSharedPointer<ThreadMessage>> m_messageQueue;
};

/**
 * @brief Thread-safe data transfer class
 * 
 * Provides thread-safe data transfer between threads
 */
template<typename T>
class ThreadSafeData
{
public:
    explicit ThreadSafeData(const T& value = T())
        : m_value(value)
    {}

    /**
     * @brief Get value
     */
    T get() const
    {
        QMutexLocker locker(&m_mutex);
        return m_value;
    }

    /**
     * @brief Set value
     */
    void set(const T& value)
    {
        QMutexLocker locker(&m_mutex);
        m_value = value;
    }

    /**
     * @brief Update value with a function
     */
    template<typename Func>
    void update(Func func)
    {
        QMutexLocker locker(&m_mutex);
        func(m_value);
    }

private:
    mutable QMutex m_mutex;
    T m_value;
};

} // namespace common

#endif // THREAD_COMMUNICATOR_H