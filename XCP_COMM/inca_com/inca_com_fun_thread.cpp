#include "inca_com_fun_thread.h"
#include <QDebug>
#include <QtCore/qmath.h>
#include <QCoreApplication>

INCA_COM_Fun_Thread::INCA_COM_Fun_Thread(QObject *parent)
{

}

INCA_COM_Fun_Thread::~INCA_COM_Fun_Thread()
{
    disconnectFromINCA();
}

void INCA_COM_Fun_Thread::setFunInfo(FunStruct *value, int index)
{
    fun = value;
    funIndex = index;
}

bool INCA_COM_Fun_Thread::connectToINCA()
{
    init();

    if(!incaCom->connectToINCA())
    {
        qWarning()<<"Connect to INCA fail";
        emit funStateChanged(-1, funIndex);

        return false;
    }

    //QTimer *timer = new QTimer();
    //timer->setInterval(readInterval);
    //connect(timer, SIGNAL(timeout()), this, SLOT(Slt_FrameSigValueUpdated()));
    //timer->start();

    emit funStateChanged(3, funIndex);

    return true;
}

void INCA_COM_Fun_Thread::disconnectFromINCA()
{
    incaConnected = false;

    timeStamp_on = 0;
    timeStamp_start = 0;

    if(incaCom)
    {
        incaCom->disconnectFromINCA();

        delete incaCom;
        incaCom = NULL;

        qDebug()<<"=====del inca com=====";

        emit funStateChanged(0, funIndex);
    }

    qDeleteAll(readPamList);
    readPamList.clear();
    readPamListHash.clear();

    qDeleteAll(writePamList);
    writePamList.clear();
    writePamListHash.clear();

}


void INCA_COM_Fun_Thread::init()
{
    if(incaCom == NULL)
    {
        incaCom = new INCACOMM();
    }

    loadXml();

    emit funStateChanged(1, funIndex);

}

void INCA_COM_Fun_Thread::wait_ms(int msec)
{
    QTime dieTime = QTime::currentTime().addMSecs(msec);
    while( QTime::currentTime() < dieTime )
        QCoreApplication::processEvents(QEventLoop::AllEvents, 100);
}

bool INCA_COM_Fun_Thread::loadXml()
{
    readPamNameList.clear();
    writePamNameList.clear();

    if(fun->setFilePath.isEmpty())
        return false;

    QString xmlPath = QCoreApplication::applicationDirPath() + fun->setFilePath;
    qDebug()<<"Load xml file:"<<xmlPath;

    QFile xmlFile(xmlPath);
    if (!xmlFile.open(QFile::ReadOnly | QFile::Text))
    {
        qDebug()<<tr("Cannot read file %1:\n%2.").arg(xmlPath).arg(xmlFile.errorString());
        return false;
    }

    QDomDocument xml("INCA_COM");

    if (!xml.setContent(&xmlFile))
    {
        xmlFile.close();
        return false;
    }
    xmlFile.close();

    QDomElement root = xml.documentElement();


    QString modName = "INCA_COM";


    return true;
}

INCACOMM *INCA_COM_Fun_Thread::getIncaCom() const
{
    return incaCom;
}


QHash<QString, QList<PARAM *> > INCA_COM_Fun_Thread::getWritePamListHash() const
{
    return writePamListHash;
}

QHash<QString, QList<PARAM *> > INCA_COM_Fun_Thread::getReadPamListHash() const
{
    return readPamListHash;
}
