/**
 * @file can_driver_factory.cpp
 * @brief CAN驱动工厂实现
 */

#include "can_driver.h"
#include "../ni/ni_can_driver.h"
#include "../ts/ts_can_driver.h"
#include "../zlg/zlg_can_driver.h"
#include <QCoreApplication>
#include <QFile>

namespace can {

ICanDriver* CanDriverFactory::create(DriverType type, QObject* parent)
{
    switch (type) {
    case DriverType::NI_XNET:
        return new NiCanDriver(parent);
    case DriverType::TS_CAN:
        return new TsCanDriver(parent);
    case DriverType::ZLG_CAN:
        return new ZlgCanDriver(parent);
    default:
        return nullptr;
    }
}

QString CanDriverFactory::driverTypeName(DriverType type)
{
    switch (type) {
    case DriverType::NI_XNET:
        return QStringLiteral("NI-XNET");
    case DriverType::TS_CAN:
        return QStringLiteral("TSCAN");
    case DriverType::ZLG_CAN:
        return QStringLiteral("ZLG");
    default:
        return QStringLiteral("Unknown");
    }
}

QList<CanDriverFactory::DriverType> CanDriverFactory::detectAvailableDrivers()
{
    QList<DriverType> available;
    
#ifdef Q_OS_WIN
    // 存根实现 - 实际检测需要检查DLL是否存在
    // 暂时返回空列表，避免误导
#endif
    
    return available;
}

} // namespace can
