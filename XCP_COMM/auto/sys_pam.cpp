#include "sys_pam.h"
#include <QApplication>
#include <QDebug>
#include "globals.h"

SYS_PAM::SYS_PAM(QObject *parent) : QObject(parent)
{
    smSize = 8 + 8*pamNum;

    createSM();

    createPams();
}

SYS_PAM::~SYS_PAM()
{
    delete sm;
}


void SYS_PAM::createSM()
{
    if(sm == NULL)
    {
        sm = new QSharedMemory(SYS_SMKEY);
    }
    sm->create(smSize);
}

void SYS_PAM::createPams()
{
    pamList.clear();

    PARAM *timePam = new PARAM();
    timePam->Unit = "s";
    timePam->DefaultValue = 0;
    timePam->LowerLimit = 0;
    timePam->UpperLimit = __DBL_MAX__;
    timePam->ScalingFactor = 1;
    timePam->ScalingOffset = 0;
    timePam->LengthBit = 64;
    timePam->StartBit = 0;
    timePam->ByteOrder = 0;
    timePam->Type = 3;
    timePam->Name = "Time";
    timePam->smKey = SYS_SMKEY;
    timePam->smSize = smSize;

    pamList.append(timePam);

    for(int i = 0; i < pamNum; i++)
    {
        QString pamName = "SYS_DB[" + QString::number(i) +"]" ;

        PARAM *pam = new PARAM();

        pam->Name = pamName;
        pam->ShortName = pamName;
        pam->LongIdentifier = pamName;

        pam->ScalingFactor = 1;
        pam->ScalingOffset = 0;

        pam->LowerLimit = -__DBL_MAX__;
        pam->UpperLimit = __DBL_MAX__;

        pam->Type = 3;  // 3 = double
        pam->StartBit = 64 * (i + 1);
        pam->LengthBit = 64;
        pam->ByteOrder = 0;  // 1 = moto

        pam->StartByte = 8 * (i + 1);

        pam->smKey = SYS_SMKEY;
        pam->smSize = smSize;

        pamList.append(pam);

    }

    //Globals::funWritePamListHash.insert(SYS_SMKEY, pamList);

    //foreach(PARAM *pam, pamList) {
    //    Globals::pamHash.insert(pam->Name + "." + SYS_SMKEY, pam);
    //}
}

bool SYS_PAM::updatePamInSM(int startByte, int byteSize, char *pamData)
{
    if(sm == NULL)
        return false;
    if (!sm->isAttached())
    {
        if (!sm->attach())
        {
            //qDebug()<<"Unable to attach auto sharedmemory";
            return false;
        }
    }

    sm->lock();

    char *from = pamData;
    char *to = (char*)sm->data() + startByte;
    memcpy(to, from, byteSize);

    sm->unlock();
}

QList<PARAM *> SYS_PAM::getPamList() const
{
    return pamList;
}

QString SYS_PAM::getSmKey() const
{
    return smKey;
}


