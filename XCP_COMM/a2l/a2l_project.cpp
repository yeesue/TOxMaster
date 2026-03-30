#include "a2l_project.h"
#include <QDebug>


A2L_Project::A2L_Project(QObject *parent)
{

}

A2L_Project::A2L_Project(QString fullFileName, A2L_PARSER_INTERFACE *a2lParserRef) :
                        a2lFullFileName(fullFileName), a2lParser(a2lParserRef)
{
    parsedStatus = a2lParser->loadFile_A2LParser(a2lFullFileName, infoList);
    if (!parsedStatus)
    {
        qDebug()<<"A2L File Prased Error.";
    }
    else
    {
        qDebug()<<"=======A2L File Prased:"<<a2lFullFileName<<"=======";
        getA2LXcpOnCanListInfo();

        getA2LDaqInfo(0);
        getA2LDaqMemInfo(0);
        getA2LEventListInfo(0);

        if (a2lDaq == NULL)
            getA2LDaqInfo();
        if (a2lDaqMem == NULL)
            getA2LDaqMemInfo();
        if (a2lEventList.isEmpty())
            getA2LEventListInfo();

        getA2LEventCanIdListInfo();
        getA2LDaqListListInfo();
        getA2LDaqListCanIdListInfo();
        getA2LMemSegListInfo();

        allMeasVarNameList = a2lParser->getMeasNameList(a2lFullFileName);
        allCharVarNameList = a2lParser->getCharNameList(a2lFullFileName);
    }
}

void A2L_Project::setFullFileName(QString fullFileName)
{
    this->a2lFullFileName = fullFileName;
}

QString A2L_Project::getA2LFileName()
{
    return a2lFullFileName;
}

void A2L_Project::setA2LParser(A2L_PARSER_INTERFACE *a2lParserRef)
{
    this->a2lParser = a2lParserRef;
}

bool A2L_Project::getParsedStatus()
{
    return parsedStatus;
}

QStringList A2L_Project::getParsedInfo()
{
    return infoList;
}

bool A2L_Project::getHexPrasedStatus()
{
    return hexParsedStatus;
}

QString A2L_Project::getA2LRefHexFileName()
{
    return a2lFullHexName;
}

QStringList A2L_Project::getEventNameList()
{
    return eventNameList;
}

QStringList A2L_Project::getEventTimeList()
{
    QStringList eventTimeList;
    if (a2lEventList.isEmpty())
        return eventTimeList;

    for (int i = 0; i < a2lEventList.count(); i++)
    {
        A2L_Event *event = a2lEventList.at(i);
        qDebug()<<"time cycle="<<event->TimeCycle<<",time unit="<<event->TimeUnit;
        QString timeStr;
        if (event->TimeUnit == 6)
        {
            timeStr = QString::number(event->TimeCycle * 1) + "ms";
        }
        else if (event->TimeUnit == 5)
        {
            timeStr = QString::number((float)event->TimeCycle * 0.1, 'g', 6) + "ms";
        }
        else if (event->TimeUnit == 7)
        {
            timeStr = QString::number((float)event->TimeCycle * 10, 'g', 6) + "ms";
        }
        else if (event->TimeUnit == 8)
        {
            timeStr = QString::number((float)event->TimeCycle * 100, 'g', 6) + "ms";
        }
        else
        {
            timeStr = "Undefined";
        }
        eventTimeList.append(timeStr);
    }
    return eventTimeList;
}

QStringList A2L_Project::getA2LAllMeasVarNameList()
{
    return allMeasVarNameList;
}

QStringList A2L_Project::getA2LAllCharVarNameList()
{
    return allCharVarNameList;
}

void A2L_Project::addSelMeasVarList(QStringList measVarNameList, QStringList measVarRateList)
{
    this->selMeasVarNameList.append(measVarNameList);

    int measVarCount = measVarNameList.count();
    int measVarRateCount = measVarRateList.count();
    if (measVarCount != measVarRateCount)
        return;

    for (int i = 0; i < measVarCount; i++)
    {
        QString measVarName = measVarNameList.at(i);
        QString measVarRate = measVarRateList.at(i);
        qDebug()<<"Selected Meas "<<i;

        A2L_VarMeas *measVar = getMeasVarByNameAndRate(measVarName, measVarRate);
        if (measVar != NULL)
        {
            selMeasVarList.append(measVar);
            qDebug()<<"Meas Var Name="<<measVar->Name;
            qDebug()<<"Meas Var Rate ms="<<measVar->rate_ms;

            updateEventRelatedMeasSize(measVar, 1);
        }
    }
    qDebug()<<"Current SelMeasVarList Count="<<selMeasVarList.count();


}

void A2L_Project::addSelCharVarList(QStringList charVarNameList)
{
    this->selCharVarNameList.append(charVarNameList);
}

void A2L_Project::removeMeasVarFromSelMeasVarList(A2L_VarMeas *measVar)
{
    if (measVar == NULL || selMeasVarList.count() == 0)
        return;
    if (getCurProXcpDaqRunningStatus())
        return;

    selMeasVarList.removeOne(measVar);
    delete measVar;

    updateEventRelatedMeasSize(measVar, 0);

    xcpMaster->setMeasVarListForDAQ(selMeasVarList);
    qDebug()<<"After Remove, Current SelMeasVarList Count="<<selMeasVarList.count();
}

void A2L_Project::updateEventRelatedMeasSize(A2L_VarMeas *measVar, quint8 addOrDel)
{
    QString eventName = measVar->daqEventRef->ShortName;
    quint32 sizeSum = eventNameSelMeasSizeSumHash.value(eventName);
    //quint32 sizeSum = eventSelMeasSizeSumHash.value(measVar->daqEventRef);
    quint32 newSizeSum = 0;
    if (addOrDel == 1)
        newSizeSum = sizeSum + measVar->DataSizeAG;
    else if (addOrDel == 0)
        newSizeSum = sizeSum - measVar->DataSizeAG;
    //eventSelMeasSizeSumHash.insert(measVar->daqEventRef, newSizeSum);
    eventNameSelMeasSizeSumHash.insert(eventName, newSizeSum);
    qDebug()<<"Event="<<eventName<<", newSizeSum="<<newSizeSum;
}

QHash<QString, quint32> A2L_Project::getEventCurSizeHash()
{
    return eventNameSelMeasSizeSumHash;
}

QHash<QString, quint32> A2L_Project::getEventMaxSizeHash()
{
    return eventNameSizeMaxHash;
}

QHash<QString, qreal> A2L_Project::getEventRateHash()
{
    return eventNameRateHash;
}

QList<A2L_VarMeas *> A2L_Project::getSelectedMeasVarList()
{
    return selMeasVarList;
}

QList<A2L_VarMeas *> A2L_Project::getAddMeasVarListByNameAndRateList(QStringList measVarNameList, QStringList measVarRateList)
{
    int measVarNameCount = measVarNameList.count();
    int measVarRateCount = measVarRateList.count();
    QList<A2L_VarMeas*> measVarList;
    if (measVarNameCount != measVarRateCount)
        return measVarList;
    if (measVarNameCount == 0)
        return measVarList;

    for (int i = 0; i < measVarNameCount; i++)
    {
        QString measVarName = measVarNameList.at(i);
        QString measVarRate = measVarRateList.at(i);

        A2L_VarMeas *measVar = findMeasVarInSelMeasVarList(measVarName, measVarRate);
        if (measVar != NULL)
        {
            measVarList.append(measVar);
        }
        else
        {
            A2L_VarMeas *measVarNew = getMeasVarByNameAndRate(measVarName, measVarRate);
            if (measVarNew != NULL)
            {
                measVarList.append(measVarNew);
                selMeasVarList.append(measVarNew);

                updateEventRelatedMeasSize(measVarNew, 1);
            }
        }

    }
    bool xcpCanInited = getCurProXcpCanInitStatus();
    bool xcpConnected = getCurProXcpSessionConnectedStatus();
    bool xcpDaqRunning = getCurProXcpDaqRunningStatus();

    qDebug()<<"canInit="<<xcpCanInited<<",connected="<<xcpConnected
           <<",daqrunning="<<xcpDaqRunning;

    qDebug()<<"Add measurment variable count="<<measVarList.count();
    qDebug()<<"Current selected all measurement variable count="<<selMeasVarList.count();

    return measVarList;
}

A2L_VarMeas *A2L_Project::getMeasVarByNameAndRate(QString measVarName, QString measVarRate)
{
    QString measVarName_Root = measVarName;
    measVarName_Root.replace(QRegExp("_(\\[\\d\\])+"), "");
    QMap<QString, QString> *measInfoMap = a2lParser->GetMeasNodeInfoByName(a2lFullFileName, measVarName);
    if (measInfoMap == NULL)
    {
        measInfoMap = a2lParser->GetMeasNodeInfoByName(a2lFullFileName, measVarName_Root);
        if (measInfoMap == NULL)
            return NULL;
    }

    QString compuMethod = measInfoMap->value("Conversion");

    QMap<QString, QString> *compMInfoMap = a2lParser->GetCompMNodeInfoByName(a2lFullFileName, compuMethod);
    if (compMInfoMap == NULL && compuMethod != "NO_COMPU_METHOD")
        return NULL;

    A2L_Event *refEvent = findEventByShortName(measVarRate);

    A2L_VarMeas *measVar = new A2L_VarMeas;

    measVar->Name = measVarName;
    measVar->ShortName = measInfoMap->value("Name");
    measVar->LongIdentifier = measInfoMap->value("LongIdentifier");
    measVar->Conversion = measInfoMap->value("Conversion");
    measVar->DataType = measInfoMap->value("DataType");
    measVar->LowerLimit = measInfoMap->value("LowerLimit").toDouble();
    measVar->UpperLimit = measInfoMap->value("UpperLimit").toDouble();
    measVar->Resolution = measInfoMap->value("Resolution").toUInt(0, 10);
    measVar->Accuracy = measInfoMap->value("Accuracy").toDouble();
    measVar->DataSizeAG = getSizeFromDataType(measVar->DataType);

    measVar->ArraySize = measInfoMap->value("ArraySize").toInt();

    measVar->xDim = measInfoMap->value("MatrixDimX").toInt();
    measVar->yDim = measInfoMap->value("MatrixDimY").toInt();
    measVar->zDim = measInfoMap->value("MatrixDimZ").toInt();

    QString addStr = measInfoMap->value("ECUAddress");
    if (addStr.mid(0, 2) == "0x")
        measVar->ECUAddress = (quint32)(addStr.mid(2, -1).toUInt(0, 16));
    else
        measVar->ECUAddress = (quint32)(addStr.toUInt(0, 10));

    if(compuMethod == "NO_COMPU_METHOD")
    {
        measVar->ConversionType = "RAT_FUNC";
        measVar->COEFFS = "0,1,0,0,0,1";
        measVar->ConversionTable = "";
        measVar->Format = "%2.0";
        measVar->Unit = "None";
    }
    else
    {
        measVar->ConversionType = compMInfoMap->value("ConversionType");
        measVar->COEFFS = compMInfoMap->value("COEFFS");
        measVar->ConversionTable = compMInfoMap->value("ConversionTable");
        measVar->Format = compMInfoMap->value("Format");
        measVar->Unit = compMInfoMap->value("Unit");
    }

    QStringList coefStrList = measVar->COEFFS.split(",");
    for (int i = 0; i < coefStrList.count(); i++)
    {
        QString str = coefStrList.at(i);
        measVar->CoefList.append(str.toDouble());
    }

    if(measVar->ConversionType == "RAT_FUNC")
        getScalingNumFromCoefs(measVar->CoefList, measVar->ScalingFactor, measVar->ScalingOffset);

    measVar->daqEventRef = refEvent;
    if(refEvent)
    {
        quint8 timeUnit = refEvent->TimeUnit;
        switch (timeUnit) {
        case 5:
            measVar->rate_ms =  0.1*refEvent->TimeCycle;
            break;
        case 6:
            measVar->rate_ms = 1*refEvent->TimeCycle;
            break;
        case 7:
            measVar->rate_ms =  10*refEvent->TimeCycle;
            break;
        case 8:
            measVar->rate_ms = 100*refEvent->TimeCycle;
            break;
        default:
            break;
        }
    }

    if (measVar->Name != measVar->ShortName)
    {
        QRegExp rx("\\[\\d\\]");
        int pos = 0;
        QStringList list;
        QList<quint8> dimList = {0,0,0};
        int index = 0;
        while ((pos = rx.indexIn(measVarName, pos)) != -1)
        {
            list << rx.cap(0);
            dimList.replace(index, rx.cap(0).remove(0,1).remove(-1,1).toInt());
            pos += rx.matchedLength();
            index++;
        }

        if(measVar->ArraySize > 1)
        {
            measVar->ECUAddress += (measVar->DataSizeAG * dimList.at(0));
        }
        else if(measVar->xDim >1 || measVar->yDim > 1 || measVar->zDim > 1)
        {
            measVar->ECUAddress += (measVar->DataSizeAG *
                                    (dimList.at(0) * measVar->yDim * measVar->zDim +
                                     dimList.at(1) * measVar->zDim +
                                     dimList.at(2)
                                     ));
        }
    }
    qDebug()<<"MeasName="<<measVar->Name<<", short name:"<<measVar->ShortName<<", Address=0x"<<QString::number(measVar->ECUAddress, 16);
    return measVar;
}

