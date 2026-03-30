#include "auto_pam.h"
#include <QApplication>
#include <QDebug>
#include "globals.h"

AUTO_PAM::AUTO_PAM(QObject *parent) : QObject(parent)
{
    smSize = 8 + 1 + 4 * (pamNameList.count() - 1);

    createSM();

    //createPamXml();

    createPams();
}

AUTO_PAM::~AUTO_PAM()
{
    delete sm;
}

void AUTO_PAM::counterHandle(int index, QString type, int arg)
{
    if(index > 5 || index < 0)
        return;
    quint32 ctrValue = counter[index];

    int typeIndex = calTypeList.indexOf(type);
    if(typeIndex == -1)
        return;


    switch (typeIndex) {
    case 0:
    {
        ctrValue += arg;
        break;
    }
    case 1:
    {
        ctrValue -= arg;
        break;
    }
    case 2:
    {
        ctrValue *= arg;
        break;
    }
    case 3:
    {
        ctrValue /= arg;
        break;
    }
    case 4:
    {
        ctrValue %= arg;
        break;
    }
    case 5:
    {
        ctrValue ++;
        break;
    }
    case 6:
    {
        ctrValue --;
        break;
    }
    case 7:
    {
        ctrValue = arg;
        break;
    }
    case 8:
    {
        ctrValue *= arg;
        break;
    }
    case 9:
    {
        ctrValue *= arg;
        break;
    }
    case 10:
    {
        ctrValue /= arg;
        break;
    }
    default:
        break;
    }

    counter[index] = ctrValue;
    updatePamInSM(25 + index*4, 4, (char*)counter + index*4);
}

void AUTO_PAM::setAutoRun(quint8 autoRun)
{
    m_autoRun = autoRun;
    updatePamInSM(8, 1, (char*)&m_autoRun);

    pamList[1]->setValue(autoRun);

    emit autoRunChanged();
}

void AUTO_PAM::setStartStep(quint32 startStep)
{
    m_startStep = startStep;
    updatePamInSM(9, 4, (char*)&m_startStep);

    pamList[2]->setValue(startStep);

    emit startStepChanged();
}

void AUTO_PAM::setEndStep(quint32 endStep)
{
    m_endStep = endStep;
    updatePamInSM(13, 4, (char*)&m_endStep);

    pamList[3]->setValue(endStep);

    emit endStepChanged();
}

void AUTO_PAM::setCurStep(quint32 curStep)
{
    m_curStep = curStep;
    updatePamInSM(17, 4, (char*)&m_curStep);

    pamList[4]->setValue(curStep);

    emit curStepChanged();
}

void AUTO_PAM::setNextStep(quint32 nextStep)
{
    m_nextStep = nextStep;
    updatePamInSM(21, 4, (char*)&m_nextStep);

    pamList[5]->setValue(nextStep);

    emit nextStepChanged();
}

void AUTO_PAM::setCounter0(quint32 counter0)
{
    counter[0] = counter0;
    updatePamInSM(25, 4, (char*)counter);

    pamList[6]->setValue(counter0);

    emit counter0Changed();
}

void AUTO_PAM::setCounter1(quint32 counter1)
{
    counter[1] = counter1;
    updatePamInSM(29, 4, (char*)counter+4);

    pamList[7]->setValue(counter1);

    emit counter1Changed();
}

void AUTO_PAM::setCounter2(quint32 counter2)
{
    counter[2] = counter2;
    updatePamInSM(33, 4, (char*)counter+8);

    pamList[8]->setValue(counter2);

    emit counter2Changed();
}

void AUTO_PAM::setCounter3(quint32 counter3)
{
    counter[3] = counter3;
    updatePamInSM(37, 4, (char*)counter+12);

    pamList[9]->setValue(counter3);

    emit counter3Changed();
}

void AUTO_PAM::setCounter4(quint32 counter4)
{
    counter[4] = counter4;
    updatePamInSM(41, 4, (char*)counter+16);

    pamList[10]->setValue(counter4);

    emit counter4Changed();
}

void AUTO_PAM::setCounter5(quint32 counter5)
{
    counter[5] = counter5;
    updatePamInSM(45, 4, (char*)counter+20);

    pamList[11]->setValue(counter5);

    emit counter5Changed();
}

void AUTO_PAM::setAutoProgLevel(quint32 autoProgLevel)
{
    m_autoProgLevel = autoProgLevel;
    updatePamInSM(49, 4, (char*)&m_autoProgLevel);

    pamList[12]->setValue(autoProgLevel);

    emit autoProgLevelChanged();
}


void AUTO_PAM::createSM()
{
    if(sm == NULL)
    {
        sm = new QSharedMemory(AUTO_SMKEY);
    }
    sm->create(smSize);
}

