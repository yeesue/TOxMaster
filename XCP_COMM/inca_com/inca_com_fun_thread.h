#ifndef INCA_COM_FUN_THREAD_H
#define INCA_COM_FUN_THREAD_H

#include <QThread>
#include <QDomDocument>
#include "param.h"
#include "incacomm.h"
#include "common.h"

class  INCA_COM_Fun_Thread : public QThread
{
    Q_OBJECT
public:
    INCA_COM_Fun_Thread(QObject *parent = 0);
    ~INCA_COM_Fun_Thread();

    void setFunInfo(FunStruct *value, int index);

    QHash<QString, QList<PARAM *> > getReadPamListHash() const;

    QHash<QString, QList<PARAM *> > getWritePamListHash() const;

    INCACOMM *getIncaCom() const;

public slots:
    bool connectToINCA();
    void disconnectFromINCA();

signals:
    void funStateChanged(int state, int funIndex);

private slots:


private:
    void init();

    void wait_ms(int msec);

    bool loadXml();


private:
    FunStruct *fun = NULL;
    int funIndex = -1;
    quint32 curState = 0;

    INCACOMM *incaCom = NULL;

    quint16 readInterval = 100;

    QStringList readPamNameList;
    QStringList writePamNameList;

    QList<PARAM*> readPamList;
    QList<PARAM*> writePamList;

    QHash<QString, QList<PARAM*>> readPamListHash;
    QHash<QString, QList<PARAM*>> writePamListHash;


    bool incaConnected = false;

    quint64 timeStamp_on = 0;
    quint64 timeStamp_start = 0;
};

#endif // INCA_COM_FUN_THREAD_H
