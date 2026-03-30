#ifndef DRIVERINTERFACE_H
#define DRIVERINTERFACE_H
#include <QtPlugin>

class DriverInterface
{
public:
    virtual ~DriverInterface() {}
    virtual void DRV_Init()=0;  // 接口初始化
    virtual void DRV_Show()=0;  // 显示接口界面
    virtual void DRV_Stop()=0;  // 关闭接口
};

#define DriverInterface_iid "SAIC.SEAT.YangZhihao.BenchConsole.DriverInterface"
Q_DECLARE_INTERFACE(DriverInterface, DriverInterface_iid)

#if QT_VERSION <0x050000
Q_EXPORT_PLUGIN2(DriverInterface,DriverInterface)
#endif

#endif // DRIVERINTERFACE_H
