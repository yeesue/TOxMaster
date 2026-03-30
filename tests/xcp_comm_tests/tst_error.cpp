/**
 * @file tst_error.cpp
 * @brief 错误处理机制单元测试
 */

#include <QtTest/QtTest>
#include "error.h"

using namespace tcm;

class TestError : public QObject
{
    Q_OBJECT

private slots:
    // Error 类测�?    void testErrorCreation();
    void testErrorStaticMethods();
    void testErrorToString();
    
    // Result<T> 测试
    void testResultOk();
    void testResultError();
    void testResultValueOr();
    void testResultBoolConversion();
    
    // ResultVoid 测试
    void testResultVoidOk();
    void testResultVoidError();
    
    // 实际使用场景测试
    void testDivideFunction();
    void testFileOperation();
};

// ==================== Error 类测�?====================

void TestError::testErrorCreation()
{
    // 默认构�?    Error e1;
    QVERIFY(e1.isSuccess());
    QVERIFY(!e1.isFailed());
    QCOMPARE(e1.code(), ErrorCode::Success);
    
    // 带参数构�?    Error e2(ErrorCode::FileNotFound, "test.txt");
    QVERIFY(e2.isFailed());
    QCOMPARE(e2.code(), ErrorCode::FileNotFound);
    QCOMPARE(e2.message(), QString("test.txt"));
}

void TestError::testErrorStaticMethods()
{
    auto e1 = Error::ok();
    QVERIFY(e1.isSuccess());
    
    auto e2 = Error::fileNotFound("config.ini");
    QCOMPARE(e2.code(), ErrorCode::FileNotFound);
    QVERIFY(e2.message().contains("config.ini"));
    
    auto e3 = Error::timeout();
    QCOMPARE(e3.code(), ErrorCode::Timeout);
    
    auto e4 = Error::invalidParameter("baudRate");
    QCOMPARE(e4.code(), ErrorCode::InvalidParameter);
    QVERIFY(e4.message().contains("baudRate"));
}

void TestError::testErrorToString()
{
    Error e(ErrorCode::ConnectionFailed, "CAN device not found");
    QString str = e.toString();
    
    QVERIFY(str.contains("Error"));
    QVERIFY(str.contains("ConnectionFailed"));
    QVERIFY(str.contains("CAN device not found"));
}

// ==================== Result<T> 测试 ====================

void TestError::testResultOk()
{
    Result<int> result = Result<int>::ok(42);
    
    QVERIFY(result.isOk());
    QVERIFY(!result.isErr());
    QVERIFY(result);
    QCOMPARE(result.value(), 42);
}

void TestError::testResultError()
{
    Result<int> result = Result<int>::error(ErrorCode::Timeout, "Operation timed out");
    
    QVERIFY(result.isErr());
    QVERIFY(!result.isOk());
    QVERIFY(!result);
    QCOMPARE(result.error().code(), ErrorCode::Timeout);
}

void TestError::testResultValueOr()
{
    Result<int> okResult = Result<int>::ok(100);
    Result<int> errResult = Result<int>::error(ErrorCode::ParseError, "Invalid number");
    
    QCOMPARE(okResult.valueOr(0), 100);
    QCOMPARE(errResult.valueOr(0), 0);
}

void TestError::testResultBoolConversion()
{
    Result<QString> r1 = Result<QString>::ok("success");
    Result<QString> r2 = Result<QString>::error(ErrorCode::ParseError, "fail");
    
    if (r1) {
        // 成功路径
        QVERIFY(true);
    } else {
        QFAIL("Should not reach here");
    }
    
    if (!r2) {
        // 错误路径
        QVERIFY(true);
    } else {
        QFAIL("Should not reach here");
    }
}

// ==================== ResultVoid 测试 ====================

void TestError::testResultVoidOk()
{
    ResultVoid result = ResultVoid::ok();
    
    QVERIFY(result.isOk());
    QVERIFY(result);
}

void TestError::testResultVoidError()
{
    ResultVoid result = ResultVoid::error(ErrorCode::PermissionDenied, "Access denied");
    
    QVERIFY(result.isErr());
    QVERIFY(!result);
    QCOMPARE(result.error().code(), ErrorCode::PermissionDenied);
}

// ==================== 实际使用场景测试 ====================

// 辅助函数：安全的除法
Result<double> safeDivide(double a, double b) {
    if (b == 0) {
        return Result<double>::error(ErrorCode::InvalidParameter, "Division by zero");
    }
    return Result<double>::ok(a / b);
}

void TestError::testDivideFunction()
{
    // 正常情况
    auto r1 = safeDivide(10.0, 2.0);
    QVERIFY(r1.isOk());
    QCOMPARE(r1.value(), 5.0);
    
    // 异常情况
    auto r2 = safeDivide(10.0, 0.0);
    QVERIFY(r2.isErr());
    QCOMPARE(r2.error().code(), ErrorCode::InvalidParameter);
}

// 辅助函数：模拟文件操�?ResultVoid openFile(const QString& filename) {
    if (filename.isEmpty()) {
        return ResultVoid::error(ErrorCode::InvalidParameter, "Filename is empty");
    }
    if (!filename.endsWith(".txt")) {
        return ResultVoid::error(ErrorCode::FileNotFound, filename + " is not a txt file");
    }
    return ResultVoid::ok();
}

void TestError::testFileOperation()
{
    auto r1 = openFile("test.txt");
    QVERIFY(r1.isOk());
    
    auto r2 = openFile("");
    QVERIFY(r2.isErr());
    QCOMPARE(r2.error().code(), ErrorCode::InvalidParameter);
    
    auto r3 = openFile("test.doc");
    QVERIFY(r3.isErr());
    QCOMPARE(r3.error().code(), ErrorCode::FileNotFound);
}

QTEST_MAIN(TestError)
#include "tst_error.moc"
