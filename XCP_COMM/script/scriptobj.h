#ifndef SCRIPTOBJ_H
#define SCRIPTOBJ_H

#include <QObject>
#include <QScriptEngine>
#include <QTimer>
#include "globals.h"

class ScriptTimer : public QTimer
{
    Q_OBJECT
public:
    Q_INVOKABLE ScriptTimer(QObject *parent = 0);
    ~ScriptTimer();

signals:


public slots:
    void setInterval(int msec);

    void start(int msec);
    void start();
};

class ScriptObj : public QObject
{
    Q_OBJECT
public:
    explicit ScriptObj(QObject *parent = nullptr, QScriptEngine *scriptEngine = 0);
    ~ScriptObj();

    void setScriptFilePath(QString filePath);

public slots:
    void wait(quint32 msec);
    void msleep(quint32 msec);

    QScriptValue createTimer();

    void log(QString msg);
    void logDoubleValue(double value);

    bool setSMPamValue(QString smKey, QString pamName, qreal value);
    qreal getSMPamValue(QString smKey, QString pamName);
    QString getSMPamValueStr(QString smKey, QString pamName);

    bool setPamValue(QString modName, QString pamName, qreal value);
    qreal getPamValue(QString modName, QString pamName);
    QString getPamValueStr(QString modName, QString pamName);

    bool setMapPamValue(QString modName, QString pamName, int index_x, int index_y, qreal value);

private slots:
    void transferPhyValueToRawData(A2L_VarChar *charVar, double value, char *data);
    void updateMapValueInSM(A2L_VarChar *map, quint16 index_x, quint16 index_y, char *data);

signals:
    void logMsg(QString msg);
    void logClear();

private:

    QScriptEngine *engine = NULL;
    QString scriptFilePath = "";

    QStringList dataTypeEnum = {"UBYTE", "SBYTE", "UWORD", "SWORD",
                               "ULONG", "SLONG", "A_UINT64", "A_INT64",
                               "FLOAT32_IEEE", "FLOAT64_IEEE"};


};

#endif // SCRIPTOBJ_H
