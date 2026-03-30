#ifndef SYS_PAM_H
#define SYS_PAM_H

#include <QObject>
#include <QSharedMemory>
#include <QDomDocument>
#include <QFile>
#include <QtMath>
#include "param.h"

#define  SYS_SMKEY   "WP_SYS"

class SYS_PAM : public QObject
{
    Q_OBJECT

public:
    explicit SYS_PAM(QObject *parent = nullptr);
    ~SYS_PAM();


    QString getSmKey() const;

    QList<PARAM *> getPamList() const;

public slots:


signals:


private:
    void createSM();
    void createPams();

    bool updatePamInSM(int startByte, int byteSize, char *pamData);

private:
    QSharedMemory *sm = NULL;
    int smSize = 0;
    QString smKey = SYS_SMKEY;

    int pamNum = 100;
    QStringList pamNameList;

    QList<PARAM*> pamList;



};

#endif // SYS_PAM_H
