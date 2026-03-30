#ifndef A2L_PARSER_CORE_MEMORY_STRING_INTERN_H
#define A2L_PARSER_CORE_MEMORY_STRING_INTERN_H

#include <QString>
#include <QHash>
#include <QMutex>
#include <QStringRef>
#include <memory>

namespace a2l {

/**
 * @brief String interning pool for reducing memory usage of repeated strings.
 * 
 * A2L files contain many repeated strings (e.g., type names, keywords).
 * This pool stores unique copies only, reducing memory footprint.
 * 
 * Usage:
 *   StringInternPool& pool = StringInternPool::instance();
 *   QString interned = pool.intern("SOME_KEYWORD");
 *   // All subsequent calls with same string return same reference
 */
class StringInternPool
{
public:
    static StringInternPool& instance()
    {
        static StringInternPool instance;
        return instance;
    }

    /**
     * @brief Intern a string - returns shared copy.
     * @param str String to intern
     * @return Interned string (shared among all same values)
     */
    QString intern(const QString& str)
    {
        if (str.isEmpty()) {
            return QString();
        }
        
        QMutexLocker locker(&m_mutex);
        auto it = m_pool.find(str);
        if (it != m_pool.end()) {
            return it.value();
        }
        
        // Store a copy in the pool
        QString interned(str);
        m_pool.insert(interned, interned);
        return interned;
    }

    /**
     * @brief Intern a string from QStringRef (avoids temporary QString).
     */
    QString intern(const QStringRef& ref)
    {
        if (ref.isEmpty()) {
            return QString();
        }
        
        return intern(ref.toString());
    }

    /**
     * @brief Intern a string from char* (for literal strings).
     */
    QString intern(const char* str)
    {
        if (!str || !str[0]) {
            return QString();
        }
        
        QString qstr = QString::fromLatin1(str);
        return intern(qstr);
    }

    /**
     * @brief Clear the pool (release memory).
     */
    void clear()
    {
        QMutexLocker locker(&m_mutex);
        m_pool.clear();
    }

    /**
     * @brief Get pool statistics.
     */
    size_t size() const
    {
        QMutexLocker locker(&m_mutex);
        return static_cast<size_t>(m_pool.size());
    }

    /**
     * @brief Estimate memory savings.
     * @return Approximate bytes saved by interning
     */
    size_t memorySaved() const
    {
        // Each interned string saves (refCount - 1) * stringSize
        // This is a rough estimate
        QMutexLocker locker(&m_mutex);
        return m_pool.size() * sizeof(QString);
    }

private:
    StringInternPool() = default;
    ~StringInternPool() = default;
    StringInternPool(const StringInternPool&) = delete;
    StringInternPool& operator=(const StringInternPool&) = delete;

    mutable QMutex m_mutex;
    QHash<QString, QString> m_pool;
};

/**
 * @brief RAII helper for temporary string buffers.
 * 
 * Reuses pre-allocated buffers for string operations,
 * avoiding repeated memory allocations.
 */
class StringBuffer
{
public:
    static StringBuffer& instance()
    {
        static StringBuffer instance;
        return instance;
    }

    /**
     * @brief Get a temporary buffer for string building.
     */
    QString& acquire()
    {
        QMutexLocker locker(&m_mutex);
        if (m_buffers.isEmpty()) {
            m_buffers.append(QString());
        }
        QString& buf = m_buffers.last();
        m_buffers.removeLast();
        buf.clear();
        return buf;
    }

    /**
     * @brief Return buffer to pool after use.
     */
    void release(QString& buffer)
    {
        QMutexLocker locker(&m_mutex);
        m_buffers.append(buffer);
    }

private:
    StringBuffer() 
    {
        // Pre-allocate some buffers
        for (int i = 0; i < 4; ++i) {
            m_buffers.append(QString());
        }
    }
    
    ~StringBuffer() = default;

    mutable QMutex m_mutex;
    QList<QString> m_buffers;
};

/**
 * @brief Helper class for efficient string building.
 */
class StringBuilder
{
public:
    StringBuilder() : m_buffer(StringBuffer::instance().acquire()) {}
    ~StringBuilder() { StringBuffer::instance().release(m_buffer); }

    StringBuilder& append(const QString& str)
    {
        m_buffer.append(str);
        return *this;
    }

    StringBuilder& append(const QStringRef& ref)
    {
        m_buffer.append(ref);
        return *this;
    }

    StringBuilder& append(char c)
    {
        m_buffer.append(c);
        return *this;
    }

    StringBuilder& append(const char* str)
    {
        m_buffer.append(QString::fromLatin1(str));
        return *this;
    }

    QString toString() const
    {
        return m_buffer;
    }

    void clear()
    {
        m_buffer.clear();
    }

private:
    QString& m_buffer;
};

} // namespace a2l

#endif // A2L_PARSER_CORE_MEMORY_STRING_INTERN_H
