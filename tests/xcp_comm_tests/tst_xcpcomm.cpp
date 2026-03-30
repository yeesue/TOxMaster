/**
 * @file tst_xcpcomm.cpp
 * @brief XCP Communication 模块单元测试（合并版�? * 
 * 测试范围�? * - XCP 协议基础命令构�? * - CAN 帧数据解�? * - Mock CAN 驱动测试
 * - 错误处理机制测试
 * - CAN驱动抽象接口测试
 */

#include <QtTest/QtTest>
#include <QByteArray>
#include "mock_can_driver.h"
#include "error.h"
#include "can_types.h"
#include "can_driver.h"

using namespace tcm;
using namespace can;

// ==================== XCP 命令码定�?====================
namespace XcpCommand {
    const quint8 CONNECT      = 0xFF;
    const quint8 DISCONNECT   = 0xFE;
    const quint8 GET_STATUS   = 0xFD;
    const quint8 SYNCH        = 0xFC;
    const quint8 GET_COMM_MODE_INFO = 0xFB;
    const quint8 GET_ID       = 0xFA;
    const quint8 SET_REQUEST  = 0xF9;
    const quint8 GET_SEED     = 0xF8;
    const quint8 UNLOCK       = 0xF7;
    const quint8 SET_MTA      = 0xF6;
    const quint8 UPLOAD       = 0xF5;
    const quint8 DOWNLOAD     = 0xF4;
    const quint8 DOWNLOAD_NEXT = 0xF3;
    const quint8 DOWNLOAD_MAX  = 0xF2;
    const quint8 SHORT_UPLOAD  = 0xF1;
    const quint8 SET_CAL_PAGE  = 0xEB;
    const quint8 GET_CAL_PAGE  = 0xEA;
}

// ==================== XCP 协议测试�?====================
class TestXcpComm : public QObject
{
    Q_OBJECT

private slots:
    // XCP 协议基础测试
    void testXcpCommandCodes();
    void testConnectCommand();
    void testDisconnectCommand();
    void testGetStatusCommand();
    void testSynchCommand();
    
    // 数据解析测试
    void testParseResponse();
    void testByteOrder();
    
    // 工具函数测试
    void testChecksum();
    
    // Mock CAN 驱动测试
    void testMockCanDriverOpenClose();
    void testMockCanDriverSendReceive();
    void testMockCanDriverBaudRate();
    void testMockCanDriverFilter();
};

void TestXcpComm::testXcpCommandCodes()
{
    QCOMPARE(XcpCommand::CONNECT, (quint8)0xFF);
    QCOMPARE(XcpCommand::DISCONNECT, (quint8)0xFE);
    QCOMPARE(XcpCommand::GET_STATUS, (quint8)0xFD);
    QCOMPARE(XcpCommand::SYNCH, (quint8)0xFC);
    QCOMPARE(XcpCommand::UPLOAD, (quint8)0xF5);
    QCOMPARE(XcpCommand::DOWNLOAD, (quint8)0xF4);
}

void TestXcpComm::testConnectCommand()
{
    QByteArray cmd;
    cmd.append((char)XcpCommand::CONNECT);
    cmd.append((char)0x00);
    
    QCOMPARE(cmd.size(), 2);
    QCOMPARE((quint8)cmd[0], XcpCommand::CONNECT);
    QCOMPARE((quint8)cmd[1], (quint8)0x00);
}

void TestXcpComm::testDisconnectCommand()
{
    QByteArray cmd;
    cmd.append((char)XcpCommand::DISCONNECT);
    
    QCOMPARE(cmd.size(), 1);
    QCOMPARE((quint8)cmd[0], XcpCommand::DISCONNECT);
}

void TestXcpComm::testGetStatusCommand()
{
    QByteArray cmd;
    cmd.append((char)XcpCommand::GET_STATUS);
    
    QCOMPARE(cmd.size(), 1);
    QCOMPARE((quint8)cmd[0], XcpCommand::GET_STATUS);
}

