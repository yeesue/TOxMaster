#ifndef DB_TESTBED_H
#define DB_TESTBED_H

#include <QObject>
#include <QtSql>

#include "globals.h"
#include "a2l_varchar.h"
#include "a2l_varmeas.h"


class DB_Testbed : public QObject
{
    Q_OBJECT
public:
    explicit DB_Testbed(QObject *parent = 0);
    ~DB_Testbed();

    bool openDB();
    void closeDB();

public slots:    

    QList<A2L_VarChar*> getCharPams(QString smKeyWrite, quint64 &size);
    QList<A2L_VarMeas*> getMeasPams(QString smKeyRead, quint64 &size);

    bool saveCharPams(QList<A2L_VarChar*> charPamList);
    bool saveMeasPams(QList<A2L_VarMeas*> measPamList);


signals:

public slots:

private:
    QSqlDatabase db;

};

#endif // DB_TESTBED_H