A2L_VarMeas *A2L_Project::findMeasVarInSelMeasVarList(QString measVarName, QString measVarRate)
{
    if (selMeasVarList.isEmpty())
        return NULL;
    for(int i = 0; i < selMeasVarList.count(); i++)
    {
        A2L_VarMeas *measVar = selMeasVarList.at(i);
        if (measVar->Name == measVarName)
        {
            return measVar;
        }
    }

    return NULL;
}

A2L_VarMeas *A2L_Project::getMeasVarByName(QString measVarName)
{
    QString measVarName_Root = measVarName;
    measVarName_Root.replace(QRegExp("_(\\[\\d\\])+"), "");
    qDebug()<<"measVarName:"<<measVarName<<", measVar root:"<<measVarName_Root;

    QMap<QString, QString> *measInfoMap = a2lParser->GetMeasNodeInfoByName(a2lFullFileName, measVarName);
    if (measInfoMap == NULL)
    {
        measInfoMap = a2lParser->GetMeasNodeInfoByName(a2lFullFileName, measVarName_Root);
        if (measInfoMap == NULL)
            return NULL;
    }

    QString compuMethod = measInfoMap->value("Conversion");
    QMap<QString, QString> *compMInfoMap = a2lParser->GetCompMNodeInfoByName(a2lFullFileName, compuMethod);
    if (compMInfoMap == NULL && compuMethod != "NO_COMPU_METHOD")
        return NULL;


    A2L_VarMeas *measVar = new A2L_VarMeas;

    measVar->Name = measVarName;
    measVar->ShortName = measInfoMap->value("Name");
    measVar->LongIdentifier = measInfoMap->value("LongIdentifier");
    measVar->Conversion = measInfoMap->value("Conversion");
    measVar->DataType = measInfoMap->value("DataType");
    measVar->LowerLimit = measInfoMap->value("LowerLimit").toDouble();
    measVar->UpperLimit = measInfoMap->value("UpperLimit").toDouble();
    measVar->Resolution = measInfoMap->value("Resolution").toUInt(0, 10);
    measVar->Accuracy = measInfoMap->value("Accuracy").toDouble();
    measVar->DataSizeAG = getSizeFromDataType(measVar->DataType);

    measVar->ArraySize = measInfoMap->value("ArraySize").toInt();

    measVar->xDim = measInfoMap->value("MatrixDimX").toInt();
    measVar->yDim = measInfoMap->value("MatrixDimY").toInt();
    measVar->zDim = measInfoMap->value("MatrixDimZ").toInt();

    QString addStr = measInfoMap->value("ECUAddress");
    if (addStr.mid(0, 2) == "0x")
        measVar->ECUAddress = (quint32)(addStr.mid(2, -1).toUInt(0, 16));
    else
        measVar->ECUAddress = (quint32)(addStr.toUInt(0, 10));

    if(compuMethod == "NO_COMPU_METHOD")
    {
        measVar->ConversionType = "RAT_FUNC";
        measVar->COEFFS = "0,1,0,0,0,1";
        measVar->ConversionTable = "";
        measVar->Format = "%2.0";
        measVar->Unit = "None";
    }
    else
    {
        measVar->ConversionType = compMInfoMap->value("ConversionType");
        measVar->COEFFS = compMInfoMap->value("COEFFS");
        measVar->ConversionTable = compMInfoMap->value("ConversionTable");
        measVar->Format = compMInfoMap->value("Format");
        measVar->Unit = compMInfoMap->value("Unit");
    }

    QStringList coefStrList = measVar->COEFFS.split(",");
    for (int i = 0; i < coefStrList.count(); i++)
    {
        QString str = coefStrList.at(i);
        measVar->CoefList.append(str.toDouble());
    }

    if(measVar->ConversionType == "RAT_FUNC")
    {
        getScalingNumFromCoefs(measVar->CoefList, measVar->ScalingFactor, measVar->ScalingOffset);
    }
    else if(measVar->ConversionType == "TAB_VERB")
    {
        measVar->COEFFS = "0,1,0,0,0,1";
    }

    if (measVar->Name != measVar->ShortName)
    {
        QRegExp rx("\\[\\d\\]");
        int pos = 0;
        QStringList list;
        QList<quint8> dimList = {0,0,0};
        int index = 0;
        while ((pos = rx.indexIn(measVarName, pos)) != -1)
        {
            list << rx.cap(0);
            dimList.replace(index, rx.cap(0).remove(0,1).remove(-1,1).toInt());
            pos += rx.matchedLength();
            index++;
        }

        if(measVar->ArraySize > 1)
        {
            measVar->ECUAddress += (measVar->DataSizeAG * dimList.at(0));
        }
        else if(measVar->xDim >1 || measVar->yDim > 1 || measVar->zDim > 1)
        {
            measVar->ECUAddress += (measVar->DataSizeAG *
                                    (dimList.at(0) * measVar->yDim * measVar->zDim +
                                     dimList.at(1) * measVar->zDim +
                                     dimList.at(2)
                                     ));
        }
    }
    qDebug()<<"Meas name:"<<measVar->Name<<", shortName:"<<measVar->ShortName<<", Address=0x"<<QString::number(measVar->ECUAddress, 16);
    qDebug()<<"=======================================";
    return measVar;
}

void A2L_Project::getScalingNumFromCoefs(QList<qreal> coefList, qreal &scalingFactor, qreal &scalingOffset)
{
    scalingFactor = 1;
    scalingOffset = 0;

    if (coefList.count() != 6)
        return;

    qreal a = coefList.at(0);
    qreal b = coefList.at(1);
    qreal c = coefList.at(2);
    qreal d = coefList.at(3);
    qreal e = coefList.at(4);
    qreal f = coefList.at(5);

    if(a != 0 || d!= 0 || e != 0)
        return;

    scalingFactor = f/b;
    scalingOffset = c/b;


}

QList<A2L_VarChar *> A2L_Project::getAddCharVarListByNameList(QStringList charVarNameList)
{
    int charVarNameCount = charVarNameList.count();
    QList<A2L_VarChar*> charVarList;
    if (charVarNameCount == 0)
        return charVarList;

    for (int i = 0; i < charVarNameCount; i++)
    {
        QString charVarName = charVarNameList.at(i);
        A2L_VarChar *charVar = getCharVarByName(charVarName);

        if (charVar != NULL)
        {
            charVarList.append(charVar);
            selCharVarList.append(charVar);
        }
    }
    qDebug()<<"Add characteristic variable count="<<charVarList.count();
    qDebug()<<"Current selected all characteristic variable count="<<selCharVarList.count();

    return charVarList;
}

