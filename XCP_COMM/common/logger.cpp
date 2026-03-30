/**
 * @file logger.cpp
 * @brief 统一日志系统实现
 */

#include "logger.h"
#include <QCoreApplication>
#include <QThread>
#include <QDebug>
#include <QDir>

// 注册LogMessage为元类型，以便在信号槽中传递
Q_DECLARE_METATYPE(LogMessage)

// Qt消息处理器（用于捕获qDebug等输出）
static QtMessageHandler originalHandler = nullptr;
static bool customHandlerInstalled = false;

static void customMessageHandler(QtMsgType type, const QMessageLogContext &context, const QString &msg)
{
    // 转发到Logger
    LogLevel level;
    switch (type) {
        case QtDebugMsg:    level = LogLevel::Debug; break;
        case QtInfoMsg:     level = LogLevel::Info; break;
        case QtWarningMsg:  level = LogLevel::Warning; break;
        case QtCriticalMsg: level = LogLevel::Error; break;
        case QtFatalMsg:    level = LogLevel::Critical; break;
        default:            level = LogLevel::Debug; break;
    }

    Logger::instance().log(level, msg, context.file, context.line, context.function);

    // 如果原始处理器存在，也调用它
    if (originalHandler) {
        originalHandler(type, context, msg);
    }
}

void installQtMessageHandler()
{
    if (!customHandlerInstalled) {
        originalHandler = qInstallMessageHandler(customMessageHandler);
        customHandlerInstalled = true;
    }
}

//=============================================================================
// Logger实现
//=============================================================================

Logger& Logger::instance()
{
    static Logger instance;
    return instance;
}

Logger::Logger()
    : QObject(nullptr)
{
    // 注册元类型以支持信号槽传递
    qRegisterMetaType<LogMessage>("LogMessage");
    m_startupTime.start();
}

Logger::~Logger()
{
    close();
}

void Logger::setLogLevel(LogLevel level)
{
    QMutexLocker locker(&m_mutex);
    m_logLevel = level;
}

void Logger::setOutputFlags(LogOutputFlags flags)
{
    QMutexLocker locker(&m_mutex);
    m_outputFlags = flags;
}

bool Logger::setFileOutput(const QString& filePath, bool append)
{
    QMutexLocker locker(&m_mutex);

    // 关闭现有文件
    if (m_logFile.isOpen()) {
        m_logStream.flush();
        m_logFile.close();
        m_fileEnabled = false;
    }

    if (filePath.isEmpty()) {
        return true;  // 禁用文件输出
    }

    // 确保目录存在
    QFileInfo fileInfo(filePath);
    QDir dir = fileInfo.absoluteDir();
    if (!dir.exists()) {
        dir.mkpath(".");
    }

    // 打开文件
    m_logFile.setFileName(filePath);
    QIODevice::OpenMode mode = QIODevice::WriteOnly | QIODevice::Text;
    if (append) {
        mode |= QIODevice::Append;
    }

    if (m_logFile.open(mode)) {
        m_logStream.setDevice(&m_logFile);
        m_logStream.setCodec("UTF-8");
        m_fileEnabled = true;

        // 写入文件头
        m_logStream << "\n========================================\n";
        m_logStream << "Log started: " << QDateTime::currentDateTime().toString("yyyy-MM-dd HH:mm:ss") << "\n";
        m_logStream << "========================================\n\n";
        m_logStream.flush();

        return true;
    } else {
        qWarning() << "Failed to open log file:" << filePath;
        return false;
    }
}

void Logger::setCallback(LogCallback callback, void* userData)
{
    QMutexLocker locker(&m_mutex);
    m_callback = callback;
    m_callbackUserData = userData;
}

void Logger::setFormat(const QString& format)
{
    QMutexLocker locker(&m_mutex);
    m_format = format;
}

