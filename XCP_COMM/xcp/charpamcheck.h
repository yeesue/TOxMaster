#ifndef CHARPAMCHECK_H
#define CHARPAMCHECK_H

#include <QThread>

#include "xcpmaster.h"
#include "a2l_varchar.h"


class CharPamCheck : public QThread
{
    Q_OBJECT
public:
    CharPamCheck(QObject *parent = 0, XCPMaster *master = 0);

    void setCharPamList(QList<A2L_VarChar*> charList);
    void setCharCheckRunFlag(bool runFlag);
    void setSMKey(QString smKeyWrite);

    bool charArrayEqual(char *one, char *two, int size);

    void setSmWriteSize(const quint32 &value);

    void setIsStop(bool value);

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

};


class MapCharPamCheckThread : public QThread
{
    Q_OBJECT
public:
    MapCharPamCheckThread(QObject *parent = 0, XCPMaster *master = 0);

    void setMapCharPamList(QList<A2L_VarChar*> mapCharList);
    void setMapCharSMHash(QHash<A2L_VarChar*, QSharedMemory*> mapSMHash);
    void setMapCharCheckRunFlag(bool runFlag);
    void setMapSMKey(QString smKeyMapWrite);

    bool charArrayEqual(char *one, char *two, int size);

    void setIsStop(bool value);

protected:
    void run();

signals:
    void addMapCaliAction(QVariant);

private:
    XCPMaster *xcpMaster = NULL;

    QList<A2L_VarChar*> mapCharPamList;
    QHash<A2L_VarChar*, QSharedMemory*> mapSMHash;

    QList<char*> buffer_last;
    QList<char*> buffer_new;
    QHash<A2L_VarChar*, char*> bufferHash_last;
    QHash<A2L_VarChar*, char*> bufferHash_new;

    bool isStop = false;
    bool mapCharCheckRunFlag = false;
    int checkRate_ms = 5;

};

#endif // CHARPAMCHECK_H
