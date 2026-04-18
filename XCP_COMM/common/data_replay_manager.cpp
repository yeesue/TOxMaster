#include "data_replay_manager.h"
#include <QDir>
#include <QDebug>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QXmlStreamReader>

DataReplayManager::DataReplayManager(QObject *parent) : QObject(parent)
{
    m_replayTimer = new QTimer(this);
    connect(m_replayTimer, &QTimer::timeout, this, &DataReplayManager::onTimerTimeout);
}

DataReplayManager::~DataReplayManager()
{
    stopReplay();
}

void DataReplayManager::setReplayConfig(const ReplayConfig& config)
{
    m_config = config;
}

ReplayConfig DataReplayManager::getReplayConfig() const
{
    return m_config;
}

bool DataReplayManager::loadDataFile(const QString& filePath)
{
    if (filePath.isEmpty()) {
        emit replayError("File path is empty");
        return false;
    }

    QString format = detectFileFormat(filePath);
    
    if (format == "csv") {
        return loadDataFromCSV(filePath);
    } else if (format == "json") {
        return loadDataFromJSON(filePath);
    } else if (format == "xml") {
        return loadDataFromXML(filePath);
    } else {
        emit replayError("Unsupported file format: " + format);
        return false;
    }
}

bool DataReplayManager::loadDataFromCSV(const QString& filePath)
{
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        emit replayError("Failed to open file: " + filePath);
        return false;
    }

    clearData();
    
    QTextStream stream(&file);
    stream.setCodec("UTF-8");
    
    // 读取表头
    QString headerLine = stream.readLine();
    if (headerLine.isEmpty()) {
        emit replayError("Empty file or missing header");
        file.close();
        return false;
    }
    
    // 解析表头
    QStringList headers = headerLine.split(",");
    for (const QString& header : headers) {
        QString trimmed = header.trimmed();
        if (trimmed != "Timestamp" && !trimmed.isEmpty()) {
            m_paramNames.append(trimmed);
        }
    }
    
    // 读取数据行
    int lineNumber = 1;
    while (!stream.atEnd()) {
        QString line = stream.readLine();
        lineNumber++;
        
        if (line.isEmpty()) continue;
        
        ReplayDataPoint dataPoint;
        if (parseCSVLine(line, dataPoint)) {
            m_dataPoints.append(dataPoint);
        } else {
            qWarning() << "Failed to parse line" << lineNumber << ":" << line;
        }
    }
    
    file.close();
    
    if (m_dataPoints.isEmpty()) {
        emit replayError("No valid data points found in file");
        return false;
    }
    
    qInfo() << "Loaded" << m_dataPoints.count() << "data points from CSV file";
    return true;
}

bool DataReplayManager::loadDataFromJSON(const QString& filePath)
{
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly)) {
        emit replayError("Failed to open file: " + filePath);
        return false;
    }

    QByteArray jsonData = file.readAll();
    file.close();

    return parseJSONData(jsonData);
}

bool DataReplayManager::loadDataFromXML(const QString& filePath)
{
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly)) {
        emit replayError("Failed to open file: " + filePath);
        return false;
    }

    QByteArray xmlData = file.readAll();
    file.close();

    return parseXMLData(xmlData);
}

void DataReplayManager::clearData()
{
    m_dataPoints.clear();
    m_paramNames.clear();
    m_currentIndex = 0;
    m_elapsedTime_ms = 0;
}

bool DataReplayManager::startReplay()
{
    if (m_dataPoints.isEmpty()) {
        emit replayError("No data loaded");
        return false;
    }

    if (m_status == ReplayStatus::Playing) {
        return true;
    }

    m_currentIndex = 0;
    m_elapsedTime_ms = m_config.startTime_ms;
    m_startTime = QDateTime::currentDateTime();
    m_loopPlayback = m_config.loopPlayback;
    
    // 计算第一个时间间隔
    qint64 interval = calculateTimeInterval(m_currentIndex);
    if (interval > 0) {
        m_replayTimer->start(static_cast<int>(interval));
    } else {
        m_replayTimer->start(100); // 默认100ms
    }
    
    m_status = ReplayStatus::Playing;
    emit replayStarted();
    updateCurrentDataPoint();
    
    return true;
}