void Logger::log(LogLevel level, const QString& message,
                 const char* file, int line,
                 const char* function,
                 const QString& category)
{
    // 检查日志级别
    if (level < m_logLevel) {
        return;
    }

    QMutexLocker locker(&m_mutex);

    // 构建日志消息
    LogMessage msg;
    msg.level = level;
    msg.message = message;
    msg.elapsedMs = m_startupTime.elapsed();

    if (file) {
        msg.context.file = QFileInfo(file).fileName();
    }
    msg.context.line = line;
    if (function) {
        msg.context.function = function;
    }
    msg.context.category = category;
    msg.context.timestamp = QDateTime::currentDateTime();
    msg.context.threadId = currentThreadId();

    // 输出到各个目标
    if (m_outputFlags & OutputConsole) {
        outputToConsole(msg);
    }
    if (m_outputFlags & OutputFile) {
        outputToFile(msg);
    }
    if (m_outputFlags & OutputSignal) {
        outputToSignal(msg);
    }
    if (m_outputFlags & OutputCallback) {
        outputToCallback(msg);
    }
}

void Logger::debug(const QString& message,
                   const char* file, int line,
                   const char* function)
{
    log(LogLevel::Debug, message, file, line, function);
}

void Logger::info(const QString& message,
                  const char* file, int line,
                  const char* function)
{
    log(LogLevel::Info, message, file, line, function);
}

void Logger::warning(const QString& message,
                     const char* file, int line,
                     const char* function)
{
    log(LogLevel::Warning, message, file, line, function);
}

void Logger::error(const QString& message,
                   const char* file, int line,
                   const char* function)
{
    log(LogLevel::Error, message, file, line, function);
}

void Logger::critical(const QString& message,
                      const char* file, int line,
                      const char* function)
{
    log(LogLevel::Critical, message, file, line, function);
}

//=============================================================================
// 性能统计
//=============================================================================

void Logger::perfStart(const QString& name)
{
    QMutexLocker locker(&m_mutex);

    PerfRecord& record = m_perfRecords[name];
    record.name = name;
    record.startTimeNs = m_startupTime.nsecsElapsed();
    record.running = true;
}

qint64 Logger::perfEnd(const QString& name)
{
    QMutexLocker locker(&m_mutex);

    if (!m_perfRecords.contains(name)) {
        return -1;
    }

    PerfRecord& record = m_perfRecords[name];
    if (!record.running) {
        return -1;
    }

    record.durationNs = m_startupTime.nsecsElapsed() - record.startTimeNs;
    record.running = false;
    record.count++;
    record.totalNs += record.durationNs;

    return record.durationNs / 1000000;  // 转换为毫秒
}

const PerfRecord* Logger::perfRecord(const QString& name) const
{
    QMutexLocker locker(&m_mutex);
    auto it = m_perfRecords.constFind(name);
    if (it != m_perfRecords.constEnd()) {
        return &it.value();
    }
    return nullptr;
}

QHash<QString, PerfRecord> Logger::perfRecords() const
{
    QMutexLocker locker(&m_mutex);
    return m_perfRecords;
}

void Logger::perfClear()
{
    QMutexLocker locker(&m_mutex);
    m_perfRecords.clear();
}

void Logger::perfReport()
{
    QMutexLocker locker(&m_mutex);

    QString report = "\n========== Performance Report ==========\n";
    report += QString("%-40s %8s %12s %12s\n")
              .arg("Name", "Count", "Avg(ms)", "Total(ms)");

    for (const PerfRecord& record : m_perfRecords) {
        qint64 avgMs = record.count > 0 ? (record.totalNs / record.count / 1000000) : 0;
        qint64 totalMs = record.totalNs / 1000000;

        report += QString("%-40s %8d %12lld %12lld\n")
                  .arg(record.name)
                  .arg(record.count)
                  .arg(avgMs)
                  .arg(totalMs);
    }

    report += "========================================\n";

    // 直接输出，避免递归
    if (m_outputFlags & OutputConsole) {
        qDebug().noquote() << report;
    }
    if (m_outputFlags & OutputFile && m_fileEnabled) {
        m_logStream << report;
        m_logStream.flush();
    }
}

