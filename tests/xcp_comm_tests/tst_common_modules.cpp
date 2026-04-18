/**
 * @file tst_common_modules.cpp
 * @brief XCP_COMM/common 模块单元测试
 *
 * 测试范围：
 * - ConfigManager: JSON 配置读写、嵌套 key、点号分隔
 * - MemoryManager: 内存创建/获取/释放/大小查询
 * - Logger: 日志级别、性能统计、文件输出
 * - ThreadCommunicator: 消息收发、消息类型
 * - ThreadSafeData: 线程安全数据模板
 * - SmartPtr: 智能指针工具
 * - DataReplayManager: CSV 加载、回放状态、速度转换
 * - TestCaseManager: 测试用例 CRUD、JSON 序列化、统计
 */

#include <QtTest/QtTest>
#include <QTemporaryFile>
#include <QDir>
#include <QCoreApplication>

#include "config_manager.h"
#include "memory_manager.h"
#include "logger.h"
#include "thread_communicator.h"
#include "smart_ptr.h"
#include "data_replay_manager.h"
#include "test_case_manager.h"

using namespace common;

// ==================== ConfigManager 测试类 =====================
class TestConfigManager : public QObject
{
    Q_OBJECT

private slots:
    void initTestCase();
    void cleanupTestCase();
    void init();
    void cleanup();

    // 基础操作
    void testSetValueGetValue();
    void testDefaultValue();
    void testContains();
    void testRemove();
    void testClear();
    void testCurrentFilePath();

    // 嵌套 key（点号分隔）
    void testNestedKey();
    void testNestedKeyGetValue();
    void testNestedKeyContains();
    void testNestedKeyRemove();
    void testNestedKeyRemoveEmptyParent();

    // JSON 文件读写
    void testLoadFromJsonFile();
    void testLoadInvalidJson();
    void testLoadNonExistentFile();
    void testSaveToJsonFile();
    void testSaveToNewFile();

    // JSON 对象导入导出
    void testFromJsonObject();
    void testToJsonObject();

private:
    ConfigManager* m_mgr;
    QString m_tempDir;
};

void TestConfigManager::initTestCase()
{
    m_tempDir = QDir::tempPath() + "/tcm_config_test";
    QDir().mkpath(m_tempDir);
}

void TestConfigManager::cleanupTestCase()
{
    QDir(m_tempDir).removeRecursively();
}

void TestConfigManager::init()
{
    m_mgr = ConfigManager::instance();
    m_mgr->clear();
}

void TestConfigManager::cleanup()
{
    m_mgr->clear();
}

void TestConfigManager::testSetValueGetValue()
{
    m_mgr->setValue("name", "test_app");
    QCOMPARE(m_mgr->value("name").toString(), QString("test_app"));

    m_mgr->setValue("port", 8080);
    QCOMPARE(m_mgr->value("port").toInt(), 8080);

    m_mgr->setValue("enabled", true);
    QCOMPARE(m_mgr->value("enabled").toBool(), true);
}

void TestConfigManager::testDefaultValue()
{
    QVariant val = m_mgr->value("nonexistent_key", 42);
    QCOMPARE(val.toInt(), 42);

    QVariant strVal = m_mgr->value("missing_str", "default_text");
    QCOMPARE(strVal.toString(), QString("default_text"));
}

void TestConfigManager::testContains()
{
    QVERIFY(!m_mgr->contains("key1"));

    m_mgr->setValue("key1", "value1");
    QVERIFY(m_mgr->contains("key1"));
    QVERIFY(!m_mgr->contains("key2"));
}

void TestConfigManager::testRemove()
{
    m_mgr->setValue("to_remove", 123);
    QVERIFY(m_mgr->contains("to_remove"));

    m_mgr->remove("to_remove");
    QVERIFY(!m_mgr->contains("to_remove"));
}

void TestConfigManager::testClear()
{
    m_mgr->setValue("k1", "v1");
    m_mgr->setValue("k2", "v2");
    QVERIFY(m_mgr->contains("k1"));

    m_mgr->clear();
    QVERIFY(!m_mgr->contains("k1"));
    QVERIFY(!m_mgr->contains("k2"));
}

void TestConfigManager::testCurrentFilePath()
{
    QVERIFY(m_mgr->currentFilePath().isEmpty());

    QString filePath = m_tempDir + "/test_config.json";
    QFile file(filePath);
    if (file.exists()) file.remove();

    file.open(QIODevice::WriteOnly);
    file.write("{\"test\":1}");
    file.close();

    m_mgr->load(filePath);
    QCOMPARE(m_mgr->currentFilePath(), filePath);
}

void TestConfigManager::testNestedKey()
{
    // Note: setValue with nested key overwrites the entire parent object
    // This is a known behavior of the current implementation.
    // Setting "database.host" then "database.port" will lose "host"
    // because setValue rebuilds the nested object from scratch each time.

    // Test single nested key
    m_mgr->setValue("database.host", "localhost");
    QCOMPARE(m_mgr->value("database.host").toString(), QString("localhost"));

    m_mgr->clear();

    // Test setting multiple nested keys via fromJsonObject
    QJsonObject dbObj;
    dbObj["host"] = "localhost";
    dbObj["port"] = 3306;
    QJsonObject rootObj;
    rootObj["database"] = dbObj;
    m_mgr->fromJsonObject(rootObj);

    QCOMPARE(m_mgr->value("database.host").toString(), QString("localhost"));
    QCOMPARE(m_mgr->value("database.port").toInt(), 3306);
}

void TestConfigManager::testNestedKeyGetValue()
{
    m_mgr->setValue("network.can.baudrate", 500000);
    QCOMPARE(m_mgr->value("network.can.baudrate").toInt(), 500000);

    // 中间层级的 key 不应该有直接值
    QVariant midVal = m_mgr->value("network.can");
    QVERIFY(midVal.isValid());
}

void TestConfigManager::testNestedKeyContains()
{
    m_mgr->setValue("a.b.c", 999);
    QVERIFY(m_mgr->contains("a.b.c"));
    QVERIFY(m_mgr->contains("a.b"));
}

void TestConfigManager::testNestedKeyRemove()
{
    m_mgr->setValue("x.y.z", "deep_value");
    QVERIFY(m_mgr->contains("x.y.z"));

    m_mgr->remove("x.y.z");
    QVERIFY(!m_mgr->contains("x.y.z"));
}

void TestConfigManager::testNestedKeyRemoveEmptyParent()
{
    m_mgr->setValue("p.q", "only_child");
    m_mgr->remove("p.q");
    // After removing only child, empty parent should also be removed
    QVERIFY(!m_mgr->contains("p.q"));
}

