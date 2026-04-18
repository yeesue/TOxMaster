#ifndef DATA_EXPORT_MANAGER_H
#define DATA_EXPORT_MANAGER_H

#include <QObject>
#include <QString>
#include <QStringList>
#include <QList>
#include <QDateTime>
#include <QJsonObject>
#include <QJsonArray>
#include <QJsonDocument>
#include <QXmlStreamWriter>
#include <QFile>
#include <memory>

// 导出格式枚举
enum class ExportFormat {
    CSV,
    JSON,
    XML,
    Excel
};

// 数据记录结构
struct DataRecord {
    QDateTime timestamp;
    QStringList paramNames;
    QList<double> paramValues;
    
    DataRecord() = default;
    DataRecord(const QDateTime& time, const QStringList& names, const QList<double>& values)
        : timestamp(time), paramNames(names), paramValues(values) {}
};

// 导出配置结构
struct ExportConfig {
    ExportFormat format = ExportFormat::CSV;
    QString filePath;
    QString fileName;
    bool includeTimestamp = true;
    bool includeHeader = true;
    QString delimiter = ",";
    QString encoding = "UTF-8";
};

class DataExportManager : public QObject
{
    Q_OBJECT
public:
    explicit DataExportManager(QObject *parent = nullptr);
    ~DataExportManager();

    // 设置导出配置
    void setExportConfig(const ExportConfig& config);
    ExportConfig getExportConfig() const;

    // 设置参数列表
    void setParamNames(const QStringList& paramNames);
    QStringList getParamNames() const;

    // 数据记录操作
    void addRecord(const DataRecord& record);
    void addRecord(const QList<double>& values);
    void clearRecords();
    int getRecordCount() const;

    // 导出操作
    bool startExport();
    bool exportRecord(const DataRecord& record);
    bool exportRecord(const QList<double>& values);
    bool finishExport();
    
    // 批量导出
    bool exportAllRecords();
    bool exportRecords(const QList<DataRecord>& records);

    // 获取支持的格式列表
    static QStringList getSupportedFormats();
    static QString getFormatExtension(ExportFormat format);
    static QString getFormatDescription(ExportFormat format);

    // 格式转换
    static ExportFormat formatFromString(const QString& formatStr);
    static QString formatToString(ExportFormat format);

signals:
    void exportProgress(int current, int total);
    void exportFinished(bool success, const QString& message);
    void exportError(const QString& errorMessage);

private:
    // 各格式导出方法
    bool exportToCSV(const DataRecord& record);
    bool exportToJSON(const DataRecord& record);
    bool exportToXML(const DataRecord& record);
    bool exportToExcel(const DataRecord& record);

    bool startCSVExport();
    bool startJSONExport();
    bool startXMLExport();
    bool startExcelExport();

    bool finishCSVExport();
    bool finishJSONExport();
    bool finishXMLExport();
    bool finishExcelExport();

    // 辅助方法
    QString generateFileName();
    bool ensureDirectoryExists(const QString& path);
    QString formatTimestamp(const QDateTime& timestamp);

private:
    ExportConfig m_config;
    QStringList m_paramNames;
    QList<DataRecord> m_records;
    
    std::unique_ptr<QFile> m_file;
    std::unique_ptr<QXmlStreamWriter> m_xmlWriter;
    QJsonArray m_jsonArray;
    
    bool m_isExporting = false;
    int m_recordCount = 0;
};

#endif // DATA_EXPORT_MANAGER_H
