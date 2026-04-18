/**
 * @file thread_communicator.cpp
 * @brief Thread communication implementation
 */

#include "thread_communicator.h"

namespace common {

ThreadCommunicator::ThreadCommunicator(QObject* parent)
    : QObject(parent)
{
    // Connect the messageReceived signal to the onMessageReceived slot
    connect(this, &ThreadCommunicator::messageReceived, this, &ThreadCommunicator::onMessageReceived, Qt::QueuedConnection);
}

ThreadCommunicator::~ThreadCommunicator()
{
    clearPendingMessages();
}

void ThreadCommunicator::sendMessage(const ThreadMessage& message)
{
    emit messageReceived(message);
}

void ThreadCommunicator::sendData(const QVariant& data)
{
    ThreadMessage message(ThreadMessage::DataMessage);
    message.setData(data);
    sendMessage(message);
}

void ThreadCommunicator::sendCommand(const QString& command, const QVariant& data)
{
    ThreadMessage message(ThreadMessage::CommandMessage);
    QVariantMap commandData;
    commandData["command"] = command;
    commandData["data"] = data;
    message.setData(commandData);
    sendMessage(message);
}

void ThreadCommunicator::sendStatus(const QString& status, int code)
{
    ThreadMessage message(ThreadMessage::StatusMessage);
    QVariantMap statusData;
    statusData["status"] = status;
    statusData["code"] = code;
    message.setData(statusData);
    sendMessage(message);
}

void ThreadCommunicator::sendError(const QString& error, int code)
{
    ThreadMessage message(ThreadMessage::ErrorMessage);
    QVariantMap errorData;
    errorData["error"] = error;
    errorData["code"] = code;
    message.setData(errorData);
    sendMessage(message);
}

QSharedPointer<ThreadMessage> ThreadCommunicator::receiveMessage(int timeoutMs)
{
    QMutexLocker locker(&m_mutex);
    
    if (m_messageQueue.isEmpty()) {
        if (timeoutMs < 0) {
            // Wait indefinitely
            m_condition.wait(&m_mutex);
        } else {
            // Wait for timeout
            if (!m_condition.wait(&m_mutex, timeoutMs)) {
                // Timeout
                return nullptr;
            }
        }
    }
    
    if (!m_messageQueue.isEmpty()) {
        return m_messageQueue.dequeue();
    }
    
    return nullptr;
}

bool ThreadCommunicator::hasPendingMessages() const
{
    QMutexLocker locker(&m_mutex);
    return !m_messageQueue.isEmpty();
}

void ThreadCommunicator::clearPendingMessages()
{
    QMutexLocker locker(&m_mutex);
    m_messageQueue.clear();
}

void ThreadCommunicator::onMessageReceived(const ThreadMessage& message)
{
    QSharedPointer<ThreadMessage> messagePtr(new ThreadMessage(message));
    
    { 
        QMutexLocker locker(&m_mutex);
        m_messageQueue.enqueue(messagePtr);
        m_condition.wakeOne();
    }
    
    // Emit specific signals based on message type
    switch (message.type()) {
    case ThreadMessage::DataMessage:
        emit dataReceived(message.data());
        break;
    case ThreadMessage::CommandMessage:
    {
        QVariantMap commandData = message.data().toMap();
        QString command = commandData.value("command").toString();
        QVariant data = commandData.value("data");
        emit commandReceived(command, data);
        break;
    }
    case ThreadMessage::StatusMessage:
    {
        QVariantMap statusData = message.data().toMap();
        QString status = statusData.value("status").toString();
        int code = statusData.value("code").toInt();
        emit statusReceived(status, code);
        break;
    }
    case ThreadMessage::ErrorMessage:
    {
        QVariantMap errorData = message.data().toMap();
        QString error = errorData.value("error").toString();
        int code = errorData.value("code").toInt();
        emit errorReceived(error, code);
        break;
    }
    default:
        break;
    }
}

} // namespace common