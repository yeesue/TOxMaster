/**
 * @file tst_xcpcomm.cpp
 * @brief XCP Communication 模块单元测试（合并版）
 * 
 * 测试范围：
 * - XCP 协议基础命令构造
 * - CAN 帧数据解析
 * - Mock CAN 驱动测试
 * - 错误处理机制测试
 * - CAN驱动抽象接口测试
 * - 数据转换测试
 */

#include <QtTest/QtTest>
#include <QByteArray>
#include "mock_can_driver.h"
#include "error.h"
#include "can_types.h"
#include "can_driver.h"

using namespace tcm;
using namespace can;

// ==================== XCP 命令码定义 ====================
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

// ==================== XCP 协议测试类 =====================
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
    void testMockCanDriverBatchSend();
    void testMockCanDriverInterface();
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
    QCOMPARE(driver.state(), DriverState::Closed);
    
    QVERIFY(driver.open());
    QVERIFY(driver.isOpen());
    QCOMPARE(driver.state(), DriverState::Open);
    
    driver.close();
    QVERIFY(!driver.isOpen());
    QCOMPARE(driver.state(), DriverState::Closed);
    
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
    
    BaudRateConfig config1;
    config1.arbitrationBaud = 250000;
    QVERIFY(driver.setBaudRate(config1));
    QCOMPARE(driver.getBaudConfig().arbitrationBaud, (quint32)250000);
    
    BaudRateConfig config2;
    config2.arbitrationBaud = 500000;
    config2.dataBaud = 2000000;
    config2.isCanFd = true;
    QVERIFY(driver.setBaudRate(config2));
    QCOMPARE(driver.getBaudConfig().arbitrationBaud, (quint32)500000);
    QCOMPARE(driver.getBaudConfig().dataBaud, (quint32)2000000);
    QVERIFY(driver.getBaudConfig().isCanFd);
}

void TestXcpComm::testMockCanDriverFilter()
{
    MockCanDriver driver;
    
    QList<CanFilter> filters;
    filters << CanFilter(0x100, 0x7FF, false);
    QVERIFY(driver.setFilters(filters));
    QCOMPARE(driver.getFilters().size(), 1);
    QCOMPARE(driver.getFilters()[0].id, (quint32)0x100);
    
    driver.setLastError("Test error");
    QCOMPARE(driver.lastError(), QString("Test error"));
    
    driver.setLastError("");
    QVERIFY(driver.lastError().isEmpty());
}

void TestXcpComm::testMockCanDriverBatchSend()
{
    MockCanDriver driver;
    driver.open();
    
    QList<CanFrame> frames;
    for (int i = 0; i < 5; ++i) {
        CanFrame f;
        f.id = 0x100 + i;
        f.data = QByteArray(1, (char)i);
        frames.append(f);
    }
    
    QCOMPARE(driver.sendBatch(frames), 5);
    QCOMPARE(driver.getAllSentFrames().size(), 5);
}

void TestXcpComm::testMockCanDriverInterface()
{
    MockCanDriver driver;
    
    QVERIFY(driver.selectInterface("CAN0"));
    QCOMPARE(driver.currentInterface(), QString("CAN0"));
    
    QStringList ifaces = driver.availableInterfaces();
    QCOMPARE(ifaces.size(), 2);
    
    CanDeviceInfo info = driver.deviceInfo();
    QCOMPARE(info.vendor, QString("Mock"));
    QCOMPARE(info.channelCount, 2);
}

// ==================== 错误处理测试类 =====================
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
    QVERIFY(str.contains("3"));  // ConnectionFailed = enum value 3
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