A2L_VarChar *A2L_Project::getCharVarByName(QString charVarName)
{
    QMap<QString, QString> *charInfoMap = a2lParser->GetCharNodeInfoByName(a2lFullFileName, charVarName);
    if (charInfoMap == NULL)
        return NULL;

    //Conversion_ 转换
    QString compuMethod = charInfoMap->value("Conversion");
    QMap<QString, QString> *compMInfoMap = a2lParser->GetCompMNodeInfoByName(a2lFullFileName, compuMethod);
    if (compMInfoMap == NULL)
        return NULL;


    //Deposit_ recordlayout 储存格式
    QString recordLayout = charInfoMap->value("Deposit");
    QMap<QString, QString> *recordLayoutInfoMap = a2lParser->GetRecordLayoutNodeInfoByName(a2lFullFileName, recordLayout);
    if (recordLayoutInfoMap == NULL)
        return NULL;


    //创建标定参数对象
    A2L_VarChar *charVar = new A2L_VarChar;
    qDebug()<<"0, char:"<<charVar;
    charVar->Name = charInfoMap->value("Name");
    charVar->LongIdentifier = charInfoMap->value("LongIdentifier");
    charVar->LowerLimit = charInfoMap->value("LowerLimit").toDouble();
    charVar->UpperLimit = charInfoMap->value("UpperLimit").toDouble();
    charVar->Conversion = compuMethod;
    charVar->Deposit = recordLayout;
    charVar->charType = charInfoMap->value("Type");
    charVar->MaxDiff = charInfoMap->value("MaxDiff").toDouble();

    charVar->DataType = recordLayoutInfoMap->value("FNC_VALUES_DataType");
    charVar->DataSizeAG = getSizeFromDataType(charVar->DataType);
    charVar->Fnc_Values_Position = recordLayoutInfoMap->value("FNC_VALUES_Position").toInt();
    charVar->Fnc_Values_DataType = recordLayoutInfoMap->value("FNC_VALUES_DataType");
    charVar->Fnc_Values_IndexMode = recordLayoutInfoMap->value("FNC_VALUES_IndexMode");
    charVar->Fnc_Values_AddressType = recordLayoutInfoMap->value("FNC_VALUES_Addesstype");

    QString addStr = charInfoMap->value("Address");
    if (addStr.mid(0, 2) == "0x")
        charVar->ECUAddress = (quint32)(addStr.mid(2, -1).toUInt(0, 16));
    else
        charVar->ECUAddress = (quint32)(addStr.toUInt(0, 10));

    charVar->ConversionType = compMInfoMap->value("ConversionType");
    charVar->COEFFS = compMInfoMap->value("COEFFS");
    charVar->ConversionTable = compMInfoMap->value("ConversionTable");
    charVar->Format = compMInfoMap->value("Format");
    charVar->Unit = compMInfoMap->value("Unit");

    QStringList coefStrList = charVar->COEFFS.split(",");
    for (int i = 0; i < coefStrList.count(); i++)
    {
        QString str = coefStrList.at(i);
        charVar->CoefList.append(str.toDouble());
    }

    if(charVar->ConversionType == "RAT_FUNC")
    {
        getScalingNumFromCoefs(charVar->CoefList, charVar->ScalingFactor, charVar->ScalingOffset);
    }
    if(charVar->ConversionType == "TAB_VERB")
    {
        QString tabVerbName = charVar->ConversionTable;
        QMap<QString, QString> *tabVerbInfoMap = a2lParser->GetCompVtabNodeInfoByName(a2lFullFileName, tabVerbName);
        if(tabVerbInfoMap)
        {
            charVar->ConversionVTab = tabVerbInfoMap->value("ValuePairs");
        }
    }


    //axis_descr X轴信息
    QString compuMethod_axisDescrX = charInfoMap->value("AXIS_X_Conversion");
    QMap<QString, QString> *compMInfoMap_axisDescrX = a2lParser->GetCompMNodeInfoByName(a2lFullFileName, compuMethod_axisDescrX);
    QString recordLayout_axisDescrX = charInfoMap->value("AXIS_X_Deposit");
    QMap<QString, QString> *recordLayoutInfoMap_axisDescrX = a2lParser->GetRecordLayoutNodeInfoByName(a2lFullFileName, recordLayout_axisDescrX);

    //axis_descr Y轴信息
    QString compuMethod_axisDescrY = charInfoMap->value("AXIS_Y_Conversion");
    QMap<QString, QString> *compMInfoMap_axisDescrY = a2lParser->GetCompMNodeInfoByName(a2lFullFileName, compuMethod_axisDescrY);
    QString recordLayout_axisDescrY = charInfoMap->value("AXIS_Y_Deposit");
    QMap<QString, QString> *recordLayoutInfoMap_axisDescrY = a2lParser->GetRecordLayoutNodeInfoByName(a2lFullFileName, recordLayout_axisDescrY);

    // axispts X轴信息
    QString compuMethod_axisX;
    QString recordLayout_axisX;

    QString axis_pts_x = charInfoMap->value("AXIS_X_PTS_REF");
    QMap<QString, QString> *axisPtsInfoMap_X = a2lParser->GetAxisPtsNodeInfoByName(a2lFullFileName, axis_pts_x);
    if (axisPtsInfoMap_X != NULL)
    {
        //Conversion_ x轴转换
        compuMethod_axisX = axisPtsInfoMap_X->value("Conversion");

        //Deposit_ recordlayout x轴储存格式
        recordLayout_axisX = axisPtsInfoMap_X->value("Deposit");

    }
    QMap<QString, QString> *compMInfoMap_axisX = a2lParser->GetCompMNodeInfoByName(a2lFullFileName, compuMethod_axisX);
    QMap<QString, QString> *recordLayoutInfoMap_axisX = a2lParser->GetRecordLayoutNodeInfoByName(a2lFullFileName, recordLayout_axisX);

    // axispts Y轴信息
    QString compuMethod_axisY;
    QString recordLayout_axisY;
    QString axis_pts_y = charInfoMap->value("AXIS_Y_PTS_REF");
    QMap<QString, QString> *axisPtsInfoMap_Y = a2lParser->GetAxisPtsNodeInfoByName(a2lFullFileName, axis_pts_y);
    if (axisPtsInfoMap_Y != NULL)
    {
        //Conversion_ y轴转换
        compuMethod_axisY = axisPtsInfoMap_Y->value("Conversion");

        //Deposit_ recordlayout y轴储存格式
        recordLayout_axisY = axisPtsInfoMap_Y->value("Deposit");

    }
    QMap<QString, QString> *compMInfoMap_axisY = a2lParser->GetCompMNodeInfoByName(a2lFullFileName, compuMethod_axisY);
    QMap<QString, QString> *recordLayoutInfoMap_axisY = a2lParser->GetRecordLayoutNodeInfoByName(a2lFullFileName, recordLayout_axisY);

    QString xCount, yCount, zCount;
    QList<qreal> xValueList, yValueList, zValueList;



    if (charVar->charType == "CURVE")
    {
        // Axix_X
        charVar->Axis_X_Ref = charInfoMap->value("AXIS_X_PTS_REF");
        charVar->Axis_X_Attribute = axisPtsInfoMap_X->value("AXIS_X_Attribute");
        charVar->Axis_X_InputQuantity = charInfoMap->value("AXIS_X_InputQuantity");
        charVar->Axis_X_Conversion = charInfoMap->value("AXIS_X_Conversion");
        charVar->Axis_X_MaxAxisPoints = charInfoMap->value("AXIS_X_MaxAxisPoints").toInt();
        charVar->Axis_X_LowerLimit = charInfoMap->value("AXIS_X_LowerLimit").toDouble();
        charVar->Axis_X_UpperLimit = charInfoMap->value("AXIS_X_UpperLimit").toDouble();
        charVar->Axis_X_Format = charInfoMap->value("AXIS_X_Format");
        charVar->Axis_X_Deposit = charInfoMap->value("AXIS_X_Deposit");

        //from AXIS_DESCR
        if(compMInfoMap_axisDescrX)
        {
            charVar->Axis_X_Type = compMInfoMap_axisDescrX->value("Type");
            charVar->Axis_X_ConversionType = compMInfoMap_axisDescrX->value("ConversionType");
            charVar->Axis_X_ConversionTable = compMInfoMap_axisDescrX->value("ConversionTable");
            charVar->Axis_X_COEFFS = compMInfoMap_axisDescrX->value("COEFFS");
            charVar->Axis_X_Unit = compMInfoMap_axisDescrX->value("Unit");
        }
        if(recordLayoutInfoMap_axisDescrX)
        {
            if(recordLayoutInfoMap_axisX->keys().contains("FNC_VALUES_DataType"))
                charVar->Axis_X_DataType = recordLayoutInfoMap_axisX->value("FNC_VALUES_DataType");
            if(recordLayoutInfoMap_axisX->keys().contains("AXIS_PTS_X_DataType"))
                charVar->Axis_X_DataType = recordLayoutInfoMap_axisX->value("AXIS_PTS_X_DataType");

            charVar->Axis_X_DataSizeAG = getSizeFromDataType(charVar->Axis_X_DataType);
        }

        //from AXIS_PTS
        if(axisPtsInfoMap_X)
        {
            charVar->Axis_X_InputQuantity = axisPtsInfoMap_X->value("InputQuantity");
            charVar->Axis_X_Conversion = axisPtsInfoMap_X->value("Conversion");
            charVar->Axis_X_MaxAxisPoints = axisPtsInfoMap_X->value("MaxAxisPoints").toInt();
            charVar->Axis_X_LowerLimit = axisPtsInfoMap_X->value("LowerLimit").toDouble();
            charVar->Axis_X_UpperLimit = axisPtsInfoMap_X->value("UpperLimit").toDouble();
            charVar->Axis_X_Deposit = axisPtsInfoMap_X->value("Deposit");
            charVar->Axis_X_Unit = axisPtsInfoMap_X->value("Unit");
            QString addStr_x = axisPtsInfoMap_X->value("Address");
            if (addStr_x.mid(0, 2) == "0x")
                charVar->Axis_X_ECUAddress = (quint32)(addStr_x.mid(2, -1).toUInt(0, 16));
            else
                charVar->Axis_X_ECUAddress = (quint32)(addStr_x.toUInt(0, 10));
        }
        if(compMInfoMap_axisX)
        {
            charVar->Axis_X_Type = compMInfoMap_axisX->value("Type");
            charVar->Axis_X_ConversionType = compMInfoMap_axisX->value("ConversionType");
            charVar->Axis_X_ConversionTable = compMInfoMap_axisX->value("ConversionTable");
            charVar->Axis_X_COEFFS = compMInfoMap_axisX->value("COEFFS");
            charVar->Axis_X_Unit = compMInfoMap_axisX->value("Unit");
        }
        if(recordLayoutInfoMap_axisX)
        {
            if(recordLayoutInfoMap_axisX->keys().contains("FNC_VALUES_DataType"))
                charVar->Axis_X_DataType = recordLayoutInfoMap_axisX->value("FNC_VALUES_DataType");
            if(recordLayoutInfoMap_axisX->keys().contains("AXIS_PTS_X_DataType"))
                charVar->Axis_X_DataType = recordLayoutInfoMap_axisX->value("AXIS_PTS_X_DataType");

            charVar->Axis_X_DataSizeAG = getSizeFromDataType(charVar->Axis_X_DataType);
        }

        QStringList coefStrList_axisX = charVar->Axis_X_COEFFS.split(",");
        for (int i = 0; i < coefStrList_axisX.count(); i++)
        {
            QString str = coefStrList_axisX.at(i);
            charVar->Axis_X_CoefList.append(str.toDouble());
        }
        if(charVar->ConversionType == "RAT_FUNC")
        {
            getScalingNumFromCoefs(charVar->Axis_X_CoefList, charVar->Axis_X_ScalingFactor, charVar->Axis_X_ScalingOffset);
        }

        if(charVar->Axis_X_ConversionType == "TAB_VERB")
        {
            QString tabVerbName_axisX = charVar->Axis_X_ConversionTable;
            QMap<QString, QString> *tabVerbInfoMap_axisX = a2lParser->GetCompVtabNodeInfoByName(a2lFullFileName, tabVerbName_axisX);
            if(tabVerbInfoMap_axisX)
            {
                charVar->Axis_X_ConversionVTab = tabVerbInfoMap_axisX->value("ValuePairs");
            }
        }
    }
    else if (charVar->charType == "MAP")
    {
        // Axix_X
        charVar->Axis_X_Ref = charInfoMap->value("AXIS_X_PTS_REF");
        charVar->Axis_X_Attribute = charInfoMap->value("AXIS_X_Attribute");
        charVar->Axis_X_InputQuantity = charInfoMap->value("AXIS_X_InputQuantity");
        charVar->Axis_X_Conversion = charInfoMap->value("AXIS_X_Conversion");
        charVar->Axis_X_MaxAxisPoints = charInfoMap->value("AXIS_X_MaxAxisPoints").toInt();
        charVar->Axis_X_LowerLimit = charInfoMap->value("AXIS_X_LowerLimit").toDouble();
        charVar->Axis_X_UpperLimit = charInfoMap->value("AXIS_X_UpperLimit").toDouble();
        charVar->Axis_X_Format = charInfoMap->value("AXIS_X_Format");
        charVar->Axis_X_Deposit = charInfoMap->value("AXIS_X_Deposit");

        //from AXIS_DESCR
        if(compMInfoMap_axisDescrX)
        {
            charVar->Axis_X_Type = compMInfoMap_axisDescrX->value("Type");
            charVar->Axis_X_ConversionType = compMInfoMap_axisDescrX->value("ConversionType");
            charVar->Axis_X_ConversionTable = compMInfoMap_axisDescrX->value("ConversionTable");
            charVar->Axis_X_COEFFS = compMInfoMap_axisDescrX->value("COEFFS");
            charVar->Axis_X_Unit = compMInfoMap_axisDescrX->value("Unit");
        }
        if(recordLayoutInfoMap_axisDescrX)
        {
            if(recordLayoutInfoMap_axisDescrX->keys().contains("FNC_VALUES_DataType"))
                charVar->Axis_X_DataType = recordLayoutInfoMap_axisDescrX->value("FNC_VALUES_DataType");
            if(recordLayoutInfoMap_axisDescrX->keys().contains("AXIS_PTS_X_DataType"))
                charVar->Axis_X_DataType = recordLayoutInfoMap_axisDescrX->value("AXIS_PTS_X_DataType");

            charVar->Axis_X_DataSizeAG = getSizeFromDataType(charVar->Axis_X_DataType);
        }

        if(axisPtsInfoMap_X)
        {
            charVar->Axis_X_InputQuantity = axisPtsInfoMap_X->value("InputQuantity");
            charVar->Axis_X_Conversion = axisPtsInfoMap_X->value("Conversion");
            charVar->Axis_X_MaxAxisPoints = axisPtsInfoMap_X->value("MaxAxisPoints").toInt();
            charVar->Axis_X_LowerLimit = axisPtsInfoMap_X->value("LowerLimit").toDouble();
            charVar->Axis_X_UpperLimit = axisPtsInfoMap_X->value("UpperLimit").toDouble();
            charVar->Axis_X_Deposit = axisPtsInfoMap_X->value("Deposit");
            charVar->Axis_X_Unit = axisPtsInfoMap_X->value("Unit");
            QString addStr_x = axisPtsInfoMap_X->value("Address");
            if (addStr_x.mid(0, 2) == "0x")
                charVar->Axis_X_ECUAddress = (quint32)(addStr_x.mid(2, -1).toUInt(0, 16));
            else
                charVar->Axis_X_ECUAddress = (quint32)(addStr_x.toUInt(0, 10));
        }
        if(compMInfoMap_axisX)
        {
            charVar->Axis_X_Type = compMInfoMap_axisX->value("Type");
            charVar->Axis_X_ConversionType = compMInfoMap_axisX->value("ConversionType");
            charVar->Axis_X_ConversionTable = compMInfoMap_axisX->value("ConversionTable");
            charVar->Axis_X_COEFFS = compMInfoMap_axisX->value("COEFFS");
            charVar->Axis_X_Unit = compMInfoMap_axisX->value("Unit");
        }
        if(recordLayoutInfoMap_axisX)
        {
            if(recordLayoutInfoMap_axisX->keys().contains("FNC_VALUES_DataType"))
                charVar->Axis_X_DataType = recordLayoutInfoMap_axisX->value("FNC_VALUES_DataType");
            if(recordLayoutInfoMap_axisX->keys().contains("AXIS_PTS_X_DataType"))
                charVar->Axis_X_DataType = recordLayoutInfoMap_axisX->value("AXIS_PTS_X_DataType");

            charVar->Axis_X_DataSizeAG = getSizeFromDataType(charVar->Axis_X_DataType);
        }

        QStringList coefStrList_axisX = charVar->Axis_X_COEFFS.split(",");
        for (int i = 0; i < coefStrList_axisX.count(); i++)
        {
            QString str = coefStrList_axisX.at(i);
            charVar->Axis_X_CoefList.append(str.toDouble());
        }
        if(charVar->ConversionType == "RAT_FUNC")
        {
            getScalingNumFromCoefs(charVar->Axis_X_CoefList, charVar->Axis_X_ScalingFactor, charVar->Axis_X_ScalingOffset);
        }
        if(charVar->Axis_X_ConversionType == "TAB_VERB")
        {
            QString tabVerbName_axisX = charVar->Axis_X_ConversionTable;
            QMap<QString, QString> *tabVerbInfoMap_axisX = a2lParser->GetCompVtabNodeInfoByName(a2lFullFileName, tabVerbName_axisX);
            if(tabVerbInfoMap_axisX)
            {
                charVar->Axis_X_ConversionVTab = tabVerbInfoMap_axisX->value("ValuePairs");
            }
        }


        //==================================================================
        // Axix_Y
        charVar->Axis_Y_Ref = charInfoMap->value("AXIS_Y_PTS_REF");
        charVar->Axis_Y_Attribute = charInfoMap->value("AXIS_Y_Attribute");
        charVar->Axis_Y_InputQuantity = charInfoMap->value("AXIS_Y_InputQuantity");
        charVar->Axis_Y_Conversion = charInfoMap->value("AXIS_Y_Conversion");
        charVar->Axis_Y_MaxAxisPoints = charInfoMap->value("AXIS_Y_MaxAxisPoints").toInt();
        charVar->Axis_Y_LowerLimit = charInfoMap->value("AXIS_Y_LowerLimit").toDouble();
        charVar->Axis_Y_UpperLimit = charInfoMap->value("AXIS_Y_UpperLimit").toDouble();
        charVar->Axis_Y_Format = charInfoMap->value("AXIS_Y_Format");
        charVar->Axis_Y_Deposit = charInfoMap->value("AXIS_Y_Deposit");

        //from AXIS_DESCR
        if(compMInfoMap_axisDescrY)
        {
            charVar->Axis_Y_Type = compMInfoMap_axisDescrY->value("Type");
            charVar->Axis_Y_ConversionType = compMInfoMap_axisDescrY->value("ConversionType");
            charVar->Axis_Y_ConversionTable = compMInfoMap_axisDescrY->value("ConversionTable");
            charVar->Axis_Y_COEFFS = compMInfoMap_axisDescrY->value("COEFFS");
            charVar->Axis_Y_Unit = compMInfoMap_axisDescrY->value("Unit");
        }
        if(recordLayoutInfoMap_axisDescrY)
        {
            if(recordLayoutInfoMap_axisDescrY->keys().contains("FNC_VALUES_DataType"))
                charVar->Axis_Y_DataType = recordLayoutInfoMap_axisDescrY->value("FNC_VALUES_DataType");
            if(recordLayoutInfoMap_axisDescrY->keys().contains("AXIS_PTS_Y_DataType"))
                charVar->Axis_Y_DataType = recordLayoutInfoMap_axisDescrY->value("AXIS_PTS_Y_DataType");

            charVar->Axis_Y_DataSizeAG = getSizeFromDataType(charVar->Axis_Y_DataType);
        }

        if(axisPtsInfoMap_Y)
        {
            charVar->Axis_Y_InputQuantity = axisPtsInfoMap_Y->value("InputQuantity");
            charVar->Axis_Y_Conversion = axisPtsInfoMap_Y->value("Conversion");
            charVar->Axis_Y_MaxAxisPoints = axisPtsInfoMap_Y->value("MaxAxisPoints").toInt();
            charVar->Axis_Y_LowerLimit = axisPtsInfoMap_Y->value("LowerLimit").toDouble();
            charVar->Axis_Y_UpperLimit = axisPtsInfoMap_Y->value("UpperLimit").toDouble();
            charVar->Axis_Y_Deposit = axisPtsInfoMap_Y->value("Deposit");
            charVar->Axis_Y_Unit = axisPtsInfoMap_Y->value("Unit");
            QString addStr_y = axisPtsInfoMap_Y->value("Address");
            if (addStr_y.mid(0, 2) == "0x")
                charVar->Axis_Y_ECUAddress = (quint32)(addStr_y.mid(2, -1).toUInt(0, 16));
            else
                charVar->Axis_Y_ECUAddress = (quint32)(addStr_y.toUInt(0, 10));
        }
        if(compMInfoMap_axisY)
        {
            charVar->Axis_Y_Type = compMInfoMap_axisY->value("Type");
            charVar->Axis_Y_ConversionType = compMInfoMap_axisY->value("ConversionType");
            charVar->Axis_Y_ConversionTable = compMInfoMap_axisY->value("ConversionTable");
            charVar->Axis_Y_COEFFS = compMInfoMap_axisY->value("COEFFS");
            charVar->Axis_Y_Unit = compMInfoMap_axisY->value("Unit");
        }
        if(recordLayoutInfoMap_axisY)
        {
            if(recordLayoutInfoMap_axisY->keys().contains("FNC_VALUES_DataType"))
                charVar->Axis_Y_DataType = recordLayoutInfoMap_axisY->value("FNC_VALUES_DataType");
            if(recordLayoutInfoMap_axisY->keys().contains("AXIS_PTS_X_DataType"))
                charVar->Axis_Y_DataType = recordLayoutInfoMap_axisY->value("AXIS_PTS_X_DataType");

            charVar->Axis_Y_DataSizeAG = getSizeFromDataType(charVar->Axis_Y_DataType);
        }

        QStringList coefStrList_axisY = charVar->Axis_Y_COEFFS.split(",");
        for (int i = 0; i < coefStrList_axisY.count(); i++)
        {
            QString str = coefStrList_axisY.at(i);
            charVar->Axis_Y_CoefList.append(str.toDouble());
        }
        if(charVar->ConversionType == "RAT_FUNC")
        {
            getScalingNumFromCoefs(charVar->Axis_Y_CoefList, charVar->Axis_Y_ScalingFactor, charVar->Axis_Y_ScalingOffset);
        }
        if(charVar->Axis_Y_ConversionType == "TAB_VERB")
        {
            QString tabVerbName_axisY = charVar->Axis_Y_ConversionTable;
            QMap<QString, QString> *tabVerbInfoMap_axisY = a2lParser->GetCompVtabNodeInfoByName(a2lFullFileName, tabVerbName_axisY);
            if(tabVerbInfoMap_axisY)
            {
                charVar->Axis_Y_ConversionVTab = tabVerbInfoMap_axisY->value("ValuePairs");
            }
        }
    }
    else if(charVar->charType == "VAL_BLK")
    {
        charVar->xDim_BLK = charInfoMap->value("xDim").toInt();
        charVar->yDim_BLK = charInfoMap->value("yDim").toInt();
        charVar->zDim_BLK = charInfoMap->value("zDim").toInt();
        charVar->number_BLK = charInfoMap->value("Number").toInt();
    }

    //charVar->xCount = xCount.toInt();
    //charVar->yCount = yCount.toInt();
    //charVar->zCount = zCount.toInt();
    //charVar->hexValue_XList = xValueList;
    //charVar->hexValue_YList = yValueList;
    //charVar->hexValue_ZList = zValueList;
    //charVar->Blk_Values = zValueList;
    //charVar->setValue(zValueList.at(0));

    if(hexParsedStatus)
    {
        qDebug()<<"In hex prased status";
        //加载hex文件的处理
        //获取标定参数在hex文件中的值
        bool ok = false;
        QMap<QString, QStringList> *charHexInfoMap = a2lParser->GetCharHexValueByName(a2lFullFileName, a2lFullHexName, charVarName, &ok);

        if (charHexInfoMap == NULL)
        {
            qDebug()<<"Fail to get CHAR value from HEX file";
            delete charVar;
            return NULL;
        }

        QStringList xyzCountList = charHexInfoMap->value("xyzCountList");
        QStringList xList = charHexInfoMap->value("charXList");
        QStringList yList = charHexInfoMap->value("charYList");
        QStringList zList = charHexInfoMap->value("charZList");
        //QString xCount, yCount, zCount;
        xCount = xyzCountList.at(0);
        yCount = xyzCountList.at(1);
        zCount = xyzCountList.at(2);
        //QList<qreal> xValueList, yValueList, zValueList;
        if (xCount.toInt() != 0)
        {
            for (int i = 0; i < xCount.toInt(); i++)
            {
                QString valueStr = xList.at(i);
                xValueList.append(valueStr.toDouble());
            }
        }
        if (yCount.toInt() != 0)
        {
            for (int i = 0; i < yCount.toInt(); i++)
            {
                QString valueStr = yList.at(i);
                yValueList.append(valueStr.toDouble());
            }
        }
        if (zCount.toInt() != 0)
        {
            for (int i = 0; i < zCount.toInt(); i++)
            {
                QString valueStr = zList.at(i);
                zValueList.append(valueStr.toDouble());
            }
        }

        charVar->xCount = xCount.toInt();
        charVar->yCount = yCount.toInt();
        charVar->zCount = zCount.toInt();
        charVar->hexValue_XList = xValueList;
        charVar->hexValue_YList = yValueList;
        charVar->hexValue_ZList = zValueList;
        charVar->Blk_Values = zValueList;
        charVar->setValue(zValueList.at(0));
    }
    else
    {
        qDebug()<<"In no hex prased status";
        //不加载hex文件的处理
        zValueList.append(0);

        charVar->xCount = 1;
        charVar->yCount = 1;
        charVar->zCount = 1;
        charVar->hexValue_XList = xValueList;
        charVar->hexValue_YList = yValueList;
        charVar->hexValue_ZList = zValueList;
        charVar->Blk_Values = zValueList;
        charVar->setValue(0);
    }


    qDebug()<<"CharVar:"<<charVar->Name<<", Address=0x"<<QString::number(charVar->ECUAddress, 16);
    if(charVar->charType == "VAL_BLK")
    {
        qDebug()<<"blk value 0:"<<charVar->Blk_Values[0]<<", blk count:"<<charVar->Blk_Values.count();
    }
    qDebug()<<"=======================================";

    return charVar;

}

