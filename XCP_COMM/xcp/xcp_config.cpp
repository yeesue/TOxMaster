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
    // Load default configuration if no config file is loaded
    if (!common::ConfigManager::instance()->contains("xcp.timeout")) {
        resetToDefault();
    }
}

XCPConfig::~XCPConfig()
{
}

XCPTimeConfig XCPConfig::timeConfig() const
{
    XCPTimeConfig config;
    
    // Load configuration from ConfigManager
    config.connectTimeout = common::CONFIG_VALUE("xcp.timeout.connect", config.connectTimeout).toUInt();
    config.commandTimeout = common::CONFIG_VALUE("xcp.timeout.command", config.commandTimeout).toUInt();
    config.daqConfigTimeout = common::CONFIG_VALUE("xcp.timeout.daqConfig", config.daqConfigTimeout).toUInt();
    config.uploadTimeout = common::CONFIG_VALUE("xcp.timeout.upload", config.uploadTimeout).toUInt();
    config.downloadTimeout = common::CONFIG_VALUE("xcp.timeout.download", config.downloadTimeout).toUInt();
    config.calPageTimeout = common::CONFIG_VALUE("xcp.timeout.calPage", config.calPageTimeout).toUInt();
    config.seedKeyTimeout = common::CONFIG_VALUE("xcp.timeout.seedKey", config.seedKeyTimeout).toUInt();
    config.checksumTimeout = common::CONFIG_VALUE("xcp.timeout.checksum", config.checksumTimeout).toUInt();
    config.retryCount = common::CONFIG_VALUE("xcp.retry.count", config.retryCount).toUInt();
    config.retryDelay = common::CONFIG_VALUE("xcp.retry.delay", config.retryDelay).toUInt();
    
    return config;
}

void XCPConfig::setTimeConfig(const XCPTimeConfig& config)
{
    // Save configuration to ConfigManager
    common::CONFIG_SET_VALUE("xcp.timeout.connect", config.connectTimeout);
    common::CONFIG_SET_VALUE("xcp.timeout.command", config.commandTimeout);
    common::CONFIG_SET_VALUE("xcp.timeout.daqConfig", config.daqConfigTimeout);
    common::CONFIG_SET_VALUE("xcp.timeout.upload", config.uploadTimeout);
    common::CONFIG_SET_VALUE("xcp.timeout.download", config.downloadTimeout);
    common::CONFIG_SET_VALUE("xcp.timeout.calPage", config.calPageTimeout);
    common::CONFIG_SET_VALUE("xcp.timeout.seedKey", config.seedKeyTimeout);
    common::CONFIG_SET_VALUE("xcp.timeout.checksum", config.checksumTimeout);
    common::CONFIG_SET_VALUE("xcp.retry.count", config.retryCount);
    common::CONFIG_SET_VALUE("xcp.retry.delay", config.retryDelay);
    
    emit timeConfigChanged(config);
    emit configChanged();
}

quint32 XCPConfig::getTimeoutForCommand(quint8 commandCode) const
{
    // XCP command codes
    switch (commandCode) {
    case 0xFF: // CONNECT
        return common::CONFIG_VALUE("xcp.timeout.connect", 1000).toUInt();
    case 0xF5: // GET_SEED
    case 0xF6: // UNLOCK
        return common::CONFIG_VALUE("xcp.timeout.seedKey", 2000).toUInt();
    case 0xF0: // UPLOAD
        return common::CONFIG_VALUE("xcp.timeout.upload", 3000).toUInt();
    case 0xF1: // SHORT_UPLOAD
        return common::CONFIG_VALUE("xcp.timeout.upload", 3000).toUInt();
    case 0xF2: // DOWNLOAD
    case 0xF3: // DOWNLOAD_NEXT
        return common::CONFIG_VALUE("xcp.timeout.download", 3000).toUInt();
    case 0xF4: // SELECT_CAL_PAGE
    case 0xEB: // GET_CAL_PAGE
        return common::CONFIG_VALUE("xcp.timeout.calPage", 2000).toUInt();
    case 0xC0: // ALLOC_DAQ
    case 0xC1: // ALLOC_ODT
    case 0xC2: // ALLOC_ODT_ENTRY
    case 0xC3: // FREE_DAQ
        return common::CONFIG_VALUE("xcp.timeout.daqConfig", 2000).toUInt();
    case 0xE8: // BUILD_CHECKSUM
        return common::CONFIG_VALUE("xcp.timeout.checksum", 5000).toUInt();
    default:
        return common::CONFIG_VALUE("xcp.timeout.command", 1000).toUInt();
    }
}

bool XCPConfig::loadFromFile(const QString& path)
{
    bool success = common::ConfigManager::instance()->load(path);
    if (success) {
        emit configChanged();
    }
    return success;
}

bool XCPConfig::saveToFile(const QString& path)
{
    return common::ConfigManager::instance()->save(path);
}

void XCPConfig::resetToDefault()
{
    XCPTimeConfig defaultConfig;
    setTimeConfig(defaultConfig);
    emit configChanged();
}

void XCPConfig::printConfig() const
{
    XCPTimeConfig config = timeConfig();
    qDebug() << "XCP Configuration:";
    qDebug() << "  connectTimeout:" << config.connectTimeout;
    qDebug() << "  commandTimeout:" << config.commandTimeout;
    qDebug() << "  daqConfigTimeout:" << config.daqConfigTimeout;
    qDebug() << "  uploadTimeout:" << config.uploadTimeout;
    qDebug() << "  downloadTimeout:" << config.downloadTimeout;
    qDebug() << "  calPageTimeout:" << config.calPageTimeout;
    qDebug() << "  seedKeyTimeout:" << config.seedKeyTimeout;
    qDebug() << "  checksumTimeout:" << config.checksumTimeout;
    qDebug() << "  retryCount:" << config.retryCount;
    qDebug() << "  retryDelay:" << config.retryDelay;
}
