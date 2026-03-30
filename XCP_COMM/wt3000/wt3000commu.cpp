#include "wt3000commu.h"
#include <QDebug>


void WT3000Commu::DRV_Init()
{
    if (window==0)
    {
        window=new MainWindow;
    }
    else
    {
        qInfo()<<Q_FUNC_INFO<<"Dll WT3000Commu is already initialized.";
    }
}

void WT3000Commu::DRV_Show()
{
    if(window!=0)
    {
        window->show();
        if(window->isMinimized())
            window->showNormal();
    }
    else
    {
        qInfo()<<Q_FUNC_INFO<<"Init the WT3000Commu dll first,then show dll";
    }

}

void WT3000Commu::DRV_Stop()
{
    if(window!=0)
    {
        window->hide();
        delete window;
        window=0;
    }
    else
    {
        qInfo()<<Q_FUNC_INFO<<"Dll WT3000Commu is already deleted.";
    }
}


