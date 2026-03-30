#ifndef WT5000COMMU_H
#define WT5000COMMU_H

#include "driverinterface.h"
#include "win_wt5000.h"
#include <QObject>

class WT5000Commu : public QObject, DriverInterface
{
    Q_OBJECT

    # if QT_VERSION>=0X050000
    Q_PLUGIN_METADATA(IID DriverInterface_iid)
    Q_INTERFACES(DriverInterface)
    # endif //Version >= QT5.0
public:
    virtual void DRV_Init() Q_DECL_OVERRIDE;  // 接口初始化
    virtual void DRV_Show() Q_DECL_OVERRIDE;  // 显示接口界面
    virtual void DRV_Stop() Q_DECL_OVERRIDE;  // 关闭接口
private:
    MainWindow* window=0;
};

#endif // WT5000COMMU_H
