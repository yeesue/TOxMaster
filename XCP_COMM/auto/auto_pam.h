#ifndef AUTO_PAM_H
#define AUTO_PAM_H

#include <QObject>
#include <QSharedMemory>
#include <QDomDocument>
#include <QFile>
#include <QtMath>
#include "param.h"

#define  AUTO_SMKEY   "WP_AUTO"

class AUTO_PAM : public QObject
{
    Q_OBJECT

    Q_PROPERTY(quint8 autoRun READ autoRun WRITE setAutoRun NOTIFY autoRunChanged)
    Q_PROPERTY(quint32 startStep READ startStep WRITE setStartStep NOTIFY startStepChanged)
    Q_PROPERTY(quint32 endStep READ endStep WRITE setEndStep NOTIFY endStepChanged)
    Q_PROPERTY(quint32 curStep READ curStep WRITE setCurStep NOTIFY curStepChanged)
    Q_PROPERTY(quint32 nextStep READ nextStep WRITE setNextStep NOTIFY nextStepChanged)
    Q_PROPERTY(quint32 counter0 READ counter0 WRITE setCounter0 NOTIFY counter0Changed)
    Q_PROPERTY(quint32 counter1 READ counter1 WRITE setCounter1 NOTIFY counter1Changed)
    Q_PROPERTY(quint32 counter2 READ counter2 WRITE setCounter2 NOTIFY counter2Changed)
    Q_PROPERTY(quint32 counter3 READ counter3 WRITE setCounter3 NOTIFY counter3Changed)
    Q_PROPERTY(quint32 counter4 READ counter4 WRITE setCounter4 NOTIFY counter4Changed)
    Q_PROPERTY(quint32 counter5 READ counter5 WRITE setCounter5 NOTIFY counter5Changed)
    Q_PROPERTY(quint32 autoProgLevel READ autoProgLevel WRITE setAutoProgLevel NOTIFY autoProgLevelChanged)
public:
    explicit AUTO_PAM(QObject *parent = nullptr);
    ~AUTO_PAM();


    QString getSmKey() const;

    QList<PARAM *> getPamList() const;

public slots:
    void counterHandle(int index, QString type, int arg);

    quint8 autoRun() const {return m_autoRun;}
    quint32 startStep() const {return m_startStep;}
    quint32 endStep() const {return m_endStep;}
    quint32 curStep() const {return m_curStep;}
    quint32 nextStep() const {return m_nextStep;}
    quint32 counter0() const {return counter[0];}
    quint32 counter1() const {return counter[1];}
    quint32 counter2() const {return counter[2];}
    quint32 counter3() const {return counter[3];}
    quint32 counter4() const {return counter[4];}
    quint32 counter5() const {return counter[5];}
    quint32 autoProgLevel() const {return m_autoProgLevel;}

    void setAutoRun(quint8 autoRun);
    void setStartStep(quint32 startStep);
    void setEndStep(quint32 endStep);
    void setCurStep(quint32 curStep);
    void setNextStep(quint32 nextStep);
    void setCounter0(quint32 counter0);
    void setCounter1(quint32 counter1);
    void setCounter2(quint32 counter2);
    void setCounter3(quint32 counter3);
    void setCounter4(quint32 counter4);
    void setCounter5(quint32 counter5);
    void setAutoProgLevel(quint32 autoProgLevel);


signals:
    void autoRunChanged();
    void startStepChanged();
    void endStepChanged();
    void curStepChanged();
    void nextStepChanged();
    void counter0Changed();
    void counter1Changed();
    void counter2Changed();
    void counter3Changed();
    void counter4Changed();
    void counter5Changed();
    void autoProgLevelChanged();

private:
    void createSM();
    void createPams();
    bool createPamXml();

    bool updatePamInSM(int startByte, int byteSize, char *pamData);

private:
    QSharedMemory *sm = NULL;
    int smSize = 0;
    QString smKey = AUTO_SMKEY;

    QStringList pamNameList = {"AutoRun", "StartStep", "EndStep", "CurStep", "NextStep",
                               "Counter0", "Counter1", "Counter2", "Counter3", "Counter4", "Counter5",
                               "AutoProgLevel",
                              };
    QStringList calTypeList = {"+", "-", "x", "/", "%", "++", "--", "=", "*", "X"};

    quint8 m_autoRun = 0;
    quint32 m_startStep = 0;
    quint32 m_endStep = 0;
    quint32 m_curStep = 0;
    quint32 m_nextStep = 0;
    quint32 counter[6];
    quint32 m_autoProgLevel = 1;

    QList<PARAM*> pamList;



};

#endif // AUTO_PAM_H
