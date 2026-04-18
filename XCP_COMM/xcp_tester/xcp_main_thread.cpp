#include "xcp_main_thread.h"

XCP_Main_Thread::XCP_Main_Thread(QObject *parent, A2LProjectWin *win):
    QThread(parent),   a2lWin(win)
{

}

XCP_Main_Thread::~XCP_Main_Thread()
{
    stopXCP();
}

void XCP_Main_Thread::setCharsDownloadActive()
{
    emit sigDownload();
}

void XCP_Main_Thread::setMdfRecordStatus(bool status)
{
    emit recordActive(status);
}

void XCP_Main_Thread::run()
{
    init();

    startXCP();

    exec();

}

void XCP_Main_Thread::fromReadSMToMeasVars()
{
    if(smRead == NULL)
        return;

    QByteArray buffer(sizeRead, Qt::Uninitialized);

    if(!smRead->isAttached())
    {
        if(!smRead->attach())
        {
            qDebug()<<"Unable attach to read sharedmemory.";
            return;
        }
    }
    qreal readTime = 0;
    smRead->lock();
    memcpy((char*)&readTime, (char*)smRead->data(), 8);
    memcpy(buffer.data(), (char*)smRead->data()+8, sizeRead);
    smRead->unlock();

    fromReadRawDataToMeasVars(buffer.data(), sizeRead);
}

void XCP_Main_Thread::fromReadRawDataToMeasVars(char *data, quint64 size)
{
    for(int i = 0; i < measPamList.count(); i++)
    {
        A2L_VarMeas *measVar = measPamList.at(i);

        int startByte = measVar->StartByte;
        int dataSize = measVar->DataSizeAG;
        QString type = measVar->DataType;

        if(startByte + dataSize > (int)size)
            continue;

        QByteArray temp(data+startByte, dataSize);

        qreal measValue = 0;
        switch (dataSize) {
        case 1:
        {
            if(type == "UBYTE")
                measValue = *(quint8*)temp.constData();
            else if(type == "SBYTE")
                measValue = *(qint8*)temp.constData();
            break;
        }
        case 2:
        {
            if(type == "UWORD")
                measValue = *(quint16*)temp.constData();
            else if(type == "SWORD")
                measValue = *(qint16*)temp.constData();
            break;
        }
        case 4:
        {
            if(type == "ULONG")
                measValue = *(quint32*)temp.constData();
            else if(type == "SLONG")
                measValue = *(qint32*)temp.constData();
            else if(type == "FLOAT32_IEEE")
                measValue = *(float*)temp.constData();
            break;
        }
        case 8:
        {
            if(type == "A_UINT64")
                measValue = *(quint64*)temp.constData();
            else if(type == "A_INT64")
                measValue = *(qint64*)temp.constData();
            else if(type == "FLOAT64_IEEE")
                measValue = *(qreal*)temp.constData();
            break;
        }
        default:
            break;
        }

        qreal measPhysValue = 0;
        if (measVar->ConversionType == "RAT_FUNC")
        {
             measPhysValue = xcpMaster->RatFuncConvFromIntToPhys(measValue, measVar->CoefList);
        }
        else if (measVar->ConversionType == "TAB_VERB")
        {
            measPhysValue = measValue;
        }
        else
        {
            measPhysValue = measValue;
        }

        measVar->setValue(measPhysValue);
    }

}


void XCP_Main_Thread::setCharMapPamList(const QList<A2L_VarChar *> &value)
{
    charMapPamList = value;
}

void XCP_Main_Thread::setDownloadCharList(const QList<A2L_VarChar *> &value)
{
    if(xcpMaster)
    {
        xcpMaster->setCharList(value);
    }
}

void XCP_Main_Thread::setCharPamList(const QList<A2L_VarChar *> &value)
{
    charPamList = value;
}

void XCP_Main_Thread::setDaqMeasPamList(const QList<A2L_VarMeas *> &value)
{
    daqMeasPamList = value;
}

void XCP_Main_Thread::setMeasPamList(const QList<A2L_VarMeas *> &value)
{
    measPamList = value;
}


QList<A2L_VarChar *> XCP_Main_Thread::getCharMapPamList() const
{
    return charMapPamList;
}

QList<A2L_VarChar *> XCP_Main_Thread::getCharPamList() const
{
    return charPamList;
}

QList<A2L_VarMeas *> XCP_Main_Thread::getDaqMeasPamList() const
{
    return daqMeasPamList;
}

