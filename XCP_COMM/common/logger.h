/**
 * @file logger.h
 * @brief Unified Logger System
 */

#ifndef LOGGER_H
#define LOGGER_H

#include <QObject>
#include <QString>
#include <QFile>
#include <QTextStream>
#include <QMutex>
#include <QElapsedTimer>
#include <QDateTime>
#include <QHash>
#include <QDebug>
#include <cstdio>

// Log level definitions
#define LOG_LEVEL_DEBUG     0
#define LOG_LEVEL_INFO      1
#define LOG_LEVEL_WARNING   2
#define LOG_LEVEL_ERROR     3
#define LOG_LEVEL_CRITICAL  4
#define LOG_LEVEL_OFF       5

// Log level enum
enum class LogLevel
{
    Debug = LOG_LEVEL_DEBUG,
    Info = LOG_LEVEL_INFO,
    Warning = LOG_LEVEL_WARNING,
    Error = LOG_LEVEL_ERROR,
    Critical = LOG_LEVEL_CRITICAL,
    Off = LOG_LEVEL_OFF
};

// Log output target flags
enum LogOutputFlag
{
    OutputNone      = 0x00,
    OutputConsole   = 0x01,
    OutputFile      = 0x02,
    OutputSignal    = 0x04,
    OutputCallback  = 0x08,
    OutputAll       = 0xFF
};
Q_DECLARE_FLAGS(LogOutputFlags, LogOutputFlag)

// Log context information
struct LogContext
{
    QString file;
    int line;
    QString function;
    QString category;
    QDateTime timestamp;
    quint64 threadId;
};

// Performance statistics record
struct PerfRecord
{
    QString name;
    qint64 startTimeNs;
    qint64 durationNs;
    bool running;
    int count;
    qint64 totalNs;
};

// Log message structure
struct LogMessage
{
    LogLevel level;
    QString message;
    LogContext context;
    qint64 elapsedMs;
};

// Log callback function type
typedef void (*LogCallback)(const LogMessage& msg, void* userData);

/**
 * @class Logger
 * @brief Unified Logger Manager
 */
class Logger : public QObject
{
    Q_OBJECT

public:
    static Logger& instance();

    // Configuration interfaces
    void setLogLevel(LogLevel level);
    LogLevel logLevel() const { return m_logLevel; }
    void setOutputFlags(LogOutputFlags flags);
    LogOutputFlags outputFlags() const { return m_outputFlags; }
    bool setFileOutput(const QString& filePath, bool append = false);
    void setCallback(LogCallback callback, void* userData = nullptr);
    void setFormat(const QString& format);

    // Log output
    void log(LogLevel level, const QString& message,
              const char* file = nullptr, int line = 0,
              const char* function = nullptr,
              const QString& category = QString());
    void debug(const QString& message,
               const char* file = nullptr, int line = 0,
               const char* function = nullptr);
    void info(const QString& message,
              const char* file = nullptr, int line = 0,
              const char* function = nullptr);
    void warning(const QString& message,
                 const char* file = nullptr, int line = 0,
                 const char* function = nullptr);
    void error(const QString& message,
               const char* file = nullptr, int line = 0,
               const char* function = nullptr);
    void critical(const QString& message,
                  const char* file = nullptr, int line = 0,
                  const char* function = nullptr);

    // Performance statistics
    void perfStart(const QString& name);
    qint64 perfEnd(const QString& name);
    const PerfRecord* perfRecord(const QString& name) const;
    QHash<QString, PerfRecord> perfRecords() const;
    void perfClear();
    void perfReport();

    // Utility interfaces
    void flush();
    void close();

signals:
    void logMessage(const LogMessage& message);
    void logMessageString(int level, const QString& message);

private:
    Logger();
    ~Logger();
    Q_DISABLE_COPY(Logger)

    void outputToConsole(const LogMessage& msg);
    void outputToFile(const LogMessage& msg);
    void outputToSignal(const LogMessage& msg);
    void outputToCallback(const LogMessage& msg);

    QString formatMessage(const LogMessage& msg);
    QString levelToString(LogLevel level);
    QString levelToColor(LogLevel level);
    quint64 currentThreadId();

private:
    mutable QMutex m_mutex;
    LogLevel m_logLevel = LogLevel::Debug;
    LogOutputFlags m_outputFlags = OutputConsole;
    QString m_format = "[{time}] [{level}] {message}";

