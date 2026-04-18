/**
 * @file thread_pool.h
 * @brief Thread pool implementation
 * 
 * Provides a thread pool for managing worker threads
 * Supports task queueing and thread lifecycle management
 */

#ifndef THREAD_POOL_H
#define THREAD_POOL_H

#include <QObject>
#include <QThread>
#include <QRunnable>
#include <QThreadPool>
#include <QFuture>
#include <QFutureWatcher>
#include <QtConcurrent>
#include <functional>

namespace common {

/**
 * @brief Thread pool class
 * 
 * Manages a pool of worker threads for executing tasks
 * Provides a unified interface for thread creation and management
 */
class ThreadPool : public QObject
{
    Q_OBJECT

public:
    /**
     * @brief Get thread pool instance (singleton)
     */
    static ThreadPool* instance();
    
    /**
     * @brief Destructor
     */
    ~ThreadPool();
    
    /**
     * @brief Set maximum thread count
     * @param maxThreadCount Maximum number of threads
     */
    void setMaxThreadCount(int maxThreadCount);
    
    /**
     * @brief Get maximum thread count
     */
    int maxThreadCount() const;
    
    /**
     * @brief Start the thread pool
     */
    void start();
    
    /**
     * @brief Stop the thread pool
     * @param waitForDone Whether to wait for all tasks to complete
     */
    void stop(bool waitForDone = true);
    
    /**
     * @brief Execute a task
     * @param task Task function
     * @return Future object for the task
     */
    QFuture<void> execute(const std::function<void()>& task);
    
    /**
     * @brief Execute a task with result
     * @param task Task function that returns a result
     * @return Future object for the task
     */
    template<typename T>
    QFuture<T> executeWithResult(const std::function<T()>& task)
    {
        return QtConcurrent::run(m_threadPool, task);
    }
    
    /**
     * @brief Check if the thread pool is running
     */
    bool isRunning() const;
    
    /**
     * @brief Get current thread count
     */
    int activeThreadCount() const;
    
    /**
     * @brief Clear all pending tasks
     */
    void clearPendingTasks();
    
    /**
     * @brief Wait for all tasks to complete
     * @param msecs Timeout in milliseconds, -1 for infinite
     * @return true if all tasks completed within timeout
     */
    bool waitForDone(int msecs = -1);
    
signals:
    /**
     * @brief Emitted when a task starts
     */
    void taskStarted();
    
    /**
     * @brief Emitted when a task finishes
     */
    void taskFinished();
    
    /**
     * @brief Emitted when the thread pool starts
     */
    void started();
    
    /**
     * @brief Emitted when the thread pool stops
     */
    void stopped();
    
private:
    /**
     * @brief Constructor (private for singleton)
     */
    explicit ThreadPool(QObject* parent = nullptr);
    
    /**
     * @brief Initialize the thread pool
     */
    void initialize();
    
private:
    static ThreadPool* m_instance;
    QThreadPool* m_threadPool;
    bool m_running;
};

/**
 * @brief Task wrapper for QRunnable
 */
class RunnableTask : public QRunnable
{
public:
    explicit RunnableTask(const std::function<void()>& task, ThreadPool* pool);
    
    void run() override;
    
private:
    std::function<void()> m_task;
    ThreadPool* m_pool;
};

} // namespace common

#endif // THREAD_POOL_H