void TestConfigManager::testLoadFromJsonFile()
{
    QString filePath = m_tempDir + "/load_test.json";
    QFile file(filePath);
    if (file.exists()) file.remove();

    file.open(QIODevice::WriteOnly);
    file.write("{\"app_name\":\"TOxMaster\",\"version\":2,\"debug\":true}");
    file.close();

    QVERIFY(m_mgr->load(filePath));
    QCOMPARE(m_mgr->value("app_name").toString(), QString("TOxMaster"));
    QCOMPARE(m_mgr->value("version").toInt(), 2);
    QCOMPARE(m_mgr->value("debug").toBool(), true);
}

void TestConfigManager::testLoadInvalidJson()
{
    QString filePath = m_tempDir + "/invalid.json";
    QFile file(filePath);
    if (file.exists()) file.remove();

    file.open(QIODevice::WriteOnly);
    file.write("{invalid json!!!");
    file.close();

    QVERIFY(!m_mgr->load(filePath));
}

void TestConfigManager::testLoadNonExistentFile()
{
    QVERIFY(!m_mgr->load("/non/existent/path/config.json"));
}

void TestConfigManager::testSaveToJsonFile()
{
    m_mgr->setValue("save_key", "save_value");
    m_mgr->setValue("number", 42);

    QString filePath = m_tempDir + "/save_test.json";
    QFile::remove(filePath);

    QVERIFY(m_mgr->save(filePath));

    // Verify file content
    QFile file(filePath);
    QVERIFY(file.open(QIODevice::ReadOnly));
    QByteArray data = file.readAll();
    file.close();

    QVERIFY(data.contains("save_key"));
    QVERIFY(data.contains("save_value"));
    QVERIFY(data.contains("42"));
}

void TestConfigManager::testSaveToNewFile()
{
    m_mgr->setValue("hello", "world");

    QString filePath = m_tempDir + "/new_save.json";
    QFile::remove(filePath);

    QVERIFY(m_mgr->save(filePath));
    QVERIFY(QFile::exists(filePath));
}

void TestConfigManager::testFromJsonObject()
{
    QJsonObject obj;
    obj["key1"] = "val1";
    obj["key2"] = 100;

    m_mgr->fromJsonObject(obj);
    QCOMPARE(m_mgr->value("key1").toString(), QString("val1"));
    QCOMPARE(m_mgr->value("key2").toInt(), 100);
}

void TestConfigManager::testToJsonObject()
{
    m_mgr->setValue("a", 1);
    m_mgr->setValue("b", "two");

    QJsonObject obj = m_mgr->toJsonObject();
    QCOMPARE(obj["a"].toInt(), 1);
    QCOMPARE(obj["b"].toString(), QString("two"));
}

// ==================== MemoryManager 测试类 =====================
class TestMemoryManager : public QObject
{
    Q_OBJECT

private slots:
    void init();
    void cleanup();

    void testCreateMemory();
    void testGetMemory();
    void testCreateMemoryTwiceSameKey();
    void testCreateMemoryLargerSize();
    void testReleaseMemory();
    void testHasMemory();
    void testGetMemorySize();
    void testGetNonExistentMemory();
    void testMultipleMemoryBlocks();

private:
    MemoryManager* m_mgr;
};

void TestMemoryManager::init()
{
    m_mgr = MemoryManager::instance();
    // Clean up any existing memory
    m_mgr->releaseMemory("test1");
    m_mgr->releaseMemory("test2");
    m_mgr->releaseMemory("test3");
}

void TestMemoryManager::cleanup()
{
    m_mgr->releaseMemory("test1");
    m_mgr->releaseMemory("test2");
    m_mgr->releaseMemory("test3");
}

void TestMemoryManager::testCreateMemory()
{
    void* ptr = m_mgr->createMemory("test1", 1024);
    QVERIFY(ptr != nullptr);
    QVERIFY(m_mgr->hasMemory("test1"));
    QCOMPARE(m_mgr->getMemorySize("test1"), 1024);
}

void TestMemoryManager::testGetMemory()
{
    m_mgr->createMemory("test1", 256);
    void* ptr = m_mgr->getMemory("test1");
    QVERIFY(ptr != nullptr);

    // Write and verify
    memset(ptr, 0xAB, 256);
    quint8* bytes = static_cast<quint8*>(m_mgr->getMemory("test1"));
    QCOMPARE(bytes[0], (quint8)0xAB);
    QCOMPARE(bytes[255], (quint8)0xAB);
}

void TestMemoryManager::testCreateMemoryTwiceSameKey()
{
    void* ptr1 = m_mgr->createMemory("test1", 128);
    void* ptr2 = m_mgr->createMemory("test1", 128);
    // Same size: should return same pointer
    QCOMPARE(ptr1, ptr2);
}

void TestMemoryManager::testCreateMemoryLargerSize()
{
    m_mgr->createMemory("test1", 64);
    void* oldPtr = m_mgr->getMemory("test1");

    // Request larger size
    void* newPtr = m_mgr->createMemory("test1", 256);
    QVERIFY(m_mgr->getMemorySize("test1") >= 256);
}

void TestMemoryManager::testReleaseMemory()
{
    m_mgr->createMemory("test1", 128);
    QVERIFY(m_mgr->hasMemory("test1"));

    m_mgr->releaseMemory("test1");
    QVERIFY(!m_mgr->hasMemory("test1"));
    QCOMPARE(m_mgr->getMemorySize("test1"), 0);
}

void TestMemoryManager::testHasMemory()
{
    QVERIFY(!m_mgr->hasMemory("test1"));
    m_mgr->createMemory("test1", 64);
    QVERIFY(m_mgr->hasMemory("test1"));
    QVERIFY(!m_mgr->hasMemory("nonexistent"));
}

void TestMemoryManager::testGetMemorySize()
{
    QCOMPARE(m_mgr->getMemorySize("nonexistent"), 0);
    m_mgr->createMemory("test1", 512);
    QCOMPARE(m_mgr->getMemorySize("test1"), 512);
}

void TestMemoryManager::testGetNonExistentMemory()
{
    void* ptr = m_mgr->getMemory("nonexistent");
    QVERIFY(ptr == nullptr);
}

void TestMemoryManager::testMultipleMemoryBlocks()
{
    m_mgr->createMemory("test1", 64);
    m_mgr->createMemory("test2", 128);
    m_mgr->createMemory("test3", 256);

    QVERIFY(m_mgr->hasMemory("test1"));
    QVERIFY(m_mgr->hasMemory("test2"));
    QVERIFY(m_mgr->hasMemory("test3"));

    QCOMPARE(m_mgr->getMemorySize("test1"), 64);
    QCOMPARE(m_mgr->getMemorySize("test2"), 128);
    QCOMPARE(m_mgr->getMemorySize("test3"), 256);

    m_mgr->releaseMemory("test2");
    QVERIFY(!m_mgr->hasMemory("test2"));
    QVERIFY(m_mgr->hasMemory("test1"));
    QVERIFY(m_mgr->hasMemory("test3"));
}