void DataReplayManager::pauseReplay()
{
    if (m_status != ReplayStatus::Playing) {
        return;
    }

    m_replayTimer->stop();
    m_elapsedTime_ms += m_startTime.msecsTo(QDateTime::currentDateTime());
    m_status = ReplayStatus::Paused;
    emit replayPaused();
}

void DataReplayManager::resumeReplay()
{
    if (m_status != ReplayStatus::Paused) {
        return;
    }

    m_startTime = QDateTime::currentDateTime();
    
    qint64 interval = calculateTimeInterval(m_currentIndex);
    if (interval > 0) {
        m_replayTimer->start(static_cast<int>(interval));
    } else {
        m_replayTimer->start(100);
    }
    
    m_status = ReplayStatus::Playing;
    emit replayResumed();
}

void DataReplayManager::stopReplay()
{
    if (m_status == ReplayStatus::Stopped) {
        return;
    }

    m_replayTimer->stop();
    m_currentIndex = 0;
    m_elapsedTime_ms = 0;
    m_status = ReplayStatus::Stopped;
    emit replayStopped();
}

void DataReplayManager::seekTo(qint64 time_ms)
{
    if (m_dataPoints.isEmpty()) {
        return;
    }

    // 查找对应时间的数据点索引
    for (int i = 0; i < m_dataPoints.count(); ++i) {
        qint64 pointTime = m_dataPoints.at(i).timestamp.toMSecsSinceEpoch();
        if (pointTime >= time_ms) {
            m_currentIndex = i;
            m_elapsedTime_ms = time_ms;
            updateCurrentDataPoint();
            return;
        }
    }
    
    // 如果时间超过最后一个数据点，定位到最后
    m_currentIndex = m_dataPoints.count() - 1;
    updateCurrentDataPoint();
}

void DataReplayManager::seekToPercent(int percent)
{
    if (m_dataPoints.isEmpty() || percent < 0 || percent > 100) {
        return;
    }

    int index = (percent * m_dataPoints.count()) / 100;
    m_currentIndex = qBound(0, index, m_dataPoints.count() - 1);
    updateCurrentDataPoint();
}

ReplayStatus DataReplayManager::getStatus() const
{
    return m_status;
}

bool DataReplayManager::isPlaying() const
{
    return m_status == ReplayStatus::Playing;
}

bool DataReplayManager::isPaused() const
{
    return m_status == ReplayStatus::Paused;
}

bool DataReplayManager::isStopped() const
{
    return m_status == ReplayStatus::Stopped;
}

qint64 DataReplayManager::getCurrentTime() const
{
    if (m_status == ReplayStatus::Playing) {
        return m_elapsedTime_ms + m_startTime.msecsTo(QDateTime::currentDateTime());
    }
    return m_elapsedTime_ms;
}

qint64 DataReplayManager::getTotalDuration() const
{
    if (m_dataPoints.count() < 2) {
        return 0;
    }
    
    qint64 startTime = m_dataPoints.first().timestamp.toMSecsSinceEpoch();
    qint64 endTime = m_dataPoints.last().timestamp.toMSecsSinceEpoch();
    return endTime - startTime;
}

int DataReplayManager::getProgressPercent() const
{
    if (m_dataPoints.isEmpty()) {
        return 0;
    }
    return (m_currentIndex * 100) / m_dataPoints.count();
}

int DataReplayManager::getCurrentDataIndex() const
{
    return m_currentIndex;
}

int DataReplayManager::getTotalDataCount() const
{
    return m_dataPoints.count();
}

QStringList DataReplayManager::getParamNames() const
{
    return m_paramNames;
}

ReplayDataPoint DataReplayManager::getCurrentDataPoint() const
{
    if (m_currentIndex >= 0 && m_currentIndex < m_dataPoints.count()) {
        return m_dataPoints.at(m_currentIndex);
    }
    return ReplayDataPoint();
}