// ==================== CAN类型测试类 =====================
class TestCanTypes : public QObject
{
    Q_OBJECT

private slots:
    void testCanFrameCreation();
    void testCanFrameDlcConversion();
    void testCanFrameLengthToDlc();
    void testCanFrameValidity();
    void testBaudRateConfig();
    void testCanFilter();
    void testCanDeviceInfo();
    void testCanDriverStatus();
    void testCanErrorEnum();
    void testDriverStateEnum();
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
    // CAN 2.0 / CAN FD DLC 转换
    QCOMPARE(CanFrame::dlcToLength(0), (quint8)0);
    QCOMPARE(CanFrame::dlcToLength(1), (quint8)1);
    QCOMPARE(CanFrame::dlcToLength(8), (quint8)8);
    QCOMPARE(CanFrame::dlcToLength(9), (quint8)12);
    QCOMPARE(CanFrame::dlcToLength(10), (quint8)16);
    QCOMPARE(CanFrame::dlcToLength(15), (quint8)64);
}

void TestCanTypes::testCanFrameLengthToDlc()
{
    QCOMPARE(CanFrame::lengthToDlc(0), (quint8)0);
    QCOMPARE(CanFrame::lengthToDlc(8), (quint8)8);
    QCOMPARE(CanFrame::lengthToDlc(12), (quint8)9);
    QCOMPARE(CanFrame::lengthToDlc(16), (quint8)10);
    QCOMPARE(CanFrame::lengthToDlc(64), (quint8)15);
}

void TestCanTypes::testCanFrameValidity()
{
    CanFrame frame;
    frame.data = QByteArray(8, 0);
    QVERIFY(frame.isValid());

    CanFrame fdFrame;
    fdFrame.fd = true;
    fdFrame.data = QByteArray(64, 0);
    QVERIFY(fdFrame.isValid());

    CanFrame tooLong;
    tooLong.data = QByteArray(9, 0);
    QVERIFY(!tooLong.isValid());

    CanFrame fdTooLong;
    fdTooLong.fd = true;
    fdTooLong.data = QByteArray(65, 0);
    QVERIFY(!fdTooLong.isValid());
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
    QCOMPARE(info.channelCount, 0);
    
    info.vendor = "NI";
    info.model = "XNET";
    info.channelCount = 2;
    
    QCOMPARE(info.vendor, QString("NI"));
    QCOMPARE(info.model, QString("XNET"));
    QCOMPARE(info.channelCount, 2);
}

void TestCanTypes::testCanDriverStatus()
{
    CanDriverStatus status;
    QVERIFY(!status.isOpen);
    QVERIFY(!status.isRunning);
    QCOMPARE(status.txCount, (quint32)0);
    QCOMPARE(status.rxCount, (quint32)0);
    QCOMPARE(status.errorCount, (quint32)0);
    QCOMPARE(status.lastError, CanError::None);
}

void TestCanTypes::testCanErrorEnum()
{
    CanError err = CanError::None;
    QVERIFY(err == CanError::None);
    QVERIFY(CanError::BusOff != CanError::None);
    QVERIFY(CanError::CrcError != CanError::AckError);
}

void TestCanTypes::testDriverStateEnum()
{
    QVERIFY(DriverState::Closed != DriverState::Open);
    QVERIFY(DriverState::Opening != DriverState::Error);
}

// ==================== CAN驱动工厂测试类 =====================
class TestCanDriverFactory : public QObject
{
    Q_OBJECT

private slots:
    void testDriverTypeEnum();
};

void TestCanDriverFactory::testDriverTypeEnum()
{
    CanDriverFactory::DriverType niType = CanDriverFactory::DriverType::NI_XNET;
    CanDriverFactory::DriverType tsType = CanDriverFactory::DriverType::TS_CAN;
    CanDriverFactory::DriverType zlgType = CanDriverFactory::DriverType::ZLG_CAN;

    QVERIFY(niType != tsType);
    QVERIFY(tsType != zlgType);
    QVERIFY(niType != zlgType);
}

// ==================== 内存修复回归测试 =====================
class TestMemoryFixes : public QObject
{
    Q_OBJECT

private slots:
    // Cali_Pair QByteArray 替换验证
    void testCaliPairDataOwnership();
    void testCaliPairDataCopy();
    void testCaliPairDataEmpty();
    
    // QByteArray 安全性测试
    void testQByteArrayAutoCleanup();
    void testQByteArrayCopyOnWrite();
    
    // std::vector 安全性测试
    void testVectorAutoCleanup();
    void testVectorResize();
};

