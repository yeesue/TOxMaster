/**
 * @file ratfunc_converter.cpp
 * @brief RAT_FUNC converter implementation
 */

#include "ratfunc_converter.h"
#include "logger.h"
#include <QDebug>
#include <QtMath>

// Static member initialization
qreal RatFuncConverter::s_epsilon = 1e-10;

//=============================================================================
// Constructor functions
//=============================================================================

RatFuncConverter::RatFuncConverter(const QList<qreal>& coeffs)
{
    m_valid = validateCoefficients(coeffs);
}

RatFuncConverter::RatFuncConverter(qreal a, qreal b, qreal c, qreal d, qreal e, qreal f)
    : m_a(a), m_b(b), m_c(c), m_d(d), m_e(e), m_f(f)
{
    // Coefficients already set, check if all zero (invalid)
    if (m_a == 0 && m_b == 0 && m_c == 0 && m_d == 0 && m_e == 0 && m_f == 0) {
        m_valid = false;
        m_errorMessage = "All coefficients are zero";
    } else {
        m_valid = true;
    }
}

//=============================================================================
// Core conversion functions
//=============================================================================

RatFuncResult RatFuncConverter::toPhys(qreal intValue) const
{
    if (!m_valid) {
        LOG_WARN_STREAM() << "RatFuncConverter::toPhys - Converter is invalid:" << m_errorMessage;
        return RatFuncResult::failure();
    }
    
    qreal x = intValue;
    
    // Calculate denominator: d*x^2 + e*x + f
    qreal denominator = m_d * x * x + m_e * x + m_f;
    
    // Division-by-zero detection
    if (isNearZero(denominator)) {
        LOG_WARN_STREAM() << "RatFuncConverter::toPhys - Division by zero detected"
                   << "intValue=" << intValue
                   << "denominator=" << denominator;
        return RatFuncResult::failure();
    }
    
    // Calculate numerator: a*x^2 + b*x + c
    qreal numerator = m_a * x * x + m_b * x + m_c;
    
    qreal result = numerator / denominator;
    
    // Check if result is valid (not NaN, not infinite)
    if (qIsNaN(result) || qIsInf(result)) {
        LOG_WARN_STREAM() << "RatFuncConverter::toPhys - Invalid result"
                   << "intValue=" << intValue
                   << "result=" << result;
        return RatFuncResult::failure();
    }
    
    return RatFuncResult::ok(result);
}

RatFuncResult RatFuncConverter::toInt(qreal physValue) const
{
    if (!m_valid) {
        LOG_WARN_STREAM() << "RatFuncConverter::toInt - Converter is invalid:" << m_errorMessage;
        return RatFuncResult::failure();
    }
    
    qreal p = physValue;
    
    // Reverse conversion requires solving quadratic equation
    // From PHYS = (a*x^2 + b*x + c) / (d*x^2 + e*x + f)
    // We get (d - a*PHYS)*x^2 + (e - b*PHYS)*x + (f - c*PHYS) = 0
    
    qreal A = m_d - m_a * p;
    qreal B = m_e - m_b * p;
    qreal C = m_f - m_c * p;
    
    // Case 1: Quadratic equation (A != 0)
    if (!isNearZero(A)) {
        qreal discriminant = B * B - 4 * A * C;
        
        if (discriminant < 0) {
            LOG_WARN_STREAM() << "RatFuncConverter::toInt - No real solution (discriminant < 0)"
                       << "physValue=" << physValue
                       << "discriminant=" << discriminant;
            return RatFuncResult::failure();
        }
        
        qreal sqrtD = qSqrt(discriminant);
        qreal x1 = (-B + sqrtD) / (2 * A);
        qreal x2 = (-B - sqrtD) / (2 * A);
        
        qreal result = selectRoot(x1, x2);
        
        if (qIsNaN(result) || qIsInf(result)) {
            LOG_WARN_STREAM() << "RatFuncConverter::toInt - Invalid result from quadratic solution"
                       << "physValue=" << physValue;
            return RatFuncResult::failure();
        }
        
        return RatFuncResult::ok(result);
    }
    
    // Case 2: Linear equation (A == 0, B != 0)
    if (!isNearZero(B)) {
        qreal result = -C / B;
        
        if (qIsNaN(result) || qIsInf(result)) {
            LOG_WARN_STREAM() << "RatFuncConverter::toInt - Invalid result from linear solution"
                       << "physValue=" << physValue;
            return RatFuncResult::failure();
        }
        
        return RatFuncResult::ok(result);
    }
    
    // Case 3: No solution or infinite solutions (A == 0, B == 0)
    LOG_WARN_STREAM() << "RatFuncConverter::toInt - No unique solution (A=0, B=0)"
               << "physValue=" << physValue;
    return RatFuncResult::failure();
}

//=============================================================================
// Batch conversion
//=============================================================================

