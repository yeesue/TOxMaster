#include <QtTest/QtTest>
#include <QString>
#include <QElapsedTimer>
#include "../../A2L_Praser/core/memory/object_pool.h"
#include "../../A2L_Praser/core/memory/string_intern.h"
#include "../../A2L_Praser/core/concurrent/parallel_parser.h"

using namespace a2l;

/**
 * @brief Test class for ObjectPool
 */
class TestObject
{
public:
    TestObject() : m_value(0), m_name("") {}
    explicit TestObject(int value) : m_value(value), m_name("") {}
    TestObject(int value, const QString& name) : m_value(value), m_name(name) {}
    
    void reset()
    {
        m_value = 0;
        m_name.clear();
    }
    
    int value() const { return m_value; }
    QString name() const { return m_name; }
    void setValue(int v) { m_value = v; }
    void setName(const QString& n) { m_name = n; }

private:
    int m_value;
    QString m_name;
};

class TestPerformance : public QObject
{
    Q_OBJECT

private slots:
    // ObjectPool tests
    void testObjectPool_BasicAcquire();
    void testObjectPool_PreAllocate();
    void testObjectPool_Reuse();
    void testObjectPool_Statistics();
    void testObjectPool_ThreadSafety();
    
    // StringInternPool tests
    void testStringIntern_BasicIntern();
    void testStringIntern_MemorySaving();
    void testStringIntern_Clear();
    
    // ParallelParser tests
    void testParallelParser_SingleFile();
    void testParallelParser_MultipleFiles();
    
    // Performance benchmarks
    void benchmark_ObjectPool_vs_new();
    void benchmark_StringIntern_vs_copy();

private:
    QElapsedTimer m_timer;
};

// ==================== ObjectPool Tests ====================

void TestPerformance::testObjectPool_BasicAcquire()
{
    ObjectPool<TestObject> pool;
    
    auto obj = pool.acquire();
    QVERIFY(obj != nullptr);
    QCOMPARE(obj->value(), 0);
    QCOMPARE(pool.available(), 0);
    
    // Object returns to pool when destroyed
    obj.reset();
    QCOMPARE(pool.available(), 1);
}

void TestPerformance::testObjectPool_PreAllocate()
{
    ObjectPool<TestObject> pool;
    pool.preAllocate(100);
    
    QCOMPARE(pool.available(), 100);
    QCOMPARE(pool.totalCreated(), 100);
    
    // Acquire should use pre-allocated object
    auto obj = pool.acquire();
    QCOMPARE(pool.available(), 99);
}

void TestPerformance::testObjectPool_Reuse()
{
    ObjectPool<TestObject> pool;
    
    // Acquire and modify
    {
        auto obj = pool.acquire(42, "test");
        obj->setValue(100);
        obj->setName("modified");
    }
    
    // Object should be back in pool
    QCOMPARE(pool.available(), 1);
    
    // Re-acquire - should get same memory location
    auto obj2 = pool.acquire();
    // Note: After reset, values should be default
    QCOMPARE(obj2->value(), 0);
    QCOMPARE(obj2->name(), QString());
}

void TestPerformance::testObjectPool_Statistics()
{
    ObjectPool<TestObject> pool;
    
    QCOMPARE(pool.totalCreated(), 0);
    QCOMPARE(pool.available(), 0);
    
    pool.preAllocate(50);
    QCOMPARE(pool.totalCreated(), 50);
    QCOMPARE(pool.available(), 50);
    
    auto obj1 = pool.acquire();
    auto obj2 = pool.acquire();
    QCOMPARE(pool.available(), 48);
    
    // New object created when pool empty
    ObjectPool<TestObject> smallPool;
    smallPool.preAllocate(1);
    auto o1 = smallPool.acquire();
    QCOMPARE(smallPool.available(), 0);
    auto o2 = smallPool.acquire();  // Creates new
    QCOMPARE(smallPool.totalCreated(), 2);
}

void TestPerformance::testObjectPool_ThreadSafety()
{
    ObjectPool<TestObject> pool;
    pool.preAllocate(1000);
    
    QList<QFuture<void>> futures;
    QAtomicInt acquiredCount(0);
    
    // Multiple threads acquiring from same pool
    for (int i = 0; i < 10; ++i) {
        futures.append(QtConcurrent::run([&pool, &acquiredCount]() {
            for (int j = 0; j < 100; ++j) {
                auto obj = pool.acquire();
                obj->setValue(j);
                acquiredCount.fetchAndAddOrdered(1);
                // Small delay
                QThread::usleep(10);
            }
        }));
    }
    
    // Wait for all threads
    for (auto& f : futures) {
        f.waitForFinished();
    }
    
    QCOMPARE(acquiredCount.load(), 1000);
}