A2L_VarChar *A2L_Project::findCharVarInSelCharVarList(QString charVarName)
{
    if (!selValueCharVarList.isEmpty())
    {
        for (int i = 0; i < selValueCharVarList.count(); i++)
        {
            A2L_VarChar *charVar = selValueCharVarList.at(i);
            if (charVar->Name == charVarName)
            {
                return charVar;
            }
        }
    }

    if (!selCurveCharVarList.isEmpty())
    {
        for (int i = 0; i < selCurveCharVarList.count(); i++)
        {
            A2L_VarChar *charVar = selCurveCharVarList.at(i);
            if (charVar->Name == charVarName)
            {
                return charVar;
            }
        }
    }

    if (!selMapCharVarList.isEmpty())
    {
        for (int i = 0; i < selMapCharVarList.count(); i++)
        {
            A2L_VarChar *charVar = selMapCharVarList.at(i);
            if (charVar->Name == charVarName)
            {
                return charVar;
            }
        }
    }

    return NULL;

}

A2L_VarChar *A2L_Project::findValueCharVarInSelCharVarList(QString charVarName)
{
    if (!selValueCharVarList.isEmpty())
    {
        for (int i = 0; i < selValueCharVarList.count(); i++)
        {
            A2L_VarChar *charVar = selValueCharVarList.at(i);
            if (charVar->Name == charVarName)
            {
                return charVar;
            }
        }
    }


    return NULL;
}

