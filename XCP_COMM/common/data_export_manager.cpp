#include "data_export_manager.h"
#include <QDir>
#include <QTextStream>
#include <QDebug>

DataExportManager::DataExportManager(QObject *parent) : QObject(parent)
{
}

DataExportManager::~DataExportManager()
{
    if (m_isExporting) {
        finishExport();
    }
}

void DataExportManager::setExportConfig(const ExportConfig& config)
{
    m_config = config;
}

ExportConfig DataExportManager::getExportConfig() const
{
    return m_config;
}

void DataExportManager::setParamNames(const QStringList& paramNames)
{
    m_paramNames = paramNames;
}

QStringList DataExportManager::getParamNames() const
{
    return m_paramNames;
}

void DataExportManager::addRecord(const DataRecord& record)
{
    m_records.append(record);
    m_recordCount = m_records.count();
}

void DataExportManager::addRecord(const QList<double>& values)
{
    DataRecord record(QDateTime::currentDateTime(), m_paramNames, values);
    addRecord(record);
}

void DataExportManager::clearRecords()
{
    m_records.clear();
    m_recordCount = 0;
}

int DataExportManager::getRecordCount() const
{
    return m_recordCount;
}

bool DataExportManager::startExport()
{
    if (m_isExporting) {
        emit exportError("Export already in progress");
        return false;
    }

    if (m_paramNames.isEmpty()) {
        emit exportError("No parameter names set");
        return false;
    }

    // 生成文件名
    if (m_config.fileName.isEmpty()) {
        m_config.fileName = generateFileName();
    }

    // 确保目录存在
    if (!ensureDirectoryExists(m_config.filePath)) {
        emit exportError("Failed to create directory: " + m_config.filePath);
        return false;
    }

    QString fullPath = m_config.filePath + "/" + m_config.fileName;
    m_file.reset(new QFile(fullPath));

    if (!m_file->open(QIODevice::WriteOnly | QIODevice::Text)) {
        emit exportError("Failed to open file: " + fullPath);
        m_file.reset();
        return false;
    }

    m_isExporting = true;
    m_recordCount = 0;

    // 根据格式启动导出
    bool success = false;
    switch (m_config.format) {
    case ExportFormat::CSV:
        success = startCSVExport();
        break;
    case ExportFormat::JSON:
        success = startJSONExport();
        break;
    case ExportFormat::XML:
        success = startXMLExport();
        break;
    case ExportFormat::Excel:
        success = startExcelExport();
        break;
    }

    if (!success) {
        m_isExporting = false;
        m_file->close();
        m_file.reset();
    }

    return success;
}

bool DataExportManager::exportRecord(const DataRecord& record)
{
    if (!m_isExporting) {
        emit exportError("Export not started");
        return false;
    }

    bool success = false;
    switch (m_config.format) {
    case ExportFormat::CSV:
        success = exportToCSV(record);
        break;
    case ExportFormat::JSON:
        success = exportToJSON(record);
        break;
    case ExportFormat::XML:
        success = exportToXML(record);
        break;
    case ExportFormat::Excel:
        success = exportToExcel(record);
        break;
    }

    if (success) {
        m_recordCount++;
        emit exportProgress(m_recordCount, m_records.count());
    }

    return success;
}

bool DataExportManager::exportRecord(const QList<double>& values)
{
    DataRecord record(QDateTime::currentDateTime(), m_paramNames, values);
    return exportRecord(record);
}

bool DataExportManager::finishExport()
{
    if (!m_isExporting) {
        return true;
    }

    bool success = false;
    switch (m_config.format) {
    case ExportFormat::CSV:
        success = finishCSVExport();
        break;
    case ExportFormat::JSON:
        success = finishJSONExport();
        break;
    case ExportFormat::XML:
        success = finishXMLExport();
        break;
    case ExportFormat::Excel:
        success = finishExcelExport();
        break;
    }

    if (m_file) {
        m_file->close();
        m_file.reset();
    }

    m_xmlWriter.reset();
    m_isExporting = false;

    if (success) {
        emit exportFinished(true, "Export completed successfully. " + 
                           QString::number(m_recordCount) + " records exported.");
    }

    return success;
}

