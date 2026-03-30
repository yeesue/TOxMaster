/**
 * @file xcp_config.cpp
 * @brief XCP configuration manager implementation
 */

#include "xcp_config.h"

XCPConfig& XCPConfig::instance()
{
    static XCPConfig instance;
    return instance;
}

XCPConfig::XCPConfig()
    : QObject(nullptr)
{
}

XCPConfig::~XCPConfig()
{
}

void XCPConfig::setTimeConfig(const XCPTimeConfig& config)
{
    m_timeConfig = config;
    emit timeConfigChanged(config);
    emit configChanged();
}

quint32 XCPConfig::getTimeoutForCommand(quint8 commandCode) const
{
    // XCP command codes
    switch (commandCode) {
    case 0xFF: // CONNECT
        return m_timeConfig.connectTimeout;
    case 0xF5: // GET_SEED
    case 0xF6: // UNLOCK
        return m_timeConfig.seedKeyTimeout;
    case 0xF0: // UPLOAD
        return m_timeConfig.uploadTimeout;
    case 0xF1: // SHORT_UPLOAD
        return m_timeConfig.uploadTimeout;
    case 0xF2: // DOWNLOAD
    case 0xF3: // DOWNLOAD_NEXT
        return m_timeConfig.downloadTimeout;
    case 0xF4: // SELECT_CAL_PAGE
    case 0xEB: // GET_CAL_PAGE
        return m_timeConfig.calPageTimeout;
    case 0xC0: // ALLOC_DAQ
    case 0xC1: // ALLOC_ODT
    case 0xC2: // ALLOC_ODT_ENTRY
    case 0xC3: // FREE_DAQ
        return m_timeConfig.daqConfigTimeout;
    case 0xE8: // BUILD_CHECKSUM
        return m_timeConfig.checksumTimeout;
    default:
        return m_timeConfig.commandTimeout;
    }
}

bool XCPConfig::loadFromFile(const QString& path)
{
    QFile file(path);
    if (!file.open(QIODevice::ReadOnly)) {
        qWarning() << "XCPConfig: Failed to open config file:" << path;
        return false;
    }
    
    QByteArray data = file.readAll();
    file.close();
    
    QJsonParseError error;
    QJsonDocument doc = QJsonDocument::fromJson(data, &error);
    if (error.error != QJsonParseError::NoError) {
        qWarning() << "XCPConfig: JSON parse error:" << error.errorString();
        return false;
    }
    
    m_timeConfig.fromJson(doc.object());
    m_configFilePath = path;
    
    emit configChanged();
    return true;
}

bool XCPConfig::saveToFile(const QString& path)
{
    QFile file(path);
    if (!file.open(QIODevice::WriteOnly)) {
        qWarning() << "XCPConfig: Failed to create config file:" << path;
        return false;
    }
    
    QJsonDocument doc(m_timeConfig.toJson());
    file.write(doc.toJson());
    file.close();
    
    return true;
}

void XCPConfig::resetToDefault()
{
    m_timeConfig = XCPTimeConfig();
    emit configChanged();
}

void XCPConfig::printConfig() const
{
    qDebug() << "XCP Configuration:";
    qDebug() << "  connectTimeout:" << m_timeConfig.connectTimeout;
    qDebug() << "  commandTimeout:" << m_timeConfig.commandTimeout;
    qDebug() << "  daqConfigTimeout:" << m_timeConfig.daqConfigTimeout;
    qDebug() << "  uploadTimeout:" << m_timeConfig.uploadTimeout;
    qDebug() << "  downloadTimeout:" << m_timeConfig.downloadTimeout;
    qDebug() << "  retryCount:" << m_timeConfig.retryCount;
    qDebug() << "  retryDelay:" << m_timeConfig.retryDelay;
}