A2L_VarChar *A2L_Project::findCurveCharVarInSelCharVarList(QString charVarName)
{
    if (!selCurveCharVarList.isEmpty())
    {
        for (int i = 0; i < selCurveCharVarList.count(); i++)
        {
            A2L_VarChar *charVar = selCurveCharVarList.at(i);
            if (charVar->Name == charVarName)
            {
                return charVar;
            }
        }
    }


    return NULL;
}

A2L_VarChar *A2L_Project::findMapCharVarInSelCharVarList(QString charVarName)
{
    if (!selMapCharVarList.isEmpty())
    {
        for (int i = 0; i < selMapCharVarList.count(); i++)
        {
            A2L_VarChar *charVar = selMapCharVarList.at(i);
            if (charVar->Name == charVarName)
            {
                return charVar;
            }
        }
    }

    return NULL;
}

QList<A2L_VarChar *> A2L_Project::getAddValueCharVarListByNameList(QStringList charVarNameList)
{
    int charVarNameCount = charVarNameList.count();
    QList<A2L_VarChar*> valueCharVarList;
    if (charVarNameCount == 0)
        return valueCharVarList;

    for (int i = 0; i < charVarNameCount; i++)
    {
        QString charVarName = charVarNameList.at(i);

        A2L_VarChar *charVar = findValueCharVarInSelCharVarList(charVarName);
        if (charVar != NULL)
        {
            valueCharVarList.append(charVar);
        }
        else
        {
            A2L_VarChar *charVarNew = getCharVarByName(charVarName);
            if (charVarNew != NULL && charVarNew->charType == "VALUE")
            {
                valueCharVarList.append(charVarNew);
                selValueCharVarList.append(charVarNew);

                connect(charVarNew, SIGNAL(charVarValueChanged(A2L_VarChar*)), xcpMaster, SLOT(charVarValueChangedSlot(A2L_VarChar*)));
            }
        }
    }
    qDebug()<<"Add value characteristic variable count="<<valueCharVarList.count();
    qDebug()<<"Current selected all value characteristic variable count="<<selValueCharVarList.count();

    return valueCharVarList;
}

QList<A2L_VarChar *> A2L_Project::getAddCurveCharVarListByNameList(QStringList charVarNameList)
{
    int charVarNameCount = charVarNameList.count();
    QList<A2L_VarChar*> curveCharVarList;
    if (charVarNameCount == 0)
        return curveCharVarList;

    for (int i = 0; i < charVarNameCount; i++)
    {
        QString charVarName = charVarNameList.at(i);

        A2L_VarChar *charVar = findCurveCharVarInSelCharVarList(charVarName);
        if (charVar != NULL)
        {
            curveCharVarList.append(charVar);
        }
        else
        {
            A2L_VarChar *charVarNew = getCharVarByName(charVarName);

            if (charVarNew != NULL && (charVarNew->charType == "CURVE") || (charVarNew->charType == "VAL_BLK"))
            {
                curveCharVarList.append(charVarNew);
                selCurveCharVarList.append(charVarNew);

                connect(charVarNew, SIGNAL(curveCharZValueChanged(A2L_VarChar*, int, qreal)), xcpMaster, SLOT(curveCharZValueChangedSlot(A2L_VarChar*, int, qreal)));
            }
        }

    }
    qDebug()<<"Add curve characteristic variable count="<<curveCharVarList.count();
    qDebug()<<"Current selected all curve characteristic variable count="<<selCurveCharVarList.count();

    return curveCharVarList;
}

QList<A2L_VarChar *> A2L_Project::getAddMapCharVarListByNameList(QStringList charVarNameList)
{
    int charVarNameCount = charVarNameList.count();
    QList<A2L_VarChar*> mapCharVarList;
    if (charVarNameCount == 0)
        return mapCharVarList;

    for (int i = 0; i < charVarNameCount; i++)
    {
        QString charVarName = charVarNameList.at(i);

        A2L_VarChar *charVar = findMapCharVarInSelCharVarList(charVarName);
        if (charVar != NULL)
        {
            mapCharVarList.append(charVar);
        }
        else
        {
            A2L_VarChar *charVarNew = getCharVarByName(charVarName);

            if (charVarNew != NULL && charVarNew->charType == "MAP")
            {
                mapCharVarList.append(charVarNew);
                selMapCharVarList.append(charVarNew);

                //connect(charVarNew, SIGNAL(curveCharZValueChanged(A2L_VarChar*, int, qreal)), xcpMaster, SLOT(curveCharZValueChangedSlot(A2L_VarChar*, int, qreal)));
            }
        }

    }
    qDebug()<<"Add map characteristic variable count="<<mapCharVarList.count();
    qDebug()<<"Current selected all map characteristic variable count="<<selMapCharVarList.count();

    return mapCharVarList;
}

void A2L_Project::removeCharVarFromSelCharVarList(A2L_VarChar *charVar)
{
    if (charVar == NULL)
        return;
    QString charType = charVar->charType;
    if (charType == "VALUE" || charType == "VAL_BLK")
    {
        if (selValueCharVarList.count() == 0)
            return;

        selValueCharVarList.removeOne(charVar);

        qDebug()<<"After Remove, Current SelValueCharVarList Count="<<selValueCharVarList.count();
    }
    if (charType == "CURVE")
    {
        if (selCurveCharVarList.count() == 0)
            return;

        selCurveCharVarList.removeOne(charVar);

        qDebug()<<"After Remove, Current SelCurveCharVarList Count="<<selCurveCharVarList.count();
    }
    if (charType == "MAP")
    {
        if (selMapCharVarList.count() == 0)
            return;

        selMapCharVarList.removeOne(charVar);

        qDebug()<<"After Remove, Current SelMapCharVarList Count="<<selMapCharVarList.count();
    }
    delete charVar;

}