QList<A2L_VarMeas *> XCP_Main_Thread::getMeasPamList() const
{
    return measPamList;
}
/*
void XCP_Main_Thread::readCurSetting()
{
    QString appPath = QApplication::applicationDirPath();
    QSettings setting(appPath + "//setting//" + SET_FILENAME, QSettings::IniFormat);


    QString curProjName = setting.value("CurSetProject").toString();

    int sizeBitRead = 0;
    int sizeBitWrite = 0;

    int size = setting.beginReadArray("SetProjectList");
    for (int i = 0; i < size; i++)
    {
        setting.setArrayIndex(i);
        Proj proj;
        proj.Proj_name = setting.value("SetProjectName").toString();
        proj.intfName = setting.value("Interface").toString();
        proj.a2lProjectName = setting.value("A2LProjectName").toString();
        proj.a2lFilePath = setting.value("A2LFilePath").toString();
        proj.hexFilePath = setting.value("HexFilePath").toString();
        proj.index_XcpOnCan = setting.value("Index_XcpOnCan").toInt();
        proj.baudrate = setting.value("Baudrate").toInt();
        proj.id_CMD = setting.value("ID_CMD").toInt();
        proj.id_RES = setting.value("ID_RES").toInt();

        sizeBitRead = 0;
        sizeBitWrite = 0;

        // load readPams
        QList<XCP_PAM> pamList;

        QStringList pamNames, pamPollTypes, pamDefaultValues, pamCaliTypes, pamDaqEventTypes;
        QString pamName = setting.value("readPamNames").toString();
        QString pamPollType = setting.value("readPamPollTypes").toString();
        QString pamDefaultValue = setting.value("readPamDefaultValues").toString();

        if (!pamName.isEmpty())
        {
            pamNames = pamName.split("|");
            pamPollTypes = pamPollType.split("|");
            pamDefaultValues = pamDefaultValue.split("|");
        }
        for(int j = 0; j < pamNames.count(); j++)
        {
            XCP_PAM pam;
            pam.pamName = pamNames.at(j);
            pam.pollType = pamPollTypes.at(j);
            QString valueStr = pamDefaultValues.at(j);
            pam.pamDefaultValue = valueStr.toDouble();
            pamList.append(pam);

            int lenBit = 0;
            sizeBitRead += lenBit;
        }
        proj.readPamList = pamList;


        // load writePams
        pamList.clear();
        pamNames.clear();
        pamDefaultValues.clear();

        QString pamCaliType;

        pamName = setting.value("writePamNames").toString();
        pamDefaultValue = setting.value("writePamDefaultValues").toString();
        pamCaliType = setting.value("writePamTypes").toString();

        if (!pamName.isEmpty())
        {
            pamNames = pamName.split("|");
            pamDefaultValues = pamDefaultValue.split("|");
            pamCaliTypes = pamCaliType.split("|");
        }
        for(int j = 0; j < pamNames.count(); j++)
        {
            XCP_PAM pam;
            pam.pamName = pamNames.at(j);
            QString valueStr = pamDefaultValues.at(j);
            pam.pamDefaultValue = valueStr.toDouble();
            pam.caliType = pamCaliTypes.at(j);

            pamList.append(pam);

            int lenBit = 0;
            sizeBitWrite += lenBit;
        }
        proj.writePamList = pamList;

        // load daqReadPams
        pamList.clear();
        pamNames.clear();
        pamDaqEventTypes.clear();
        pamDefaultValues.clear();
        pamName = setting.value("daqReadPamNames").toString();
        QString pamDaqEventType = setting.value("daqReadPamEventTypes").toString();
        pamDefaultValue = setting.value("daqReadPamDefaultValues").toString();

        if (!pamName.isEmpty())
        {
            pamNames = pamName.split("|");
            pamDaqEventTypes = pamDaqEventType.split("|");
            pamDefaultValues = pamDefaultValue.split("|");
        }

        for(int j = 0; j < pamNames.count(); j++)
        {
            XCP_PAM pam;
            pam.pamName = pamNames.at(j);
            pam.daqEventTime = pamDaqEventTypes.at(j);
            QString valueStr = pamDefaultValues.at(j);
            pam.pamDefaultValue = valueStr.toDouble();
            pamList.append(pam);
        }
        proj.daqReadPamList = pamList;

        if (curProjName == proj.Proj_name)
        {
            curProj = proj;
        }

        if(a2lWin)
        {
            if(!a2lWin->getProjectByName(proj.a2lProjectName))
            {
                a2lWin->addA2LProject(proj.a2lFilePath, proj.hexFilePath);
            }
        }

    }
    setting.endArray();

    smKeyRead = "RP_" + curProj.Proj_name;
    smKeyWrite = "WP_" + curProj.Proj_name;

    qDebug()<<"CurSetProj"<<curProj.Proj_name<<", numRead:"<<curProj.readPamList.count()<<", numWrite:"<<curProj.writePamList.count()
           <<", numReadDAQ:"<<curProj.daqReadPamList.count();
}
*/
bool XCP_Main_Thread::genXML()
{
    QString appPath = QApplication::applicationDirPath();
    QString FileName = "XCP_COMM_"+curProj.Proj_name + ".xml";
    QString xmlPath = appPath + "/HQ_Testing/pamFactory/" + FileName;

    if (xmlPath.isEmpty())
        return false;

    QFile xmlFile(xmlPath);
    if (!xmlFile.open(QFile::WriteOnly | QFile::Text))
    {
        qDebug()<<tr("Cannot write file %1:\n%2.").arg(xmlPath).arg(xmlFile.errorString());
        return false;
    }

    qDebug()<<"==Generating pam transfer xml file:"<<xmlPath;
    QDomDocument xml("XCP");

    QDomElement root = xml.createElement("Root");

    xml.appendChild(root);

    QList<A2L_VarMeas*> pamList_r = measPamList;

    int sigIndex_r = 0;
    QDomElement ReadSigElem = xml.createElement(smKeyRead);
    root.appendChild(ReadSigElem);

    // add time channel
    QDomElement timeElem_r = xml.createElement("Param_"+QString::number(sigIndex_r));
    ReadSigElem.appendChild(timeElem_r);
    timeElem_r.setAttribute("Unit", "s");
    timeElem_r.setAttribute("DefaultValue", QString::number(0,10));
    timeElem_r.setAttribute("Minimum", QString::number(0, 10));
    timeElem_r.setAttribute("Maximum", QString::number(0, 10));
    timeElem_r.setAttribute("ScalingOffset", QString::number(0));
    timeElem_r.setAttribute("ScalingFactor", QString::number(1));
    timeElem_r.setAttribute("LengthBit", QString::number(64, 10));
    timeElem_r.setAttribute("StartBit", QString::number(0, 10));
    timeElem_r.setAttribute("StartBit_Oth", QString::number(0, 10));
    timeElem_r.setAttribute("ByteOrder", QString::number(0,10));
    timeElem_r.setAttribute("Type", QString::number(3, 10));
    timeElem_r.setAttribute("Param_Name", "Time");

    sigIndex_r++;


    for(int i=0; i < pamList_r.count(); i++)
    {
        A2L_VarMeas *measVar = pamList_r.at(i);
        QDomElement sigElem = xml.createElement("Param_"+QString::number(sigIndex_r));
        ReadSigElem.appendChild(sigElem);

        QString dataType = measVar->DataType;
        int type = 1;
        if(dataType == "UBYTE" || dataType == "UWORD" || dataType == "ULONG" || dataType == "A_UINT64")
            type = 1;
        else if(dataType == "SBYTE" || dataType == "SWORD" || dataType == "SLONG" || dataType == "A_INT64")
            type = 0;
        else if(dataType == "FLOAT32_IEEE")
            type = 2;
        else if(dataType == "FLOAT64_IEEE")
            type = 3;

        sigElem.setAttribute("Unit", measVar->Unit);
        sigElem.setAttribute("DefaultValue", QString::number(0, 'g', 6));
        sigElem.setAttribute("Minimum", QString::number(measVar->LowerLimit, 'g', 6));
        sigElem.setAttribute("Maximum", QString::number(measVar->UpperLimit, 'g', 6));
        sigElem.setAttribute("ScalingOffset", QString::number(measVar->ScalingOffset, 'g', 6));
        sigElem.setAttribute("ScalingFactor", QString::number(measVar->ScalingFactor, 'g', 6));
        sigElem.setAttribute("LengthBit", QString::number(measVar->DataSizeAG*8, 10));
        sigElem.setAttribute("StartBit", QString::number(measVar->StartByte*8+64, 10));
        sigElem.setAttribute("ByteOrder", QString::number(0,10));
        sigElem.setAttribute("Type", QString::number(type, 10));
        sigElem.setAttribute("Param_Name", measVar->Name);

        sigIndex_r++;
    }
    ReadSigElem.setAttribute("Size_Bytes", 8+sizeRead);
    ReadSigElem.setAttribute("Sample_Rate", QString::number(100,10));
    ReadSigElem.setAttribute("isRead", QString::number(1,10));
    //////////////////////////////////////////////////////////////////////////////
    /*
    QList<A2L_VarMeas*> pamList_r_daq = daqMeasPamList;

    int sigIndex_r_daq = 0;
    QDomElement ReadSigElemDAQ = xml.createElement(smKeyReadDAQ);
    root.appendChild(ReadSigElemDAQ);

    // add time channel
    QDomElement timeElem_r_daq = xml.createElement("Param_"+QString::number(sigIndex_r_daq));
    ReadSigElemDAQ.appendChild(timeElem_r_daq);
    timeElem_r_daq.setAttribute("Unit", "s");
    timeElem_r_daq.setAttribute("DefaultValue", QString::number(0,10));
    timeElem_r_daq.setAttribute("Minimum", QString::number(0, 10));
    timeElem_r_daq.setAttribute("Maximum", QString::number(0, 10));
    timeElem_r_daq.setAttribute("ScalingOffset", QString::number(0));
    timeElem_r_daq.setAttribute("ScalingFactor", QString::number(1));
    timeElem_r_daq.setAttribute("LengthBit", QString::number(64, 10));
    timeElem_r_daq.setAttribute("StartBit", QString::number(0, 10));
    timeElem_r_daq.setAttribute("StartBit_Oth", QString::number(0, 10));
    timeElem_r_daq.setAttribute("ByteOrder", QString::number(0,10));
    timeElem_r_daq.setAttribute("Type", QString::number(3, 10));
    timeElem_r_daq.setAttribute("Param_Name", "Time");

    sigIndex_r_daq++;


    for(int i=0; i < pamList_r_daq.count(); i++)
    {
        A2L_VarMeas *measVar = pamList_r_daq.at(i);
        QDomElement sigElem = xml.createElement("Param_"+QString::number(sigIndex_r_daq));
        ReadSigElem.appendChild(sigElem);

        QString dataType = measVar->DataType;
        int type = 1;
        if(dataType == "UBYTE" || dataType == "UWORD" || dataType == "ULONG" || dataType == "A_UINT64")
            type = 1;
        else if(dataType == "SBYTE" || dataType == "SWORD" || dataType == "SLONG" || dataType == "A_INT64")
            type = 0;
        else if(dataType == "FLOAT32_IEEE")
            type = 2;
        else if(dataType == "FLOAT64_IEEE")
            type = 3;

        sigElem.setAttribute("Unit", measVar->Unit);
        sigElem.setAttribute("DefaultValue", QString::number(0, 'g', 6));
        sigElem.setAttribute("Minimum", QString::number(measVar->LowerLimit, 'g', 6));
        sigElem.setAttribute("Maximum", QString::number(measVar->UpperLimit, 'g', 6));
        sigElem.setAttribute("ScalingOffset", QString::number(measVar->ScalingOffset, 'g', 6));
        sigElem.setAttribute("ScalingFactor", QString::number(measVar->ScalingFactor, 'g', 6));
        sigElem.setAttribute("LengthBit", QString::number(measVar->DataSizeAG*8, 10));
        sigElem.setAttribute("StartBit", QString::number(measVar->StartByte*8+64, 10));
        sigElem.setAttribute("ByteOrder", QString::number(0,10));
        sigElem.setAttribute("Type", QString::number(type, 10));
        sigElem.setAttribute("Param_Name", measVar->Name);

        sigIndex_r_daq++;
    }
    ReadSigElemDAQ.setAttribute("Size_Bytes", 8+sizeReadDAQ);
    ReadSigElemDAQ.setAttribute("Sample_Rate", QString::number(100,10));
    ReadSigElemDAQ.setAttribute("isRead", QString::number(1,10));
    */
    //////////////////////////////////////////////////////////////////////////////

    QHash<QString, QList<A2L_VarMeas *>> dgNameVarHash;
    if(xcpMaster)
    {
        dgNameVarHash = xcpMaster->getDgNameVarHash();
    }
    QStringList keys = dgNameVarHash.uniqueKeys();
    for(int i = 0; i < keys.count(); i++)
    {
        QString dgName = keys.at(i);
        QList<A2L_VarMeas *> vars = dgNameVarHash.value(dgName);
        if(vars.isEmpty())
            continue;
        if(dgName.contains("poll"))
            continue;

        QList<A2L_VarMeas*> pamList_r_daq = vars;

        int sigIndex_r_daq = 0;
        quint32 blockSize = 0;
        quint32 startBit = 0;
        QDomElement ReadSigElemDAQ = xml.createElement(dgName);
        root.appendChild(ReadSigElemDAQ);

        // add time channel
        QDomElement timeElem_r_daq = xml.createElement("Param_"+QString::number(sigIndex_r_daq));
        ReadSigElemDAQ.appendChild(timeElem_r_daq);
        timeElem_r_daq.setAttribute("Unit", "s");
        timeElem_r_daq.setAttribute("DefaultValue", QString::number(0,10));
        timeElem_r_daq.setAttribute("Minimum", QString::number(0, 10));
        timeElem_r_daq.setAttribute("Maximum", QString::number(0, 10));
        timeElem_r_daq.setAttribute("ScalingOffset", QString::number(0));
        timeElem_r_daq.setAttribute("ScalingFactor", QString::number(1));
        timeElem_r_daq.setAttribute("LengthBit", QString::number(64, 10));
        timeElem_r_daq.setAttribute("StartBit", QString::number(startBit, 10));
        timeElem_r_daq.setAttribute("StartBit_Oth", QString::number(0, 10));
        timeElem_r_daq.setAttribute("ByteOrder", QString::number(0,10));
        timeElem_r_daq.setAttribute("Type", QString::number(3, 10));
        timeElem_r_daq.setAttribute("Param_Name", "Time");

        sigIndex_r_daq++;
        blockSize += 8;
        startBit += 64;


        for(int i=0; i < pamList_r_daq.count(); i++)
        {
            A2L_VarMeas *measVar = pamList_r_daq.at(i);
            QDomElement sigElem = xml.createElement("Param_"+QString::number(sigIndex_r_daq));
            ReadSigElemDAQ.appendChild(sigElem);

            QString dataType = measVar->DataType;
            int type = 1;
            if(dataType == "UBYTE" || dataType == "UWORD" || dataType == "ULONG" || dataType == "A_UINT64")
                type = 1;
            else if(dataType == "SBYTE" || dataType == "SWORD" || dataType == "SLONG" || dataType == "A_INT64")
                type = 0;
            else if(dataType == "FLOAT32_IEEE")
                type = 2;
            else if(dataType == "FLOAT64_IEEE")
                type = 3;

            sigElem.setAttribute("Unit", measVar->Unit);
            sigElem.setAttribute("DefaultValue", QString::number(0, 'g', 6));
            sigElem.setAttribute("Minimum", QString::number(measVar->LowerLimit, 'g', 6));
            sigElem.setAttribute("Maximum", QString::number(measVar->UpperLimit, 'g', 6));
            sigElem.setAttribute("ScalingOffset", QString::number(measVar->ScalingOffset, 'g', 6));
            sigElem.setAttribute("ScalingFactor", QString::number(measVar->ScalingFactor, 'g', 6));
            sigElem.setAttribute("LengthBit", QString::number(measVar->DataSizeAG*8, 10));
            sigElem.setAttribute("StartBit", QString::number(startBit, 10));
            sigElem.setAttribute("ByteOrder", QString::number(0,10));
            sigElem.setAttribute("Type", QString::number(type, 10));
            sigElem.setAttribute("Param_Name", measVar->Name);

            sigIndex_r_daq++;
            blockSize += measVar->DataSizeAG;
            startBit += (8 * measVar->DataSizeAG);
        }
        ReadSigElemDAQ.setAttribute("Size_Bytes", blockSize);
        ReadSigElemDAQ.setAttribute("Sample_Rate", QString::number(100,10));
        ReadSigElemDAQ.setAttribute("isRead", QString::number(1,10));
    }

    /////////////////////////////////////////////////////////////////////////////
    QList<A2L_VarChar*> pamList_w = charPamList;

    int sigIndex_w = 0;
    QDomElement WriteSigElem = xml.createElement(smKeyWrite);
    root.appendChild(WriteSigElem);

    // add time channel
    QDomElement timeElem_w = xml.createElement("Param_"+QString::number(sigIndex_w));
    WriteSigElem.appendChild(timeElem_w);
    timeElem_w.setAttribute("Unit", "s");
    timeElem_w.setAttribute("DefaultValue", QString::number(0,10));
    timeElem_w.setAttribute("Minimum", QString::number(0, 10));
    timeElem_w.setAttribute("Maximum", QString::number(0, 10));
    timeElem_w.setAttribute("ScalingOffset", QString::number(0));
    timeElem_w.setAttribute("ScalingFactor", QString::number(1));
    timeElem_w.setAttribute("LengthBit", QString::number(64, 10));
    timeElem_w.setAttribute("StartBit", QString::number(0, 10));
    timeElem_w.setAttribute("StartBit_Oth", QString::number(0, 10));
    timeElem_w.setAttribute("ByteOrder", QString::number(0,10));
    timeElem_w.setAttribute("Type", QString::number(3, 10));
    timeElem_w.setAttribute("Param_Name", "Time");

    sigIndex_w++;

    for(int i=0; i < pamList_w.count(); i++)
    {
        A2L_VarChar *charVar = pamList_w.at(i);
        QDomElement sigElem = xml.createElement("Param_"+QString::number(sigIndex_w));
        WriteSigElem.appendChild(sigElem);

        QString dataType = charVar->DataType;
        int type = 1;
        if(dataType == "UBYTE" || dataType == "UWORD" || dataType == "ULONG" || dataType == "A_UINT64")
            type = 1;
        else if(dataType == "SBYTE" || dataType == "SWORD" || dataType == "SLONG" || dataType == "A_INT64")
            type = 0;
        else if(dataType == "FLOAT32_IEEE")
            type = 2;
        else if(dataType == "FLOAT64_IEEE")
            type = 3;

        sigElem.setAttribute("Unit", charVar->Unit);
        sigElem.setAttribute("DefaultValue", QString::number(charVar->getValue(), 'g', 6));
        sigElem.setAttribute("Minimum", QString::number(charVar->LowerLimit, 'g', 6));
        sigElem.setAttribute("Maximum", QString::number(charVar->UpperLimit, 'g', 6));
        sigElem.setAttribute("ScalingOffset", QString::number(charVar->ScalingOffset, 'g', 6));
        sigElem.setAttribute("ScalingFactor", QString::number(charVar->ScalingFactor, 'g', 6));
        sigElem.setAttribute("LengthBit", QString::number(charVar->DataSizeAG*8, 10));
        sigElem.setAttribute("StartBit", QString::number(charVar->StartByte*8+64, 10));
        sigElem.setAttribute("ByteOrder", QString::number(0,10));
        sigElem.setAttribute("Type", QString::number(type, 10));
        sigElem.setAttribute("Param_Name", charVar->Name);

        sigIndex_w++;
    }
    WriteSigElem.setAttribute("Size_Bytes", 8+sizeWrite);
    WriteSigElem.setAttribute("Sample_Rate", QString::number(100,10));
    WriteSigElem.setAttribute("isRead", QString::number(0,10));


    QString xmlString = xml.toString();
    QTextStream out(&xmlFile);
    out << xmlString;

    xmlFile.close();

    return true;
}

