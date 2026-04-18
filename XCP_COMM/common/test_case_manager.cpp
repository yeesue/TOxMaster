#include "test_case_manager.h"
#include <QJsonDocument>
#include <QFile>
#include <QTextStream>
#include <QDebug>
#include <QUuid>

// TestStep implementation
QJsonObject TestStep::toJson() const
{
    QJsonObject json;
    json["stepNumber"] = stepNumber;
    json["name"] = name;
    json["type"] = TestCaseManager::testStepTypeToString(type);
    json["targetParam"] = targetParam;
    json["targetValue"] = targetValue;
    json["condition"] = condition;
    json["waitTime_ms"] = waitTime_ms;
    json["scriptCode"] = scriptCode;
    json["description"] = description;
    json["enabled"] = enabled;
    return json;
}

TestStep TestStep::fromJson(const QJsonObject& json)
{
    TestStep step;
    step.stepNumber = json["stepNumber"].toInt();
    step.name = json["name"].toString();
    step.type = TestCaseManager::stringToTestStepType(json["type"].toString());
    step.targetParam = json["targetParam"].toString();
    step.targetValue = json["targetValue"].toDouble();
    step.condition = json["condition"].toString();
    step.waitTime_ms = json["waitTime_ms"].toInt();
    step.scriptCode = json["scriptCode"].toString();
    step.description = json["description"].toString();
    step.enabled = json["enabled"].toBool(true);
    return step;
}

// TestCase implementation
QJsonObject TestCase::toJson() const
{
    QJsonObject json;
    json["id"] = id;
    json["name"] = name;
    json["description"] = description;
    json["category"] = category;
    json["tags"] = QJsonArray::fromStringList(tags);
    json["createdTime"] = createdTime.toString(Qt::ISODate);
    json["modifiedTime"] = modifiedTime.toString(Qt::ISODate);
    json["enabled"] = enabled;
    json["timeout_ms"] = timeout_ms;
    
    QJsonArray stepsArray;
    for (const auto& step : steps) {
        stepsArray.append(step.toJson());
    }
    json["steps"] = stepsArray;
    
    return json;
}

TestCase TestCase::fromJson(const QJsonObject& json)
{
    TestCase testCase;
    testCase.id = json["id"].toString();
    testCase.name = json["name"].toString();
    testCase.description = json["description"].toString();
    testCase.category = json["category"].toString();
    
    QJsonArray tagsArray = json["tags"].toArray();
    for (const auto& tag : tagsArray) {
        testCase.tags.append(tag.toString());
    }
    
    testCase.createdTime = QDateTime::fromString(json["createdTime"].toString(), Qt::ISODate);
    testCase.modifiedTime = QDateTime::fromString(json["modifiedTime"].toString(), Qt::ISODate);
    testCase.enabled = json["enabled"].toBool(true);
    testCase.timeout_ms = json["timeout_ms"].toInt(60000);
    
    QJsonArray stepsArray = json["steps"].toArray();
    for (const auto& stepValue : stepsArray) {
        testCase.steps.append(TestStep::fromJson(stepValue.toObject()));
    }
    
    return testCase;
}

// TestStepResult implementation
QJsonObject TestStepResult::toJson() const
{
    QJsonObject json;
    json["stepNumber"] = stepNumber;
    json["stepName"] = stepName;
    json["result"] = TestCaseManager::testResultToString(result);
    json["message"] = message;
    json["executionTime_ms"] = executionTime_ms;
    json["startTime"] = startTime.toString(Qt::ISODate);
    json["endTime"] = endTime.toString(Qt::ISODate);
    json["actualValue"] = actualValue;
    return json;
}