A2L_Daq *A2L_Project::getDaqInfo()
{
    return a2lDaq;
}

A2L_Xcp_On_Can *A2L_Project::getXcpOnCanInfo(int index)
{
    if (index >= a2lXcpOnCanList.count())
        return NULL;
    getA2LDaqInfo(index);
    getA2LDaqMemInfo(index);
    getA2LEventListInfo(index);

    return a2lXcpOnCanList.at(index);
}

void A2L_Project::setCurUseXcpOnCanIndex(int index)
{
    if (index >= a2lXcpOnCanList.count())
        return;

    curIndexXcpOnCan = index;
    curXcpOnCan = a2lXcpOnCanList.at(index);
}

A2L_Xcp_On_Can *A2L_Project::getCurUseXcpOnCan()
{
    return curXcpOnCan;
}

QStringList A2L_Project::getXcpOnCanInstanceNameList()
{
    return xcpOnCanInstanceNameList;
}

QStringList A2L_Project::getDaqListCanIdList()
{
    QStringList idList;
    if (a2lDaqListCanIdList.isEmpty())
        return idList;
    for (int i = 0; i < a2lDaqListCanIdList.count(); i++)
    {
        A2L_DaqListCanId *daqListCanId = a2lDaqListCanIdList.at(i);
        QString idStr = "0x" + QString::number(daqListCanId->DaqList_CAN_ID, 16).toUpper();
        idList.append(idStr);
    }
    return idList;
}

QStringList A2L_Project::getEventCanIdList()
{
    QStringList idList;
    if (a2lEventCanIdList.isEmpty())
        return idList;
    for (int i = 0; i < a2lEventCanIdList.count(); i++)
    {
        A2L_EventCanId *eventCanId = a2lEventCanIdList.at(i);
        QString idStr = "0x" + QString::number(eventCanId->Event_CAN_ID, 16).toUpper();
        idList.append(idStr);
    }
    return idList;
}

QStringList A2L_Project::getDaqListList()
{
    QStringList daqListList;
    if (a2lDaqListList.isEmpty())
        return daqListList;
    for (int i = 0; i < a2lDaqListList.count(); i++)
    {
        A2L_Daq_List *daqList = a2lDaqListList.at(i);
        QString daqListStr = "0x" + QString::number(daqList->DaqListNumber, 16);
        daqListList.append(daqListStr);
    }
    return daqListList;
}

QList<A2L_Daq_List *> A2L_Project::getDaqListStructList()
{
    return a2lDaqListList;
}

A2L_Daq_Memory_Consumption *A2L_Project::getDaqMemoryConsumption()
{
    return a2lDaqMem;
}

bool A2L_Project::addHexFile(QString fullHexName)
{
    QStringList infoList;
    bool status = a2lParser->addHexToA2LFile(a2lFullFileName, fullHexName, infoList);
    hexParsedStatus = status;
    if (!status)  return false;

    a2lFullHexName = fullHexName;
    return status;
}

void A2L_Project::setXcpMaster(XCPMaster *xcpMaster)
{
    this->xcpMaster = xcpMaster;
    if (xcpMaster == NULL)
        return;
    xcpMaster->setXcpDaq(a2lDaq);
    xcpMaster->setXcpDaqList(a2lDaqListList);
    xcpMaster->setXcpEventList(a2lEventList);
}

bool A2L_Project::getCurProXcpCanInitStatus()
{
    return xcpMaster->getXcpCanInitStatus();
}

bool A2L_Project::getCurProXcpSessionConnectedStatus()
{
    return xcpMaster->getXcpSessionConnectedStatus();
}

bool A2L_Project::getCurProXcpDaqRunningStatus()
{
    return xcpMaster->getXcpDaqRunningStatus();
}

bool A2L_Project::xcpDaqDynConfig()
{
    if (xcpMaster == NULL)
        return false;
    if (!getCurProXcpCanInitStatus())
        return false;
    if (!getCurProXcpSessionConnectedStatus())
        return false;

    if (selMeasVarList.count() == 0)
        return false;
    xcpMaster->setMeasVarListForDAQ(selMeasVarList);

    if (!xcpMaster->XCP_DAQ_Demo())
        return false;

    int count = 0;
    while (!getCurProXcpDaqRunningStatus() && count < 3)
    {
        xcpMaster->XCP_DAQ_Start_Stop(true);
        count++;
    }

    if (!getCurProXcpDaqRunningStatus())
        return false;
    else
        return true;

}

bool A2L_Project::setXcpDaqStartStop(bool status)
{
    if (xcpMaster == NULL)
        return false;
    if (!getCurProXcpCanInitStatus())
        return false;
    if (!getCurProXcpSessionConnectedStatus())
        return false;

    int count = 0;
    while ((getCurProXcpDaqRunningStatus() != status) && count < 3)
    {
        xcpMaster->XCP_DAQ_Start_Stop(true);
        count++;
    }
    if (getCurProXcpDaqRunningStatus() != status)
        return false;
    else
        return true;
}

bool A2L_Project::xcpDaqConfig()
{
    if (xcpMaster == NULL)
        return false;
    if (!getCurProXcpCanInitStatus())
        return false;
    if (!getCurProXcpSessionConnectedStatus())
        return false;

    if (selMeasVarList.count() == 0)
        return false;
    xcpMaster->setMeasVarListForDAQ(selMeasVarList);

    if (!xcpMaster->XCP_DAQ_CONFIG())
        return false;

    int count = 0;
    while (!getCurProXcpDaqRunningStatus() && count < 3)
    {
        xcpMaster->XCP_DAQ_Start_Stop(true);
        count++;
    }

    if (!getCurProXcpDaqRunningStatus())
        return false;
    else
        return true;
}

bool A2L_Project::xcpDaqStatConfig()
{
    if (xcpMaster == NULL)
        return false;
    if (!getCurProXcpCanInitStatus())
        return false;
    if (!getCurProXcpSessionConnectedStatus())
        return false;

    if (selMeasVarList.count() == 0)
        return false;
    xcpMaster->setMeasVarListForDAQ(selMeasVarList);

    if (!xcpMaster->XCP_DAQ_Demo())
        return false;

    int count = 0;
    while (!getCurProXcpDaqRunningStatus() && count < 3)
    {
        xcpMaster->XCP_DAQ_Start_Stop(true);
        count++;
    }

    if (!getCurProXcpDaqRunningStatus())
        return false;
    else
        return true;


}

quint32 A2L_Project::hexOrDecStrToU32Num(QString str)
{
    if (str.isEmpty())
        return 0;
    int len = str.length();
    if (str.mid(0, 2) == "0x")
    {
        quint32 num = str.mid(2, len-2).toUInt(0, 16);
        return num;
    }
    else
    {
        quint32 num = str.toUInt(0, 10);
        return num;
    }
}

quint32 A2L_Project::getSizeFromDataType(QString dataType)
{
    if (dataType.isEmpty())
        return 0;
    if (dataType == "UBYTE" || dataType == "SBYTE")
        return 1;
    else if (dataType == "UWORD" || dataType == "SWORD")
        return 2;
    else if (dataType == "ULONG" || dataType == "SLONG" || dataType == "FLOAT32_IEEE")
        return 4;
    else if (dataType == "A_UINT64" || dataType == "A_INT64" || dataType == "FLOAT64_IEEE")
        return 8;
    else
        return 0;
}

A2L_Event *A2L_Project::findEventByShortName(QString shortName)
{
    if (a2lEventList.isEmpty())
        return NULL;
    for (int i = 0; i < a2lEventList.count(); i++)
    {
        A2L_Event *event = a2lEventList.at(i);
        if (event->ShortName == shortName)
            return event;
    }
    return NULL;
}

A2L_Event *A2L_Project::findEventByChannelNumber(quint8 channelNumber)
{
    if (a2lEventList.isEmpty())
        return NULL;
    for (int i = 0; i < a2lEventList.count(); i++)
    {
        A2L_Event *event = a2lEventList.at(i);
        if (event->ChannelNumber == channelNumber)
            return event;
    }
    return NULL;
}

QList<A2L_Event *> A2L_Project::getEventList()
{
    return a2lEventList;
}

QList<A2L_EventCanId *> A2L_Project::getA2lEventCanIdList() const
{
    return a2lEventCanIdList;
}

QList<A2L_MemorySegment *> A2L_Project::getA2lMemSegList() const
{
    return a2lMemSegList;
}


void A2L_Project::getA2LDaqInfo(int indexXcpOnCan)
{
    if (a2lParser)
    {
        QMap<QString, QString> *daqInfoMap = a2lParser->GetDaqNodeInfoInXcpOnCanByName(a2lFullFileName, "DAQ", indexXcpOnCan);
        if (daqInfoMap != NULL)
        {
            if (a2lDaq == NULL)
                a2lDaq = new A2L_Daq;
            a2lDaq->Name = daqInfoMap->value("Name");
            a2lDaq->DaqType = daqInfoMap->value("DaqType");
            a2lDaq->GranularityOdtEntrySize = daqInfoMap->value("GranularityOdtEntrySize");
            a2lDaq->IdentificationField = daqInfoMap->value("IdentificationField");
            a2lDaq->AddressExtType = daqInfoMap->value("AddressExtType");
            a2lDaq->OptimisationType = daqInfoMap->value("OptimisationType");
            a2lDaq->Max_Daq = (quint16)hexOrDecStrToU32Num(daqInfoMap->value("MAX_DAQ"));
            a2lDaq->Min_Daq = (quint16)hexOrDecStrToU32Num(daqInfoMap->value("MIN_DAQ"));
            a2lDaq->Max_Event_Channel = (quint8)hexOrDecStrToU32Num(daqInfoMap->value("MAX_EVENT_CHANNEL"));
            a2lDaq->Max_Odt_Entry_Size_Daq = (quint8)hexOrDecStrToU32Num(daqInfoMap->value("MAX_ODT_ENTRY_SIZE_DAQ"));

        }
    }
}

void A2L_Project::getA2LDaqMemInfo(int indexXcpOnCan)
{
    if (a2lParser)
    {
        QMap<QString, QString> *daqMemInfoMap = a2lParser->GetDaqMemConsumpNodeInfoInXcpOnCanByName(a2lFullFileName, "DAQ_MEMORY_CONSUMPTION", indexXcpOnCan);
        if (daqMemInfoMap != NULL)
        {
            if (a2lDaqMem == NULL)
                a2lDaqMem = new A2L_Daq_Memory_Consumption;
            a2lDaqMem->Name = daqMemInfoMap->value("Name");
            a2lDaqMem->Daq_Memory_Limit = (quint32)hexOrDecStrToU32Num(daqMemInfoMap->value("DAQ_MEMORY_LIMIT"));
            a2lDaqMem->Daq_Size = (quint8)hexOrDecStrToU32Num(daqMemInfoMap->value("DAQ_SIZE"));
            a2lDaqMem->Odt_Size = (quint8)hexOrDecStrToU32Num(daqMemInfoMap->value("ODT_SIZE"));
            a2lDaqMem->Odt_Entry_Size = (quint8)hexOrDecStrToU32Num(daqMemInfoMap->value("ODT_ENTRY_SIZE"));
            a2lDaqMem->Odt_Daq_Buffer_Element_Size = (quint8)hexOrDecStrToU32Num(daqMemInfoMap->value("ODT_DAQ_BUFFER_ELEMENT_SIZE"));
            a2lDaqMem->Odt_Stim_Buffer_Element_Size = (quint8)hexOrDecStrToU32Num(daqMemInfoMap->value("ODT_STIM_BUFFER_ELEMENT_SIZE"));
        }
    }
}

