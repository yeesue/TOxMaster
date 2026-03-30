#ifndef A2L_PARSER_CORE_MEMORY_NODE_POOL_H
#define A2L_PARSER_CORE_MEMORY_NODE_POOL_H

#include "object_pool.h"
#include "string_intern.h"
#include <QMutex>
#include <QHash>
#include <functional>

// Forward declarations
class Node;
class Item;

namespace a2l {

/**
 * @brief Memory pool manager specifically for A2L Node/Item objects.
 * 
 * Provides centralized memory management for the parser AST nodes.
 * Usage:
 *   NodePoolManager::instance().initialize(10000, 50000);
 *   Node* node = NodePoolManager::instance().createNode<NodeType>(parent, lexer, errors);
 *   node->setName("CHARACTERISTIC");  // Interned automatically
 */
class NodePoolManager
{
public:
    static NodePoolManager& instance()
    {
        static NodePoolManager instance;
        return instance;
    }

    /**
     * @brief Initialize pools with expected object counts.
     * Call before parsing large A2L files.
     */
    void initialize(size_t nodeCount = 10000, size_t itemCount = 50000)
    {
        QMutexLocker locker(&m_mutex);
        m_nodeCount = nodeCount;
        m_itemCount = itemCount;
        m_initialized = true;
    }

    /**
     * @brief Get string interning pool.
     */
    StringInternPool& stringPool()
    {
        return StringInternPool::instance();
    }

    /**
     * @brief Intern a string for node name.
     * Reduces memory for repeated keywords like "CHARACTERISTIC", "MEASUREMENT".
     */
    QString internString(const QString& str)
    {
        return StringInternPool::instance().intern(str);
    }

    /**
     * @brief Intern a C-string for node name.
     */
    QString internString(const char* str)
    {
        return StringInternPool::instance().intern(str);
    }

    /**
     * @brief Intern a string from char* with explicit length.
     */
    QString internString(const char* str, int length)
    {
        if (!str || length <= 0) return QString();
        return StringInternPool::instance().intern(QString::fromLatin1(str, length));
    }

    /**
     * @brief Get statistics.
     */
    struct Stats {
        size_t nodeCount;
        size_t itemCount;
        size_t stringCount;
        size_t totalMemoryEstimate;
    };
    
    Stats getStats() const
    {
        QMutexLocker locker(&m_mutex);
        Stats s;
        s.nodeCount = m_nodeCount;
        s.itemCount = m_itemCount;
        s.stringCount = StringInternPool::instance().size();
        // Rough estimate: Node ~200 bytes, Item ~100 bytes
        s.totalMemoryEstimate = m_nodeCount * 200 + m_itemCount * 100;
        return s;
    }

    /**
     * @brief Reset pools (call between parsing different files).
     */
    void reset()
    {
        QMutexLocker locker(&m_mutex);
        StringInternPool::instance().clear();
    }

    bool isInitialized() const { return m_initialized; }

private:
    NodePoolManager() : m_initialized(false), m_nodeCount(0), m_itemCount(0) {}
    ~NodePoolManager() = default;
    NodePoolManager(const NodePoolManager&) = delete;
    NodePoolManager& operator=(const NodePoolManager&) = delete;

    mutable QMutex m_mutex;
    bool m_initialized;
    size_t m_nodeCount;
    size_t m_itemCount;
};

/**
 * @brief RAII helper for parsing sessions.
 * Automatically initializes pools and cleans up.
 */
class ParseSession
{
public:
    explicit ParseSession(size_t expectedNodes = 10000, size_t expectedItems = 50000)
    {
        NodePoolManager::instance().initialize(expectedNodes, expectedItems);
    }
    
    ~ParseSession()
    {
        // Optionally reset pools after parsing
        // NodePoolManager::instance().reset();
    }

    // Get statistics after parsing
    NodePoolManager::Stats getStats() const
    {
        return NodePoolManager::instance().getStats();
    }
};

/**
 * @brief Helper to intern node names automatically.
 * Use this when setting node->name to ensure string deduplication.
 */
class InternedName
{
public:
    explicit InternedName(const QString& name)
        : m_interned(NodePoolManager::instance().internString(name))
        , m_cstr(nullptr)
    {
    }
    
    explicit InternedName(const char* name)
        : m_interned(NodePoolManager::instance().internString(name))
        , m_cstr(nullptr)
    {
    }

    // Get as QString (shared copy)
    const QString& asQString() const { return m_interned; }
    
    // Get as C-string (WARNING: only valid while QString exists)
    const char* c_str() const
    {
        if (!m_cstr) {
            m_localBuffer = m_interned.toLocal8Bit();
            m_cstr = m_localBuffer.constData();
        }
        return m_cstr;
    }

private:
    QString m_interned;
    mutable QByteArray m_localBuffer;
    mutable const char* m_cstr;
};

} // namespace a2l

#endif // A2L_PARSER_CORE_MEMORY_NODE_POOL_H