bool XCP_Main_Thread::charVars_Download(QList<A2L_VarChar *> charVars)
{
    if(!xcpMaster)
        return false;
    if(!xcpMaster->getXcpSessionConnectedStatus())
        return false;

    for(int i = 0; i < charVars.count(); i++)
    {
        A2L_VarChar *charVar = charVars.at(i);

        if(!xcpMaster->XCP_Cal_VALUE(charVar))
        {
            qDebug()<<"Charac:"<<charVar->Name<<", Calibrate error.";
            return false;
        }
        else
        {
            qDebug()<<"Charac:"<<charVar->Name<<", download ok:"<<charVar->getValue();
        }

    }

    return true;
}

void XCP_Main_Thread::init()
{

    xcpMaster = new XCPMaster(nullptr, curProj.Proj_name);
    connect(this, &XCP_Main_Thread::sigDownload, xcpMaster, &XCPMaster::downloadCals);
    connect(this, &XCP_Main_Thread::daqRunStatusActive, xcpMaster, &XCPMaster::sltDaqRunStatusActive);

    xcpPollThread = new XCP_Polling_Thread(nullptr,  xcpMaster);
    charPamCheckThread = new CharPamCheck(nullptr, xcpMaster);
    mapCharPamCheckThread = new MapCharPamCheckThread(nullptr, xcpMaster);

    connect(xcpPollThread, &XCP_Polling_Thread::measPamsValueUpdated, this, &XCP_Main_Thread::fromReadSMToMeasVars);
    connect(xcpPollThread, &XCP_Polling_Thread::pollingStatusChanged, this, &XCP_Main_Thread::pollingStatusChanged);
    connect(xcpPollThread, &XCP_Polling_Thread::caliStatusChanged, this, &XCP_Main_Thread::caliStatusChanged);

    connect(this, &XCP_Main_Thread::pollingStatusActive, xcpPollThread, &XCP_Polling_Thread::sltPollingStatusActive);

}