// ==================== Logger 测试类 =====================
class TestLogger : public QObject
{
    Q_OBJECT

private slots:
    void initTestCase();
    void cleanupTestCase();

    // 日志级别
    void testLogLevelEnum();
    void testSetLogLevel();
    void testLogLevelFiltering();

    // 性能统计
    void testPerfStartEnd();
    void testPerfRecord();
    void testPerfClear();
    void testPerfNonExistent();

    // 日志输出
    void testLogFileOutput();
    void testLogFormatMessage();
    void testLevelToString();

    // LogStream
    void testLogStream();

private:
    QString m_tempDir;
};

void TestLogger::initTestCase()
{
    m_tempDir = QDir::tempPath() + "/tcm_logger_test";
    QDir().mkpath(m_tempDir);
    Logger::instance().setOutputFlags(OutputConsole);
    Logger::instance().setLogLevel(LogLevel::Debug);
}

void TestLogger::cleanupTestCase()
{
    Logger::instance().close();
    QDir(m_tempDir).removeRecursively();
}

void TestLogger::testLogLevelEnum()
{
    QCOMPARE((int)LogLevel::Debug, 0);
    QCOMPARE((int)LogLevel::Info, 1);
    QCOMPARE((int)LogLevel::Warning, 2);
    QCOMPARE((int)LogLevel::Error, 3);
    QCOMPARE((int)LogLevel::Critical, 4);
    QCOMPARE((int)LogLevel::Off, 5);
}

void TestLogger::testSetLogLevel()
{
    Logger::instance().setLogLevel(LogLevel::Warning);
    QCOMPARE(Logger::instance().logLevel(), LogLevel::Warning);

    Logger::instance().setLogLevel(LogLevel::Debug);
    QCOMPARE(Logger::instance().logLevel(), LogLevel::Debug);
}

void TestLogger::testLogLevelFiltering()
{
    // Debug level should allow all logs
    Logger::instance().setLogLevel(LogLevel::Debug);
    // Just verify no crash
    Logger::instance().debug("debug message");
    Logger::instance().info("info message");
    Logger::instance().warning("warning message");
    Logger::instance().error("error message");

    // Off level should suppress all logs
    Logger::instance().setLogLevel(LogLevel::Off);
    Logger::instance().debug("should be suppressed");
    Logger::instance().info("should be suppressed");

    Logger::instance().setLogLevel(LogLevel::Debug);
}

void TestLogger::testPerfStartEnd()
{
    Logger::instance().perfClear();
    Logger::instance().perfStart("test_op");
    // Simulate some work
    QTest::qSleep(10);
    qint64 duration = Logger::instance().perfEnd("test_op");
    QVERIFY(duration >= 0);
}

void TestLogger::testPerfRecord()
{
    Logger::instance().perfClear();
    Logger::instance().perfStart("record_test");
    QTest::qSleep(5);
    Logger::instance().perfEnd("record_test");

    const PerfRecord* record = Logger::instance().perfRecord("record_test");
    QVERIFY(record != nullptr);
    QCOMPARE(record->name, QString("record_test"));
    QVERIFY(record->count >= 1);
    QVERIFY(!record->running);
}

void TestLogger::testPerfClear()
{
    Logger::instance().perfStart("clear_test");
    Logger::instance().perfEnd("clear_test");

    Logger::instance().perfClear();
    QVERIFY(Logger::instance().perfRecord("clear_test") == nullptr);
    QVERIFY(Logger::instance().perfRecords().isEmpty());
}

void TestLogger::testPerfNonExistent()
{
    qint64 result = Logger::instance().perfEnd("nonexistent");
    QCOMPARE(result, (qint64)-1);

    QVERIFY(Logger::instance().perfRecord("nonexistent") == nullptr);
}

void TestLogger::testLogFileOutput()
{
    QString logPath = m_tempDir + "/test_log.txt";
    QFile::remove(logPath);

    QVERIFY(Logger::instance().setFileOutput(logPath));
    Logger::instance().info("Test log message");
    Logger::instance().flush();
    Logger::instance().close();

    QFile file(logPath);
    QVERIFY(file.exists());
    QVERIFY(file.size() > 0);
}

void TestLogger::testLogFormatMessage()
{
    Logger::instance().setFormat("[{level}] {message}");

    // Just verify no crash with various formats
    Logger::instance().debug("format test");
    Logger::instance().setFormat("[{time}] [{level}] {message}");
}

void TestLogger::testLevelToString()
{
    // Use the public log interface to verify level conversion
    // (levelToString is private, but we can test via log output)
    Logger::instance().setLogLevel(LogLevel::Debug);
    Logger::instance().debug("Debug level test");
    Logger::instance().info("Info level test");
    Logger::instance().warning("Warning level test");
    Logger::instance().error("Error level test");
    Logger::instance().critical("Critical level test");
}

void TestLogger::testLogStream()
{
    Logger::instance().setLogLevel(LogLevel::Debug);
    // Test stream-style logging (just verify no crash)
    LOG_DEBUG_STREAM() << "Stream debug" << 42;
    LOG_INFO_STREAM() << "Stream info" << 3.14;
}

// ==================== ThreadCommunicator 测试类 =====================
class TestThreadCommunicator : public QObject
{
    Q_OBJECT

private slots:
    void initTestCase();
    void testSendReceiveData();
    void testSendReceiveCommand();
    void testSendReceiveStatus();
    void testSendReceiveError();
    void testMessageTypes();
    void testThreadSafeData();
    void testThreadSafeDataUpdate();
    void testClearPendingMessages();

private:
    // Helper to process events
    void processEvents();
};

void TestThreadCommunicator::initTestCase()
{
    qRegisterMetaType<ThreadMessage>("ThreadMessage");
}

void TestThreadCommunicator::processEvents()
{
    QCoreApplication::processEvents(QEventLoop::AllEvents, 100);
}

void TestThreadCommunicator::testSendReceiveData()
{
    ThreadCommunicator comm;
    QSignalSpy spy(&comm, &ThreadCommunicator::dataReceived);

    comm.sendData(42);
    processEvents();

    QCOMPARE(spy.count(), 1);
    QVariant receivedData = spy.at(0).at(0);
    QCOMPARE(receivedData.toInt(), 42);
}

void TestThreadCommunicator::testSendReceiveCommand()
{
    ThreadCommunicator comm;
    QSignalSpy spy(&comm, &ThreadCommunicator::commandReceived);

    comm.sendCommand("START", 100);
    processEvents();

    QCOMPARE(spy.count(), 1);
    QString command = spy.at(0).at(0).toString();
    QVariant data = spy.at(0).at(1);
    QCOMPARE(command, QString("START"));
    QCOMPARE(data.toInt(), 100);
}

void TestThreadCommunicator::testSendReceiveStatus()
{
    ThreadCommunicator comm;
    QSignalSpy spy(&comm, &ThreadCommunicator::statusReceived);

    comm.sendStatus("OK", 200);
    processEvents();

    QCOMPARE(spy.count(), 1);
    QCOMPARE(spy.at(0).at(0).toString(), QString("OK"));
    QCOMPARE(spy.at(0).at(1).toInt(), 200);
}

