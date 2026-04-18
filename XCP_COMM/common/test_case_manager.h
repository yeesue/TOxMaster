#ifndef TEST_CASE_MANAGER_H
#define TEST_CASE_MANAGER_H

#include <QObject>
#include <QString>
#include <QStringList>
#include <QList>
#include <QDateTime>
#include <QJsonObject>
#include <QJsonArray>
#include <QMap>

// 测试步骤类型枚举
enum class TestStepType {
    SetValue,       // 设置参数值
    Wait,           // 等待
    CheckValue,     // 检查参数值
    RunScript,      // 运行脚本
    StartRecord,    // 开始记录
    StopRecord,     // 停止记录
    Connect,        // 连接设备
    Disconnect,     // 断开设备
    Custom          // 自定义操作
};

// 测试步骤结构
struct TestStep {
    int stepNumber = 0;
    QString name;
    TestStepType type = TestStepType::Custom;
    QString targetParam;           // 目标参数
    double targetValue = 0.0;      // 目标值
    QString condition;             // 条件表达式
    int waitTime_ms = 0;           // 等待时间（毫秒）
    QString scriptCode;            // 脚本代码
    QString description;           // 步骤描述
    bool enabled = true;           // 是否启用
    
    // 转换为JSON
    QJsonObject toJson() const;
    static TestStep fromJson(const QJsonObject& json);
};

// 测试用例结构
struct TestCase {
    QString id;
    QString name;
    QString description;
    QString category;              // 分类
    QStringList tags;              // 标签
    QList<TestStep> steps;         // 测试步骤
    QDateTime createdTime;
    QDateTime modifiedTime;
    bool enabled = true;           // 是否启用
    int timeout_ms = 60000;        // 超时时间（毫秒）
    
    // 转换为JSON
    QJsonObject toJson() const;
    static TestCase fromJson(const QJsonObject& json);
};

// 测试结果枚举
enum class TestResult {
    NotRun,
    Running,
    Passed,
    Failed,
    Skipped,
    Timeout,
    Error
};

// 测试步骤结果结构
struct TestStepResult {
    int stepNumber = 0;
    QString stepName;
    TestResult result = TestResult::NotRun;
    QString message;
    qint64 executionTime_ms = 0;
    QDateTime startTime;
    QDateTime endTime;
    double actualValue = 0.0;      // 实际值（用于检查步骤）
    
    QJsonObject toJson() const;
    static TestStepResult fromJson(const QJsonObject& json);
};

// 测试用例结果结构
struct TestCaseResult {
    QString testCaseId;
    QString testCaseName;
    TestResult result = TestResult::NotRun;
    QString message;
    QList<TestStepResult> stepResults;
    qint64 totalExecutionTime_ms = 0;
    QDateTime startTime;
    QDateTime endTime;
    
    QJsonObject toJson() const;
    static TestCaseResult fromJson(const QJsonObject& json);
};

// 测试套件结构
struct TestSuite {
    QString id;
    QString name;
    QString description;
    QStringList testCaseIds;       // 包含的测试用例ID
    QDateTime createdTime;
    QDateTime modifiedTime;
    
    QJsonObject toJson() const;
    static TestSuite fromJson(const QJsonObject& json);
};

class TestCaseManager : public QObject
{
    Q_OBJECT
public:
    explicit TestCaseManager(QObject *parent = nullptr);
    ~TestCaseManager();

    // 测试用例管理
    bool addTestCase(const TestCase& testCase);
    bool updateTestCase(const QString& id, const TestCase& testCase);
    bool removeTestCase(const QString& id);
    TestCase getTestCase(const QString& id) const;
    QList<TestCase> getAllTestCases() const;
    QList<TestCase> getTestCasesByCategory(const QString& category) const;
    QList<TestCase> getTestCasesByTag(const QString& tag) const;
    bool testCaseExists(const QString& id) const;

    // 测试步骤管理
    bool addTestStep(const QString& testCaseId, const TestStep& step);
    bool updateTestStep(const QString& testCaseId, int stepNumber, const TestStep& step);
    bool removeTestStep(const QString& testCaseId, int stepNumber);
    bool moveTestStep(const QString& testCaseId, int fromIndex, int toIndex);

    // 测试套件管理
    bool addTestSuite(const TestSuite& suite);
    bool updateTestSuite(const QString& id, const TestSuite& suite);
    bool removeTestSuite(const QString& id);
    TestSuite getTestSuite(const QString& id) const;
    QList<TestSuite> getAllTestSuites() const;

    // 文件操作
    bool saveTestCasesToFile(const QString& filePath);
    bool loadTestCasesFromFile(const QString& filePath);
    bool saveTestSuitesToFile(const QString& filePath);
    bool loadTestSuitesFromFile(const QString& filePath);
    bool exportTestCasesToCSV(const QString& filePath);
    bool importTestCasesFromCSV(const QString& filePath);

    // 测试执行
    bool startTestCase(const QString& testCaseId);
    bool startTestSuite(const QString& suiteId);
    void stopTestExecution();
    void pauseTestExecution();
    void resumeTestExecution();

    // 结果管理
    QList<TestCaseResult> getTestResults() const;
    TestCaseResult getTestResult(const QString& testCaseId) const;
    bool saveTestResultsToFile(const QString& filePath);
    bool loadTestResultsFromFile(const QString& filePath);
    void clearTestResults();

    // 统计信息
    int getTotalTestCaseCount() const;
    int getEnabledTestCaseCount() const;
    int getPassedTestCount() const;
    int getFailedTestCount() const;
    double getPassRate() const;

    // 工具方法
    static QString testStepTypeToString(TestStepType type);
    static TestStepType stringToTestStepType(const QString& str);
    static QString testResultToString(TestResult result);
    static QString generateTestCaseId();
    static QString generateTestSuiteId();

signals:
    void testCaseAdded(const QString& id);
    void testCaseUpdated(const QString& id);
    void testCaseRemoved(const QString& id);
    void testSuiteAdded(const QString& id);
    void testSuiteUpdated(const QString& id);
    void testSuiteRemoved(const QString& id);
    
    void testExecutionStarted(const QString& testCaseId);
    void testExecutionFinished(const QString& testCaseId, TestResult result);
    void testStepStarted(const QString& testCaseId, int stepNumber);
    void testStepFinished(const QString& testCaseId, int stepNumber, TestResult result);
    void testExecutionError(const QString& errorMessage);
    void testProgressUpdated(int current, int total);

private:
    QMap<QString, TestCase> m_testCases;
    QMap<QString, TestSuite> m_testSuites;
    QList<TestCaseResult> m_testResults;
    
    bool m_isExecuting = false;
    bool m_isPaused = false;
    QString m_currentTestCaseId;
    int m_currentStepIndex = 0;
};

#endif // TEST_CASE_MANAGER_H