ReplayDataPoint DataReplayManager::getDataPointAt(int index) const
{
    if (index >= 0 && index < m_dataPoints.count()) {
        return m_dataPoints.at(index);
    }
    return ReplayDataPoint();
}

QList<ReplayDataPoint> DataReplayManager::getAllDataPoints() const
{
    return m_dataPoints;
}

void DataReplayManager::setReplaySpeed(ReplaySpeed speed)
{
    m_config.speed = speed;
}

ReplaySpeed DataReplayManager::getReplaySpeed() const
{
    return m_config.speed;
}

QString DataReplayManager::speedToString(ReplaySpeed speed)
{
    switch (speed) {
    case ReplaySpeed::Speed_0_5x: return "0.5x";
    case ReplaySpeed::Speed_1x: return "1x";
    case ReplaySpeed::Speed_2x: return "2x";
    case ReplaySpeed::Speed_5x: return "5x";
    case ReplaySpeed::Speed_10x: return "10x";
    default: return "1x";
    }
}

double DataReplayManager::speedToFactor(ReplaySpeed speed)
{
    switch (speed) {
    case ReplaySpeed::Speed_0_5x: return 0.5;
    case ReplaySpeed::Speed_1x: return 1.0;
    case ReplaySpeed::Speed_2x: return 2.0;
    case ReplaySpeed::Speed_5x: return 5.0;
    case ReplaySpeed::Speed_10x: return 10.0;
    default: return 1.0;
    }
}

QStringList DataReplayManager::getSupportedFormats()
{
    return QStringList() << "CSV" << "JSON" << "XML";
}

QString DataReplayManager::getFormatFilter()
{
    return "Data Files (*.csv *.json *.xml);;CSV Files (*.csv);;JSON Files (*.json);;XML Files (*.xml);;All Files (*)";
}

void DataReplayManager::onTimerTimeout()
{
    if (m_status != ReplayStatus::Playing) {
        return;
    }

    // 发送当前数据点
    updateCurrentDataPoint();

    // 移动到下一个数据点
    m_currentIndex++;

    // 检查是否播放完成
    if (m_currentIndex >= m_dataPoints.count()) {
        if (m_loopPlayback) {
            m_currentIndex = 0;
            m_elapsedTime_ms = 0;
            m_startTime = QDateTime::currentDateTime();
        } else {
            stopReplay();
            m_status = ReplayStatus::Finished;
            emit replayFinished();
            return;
        }
    }

    // 设置下一个定时器
    qint64 interval = calculateTimeInterval(m_currentIndex);
    if (interval > 0) {
        m_replayTimer->start(static_cast<int>(interval));
    } else {
        m_replayTimer->start(100);
    }
}

bool DataReplayManager::parseCSVLine(const QString& line, ReplayDataPoint& dataPoint)
{
    QStringList values = line.split(",");
    if (values.isEmpty()) {
        return false;
    }

    // 第一个值应该是时间戳
    QString timestampStr = values.first().trimmed();
    dataPoint.timestamp = parseTimestamp(timestampStr);
    
    // 解析数值
    dataPoint.paramNames = m_paramNames;
    for (int i = 1; i < values.count() && (i-1) < m_paramNames.count(); ++i) {
        bool ok;
        double value = values.at(i).trimmed().toDouble(&ok);
        if (ok) {
            dataPoint.paramValues.append(value);
        } else {
            dataPoint.paramValues.append(0.0);
        }
    }

    return !dataPoint.paramValues.isEmpty();
}

bool DataReplayManager::parseJSONData(const QByteArray& jsonData)
{
    QJsonDocument doc = QJsonDocument::fromJson(jsonData);
    if (doc.isNull() || !doc.isArray()) {
        emit replayError("Invalid JSON format");
        return false;
    }

    clearData();
    
    QJsonArray array = doc.array();
    for (const QJsonValue& value : array) {
        if (!value.isObject()) continue;
        
        QJsonObject obj = value.toObject();
        ReplayDataPoint point;
        
        if (obj.contains("timestamp")) {
            point.timestamp = QDateTime::fromString(obj["timestamp"].toString(), Qt::ISODate);
        }
        
        if (obj.contains("data") && obj["data"].isObject()) {
            QJsonObject dataObj = obj["data"].toObject();
            for (const QString& key : dataObj.keys()) {
                if (m_paramNames.isEmpty()) {
                    m_paramNames.append(key);
                }
                point.paramNames.append(key);
                point.paramValues.append(dataObj[key].toDouble());
            }
        }
        
        m_dataPoints.append(point);
    }

    return !m_dataPoints.isEmpty();
}