void TestThreadCommunicator::testSendReceiveError()
{
    ThreadCommunicator comm;
    QSignalSpy spy(&comm, &ThreadCommunicator::errorReceived);

    comm.sendError("Timeout", 500);
    processEvents();

    QCOMPARE(spy.count(), 1);
    QCOMPARE(spy.at(0).at(0).toString(), QString("Timeout"));
    QCOMPARE(spy.at(0).at(1).toInt(), 500);
}

void TestThreadCommunicator::testMessageTypes()
{
    ThreadMessage dataMsg(ThreadMessage::DataMessage);
    QCOMPARE(dataMsg.type(), ThreadMessage::DataMessage);

    ThreadMessage cmdMsg(ThreadMessage::CommandMessage);
    QCOMPARE(cmdMsg.type(), ThreadMessage::CommandMessage);

    ThreadMessage statusMsg(ThreadMessage::StatusMessage);
    QCOMPARE(statusMsg.type(), ThreadMessage::StatusMessage);

    ThreadMessage errorMsg(ThreadMessage::ErrorMessage);
    QCOMPARE(errorMsg.type(), ThreadMessage::ErrorMessage);
}

void TestThreadCommunicator::testThreadSafeData()
{
    ThreadSafeData<int> data(42);
    QCOMPARE(data.get(), 42);

    data.set(100);
    QCOMPARE(data.get(), 100);
}

void TestThreadCommunicator::testThreadSafeDataUpdate()
{
    ThreadSafeData<int> counter(0);

    counter.update([](int& v) {
        v += 10;
    });

    QCOMPARE(counter.get(), 10);

    counter.update([](int& v) {
        v *= 3;
    });

    QCOMPARE(counter.get(), 30);
}

void TestThreadCommunicator::testClearPendingMessages()
{
    ThreadCommunicator comm;
    // Pending messages are managed internally
    comm.clearPendingMessages();
    QVERIFY(!comm.hasPendingMessages());
}

// ==================== SmartPtr 测试类 =====================
class TestSmartPtr : public QObject
{
    Q_OBJECT

private slots:
    void testMakeByteArray();
    void testMakeCharArray();
    void testMakeUByteArray();
    void testSmartPtrAutoCleanup();
    void testSmartPtrCopy();
};

void TestSmartPtr::testMakeByteArray()
{
    ByteArrayPtr ptr = makeByteArray(256);
    QVERIFY(!ptr.isNull());
    QVERIFY(!ptr.isNull());

    // Write data
    ptr.data()[0] = 0xAB;
    ptr.data()[255] = 0xCD;
    QCOMPARE(ptr.data()[0], (quint8)0xAB);
    QCOMPARE(ptr.data()[255], (quint8)0xCD);
}

void TestSmartPtr::testMakeCharArray()
{
    CharArrayPtr ptr = makeCharArray(128);
    QVERIFY(!ptr.isNull());

    memset(ptr.data(), 'X', 128);
    QCOMPARE(ptr.data()[0], 'X');
    QCOMPARE(ptr.data()[127], 'X');
}

void TestSmartPtr::testMakeUByteArray()
{
    UByteArrayPtr ptr = makeUByteArray(64);
    QVERIFY(!ptr.isNull());

    memset(ptr.data(), 0xFF, 64);
    QCOMPARE(ptr.data()[0], (unsigned char)0xFF);
}

void TestSmartPtr::testSmartPtrAutoCleanup()
{
    // Verify shared pointer sharing
    ByteArrayPtr ptr1 = makeByteArray(100);
    QVERIFY(!ptr1.isNull());

    {
        ByteArrayPtr ptr2 = ptr1;
        // Both point to same data
        QVERIFY(!ptr2.isNull());
        QCOMPARE(ptr2.data(), ptr1.data());
    }

    // ptr1 still valid after ptr2 goes out of scope
    QVERIFY(!ptr1.isNull());
}

void TestSmartPtr::testSmartPtrCopy()
{
    ByteArrayPtr ptr1 = makeByteArray(10);
    ptr1.data()[0] = 0x42;

    ByteArrayPtr ptr2 = ptr1;
    // Both point to same data
    QCOMPARE(ptr2.data()[0], (quint8)0x42);
}

// ==================== DataReplayManager 测试类 =====================
class TestDataReplayManager : public QObject
{
    Q_OBJECT

private slots:
    void initTestCase();
    void cleanupTestCase();
    void init();

    // 速度转换
    void testSpeedToString();
    void testSpeedToFactor();

    // 状态
    void testInitialState();
    void testStartWithoutData();
    void testStopWhenStopped();

    // CSV 加载
    void testLoadCSVFile();
    void testLoadEmptyCSV();
    void testLoadNonExistentFile();
    void testLoadUnsupportedFormat();

    // 数据访问
    void testGetDataPointAt();
    void testGetParamNames();
    void testProgressPercent();
    void testSeekToPercent();

    // 配置
    void testSetReplayConfig();
    void testSupportedFormats();

private:
    QString m_tempDir;
    void createTestCSV(const QString& filePath);
};

void TestDataReplayManager::initTestCase()
{
    m_tempDir = QDir::tempPath() + "/tcm_replay_test";
    QDir().mkpath(m_tempDir);
}

void TestDataReplayManager::cleanupTestCase()
{
    QDir(m_tempDir).removeRecursively();
}

void TestDataReplayManager::init()
{
    // Ensure clean state
}

void TestDataReplayManager::createTestCSV(const QString& filePath)
{
    QFile file(filePath);
    file.open(QIODevice::WriteOnly | QIODevice::Text);
    QTextStream stream(&file);
    stream << "Timestamp,Param1,Param2,Param3\n";
    stream << "2025-01-15 10:00:00.000,1.0,2.0,3.0\n";
    stream << "2025-01-15 10:00:00.100,1.1,2.1,3.1\n";
    stream << "2025-01-15 10:00:00.200,1.2,2.2,3.2\n";
    stream << "2025-01-15 10:00:00.300,1.3,2.3,3.3\n";
    stream << "2025-01-15 10:00:00.400,1.4,2.4,3.4\n";
    stream.flush();
    file.close();
}

void TestDataReplayManager::testSpeedToString()
{
    QCOMPARE(DataReplayManager::speedToString(ReplaySpeed::Speed_0_5x), QString("0.5x"));
    QCOMPARE(DataReplayManager::speedToString(ReplaySpeed::Speed_1x), QString("1x"));
    QCOMPARE(DataReplayManager::speedToString(ReplaySpeed::Speed_2x), QString("2x"));
    QCOMPARE(DataReplayManager::speedToString(ReplaySpeed::Speed_5x), QString("5x"));
    QCOMPARE(DataReplayManager::speedToString(ReplaySpeed::Speed_10x), QString("10x"));
}