void XCP_Main_Thread::startXCP()
{
    if(measPamList.isEmpty() && charPamList.isEmpty() && charMapPamList.isEmpty() && daqMeasPamList.isEmpty())
        return;

    //creat sharedMemory
    createSM();
    createSMInMAP();

    //init write sharedMemory
    fromCharVarsToWriteSM();

    //init map shareMemory
    fromMapCharVarsToMapWriteSM();

    xcpMaster->setXcpName(curProj.Proj_name);
    xcpMaster->setXcpCanInterface(curProj.intfName);
    xcpMaster->setBaudrate(curProj.baudrate);
    xcpMaster->setIDMasterToSlave(curProj.id_CMD);
    xcpMaster->setIDSlaveToMaster(curProj.id_RES);
    xcpMaster->setPollMeasList(this->measPamList);

    //xcp daq
    if(curA2LProject)
    {
        xcpMaster->setXcpDaq(curA2LProject->getDaqInfo());
        xcpMaster->setXcpDaqList(curA2LProject->getDaqListStructList());
        xcpMaster->setXcpEventList(curA2LProject->getEventList());
        xcpMaster->setXcpEventCanIdList(curA2LProject->getA2lEventCanIdList());
    }
    else
    {
        xcpMaster->setXcpEventList(this->a2lEventList);
        xcpMaster->setXcpEventCanIdList(this->a2lEventCanIdList);
    }


    xcpMaster->setMeasVarListForDAQ(this->daqMeasPamList);

    //XCP CAN Init
    if(!xcpMaster->XCPInit())
    {
        qDebug()<<"==XCP can init fail==";
        emit xcpMsg("XCP can init fail");
        xcpCanInitOk = false;

        return;
    }
    xcpCanInitOk = true;
    emit xcpCanInitStatus(2);
    emit xcpMsg("XCP can init ok");
    qDebug()<<"==XCP can init ok==";

    //XCP  setup connection
    if(!xcpMaster->XCP_Setup_Session())
    {
        qDebug()<<"XCP setup fail.";
        emit xcpMsg("XCP setup fail");
        return;
    }
    xcpSetupOk = true;
    emit xcpConnStatus(2);
    emit xcpMsg("XCP on! Ready to Communication");
    qDebug()<<"==XCP on! Ready to Communication==";

    //char pams init download
    charVars_Download(charPamList);

    //xcp daq config
    if(!xcpDaqConfig())
    {
        qDebug()<<"==XCP daq config fail.==";
        emit xcpMsg("XCP daq config fail");
    }
    emit xcpMsg("XCP daq config ok");

    //save pam info to xml
    genXML();

    //init mdfRecord
    //initMdfRecord();

    //map pam check
    mapCharPamCheckThread->setMapCharPamList(charMapPamList);
    mapCharPamCheckThread->setMapCharSMHash(smMapWriteHash);
    mapCharPamCheckThread->setMapCharCheckRunFlag(true);
    mapCharPamCheckThread->start();
    qDebug()<<"==XCP map pam check start.==";

    //char pam check
    charPamCheckThread->setCharPamList(charPamList);
    charPamCheckThread->setSMKey(smKeyWrite);
    charPamCheckThread->setSmWriteSize(this->sizeWrite);
    charPamCheckThread->setCharCheckRunFlag(true);
    charPamCheckThread->start();
    qDebug()<<"==XCP char pam check start.==";

    //polling thread
    xcpPollThread->setMeasPamList(measPamList);
    xcpPollThread->setSmRead(this->smRead);
    xcpPollThread->setSMKey(this->smKeyRead);
    xcpPollThread->setSmReadSize(this->sizeRead);
    xcpPollThread->setPollRunFlag(pollingRun);
    xcpPollThread->start();
    qDebug()<<"==XCP meas polling start.==";

    emit xcpPollingStatus(2);
    emit xcpCaliStatus(2);
    emit xcpMsg("XCP cali and polling start");

    connect(charPamCheckThread, &CharPamCheck::addCaliAction, xcpPollThread, &XCP_Polling_Thread::addCaliActionSlot);
    connect(mapCharPamCheckThread, &MapCharPamCheckThread::addMapCaliAction, xcpPollThread, &XCP_Polling_Thread::addMapCaliActionSlot);

    // xcp daq start
    if(!setXcpDaqStartStop(daqRun))
    {
        qDebug()<<"==XCP daq START fail.==";
    }
    else
    {
        emit xcpDaqRunStatus(2);
        emit xcpMsg("XCP meas daq start");
    }

}