//=============================================================================
// 输出实现
//=============================================================================

void Logger::outputToConsole(const LogMessage& msg)
{
    QString formatted = formatMessage(msg);

    // 根据级别使用不同的Qt输出
    switch (msg.level) {
        case LogLevel::Debug:
            qDebug().noquote() << formatted;
            break;
        case LogLevel::Info:
            qInfo().noquote() << formatted;
            break;
        case LogLevel::Warning:
            qWarning().noquote() << formatted;
            break;
        case LogLevel::Error:
            qCritical().noquote() << formatted;
            break;
        case LogLevel::Critical:
            qCritical().noquote() << "[CRITICAL]" << formatted;
            break;
        default:
            break;
    }
}

void Logger::outputToFile(const LogMessage& msg)
{
    if (!m_fileEnabled || !m_logFile.isOpen()) {
        return;
    }

    QString formatted = formatMessage(msg);
    m_logStream << formatted << "\n";
    m_logStream.flush();
}

void Logger::outputToSignal(const LogMessage& msg)
{
    emit logMessage(msg);
    emit logMessageString(static_cast<int>(msg.level), msg.message);
}

void Logger::outputToCallback(const LogMessage& msg)
{
    if (m_callback) {
        m_callback(msg, m_callbackUserData);
    }
}

QString Logger::formatMessage(const LogMessage& msg)
{
    QString result = m_format;

    result.replace("{time}", msg.context.timestamp.toString("HH:mm:ss.zzz"));
    result.replace("{date}", msg.context.timestamp.toString("yyyy-MM-dd"));
    result.replace("{level}", levelToString(msg.level));
    result.replace("{message}", msg.message);
    result.replace("{file}", msg.context.file);
    result.replace("{line}", QString::number(msg.context.line));
    result.replace("{function}", msg.context.function);
    result.replace("{thread}", QString::number(msg.context.threadId));
    result.replace("{category}", msg.context.category);
    result.replace("{elapsed}", QString::number(msg.elapsedMs));

    return result;
}

QString Logger::levelToString(LogLevel level)
{
    switch (level) {
        case LogLevel::Debug:    return "DEBUG";
        case LogLevel::Info:     return "INFO ";
        case LogLevel::Warning:  return "WARN ";
        case LogLevel::Error:    return "ERROR";
        case LogLevel::Critical: return "CRIT ";
        case LogLevel::Off:      return "OFF  ";
        default:                 return "?????";
    }
}

QString Logger::levelToColor(LogLevel level)
{
    // 用于终端颜色输出（ANSI颜色码）
    switch (level) {
        case LogLevel::Debug:    return "\033[37m";      // 白色
        case LogLevel::Info:     return "\033[32m";      // 绿色
        case LogLevel::Warning:  return "\033[33m";      // 黄色
        case LogLevel::Error:    return "\033[31m";      // 红色
        case LogLevel::Critical: return "\033[35m";      // 紫色
        default:                 return "\033[0m";       // 重置
    }
}

quint64 Logger::currentThreadId()
{
    return reinterpret_cast<quint64>(QThread::currentThreadId());
}

void Logger::flush()
{
    QMutexLocker locker(&m_mutex);
    if (m_logFile.isOpen()) {
        m_logStream.flush();
    }
}

void Logger::close()
{
    QMutexLocker locker(&m_mutex);

    if (m_logFile.isOpen()) {
        m_logStream << "\n========================================\n";
        m_logStream << "Log ended: " << QDateTime::currentDateTime().toString("yyyy-MM-dd HH:mm:ss") << "\n";
        m_logStream << "========================================\n";
        m_logStream.flush();
        m_logFile.close();
    }
}