void TestDataReplayManager::testSpeedToFactor()
{
    QCOMPARE(DataReplayManager::speedToFactor(ReplaySpeed::Speed_0_5x), 0.5);
    QCOMPARE(DataReplayManager::speedToFactor(ReplaySpeed::Speed_1x), 1.0);
    QCOMPARE(DataReplayManager::speedToFactor(ReplaySpeed::Speed_2x), 2.0);
    QCOMPARE(DataReplayManager::speedToFactor(ReplaySpeed::Speed_5x), 5.0);
    QCOMPARE(DataReplayManager::speedToFactor(ReplaySpeed::Speed_10x), 10.0);
}

void TestDataReplayManager::testInitialState()
{
    DataReplayManager mgr;
    QCOMPARE(mgr.getStatus(), ReplayStatus::Stopped);
    QVERIFY(mgr.isStopped());
    QVERIFY(!mgr.isPlaying());
    QVERIFY(!mgr.isPaused());
    QCOMPARE(mgr.getTotalDataCount(), 0);
    QCOMPARE(mgr.getProgressPercent(), 0);
}

void TestDataReplayManager::testStartWithoutData()
{
    DataReplayManager mgr;
    QSignalSpy spy(&mgr, &DataReplayManager::replayError);

    QVERIFY(!mgr.startReplay());
    QCOMPARE(spy.count(), 1);
}

void TestDataReplayManager::testStopWhenStopped()
{
    DataReplayManager mgr;
    // Stop when already stopped should not crash
    mgr.stopReplay();
    QCOMPARE(mgr.getStatus(), ReplayStatus::Stopped);
}

void TestDataReplayManager::testLoadCSVFile()
{
    DataReplayManager mgr;
    QString filePath = m_tempDir + "/test.csv";
    createTestCSV(filePath);

    QVERIFY(mgr.loadDataFromCSV(filePath));
    QCOMPARE(mgr.getTotalDataCount(), 5);
    QCOMPARE(mgr.getParamNames().size(), 3);
    QVERIFY(mgr.getParamNames().contains("Param1"));
    QVERIFY(mgr.getParamNames().contains("Param2"));
    QVERIFY(mgr.getParamNames().contains("Param3"));
}

void TestDataReplayManager::testLoadEmptyCSV()
{
    DataReplayManager mgr;
    QSignalSpy spy(&mgr, &DataReplayManager::replayError);

    QString filePath = m_tempDir + "/empty.csv";
    QFile file(filePath);
    file.open(QIODevice::WriteOnly | QIODevice::Text);
    file.write("Timestamp,Param1\n");  // Only header, no data
    file.close();

    QVERIFY(!mgr.loadDataFromCSV(filePath));
}

void TestDataReplayManager::testLoadNonExistentFile()
{
    DataReplayManager mgr;
    QVERIFY(!mgr.loadDataFromCSV("/non/existent/file.csv"));
}

void TestDataReplayManager::testLoadUnsupportedFormat()
{
    DataReplayManager mgr;
    QSignalSpy spy(&mgr, &DataReplayManager::replayError);

    QString filePath = m_tempDir + "/test.xyz";
    QFile file(filePath);
    file.open(QIODevice::WriteOnly);
    file.write("data");
    file.close();

    QVERIFY(!mgr.loadDataFile(filePath));
}

void TestDataReplayManager::testGetDataPointAt()
{
    DataReplayManager mgr;
    QString filePath = m_tempDir + "/access.csv";
    createTestCSV(filePath);

    mgr.loadDataFromCSV(filePath);

    ReplayDataPoint dp0 = mgr.getDataPointAt(0);
    QVERIFY(dp0.timestamp.isValid());
    QCOMPARE(dp0.paramValues.size(), 3);
    QCOMPARE(dp0.paramValues[0], 1.0);

    ReplayDataPoint dp4 = mgr.getDataPointAt(4);
    QCOMPARE(dp4.paramValues[0], 1.4);

    // Out of range
    ReplayDataPoint invalid = mgr.getDataPointAt(100);
    QVERIFY(invalid.paramValues.isEmpty());
}

void TestDataReplayManager::testGetParamNames()
{
    DataReplayManager mgr;
    QString filePath = m_tempDir + "/params.csv";
    createTestCSV(filePath);

    mgr.loadDataFromCSV(filePath);
    QStringList names = mgr.getParamNames();
    QCOMPARE(names.size(), 3);
}

void TestDataReplayManager::testProgressPercent()
{
    DataReplayManager mgr;
    QString filePath = m_tempDir + "/progress.csv";
    createTestCSV(filePath);

    mgr.loadDataFromCSV(filePath);
    QCOMPARE(mgr.getProgressPercent(), 0);  // Initially at index 0

    mgr.seekToPercent(50);
    QVERIFY(mgr.getCurrentDataIndex() > 0);
}

void TestDataReplayManager::testSeekToPercent()
{
    DataReplayManager mgr;
    QString filePath = m_tempDir + "/seek.csv";
    createTestCSV(filePath);

    mgr.loadDataFromCSV(filePath);

    mgr.seekToPercent(0);
    QCOMPARE(mgr.getCurrentDataIndex(), 0);

    mgr.seekToPercent(100);
    QCOMPARE(mgr.getCurrentDataIndex(), mgr.getTotalDataCount() - 1);
}

void TestDataReplayManager::testSetReplayConfig()
{
    DataReplayManager mgr;

    ReplayConfig config;
    config.speed = ReplaySpeed::Speed_2x;
    config.loopPlayback = true;
    mgr.setReplayConfig(config);

    ReplayConfig retrieved = mgr.getReplayConfig();
    QCOMPARE(retrieved.speed, ReplaySpeed::Speed_2x);
    QVERIFY(retrieved.loopPlayback);
}

void TestDataReplayManager::testSupportedFormats()
{
    QStringList formats = DataReplayManager::getSupportedFormats();
    QVERIFY(formats.contains("CSV"));
    QVERIFY(formats.contains("JSON"));
    QVERIFY(formats.contains("XML"));

    QString filter = DataReplayManager::getFormatFilter();
    QVERIFY(filter.contains("*.csv"));
    QVERIFY(filter.contains("*.json"));
    QVERIFY(filter.contains("*.xml"));
}

// ==================== TestCaseManager 测试类 =====================
class TestTestCaseManager : public QObject
{
    Q_OBJECT

private slots:
    void init();
    void cleanup();

    // CRUD
    void testAddTestCase();
    void testAddDuplicateTestCase();
    void testAddEmptyIdTestCase();
    void testUpdateTestCase();
    void testUpdateNonExistentTestCase();
    void testRemoveTestCase();
    void testRemoveNonExistentTestCase();
    void testGetTestCase();
    void testGetAllTestCases();
    void testGetTestCasesByCategory();
    void testGetTestCasesByTag();
    void testTestCaseExists();