bool DataExportManager::exportAllRecords()
{
    if (!startExport()) {
        return false;
    }

    for (int i = 0; i < m_records.count(); ++i) {
        if (!exportRecord(m_records.at(i))) {
            finishExport();
            return false;
        }
    }

    return finishExport();
}

bool DataExportManager::exportRecords(const QList<DataRecord>& records)
{
    if (!startExport()) {
        return false;
    }

    for (const auto& record : records) {
        if (!exportRecord(record)) {
            finishExport();
            return false;
        }
    }

    return finishExport();
}

QStringList DataExportManager::getSupportedFormats()
{
    return QStringList() << "CSV" << "JSON" << "XML" << "Excel";
}

QString DataExportManager::getFormatExtension(ExportFormat format)
{
    switch (format) {
    case ExportFormat::CSV:
        return ".csv";
    case ExportFormat::JSON:
        return ".json";
    case ExportFormat::XML:
        return ".xml";
    case ExportFormat::Excel:
        return ".xlsx";
    default:
        return ".txt";
    }
}

QString DataExportManager::getFormatDescription(ExportFormat format)
{
    switch (format) {
    case ExportFormat::CSV:
        return "Comma Separated Values";
    case ExportFormat::JSON:
        return "JavaScript Object Notation";
    case ExportFormat::XML:
        return "Extensible Markup Language";
    case ExportFormat::Excel:
        return "Microsoft Excel Spreadsheet";
    default:
        return "Unknown Format";
    }
}

ExportFormat DataExportManager::formatFromString(const QString& formatStr)
{
    QString upper = formatStr.toUpper();
    if (upper == "CSV") return ExportFormat::CSV;
    if (upper == "JSON") return ExportFormat::JSON;
    if (upper == "XML") return ExportFormat::XML;
    if (upper == "EXCEL" || upper == "XLSX") return ExportFormat::Excel;
    return ExportFormat::CSV;
}

QString DataExportManager::formatToString(ExportFormat format)
{
    switch (format) {
    case ExportFormat::CSV:
        return "CSV";
    case ExportFormat::JSON:
        return "JSON";
    case ExportFormat::XML:
        return "XML";
    case ExportFormat::Excel:
        return "Excel";
    default:
        return "Unknown";
    }
}

// CSV导出实现
bool DataExportManager::startCSVExport()
{
    if (!m_config.includeHeader) {
        return true;
    }

    QTextStream stream(m_file.get());
    stream.setCodec(m_config.encoding.toUtf8());

    // 写入表头
    if (m_config.includeTimestamp) {
        stream << "Timestamp" << m_config.delimiter;
    }

    for (int i = 0; i < m_paramNames.count(); ++i) {
        stream << m_paramNames.at(i);
        if (i < m_paramNames.count() - 1) {
            stream << m_config.delimiter;
        }
    }
    stream << "\n";
    stream.flush();

    return true;
}

bool DataExportManager::exportToCSV(const DataRecord& record)
{
    QTextStream stream(m_file.get());
    stream.setCodec(m_config.encoding.toUtf8());

    if (m_config.includeTimestamp) {
        stream << formatTimestamp(record.timestamp) << m_config.delimiter;
    }

    for (int i = 0; i < record.paramValues.count(); ++i) {
        stream << QString::number(record.paramValues.at(i), 'g', 10);
        if (i < record.paramValues.count() - 1) {
            stream << m_config.delimiter;
        }
    }
    stream << "\n";
    stream.flush();

    return true;
}

bool DataExportManager::finishCSVExport()
{
    return true;
}

// JSON导出实现
bool DataExportManager::startJSONExport()
{
    m_jsonArray = QJsonArray();
    return true;
}