void A2L_Project::getA2LEventListInfo(int indexXcpOnCan)
{
    if (a2lParser)
    {
        a2lEventInXcpOnCanList.clear();
        eventInXcpOnCanNameList.clear();
        for (int i = 0; i < 10; i++)
        {
            qDebug()<<"Event in XcpOnCan "<<i<< ":=========";
            QMap<QString, QString> *eventInfoMap = a2lParser->GetEventNodeInfoInXcpOnCanByNameAndIndex(a2lFullFileName, "EVENT", i, indexXcpOnCan);
            if (eventInfoMap != NULL)
            {
                A2L_Event *event = new A2L_Event;
                event->Name = eventInfoMap->value("Name");
                event->ChannelNumber = (quint8)hexOrDecStrToU32Num(eventInfoMap->value("ChannelNumber"));
                event->EventChannelName = eventInfoMap->value("EventChannelName");
                event->EventType = eventInfoMap->value("EventType");
                event->Priority = (quint8)hexOrDecStrToU32Num(eventInfoMap->value("EventChannelPriority"));
                event->ShortName = eventInfoMap->value("ShortName");
                event->TimeCycle = (quint8)hexOrDecStrToU32Num(eventInfoMap->value("EventChannelTimeCycle"));
                event->TimeUnit = (quint8)hexOrDecStrToU32Num(eventInfoMap->value("EventChannelTimeUnit"));

                if (event->TimeUnit == 6)   // 1ms
                {
                    event->Rate_ms = (float)event->TimeCycle * 1;
                }
                else if (event->TimeUnit == 5)   // 0.1ms
                {
                    event->Rate_ms = (float)event->TimeCycle * 0.1;
                }
                else if (event->TimeUnit == 7)   // 10ms
                {
                    event->Rate_ms = (float)event->TimeCycle * 10;
                }
                else if (event->TimeUnit == 8)   // 100ms
                {
                    event->Rate_ms = (float)event->TimeCycle * 100;
                }
                else
                {
                    event->Rate_ms = 0;
                }

                event->ShortName = QString::number(event->Rate_ms, 'g', 6) + "ms";

                a2lEventInXcpOnCanList.append(event);
                eventInXcpOnCanNameList.append(event->ShortName);

                eventSelMeasSizeSumHash.insert(event, 0);
                eventNameSelMeasSizeSumHash.insert(event->ShortName, 0);
                eventNameSizeMaxHash.insert(event->ShortName, 0);
                eventNameRateHash.insert(event->ShortName, event->Rate_ms);
            }
        }
        qDebug()<<"Event In XcpOnCan Count="<<a2lEventInXcpOnCanList.count();
    }
}

void A2L_Project::getA2LEventCanIdListInfo(int indexXcpOnCan)
{

}

void A2L_Project::getA2LDaqInfo()
{
    if (a2lParser)
    {
        QMap<QString, QString> *daqInfoMap = a2lParser->GetDaqNodeInfoByName(a2lFullFileName, "DAQ");
        if (daqInfoMap != NULL)
        {
            if (a2lDaq == NULL)
                a2lDaq = new A2L_Daq;
            a2lDaq->Name = daqInfoMap->value("Name");
            a2lDaq->DaqType = daqInfoMap->value("DaqType");
            a2lDaq->GranularityOdtEntrySize = daqInfoMap->value("GranularityOdtEntrySize");
            a2lDaq->IdentificationField = daqInfoMap->value("IdentificationField");
            a2lDaq->AddressExtType = daqInfoMap->value("AddressExtType");
            a2lDaq->OptimisationType = daqInfoMap->value("OptimisationType");
            a2lDaq->Max_Daq = (quint16)hexOrDecStrToU32Num(daqInfoMap->value("MAX_DAQ"));
            a2lDaq->Min_Daq = (quint16)hexOrDecStrToU32Num(daqInfoMap->value("MIN_DAQ"));
            a2lDaq->Max_Event_Channel = (quint8)hexOrDecStrToU32Num(daqInfoMap->value("MAX_EVENT_CHANNEL"));
            a2lDaq->Max_Odt_Entry_Size_Daq = (quint8)hexOrDecStrToU32Num(daqInfoMap->value("MAX_ODT_ENTRY_SIZE_DAQ"));

        }
    }
}

void A2L_Project::getA2LDaqMemInfo()
{
    if (a2lParser)
    {
        QMap<QString, QString> *daqMemInfoMap = a2lParser->GetDaqMemConsumpNodeInfoByName(a2lFullFileName, "DAQ_MEMORY_CONSUMPTION");
        if (daqMemInfoMap != NULL)
        {
            if (a2lDaqMem == NULL)
                a2lDaqMem = new A2L_Daq_Memory_Consumption;
            a2lDaqMem->Name = daqMemInfoMap->value("Name");
            a2lDaqMem->Daq_Memory_Limit = (quint32)hexOrDecStrToU32Num(daqMemInfoMap->value("DAQ_MEMORY_LIMIT"));
            a2lDaqMem->Daq_Size = (quint8)hexOrDecStrToU32Num(daqMemInfoMap->value("DAQ_SIZE"));
            a2lDaqMem->Odt_Size = (quint8)hexOrDecStrToU32Num(daqMemInfoMap->value("ODT_SIZE"));
            a2lDaqMem->Odt_Entry_Size = (quint8)hexOrDecStrToU32Num(daqMemInfoMap->value("ODT_ENTRY_SIZE"));
            a2lDaqMem->Odt_Daq_Buffer_Element_Size = (quint8)hexOrDecStrToU32Num(daqMemInfoMap->value("ODT_DAQ_BUFFER_ELEMENT_SIZE"));
            a2lDaqMem->Odt_Stim_Buffer_Element_Size = (quint8)hexOrDecStrToU32Num(daqMemInfoMap->value("ODT_STIM_BUFFER_ELEMENT_SIZE"));
        }
    }
}

void A2L_Project::getA2LEventListInfo()
{
    if (a2lParser)
    {
        a2lEventList.clear();
        eventNameList.clear();
        for (int i = 0; i < 10; i++)
        {
            qDebug()<<"Event "<<i<< ":=========";
            QMap<QString, QString> *eventInfoMap = a2lParser->GetEventNodeInfoByNameAndIndex(a2lFullFileName, "EVENT", i);
            if (eventInfoMap != NULL)
            {
                A2L_Event *event = new A2L_Event;
                event->Name = eventInfoMap->value("Name");
                event->ChannelNumber = (quint8)hexOrDecStrToU32Num(eventInfoMap->value("ChannelNumber"));
                event->EventChannelName = eventInfoMap->value("EventChannelName");
                event->EventType = eventInfoMap->value("EventType");
                event->Priority = (quint8)hexOrDecStrToU32Num(eventInfoMap->value("EventChannelPriority"));
                event->ShortName = eventInfoMap->value("ShortName");
                event->TimeCycle = (quint8)hexOrDecStrToU32Num(eventInfoMap->value("EventChannelTimeCycle"));
                event->TimeUnit = (quint8)hexOrDecStrToU32Num(eventInfoMap->value("EventChannelTimeUnit"));

                if (event->TimeUnit == 6)   // 1ms
                {
                    event->Rate_ms = (float)event->TimeCycle * 1;
                }
                else if (event->TimeUnit == 5)   // 0.1ms
                {
                    event->Rate_ms = (float)event->TimeCycle * 0.1;
                }
                else if(event->TimeUnit == 7)  //10ms
                {
                    event->Rate_ms = (float)event->TimeCycle * 10;
                }
                else if(event->TimeUnit == 8)  //100ms
                {
                    event->Rate_ms = (float)event->TimeCycle * 100;
                }
                else
                {
                    event->Rate_ms = 0;
                }

                event->ShortName = QString::number(event->Rate_ms, 'g', 6) + "ms";

                a2lEventList.append(event);
                eventNameList.append(event->ShortName);

                eventSelMeasSizeSumHash.insert(event, 0);
                eventNameSelMeasSizeSumHash.insert(event->ShortName, 0);
                eventNameSizeMaxHash.insert(event->ShortName, 0);
                eventNameRateHash.insert(event->ShortName, event->Rate_ms);
            }
        }
        qDebug()<<"Event Count="<<a2lEventList.count();
    }
}

