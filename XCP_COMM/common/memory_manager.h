#ifndef MEMORY_MANAGER_H
#define MEMORY_MANAGER_H

#include <QObject>
#include <QHash>
#include <QMutex>
#include <QSharedPointer>

class MemoryManager : public QObject
{
    Q_OBJECT

public:
    static MemoryManager* instance();

    // 创建或获取共享内存
    void* createMemory(const QString& key, int size);

    // 获取共享内存
    void* getMemory(const QString& key);

    // 释放共享内存
    void releaseMemory(const QString& key);

    // 检查共享内存是否存在
    bool hasMemory(const QString& key);

    // 获取共享内存大小
    int getMemorySize(const QString& key);

private:
    MemoryManager(QObject *parent = nullptr);
    ~MemoryManager();

    static MemoryManager* m_instance;
    static QMutex m_instanceMutex;

    QHash<QString, QSharedPointer<QByteArray>> m_memoryMap;
    QMutex m_memoryMutex;
};

#endif // MEMORY_MANAGER_H