// ==================== StringIntern Tests ====================

void TestPerformance::testStringIntern_BasicIntern()
{
    StringInternPool& pool = StringInternPool::instance();
    pool.clear();
    
    QString s1 = pool.intern("TEST_STRING");
    QString s2 = pool.intern("TEST_STRING");
    
    // Same string should return same instance
    QCOMPARE(s1, s2);
    
    // Different strings should be different
    QString s3 = pool.intern("OTHER_STRING");
    QVERIFY(s1 != s3);
}

void TestPerformance::testStringIntern_MemorySaving()
{
    StringInternPool& pool = StringInternPool::instance();
    pool.clear();
    
    // Intern same string many times
    for (int i = 0; i < 1000; ++i) {
        pool.intern("REPEATED_KEYWORD");
    }
    
    QCOMPARE(pool.size(), 1);
    
    // Intern different strings
    for (int i = 0; i < 100; ++i) {
        pool.intern(QString("KEY_%1").arg(i));
    }
    
    QCOMPARE(pool.size(), 101);  // 1 + 100
}

void TestPerformance::testStringIntern_Clear()
{
    StringInternPool& pool = StringInternPool::instance();
    
    pool.intern("SOME_STRING");
    QVERIFY(pool.size() > 0);
    
    pool.clear();
    QCOMPARE(pool.size(), 0);
}

// ==================== ParallelParser Tests ====================

void TestPerformance::testParallelParser_SingleFile()
{
    int callCount = 0;
    ParallelParser parser([&callCount](const QString& file) {
        callCount++;
        return ParallelParseResult{file, true, "", 100, 10.0, nullptr};
    });
    
    QStringList files;
    files << "test1.a2l";
    
    QFuture<ParallelParseResult> result = parser.parseFiles(files);
    result.waitForFinished();
    
    QCOMPARE(callCount, 1);
    QCOMPARE(result.results().size(), 1);
    QCOMPARE(result.results().at(0).success, true);
}

void TestPerformance::testParallelParser_MultipleFiles()
{
    QStringList processedFiles;
    QMutex mutex;
    
    ParallelParser parser([&processedFiles, &mutex](const QString& file) {
        QMutexLocker locker(&mutex);
        processedFiles.append(file);
        return ParallelParseResult{file, true, "", 100, 10.0, nullptr};
    });
    
    parser.setMaxThreadCount(4);
    
    QStringList files;
    for (int i = 0; i < 10; ++i) {
        files << QString("test%1.a2l").arg(i);
    }
    
    QFuture<ParallelParseResult> result = parser.parseFiles(files);
    result.waitForFinished();
    
    QCOMPARE(processedFiles.size(), 10);
    for (int i = 0; i < 10; ++i) {
        QVERIFY(processedFiles.contains(QString("test%1.a2l").arg(i)));
    }
}

// ==================== Benchmarks ====================

void TestPerformance::benchmark_ObjectPool_vs_new()
{
    const int iterations = 10000;
    
    // Benchmark new/delete
    QBENCHMARK {
        for (int i = 0; i < iterations; ++i) {
            TestObject* obj = new TestObject(i);
            delete obj;
        }
    }
    
    // Benchmark ObjectPool
    ObjectPool<TestObject> pool;
    pool.preAllocate(iterations);
    
    QBENCHMARK {
        for (int i = 0; i < iterations; ++i) {
            auto obj = pool.acquire(i);
            Q_UNUSED(obj);
        }
    }
}

void TestPerformance::benchmark_StringIntern_vs_copy()
{
    const int iterations = 10000;
    const QString testString = "SOME_LONG_KEYWORD_NAME_THAT_IS_REPEATED";
    
    // Benchmark QString copy
    QBENCHMARK {
        for (int i = 0; i < iterations; ++i) {
            QString copy = testString;
            Q_UNUSED(copy);
        }
    }
    
    // Benchmark StringIntern
    StringInternPool& pool = StringInternPool::instance();
    pool.clear();
    
    QBENCHMARK {
        for (int i = 0; i < iterations; ++i) {
            QString interned = pool.intern(testString);
            Q_UNUSED(interned);
        }
    }
}

QTEST_MAIN(TestPerformance)
#include "tst_performance.moc"
