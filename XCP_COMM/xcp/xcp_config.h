/**
 * @file xcp_config.h
 * @brief XCP configuration manager - unified management of timeout, retry and other config parameters
 */

#ifndef XCP_CONFIG_H
#define XCP_CONFIG_H

#include <QObject>
#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonValue>
#include <QDebug>
#include "common/config_manager.h"

/**
 * @struct XCPTimeConfig
 * @brief XCP timeout configuration structure
 */
struct XCPTimeConfig {
    quint32 connectTimeout = 1000;      ///< Connection timeout (CONNECT command)
    quint32 commandTimeout = 1000;      ///< Normal command timeout (GET_STATUS, etc.)
    quint32 daqConfigTimeout = 2000;    ///< DAQ configuration timeout (ALLOC_DAQ, etc.)
    quint32 uploadTimeout = 3000;       ///< Upload timeout (UPLOAD command)
    quint32 downloadTimeout = 3000;     ///< Download timeout (DOWNLOAD command)
    quint32 calPageTimeout = 2000;      ///< Calibration page switch timeout
    quint32 seedKeyTimeout = 2000;      ///< Seed/Key unlock timeout
    quint32 checksumTimeout = 5000;     ///< Checksum calculation timeout
    
    quint32 retryCount = 3;             ///< Retry count
    quint32 retryDelay = 100;           ///< Retry interval (milliseconds)
    
    XCPTimeConfig() = default;
    
    void fromJson(const QJsonObject& json) {
        if (json.contains("timeout")) {
            QJsonObject timeout = json["timeout"].toObject();
            connectTimeout = timeout.value("connect").toInt(connectTimeout);
            commandTimeout = timeout.value("command").toInt(commandTimeout);
            daqConfigTimeout = timeout.value("daqConfig").toInt(daqConfigTimeout);
            uploadTimeout = timeout.value("upload").toInt(uploadTimeout);
            downloadTimeout = timeout.value("download").toInt(downloadTimeout);
            calPageTimeout = timeout.value("calPage").toInt(calPageTimeout);
            seedKeyTimeout = timeout.value("seedKey").toInt(seedKeyTimeout);
            checksumTimeout = timeout.value("checksum").toInt(checksumTimeout);
        }
        if (json.contains("retry")) {
            QJsonObject retry = json["retry"].toObject();
            retryCount = retry.value("count").toInt(retryCount);
            retryDelay = retry.value("delay").toInt(retryDelay);
        }
    }
    
    QJsonObject toJson() const {
        QJsonObject json;
        
        QJsonObject timeout;
        timeout["connect"] = static_cast<int>(connectTimeout);
        timeout["command"] = static_cast<int>(commandTimeout);
        timeout["daqConfig"] = static_cast<int>(daqConfigTimeout);
        timeout["upload"] = static_cast<int>(uploadTimeout);
        timeout["download"] = static_cast<int>(downloadTimeout);
        timeout["calPage"] = static_cast<int>(calPageTimeout);
        timeout["seedKey"] = static_cast<int>(seedKeyTimeout);
        timeout["checksum"] = static_cast<int>(checksumTimeout);
        json["timeout"] = timeout;
        
        QJsonObject retry;
        retry["count"] = static_cast<int>(retryCount);
        retry["delay"] = static_cast<int>(retryDelay);
        json["retry"] = retry;
        
        return json;
    }
};

/**
 * @class XCPConfig
 * @brief XCP configuration manager (singleton)
 */
class XCPConfig : public QObject
{
    Q_OBJECT

public:
    static XCPConfig& instance();
    
    XCPTimeConfig timeConfig() const;
    void setTimeConfig(const XCPTimeConfig& config);
    quint32 getTimeoutForCommand(quint8 commandCode) const;
    
    bool loadFromFile(const QString& path);
    bool saveToFile(const QString& path);
    void resetToDefault();
    void printConfig() const;

signals:
    void configChanged();
    void timeConfigChanged(const XCPTimeConfig& config);

private:
    XCPConfig();
    ~XCPConfig();
    XCPConfig(const XCPConfig&) = delete;
    XCPConfig& operator=(const XCPConfig&) = delete;
    
    XCPTimeConfig m_timeConfig;
    QString m_configFilePath;
};

#endif // XCP_CONFIG_H
