#ifndef WT3000THREAD_H
#define WT3000THREAD_H
#include <QThread>
#include <QSharedMemory>
#include "common.h"

class WT3000Thread : public QThread
{
    Q_OBJECT
public:
    WT3000Thread();
    ~WT3000Thread();
    bool connectDevice();
    bool disconnectDevice();
    void setIPAddress(QString address);

    void setFunInfo(FunStruct *value, int index);

    QSharedMemory* sm_wt3000;

signals:
    void reconnect();
    void wtFunStateChanged(int, int); // 0 = stop, 1 = init, 2 = setup, 3 = wtRun


private:
    FunStruct *fun = NULL;
    int funIndex = -1;

    QString IPAddress;
    bool stopFlg=true;
    uint m_periodms=10;
    int sta;
    int deviceID;
    int rlen;

    void run();

};

#endif // WT3000THREAD_H