bool DataReplayManager::parseXMLData(const QByteArray& xmlData)
{
    QXmlStreamReader reader(xmlData);
    clearData();

    while (!reader.atEnd() && !reader.hasError()) {
        reader.readNext();
        
        if (reader.isStartElement() && reader.name() == "Record") {
            ReplayDataPoint point;
            
            if (reader.attributes().hasAttribute("timestamp")) {
                point.timestamp = QDateTime::fromString(
                    reader.attributes().value("timestamp").toString(), Qt::ISODate);
            }
            
            // 解析子元素
            while (!(reader.isEndElement() && reader.name() == "Record")) {
                reader.readNext();
                
                if (reader.isStartElement() && reader.name() == "Value") {
                    QString name = reader.attributes().value("name").toString();
                    QString valueStr = reader.readElementText();
                    
                    if (m_paramNames.isEmpty()) {
                        m_paramNames.append(name);
                    }
                    point.paramNames.append(name);
                    point.paramValues.append(valueStr.toDouble());
                }
            }
            
            m_dataPoints.append(point);
        }
    }

    if (reader.hasError()) {
        emit replayError("XML parsing error: " + reader.errorString());
        return false;
    }

    return !m_dataPoints.isEmpty();
}

qint64 DataReplayManager::calculateTimeInterval(int currentIndex)
{
    if (currentIndex <= 0 || currentIndex >= m_dataPoints.count()) {
        return 100; // 默认100ms
    }

    qint64 time1 = m_dataPoints.at(currentIndex - 1).timestamp.toMSecsSinceEpoch();
    qint64 time2 = m_dataPoints.at(currentIndex).timestamp.toMSecsSinceEpoch();
    qint64 interval = time2 - time1;

    // 应用回放速度
    double speedFactor = speedToFactor(m_config.speed);
    interval = static_cast<qint64>(interval / speedFactor);

    return qMax(interval, static_cast<qint64>(10)); // 最小10ms
}

void DataReplayManager::updateCurrentDataPoint()
{
    if (m_currentIndex >= 0 && m_currentIndex < m_dataPoints.count()) {
        ReplayDataPoint point = m_dataPoints.at(m_currentIndex);
        emit dataPointReady(point);
        emit progressChanged(m_currentIndex, m_dataPoints.count());
        emit percentChanged(getProgressPercent());
        
        qint64 currentTime = point.timestamp.toMSecsSinceEpoch();
        qint64 totalTime = getTotalDuration();
        emit timeChanged(currentTime, totalTime);
    }
}

QString DataReplayManager::detectFileFormat(const QString& filePath)
{
    QString extension = QFileInfo(filePath).suffix().toLower();
    return extension;
}

QDateTime DataReplayManager::parseTimestamp(const QString& timestampStr)
{
    // 尝试多种时间格式
    QStringList formats;
    formats << "yyyy-MM-dd HH:mm:ss.zzz"
            << "yyyy-MM-dd HH:mm:ss"
            << "yyyy/MM/dd HH:mm:ss.zzz"
            << "yyyy/MM/dd HH:mm:ss"
            << "HH:mm:ss.zzz"
            << "HH:mm:ss";

    for (int i = 0; i < formats.size(); ++i) {
        QDateTime dt = QDateTime::fromString(timestampStr, formats[i]);
        if (dt.isValid()) {
            return dt;
        }
    }

    // 尝试 ISO 格式
    QDateTime dt = QDateTime::fromString(timestampStr, Qt::ISODate);
    if (dt.isValid()) {
        return dt;
    }

    // 如果所有格式都失败，返回当前时间
    return QDateTime::currentDateTime();
}