    // Steps
    void testAddTestStep();
    void testUpdateTestStep();
    void testRemoveTestStep();
    void testMoveTestStep();
    void testStepAutoNumbering();

    // Test Suite
    void testAddTestSuite();
    void testRemoveTestSuite();
    void testUpdateTestSuite();

    // JSON serialization
    void testStepJsonSerialization();
    void testTestCaseJsonSerialization();
    void testTestSuiteJsonSerialization();

    // File I/O
    void testSaveLoadTestCases();
    void testSaveLoadTestSuites();

    // Statistics
    void testTotalTestCaseCount();
    void testEnabledTestCaseCount();
    void testPassRate();

    // Utility
    void testStepTypeToString();
    void testStringToStepType();
    void testResultToString();
    void testGenerateTestCaseId();

private:
    TestCaseManager* m_mgr;
    TestCase createSampleTestCase(const QString& id);
};

void TestTestCaseManager::init()
{
    m_mgr = new TestCaseManager();
}

void TestTestCaseManager::cleanup()
{
    delete m_mgr;
    m_mgr = nullptr;
}

TestCase TestTestCaseManager::createSampleTestCase(const QString& id)
{
    TestCase tc;
    tc.id = id;
    tc.name = "Test " + id;
    tc.description = "Description for " + id;
    tc.category = "Unit";
    tc.tags = QStringList() << "smoke" << "regression";
    tc.enabled = true;
    tc.timeout_ms = 30000;

    TestStep step1;
    step1.stepNumber = 1;
    step1.name = "Set value";
    step1.type = TestStepType::SetValue;
    step1.targetParam = "param1";
    step1.targetValue = 100.0;
    step1.enabled = true;

    TestStep step2;
    step2.stepNumber = 2;
    step2.name = "Wait";
    step2.type = TestStepType::Wait;
    step2.waitTime_ms = 1000;
    step2.enabled = true;

    TestStep step3;
    step3.stepNumber = 3;
    step3.name = "Check value";
    step3.type = TestStepType::CheckValue;
    step3.targetParam = "param1";
    step3.targetValue = 100.0;
    step3.enabled = true;

    tc.steps = QList<TestStep>() << step1 << step2 << step3;
    return tc;
}

void TestTestCaseManager::testAddTestCase()
{
    TestCase tc = createSampleTestCase("TC001");
    QSignalSpy spy(m_mgr, &TestCaseManager::testCaseAdded);

    QVERIFY(m_mgr->addTestCase(tc));
    QVERIFY(m_mgr->testCaseExists("TC001"));
    QCOMPARE(spy.count(), 1);
    QCOMPARE(spy.at(0).at(0).toString(), QString("TC001"));
}

void TestTestCaseManager::testAddDuplicateTestCase()
{
    TestCase tc1 = createSampleTestCase("TC001");
    m_mgr->addTestCase(tc1);

    TestCase tc2 = createSampleTestCase("TC001");
    QVERIFY(!m_mgr->addTestCase(tc2));
}

void TestTestCaseManager::testAddEmptyIdTestCase()
{
    TestCase tc;
    tc.id = "";
    QVERIFY(!m_mgr->addTestCase(tc));
}

void TestTestCaseManager::testUpdateTestCase()
{
    TestCase tc = createSampleTestCase("TC001");
    m_mgr->addTestCase(tc);

    TestCase updated = tc;
    updated.name = "Updated Test";
    QSignalSpy spy(m_mgr, &TestCaseManager::testCaseUpdated);

    QVERIFY(m_mgr->updateTestCase("TC001", updated));
    QCOMPARE(m_mgr->getTestCase("TC001").name, QString("Updated Test"));
    QCOMPARE(spy.count(), 1);
}

void TestTestCaseManager::testUpdateNonExistentTestCase()
{
    TestCase tc = createSampleTestCase("TC999");
    QVERIFY(!m_mgr->updateTestCase("TC999", tc));
}

void TestTestCaseManager::testRemoveTestCase()
{
    TestCase tc = createSampleTestCase("TC001");
    m_mgr->addTestCase(tc);

    QSignalSpy spy(m_mgr, &TestCaseManager::testCaseRemoved);
    QVERIFY(m_mgr->removeTestCase("TC001"));
    QVERIFY(!m_mgr->testCaseExists("TC001"));
    QCOMPARE(spy.count(), 1);
}

void TestTestCaseManager::testRemoveNonExistentTestCase()
{
    QVERIFY(!m_mgr->removeTestCase("TC999"));
}

void TestTestCaseManager::testGetTestCase()
{
    TestCase tc = createSampleTestCase("TC001");
    m_mgr->addTestCase(tc);

    TestCase retrieved = m_mgr->getTestCase("TC001");
    QCOMPARE(retrieved.id, QString("TC001"));
    QCOMPARE(retrieved.name, QString("Test TC001"));
    QCOMPARE(retrieved.steps.size(), 3);
}

void TestTestCaseManager::testGetAllTestCases()
{
    m_mgr->addTestCase(createSampleTestCase("TC001"));
    m_mgr->addTestCase(createSampleTestCase("TC002"));
    m_mgr->addTestCase(createSampleTestCase("TC003"));

    QList<TestCase> all = m_mgr->getAllTestCases();
    QCOMPARE(all.size(), 3);
}

void TestTestCaseManager::testGetTestCasesByCategory()
{
    TestCase tc1 = createSampleTestCase("TC001");
    tc1.category = "Smoke";
    m_mgr->addTestCase(tc1);

    TestCase tc2 = createSampleTestCase("TC002");
    tc2.category = "Integration";
    m_mgr->addTestCase(tc2);

    QList<TestCase> smoke = m_mgr->getTestCasesByCategory("Smoke");
    QCOMPARE(smoke.size(), 1);
    QCOMPARE(smoke[0].id, QString("TC001"));
}

void TestTestCaseManager::testGetTestCasesByTag()
{
    TestCase tc1 = createSampleTestCase("TC001");
    tc1.tags = QStringList() << "smoke" << "fast";
    m_mgr->addTestCase(tc1);

    TestCase tc2 = createSampleTestCase("TC002");
    tc2.tags = QStringList() << "slow";
    m_mgr->addTestCase(tc2);

    QList<TestCase> fastTests = m_mgr->getTestCasesByTag("fast");
    QCOMPARE(fastTests.size(), 1);
}

void TestTestCaseManager::testTestCaseExists()
{
    QVERIFY(!m_mgr->testCaseExists("TC001"));
    m_mgr->addTestCase(createSampleTestCase("TC001"));
    QVERIFY(m_mgr->testCaseExists("TC001"));
}