void AUTO_PAM::createPams()
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
    timePam->smKey = AUTO_SMKEY;
    timePam->smSize = 200;

    pamList.append(timePam);

    quint32 startBit = 64;
    quint32 startByte = 8;
    int type = 0;
    qreal min = 0;
    qreal max = UINT32_MAX;
    quint32 len = 32;

    for(int i = 0; i < pamNameList.count(); i++)
    {
        QString pamName = pamNameList.at(i);

        PARAM *pam = new PARAM();

        if(i == 0)
        {
            max = UINT8_MAX;
            len = 8;
        }
        else
        {
            max = UINT32_MAX;
            len = 32;
        }

        pam->Name = pamName;
        pam->ShortName = pamName;
        pam->LongIdentifier = pamName;

        pam->ScalingFactor = 1;
        pam->ScalingOffset = 0;

        pam->LowerLimit = min;
        pam->UpperLimit = max;

        pam->Type = type;  // 0 = uint()
        pam->StartBit = startBit;
        pam->LengthBit = len;
        pam->ByteOrder = 0;  // 1 = moto

        pam->StartByte = startByte;

        pam->smKey = AUTO_SMKEY;
        pam->smSize = 200;

        pamList.append(pam);

        startBit += len;
        startByte += (len/8);
    }


    //Globals::funWritePamListHash.insert(AUTO_SMKEY, pamList);

    //foreach(PARAM *pam, pamList) {
    //    Globals::pamHash.insert(pam->Name + "." + AUTO_SMKEY, pam);
    //}
}

bool AUTO_PAM::createPamXml()
{
    QString FilePath = QApplication::applicationDirPath();
    QString FileName = "";
    FileName.append(AUTO_SMKEY);
    FileName.append(".xml");
    QString xmlPath = FilePath + "/pamFactory/" + FileName;

    if (xmlPath.isEmpty())
        return false;

    QFile xmlFile(xmlPath);
    if (!xmlFile.open(QFile::WriteOnly | QFile::Text))
    {
        qDebug()<<tr("Cannot write file %1:\n%2.").arg(xmlPath).arg(xmlFile.errorString());
        return false;
    }

    QDomDocument xml("AUTO");

    QDomElement root = xml.createElement("Root");

    xml.appendChild(root);


    int sigIndex = 0;
    QDomElement WriteSigElem = xml.createElement(AUTO_SMKEY);
    root.appendChild(WriteSigElem);

    // add time channel
    QDomElement timeElem_w = xml.createElement("param_"+QString::number(sigIndex));
    WriteSigElem.appendChild(timeElem_w);
    timeElem_w.setAttribute("unit", "s");
    timeElem_w.setAttribute("value_default", QString::number(0,10));
    timeElem_w.setAttribute("minimum", QString::number(0, 'g', 6));
    timeElem_w.setAttribute("maximum", QString::number(__DBL_MAX__, 'g', 6));
    timeElem_w.setAttribute("scalingoffset", QString::number(0));
    timeElem_w.setAttribute("scalingfactor", QString::number(1));
    timeElem_w.setAttribute("lengthbit", QString::number(64, 10));
    timeElem_w.setAttribute("startbit", QString::number(0, 10));
    timeElem_w.setAttribute("startbit_oth", QString::number(0, 10));
    timeElem_w.setAttribute("byteorder", QString::number(0,10));
    timeElem_w.setAttribute("type", QString::number(3, 10));
    timeElem_w.setAttribute("param_name", "Time");

    sigIndex++;

    quint32 startBit = 64;

    int type = 1;
    qreal min = 0;
    qreal max = UINT32_MAX;
    quint32 len = 32;
    int index = 0;

    for(int j = 0; j < pamNameList.count(); j++)
    {
        if(j == 0)
        {
            max = UINT8_MAX;
            len = 8;
        }
        else
        {
            max = UINT32_MAX;
            len = 32;
        }
        QDomElement sigElem = xml.createElement("param_"+QString::number(sigIndex));
        WriteSigElem.appendChild(sigElem);

        sigElem.setAttribute("unit", "");
        sigElem.setAttribute("value_default", QString::number(0, 'g', 6));
        sigElem.setAttribute("minimum", QString::number(min, 'g', 6));
        sigElem.setAttribute("maximum", QString::number(max, 'g', 6));
        sigElem.setAttribute("scalingoffset", QString::number(0, 'g', 6));
        sigElem.setAttribute("scalingfactor", QString::number(1, 'g', 6));
        sigElem.setAttribute("lengthbit", QString::number(len, 10));
        sigElem.setAttribute("startbit", QString::number(startBit, 10));
        sigElem.setAttribute("byteorder", QString::number(0,10));
        sigElem.setAttribute("type", QString::number(type, 10));
        sigElem.setAttribute("param_name", pamNameList.at(j));

        sigIndex++;
        startBit += len;
        index++;
    }
    WriteSigElem.setAttribute("size_bytes", smSize);
    WriteSigElem.setAttribute("sample_rate", QString::number(0,10));
    WriteSigElem.setAttribute("is_read", QString::number(0,10));


    QString xmlString = xml.toString();
    QTextStream out(&xmlFile);
    out << xmlString;

    xmlFile.close();

    return true;
}

bool AUTO_PAM::updatePamInSM(int startByte, int byteSize, char *pamData)
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

QList<PARAM *> AUTO_PAM::getPamList() const
{
    return pamList;
}

QString AUTO_PAM::getSmKey() const
{
    return smKey;
}


