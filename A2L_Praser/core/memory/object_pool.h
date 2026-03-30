#ifndef A2L_PARSER_CORE_MEMORY_OBJECT_POOL_H
#define A2L_PARSER_CORE_MEMORY_OBJECT_POOL_H

#include <stack>
#include <mutex>
#include <memory>
#include <functional>
#include <atomic>

namespace a2l {

/**
 * @brief Thread-safe object pool for reducing memory allocation overhead.
 * 
 * Usage:
 *   ObjectPool<Node> pool;
 *   pool.preAllocate(1000);
 *   auto node = pool.acquire();
 *   // use node...
 *   // node returns to pool when unique_ptr is destroyed
 */
template<typename T>
class ObjectPool
{
public:
    using Deleter = std::function<void(T*)>;
    using PooledPtr = std::unique_ptr<T, Deleter>;

    ObjectPool() = default;
    
    ~ObjectPool()
    {
        std::lock_guard<std::mutex> lock(m_mutex);
        while (!m_pool.empty()) {
            delete m_pool.top();
            m_pool.pop();
        }
    }

    // Non-copyable, non-movable
    ObjectPool(const ObjectPool&) = delete;
    ObjectPool& operator=(const ObjectPool&) = delete;
    ObjectPool(ObjectPool&&) = delete;
    ObjectPool& operator=(ObjectPool&&) = delete;

    /**
     * @brief Acquire an object from pool or create new one.
     * @param args Constructor arguments
     * @return unique_ptr that returns object to pool on destruction
     */
    template<typename... Args>
    PooledPtr acquire(Args&&... args)
    {
        std::lock_guard<std::mutex> lock(m_mutex);
        
        T* obj = nullptr;
        if (m_pool.empty()) {
            obj = new T(std::forward<Args>(args)...);
            ++m_totalCreated;
        } else {
            obj = m_pool.top();
            m_pool.pop();
            --m_availableCount;
            // Reinitialize object if it has reset method
            resetObject(obj, std::forward<Args>(args)...);
        }
        
        return PooledPtr(obj, [this](T* ptr) { release(ptr); });
    }

    /**
     * @brief Pre-allocate objects to reduce runtime allocation.
     * @param count Number of objects to pre-allocate
     */
    void preAllocate(size_t count)
    {
        std::lock_guard<std::mutex> lock(m_mutex);
        for (size_t i = 0; i < count; ++i) {
            m_pool.push(new T());
            ++m_totalCreated;
            ++m_availableCount;
        }
    }

    /**
     * @brief Get statistics
     */
    size_t available() const 
    { 
        return m_availableCount.load(); 
    }
    
    size_t totalCreated() const 
    { 
        return m_totalCreated.load(); 
    }

private:
    void release(T* obj)
    {
        std::lock_guard<std::mutex> lock(m_mutex);
        m_pool.push(obj);
        ++m_availableCount;
    }

    // SFINAE to call reset() if available
    template<typename U, typename... Args>
    auto resetObject(U* obj, Args&&...) -> decltype(obj->reset(), void())
    {
        obj->reset();
    }
    
    template<typename U, typename... Args>
    auto resetObject(U* obj, Args&&... args) -> decltype(void())
    {
        // No reset method, reconstruct in place
        obj->~U();
        new (obj) U(std::forward<Args>(args)...);
    }

    std::stack<T*> m_pool;
    mutable std::mutex m_mutex;
    std::atomic<size_t> m_availableCount{0};
    std::atomic<size_t> m_totalCreated{0};
};

/**
 * @brief Memory pool manager for multiple object types.
 */
class MemoryPoolManager
{
public:
    static MemoryPoolManager& instance()
    {
        static MemoryPoolManager instance;
        return instance;
    }

    // Pre-allocate all pools for expected workload
    void warmUp(size_t nodeCount = 10000, size_t itemCount = 50000)
    {
        // Will be implemented based on actual Node/Item types
        Q_UNUSED(nodeCount);
        Q_UNUSED(itemCount);
    }

    // Clear all pools (release memory)
    void clear()
    {
        // Implementation based on actual pool storage
    }

private:
    MemoryPoolManager() = default;
    ~MemoryPoolManager() = default;
};

} // namespace a2l

#endif // A2L_PARSER_CORE_MEMORY_OBJECT_POOL_H