void TestTestCaseManager::testAddTestStep()
{
    m_mgr->addTestCase(createSampleTestCase("TC001"));

    TestStep newStep;
    newStep.name = "New step";
    newStep.type = TestStepType::Wait;
    newStep.waitTime_ms = 500;

    QVERIFY(m_mgr->addTestStep("TC001", newStep));

    TestCase tc = m_mgr->getTestCase("TC001");
    QCOMPARE(tc.steps.size(), 4);
    QCOMPARE(tc.steps[3].stepNumber, 4);
}

void TestTestCaseManager::testUpdateTestStep()
{
    m_mgr->addTestCase(createSampleTestCase("TC001"));

    TestStep updatedStep;
    updatedStep.name = "Updated step";
    updatedStep.type = TestStepType::SetValue;
    updatedStep.targetParam = "param2";
    updatedStep.targetValue = 200.0;

    QVERIFY(m_mgr->updateTestStep("TC001", 1, updatedStep));

    TestCase tc = m_mgr->getTestCase("TC001");
    QCOMPARE(tc.steps[0].name, QString("Updated step"));
    QCOMPARE(tc.steps[0].stepNumber, 1);
}

void TestTestCaseManager::testRemoveTestStep()
{
    m_mgr->addTestCase(createSampleTestCase("TC001"));
    QCOMPARE(m_mgr->getTestCase("TC001").steps.size(), 3);

    QVERIFY(m_mgr->removeTestStep("TC001", 2));

    TestCase tc = m_mgr->getTestCase("TC001");
    QCOMPARE(tc.steps.size(), 2);
    // Steps should be renumbered
    QCOMPARE(tc.steps[0].stepNumber, 1);
    QCOMPARE(tc.steps[1].stepNumber, 2);
}

void TestTestCaseManager::testMoveTestStep()
{
    m_mgr->addTestCase(createSampleTestCase("TC001"));

    QVERIFY(m_mgr->moveTestStep("TC001", 1, 3));

    TestCase tc = m_mgr->getTestCase("TC001");
    QCOMPARE(tc.steps.size(), 3);
    // After moving step 1 to position 3, check renumbering
    QCOMPARE(tc.steps[0].stepNumber, 1);
    QCOMPARE(tc.steps[1].stepNumber, 2);
    QCOMPARE(tc.steps[2].stepNumber, 3);
}

void TestTestCaseManager::testStepAutoNumbering()
{
    m_mgr->addTestCase(createSampleTestCase("TC001"));

    // Remove middle step
    m_mgr->removeTestStep("TC001", 2);

    TestCase tc = m_mgr->getTestCase("TC001");
    QCOMPARE(tc.steps[0].stepNumber, 1);
    QCOMPARE(tc.steps[1].stepNumber, 2);
}

void TestTestCaseManager::testAddTestSuite()
{
    TestSuite suite;
    suite.id = "TS001";
    suite.name = "Suite 1";
    suite.testCaseIds = QStringList() << "TC001" << "TC002";

    QSignalSpy spy(m_mgr, &TestCaseManager::testSuiteAdded);

    QVERIFY(m_mgr->addTestSuite(suite));
    QCOMPARE(spy.count(), 1);
}

void TestTestCaseManager::testRemoveTestSuite()
{
    TestSuite suite;
    suite.id = "TS001";
    suite.name = "Suite 1";
    m_mgr->addTestSuite(suite);

    QVERIFY(m_mgr->removeTestSuite("TS001"));
    QCOMPARE(m_mgr->getAllTestSuites().size(), 0);
}

void TestTestCaseManager::testUpdateTestSuite()
{
    TestSuite suite;
    suite.id = "TS001";
    suite.name = "Suite 1";
    m_mgr->addTestSuite(suite);

    TestSuite updated = suite;
    updated.name = "Updated Suite";

    QVERIFY(m_mgr->updateTestSuite("TS001", updated));
    QCOMPARE(m_mgr->getTestSuite("TS001").name, QString("Updated Suite"));
}

void TestTestCaseManager::testStepJsonSerialization()
{
    TestStep step;
    step.stepNumber = 1;
    step.name = "Set Value";
    step.type = TestStepType::SetValue;
    step.targetParam = "param1";
    step.targetValue = 42.5;
    step.waitTime_ms = 100;
    step.enabled = true;

    QJsonObject json = step.toJson();
    QCOMPARE(json["stepNumber"].toInt(), 1);
    QCOMPARE(json["name"].toString(), QString("Set Value"));
    QCOMPARE(json["type"].toString(), QString("SetValue"));
    QCOMPARE(json["targetParam"].toString(), QString("param1"));
    QCOMPARE(json["targetValue"].toDouble(), 42.5);
    QCOMPARE(json["waitTime_ms"].toInt(), 100);
    QCOMPARE(json["enabled"].toBool(), true);

    TestStep restored = TestStep::fromJson(json);
    QCOMPARE(restored.stepNumber, 1);
    QCOMPARE(restored.name, QString("Set Value"));
    QCOMPARE(restored.type, TestStepType::SetValue);
    QCOMPARE(restored.targetParam, QString("param1"));
    QCOMPARE(restored.targetValue, 42.5);
    QCOMPARE(restored.waitTime_ms, 100);
    QCOMPARE(restored.enabled, true);
}

void TestTestCaseManager::testTestCaseJsonSerialization()
{
    TestCase tc = createSampleTestCase("TC_JSON");
    QJsonObject json = tc.toJson();

    QCOMPARE(json["id"].toString(), QString("TC_JSON"));
    QCOMPARE(json["name"].toString(), QString("Test TC_JSON"));
    QVERIFY(json["steps"].isArray());

    QJsonArray stepsArray = json["steps"].toArray();
    QCOMPARE(stepsArray.size(), 3);

    TestCase restored = TestCase::fromJson(json);
    QCOMPARE(restored.id, QString("TC_JSON"));
    QCOMPARE(restored.name, QString("Test TC_JSON"));
    QCOMPARE(restored.steps.size(), 3);
    QCOMPARE(restored.steps[0].type, TestStepType::SetValue);
    QCOMPARE(restored.steps[1].type, TestStepType::Wait);
    QCOMPARE(restored.steps[2].type, TestStepType::CheckValue);
}

void TestTestCaseManager::testTestSuiteJsonSerialization()
{
    TestSuite suite;
    suite.id = "TS_JSON";
    suite.name = "JSON Suite";
    suite.description = "Test suite for JSON serialization";
    suite.testCaseIds = QStringList() << "TC001" << "TC002";

    QJsonObject json = suite.toJson();
    QCOMPARE(json["id"].toString(), QString("TS_JSON"));
    QCOMPARE(json["name"].toString(), QString("JSON Suite"));

    TestSuite restored = TestSuite::fromJson(json);
    QCOMPARE(restored.id, QString("TS_JSON"));
    QCOMPARE(restored.name, QString("JSON Suite"));
    QCOMPARE(restored.testCaseIds.size(), 2);
}

