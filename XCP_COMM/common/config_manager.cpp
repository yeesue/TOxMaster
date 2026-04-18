/**
 * @file config_manager.cpp
 * @brief Configuration management module implementation
 */

#include "config_manager.h"

namespace common {

ConfigManager* ConfigManager::m_instance = nullptr;

ConfigManager::ConfigManager(QObject* parent)
    : QObject(parent)
{}

ConfigManager* ConfigManager::instance()
{
    if (!m_instance) {
        m_instance = new ConfigManager();
    }
    return m_instance;
}

bool ConfigManager::load(const QString& filePath)
{
    QMutexLocker locker(&m_mutex);
    
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        return false;
    }
    
    QByteArray jsonData = file.readAll();
    file.close();
    
    QJsonParseError parseError;
    QJsonDocument jsonDoc = QJsonDocument::fromJson(jsonData, &parseError);
    if (parseError.error != QJsonParseError::NoError) {
        return false;
    }
    
    if (!jsonDoc.isObject()) {
        return false;
    }
    
    m_config = jsonDoc.object();
    m_currentFilePath = filePath;
    
    emit configurationChanged();
    return true;
}

bool ConfigManager::save(const QString& filePath)
{
    QMutexLocker locker(&m_mutex);
    
    QString savePath = filePath.isEmpty() ? m_currentFilePath : filePath;
    if (savePath.isEmpty()) {
        return false;
    }
    
    QFile file(savePath);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        return false;
    }
    
    QJsonDocument jsonDoc(m_config);
    QByteArray jsonData = jsonDoc.toJson(QJsonDocument::Indented);
    
    qint64 bytesWritten = file.write(jsonData);
    file.close();
    
    if (bytesWritten != jsonData.size()) {
        return false;
    }
    
    if (filePath.isEmpty()) {
        m_currentFilePath = savePath;
    }
    
    return true;
}

QVariant ConfigManager::value(const QString& key, const QVariant& defaultValue) const
{
    QMutexLocker locker(&m_mutex);
    return getNestedValue(key, defaultValue);
}

void ConfigManager::setValue(const QString& key, const QVariant& value)
{
    QMutexLocker locker(&m_mutex);
    
    QStringList parts = key.split('.');
    if (parts.isEmpty()) {
        return;
    }
    
    // 从最内层开始构建对象
    QJsonValue jsonValue = QJsonValue::fromVariant(value);
    
    for (int i = parts.size() - 2; i >= 0; --i) {
        QJsonObject obj;
        obj[parts[i+1]] = jsonValue;
        jsonValue = obj;
    }
    
    // 更新最外层对象
    m_config[parts[0]] = jsonValue;
    
    emit configurationChanged();
}

bool ConfigManager::contains(const QString& key) const
{
    QMutexLocker locker(&m_mutex);
    return !getNestedValue(key, QVariant()).isNull();
}

void ConfigManager::remove(const QString& key)
{
    QMutexLocker locker(&m_mutex);
    
    QStringList parts = key.split('.');
    if (parts.isEmpty()) {
        return;
    }
    
    // 递归删除键
    bool removed = removeNestedKey(m_config, parts, 0);
    if (removed) {
        emit configurationChanged();
    }
}

bool ConfigManager::removeNestedKey(QJsonObject& obj, const QStringList& parts, int index)
{
    if (index >= parts.size() - 1) {
        // 到达最后一级，删除键
        const QString& key = parts.last();
        if (obj.contains(key)) {
            obj.remove(key);
            return true;
        }
        return false;
    }
    
    const QString& key = parts[index];
    if (!obj.contains(key) || !obj[key].isObject()) {
        return false;
    }
    
    QJsonObject nestedObj = obj[key].toObject();
    bool removed = removeNestedKey(nestedObj, parts, index + 1);
    
    if (removed) {
        if (nestedObj.isEmpty()) {
            obj.remove(key);
        } else {
            obj[key] = nestedObj;
        }
        return true;
    }
    
    return false;
}

void ConfigManager::clear()
{
    QMutexLocker locker(&m_mutex);
    m_config = QJsonObject();
    emit configurationChanged();
}

QString ConfigManager::currentFilePath() const
{
    QMutexLocker locker(&m_mutex);
    return m_currentFilePath;
}

QJsonObject ConfigManager::toJsonObject() const
{
    QMutexLocker locker(&m_mutex);
    return m_config;
}

void ConfigManager::fromJsonObject(const QJsonObject& object)
{
    QMutexLocker locker(&m_mutex);
    m_config = object;
    emit configurationChanged();
}

QJsonObject ConfigManager::getNestedObject(const QString& key, bool createIfNotFound)
{
    QStringList parts = key.split('.');
    if (parts.isEmpty()) {
        return QJsonObject();
    }
    
    QJsonObject current = m_config;
    
    for (int i = 0; i < parts.size(); ++i) {
        const QString& part = parts[i];
        if (!current.contains(part)) {
            if (!createIfNotFound) {
                return QJsonObject();
            }
            current[part] = QJsonObject();
        } else if (!current[part].isObject()) {
            if (!createIfNotFound) {
                return QJsonObject();
            }
            current[part] = QJsonObject();
        }
        current = current[part].toObject();
    }
    
    return current;
}

QVariant ConfigManager::getNestedValue(const QString& key, const QVariant& defaultValue) const
{
    QStringList parts = key.split('.');
    if (parts.isEmpty()) {
        return defaultValue;
    }
    
    QJsonObject current = m_config;
    
    for (int i = 0; i < parts.size() - 1; ++i) {
        const QString& part = parts[i];
        if (!current.contains(part) || !current[part].isObject()) {
            return defaultValue;
        }
        current = current[part].toObject();
    }
    
    const QString& lastPart = parts.last();
    if (!current.contains(lastPart)) {
        return defaultValue;
    }
    
    return current[lastPart].toVariant();
}

} // namespace common