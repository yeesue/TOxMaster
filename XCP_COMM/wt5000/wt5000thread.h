#ifndef WT5000THREAD_H
#define WT5000THREAD_H
#include <QThread>
#include <QSharedMemory>
#include "common.h"

class WT5000Thread : public QThread
{
    Q_OBJECT
public:
    WT5000Thread();
    ~WT5000Thread();
    bool connectDevice();
    bool disconnectDevice();
    void setIPAddress(QString address);

    QSharedMemory* sm_wt5000;
private:
    QString IPAddress;
    bool stopFlg=true;
    uint m_periodms=10;
    int sta;
    int deviceID;
    int rlen;

    void run();

signals:
    void reconnect();
    void rcvUpdated();
};

#endif // WT5000THREAD_H