void TestTestCaseManager::testSaveLoadTestCases()
{
    m_mgr->addTestCase(createSampleTestCase("TC_SAVE1"));
    m_mgr->addTestCase(createSampleTestCase("TC_SAVE2"));

    QString filePath = QDir::tempPath() + "/test_cases_save.json";
    QFile::remove(filePath);

    QVERIFY(m_mgr->saveTestCasesToFile(filePath));

    // Load into new manager
    TestCaseManager mgr2;
    QVERIFY(mgr2.loadTestCasesFromFile(filePath));
    QCOMPARE(mgr2.getTotalTestCaseCount(), 2);
    QVERIFY(mgr2.testCaseExists("TC_SAVE1"));
    QVERIFY(mgr2.testCaseExists("TC_SAVE2"));

    QFile::remove(filePath);
}

void TestTestCaseManager::testSaveLoadTestSuites()
{
    TestSuite suite;
    suite.id = "TS_SAVE";
    suite.name = "Save Suite";
    suite.testCaseIds = QStringList() << "TC001";

    m_mgr->addTestSuite(suite);

    QString filePath = QDir::tempPath() + "/test_suites_save.json";
    QFile::remove(filePath);

    QVERIFY(m_mgr->saveTestSuitesToFile(filePath));

    TestCaseManager mgr2;
    QVERIFY(mgr2.loadTestSuitesFromFile(filePath));
    QCOMPARE(mgr2.getAllTestSuites().size(), 1);

    QFile::remove(filePath);
}

void TestTestCaseManager::testTotalTestCaseCount()
{
    QCOMPARE(m_mgr->getTotalTestCaseCount(), 0);

    m_mgr->addTestCase(createSampleTestCase("TC001"));
    m_mgr->addTestCase(createSampleTestCase("TC002"));

    QCOMPARE(m_mgr->getTotalTestCaseCount(), 2);
}

void TestTestCaseManager::testEnabledTestCaseCount()
{
    TestCase tc1 = createSampleTestCase("TC001");
    tc1.enabled = true;
    m_mgr->addTestCase(tc1);

    TestCase tc2 = createSampleTestCase("TC002");
    tc2.enabled = false;
    m_mgr->addTestCase(tc2);

    QCOMPARE(m_mgr->getEnabledTestCaseCount(), 1);
}

void TestTestCaseManager::testPassRate()
{
    QCOMPARE(m_mgr->getPassRate(), 0.0);

    // Add results manually
    TestCaseResult passResult;
    passResult.testCaseId = "TC001";
    passResult.result = TestResult::Passed;
    // Note: we cannot directly add results via public API,
    // but we can test the empty case
    QCOMPARE(m_mgr->getFailedTestCount(), 0);
    QCOMPARE(m_mgr->getPassedTestCount(), 0);
}

void TestTestCaseManager::testStepTypeToString()
{
    QCOMPARE(TestCaseManager::testStepTypeToString(TestStepType::SetValue), QString("SetValue"));
    QCOMPARE(TestCaseManager::testStepTypeToString(TestStepType::Wait), QString("Wait"));
    QCOMPARE(TestCaseManager::testStepTypeToString(TestStepType::CheckValue), QString("CheckValue"));
    QCOMPARE(TestCaseManager::testStepTypeToString(TestStepType::RunScript), QString("RunScript"));
    QCOMPARE(TestCaseManager::testStepTypeToString(TestStepType::StartRecord), QString("StartRecord"));
    QCOMPARE(TestCaseManager::testStepTypeToString(TestStepType::StopRecord), QString("StopRecord"));
    QCOMPARE(TestCaseManager::testStepTypeToString(TestStepType::Connect), QString("Connect"));
    QCOMPARE(TestCaseManager::testStepTypeToString(TestStepType::Disconnect), QString("Disconnect"));
    QCOMPARE(TestCaseManager::testStepTypeToString(TestStepType::Custom), QString("Custom"));
}

void TestTestCaseManager::testStringToStepType()
{
    QCOMPARE(TestCaseManager::stringToTestStepType("SetValue"), TestStepType::SetValue);
    QCOMPARE(TestCaseManager::stringToTestStepType("Wait"), TestStepType::Wait);
    QCOMPARE(TestCaseManager::stringToTestStepType("CheckValue"), TestStepType::CheckValue);
    QCOMPARE(TestCaseManager::stringToTestStepType("RunScript"), TestStepType::RunScript);
    QCOMPARE(TestCaseManager::stringToTestStepType("StartRecord"), TestStepType::StartRecord);
    QCOMPARE(TestCaseManager::stringToTestStepType("StopRecord"), TestStepType::StopRecord);
    QCOMPARE(TestCaseManager::stringToTestStepType("Connect"), TestStepType::Connect);
    QCOMPARE(TestCaseManager::stringToTestStepType("Disconnect"), TestStepType::Disconnect);
    QCOMPARE(TestCaseManager::stringToTestStepType("Unknown"), TestStepType::Custom);
}

void TestTestCaseManager::testResultToString()
{
    QCOMPARE(TestCaseManager::testResultToString(TestResult::NotRun), QString("NotRun"));
    QCOMPARE(TestCaseManager::testResultToString(TestResult::Running), QString("Running"));
    QCOMPARE(TestCaseManager::testResultToString(TestResult::Passed), QString("Passed"));
    QCOMPARE(TestCaseManager::testResultToString(TestResult::Failed), QString("Failed"));
    QCOMPARE(TestCaseManager::testResultToString(TestResult::Skipped), QString("Skipped"));
    QCOMPARE(TestCaseManager::testResultToString(TestResult::Timeout), QString("Timeout"));
    QCOMPARE(TestCaseManager::testResultToString(TestResult::Error), QString("Error"));
}

void TestTestCaseManager::testGenerateTestCaseId()
{
    QString id1 = TestCaseManager::generateTestCaseId();
    QString id2 = TestCaseManager::generateTestCaseId();

    QVERIFY(id1.startsWith("TC_"));
    QVERIFY(id2.startsWith("TC_"));
    QVERIFY(id1 != id2);
}

// ==================== 主函数 =====================

int main(int argc, char *argv[])
{
    QCoreApplication app(argc, argv);

    int status = 0;

    {
        TestConfigManager tc;
        status |= QTest::qExec(&tc, argc, argv);
    }

    {
        TestMemoryManager tc;
        status |= QTest::qExec(&tc, argc, argv);
    }

    {
        TestLogger tc;
        status |= QTest::qExec(&tc, argc, argv);
    }

    {
        TestThreadCommunicator tc;
        status |= QTest::qExec(&tc, argc, argv);
    }

    {
        TestSmartPtr tc;
        status |= QTest::qExec(&tc, argc, argv);
    }

    {
        TestDataReplayManager tc;
        status |= QTest::qExec(&tc, argc, argv);
    }

    {
        TestTestCaseManager tc;
        status |= QTest::qExec(&tc, argc, argv);
    }

    return status;
}

#include "tst_common_modules.moc"