void TestXcpComm::testSynchCommand()
{
    QByteArray cmd;
    cmd.append((char)XcpCommand::SYNCH);
    
    QCOMPARE(cmd.size(), 1);
    QCOMPARE((quint8)cmd[0], XcpCommand::SYNCH);
}

void TestXcpComm::testParseResponse()
{
    QByteArray response;
    response.append((char)0xFF);
    response.append((char)0x01);
    response.append((char)0x02);
    
    QVERIFY(response.size() >= 1);
    QCOMPARE((quint8)response[0], (quint8)0xFF);
    
    QByteArray errorResponse;
    errorResponse.append((char)0xFE);
    errorResponse.append((char)0x20);
    
    QCOMPARE((quint8)errorResponse[0], (quint8)0xFE);
}

void TestXcpComm::testByteOrder()
{
    quint32 address = 0x12345678;
    
    QByteArray bigEndian;
    bigEndian.append((char)((address >> 24) & 0xFF));
    bigEndian.append((char)((address >> 16) & 0xFF));
    bigEndian.append((char)((address >> 8) & 0xFF));
    bigEndian.append((char)(address & 0xFF));
    
    QCOMPARE((quint8)bigEndian[0], (quint8)0x12);
    QCOMPARE((quint8)bigEndian[1], (quint8)0x34);
    QCOMPARE((quint8)bigEndian[2], (quint8)0x56);
    QCOMPARE((quint8)bigEndian[3], (quint8)0x78);
    
    quint32 reconstructed = 
        ((quint8)bigEndian[0] << 24) |
        ((quint8)bigEndian[1] << 16) |
        ((quint8)bigEndian[2] << 8) |
        ((quint8)bigEndian[3]);
    
    QCOMPARE(reconstructed, address);
}

void TestXcpComm::testChecksum()
{
    QByteArray data;
    data.append((char)0x01);
    data.append((char)0x02);
    data.append((char)0x03);
    data.append((char)0x04);
    
    quint8 sum = 0;
    for (int i = 0; i < data.size(); ++i) {
        sum += (quint8)data[i];
    }
    
    QCOMPARE(sum, (quint8)0x0A);
}

void TestXcpComm::testMockCanDriverOpenClose()
{
    MockCanDriver driver;
    
    QVERIFY(!driver.isOpen());
    QVERIFY(driver.open());
    QVERIFY(driver.isOpen());
    driver.close();
    QVERIFY(!driver.isOpen());
    QVERIFY(driver.open());
    QVERIFY(driver.isOpen());
}

void TestXcpComm::testMockCanDriverSendReceive()
{
    MockCanDriver driver;
    driver.open();
    
    CanFrame rxFrame;
    rxFrame.id = 0x123;
    rxFrame.data = QByteArray("\x01\x02\x03\x04", 4);
    driver.enqueueReceiveFrame(rxFrame);
    
    CanFrame received;
    QVERIFY(driver.receive(received));
    QCOMPARE(received.id, (quint32)0x123);
    QCOMPARE(received.data.size(), 4);
    
    CanFrame empty;
    QVERIFY(!driver.receive(empty));
    
    CanFrame txFrame;
    txFrame.id = 0x456;
    txFrame.data = QByteArray("\xAA\xBB", 2);
    QVERIFY(driver.send(txFrame));
    
    CanFrame sent = driver.getLastSentFrame();
    QCOMPARE(sent.id, (quint32)0x456);
    QCOMPARE(sent.data.size(), 2);
}

void TestXcpComm::testMockCanDriverBaudRate()
{
    MockCanDriver driver;
    
    QVERIFY(driver.setBaudRate(250000));
    QCOMPARE(driver.getBaudRate(), (quint32)250000);
    
    QVERIFY(driver.setBaudRate(500000));
    QCOMPARE(driver.getBaudRate(), (quint32)500000);
    
    QVERIFY(driver.setBaudRate(1000000));
    QCOMPARE(driver.getBaudRate(), (quint32)1000000);
}

void TestXcpComm::testMockCanDriverFilter()
{
    MockCanDriver driver;
    
    QVERIFY(driver.setFilter(0x100, 0x7FF));
    
    driver.setLastError("Test error");
    QCOMPARE(driver.lastError(), QString("Test error"));
    
    driver.setLastError("");
    QVERIFY(driver.lastError().isEmpty());
}

