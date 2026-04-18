#ifndef WT3000_THREAD_H
#define WT3000_THREAD_H
#include <QThread>
#include <QSharedMemory>
#include "common.h"
#include "common/smart_ptr.h"

class WT3000_Thread : public QThread
{
    Q_OBJECT
public:
    WT3000_Thread();
    ~WT3000_Thread();
    bool connectDevice();
    bool disconnectDevice();
    void setIPAddress(QString address);

    void setFunInfo(FunStruct *value, int index);

    void setStopFlg(bool value);

    int getWtState() const;

    QList<PARAM *> getReadPamList() const;

    QString getSmKey() const;

protected:
    void run();

public slots:
    void wtDataUpdatedSlot();

signals:
    void reconnect();
    void wtFunStateChanged(int, int); // -1 = error, 0 = stop, 1 = init, 2 = setup, 3 = wtRun
    void wtDataForRecord(ByteArrayPtr,quint32,QString);
    void wtDataUpdated();

private:
    void init();
    void endHandle();

    void createSM();
    void deleteSM();

    void writeSetting();
    void readSetting();

    void createPams();

    void reverseData_4bytes(char *src, char *dst);

private:
    FunStruct *fun = NULL;
    int funIndex = -1;

    QString IPAddress;
    bool stopFlg=true;
    uint m_periodms=10;
    int sta;
    int deviceID;
    int rlen;

    QString smKey = "RP_WT3000";
    QSharedMemory* sm_wt3000 = NULL;
    int wtState = 0;

    int pamCount = 48;
    QStringList pamNameList;

    QList<PARAM*> readPamList;

};

#endif // WT3000_THREAD_H