void XCP_Main_Thread::stopXCP()
{
    if(xcpPollThread == NULL || charPamCheckThread == NULL)
        return;
    if(a2lWin == NULL || xcpMaster == NULL)
        return;

    //qDebug()<<"stop xcp 1";

    if(recordThread)
    {
        recordThread->terminate();
        recordThread->wait();

        delete recordThread;
        recordThread = NULL;
    }

    if(!setXcpDaqStartStop(false))
    {
        qDebug()<<"==XCP daq STOP fail.==";
    }
    emit xcpDaqRunStatus(0);
    emit xcpMsg("XCP meas daq stop");

    disconnect(charPamCheckThread, &CharPamCheck::addCaliAction, xcpPollThread, &XCP_Polling_Thread::addCaliActionSlot);
    disconnect(mapCharPamCheckThread, &MapCharPamCheckThread::addMapCaliAction, xcpPollThread, &XCP_Polling_Thread::addMapCaliActionSlot);

    charPamCheckThread->setCharCheckRunFlag(false);
    charPamCheckThread->requestInterruption();
    charPamCheckThread->terminate();
    charPamCheckThread->wait();

    delete charPamCheckThread;
    charPamCheckThread = NULL;

    mapCharPamCheckThread->setMapCharCheckRunFlag(false);
    mapCharPamCheckThread->requestInterruption();
    mapCharPamCheckThread->terminate();
    mapCharPamCheckThread->wait();

    delete mapCharPamCheckThread;
    mapCharPamCheckThread = NULL;

    xcpPollThread->setPollRunFlag(false);
    xcpPollThread->requestInterruption();
    xcpPollThread->terminate();
    xcpPollThread->wait();

    delete xcpPollThread;
    xcpPollThread = NULL;

    emit xcpPollingStatus(0);
    emit xcpCaliStatus(0);
    emit xcpMsg("XCP cali and polling stop");

    xcpMaster->XCPStop();
    qDebug()<<"==XCP break and stop can==";
    emit xcpConnStatus(0);
    emit xcpCanInitStatus(0);
    emit xcpMsg("XCP stop");

    deleteSM();
    deleteSMInMAP();

    delete xcpMaster;
    xcpMaster = NULL;
}