TestStepResult TestStepResult::fromJson(const QJsonObject& json)
{
    TestStepResult result;
    result.stepNumber = json["stepNumber"].toInt();
    result.stepName = json["stepName"].toString();
    // Note: testResultFromString would need to be implemented
    result.message = json["message"].toString();
    result.executionTime_ms = json["executionTime_ms"].toInt();
    result.startTime = QDateTime::fromString(json["startTime"].toString(), Qt::ISODate);
    result.endTime = QDateTime::fromString(json["endTime"].toString(), Qt::ISODate);
    result.actualValue = json["actualValue"].toDouble();
    return result;
}

// TestCaseResult implementation
QJsonObject TestCaseResult::toJson() const
{
    QJsonObject json;
    json["testCaseId"] = testCaseId;
    json["testCaseName"] = testCaseName;
    json["result"] = TestCaseManager::testResultToString(result);
    json["message"] = message;
    json["totalExecutionTime_ms"] = totalExecutionTime_ms;
    json["startTime"] = startTime.toString(Qt::ISODate);
    json["endTime"] = endTime.toString(Qt::ISODate);
    
    QJsonArray stepResultsArray;
    for (const auto& stepResult : stepResults) {
        stepResultsArray.append(stepResult.toJson());
    }
    json["stepResults"] = stepResultsArray;
    
    return json;
}

TestCaseResult TestCaseResult::fromJson(const QJsonObject& json)
{
    TestCaseResult result;
    result.testCaseId = json["testCaseId"].toString();
    result.testCaseName = json["testCaseName"].toString();
    result.message = json["message"].toString();
    result.totalExecutionTime_ms = json["totalExecutionTime_ms"].toInt();
    result.startTime = QDateTime::fromString(json["startTime"].toString(), Qt::ISODate);
    result.endTime = QDateTime::fromString(json["endTime"].toString(), Qt::ISODate);
    
    QJsonArray stepResultsArray = json["stepResults"].toArray();
    for (const auto& stepResultValue : stepResultsArray) {
        result.stepResults.append(TestStepResult::fromJson(stepResultValue.toObject()));
    }
    
    return result;
}

// TestSuite implementation
QJsonObject TestSuite::toJson() const
{
    QJsonObject json;
    json["id"] = id;
    json["name"] = name;
    json["description"] = description;
    json["testCaseIds"] = QJsonArray::fromStringList(testCaseIds);
    json["createdTime"] = createdTime.toString(Qt::ISODate);
    json["modifiedTime"] = modifiedTime.toString(Qt::ISODate);
    return json;
}

TestSuite TestSuite::fromJson(const QJsonObject& json)
{
    TestSuite suite;
    suite.id = json["id"].toString();
    suite.name = json["name"].toString();
    suite.description = json["description"].toString();
    
    QJsonArray testCaseIdsArray = json["testCaseIds"].toArray();
    for (const auto& idValue : testCaseIdsArray) {
        suite.testCaseIds.append(idValue.toString());
    }
    
    suite.createdTime = QDateTime::fromString(json["createdTime"].toString(), Qt::ISODate);
    suite.modifiedTime = QDateTime::fromString(json["modifiedTime"].toString(), Qt::ISODate);
    
    return suite;
}

// TestCaseManager implementation
TestCaseManager::TestCaseManager(QObject *parent) : QObject(parent)
{
}

TestCaseManager::~TestCaseManager()
{
}

bool TestCaseManager::addTestCase(const TestCase& testCase)
{
    if (testCase.id.isEmpty()) {
        emit testExecutionError("Test case ID cannot be empty");
        return false;
    }
    
    if (m_testCases.contains(testCase.id)) {
        emit testExecutionError("Test case with ID " + testCase.id + " already exists");
        return false;
    }
    
    TestCase newTestCase = testCase;
    if (newTestCase.createdTime.isNull()) {
        newTestCase.createdTime = QDateTime::currentDateTime();
    }
    newTestCase.modifiedTime = QDateTime::currentDateTime();
    
    m_testCases.insert(newTestCase.id, newTestCase);
    emit testCaseAdded(newTestCase.id);
    
    return true;
}

