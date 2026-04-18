/**
 * @file config_manager.h
 * @brief Configuration management module
 * 
 * Provides unified configuration management interface
 * Supports JSON configuration files
 */

#ifndef CONFIG_MANAGER_H
#define CONFIG_MANAGER_H

#include <QObject>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QFile>
#include <QString>
#include <QVariant>
#include <QMutex>

namespace common {

/**
 * @brief Configuration manager class
 * 
 * Provides unified configuration management
 */
class ConfigManager : public QObject
{
    Q_OBJECT

public:
    /**
     * @brief Get singleton instance
     */
    static ConfigManager* instance();

    /**
     * @brief Load configuration from file
     * @param filePath Configuration file path
     * @return true on success
     */
    bool load(const QString& filePath);

    /**
     * @brief Save configuration to file
     * @param filePath Configuration file path, empty for current file
     * @return true on success
     */
    bool save(const QString& filePath = QString());

    /**
     * @brief Get configuration value
     * @param key Configuration key (supports dot notation: "section.key")
     * @param defaultValue Default value if key not found
     * @return Configuration value
     */
    QVariant value(const QString& key, const QVariant& defaultValue = QVariant()) const;

    /**
     * @brief Set configuration value
     * @param key Configuration key (supports dot notation: "section.key")
     * @param value Configuration value
     */
    void setValue(const QString& key, const QVariant& value);

    /**
     * @brief Check if configuration key exists
     * @param key Configuration key
     * @return true if key exists
     */
    bool contains(const QString& key) const;

    /**
     * @brief Remove configuration key
     * @param key Configuration key
     */
    void remove(const QString& key);

    /**
     * @brief Clear all configuration
     */
    void clear();

    /**
     * @brief Get current configuration file path
     */
    QString currentFilePath() const;

    /**
     * @brief Get all configuration as JSON object
     */
    QJsonObject toJsonObject() const;

    /**
     * @brief Set configuration from JSON object
     */
    void fromJsonObject(const QJsonObject& object);

signals:
    /**
     * @brief Emitted when configuration is changed
     */
    void configurationChanged();

private:
    /**
     * @brief Private constructor (singleton pattern)
     */
    ConfigManager(QObject* parent = nullptr);

    /**
     * @brief Get nested object by key path
     * @param key Configuration key
     * @param createIfNotFound Create object if not found
     * @return Nested object
     */
    QJsonObject getNestedObject(const QString& key, bool createIfNotFound = false);

    /**
     * @brief Get nested value by key path
     * @param key Configuration key
     * @param defaultValue Default value if key not found
     * @return Configuration value
     */
    QVariant getNestedValue(const QString& key, const QVariant& defaultValue) const;
    
    /**
     * @brief Remove nested key recursively
     * @param obj JSON object to modify
     * @param parts Key parts
     * @param index Current index in key parts
     * @return true if key was removed
     */
    bool removeNestedKey(QJsonObject& obj, const QStringList& parts, int index);

private:
    static ConfigManager* m_instance;
    QJsonObject m_config;
    QString m_currentFilePath;
    mutable QMutex m_mutex;
};

/**
 * @brief Configuration helper macros
 */
#define CONFIG ConfigManager::instance()
#define CONFIG_VALUE(key, defaultValue) ConfigManager::instance()->value(key, defaultValue)
#define CONFIG_SET_VALUE(key, value) ConfigManager::instance()->setValue(key, value)
#define CONFIG_CONTAINS(key) ConfigManager::instance()->contains(key)
#define CONFIG_REMOVE(key) ConfigManager::instance()->remove(key)

} // namespace common

#endif // CONFIG_MANAGER_H