void XCP_Main_Thread::initMdfRecord()
{
    if(!xcpMaster)
        return;

    //mdf record
    mdfRecordIns = new MDF_Record_Thread();
    mdfRecordIns->setPollPams(transferMeasToPams(this->measPamList));
    mdfRecordIns->setPamsHash(getPamHashFromMeasHash(xcpMaster->getDaqListVarHash()));
    // 使用MemoryManager替代QSharedMemory，不需要设置daqSmHash
    mdfRecordIns->setPamsBlockSizeHash(xcpMaster->getDaqListBlockSizeHash());
    mdfRecordIns->setRecordFileName(curProj.Proj_name);

    connect(xcpMaster, QOverload<ByteArrayPtr, quint32, QString>::of(&XCPMaster::ODTDataForRecord), mdfRecordIns, &MDF_Record_Thread::mdf_record_slot_v2);
    connect(mdfRecordIns, &MDF_Record_Thread::recordTime, this, &XCP_Main_Thread::recordTimeUpdated);

    connect(this, &XCP_Main_Thread::recordActive, mdfRecordIns, &MDF_Record_Thread::setRecordStatus_v2);

    if(xcpPollThread)
    {
        connect(xcpPollThread, QOverload<ByteArrayPtr, quint32, QString>::of(&XCP_Polling_Thread::pollDataForRecord), mdfRecordIns, &MDF_Record_Thread::mdf_record_slot_raw);
    }

    recordThread = new QThread();
    mdfRecordIns->moveToThread(recordThread);
    connect(recordThread, &QThread::finished, mdfRecordIns, &QObject::deleteLater);
    recordThread->start();

    qDebug()<<"==mdf record init in xcp_main_thread.==";

}

bool XCP_Main_Thread::xcpDaqConfig()
{
    if (xcpMaster == NULL)
        return false;
    if (!xcpMaster->getXcpCanInitStatus())
        return false;
    if (!xcpMaster->getXcpSessionConnectedStatus())
        return false;

    if (daqMeasPamList.count() == 0)
        return false;
    xcpMaster->setMeasVarListForDAQ(daqMeasPamList);

    if (!xcpMaster->XCP_DAQ_CONFIG())
        return false;
    else
        return true;

    /*
    int count = 0;
    while (!xcpMaster->getXcpDaqRunningStatus() && count < 3)
    {
        xcpMaster->XCP_DAQ_Start_Stop(true);
        count++;
    }

    if (!xcpMaster->getXcpDaqRunningStatus())
        return false;
    else
        return true;
    */
}

bool XCP_Main_Thread::setXcpDaqStartStop(bool status)
{
    if (xcpMaster == NULL)
        return false;
    if (!xcpMaster->getXcpCanInitStatus())
        return false;
    if (!xcpMaster->getXcpSessionConnectedStatus())
        return false;

    int count = 0;
    while ((xcpMaster->getXcpDaqRunningStatus() != status) && count < 3)
    {
        xcpMaster->XCP_DAQ_Start_Stop(status);
        count++;
    }

    if (xcpMaster->getXcpDaqRunningStatus() != status)
        return false;
    else
        return true;
}

void XCP_Main_Thread::createSM()
{
    if(smRead == NULL)
    {
        smRead = new QSharedMemory(smKeyRead);
    }
    smRead->create(8+sizeRead);


    if(smWrite == NULL)
    {
        smWrite = new QSharedMemory(smKeyWrite);
    }
    smWrite->create(8+sizeWrite);
}

void XCP_Main_Thread::deleteSM()
{
    delete smRead;
    smRead = NULL;

    delete smWrite;
    smWrite = NULL;
}

void XCP_Main_Thread::createSMInMAP()
{
    for(int i = 0; i < charMapPamList.count(); i++)
    {
        A2L_VarChar *mapChar = charMapPamList.at(i);

        QSharedMemory *sm = new QSharedMemory("WP_XCP_MAP_" + mapChar->Name);
        int size = mapChar->zCount * mapChar->DataSizeAG;
        if(!sm->create(size+8))
        {
            qDebug()<<"create map sharedmemory error, "<<mapChar->Name;
        }

        smMapWriteList.append(sm);
        smMapWriteHash.insert(mapChar, sm);
    }
}