bool TestCaseManager::updateTestCase(const QString& id, const TestCase& testCase)
{
    if (!m_testCases.contains(id)) {
        emit testExecutionError("Test case with ID " + id + " not found");
        return false;
    }
    
    TestCase updatedTestCase = testCase;
    updatedTestCase.id = id;
    updatedTestCase.modifiedTime = QDateTime::currentDateTime();
    
    m_testCases.insert(id, updatedTestCase);
    emit testCaseUpdated(id);
    
    return true;
}

bool TestCaseManager::removeTestCase(const QString& id)
{
    if (!m_testCases.contains(id)) {
        emit testExecutionError("Test case with ID " + id + " not found");
        return false;
    }
    
    m_testCases.remove(id);
    emit testCaseRemoved(id);
    
    return true;
}

TestCase TestCaseManager::getTestCase(const QString& id) const
{
    return m_testCases.value(id);
}

QList<TestCase> TestCaseManager::getAllTestCases() const
{
    return m_testCases.values();
}

QList<TestCase> TestCaseManager::getTestCasesByCategory(const QString& category) const
{
    QList<TestCase> result;
    for (const auto& testCase : m_testCases) {
        if (testCase.category == category) {
            result.append(testCase);
        }
    }
    return result;
}

QList<TestCase> TestCaseManager::getTestCasesByTag(const QString& tag) const
{
    QList<TestCase> result;
    for (const auto& testCase : m_testCases) {
        if (testCase.tags.contains(tag)) {
            result.append(testCase);
        }
    }
    return result;
}

bool TestCaseManager::testCaseExists(const QString& id) const
{
    return m_testCases.contains(id);
}

bool TestCaseManager::addTestStep(const QString& testCaseId, const TestStep& step)
{
    if (!m_testCases.contains(testCaseId)) {
        emit testExecutionError("Test case with ID " + testCaseId + " not found");
        return false;
    }
    
    TestCase testCase = m_testCases.value(testCaseId);
    TestStep newStep = step;
    newStep.stepNumber = testCase.steps.count() + 1;
    testCase.steps.append(newStep);
    testCase.modifiedTime = QDateTime::currentDateTime();
    
    m_testCases.insert(testCaseId, testCase);
    emit testCaseUpdated(testCaseId);
    
    return true;
}

bool TestCaseManager::updateTestStep(const QString& testCaseId, int stepNumber, const TestStep& step)
{
    if (!m_testCases.contains(testCaseId)) {
        emit testExecutionError("Test case with ID " + testCaseId + " not found");
        return false;
    }
    
    TestCase testCase = m_testCases.value(testCaseId);
    if (stepNumber < 1 || stepNumber > testCase.steps.count()) {
        emit testExecutionError("Invalid step number");
        return false;
    }
    
    TestStep updatedStep = step;
    updatedStep.stepNumber = stepNumber;
    testCase.steps[stepNumber - 1] = updatedStep;
    testCase.modifiedTime = QDateTime::currentDateTime();
    
    m_testCases.insert(testCaseId, testCase);
    emit testCaseUpdated(testCaseId);
    
    return true;
}

bool TestCaseManager::removeTestStep(const QString& testCaseId, int stepNumber)
{
    if (!m_testCases.contains(testCaseId)) {
        emit testExecutionError("Test case with ID " + testCaseId + " not found");
        return false;
    }
    
    TestCase testCase = m_testCases.value(testCaseId);
    if (stepNumber < 1 || stepNumber > testCase.steps.count()) {
        emit testExecutionError("Invalid step number");
        return false;
    }
    
    testCase.steps.removeAt(stepNumber - 1);
    
    // 重新编号
    for (int i = 0; i < testCase.steps.count(); ++i) {
        testCase.steps[i].stepNumber = i + 1;
    }
    
    testCase.modifiedTime = QDateTime::currentDateTime();
    m_testCases.insert(testCaseId, testCase);
    emit testCaseUpdated(testCaseId);
    
    return true;
}

