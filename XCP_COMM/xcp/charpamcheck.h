#ifndef CHARPAMCHECK_H
#define CHARPAMCHECK_H

#include <QThread>
#include <QMutex>
#include <QWaitCondition>

#include "xcpmaster.h"
#include "a2l_varchar.h"
#include "common/smart_ptr.h"


class CharPamCheck : public QThread
{
    Q_OBJECT
public:
    CharPamCheck(QObject *parent = 0, XCPMaster *master = 0);
    ~CharPamCheck();

    void setCharPamList(QList<A2L_VarChar*> charList);
    void setCharCheckRunFlag(bool runFlag);
    void setSMKey(QString smKeyWrite);

    bool charArrayEqual(char *one, char *two, int size);

    void setSmWriteSize(const quint32 &value);

    void setIsStop(bool value);
    void stop();  // 新增：安全停止线程

protected:
    void run();

signals:
    void addCaliAction(QVariant);

private:
    XCPMaster *xcpMaster = NULL;
    QList<A2L_VarChar*> charPamList;

    bool isStop = false;
    bool charCheckRunFlag = false;
    int checkRate_ms = 1;

    QString smKeyWrite = "";
    quint32 smWriteSize= 0;

    // 线程同步机制
    QMutex m_mutex;
    QWaitCondition m_condition;
    bool m_running = true;

};


class MapCharPamCheckThread : public QThread
{
    Q_OBJECT
public:
    MapCharPamCheckThread(QObject *parent = 0, XCPMaster *master = 0);
    ~MapCharPamCheckThread();

    void setMapCharPamList(QList<A2L_VarChar*> mapCharList);
    void setMapCharSMHash(QHash<A2L_VarChar*, QSharedMemory*> mapSMHash);
    void setMapCharCheckRunFlag(bool runFlag);
    void setMapSMKey(QString smKeyMapWrite);

    bool charArrayEqual(char *one, char *two, int size);

    void setIsStop(bool value);
    void stop();  // 新增：安全停止线程

protected:
    void run();

signals:
    void addMapCaliAction(QVariant);

private:
    XCPMaster *xcpMaster = NULL;

    QList<A2L_VarChar*> mapCharPamList;
    QHash<A2L_VarChar*, QSharedMemory*> mapSMHash;

    QList<CharArrayPtr> buffer_last;
    QList<CharArrayPtr> buffer_new;
    QHash<A2L_VarChar*, CharArrayPtr> bufferHash_last;
    QHash<A2L_VarChar*, CharArrayPtr> bufferHash_new;

    bool isStop = false;
    bool mapCharCheckRunFlag = false;
    int checkRate_ms = 5;

    // 线程同步机制
    QMutex m_mutex;
    QWaitCondition m_condition;
    bool m_running = true;

};

#endif // CHARPAMCHECK_H