void XCP_Main_Thread::deleteSMInMAP()
{
    qDeleteAll(smMapWriteList);
    smMapWriteList.clear();

    smMapWriteHash.clear();
}

void XCP_Main_Thread::setA2lWin(A2LProjectWin *value)
{
    a2lWin = value;

}

void XCP_Main_Thread::fromCharVarsToWriteSM()
{
    if(smWrite == NULL)
        return;
    QByteArray buffer(sizeWrite, Qt::Uninitialized);

    if(!smWrite->isAttached())
    {
        if(!smWrite->attach())
        {
            qDebug()<<"Unable attach to write sharedmemory.";
            return;
        }
    }

    fromCharVarsToWriteRawData(buffer.data(), sizeWrite);

    quint64 initTime = 0;
    smWrite->lock();
    memcpy((char*)smWrite->data(), (char*)&initTime, 8);
    memcpy((char*)smWrite->data()+8, buffer.constData(), sizeWrite);
    smWrite->unlock();

    emit writePamValueUpdated();
}

void XCP_Main_Thread::fromCharVarsToWriteRawData(char *data, quint64 size)
{
    QByteArray buffer(size, Qt::Uninitialized);

    for(int i = 0; i < charPamList.count(); i++)
    {
        A2L_VarChar *charVar = charPamList.at(i);

        value2IntelData(buffer.data(), charVar);
    }

    memcpy(data, buffer.constData(), size);
}

void XCP_Main_Thread::fromMapCharVarsToMapWriteSM()
{
    for(int i = 0; i < charMapPamList.count(); i++)
    {
        A2L_VarChar *charVar = charMapPamList.at(i);



        QSharedMemory *sm = smMapWriteHash.value(charVar);

        if(!sm->isAttached())
        {
            if(!sm->attach())
            {
                qDebug()<<"Unable attach to map write sharedmemory.";
                continue;
            }
        }

        int offset = 8;
        for(int j = 0; j < charVar->zCount; j++)
        {

            qreal value = charVar->hexValue_ZList.at(j);
            char data[8]; // DataSizeAG 最大为8字节
            transferPhyValueToRawData(charVar, value, data);

            sm->lock();
            memcpy((char*)sm->data()+offset, data, charVar->DataSizeAG);
            sm->unlock();

            offset += charVar->DataSizeAG;
        }

    }
}

void XCP_Main_Thread::transferPhyValueToRawData(A2L_VarChar *charVar, double value, char *data)
{
    if(charVar == NULL)
        return;
    QString dataType = charVar->DataType;
    int dataSize = charVar->DataSizeAG;

    int dataTypeIndex = dataTypeEnum.indexOf(dataType);

    switch (dataTypeIndex) {
    case 0:
    {
        quint8 val = (quint8)value;
        memcpy(data, (char*)&val, dataSize);
        break;
    }
    case 1:
    {
        qint8 val = (qint8)value;
        memcpy(data, (char*)&val, dataSize);
        break;
    }
    case 2:
    {
        quint16 val = (quint16)value;
        memcpy(data, (char*)&val, dataSize);
        break;
    }
    case 3:
    {
        qint16 val = (qint16)value;
        memcpy(data, (char*)&val, dataSize);
        break;
    }
    case 4:
    {
        quint32 val = (quint32)value;
        memcpy(data, (char*)&val, dataSize);
        break;
    }
    case 5:
    {
        qint32 val = (qint32)value;
        memcpy(data, (char*)&val, dataSize);
        break;
    }
    case 6:
    {
        quint64 val = (quint64)value;
        memcpy(data, (char*)&val, dataSize);
        break;
    }
    case 7:
    {
        qint64 val = (qint64)value;
        memcpy(data, (char*)&val, dataSize);
        break;
    }
    case 8:
    {
        float val = (float)value;
        memcpy(data, (char*)&val, dataSize);
        break;
    }
    case 9:
    {
        double val = (double)value;
        memcpy(data, (char*)&val, dataSize);
        break;
    }
    default:
        break;
    }
}

void XCP_Main_Thread::value2IntelData(char *data, A2L_VarChar *charVar)
{
    int startByte = charVar->StartByte;
    quint32 LenBit = charVar->DataSizeAG*8;
    QString type = charVar->DataType;

    qreal value = 0;
    if (charVar->ConversionType == "RAT_FUNC")
    {
         value = xcpMaster->RatFuncConvFromPhysToInt(charVar->getValue(), charVar->CoefList);
    }
    else if (charVar->ConversionType == "TAB_VERB")
    {
        value = charVar->getValue();
    }
    else
    {
        value = charVar->getValue();
    }

    char *to = data;
    switch (LenBit) {
    case 8:
    {
        if(type == "UBYTE")
            *(quint8*)(to + startByte) = (quint8)value;
        else if(type == "SBYTE")
            *(qint8*)(to + startByte) = (qint8)value;
        break;
    }
    case 16:
    {
        if(type == "UWORD")
            *(quint16*)(to + startByte) = (quint16)value;
        else if(type == "SWORD")
            *(qint16*)(to + startByte) = (qint16)value;

        break;
    }
    case 32:
    {
        if(type == "ULONG")
            *(quint32*)(to + startByte) = (quint32)value;
        else if(type == "SLONG")
            *(qint32*)(to + startByte) = (qint32)value;
        else if(type == "FLOAT32_IEEE")
            *(float*)(to + startByte) = (float)value;
        break;
    }
    case 64:
    {
        if(type == "A_UINT64")
            *(quint64*)(to + startByte) = (quint64)value;
        else if(type == "A_INT64")
            *(qint64*)(to + startByte) = (qint64)value;
        else if(type == "FLOAT64_IEEE")
            *(qreal*)(to + startByte) = (qreal)value;

        break;
    }
    default:
        break;
    }
}

QHash<quint16, QList<PARAM *> > XCP_Main_Thread::getPamHashFromMeasHash(QHash<quint16, QList<A2L_VarMeas *> > measHash)
{
    QHash<quint16, QList<PARAM *>> daqPamsHash;
    QList<quint16> keys = measHash.uniqueKeys();
    if(keys.count() == 0)
        return daqPamsHash;
    for(int i = 0; i < keys.count(); i++)
    {
        quint16 daqList = keys.at(i);
        QList<A2L_VarMeas*> meas = measHash.value(daqList);
        if(meas.isEmpty())
            continue;
        QList<PARAM*> pams = transferMeasToPams(meas);
        daqPamsHash.insert(daqList, pams);
    }

    return daqPamsHash;

}