// ==================== 错误处理测试�?====================
class TestError : public QObject
{
    Q_OBJECT

private slots:
    void testErrorCreation();
    void testErrorStaticMethods();
    void testErrorToString();
    void testResultOk();
    void testResultError();
    void testResultValueOr();
    void testResultBoolConversion();
    void testResultVoidOk();
    void testResultVoidError();
    void testDivideFunction();
    void testFileOperation();
};

void TestError::testErrorCreation()
{
    Error e1;
    QVERIFY(e1.isSuccess());
    QVERIFY(!e1.isFailed());
    QCOMPARE(e1.code(), ErrorCode::Success);
    
    Error e2(ErrorCode::FileNotFound, "test.txt");
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
        QVERIFY(true);
    } else {
        QFAIL("Should not reach here");
    }
    
    if (!r2) {
        QVERIFY(true);
    } else {
        QFAIL("Should not reach here");
    }
}

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

// 辅助函数
Result<double> safeDivide(double a, double b) {
    if (b == 0) {
        return Result<double>::error(ErrorCode::InvalidParameter, "Division by zero");
    }
    return Result<double>::ok(a / b);
}

void TestError::testDivideFunction()
{
    auto r1 = safeDivide(10.0, 2.0);
    QVERIFY(r1.isOk());
    QCOMPARE(r1.value(), 5.0);
    
    auto r2 = safeDivide(10.0, 0.0);
    QVERIFY(r2.isErr());
    QCOMPARE(r2.error().code(), ErrorCode::InvalidParameter);
}

