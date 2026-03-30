/**
 * @file ratfunc_converter.h
 * @brief RAT_FUNC converter - safe rational function conversion
 * 
 * RAT_FUNC is a rational function conversion method defined by ASAM MCD-2MC standard:
 * PHYS = (a*x^2 + b*x + c) / (d*x^2 + e*x + f)
 * 
 * Where:
 * - x: integer value (ECU internal representation)
 * - PHYS: physical value (user-visible engineering units)
 * - a,b,c,d,e,f: 6 coefficients
 * 
 * This class provides:
 * - Safe division-by-zero detection
 * - Complete integer <-> physical value bidirectional conversion
 * - Coefficient validation
 * 
 * @note Fixes the division-by-zero crash issue in original RatFuncConv function
 */

#ifndef RATFUNC_CONVERTER_H
#define RATFUNC_CONVERTER_H

#include <QList>
#include <QHash>
#include <QString>
#include <QtGlobal>
#include <cmath>
#include <limits>

/**
 * @struct RatFuncResult
 * @brief Conversion result structure (C++11 compatible, replacing std::optional)
 */
struct RatFuncResult
{
    bool success;     ///< Whether conversion succeeded
    qreal value;      ///< Conversion result value
    
    RatFuncResult() : success(false), value(qQNaN()) {}
    RatFuncResult(qreal v) : success(true), value(v) {}
    
    static RatFuncResult failure() { return RatFuncResult(); }
    static RatFuncResult ok(qreal v) { return RatFuncResult(v); }
    
    bool hasValue() const { return success; }
    qreal getValue() const { return value; }
    qreal getValueOr(qreal defaultValue) const { return success ? value : defaultValue; }
};

/**
 * @class RatFuncConverter
 * @brief Safe RAT_FUNC converter
 */
class RatFuncConverter
{
public:
    /**
     * @brief Default constructor
     */
    RatFuncConverter() = default;
    
    /**
     * @brief Construct from coefficient list
     * @param coeffs Coefficient list [a, b, c, d, e, f]
     */
    explicit RatFuncConverter(const QList<qreal>& coeffs);
    
    /**
     * @brief Construct from coefficient values
     * @param a,b,c Numerator coefficients
     * @param d,e,f Denominator coefficients
     */
    RatFuncConverter(qreal a, qreal b, qreal c, qreal d, qreal e, qreal f);
    
    /**
     * @brief Check if converter is valid
     * @return true if coefficients are valid and usable
     */
    bool isValid() const { return m_valid; }
    
    /**
     * @brief Get validity error message
     * @return Error description, empty when valid
     */
    QString errorMessage() const { return m_errorMessage; }
    
    /**
     * @brief Convert integer value to physical value
     * @param intValue ECU internal integer value
     * @return RatFuncResult, success=false when conversion fails
     * 
     * Formula: PHYS = (a*x^2 + b*x + c) / (d*x^2 + e*x + f)
     */
    RatFuncResult toPhys(qreal intValue) const;
    
    /**
     * @brief Convert physical value to integer value
     * @param physValue Physical value (engineering units)
     * @return RatFuncResult, success=false when conversion fails
     */
    RatFuncResult toInt(qreal physValue) const;
    
    /**
     * @brief Batch convert integer values to physical values
     * @param intValues Integer value list
     * @return Physical value list, failed values represented by NaN
     */
    QList<qreal> toPhysBatch(const QList<qreal>& intValues) const;
    
    /**
     * @brief Batch convert physical values to integer values
     * @param physValues Physical value list
     * @return Integer value list, failed values represented by NaN
     */
    QList<qreal> toIntBatch(const QList<qreal>& physValues) const;
    
    /**
     * @brief Get coefficient list
     */
    QList<qreal> coefficients() const;
    
    /**
     * @brief Check if linear conversion (a=d=0)
     * @return true for linear conversion
     */
    bool isLinear() const;
    
    /**
     * @brief Get conversion type description
     */
    QString conversionType() const;
    
    // Coefficient accessors
    qreal a() const { return m_a; }
    qreal b() const { return m_b; }
    qreal c() const { return m_c; }
    qreal d() const { return m_d; }
    qreal e() const { return m_e; }
    qreal f() const { return m_f; }
    
    /**
     * @brief Set zero detection threshold
     * @param epsilon Threshold value (default 1e-10)
     */
    static void setEpsilon(qreal epsilon) { s_epsilon = epsilon; }
    static qreal epsilon() { return s_epsilon; }
    
private:
    /**
     * @brief Validate coefficient validity
     */
    bool validateCoefficients(const QList<qreal>& coeffs);
    
    /**
     * @brief Check if value is near zero
     */
    static bool isNearZero(qreal value);
    
    /**
     * @brief Select correct root (for toInt)
     */
    static qreal selectRoot(qreal x1, qreal x2);
    
private:
    qreal m_a = 0, m_b = 0, m_c = 0;  ///< Numerator coefficients
    qreal m_d = 0, m_e = 0, m_f = 0;  ///< Denominator coefficients
    bool m_valid = false;              ///< Validity flag
    QString m_errorMessage;            ///< Error message
    
    static qreal s_epsilon;            ///< Zero detection threshold
};

/**
 * @class RatFuncConverterCache
 * @brief Converter cache to avoid repeated creation
 */
class RatFuncConverterCache
{
public:
    /**
     * @brief Get singleton instance
     */
    static RatFuncConverterCache& instance();
    
    /**
     * @brief Get or create converter
     * @param coeffs Coefficient list
     * @return Converter pointer (cache managed, caller should not delete)
     */
    const RatFuncConverter* get(const QList<qreal>& coeffs);
    
    /**
     * @brief Clear cache
     */
    void clear();
    
    /**
     * @brief Get cache size
     */
    int size() const { return m_cache.size(); }
    
private:
    RatFuncConverterCache() = default;
    
    QString makeKey(const QList<qreal>& coeffs) const;
    
    QHash<QString, RatFuncConverter> m_cache;
};

#endif // RATFUNC_CONVERTER_H
