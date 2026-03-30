#include "wt5000commu.h"
#include <QDebug>


void WT5000Commu::DRV_Init()
{
    if (window==0)
    {
        window=new MainWindow;
    }
    else
    {
        qInfo()<<Q_FUNC_INFO<<"Dll WT5000Commu is already initialized.";
    }
}

void WT5000Commu::DRV_Show()
{
    if(window!=0)
    {
        window->show();
        if(window->isMinimized())
            window->showNormal();
    }
    else
    {
        qInfo()<<Q_FUNC_INFO<<"Init the WT5000Commu dll first,then show dll";
    }

}

void WT5000Commu::DRV_Stop()
{
    if(window!=0)
    {
        window->hide();
        delete window;
        window=0;
    }
    else
    {
        qInfo()<<Q_FUNC_INFO<<"Dll WT5000Commu is already deleted.";
    }
}


