/**
 * @file error.h
 * @brief 统一错误处理机制
 *
 * 提供 Result<T> 类型用于函数返回值，支持成功/错误状态
 * 兼容 C++11，不依赖 std::variant（需要 C++17）
 */

#ifndef ERROR_H
#define ERROR_H

#include <QString>
#include <QPair>

namespace tcm {

/**
 * @brief 错误码枚举
 */
enum class ErrorCode {
    Success = 0,
    FileNotFound,
    ParseError,
    ConnectionFailed,
    Timeout,
    InvalidParameter,
    HardwareError,
    OutOfRange,
    MemoryError,
    NotInitialized,
    AlreadyExists,
    PermissionDenied
};

/**
 * @brief 错误信息类
 */
class Error
{
public:
    Error() : m_code(ErrorCode::Success) {}
    Error(ErrorCode code, const QString& message)
        : m_code(code), m_message(message) {}

    ErrorCode code() const { return m_code; }
    QString message() const { return m_message; }

    bool isSuccess() const { return m_code == ErrorCode::Success; }
    bool isFailed() const { return m_code != ErrorCode::Success; }

    QString toString() const {
        if (isSuccess()) return "Success";
        return QString("Error[%1]: %2").arg(static_cast<int>(m_code)).arg(m_message);
    }

    static Error ok() { return Error(); }
    static Error fileNotFound(const QString& file) {
        return Error(ErrorCode::FileNotFound, QString("File not found: %1").arg(file));
    }
    static Error parseError(const QString& detail) {
        return Error(ErrorCode::ParseError, detail);
    }
    static Error connectionFailed(const QString& detail) {
        return Error(ErrorCode::ConnectionFailed, detail);
    }
    static Error timeout(const QString& detail = "Operation timed out") {
        return Error(ErrorCode::Timeout, detail);
    }
    static Error invalidParameter(const QString& param) {
        return Error(ErrorCode::InvalidParameter, QString("Invalid parameter: %1").arg(param));
    }
    static Error hardwareError(const QString& detail) {
        return Error(ErrorCode::HardwareError, detail);
    }

private:
    ErrorCode m_code;
    QString m_message;
};

/**
 * @brief 结果类型（C++11 兼容实现）
 *
 * 用法:
 * @code
 * Result<int> divide(int a, int b) {
 *     if (b == 0) {
 *         return Result<int>::error(ErrorCode::InvalidParameter, "Division by zero");
 *     }
 *     return Result<int>::ok(a / b);
 * }
 *
 * auto result = divide(10, 2);
 * if (result.isOk()) {
 *     qDebug() << result.value();
 * } else {
 *     qWarning() << result.error().toString();
 * }
 * @endcode
 */
template<typename T>
class Result
{
public:
    // 默认构造（错误状态）
    Result() : m_hasValue(false) {}

    // 成功构造
    static Result ok(const T& value) {
        Result r;
        r.m_hasValue = true;
        r.m_value = value;
        return r;
    }

    // 错误构造
    static Result error(ErrorCode code, const QString& message) {
        Result r;
        r.m_hasValue = false;
        r.m_error = Error(code, message);
        return r;
    }

    static Result error(const Error& err) {
        Result r;
        r.m_hasValue = false;
        r.m_error = err;
        return r;
    }

    // 状态查询
    bool isOk() const { return m_hasValue; }
    bool isErr() const { return !m_hasValue; }
    explicit operator bool() const { return m_hasValue; }

    // 值访问
    const T& value() const { return m_value; }
    T& value() { return m_value; }

    const T& valueOr(const T& defaultValue) const {
        return m_hasValue ? m_value : defaultValue;
    }

    // 错误访问
    const Error& error() const { return m_error; }

private:
    bool m_hasValue;
    T m_value;
    Error m_error;
};

/**
 * @brief 无返回值的结果类型
 */
class ResultVoid
{
public:
    static ResultVoid ok() {
        ResultVoid r;
        r.m_isOk = true;
        return r;
    }

    static ResultVoid error(ErrorCode code, const QString& message) {
        ResultVoid r;
        r.m_isOk = false;
        r.m_error = Error(code, message);
        return r;
    }

    static ResultVoid error(const Error& err) {
        ResultVoid r;
        r.m_isOk = false;
        r.m_error = err;
        return r;
    }

    bool isOk() const { return m_isOk; }
    bool isErr() const { return !m_isOk; }
    explicit operator bool() const { return m_isOk; }
    const Error& error() const { return m_error; }

private:
    bool m_isOk;
    Error m_error;

    ResultVoid() : m_isOk(false) {}
};

} // namespace tcm

#endif // ERROR_H