bool DataExportManager::exportToJSON(const DataRecord& record)
{
    QJsonObject jsonObj;
    
    if (m_config.includeTimestamp) {
        jsonObj["timestamp"] = record.timestamp.toString(Qt::ISODate);
    }

    QJsonObject dataObj;
    for (int i = 0; i < record.paramNames.count() && i < record.paramValues.count(); ++i) {
        dataObj[record.paramNames.at(i)] = record.paramValues.at(i);
    }
    jsonObj["data"] = dataObj;

    m_jsonArray.append(jsonObj);
    return true;
}

bool DataExportManager::finishJSONExport()
{
    QJsonDocument jsonDoc(m_jsonArray);
    QByteArray jsonData = jsonDoc.toJson(QJsonDocument::Indented);
    
    m_file->write(jsonData);
    return true;
}

// XML导出实现
bool DataExportManager::startXMLExport()
{
    m_xmlWriter.reset(new QXmlStreamWriter(m_file.get()));
    m_xmlWriter->setAutoFormatting(true);
    m_xmlWriter->setAutoFormattingIndent(2);
    
    m_xmlWriter->writeStartDocument();
    m_xmlWriter->writeStartElement("DataExport");
    m_xmlWriter->writeAttribute("version", "1.0");
    m_xmlWriter->writeAttribute("format", "XCP Data");
    
    // 写入参数定义
    m_xmlWriter->writeStartElement("Parameters");
    for (const QString& paramName : m_paramNames) {
        m_xmlWriter->writeStartElement("Parameter");
        m_xmlWriter->writeAttribute("name", paramName);
        m_xmlWriter->writeEndElement();
    }
    m_xmlWriter->writeEndElement();
    
    // 开始数据记录
    m_xmlWriter->writeStartElement("Records");
    
    return true;
}

bool DataExportManager::exportToXML(const DataRecord& record)
{
    m_xmlWriter->writeStartElement("Record");
    
    if (m_config.includeTimestamp) {
        m_xmlWriter->writeAttribute("timestamp", record.timestamp.toString(Qt::ISODate));
    }
    
    for (int i = 0; i < record.paramNames.count() && i < record.paramValues.count(); ++i) {
        m_xmlWriter->writeStartElement("Value");
        m_xmlWriter->writeAttribute("name", record.paramNames.at(i));
        m_xmlWriter->writeCharacters(QString::number(record.paramValues.at(i), 'g', 10));
        m_xmlWriter->writeEndElement();
    }
    
    m_xmlWriter->writeEndElement();
    return true;
}

bool DataExportManager::finishXMLExport()
{
    m_xmlWriter->writeEndElement(); // Records
    m_xmlWriter->writeEndElement(); // DataExport
    m_xmlWriter->writeEndDocument();
    return true;
}

// Excel导出实现（简化版，实际项目中可以使用QtXlsxWriter或其他库）
bool DataExportManager::startExcelExport()
{
    // 对于Excel格式，我们暂时导出为CSV格式，但使用.xlsx扩展名
    // 在实际项目中，可以使用QtXlsxWriter库来实现真正的Excel导出
    return startCSVExport();
}

bool DataExportManager::exportToExcel(const DataRecord& record)
{
    // 暂时使用CSV格式
    return exportToCSV(record);
}

bool DataExportManager::finishExcelExport()
{
    // 暂时使用CSV格式
    return finishCSVExport();
}

// 辅助方法
QString DataExportManager::generateFileName()
{
    QString timestamp = QDateTime::currentDateTime().toString("yyyyMMdd_HHmmss");
    QString formatStr = formatToString(m_config.format).toLower();
    return QString("xcp_data_export_%1.%2").arg(timestamp).arg(getFormatExtension(m_config.format).mid(1));
}

bool DataExportManager::ensureDirectoryExists(const QString& path)
{
    QDir dir(path);
    if (!dir.exists()) {
        return dir.mkpath(".");
    }
    return true;
}

QString DataExportManager::formatTimestamp(const QDateTime& timestamp)
{
    return timestamp.toString("yyyy-MM-dd HH:mm:ss.zzz");
}