QList<PARAM *> XCP_Main_Thread::transferMeasToPams(QList<A2L_VarMeas *> measPamList) // add time pam
{
    QList<PARAM*> pamList;

    if(measPamList.isEmpty())
        return pamList;

    quint32 startBitIndex = 0;

    PARAM *timePam = new PARAM();
    timePam->Unit = "s";
    timePam->DefaultValue = 0;
    timePam->LowerLimit = 0;
    timePam->UpperLimit = 1000000;
    timePam->ScalingFactor = 1;
    timePam->ScalingOffset = 0;
    timePam->LengthBit = 64;
    timePam->StartBit = 0;
    timePam->ByteOrder = 0;
    timePam->Type = 3;
    timePam->Name = "Time";

    pamList.append(timePam);

    startBitIndex = 64;

    for(int i=0; i < measPamList.count(); i++)
    {
        A2L_VarMeas *measVar = measPamList.at(i);
        PARAM *varPam = new PARAM();

        QString dataType = measVar->DataType;
        int type = 0;
        if(dataType == "UBYTE" || dataType == "UWORD" || dataType == "ULONG" || dataType == "A_UINT64")
            type = 0;
        else if(dataType == "SBYTE" || dataType == "SWORD" || dataType == "SLONG" || dataType == "A_INT64")
            type = 1;
        else if(dataType == "FLOAT32_IEEE")
            type = 2;
        else if(dataType == "FLOAT64_IEEE")
            type = 3;

        varPam->Unit = measVar->Unit;
        varPam->DefaultValue = 0;
        varPam->LowerLimit = measVar->LowerLimit;
        varPam->UpperLimit = measVar->UpperLimit;
        varPam->ScalingFactor = measVar->ScalingFactor;
        varPam->ScalingOffset = measVar->ScalingOffset;
        varPam->LengthBit = measVar->DataSizeAG*8;
        varPam->StartBit = startBitIndex;
        varPam->ByteOrder = 0;      // intel
        varPam->Type = type;
        varPam->Name = measVar->Name;

        startBitIndex += (measVar->DataSizeAG * 8);

        pamList.append(varPam);
    }

    return pamList;
}

void XCP_Main_Thread::setA2lEventCanIdList(const QList<A2L_EventCanId *> &value)
{
    a2lEventCanIdList = value;
}

void XCP_Main_Thread::setA2lEventList(const QList<A2L_Event *> &value)
{
    a2lEventList = value;
}

XCP_Polling_Thread *XCP_Main_Thread::getXcpPollThread() const
{
    return xcpPollThread;
}

XCPMaster *XCP_Main_Thread::getXcpMaster() const
{
    return xcpMaster;
}

bool XCP_Main_Thread::getXcpCanInitOk() const
{
    return xcpCanInitOk;
}

bool XCP_Main_Thread::getXcpSetupOk() const
{
    return xcpSetupOk;
}

void XCP_Main_Thread::setDaqRun(bool value)
{
    daqRun = value;
    emit daqRunStatusActive(value);
}

void XCP_Main_Thread::setPollingRun(bool value)
{
    pollingRun = value;
    emit pollingStatusActive(value);
}

void XCP_Main_Thread::setSizeWrite(const quint64 &value)
{
    sizeWrite = value;
}

void XCP_Main_Thread::updateCharValueInSM(A2L_VarChar *charVar, double value)
{
    int startByte = charVar->StartByte;
    quint32 dataSize = charVar->DataSizeAG;
    QString type = charVar->DataType;

    qreal intValue = 0;
    if (charVar->ConversionType == "RAT_FUNC")
    {
         intValue = xcpMaster->RatFuncConvFromPhysToInt(value, charVar->CoefList);
    }
    else if (charVar->ConversionType == "TAB_VERB")
    {
        intValue = value;
    }
    else
    {
        intValue = value;
    }

    char data[8]; // DataSizeAG 最大为8字节
    switch (dataSize) {
    case 1:
    {
        if(type == "UBYTE")
            *(quint8*)(data) = (quint8)intValue;
        else if(type == "SBYTE")
            *(qint8*)(data) = (qint8)intValue;
        break;
    }
    case 2:
    {
        if(type == "UWORD")
            *(quint16*)(data) = (quint16)intValue;
        else if(type == "SWORD")
            *(qint16*)(data) = (qint16)intValue;

        break;
    }
    case 4:
    {
        if(type == "ULONG")
            *(quint32*)(data) = (quint32)intValue;
        else if(type == "SLONG")
            *(qint32*)(data) = (qint32)intValue;
        else if(type == "FLOAT32_IEEE")
            *(float*)(data) = (float)intValue;
        break;
    }
    case 8:
    {
        if(type == "A_UINT64")
            *(quint64*)(data) = (quint64)intValue;
        else if(type == "A_INT64")
            *(qint64*)(data) = (qint64)intValue;
        else if(type == "FLOAT64_IEEE")
            *(qreal*)(data) = (qreal)intValue;

        break;
    }
    default:
        break;
    }

    if(!smWrite)
        return;
    if(!smWrite->isAttached())
    {
        if(!smWrite->attach())
        {
            qDebug()<<"unable attach to write sharedmemory.";
            return;
        }
    }

    smWrite->lock();
    memcpy((char*)smWrite->data()+8+startByte, data, dataSize);
    smWrite->unlock();
}

void XCP_Main_Thread::setSizeReadDAQ(const quint64 &value)
{
    sizeReadDAQ = value;
}

void XCP_Main_Thread::setSizeRead(const quint64 &value)
{
    sizeRead = value;
}

void XCP_Main_Thread::setSmKeyWrite(const QString &value)
{
    smKeyWrite = value;
}

void XCP_Main_Thread::setSmKeyReadDAQ(const QString &value)
{
    smKeyReadDAQ = value;
}

void XCP_Main_Thread::setSmKeyRead(const QString &value)
{
    smKeyRead = value;
}

void XCP_Main_Thread::setCurA2LProject(A2L_Project *value)
{
    curA2LProject = value;
}

void XCP_Main_Thread::setCurProj(Proj value)
{
    curProj = value;
}

