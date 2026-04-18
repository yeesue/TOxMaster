/**
 * @file thread_pool.cpp
 * @brief Thread pool implementation
 */

#include "thread_pool.h"
#include <QtConcurrent>
#include <QCoreApplication>

namespace common {

ThreadPool* ThreadPool::m_instance = nullptr;

ThreadPool* ThreadPool::instance()
{
    if (!m_instance) {
        m_instance = new ThreadPool(QCoreApplication::instance());
    }
    return m_instance;
}

ThreadPool::ThreadPool(QObject* parent)
    : QObject(parent)
    , m_threadPool(nullptr)
    , m_running(false)
{
    initialize();
}

ThreadPool::~ThreadPool()
{
    stop(true);
    if (m_threadPool) {
        delete m_threadPool;
        m_threadPool = nullptr;
    }
}

void ThreadPool::initialize()
{
    m_threadPool = new QThreadPool(this);
    // Set default maximum thread count to number of CPU cores
    m_threadPool->setMaxThreadCount(QThread::idealThreadCount());
}

void ThreadPool::setMaxThreadCount(int maxThreadCount)
{
    if (m_threadPool) {
        m_threadPool->setMaxThreadCount(maxThreadCount);
    }
}

int ThreadPool::maxThreadCount() const
{
    if (m_threadPool) {
        return m_threadPool->maxThreadCount();
    }
    return 0;
}

void ThreadPool::start()
{
    if (!m_running && m_threadPool) {
        m_running = true;
        emit started();
    }
}

void ThreadPool::stop(bool waitForDone)
{
    if (m_running && m_threadPool) {
        m_running = false;
        if (waitForDone) {
            m_threadPool->waitForDone();
        }
        emit stopped();
    }
}

QFuture<void> ThreadPool::execute(const std::function<void()>& task)
{
    if (!m_running || !m_threadPool) {
        return QFuture<void>();
    }
    
    emit taskStarted();
    
    auto future = QtConcurrent::run(m_threadPool, [this, task]() {
        try {
            task();
        } catch (const std::exception& e) {
            qWarning() << "Thread pool task exception:" << e.what();
        } catch (...) {
            qWarning() << "Thread pool task unknown exception";
        }
        emit taskFinished();
    });
    
    return future;
}

bool ThreadPool::isRunning() const
{
    return m_running;
}

int ThreadPool::activeThreadCount() const
{
    if (m_threadPool) {
        return m_threadPool->activeThreadCount();
    }
    return 0;
}

void ThreadPool::clearPendingTasks()
{
    if (m_threadPool) {
        m_threadPool->clear();
    }
}

bool ThreadPool::waitForDone(int msecs)
{
    if (m_threadPool) {
        return m_threadPool->waitForDone(msecs);
    }
    return true;
}

RunnableTask::RunnableTask(const std::function<void()>& task, ThreadPool* pool)
    : m_task(task)
    , m_pool(pool)
{
    setAutoDelete(true);
}

void RunnableTask::run()
{
    if (m_pool && m_pool->isRunning()) {
        emit m_pool->taskStarted();
        try {
            m_task();
        } catch (const std::exception& e) {
            qWarning() << "Thread pool task exception:" << e.what();
        } catch (...) {
            qWarning() << "Thread pool task unknown exception";
        }
        emit m_pool->taskFinished();
    }
}

} // namespace common