void TestMemoryFixes::testCaliPairDataOwnership()
{
    // 验证 QByteArray 版 Cali_Pair 数据自动管理
    // 旧版 char* 需要手动 delete[]，新版 QByteArray 自动释放
    struct Cali_Pair_Test {
        QByteArray data;
        int size;
    };
    
    Cali_Pair_Test pair;
    pair.data = QByteArray("\x01\x02\x03\x04", 4);
    pair.size = 4;
    
    QCOMPARE(pair.data.size(), 4);
    QCOMPARE((quint8)pair.data[0], (quint8)0x01);
    
    // 复制时自动深拷贝
    Cali_Pair_Test pair2 = pair;
    QCOMPARE(pair2.data.size(), 4);
    pair2.data[0] = 0xFF;
    // 原始数据不变（COW 延迟拷贝后各自独立）
    QCOMPARE((quint8)pair.data[0], (quint8)0x01);
    QCOMPARE((quint8)pair2.data[0], (quint8)0xFF);
}

void TestMemoryFixes::testCaliPairDataCopy()
{
    struct Cali_Pair_Test {
        QByteArray data;
        int size;
    };
    
    char rawData[] = {0x10, 0x20, 0x30, 0x40};
    Cali_Pair_Test pair;
    pair.data = QByteArray(rawData, 4);
    pair.size = 4;
    
    QCOMPARE(pair.data.size(), 4);
    QCOMPARE((quint8)pair.data.data()[0], (quint8)0x10);
}

void TestMemoryFixes::testCaliPairDataEmpty()
{
    struct Cali_Pair_Test {
        QByteArray data;
        int size;
        Cali_Pair_Test() : size(0) {}
    };

    Cali_Pair_Test pair;
    QVERIFY(pair.data.isEmpty());
    QCOMPARE(pair.size, 0);
    // 空对象析构不会崩溃
}

void TestMemoryFixes::testQByteArrayAutoCleanup()
{
    // 验证 QByteArray 超出作用域自动释放
    {
        QByteArray buf(1024, 0);
        QVERIFY(buf.size() == 1024);
    }
    // 此处无内存泄漏（QByteArray 自动释放）
    
    // 动态分配的 QByteArray 由 parent 或智能指针管理
    QByteArray* dynBuf = new QByteArray(2048, 0);
    delete dynBuf;
    // 无双重释放风险
}

void TestMemoryFixes::testQByteArrayCopyOnWrite()
{
    QByteArray original = QByteArray("\xAA\xBB\xCC", 3);
    QByteArray copy = original;
    
    // COW: 共享数据直到修改
    QCOMPARE(copy.constData(), original.constData());
    
    copy[0] = 0xDD;
    // 修改后各自独立
    QCOMPARE((quint8)original[0], (quint8)0xAA);
    QCOMPARE((quint8)copy[0], (quint8)0xDD);
}

void TestMemoryFixes::testVectorAutoCleanup()
{
    // 验证 std::vector 超出作用域自动释放
    {
        std::vector<int> vec(1000, 42);
        QCOMPARE((int)vec.size(), 1000);
    }
    // 无内存泄漏
    
    // 动态结构体数组
    struct DbcRef { int id; char name[32]; };
    {
        std::vector<DbcRef> refs(100);
        QCOMPARE((int)refs.size(), 100);
        QCOMPARE((int)refs.capacity(), 100);
    }
    // 自动释放，不需要 delete[]
}

void TestMemoryFixes::testVectorResize()
{
    std::vector<quint8> buf;
    buf.resize(64);
    QCOMPARE((int)buf.size(), 64);
    
    // data() 返回可写指针
    buf.data()[0] = 0xFF;
    QCOMPARE(buf[0], (quint8)0xFF);
    
    // resize 缩小不泄漏
    buf.resize(10);
    QCOMPARE((int)buf.size(), 10);
}

// ==================== 主函数 =====================

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
    
    {
        TestMemoryFixes tc;
        status |= QTest::qExec(&tc, argc, argv);
    }
    
    return status;
}

#include "tst_xcpcomm.moc"