    QFile m_logFile;
    QTextStream m_logStream;
    bool m_fileEnabled = false;

    LogCallback m_callback = nullptr;
    void* m_callbackUserData = nullptr;

    QElapsedTimer m_startupTime;

    QHash<QString, PerfRecord> m_perfRecords;
};

// Log stream helper class
class LogStream
{
public:
    LogStream(LogLevel level, const char* file, int line, const char* function)
        : m_level(level), m_file(file), m_line(line), m_function(function)
    {
        m_debug = new QDebug(&m_buffer);
    }
    
    ~LogStream()
    {
        delete m_debug;
        if (!m_buffer.isEmpty()) {
            m_buffer.chop(1);
            Logger::instance().log(m_level, m_buffer, m_file, m_line, m_function);
        }
    }
    
    template<typename T>
    LogStream& operator<<(const T& value)
    {
        if (m_debug) {
            (*m_debug) << value;
        }
        return *this;
    }
    
private:
    LogLevel m_level;
    QString m_buffer;
    QDebug* m_debug = nullptr;
    const char* m_file;
    int m_line;
    const char* m_function;
};

// Printf-style log macros
#define LOG_DEBUG(...) \
    do { \
        char _log_buf[4096]; \
        std::snprintf(_log_buf, sizeof(_log_buf), __VA_ARGS__); \
        Logger::instance().log(LogLevel::Debug, QString::fromUtf8(_log_buf), __FILE__, __LINE__, Q_FUNC_INFO); \
    } while(0)

#define LOG_INFO(...) \
    do { \
        char _log_buf[4096]; \
        std::snprintf(_log_buf, sizeof(_log_buf), __VA_ARGS__); \
        Logger::instance().log(LogLevel::Info, QString::fromUtf8(_log_buf), __FILE__, __LINE__, Q_FUNC_INFO); \
    } while(0)

#define LOG_WARN(...) \
    do { \
        char _log_buf[4096]; \
        std::snprintf(_log_buf, sizeof(_log_buf), __VA_ARGS__); \
        Logger::instance().log(LogLevel::Warning, QString::fromUtf8(_log_buf), __FILE__, __LINE__, Q_FUNC_INFO); \
    } while(0)

#define LOG_ERROR(...) \
    do { \
        char _log_buf[4096]; \
        std::snprintf(_log_buf, sizeof(_log_buf), __VA_ARGS__); \
        Logger::instance().log(LogLevel::Error, QString::fromUtf8(_log_buf), __FILE__, __LINE__, Q_FUNC_INFO); \
    } while(0)

#define LOG_CRITICAL(...) \
    do { \
        char _log_buf[4096]; \
        std::snprintf(_log_buf, sizeof(_log_buf), __VA_ARGS__); \
        Logger::instance().log(LogLevel::Critical, QString::fromUtf8(_log_buf), __FILE__, __LINE__, Q_FUNC_INFO); \
    } while(0)

// Stream-style log macros
#define LOG_DEBUG_STREAM() \
    LogStream(LogLevel::Debug, __FILE__, __LINE__, Q_FUNC_INFO)

#define LOG_INFO_STREAM() \
    LogStream(LogLevel::Info, __FILE__, __LINE__, Q_FUNC_INFO)

#define LOG_WARN_STREAM() \
    LogStream(LogLevel::Warning, __FILE__, __LINE__, Q_FUNC_INFO)

#define LOG_ERROR_STREAM() \
    LogStream(LogLevel::Error, __FILE__, __LINE__, Q_FUNC_INFO)

#define LOG_CRITICAL_STREAM() \
    LogStream(LogLevel::Critical, __FILE__, __LINE__, Q_FUNC_INFO)

#define LOG_CATEGORY(cat, level) \
    Logger::instance().log(LogLevel::level, "", __FILE__, __LINE__, Q_FUNC_INFO, cat)

#define LOG_PERF_START(name) \
    Logger::instance().perfStart(name)

#define LOG_PERF_END(name) \
    Logger::instance().perfEnd(name)

class PerfScope
{
public:
    explicit PerfScope(const QString& name) : m_name(name) {
        Logger::instance().perfStart(m_name);
    }
    ~PerfScope() {
        Logger::instance().perfEnd(m_name);
    }
private:
    QString m_name;
};

#define LOG_PERF_SCOPE(name) \
    PerfScope _perfScope_##__LINE__(name)

#endif // LOGGER_H