QList<qreal> RatFuncConverter::toPhysBatch(const QList<qreal>& intValues) const
{
    QList<qreal> results;
    results.reserve(intValues.size());
    
    for (int i = 0; i < intValues.size(); i++) {
        RatFuncResult result = toPhys(intValues[i]);
        results.append(result.hasValue() ? result.getValue() : qQNaN());
    }
    
    return results;
}

QList<qreal> RatFuncConverter::toIntBatch(const QList<qreal>& physValues) const
{
    QList<qreal> results;
    results.reserve(physValues.size());
    
    for (int i = 0; i < physValues.size(); i++) {
        RatFuncResult result = toInt(physValues[i]);
        results.append(result.hasValue() ? result.getValue() : qQNaN());
    }
    
    return results;
}

//=============================================================================
// Helper functions
//=============================================================================

QList<qreal> RatFuncConverter::coefficients() const
{
    QList<qreal> coeffs;
    coeffs << m_a << m_b << m_c << m_d << m_e << m_f;
    return coeffs;
}

bool RatFuncConverter::isLinear() const
{
    return isNearZero(m_a) && isNearZero(m_d);
}

QString RatFuncConverter::conversionType() const
{
    if (!m_valid) {
        return "INVALID";
    }
    
    if (isLinear()) {
        // Linear conversion: (b*x + c) / (e*x + f)
        if (isNearZero(m_e)) {
            return "LINEAR_POLYNOMIAL";  // Pure polynomial b*x + c
        }
        return "LINEAR_RATIONAL";  // Linear rational function
    }
    
    return "QUADRATIC_RATIONAL";  // Quadratic rational function
}

//=============================================================================
// Private functions
//=============================================================================

bool RatFuncConverter::validateCoefficients(const QList<qreal>& coeffs)
{
    if (coeffs.size() < 6) {
        m_errorMessage = QString("Insufficient coefficients: got %1, need 6")
                         .arg(coeffs.size());
        return false;
    }
    
    // Check for invalid coefficients (NaN or Inf)
    for (int i = 0; i < 6; i++) {
        if (qIsNaN(coeffs[i]) || qIsInf(coeffs[i])) {
            m_errorMessage = QString("Invalid coefficient at index %1: %2")
                             .arg(i).arg(coeffs[i]);
            return false;
        }
    }
    
    m_a = coeffs[0];
    m_b = coeffs[1];
    m_c = coeffs[2];
    m_d = coeffs[3];
    m_e = coeffs[4];
    m_f = coeffs[5];
    
    // Check if all zero
    if (m_a == 0 && m_b == 0 && m_c == 0 && m_d == 0 && m_e == 0 && m_f == 0) {
        m_errorMessage = "All coefficients are zero";
        return false;
    }
    
    return true;
}

bool RatFuncConverter::isNearZero(qreal value)
{
    return qAbs(value) < s_epsilon;
}

qreal RatFuncConverter::selectRoot(qreal x1, qreal x2)
{
    // Rules:
    // 1. Prefer non-negative root
    // 2. Both negative, choose smaller absolute value
    // 3. Both non-negative, choose smaller value
    
    bool x1Valid = !qIsNaN(x1) && !qIsInf(x1);
    bool x2Valid = !qIsNaN(x2) && !qIsInf(x2);
    
    if (!x1Valid && !x2Valid) {
        return qQNaN();
    }
    
    if (!x1Valid) return x2;
    if (!x2Valid) return x1;
    
    // Both are valid values
    bool x1NonNegative = x1 >= 0;
    bool x2NonNegative = x2 >= 0;
    
    if (x1NonNegative && !x2NonNegative) return x1;
    if (!x1NonNegative && x2NonNegative) return x2;
    
    // Same sign, choose smaller absolute value (closer to typical value)
    return qAbs(x1) <= qAbs(x2) ? x1 : x2;
}

//=============================================================================
// Converter cache implementation
//=============================================================================

RatFuncConverterCache& RatFuncConverterCache::instance()
{
    static RatFuncConverterCache instance;
    return instance;
}

const RatFuncConverter* RatFuncConverterCache::get(const QList<qreal>& coeffs)
{
    QString key = makeKey(coeffs);
    
    if (!m_cache.contains(key)) {
        m_cache.insert(key, RatFuncConverter(coeffs));
    }
    
    return &m_cache[key];
}

void RatFuncConverterCache::clear()
{
    m_cache.clear();
}

QString RatFuncConverterCache::makeKey(const QList<qreal>& coeffs) const
{
    if (coeffs.size() < 6) {
        return "invalid";
    }
    
    // Use string as key, precision preserved to 6 decimal places
    return QString("%1,%2,%3,%4,%5,%6")
           .arg(coeffs[0], 0, 'g', 6)
           .arg(coeffs[1], 0, 'g', 6)
           .arg(coeffs[2], 0, 'g', 6)
           .arg(coeffs[3], 0, 'g', 6)
           .arg(coeffs[4], 0, 'g', 6)
           .arg(coeffs[5], 0, 'g', 6);
}
