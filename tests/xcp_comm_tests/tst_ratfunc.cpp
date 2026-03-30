/**
 * @file tst_ratfunc.cpp
 * @brief RatFuncConverter单元测试
 * 
 * 测试覆盖�? * 1. 正常转换（整�?->物理值）
 * 2. 除零情况处理
 * 3. 系数不足情况
 * 4. 边界值测�? * 5. 批量转换
 */

#include <QtTest>
#include <QCoreApplication>
#include "ratfunc_converter.h"

class TestRatFuncConverter : public QObject
{
    Q_OBJECT

private slots:
    void initTestCase();
    void cleanupTestCase();
    
    // 基本功能测试
    void testValidCoefficients();
    void testInvalidCoefficients();
    
    // 转换测试
    void testToIntNormal();
    void testToPhysNormal();
    void testLinearConversion();
    
    // 异常情况测试
    void testDivisionByZero();
    void testInsufficientCoefficients();
    void testNaNCoefficients();
    void testInfCoefficients();
    
    // 边界值测�?    void testBoundaryValues();
    void testExtremeValues();
    
    // 批量转换测试
    void testBatchConversion();
    
    // 缓存测试
    void testConverterCache();

private:
    // 辅助函数
    bool compareDouble(qreal a, qreal b, qreal epsilon = 1e-6);
};

void TestRatFuncConverter::initTestCase()
{
    qDebug() << "RatFuncConverter Test Suite Started";
}

void TestRatFuncConverter::cleanupTestCase()
{
    qDebug() << "RatFuncConverter Test Suite Completed";
}

void TestRatFuncConverter::testValidCoefficients()
{
    // 标准线性转换系�? PHYS = x (系数全为0,1)
    QList<qreal> coeffs;
    coeffs << 0 << 1 << 0 << 0 << 0 << 1;  // a=0, b=1, c=0, d=0, e=0, f=1
    RatFuncConverter converter(coeffs);
    
    QVERIFY(converter.isValid());
    QVERIFY(converter.errorMessage().isEmpty());
    QVERIFY(converter.isLinear());
}

void TestRatFuncConverter::testInvalidCoefficients()
{
    // 全零系数 - 无效
    QList<qreal> coeffs;
    coeffs << 0 << 0 << 0 << 0 << 0 << 0;
    RatFuncConverter converter(coeffs);
    
    QVERIFY(!converter.isValid());
    QVERIFY(!converter.errorMessage().isEmpty());
}

void TestRatFuncConverter::testToIntNormal()
{
    // 线性转�? PHYS = x (恒等转换)
    // �?PHYS = (a*x² + b*x + c) / (d*x² + e*x + f)
    // 恒等转换: a=0, b=1, c=0, d=0, e=0, f=1
    // �?PHYS = x / 1 = x
    QList<qreal> coeffs;
    coeffs << 0 << 1 << 0 << 0 << 0 << 1;
    RatFuncConverter converter(coeffs);
    
    QVERIFY(converter.isValid());
    
    // 测试几个�?    RatFuncResult result1 = converter.toInt(0.0);
    QVERIFY(result1.hasValue());
    QVERIFY(compareDouble(result1.getValue(), 0.0));
    
    RatFuncResult result2 = converter.toInt(100.0);
    QVERIFY(result2.hasValue());
    QVERIFY(compareDouble(result2.getValue(), 100.0));
    
    RatFuncResult result3 = converter.toInt(-50.0);
    QVERIFY(result3.hasValue());
    QVERIFY(compareDouble(result3.getValue(), -50.0));
}

void TestRatFuncConverter::testToPhysNormal()
{
    // 线性转�? PHYS = x
    QList<qreal> coeffs;
    coeffs << 0 << 1 << 0 << 0 << 0 << 1;
    RatFuncConverter converter(coeffs);
    
    QVERIFY(converter.isValid());
    
    RatFuncResult result1 = converter.toPhys(0.0);
    QVERIFY(result1.hasValue());
    QVERIFY(compareDouble(result1.getValue(), 0.0));
    
    RatFuncResult result2 = converter.toPhys(255.0);
    QVERIFY(result2.hasValue());
    QVERIFY(compareDouble(result2.getValue(), 255.0));
}

void TestRatFuncConverter::testLinearConversion()
{
    // 线性转�? PHYS = 2*x + 10
    // 需要配置系�? (a*x² + b*x + c) / (d*x² + e*x + f) = 2x + 10
    // �?a=0, b=2, c=10, d=0, e=0, f=1
    QList<qreal> coeffs;
    coeffs << 0 << 2 << 10 << 0 << 0 << 1;
    RatFuncConverter converter(coeffs);
    
    QVERIFY(converter.isValid());
    QVERIFY(converter.isLinear());
    QCOMPARE(converter.conversionType(), QString("LINEAR_POLYNOMIAL"));
    
    // 测试: x=0 -> PHYS=10
    RatFuncResult phys1 = converter.toPhys(0);
    QVERIFY(phys1.hasValue());
    QVERIFY(compareDouble(phys1.getValue(), 10.0));
    
    // 测试: x=10 -> PHYS=30
    RatFuncResult phys2 = converter.toPhys(10);
    QVERIFY(phys2.hasValue());
    QVERIFY(compareDouble(phys2.getValue(), 30.0));
    
    // 反向测试: PHYS=30 -> x=10
    RatFuncResult int1 = converter.toInt(30);
    QVERIFY(int1.hasValue());
    QVERIFY(compareDouble(int1.getValue(), 10.0));
}