void A2L_Project::getA2LXcpOnCanListInfo()
{
    if (a2lParser)
    {
        a2lXcpOnCanList.clear();
        xcpOnCanInstanceNameList.clear();
        for (int i = 0; i < 5; i++)
        {
            qDebug()<<"XCP_ON_CAN "<<i<< ":=========";
            QMap<QString, QString> *xcpOnCanInfoMap = a2lParser->GetXcpOnCanNodeInfoByNameAndIndex(a2lFullFileName, "XCP_ON_CAN", i);

            QMap<QString, QString> *canfdInfoMap = a2lParser->GetCanFdNodeInfoInXcpOnCanByName(a2lFullFileName, "CAN_FD", i);

            if (xcpOnCanInfoMap != NULL)
            {
                A2L_Xcp_On_Can *xcp_on_can = new A2L_Xcp_On_Can;
                xcp_on_can->Name = xcpOnCanInfoMap->value("Event");
                xcp_on_can->CAN_ID_MASTER = (quint32)hexOrDecStrToU32Num(xcpOnCanInfoMap->value("CAN_ID_MASTER"));
                xcp_on_can->CAN_ID_SLAVE = (quint32)hexOrDecStrToU32Num(xcpOnCanInfoMap->value("CAN_ID_SLAVE"));
                xcp_on_can->BAUDRATE = (quint64)hexOrDecStrToU32Num(xcpOnCanInfoMap->value("BAUDRATE"));
                xcp_on_can->SAMPLE_POINT = (quint8)hexOrDecStrToU32Num(xcpOnCanInfoMap->value("SAMPLE_POINT"));
                xcp_on_can->BTL_CYCLES = (quint8)hexOrDecStrToU32Num(xcpOnCanInfoMap->value("BTL_CYCLES"));
                xcp_on_can->SJW = (quint8)hexOrDecStrToU32Num(xcpOnCanInfoMap->value("SJW"));
                xcp_on_can->MAX_BUS_LOAD = (quint8)hexOrDecStrToU32Num(xcpOnCanInfoMap->value("MAX_BUS_LOAD"));
                xcp_on_can->SAMPLE_RATE = xcpOnCanInfoMap->value("SAMPLE_RATE");
                xcp_on_can->SYNC_EDGE = xcpOnCanInfoMap->value("SYNC_EDGE");
                xcp_on_can->TRANSPORT_LAYER_INSTANCE = xcpOnCanInfoMap->value("TRANSPORT_LAYER_INSTANCE");

                if(canfdInfoMap)
                {
                    xcp_on_can->IsCANFD = true;
                    xcp_on_can->FD_BAUDRATE = (quint64)hexOrDecStrToU32Num(canfdInfoMap->value("CAN_FD_DATA_TRANSFER_BAUDRATE"));
                    xcp_on_can->FD_SAMPLE_POINT = (quint8)hexOrDecStrToU32Num(canfdInfoMap->value("SAMPLE_POINT"));
                    xcp_on_can->FD_BTL_CYCLES = (quint8)hexOrDecStrToU32Num(canfdInfoMap->value("BTL_CYCLES"));
                    xcp_on_can->FD_SJW = (quint8)hexOrDecStrToU32Num(canfdInfoMap->value("SJW"));
                    xcp_on_can->FD_SECOND_SAMPLE_POINT = (quint8)hexOrDecStrToU32Num(canfdInfoMap->value("SECONDARY_SAMPLE_POINT"));
                    xcp_on_can->MAX_DLC = (quint8)hexOrDecStrToU32Num(canfdInfoMap->value("MAX_DLC"));
                }

                //qDebug()<<"ID1:"<<xcp_on_can->CAN_ID_MASTER<<", ID2:"<<xcp_on_can->CAN_ID_SLAVE;
                xcp_on_can->CAN_ID_MASTER &= 0x1FFFFFFF; // 限定最长29位拓展帧
                xcp_on_can->CAN_ID_SLAVE &= 0x1FFFFFFF;


                if(xcp_on_can->TRANSPORT_LAYER_INSTANCE.isEmpty())
                {
                    xcp_on_can->TRANSPORT_LAYER_INSTANCE = "XCP_ON_CAN_"+QString::number(i);
                }

                a2lXcpOnCanList.append(xcp_on_can);
                xcpOnCanInstanceNameList.append(xcp_on_can->TRANSPORT_LAYER_INSTANCE);

            }
        }
        qDebug()<<"A2L file: XCP_ON_CAN Count="<<a2lXcpOnCanList.count();

    }
}

void A2L_Project::getA2LEventCanIdListInfo()
{
    if (a2lParser)
    {
        a2lEventCanIdList.clear();
        eventCanIdNameList.clear();
        for (int i = 0; i < 10; i++)
        {
            qDebug()<<"EventCanId "<<i<< ":=========";
            QMap<QString, QString> *eventCanIdInfoMap = a2lParser->GetEventCanIdNodeInfoByNameAndIndex(a2lFullFileName, "EVENT", i);
            if (eventCanIdInfoMap != NULL)
            {
                A2L_EventCanId *eventCanId = new A2L_EventCanId;
                eventCanId->Name = eventCanIdInfoMap->value("Name");
                eventCanId->EventNumber = (quint8)hexOrDecStrToU32Num(eventCanIdInfoMap->value("EventNumber"));
                eventCanId->Event_CAN_ID = hexOrDecStrToU32Num(eventCanIdInfoMap->value("Event_CanId"));

                a2lEventCanIdList.append(eventCanId);
                eventCanIdNameList.append(QString(eventCanId->EventNumber));

                A2L_Event *event = findEventByChannelNumber(eventCanId->EventNumber);
                if(event)
                {
                    event->eventCanId = eventCanId;
                }

            }
        }
        qDebug()<<"EventCanId Count="<<a2lEventCanIdList.count();
    }
}

void A2L_Project::getA2LDaqListListInfo()
{
    if (a2lParser)
    {
        a2lDaqListList.clear();
        daqListNameList.clear();
        for (int i = 0; i < 10; i++)
        {
            qDebug()<<"DaqList "<<i<< ":=========";
            QMap<QString, QString> *daqListInfoMap = a2lParser->GetDaqListNodeInfoByNameAndIndex(a2lFullFileName, "DAQ_LIST", i);
            if (daqListInfoMap != NULL)
            {
                A2L_Daq_List *daqList = new A2L_Daq_List;
                daqList->Name = daqListInfoMap->value("Name");
                daqList->DaqListType = daqListInfoMap->value("Daq_List_Type");
                daqList->DaqListNumber = (quint16)hexOrDecStrToU32Num(daqListInfoMap->value("DaqListNumber"));
                daqList->Max_Odt = (quint8)hexOrDecStrToU32Num(daqListInfoMap->value("MAX_ODT"));
                daqList->Max_Odt_Entries = (quint8)hexOrDecStrToU32Num(daqListInfoMap->value("MAX_ODT_ENTRIES"));
                daqList->First_Pid = (quint8)hexOrDecStrToU32Num(daqListInfoMap->value("FIRST_PID"));
                daqList->Event_Fixed = (quint8)hexOrDecStrToU32Num(daqListInfoMap->value("EVENT_FIXED"));

                a2lDaqListList.append(daqList);
                daqListNameList.append(QString::number(daqList->DaqListNumber));

                A2L_Event *event = findEventByChannelNumber(daqList->Event_Fixed);
                quint32 sizeMax = daqList->Max_Odt * daqList->Max_Odt_Entries;
                eventNameSizeMaxHash.insert(event->ShortName, sizeMax);
                qDebug()<<"Event="<<event->ShortName<<", sizeMax="<<sizeMax;

            }
        }
        qDebug()<<"DaqList Count="<<a2lDaqListList.count();
    }
}

void A2L_Project::getA2LDaqListCanIdListInfo()
{
    if (a2lParser)
    {
        a2lDaqListCanIdList.clear();
        daqListCanIdNameList.clear();
        for (int i = 0; i < 10; i++)
        {
            qDebug()<<"DaqListCanId "<<i<< ":=========";
            QMap<QString, QString> *daqListCanIdInfoMap = a2lParser->GetDaqListCanIdNodeInfoByNameAndIndex(a2lFullFileName, "DAQ_LIST_CAN_ID", i);
            if (daqListCanIdInfoMap != NULL)
            {
                A2L_DaqListCanId *daqListCanId = new A2L_DaqListCanId;
                daqListCanId->Name = daqListCanIdInfoMap->value("Name");
                daqListCanId->DaqListNumber = (quint16)hexOrDecStrToU32Num(daqListCanIdInfoMap->value("DaqListNumber"));
                daqListCanId->DaqList_CAN_ID = hexOrDecStrToU32Num(daqListCanIdInfoMap->value("DaqList_CanId"));


                a2lDaqListCanIdList.append(daqListCanId);
                daqListCanIdNameList.append(QString::number(daqListCanId->DaqListNumber));
            }
        }
        qDebug()<<"DaqListCanId Count="<<a2lDaqListCanIdList.count();
    }
}

void A2L_Project::getA2LMemSegListInfo()
{
    if (a2lParser)
    {
        a2lMemSegList.clear();
        int memSegMaxNum = 3, pageMaxNum = 3;
        for (int i = 0; i < memSegMaxNum; i++)
        {
            qDebug()<<"MemorySegment "<<i<< ":=========";
            QMap<QString, QString> *memSegInfoMap = a2lParser->GetMemorySegmentNodeInfoByNameAndIndex(a2lFullFileName, "MEMORY_SEGMENT", i);
            if (memSegInfoMap != NULL)
            {
                A2L_MemorySegment *memSeg = new A2L_MemorySegment;
                memSeg->memName = memSegInfoMap->value("Name");
                memSeg->description = memSegInfoMap->value("LongIdentifier");
                memSeg->prgType = memSegInfoMap->value("PrgType");
                memSeg->memType = memSegInfoMap->value("Memorytype");
                memSeg->memAttr = memSegInfoMap->value("Attribute");
                memSeg->memAddress = (quint32)hexOrDecStrToU32Num(memSegInfoMap->value("Address"));
                memSeg->memSize = (quint32)hexOrDecStrToU32Num(memSegInfoMap->value("Size"));
                memSeg->offset1 = (qint16)hexOrDecStrToU32Num(memSegInfoMap->value("Offset1"));
                memSeg->offset2 = (qint16)hexOrDecStrToU32Num(memSegInfoMap->value("Offset2"));
                memSeg->offset3 = (qint16)hexOrDecStrToU32Num(memSegInfoMap->value("Offset3"));
                memSeg->offset4 = (qint16)hexOrDecStrToU32Num(memSegInfoMap->value("Offset4"));
                memSeg->offset5 = (qint16)hexOrDecStrToU32Num(memSegInfoMap->value("Offset5"));

                memSeg->segIndex = (quint8)hexOrDecStrToU32Num(memSegInfoMap->value("SEGMENT_INDEX"));
                memSeg->pageNum = (quint8)hexOrDecStrToU32Num(memSegInfoMap->value("PAGE_NUMBER"));
                memSeg->addExtension = (quint8)hexOrDecStrToU32Num(memSegInfoMap->value("ADDRESS_EXTENSION"));
                memSeg->compressMethod = (quint8)hexOrDecStrToU32Num(memSegInfoMap->value("COMPRESSION_METHOD"));
                memSeg->encryptionMethod = (quint8)hexOrDecStrToU32Num(memSegInfoMap->value("ENCRYPTION_METHOD"));
                memSeg->checksumType = memSegInfoMap->value("CHECKSUM_TYPE");
                memSeg->maxBlockSize = (quint32)hexOrDecStrToU32Num(memSegInfoMap->value("MAX_BLOCK_SIZE"));

                a2lMemSegList.append(memSeg);

                for (int j = 0; j < pageMaxNum; j++)
                {
                    qDebug()<<"Page "<<j<< ":=========";
                    QMap<QString, QString> *pageInfoMap = a2lParser->GetPageNodeInfoInMemorySegmentByNameAndIndex(a2lFullFileName, "PAGE", j, i);
                    if(pageInfoMap != NULL)
                    {
                        A2L_Page *page = new A2L_Page;
                        page->pageIndex = (quint8)hexOrDecStrToU32Num(pageInfoMap->value("PAGE_INDEX"));
                        page->ecuAccessType = pageInfoMap->value("ECU_ACCESS_TYPE");
                        page->xcpReadAccessType = pageInfoMap->value("XCP_READ_ACCESS_TYPE");
                        page->xcpWriteAccessType = pageInfoMap->value("XCP_WRITE_ACCESS_TYPE");

                        memSeg->a2lPageList.append(page);
                    }
                }
            }
        }
        qDebug()<<"a2lMemSegList Count="<<a2lMemSegList.count();
    }
}