bool TestCaseManager::moveTestStep(const QString& testCaseId, int fromIndex, int toIndex)
{
    if (!m_testCases.contains(testCaseId)) {
        emit testExecutionError("Test case with ID " + testCaseId + " not found");
        return false;
    }
    
    TestCase testCase = m_testCases.value(testCaseId);
    if (fromIndex < 1 || fromIndex > testCase.steps.count() ||
        toIndex < 1 || toIndex > testCase.steps.count()) {
        emit testExecutionError("Invalid step index");
        return false;
    }
    
    testCase.steps.move(fromIndex - 1, toIndex - 1);
    
    // 重新编号
    for (int i = 0; i < testCase.steps.count(); ++i) {
        testCase.steps[i].stepNumber = i + 1;
    }
    
    testCase.modifiedTime = QDateTime::currentDateTime();
    m_testCases.insert(testCaseId, testCase);
    emit testCaseUpdated(testCaseId);
    
    return true;
}

bool TestCaseManager::addTestSuite(const TestSuite& suite)
{
    if (suite.id.isEmpty()) {
        emit testExecutionError("Test suite ID cannot be empty");
        return false;
    }
    
    if (m_testSuites.contains(suite.id)) {
        emit testExecutionError("Test suite with ID " + suite.id + " already exists");
        return false;
    }
    
    TestSuite newSuite = suite;
    if (newSuite.createdTime.isNull()) {
        newSuite.createdTime = QDateTime::currentDateTime();
    }
    newSuite.modifiedTime = QDateTime::currentDateTime();
    
    m_testSuites.insert(newSuite.id, newSuite);
    emit testSuiteAdded(newSuite.id);
    
    return true;
}

bool TestCaseManager::updateTestSuite(const QString& id, const TestSuite& suite)
{
    if (!m_testSuites.contains(id)) {
        emit testExecutionError("Test suite with ID " + id + " not found");
        return false;
    }
    
    TestSuite updatedSuite = suite;
    updatedSuite.id = id;
    updatedSuite.modifiedTime = QDateTime::currentDateTime();
    
    m_testSuites.insert(id, updatedSuite);
    emit testSuiteUpdated(id);
    
    return true;
}

bool TestCaseManager::removeTestSuite(const QString& id)
{
    if (!m_testSuites.contains(id)) {
        emit testExecutionError("Test suite with ID " + id + " not found");
        return false;
    }
    
    m_testSuites.remove(id);
    emit testSuiteRemoved(id);
    
    return true;
}

TestSuite TestCaseManager::getTestSuite(const QString& id) const
{
    return m_testSuites.value(id);
}

QList<TestSuite> TestCaseManager::getAllTestSuites() const
{
    return m_testSuites.values();
}

bool TestCaseManager::saveTestCasesToFile(const QString& filePath)
{
    QJsonArray testCasesArray;
    for (const auto& testCase : m_testCases) {
        testCasesArray.append(testCase.toJson());
    }
    
    QJsonDocument doc(testCasesArray);
    QFile file(filePath);
    if (!file.open(QIODevice::WriteOnly)) {
        emit testExecutionError("Failed to open file for writing: " + filePath);
        return false;
    }
    
    file.write(doc.toJson(QJsonDocument::Indented));
    file.close();
    
    return true;
}

bool TestCaseManager::loadTestCasesFromFile(const QString& filePath)
{
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly)) {
        emit testExecutionError("Failed to open file for reading: " + filePath);
        return false;
    }
    
    QByteArray data = file.readAll();
    file.close();
    
    QJsonDocument doc = QJsonDocument::fromJson(data);
    if (doc.isNull() || !doc.isArray()) {
        emit testExecutionError("Invalid JSON format in file: " + filePath);
        return false;
    }
    
    QJsonArray testCasesArray = doc.array();
    m_testCases.clear();
    
    for (const auto& value : testCasesArray) {
        TestCase testCase = TestCase::fromJson(value.toObject());
        if (!testCase.id.isEmpty()) {
            m_testCases.insert(testCase.id, testCase);
        }
    }
    
    return true;
}

