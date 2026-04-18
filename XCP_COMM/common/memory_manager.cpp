#include "memory_manager.h"

MemoryManager* MemoryManager::m_instance = nullptr;
QMutex MemoryManager::m_instanceMutex;

MemoryManager::MemoryManager(QObject *parent) : QObject(parent)
{
}

MemoryManager::~MemoryManager()
{
}

MemoryManager* MemoryManager::instance()
{
    if (!m_instance)
    {
        QMutexLocker locker(&m_instanceMutex);
        if (!m_instance)
        {
            m_instance = new MemoryManager();
        }
    }
    return m_instance;
}

void* MemoryManager::createMemory(const QString& key, int size)
{
    QMutexLocker locker(&m_memoryMutex);
    
    if (!m_memoryMap.contains(key))
    {
        QSharedPointer<QByteArray> memory = QSharedPointer<QByteArray>(new QByteArray(size, 0));
        m_memoryMap.insert(key, memory);
    }
    else if (m_memoryMap.value(key)->size() < size)
    {
        // 如果内存大小不够，重新分配
        QSharedPointer<QByteArray> memory = QSharedPointer<QByteArray>(new QByteArray(size, 0));
        m_memoryMap.insert(key, memory);
    }
    
    return m_memoryMap.value(key)->data();
}

void* MemoryManager::getMemory(const QString& key)
{
    QMutexLocker locker(&m_memoryMutex);
    
    if (m_memoryMap.contains(key))
    {
        return m_memoryMap.value(key)->data();
    }
    return nullptr;
}

void MemoryManager::releaseMemory(const QString& key)
{
    QMutexLocker locker(&m_memoryMutex);
    m_memoryMap.remove(key);
}

bool MemoryManager::hasMemory(const QString& key)
{
    QMutexLocker locker(&m_memoryMutex);
    return m_memoryMap.contains(key);
}

int MemoryManager::getMemorySize(const QString& key)
{
    QMutexLocker locker(&m_memoryMutex);
    if (m_memoryMap.contains(key))
    {
        return m_memoryMap.value(key)->size();
    }
    return 0;
}