ResultVoid openFile(const QString& filename) {
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

// ==================== CAN驱动接口测试�?====================
class TestCanTypes : public QObject
{
    Q_OBJECT

private slots:
    void testCanFrameCreation();
    void testCanFrameDlcConversion();
    void testCanFrameLengthToDlc();
    void testBaudRateConfig();
    void testCanFilter();
    void testCanDeviceInfo();
};

void TestCanTypes::testCanFrameCreation()
{
    CanFrame frame;
    QCOMPARE(frame.id, (quint32)0);
    QVERIFY(frame.data.isEmpty());
    QVERIFY(!frame.extended);
    QVERIFY(!frame.fd);
    QCOMPARE(frame.dlc, (quint8)0);
    
    CanFrame frame2;
    frame2.id = 0x123;
    frame2.data = QByteArray("\x01\x02\x03\x04\x05\x06\x07\x08", 8);
    frame2.extended = true;
    frame2.fd = false;
    frame2.dlc = 8;
    
    QCOMPARE(frame2.id, (quint32)0x123);
    QCOMPARE(frame2.data.size(), 8);
    QVERIFY(frame2.extended);
    QVERIFY(!frame2.fd);
}

void TestCanTypes::testCanFrameDlcConversion()
{
    // CAN 2.0 DLC转换
    QCOMPARE(CanFrame::dlcToLength(0, false), (quint8)0);
    QCOMPARE(CanFrame::dlcToLength(1, false), (quint8)1);
    QCOMPARE(CanFrame::dlcToLength(8, false), (quint8)8);
    QCOMPARE(CanFrame::dlcToLength(15, false), (quint8)8);  // CAN 2.0最�?字节
    
    // CAN FD DLC转换
    QCOMPARE(CanFrame::dlcToLength(0, true), (quint8)0);
    QCOMPARE(CanFrame::dlcToLength(8, true), (quint8)8);
    QCOMPARE(CanFrame::dlcToLength(9, true), (quint8)12);
    QCOMPARE(CanFrame::dlcToLength(10, true), (quint8)16);
    QCOMPARE(CanFrame::dlcToLength(15, true), (quint8)64);
}

void TestCanTypes::testCanFrameLengthToDlc()
{
    // CAN 2.0
    QCOMPARE(CanFrame::lengthToDlc(0, false), (quint8)0);
    QCOMPARE(CanFrame::lengthToDlc(8, false), (quint8)8);
    QCOMPARE(CanFrame::lengthToDlc(20, false), (quint8)8);  // 最�?
    
    // CAN FD
    QCOMPARE(CanFrame::lengthToDlc(8, true), (quint8)8);
    QCOMPARE(CanFrame::lengthToDlc(12, true), (quint8)9);
    QCOMPARE(CanFrame::lengthToDlc(16, true), (quint8)10);
    QCOMPARE(CanFrame::lengthToDlc(64, true), (quint8)15);
}

void TestCanTypes::testBaudRateConfig()
{
    BaudRateConfig config;
    QCOMPARE(config.arbitrationBaud, (quint32)500000);
    QCOMPARE(config.dataBaud, (quint32)2000000);
    QVERIFY(!config.isCanFd);
    QVERIFY(!config.isNonIso);
    
    BaudRateConfig config2;
    config2.arbitrationBaud = 1000000;
    config2.dataBaud = 5000000;
    config2.isCanFd = true;
    config2.isNonIso = true;
    
    QCOMPARE(config2.arbitrationBaud, (quint32)1000000);
    QCOMPARE(config2.dataBaud, (quint32)5000000);
    QVERIFY(config2.isCanFd);
    QVERIFY(config2.isNonIso);
}

void TestCanTypes::testCanFilter()
{
    CanFilter filter(0x100, 0x7FF, false);
    QCOMPARE(filter.id, (quint32)0x100);
    QCOMPARE(filter.mask, (quint32)0x7FF);
    QVERIFY(!filter.extended);
    
    CanFilter extFilter(0x1FFFFFFF, 0x1FFFFFFF, true);
    QVERIFY(extFilter.extended);
}

void TestCanTypes::testCanDeviceInfo()
{
    CanDeviceInfo info;
    QVERIFY(info.vendor.isEmpty());
    QVERIFY(info.model.isEmpty());
    QCOMPARE(info.channelCount, (quint32)0);
    
    info.vendor = "NI";
    info.model = "XNET";
    info.channelCount = 2;
    
    QCOMPARE(info.vendor, QString("NI"));
    QCOMPARE(info.model, QString("XNET"));
    QCOMPARE(info.channelCount, (quint32)2);
}

// ==================== CAN驱动工厂测试�?====================
class TestCanDriverFactory : public QObject
{
    Q_OBJECT

private slots:
    void testDriverTypeName();
    void testDriverTypeEnum();
};

void TestCanDriverFactory::testDriverTypeName()
{
    QCOMPARE(CanDriverFactory::driverTypeName(CanDriverFactory::DriverType::NI_XNET), 
             QString("NI-XNET"));
    QCOMPARE(CanDriverFactory::driverTypeName(CanDriverFactory::DriverType::TS_CAN), 
             QString("TSCAN"));
    QCOMPARE(CanDriverFactory::driverTypeName(CanDriverFactory::DriverType::ZLG_CAN), 
             QString("ZLG"));
}

void TestCanDriverFactory::testDriverTypeEnum()
{
    // 验证枚举�?    CanDriverFactory::DriverType niType = CanDriverFactory::DriverType::NI_XNET;
    CanDriverFactory::DriverType tsType = CanDriverFactory::DriverType::TS_CAN;
    CanDriverFactory::DriverType zlgType = CanDriverFactory::DriverType::ZLG_CAN;
    
    QVERIFY(niType != tsType);
    QVERIFY(tsType != zlgType);
    QVERIFY(niType != zlgType);
}

// ==================== 主函�?====================

// 运行所有测试类的辅助宏
int main(int argc, char *argv[])
{
    int status = 0;
    
    {
        TestXcpComm tc;
        status |= QTest::qExec(&tc, argc, argv);
    }
    
    {
        TestError tc;
        status |= QTest::qExec(&tc, argc, argv);
    }
    
    {
        TestCanTypes tc;
        status |= QTest::qExec(&tc, argc, argv);
    }
    
    {
        TestCanDriverFactory tc;
        status |= QTest::qExec(&tc, argc, argv);
    }
    
    return status;
}

#include "tst_xcpcomm.moc"