bool TestCaseManager::saveTestSuitesToFile(const QString& filePath)
{
    QJsonArray testSuitesArray;
    for (const auto& suite : m_testSuites) {
        testSuitesArray.append(suite.toJson());
    }
    
    QJsonDocument doc(testSuitesArray);
    QFile file(filePath);
    if (!file.open(QIODevice::WriteOnly)) {
        emit testExecutionError("Failed to open file for writing: " + filePath);
        return false;
    }
    
    file.write(doc.toJson(QJsonDocument::Indented));
    file.close();
    
    return true;
}

bool TestCaseManager::loadTestSuitesFromFile(const QString& filePath)
{
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly)) {
        emit testExecutionError("Failed to open file for reading: " + filePath);
        return false;
    }
    
    QByteArray data = file.readAll();
    file.close();
    
    QJsonDocument doc = QJsonDocument::fromJson(data);
    if (doc.isNull() || !doc.isArray()) {
        emit testExecutionError("Invalid JSON format in file: " + filePath);
        return false;
    }
    
    QJsonArray testSuitesArray = doc.array();
    m_testSuites.clear();
    
    for (const auto& value : testSuitesArray) {
        TestSuite suite = TestSuite::fromJson(value.toObject());
        if (!suite.id.isEmpty()) {
            m_testSuites.insert(suite.id, suite);
        }
    }
    
    return true;
}

bool TestCaseManager::startTestCase(const QString& testCaseId)
{
    if (!m_testCases.contains(testCaseId)) {
        emit testExecutionError("Test case with ID " + testCaseId + " not found");
        return false;
    }
    
    if (m_isExecuting) {
        emit testExecutionError("Another test is already running");
        return false;
    }
    
    m_isExecuting = true;
    m_isPaused = false;
    m_currentTestCaseId = testCaseId;
    m_currentStepIndex = 0;
    
    emit testExecutionStarted(testCaseId);
    
    return true;
}

bool TestCaseManager::startTestSuite(const QString& suiteId)
{
    if (!m_testSuites.contains(suiteId)) {
        emit testExecutionError("Test suite with ID " + suiteId + " not found");
        return false;
    }
    
    TestSuite suite = m_testSuites.value(suiteId);
    
    // 执行测试套件中的所有测试用例
    for (const QString& testCaseId : suite.testCaseIds) {
        if (!startTestCase(testCaseId)) {
            return false;
        }
    }
    
    return true;
}

void TestCaseManager::stopTestExecution()
{
    m_isExecuting = false;
    m_isPaused = false;
    emit testExecutionFinished(m_currentTestCaseId, TestResult::Skipped);
}

void TestCaseManager::pauseTestExecution()
{
    if (m_isExecuting && !m_isPaused) {
        m_isPaused = true;
    }
}

void TestCaseManager::resumeTestExecution()
{
    if (m_isExecuting && m_isPaused) {
        m_isPaused = false;
    }
}

QList<TestCaseResult> TestCaseManager::getTestResults() const
{
    return m_testResults;
}

TestCaseResult TestCaseManager::getTestResult(const QString& testCaseId) const
{
    for (const auto& result : m_testResults) {
        if (result.testCaseId == testCaseId) {
            return result;
        }
    }
    return TestCaseResult();
}

bool TestCaseManager::saveTestResultsToFile(const QString& filePath)
{
    QJsonArray resultsArray;
    for (const auto& result : m_testResults) {
        resultsArray.append(result.toJson());
    }
    
    QJsonDocument doc(resultsArray);
    QFile file(filePath);
    if (!file.open(QIODevice::WriteOnly)) {
        emit testExecutionError("Failed to open file for writing: " + filePath);
        return false;
    }
    
    file.write(doc.toJson(QJsonDocument::Indented));
    file.close();
    
    return true;
}