void TestRatFuncConverter::testDivisionByZero()
{
    // 设置会导致除零的系数
    // 分母 = d*x² + e*x + f
    // 如果 d=e=0, f=0，则分母始终�?
    QList<qreal> coeffs;
    coeffs << 1 << 1 << 1 << 0 << 0 << 0;  // 分母�?
    RatFuncConverter converter(coeffs);
    
    // 这种情况下，转换应该失败
    RatFuncResult result = converter.toPhys(5.0);
    
    // 由于分母恒为0，转换应该失�?    QVERIFY(!result.hasValue());
}

void TestRatFuncConverter::testInsufficientCoefficients()
{
    // 系数不足
    QList<qreal> coeffs;
    coeffs << 1 << 2 << 3;  // 只有3个系�?    RatFuncConverter converter(coeffs);
    
    QVERIFY(!converter.isValid());
    QVERIFY(converter.errorMessage().contains("Insufficient"));
    
    // 转换应该失败
    RatFuncResult result = converter.toPhys(5.0);
    QVERIFY(!result.hasValue());
}

void TestRatFuncConverter::testNaNCoefficients()
{
    QList<qreal> coeffs;
    coeffs << 0 << 1 << qQNaN() << 0 << 0 << 1;
    RatFuncConverter converter(coeffs);
    
    QVERIFY(!converter.isValid());
    QVERIFY(converter.errorMessage().contains("Invalid coefficient"));
}

void TestRatFuncConverter::testInfCoefficients()
{
    QList<qreal> coeffs;
    coeffs << 0 << qInf() << 0 << 0 << 0 << 1;
    RatFuncConverter converter(coeffs);
    
    QVERIFY(!converter.isValid());
}

void TestRatFuncConverter::testBoundaryValues()
{
    // 恒等转换
    QList<qreal> coeffs;
    coeffs << 0 << 1 << 0 << 0 << 0 << 1;
    RatFuncConverter converter(coeffs);
    
    // 边界�?    RatFuncResult result1 = converter.toPhys(0.0);
    QVERIFY(result1.hasValue());
    
    RatFuncResult result2 = converter.toPhys(255.0);  // uint8最大�?    QVERIFY(result2.hasValue());
    
    RatFuncResult result3 = converter.toPhys(65535.0);  // uint16最大�?    QVERIFY(result3.hasValue());
}

void TestRatFuncConverter::testExtremeValues()
{
    QList<qreal> coeffs;
    coeffs << 0 << 1 << 0 << 0 << 0 << 1;
    RatFuncConverter converter(coeffs);
    
    // 非常大的�?    RatFuncResult result1 = converter.toPhys(1e10);
    QVERIFY(result1.hasValue());
    
    // 非常小的�?    RatFuncResult result2 = converter.toPhys(1e-10);
    QVERIFY(result2.hasValue());
}

void TestRatFuncConverter::testBatchConversion()
{
    QList<qreal> coeffs;
    coeffs << 0 << 2 << 0 << 0 << 0 << 1;  // PHYS = 2*x
    RatFuncConverter converter(coeffs);
    
    QList<qreal> intValues;
    intValues << 0 << 1 << 2 << 3 << 4 << 5;
    QList<qreal> physValues = converter.toPhysBatch(intValues);
    
    QCOMPARE(physValues.size(), intValues.size());
    
    for (int i = 0; i < intValues.size(); i++) {
        QVERIFY(!qIsNaN(physValues[i]));
        QVERIFY(compareDouble(physValues[i], intValues[i] * 2));
    }
}

void TestRatFuncConverter::testConverterCache()
{
    QList<qreal> coeffs1;
    coeffs1 << 0 << 1 << 0 << 0 << 0 << 1;
    QList<qreal> coeffs2;
    coeffs2 << 0 << 2 << 0 << 0 << 0 << 1;
    
    RatFuncConverterCache& cache = RatFuncConverterCache::instance();
    
    // 首次获取
    const RatFuncConverter* conv1 = cache.get(coeffs1);
    QVERIFY(conv1 != nullptr);
    QVERIFY(conv1->isValid());
    
    // 再次获取相同系数，应该返回同一对象
    const RatFuncConverter* conv1_again = cache.get(coeffs1);
    QCOMPARE(conv1, conv1_again);
    
    // 获取不同系数
    const RatFuncConverter* conv2 = cache.get(coeffs2);
    QVERIFY(conv2 != nullptr);
    QVERIFY(conv2 != conv1);
    
    // 清空缓存
    cache.clear();
    QCOMPARE(cache.size(), 0);
}

bool TestRatFuncConverter::compareDouble(qreal a, qreal b, qreal epsilon)
{
    if (qIsNaN(a) || qIsNaN(b)) {
        return qIsNaN(a) && qIsNaN(b);
    }
    return qAbs(a - b) < epsilon;
}

QTEST_APPLESS_MAIN(TestRatFuncConverter)

#include "tst_ratfunc.moc"