bool TestCaseManager::loadTestResultsFromFile(const QString& filePath)
{
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly)) {
        emit testExecutionError("Failed to open file for reading: " + filePath);
        return false;
    }
    
    QByteArray data = file.readAll();
    file.close();
    
    QJsonDocument doc = QJsonDocument::fromJson(data);
    if (doc.isNull() || !doc.isArray()) {
        emit testExecutionError("Invalid JSON format in file: " + filePath);
        return false;
    }
    
    QJsonArray resultsArray = doc.array();
    m_testResults.clear();
    
    for (const auto& value : resultsArray) {
        m_testResults.append(TestCaseResult::fromJson(value.toObject()));
    }
    
    return true;
}

void TestCaseManager::clearTestResults()
{
    m_testResults.clear();
}

int TestCaseManager::getTotalTestCaseCount() const
{
    return m_testCases.count();
}

int TestCaseManager::getEnabledTestCaseCount() const
{
    int count = 0;
    for (const auto& testCase : m_testCases) {
        if (testCase.enabled) {
            count++;
        }
    }
    return count;
}

int TestCaseManager::getPassedTestCount() const
{
    int count = 0;
    for (const auto& result : m_testResults) {
        if (result.result == TestResult::Passed) {
            count++;
        }
    }
    return count;
}

int TestCaseManager::getFailedTestCount() const
{
    int count = 0;
    for (const auto& result : m_testResults) {
        if (result.result == TestResult::Failed) {
            count++;
        }
    }
    return count;
}

double TestCaseManager::getPassRate() const
{
    if (m_testResults.isEmpty()) {
        return 0.0;
    }
    
    int passed = getPassedTestCount();
    return (static_cast<double>(passed) / m_testResults.count()) * 100.0;
}

QString TestCaseManager::testStepTypeToString(TestStepType type)
{
    switch (type) {
    case TestStepType::SetValue: return "SetValue";
    case TestStepType::Wait: return "Wait";
    case TestStepType::CheckValue: return "CheckValue";
    case TestStepType::RunScript: return "RunScript";
    case TestStepType::StartRecord: return "StartRecord";
    case TestStepType::StopRecord: return "StopRecord";
    case TestStepType::Connect: return "Connect";
    case TestStepType::Disconnect: return "Disconnect";
    case TestStepType::Custom: return "Custom";
    default: return "Custom";
    }
}

TestStepType TestCaseManager::stringToTestStepType(const QString& str)
{
    if (str == "SetValue") return TestStepType::SetValue;
    if (str == "Wait") return TestStepType::Wait;
    if (str == "CheckValue") return TestStepType::CheckValue;
    if (str == "RunScript") return TestStepType::RunScript;
    if (str == "StartRecord") return TestStepType::StartRecord;
    if (str == "StopRecord") return TestStepType::StopRecord;
    if (str == "Connect") return TestStepType::Connect;
    if (str == "Disconnect") return TestStepType::Disconnect;
    return TestStepType::Custom;
}

QString TestCaseManager::testResultToString(TestResult result)
{
    switch (result) {
    case TestResult::NotRun: return "NotRun";
    case TestResult::Running: return "Running";
    case TestResult::Passed: return "Passed";
    case TestResult::Failed: return "Failed";
    case TestResult::Skipped: return "Skipped";
    case TestResult::Timeout: return "Timeout";
    case TestResult::Error: return "Error";
    default: return "Unknown";
    }
}

QString TestCaseManager::generateTestCaseId()
{
    QString uuid = QUuid::createUuid().toString();
    uuid.remove('{').remove('}');
    return "TC_" + uuid.left(8);
}

QString TestCaseManager::generateTestSuiteId()
{
    QString uuid = QUuid::createUuid().toString();
    uuid.remove('{').remove('}');
    return "TS_" + uuid.left(8);
}
