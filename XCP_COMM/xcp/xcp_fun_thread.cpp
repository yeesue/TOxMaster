#include "xcp_fun_thread.h"

Xcp_Fun_Thread::Xcp_Fun_Thread(QObject *parent, A2L_Project *a2lProject):
    QThread(parent), curA2LProject(a2lProject)
{

}

Xcp_Fun_Thread::~Xcp_Fun_Thread()
{
    stopXCP();
}

void Xcp_Fun_Thread::preFun()
{
    readSetting();

    if(!xcpConfig.useA2lSetFile)
    {
        loadA2lProject();
        getA2LPamsByNames();
    }
    else
    {
        importA2LPamsFromCsv();
    }

}


void Xcp_Fun_Thread::setCharsDownloadActive()
{
    emit sigDownload();
}

void Xcp_Fun_Thread::setMdfRecordStatus(bool status)
{
    emit recordActive(status);
}

void Xcp_Fun_Thread::setA2lWin(A2LProjectWin *value)
{
    a2lWin = value;
}

void Xcp_Fun_Thread::setCurA2LProject(A2L_Project *value)
{
    curA2LProject = value;
}

void Xcp_Fun_Thread::setPollingRun(bool value)
{
    pollingRun = value;
    emit pollingStatusActive(value);
}

void Xcp_Fun_Thread::setDaqRun(bool value)
{
    daqRun = value;
    emit daqRunStatusActive(value);
}

bool Xcp_Fun_Thread::getXcpSetupOk() const
{
    return xcpSetupOk;
}

bool Xcp_Fun_Thread::getXcpCanInitOk() const
{
    return xcpCanInitOk;
}

XCPMaster *Xcp_Fun_Thread::getXcpMaster() const
{
    return xcpMaster;
}

XCP_Polling_Thread *Xcp_Fun_Thread::getXcpPollThread() const
{
    return xcpPollThread;
}

void Xcp_Fun_Thread::setA2lEventList(const QList<A2L_Event *> &value)
{
    a2lEventList = value;
}

void Xcp_Fun_Thread::setA2lEventCanIdList(const QList<A2L_EventCanId *> &value)
{
    a2lEventCanIdList = value;
}

void Xcp_Fun_Thread::updatePamValueInSM(PARAM *charPam, double value)
{
    if(charPam->pamModule != 1)
        return;

    updateCharValueInSM((A2L_VarChar*)charPam, value);
}

void Xcp_Fun_Thread::updateCharValueInSM(A2L_VarChar *charVar, double value)
{
    //qDebug()<<"CharVar:"<<charVar->Name<<", calValue:"<<value;

    if(!smWrite || !charVar) return;

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

    char *data = new char[dataSize];
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

    smWrite->lock();
    memcpy((char*)smWrite->data() + 8 + charVar->StartByte, data, dataSize);
    smWrite->unlock();

    delete[] data;
}

void Xcp_Fun_Thread::updatePamMapValueInSM(PARAM *charMapPam, int index_x, int index_y, double value)
{
    if(charMapPam->pamModule != 1)
        return;

    //更新共享内存中map参数的值
    updateCharMapValueInSM((A2L_VarChar*)charMapPam, index_x, index_y, value);
}

void Xcp_Fun_Thread::updateCharMapValueInSM(A2L_VarChar *mapChar, int index_x, int index_y, double value)
{
    qDebug()<<"mapName="<<mapChar->Name<<", row="<<index_x<<", col="<<index_y<<",value="<<value;
    if(index_x == 0)
    {
        qDebug()<<"Axis value updated.";
    }
    else if (index_y == 0)
    {
        qDebug()<<"Yxis value updated.";
    }
    else
    {
        qDebug()<<"Z map value updated.";

        char *data = new char[mapChar->DataSizeAG];
        transferPhyValueToRawData(mapChar, value, data);

        updateMapValueInSM(mapChar, index_y-1, index_x-1, data);
    }
}

void Xcp_Fun_Thread::run()
{
    init();

    startXCP();

    exec();
}

void Xcp_Fun_Thread::fromReadSMToMeasVars()
{
    if(smRead == NULL)
        return;

    char *buffer = new char[sizeRead];

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
    memcpy(buffer, (char*)smRead->data()+8, sizeRead);
    smRead->unlock();

    fromReadRawDataToMeasVars(buffer, sizeRead);

    delete[] buffer;
}

void Xcp_Fun_Thread::fromReadRawDataToMeasVars(char *data, quint64 size)
{
    for(int i = 0; i < measPamList.count(); i++)
    {
        A2L_VarMeas *measVar = measPamList.at(i);

        int startByte = measVar->StartByte;
        int dataSize = measVar->DataSizeAG;
        QString type = measVar->DataType;

        if(startByte + dataSize > size)
            continue;

        char *temp = new char[dataSize];
        memcpy(temp, data+startByte, dataSize);

        qreal measValue = 0;
        switch (dataSize) {
        case 1:
        {
            if(type == "UBYTE")
                measValue = *(quint8*)temp;
            else if(type == "SBYTE")
                measValue = *(qint8*)temp;
            break;
        }
        case 2:
        {
            if(type == "UWORD")
                measValue = *(quint16*)temp;
            else if(type == "SWORD")
                measValue = *(qint16*)temp;
            break;
        }
        case 4:
        {
            if(type == "ULONG")
                measValue = *(quint32*)temp;
            else if(type == "SLONG")
                measValue = *(qint32*)temp;
            else if(type == "FLOAT32_IEEE")
                measValue = *(float*)temp;
            break;
        }
        case 8:
        {
            if(type == "A_UINT64")
                measValue = *(quint64*)temp;
            else if(type == "A_INT64")
                measValue = *(qint64*)temp;
            else if(type == "FLOAT64_IEEE")
                measValue = *(qreal*)temp;
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

        delete[] temp;
    }
}

bool Xcp_Fun_Thread::readSetting()
{
    if(fun->setFilePath.isEmpty() || fun->intfName.isEmpty())
        return false;

    QString setFilePath = QCoreApplication::applicationDirPath() + fun->setFilePath;
    qDebug()<<"In xcp fun thread: read setting...";
    QSettings setting(setFilePath, QSettings::IniFormat);
    xcpConfig.workName = setting.value("Workspace").toString();
    xcpConfig.intfName = setting.value("Interface").toString();
    xcpConfig.a2lProjectName = setting.value("A2LProjectName").toString();
    xcpConfig.a2lFilePath = setting.value("A2LFilePath").toString();
    xcpConfig.hexFilePath = setting.value("HexFilePath").toString();

    xcpConfig.index_XcpOnCan = setting.value("Index_XcpOnCan").toUInt();
    xcpConfig.baudrate = setting.value("Baudrate").toUInt();
    xcpConfig.id_CMD = setting.value("ID_CMD").toUInt();
    xcpConfig.id_RES = setting.value("ID_RES").toUInt();

    xcpConfig.isCanFD = setting.value("IsCanFD").toBool();
    xcpConfig.fdBaudrate = setting.value("FdBaudrate").toUInt();
    xcpConfig.maxDLC = setting.value("MAX_DLC").toUInt();

    xcpConfig.useSeedKey = setting.value("UseSeedKey").toBool();
    xcpConfig.seedKeyDllPath = setting.value("SeedKeyDllPath").toString();

    xcpConfig.calCsvFilePath = setting.value("CalCsvFile").toString();
    xcpConfig.useA2lSetFile = setting.value("UseA2lSetFile").toBool();
    xcpConfig.usePamsInExcel = setting.value("UsePamsInExcel").toBool();
    xcpConfig.xcpDeviceType = setting.value("XcpDeviceType").toUInt();
    xcpConfig.tsDeviceIndex = setting.value("TsDeviceIndex").toUInt();
    xcpConfig.zlgDeviceType = setting.value("ZlgDeviceType").toUInt() + 41; // ZLG 设备类型对应编号 200U = 41, 100U = 42, MINI = 43
    xcpConfig.zlgDeviceIndex = setting.value("ZlgDeviceIndex").toUInt();

    // load poll pams
    QList<XcpPam> pamList;
    QStringList pamNames, pamPollTypes, pamDefaultValues, caliTypes, pamDaqEventTypes;
    QString pamName = setting.value("PollPamNames").toString();
    QString pamPollType = setting.value("PollPamPollTypes").toString();
    QString pamDefaultValue = setting.value("PollPamDefaultValues").toString();

    if (!pamName.isEmpty())
    {
        pamNames = pamName.split("|");
        pamPollTypes = pamPollType.split("|");
        pamDefaultValues = pamDefaultValue.split("|");
    }
    //qDebug()<<"read pams count:"<<pamNames.count()<<", default values count:"<<pamDefaultValues.count();
    for(int j = 0; j < pamNames.count(); j++)
    {
        XcpPam pam;
        pam.pamName = pamNames.at(j);
        pam.pollType = pamPollTypes.at(j);
        QString valueStr = pamDefaultValues.at(j);
        pam.pamDefaultValue = valueStr.toDouble();

        pamList.append(pam);
    }
    xcpConfig.pollPamList = pamList;

    // load cal pams
    pamList.clear();
    pamNames.clear();
    pamPollTypes.clear();
    pamDefaultValues.clear();
    QString caliType;

    pamName = setting.value("CalPamNames").toString();
    pamDefaultValue = setting.value("CalPamDefaultValues").toString();
    caliType = setting.value("CalPamTypes").toString();

    if (!pamName.isEmpty())
    {
        pamNames = pamName.split("|");
        pamDefaultValues = pamDefaultValue.split("|");
        caliTypes = caliType.split("|");
    }
    //qDebug()<<"write pams count:"<<pamNames.count()<<", default values count:"<<pamDefaultValues.count();
    for(int j = 0; j < pamNames.count(); j++)
    {
        XcpPam pam;
        pam.pamName = pamNames.at(j);
        QString valueStr = pamDefaultValues.at(j);
        pam.pamDefaultValue = valueStr.toDouble();

        if(caliTypes.count() > j)
            pam.caliType = caliTypes.at(j);

        pamList.append(pam);
    }
    xcpConfig.calPamList = pamList;

    // load daqReadPams
    pamList.clear();
    pamNames.clear();
    pamDaqEventTypes.clear();
    pamDefaultValues.clear();
    pamName = setting.value("DaqPamNames").toString();
    QString pamDaqEventType = setting.value("DaqPamEventTypes").toString();
    pamDefaultValue = setting.value("DaqPamDefaultValues").toString();

    if (!pamName.isEmpty())
    {
        pamNames = pamName.split("|");
        pamDaqEventTypes = pamDaqEventType.split("|");
        pamDefaultValues = pamDefaultValue.split("|");
    }

    for(int j = 0; j < pamNames.count(); j++)
    {
        XcpPam pam;
        pam.pamName = pamNames.at(j);
        pam.daqEventTime = pamDaqEventTypes.at(j);
        QString valueStr = pamDefaultValues.at(j);
        pam.pamDefaultValue = valueStr.toDouble();
        pamList.append(pam);
    }
    xcpConfig.daqPamList = pamList;

    // load [daq] [daq list] [event list] [event can id list]
    xcpConfig.daqList.clear();
    xcpConfig.eventCanIdList.clear();
    xcpConfig.eventList.clear();

    xcpConfig.a2lDaq.Name = setting.value("DaqName").toString();
    xcpConfig.a2lDaq.DaqType = setting.value("DaqType").toString();
    xcpConfig.a2lDaq.GranularityOdtEntrySize = setting.value("GranularityOdtEntrySize").toString();
    xcpConfig.a2lDaq.IdentificationField = setting.value("IdentificationField").toString();
    xcpConfig.a2lDaq.AddressExtType = setting.value("AddressExtType").toString();
    xcpConfig.a2lDaq.OptimisationType = setting.value("OptimisationType").toString();
    xcpConfig.a2lDaq.Max_Daq = (quint16)setting.value("Max_Daq").toUInt();
    xcpConfig.a2lDaq.Min_Daq = (quint16)setting.value("Min_Daq").toUInt();
    xcpConfig.a2lDaq.Max_Event_Channel = (quint8)setting.value("Max_Event_Channel").toUInt();
    xcpConfig.a2lDaq.Max_Odt_Entry_Size_Daq = (quint8)setting.value("Max_Odt_Entry_Size_Daq").toUInt();

    int daqCount = setting.beginReadArray("XcpDaqList");
    for (int i = 0; i < daqCount; i++)
    {
        setting.setArrayIndex(i);
        A2L_Daq_List *daq = new A2L_Daq_List();
        daq->Name = setting.value("DaqListName").toString();
        daq->DaqListNumber = (quint16)(setting.value("DaqListNumber").toUInt());
        daq->DaqListType = setting.value("DaqListType").toString();
        daq->Max_Odt = (quint8)setting.value("Max_Odt").toUInt();
        daq->Max_Odt_Entries = (quint8)setting.value("Max_Odt_Entries").toUInt();
        daq->First_Pid = (quint8)(setting.value("First_Pid").toUInt());
        daq->Event_Fixed = (quint8)(setting.value("Event_Fixed").toUInt());

        xcpConfig.daqList.append(daq);
    }
    setting.endArray();

    int eventCanIdCount = setting.beginReadArray("XcpEventCanIdList");
    for (int i = 0; i < eventCanIdCount; i++)
    {
        setting.setArrayIndex(i);
        A2L_EventCanId *eventCanId = new A2L_EventCanId();
        eventCanId->Name = setting.value("EventCanIdName").toString();
        eventCanId->EventNumber = (quint8)(setting.value("EventNumber").toUInt());
        eventCanId->Event_CAN_ID = (quint32)(setting.value("EventCanId").toUInt());

        xcpConfig.eventCanIdList.append(eventCanId);
    }
    setting.endArray();

    int eventCount = setting.beginReadArray("XcpEventList");
    for (int i = 0; i < eventCount; i++)
    {
        setting.setArrayIndex(i);
        A2L_Event *event = new A2L_Event();
        event->Name = setting.value("EventName").toString();
        event->ChannelNumber = (quint8)(setting.value("ChannelNumber").toUInt());
        event->EventChannelName = setting.value("EventChannelName").toString();
        event->ShortName = setting.value("ShortName").toString();
        event->EventType = setting.value("EventType").toString();
        event->Priority = (quint8)(setting.value("EventPriority").toUInt());
        event->TimeCycle = (quint8)(setting.value("TimeCycle").toUInt());
        event->TimeUnit = (quint8)(setting.value("TimeUnit").toUInt());
        event->Max_Daq_List = (quint8)(setting.value("MaxDaqList").toUInt());
        event->Rate_ms = setting.value("Rate_ms").toFloat();
        QString eventCanIdName = setting.value("RefEventCanIdName").toString();
        if(!eventCanIdName.isEmpty())
        {

        }

        xcpConfig.eventList.append(event);
    }

    // load xcp on can name list
    int xcpOnCanCount = setting.beginReadArray("XcpOnCanNameList");
    for (int i = 0; i < xcpOnCanCount; i++)
    {
        setting.setArrayIndex(i);
        QString xcpOnCanName = setting.value("XcpOnCanName").toString();
        xcpConfig.xcpOnCanNameList.append(xcpOnCanName);
    }
    setting.endArray();

    // load daq list name list
    int daqListCount = setting.beginReadArray("DaqListNameList");
    for (int i = 0; i < daqListCount; i++)
    {
        setting.setArrayIndex(i);
        QString daqListName = setting.value("DaqListName").toString();
        xcpConfig.daqListNameList.append(daqListName);
    }
    setting.endArray();

    // load daq list can id list
    int daqListCanIdCount = setting.beginReadArray("DaqListCanIdList");
    for (int i = 0; i < daqListCanIdCount; i++)
    {
        setting.setArrayIndex(i);
        QString daqListCanId = setting.value("DaqListCanId").toString();
        xcpConfig.daqListCanIdList.append(daqListCanId);
    }
    setting.endArray();

    // load event time name list
    int eventTimeCount = setting.beginReadArray("EventTimeNameList");
    for (int i = 0; i < eventTimeCount; i++)
    {
        setting.setArrayIndex(i);
        QString eventTimeName = setting.value("EventTimeName").toString();
        xcpConfig.eventTimeList.append(eventTimeName);
    }
    setting.endArray();

    smKeyRead = "RP_POLL_" + xcpConfig.workName + "_" + fun->funName;
    smKeyWrite = "WP_" + xcpConfig.workName + "_" + fun->funName;
    smKeyReadDAQ = "RP_DAQ_" + xcpConfig.workName + "_" + fun->funName;

    mapListKey = "WP_MAP_" + xcpConfig.workName + "_" + fun->funName;

    return true;
}

void Xcp_Fun_Thread::loadA2lProject()
{
    if(!a2lWin || xcpConfig.a2lProjectName.isEmpty() || xcpConfig.a2lFilePath.isEmpty() || xcpConfig.hexFilePath.isEmpty())
        return;

    a2lWin->addA2LProject(xcpConfig.a2lFilePath, xcpConfig.hexFilePath);

    qDebug()<<"In xcp fun thread: load a2l project...";
    curA2LProject = a2lWin->getProjectByName(xcpConfig.a2lProjectName);
}

void Xcp_Fun_Thread::getA2LPamsByNames()
{
    if(!curA2LProject)
        return;

    int startBitIndex = 0;
    int startByteIndex = 0;
    startBitIndex += 0;

    qDebug()<<"In xcp fun thread: load a2l pams...";
    qDebug()<<"************getting poll read meas pams from a2l project***************";
    for(int i = 0; i < xcpConfig.pollPamList.count(); i++)
    {
        XcpPam xcpPam = xcpConfig.pollPamList.at(i);

        A2L_VarMeas *measVar = curA2LProject->getMeasVarByName(xcpPam.pamName);

        if(measVar)
        {
            measPamList.append(measVar);

            measVar->StartByte = startByteIndex;
            measVar->smKey = smKeyRead;

            startBitIndex += (measVar->DataSizeAG * 8);
            startByteIndex = startBitIndex/8;

        }
    }

    sizeRead = startBitIndex/8;

    startBitIndex = 0;
    startByteIndex = 0;
    qDebug()<<"************getting cal char pams from a2l project***************";
    for(int i = 0; i < xcpConfig.calPamList.count(); i++)
    {
        XcpPam xcpPam = xcpConfig.calPamList.at(i);

        A2L_VarChar *charVar = curA2LProject->getCharVarByName(xcpPam.pamName);

        if(charVar == NULL)
            continue;

        if(charVar->charType == "VALUE")
        {
            charPamList.append(charVar);

            charVar->StartByte = startByteIndex;
            charVar->smKey = smKeyWrite;
            charVar->setValue(xcpPam.pamDefaultValue);
            charVar->funIndex = this->funIndex;

            startBitIndex += (charVar->DataSizeAG * 8);
            startByteIndex = startBitIndex/8;

            charVar->pamModule = 1;   // 1 = xcp value, 0 = can pam
        }
        else if(charVar->charType == "CURVE" || charVar->charType == "MAP" || charVar->charType == "VAL_BLK")
        {
            charMapPamList.append(charVar);
        }
    }

    sizeWrite = startBitIndex/8;

    //daq meas pams
    startBitIndex = 0;
    startByteIndex = 0;
    qDebug()<<"************getting daq read meas pams from a2l project***************";
    for(int i = 0; i < xcpConfig.daqPamList.count(); i++)
    {
        XcpPam xcpPam = xcpConfig.daqPamList.at(i);
        //qDebug()<<"xcpPam rate_ms:"<<xcpPam.daqEventTime;
        A2L_VarMeas *measVar = curA2LProject->getMeasVarByName(xcpPam.pamName);

        if(measVar)
        {
            daqMeasPamList.append(measVar);

            measVar->StartByte = startByteIndex;
            measVar->smKey = smKeyReadDAQ;
            measVar->rate_ms = xcpPam.daqEventTime.replace("ms", "").toDouble();
            //qDebug()<<"Meas rate_ms:"<<measVar->rate_ms;

            startBitIndex += (measVar->DataSizeAG * 8);
            startByteIndex = startBitIndex/8;
        }
    }

    sizeReadDAQ = startBitIndex/8;

    qDebug()<<"In fun thread:" << fun->funName << ", total size of measVars:"<<sizeRead<<", size of charVars for VALUE:"<<sizeWrite<<", size of daqMeasVars:"<<sizeReadDAQ;
    qDebug()<<"Num of charVars for MAP:"<<charMapPamList.count();
}

bool Xcp_Fun_Thread::readSetting_simple()
{
    if(fun->setFilePath.isEmpty() || fun->intfName.isEmpty())
        return false;

    QString setFilePath = QCoreApplication::applicationDirPath() + fun->setFilePath;
    qDebug()<<"In xcp fun thread: read setting simple...";
    QSettings setting(setFilePath, QSettings::IniFormat);
    xcpConfig.workName = setting.value("Workspace").toString();
    xcpConfig.intfName = setting.value("Interface").toString();
    xcpConfig.a2lProjectName = setting.value("A2LProjectName").toString();
    xcpConfig.a2lFilePath = setting.value("A2LFilePath").toString();
    xcpConfig.hexFilePath = setting.value("HexFilePath").toString();

    xcpConfig.index_XcpOnCan = setting.value("Index_XcpOnCan").toInt();
    xcpConfig.baudrate = setting.value("Baudrate").toInt();
    xcpConfig.id_CMD = setting.value("ID_CMD").toInt();
    xcpConfig.id_RES = setting.value("ID_RES").toInt();

    xcpConfig.calCsvFilePath = setting.value("CalCsvFile").toString();
    xcpConfig.useA2lSetFile = setting.value("UseA2lSetFile").toBool();
    xcpConfig.usePamsInExcel = setting.value("UsePamsInExcel").toBool();

    // load [daq] [daq list] [event list] [event can id list]
    xcpConfig.daqList.clear();
    xcpConfig.eventCanIdList.clear();
    xcpConfig.eventList.clear();

    xcpConfig.a2lDaq.Name = setting.value("DaqName").toString();
    xcpConfig.a2lDaq.DaqType = setting.value("DaqType").toString();
    xcpConfig.a2lDaq.GranularityOdtEntrySize = setting.value("GranularityOdtEntrySize").toString();
    xcpConfig.a2lDaq.IdentificationField = setting.value("IdentificationField").toString();
    xcpConfig.a2lDaq.AddressExtType = setting.value("AddressExtType").toString();
    xcpConfig.a2lDaq.OptimisationType = setting.value("OptimisationType").toString();
    xcpConfig.a2lDaq.Max_Daq = (quint16)setting.value("Max_Daq").toUInt();
    xcpConfig.a2lDaq.Min_Daq = (quint16)setting.value("Min_Daq").toUInt();
    xcpConfig.a2lDaq.Max_Event_Channel = (quint8)setting.value("Max_Event_Channel").toUInt();
    xcpConfig.a2lDaq.Max_Odt_Entry_Size_Daq = (quint8)setting.value("Max_Odt_Entry_Size_Daq").toUInt();

    int daqCount = setting.beginReadArray("XcpDaqList");
    for (int i = 0; i < daqCount; i++)
    {
        setting.setArrayIndex(i);
        A2L_Daq_List *daq = new A2L_Daq_List();
        daq->Name = setting.value("DaqListName").toString();
        daq->DaqListNumber = (quint16)(setting.value("DaqListNumber").toUInt());
        daq->DaqListType = setting.value("DaqListType").toString();
        daq->Max_Odt = (quint8)setting.value("Max_Odt").toUInt();
        daq->Max_Odt_Entries = (quint8)setting.value("Max_Odt_Entries").toUInt();
        daq->First_Pid = (quint8)(setting.value("First_Pid").toUInt());
        daq->Event_Fixed = (quint8)(setting.value("Event_Fixed").toUInt());

        xcpConfig.daqList.append(daq);
    }
    setting.endArray();

    int eventCanIdCount = setting.beginReadArray("XcpEventCanIdList");
    for (int i = 0; i < eventCanIdCount; i++)
    {
        setting.setArrayIndex(i);
        A2L_EventCanId *eventCanId = new A2L_EventCanId();
        eventCanId->Name = setting.value("EventCanIdName").toString();
        eventCanId->EventNumber = (quint8)(setting.value("EventNumber").toUInt());
        eventCanId->Event_CAN_ID = (quint32)(setting.value("EventCanId").toUInt());

        xcpConfig.eventCanIdList.append(eventCanId);
    }
    setting.endArray();

    int eventCount = setting.beginReadArray("XcpEventList");
    for (int i = 0; i < eventCount; i++)
    {
        setting.setArrayIndex(i);
        A2L_Event *event = new A2L_Event();
        event->Name = setting.value("EventName").toString();
        event->ChannelNumber = (quint8)(setting.value("ChannelNumber").toUInt());
        event->EventChannelName = setting.value("EventChannelName").toString();
        event->ShortName = setting.value("ShortName").toString();
        event->EventType = setting.value("EventType").toString();
        event->Priority = (quint8)(setting.value("EventPriority").toUInt());
        event->TimeCycle = (quint8)(setting.value("TimeCycle").toUInt());
        event->TimeUnit = (quint8)(setting.value("TimeUnit").toUInt());
        event->Max_Daq_List = (quint8)(setting.value("MaxDaqList").toUInt());
        event->Rate_ms = setting.value("Rate_ms").toFloat();
        QString eventCanIdName = setting.value("RefEventCanIdName").toString();
        if(!eventCanIdName.isEmpty())
        {

        }

        xcpConfig.eventList.append(event);
    }

    setting.endArray();

    return true;
}

void Xcp_Fun_Thread::importA2LPamsFromCsv()
{
    int lastIndex = fun->setFilePath.lastIndexOf("/");
    QString workPath = QCoreApplication::applicationDirPath() + fun->setFilePath.left(lastIndex);

    //poll meas pams
    measPamList.clear();
    sizeRead = 0;

    //daq meas pams
    daqMeasPamList.clear();
    sizeReadDAQ = 0;

    //char pams
    charPamList.clear();
    sizeWrite = 0;

    //csv char pams
    csvCharPamList.clear();

    QString pamsPath = workPath + "/xcp_pams/" + fun->funName;
    QFileInfo dirInfo(pamsPath);
    if(!dirInfo.isDir())
    {
        QDir dir;
        dir.mkpath(pamsPath);
    }

    QString measPath_poll = pamsPath + "/measurements_poll.csv";
    importMeasFromCsv(measPath_poll, measPamList, sizeRead);

    QString measPath_daq = pamsPath + "/measurements_daq.csv";
    importMeasFromCsv(measPath_daq, daqMeasPamList, sizeReadDAQ);

    QString charPath = pamsPath + "/characteristics.csv";
    importCharsFromCsv(charPath, charPamList, sizeWrite);

    QString csvCharPath = pamsPath + "/characteristics_csv.csv";
    importCharsFromCsv(csvCharPath, csvCharPamList, sizeWriteCsv);

    QString logStr = "poll meas count=" + QString::number(measPamList.count()) +
            ", daq meas count=" + QString::number(daqMeasPamList.count()) +
            ", char count=" + QString::number(charPamList.count()) +
            ", csv char count=" + QString::number(csvCharPamList.count());

    qDebug()<<logStr;
    qDebug()<<"Total size of poll measVars:"<<sizeRead<<", size of daq measVars:"<<sizeReadDAQ<<", size of charVars:"<<sizeWrite;



}

/*
void Xcp_Fun_Thread::importMeasFromExcel(QString excelPath, QList<A2L_VarMeas *> &measList, quint64 &byteSize)
{
    ExcelOperator *measExcel = new ExcelOperator();
    measExcel->open(excelPath);
    QAxObject *sheet = measExcel->getSheet("Sheet1");
    if(sheet == NULL)
    {
        qDebug()<<"import meas: pams excel sheet not found!";
        return;
    }

    QAxObject *usedRange=sheet->querySubObject("UsedRange");
    QVariant var=usedRange->dynamicCall("value");

    int startBitIndex = 0;
    int startByteIndex = 0;
    startBitIndex += 0;

    QVariantList varRowContents=var.toList();
    const int rowCount=varRowContents.size();
    QVariantList tmp;
    for(int i=0; i<rowCount; ++i)
    {
        tmp=varRowContents[i].toList();

        A2L_VarMeas *measVar = new A2L_VarMeas();
        measVar->Name = tmp[0].toString();
        measVar->ShortName = tmp[1].toString();
        measVar->DataType = tmp[2].toString();
        measVar->ScalingFactor = tmp[3].toDouble();
        measVar->ScalingOffset = tmp[4].toDouble();
        measVar->Unit = tmp[5].toString();
        measVar->ECUAddress = (quint32)(tmp[6].toInt());
        measVar->DataSizeAG = (quint32)(tmp[7].toInt());
        measVar->LowerLimit = tmp[8].toDouble();
        measVar->UpperLimit = tmp[9].toDouble();
        measVar->ConversionType = tmp[11].toString();
        measVar->COEFFS = tmp[12].toString();
        measVar->StartByte = tmp[13].toInt();
        measVar->rate_ms = tmp[14].toFloat();
        measVar->smKey = tmp[15].toString();

        QStringList coefStrList = measVar->COEFFS.split(",");
        for (int i = 0; i < coefStrList.count(); i++)
        {
            QString str = coefStrList.at(i);
            measVar->CoefList.append(str.toDouble());
        }

        //measVar->StartByte = startByteIndex;
        //measVar->smKey = key;

        startBitIndex += (measVar->DataSizeAG * 8);
        startByteIndex = startBitIndex/8;

        measList.append(measVar);
    }

    byteSize = startBitIndex/8;

    measExcel->close();
}

void Xcp_Fun_Thread::importCharsFromExcel(QString excelPath, QList<A2L_VarChar *> &charList, quint64 &byteSize)
{
    ExcelOperator *charExcel = new ExcelOperator();
    charExcel->open(excelPath);
    QAxObject *sheet = charExcel->getSheet("Sheet1");
    if(sheet == NULL)
    {
        qDebug()<<"import char: pams excel sheet not found!";
        return;
    }

    QAxObject *usedRange=sheet->querySubObject("UsedRange");
    QVariant var=usedRange->dynamicCall("value");

    int startBitIndex = 0;
    int startByteIndex = 0;
    startBitIndex += 0;

    QVariantList varRowContents=var.toList();
    const int rowCount=varRowContents.size();
    QVariantList tmp;
    for(int i=0; i<rowCount; ++i)
    {
        tmp=varRowContents[i].toList();

        A2L_VarChar *charVar = new A2L_VarChar();
        charVar->Name = tmp[0].toString();
        charVar->Type = tmp[1].toString();
        charVar->DataType = tmp[2].toString();
        charVar->ScalingFactor = tmp[3].toDouble();
        charVar->ScalingOffset = tmp[4].toDouble();
        charVar->Unit = tmp[5].toString();
        charVar->ECUAddress = (quint32)(tmp[6].toInt());
        charVar->DataSizeAG = (quint32)(tmp[7].toInt());
        charVar->LowerLimit = tmp[8].toDouble();
        charVar->UpperLimit = tmp[9].toDouble();
        charVar->setValue(tmp[10].toDouble());
        charVar->ConversionType = tmp[11].toString();
        charVar->COEFFS = tmp[12].toString();
        charVar->StartByte = tmp[13].toInt();
        charVar->smKey = tmp[14].toString();

        QStringList coefStrList = charVar->COEFFS.split(",");
        for (int i = 0; i < coefStrList.count(); i++)
        {
            QString str = coefStrList.at(i);
            charVar->CoefList.append(str.toDouble());
        }

        //charVar->StartByte = startByteIndex;
        //charVar->smKey = key;
        charVar->funIndex = this->funIndex;
        charVar->pamModule = 1;

        startBitIndex += (charVar->DataSizeAG * 8);
        startByteIndex = startBitIndex/8;

        charList.append(charVar);
    }

    byteSize = startBitIndex/8;

    charExcel->close();
}
*/

bool Xcp_Fun_Thread::importMeasFromCsv(QString csvPath, QList<A2L_VarMeas*> &measList, quint64 &byteSize)
{
    QFile *csvFile = new QFile(csvPath);

    if(!csvFile->open(QIODevice::ReadOnly | QIODevice::Text))
    {
        qCritical()<<"csv file open error."<<csvPath;
        qDebug()<<csvFile->errorString();
        delete csvFile;

        return false;
    }

    QTextStream out;
    out.setDevice(csvFile);
    out.setCodec("GB2312");

    QString sepStr = ",";

    int startBitIndex = 0;
    int startByteIndex = 0;
    startBitIndex += 0;

    while(!csvFile->atEnd())
    {
        QString line = csvFile->readLine();

        QStringList strList = line.split(sepStr);
        if(strList.count() < 16) continue;

        A2L_VarMeas *measVar = new A2L_VarMeas();
        measVar->Name = strList[0];
        measVar->ShortName = strList[1];
        measVar->DataType = strList[2];
        measVar->ScalingFactor = QString(strList[3]).toDouble();
        measVar->ScalingOffset = QString(strList[4]).toDouble();
        measVar->Unit = strList[5];
        measVar->ECUAddress = (quint32)(QString(strList[6]).toInt());
        measVar->DataSizeAG = (quint32)(QString(strList[7]).toInt());
        measVar->LowerLimit = QString(strList[8]).toDouble();
        measVar->UpperLimit = QString(strList[9]).toDouble();
        measVar->ConversionType = strList[11];
        measVar->COEFFS = strList[12];
        measVar->StartByte = QString(strList[13]).toInt();
        measVar->rate_ms = QString(strList[14]).toFloat();
        measVar->smKey = strList[15];

        QStringList coefStrList = measVar->COEFFS.split(",");
        for (int i = 0; i < coefStrList.count(); i++)
        {
            QString str = coefStrList.at(i);
            measVar->CoefList.append(str.toDouble());
        }

        startBitIndex += (measVar->DataSizeAG * 8);
        startByteIndex = startBitIndex/8;

        measList.append(measVar);
    }

    byteSize = startBitIndex/8;

    csvFile->close();

    return true;
}

bool Xcp_Fun_Thread::importCharsFromCsv(QString csvPath, QList<A2L_VarChar*> &charList, quint64 &byteSize)
{
    QFile *csvFile = new QFile(csvPath);

    if(!csvFile->open(QIODevice::ReadOnly | QIODevice::Text))
    {
        qCritical()<<"csv file open error."<<csvPath;
        qDebug()<<csvFile->errorString();
        delete csvFile;

        return false;
    }

    QTextStream out;
    out.setDevice(csvFile);
    out.setCodec("GB2312");

    QString sepStr = ",";

    int startBitIndex = 0;
    int startByteIndex = 0;
    startBitIndex += 0;

    while(!csvFile->atEnd())
    {
        QString line = csvFile->readLine();

        QStringList strList = line.split(sepStr);
        if(strList.count() < 15) continue;

        A2L_VarChar *charVar = new A2L_VarChar();
        charVar->Name = strList[0];
        charVar->charType = strList[1];
        charVar->DataType = strList[2];
        charVar->ScalingFactor = QString(strList[3]).toDouble();
        charVar->ScalingOffset = QString(strList[4]).toDouble();
        charVar->Unit = strList[5];
        charVar->ECUAddress = (quint32)(QString(strList[6]).toInt());
        charVar->DataSizeAG = (quint32)(QString(strList[7]).toInt());
        charVar->LowerLimit = QString(strList[8]).toDouble();
        charVar->UpperLimit = QString(strList[9]).toDouble();
        charVar->ConversionType = strList[11];
        charVar->COEFFS = strList[12];
        charVar->StartByte = QString(strList[13]).toInt();
        charVar->smKey = strList[14];

        QStringList coefStrList = charVar->COEFFS.split(",");
        for (int i = 0; i < coefStrList.count(); i++)
        {
            QString str = coefStrList.at(i);
            charVar->CoefList.append(str.toDouble());
        }

        charVar->funIndex = this->funIndex;
        charVar->pamModule = 1;

        startBitIndex += (charVar->DataSizeAG * 8);
        startByteIndex = startBitIndex/8;


        charList.append(charVar);
    }

    byteSize = startBitIndex/8;

    csvFile->close();

    return true;
}

bool Xcp_Fun_Thread::createPams()
{
    QString appPath = QApplication::applicationDirPath();
    QString fileName = "XCP_COMM_"+fun->funName + ".xml";
    QString xmlPath = appPath + "/pamFactory/" + fileName;

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

QList<CalInfo> Xcp_Fun_Thread::importCalInfoListFromCsv(QString csvPath)
{
    QList<CalInfo> calInfos;

    if(csvPath.isEmpty())
        return calInfos;

    CaliCsv *caliCsv = new CaliCsv(nullptr, csvPath);
    qDebug()<<"Import cal info list from csv file done!";

    return caliCsv->getCalList();

}

QList<A2L_VarChar*> Xcp_Fun_Thread::getCharVarListFromCalInfoList(A2L_Project *a2lProject, QList<CalInfo> calList)
{
    QList<A2L_VarChar*> charVarList;

    if(!a2lProject || calList.isEmpty()) return charVarList;

    for(int i = 0; i < calList.count(); i++)
    {
        CalInfo calInfo = calList.at(i);

        qDebug()<<"calInfo index:"<<i;

        if(charTypeList.indexOf(calInfo.type) == -1)
            continue;

        A2L_VarChar *charVar = a2lProject->getCharVarByName(calInfo.charName);

        if(!charVar)
            continue;

        if(charVar->charType == "VALUE")
        {
            QString valueStr = calInfo.zValueStrArray.at(0);
            bool ok = false;
            double value = valueStr.toDouble(&ok);

            charVar->setValue(ok ? value : 0);

            if(charVar->ConversionType == "TAB_VERB" && !charVar->ConversionVTab.isEmpty())
            {
                QString valuePairStr = charVar->ConversionVTab;
                QStringList valuePairList = valuePairStr.split(",");
                foreach (QString valuePair, valuePairList) {
                    int index = valuePair.indexOf(valueStr);
                    if(index != -1)
                    {
                        QString str = valuePair.split("-").first();
                        value = str.toDouble();
                        charVar->setValue(value);
                    }
                }
            }
        }
        else if(charVar->charType == "VAL_BLK")
        {
            QStringList blkValueStr = calInfo.zValueStrArray;
            bool ok = false;
            for(int i = 0; i < blkValueStr.count(); i++)
            {
                QString valueStr = blkValueStr.at(i);
                double value = valueStr.toDouble(&ok);
                if(i < charVar->number_BLK && ok)
                {
                    charVar->setBlkValue(i, 1, value);
                }
            }

            qDebug()<<"~~~Set Blk value[0]" << charVar->Blk_Values[0] << ", blk count:"<<charVar->Blk_Values.count();
        }
        else if(charVar->charType == "CURVE")
        {
            qDebug()<<"~~~from hex:curve xAxis Value:"<<charVar->hexValue_XList;
            qDebug()<<"~~~from hex:curve zValue:"<<charVar->hexValue_ZList;
            qDebug()<<"~~~from calInfo:curve xAxis Value:"<<calInfo.xAxisStrArray;
            qDebug()<<"~~~from calInfo:curve zValue:"<<calInfo.zValueStrArray;

            QStringList zValueStrList = calInfo.zValueStrArray;
            QStringList xValueStrList = calInfo.xAxisStrArray;
            bool ok1 = false;
            bool ok2 = false;
            for(int i = 0; i < zValueStrList.count(); i++)
            {
                QString valueStr = zValueStrList.at(i);
                QString xAxisStr = xValueStrList.at(i);
                double value = valueStr.toDouble(&ok1);
                if(i < charVar->zCount && ok1)
                {
                    charVar->hexValue_ZList.replace(i, value);
                }
                double xValue = xAxisStr.toDouble(&ok2);
                if(i < charVar->xCount && ok2)
                {
                    charVar->hexValue_XList.replace(i, xValue);
                }
            }

            qDebug()<<"~~~from cal csv:curve xAxis Value:"<<charVar->hexValue_XList;
            qDebug()<<"~~~from cal csv:curve zValue:"<<charVar->hexValue_ZList;
        }
        else if(charVar->charType == "MAP")
        {

        }
        else
        {

        }

        charVarList.append(charVar);
    }

    return charVarList;
}

bool Xcp_Fun_Thread::downloadCalList(A2L_Project *a2lProject, QList<CalInfo> calList)
{
    if(calList.isEmpty() || a2lProject == NULL || xcpMaster == NULL)
        return false;

    QList<A2L_VarChar*> charVarList = getCharVarListFromCalInfoList(a2lProject, calList);

    if(!xcpMaster || charVarList.isEmpty())
        return false;

    if(!xcpMaster->getXcpSessionConnectedStatus())
        return false;

    qDebug()<<"=====Begine to download cal list.=====";
    int max = charVarList.count();
    for(int i = 0; i < max; i++)
    {
        A2L_VarChar *charVar = charVarList.at(i);
        if(charVar->charType == "VALUE")
        {
            if(!xcpMaster->XCP_Cal_VALUE(charVar))
            {
                qDebug()<<"VALUE Charac:"<<charVar->Name<<", calibrate error.";
            }
            else
            {
                qDebug()<<"VALUE Charac:"<<charVar->Name<<", download ok: "<<charVar->getValue();
            }
        }
        else if(charVar->charType == "VAL_BLK")
        {
            if(!xcpMaster->XCP_Cal_VAL_BLK(charVar))
            {
                qDebug()<<"VAL_BLK Charac:"<<charVar->Name<<", calibrate error.";
            }
            else
            {
                qDebug()<<"VAL_BLK Charac:"<<charVar->Name<<", download ok: "<<charVar->Blk_Values;
            }
        }
        else if(charVar->charType == "CURVE")
        {
            if(!xcpMaster->XCP_Cal_CURVE(charVar))
            {
                qDebug()<<"CURVE Charac:"<<charVar->Name<<", calibrate error.";
            }
            else
            {
                qDebug()<<"CURVE Charac:"<<charVar->Name<<", download ok. zValues:"<<charVar->hexValue_ZList;
            }
        }
        else
        {

        }

        int percentage = ((float)(i+1))/max*100;
    }

    return true;
}

bool Xcp_Fun_Thread::charVars_Download(QList<A2L_VarChar *> charVars)
{
    if(!xcpMaster || charVars.isEmpty())
        return false;
    if(!xcpMaster->getXcpSessionConnectedStatus())
        return false;

    for(int i = 0; i < charVars.count(); i++)
    {
        A2L_VarChar *charVar = charVars.at(i);

        if(!xcpMaster->XCP_Cal_VALUE(charVar))
        {
            qCritical()<<"Charac:"<<charVar->Name<<", Calibrate error.";
            return false;
        }
        else
        {
            qDebug()<<"Charac:"<<charVar->Name<<", download ok:"<<charVar->getValue();
        }
    }

    return true;
}

void Xcp_Fun_Thread::init()
{
    xcpMaster = new XCPMaster(nullptr, fun->funName, xcpConfig.xcpDeviceType);
    connect(this, &Xcp_Fun_Thread::sigDownload, xcpMaster, &XCPMaster::downloadCals);
    connect(this, &Xcp_Fun_Thread::daqRunStatusActive, xcpMaster, &XCPMaster::sltDaqRunStatusActive);

    xcpPollThread = new XCP_Polling_Thread(nullptr,  xcpMaster);
    charPamCheckThread = new CharPamCheck(nullptr, xcpMaster);
    mapCharPamCheckThread = new MapCharPamCheckThread(nullptr, xcpMaster);

    connect(xcpPollThread, &XCP_Polling_Thread::measPamsValueUpdated, this, &Xcp_Fun_Thread::fromReadSMToMeasVars);
    connect(xcpPollThread, &XCP_Polling_Thread::pollingStatusChanged, this, &Xcp_Fun_Thread::pollingStatusChanged);
    connect(xcpPollThread, &XCP_Polling_Thread::caliStatusChanged, this, &Xcp_Fun_Thread::caliStatusChanged);

    connect(this, &Xcp_Fun_Thread::pollingStatusActive, xcpPollThread, &XCP_Polling_Thread::sltPollingStatusActive);

    xcpState = 0;
    emit xcpFunStateChanged(0, funIndex);

    this->calInfoList = importCalInfoListFromCsv(xcpConfig.calCsvFilePath);
}

void Xcp_Fun_Thread::startXCP()
{
    xcpStartFinished = false;
    xcpStopFinished = false;

    if(measPamList.isEmpty() && charPamList.isEmpty() && charMapPamList.isEmpty() && daqMeasPamList.isEmpty())
        return;

    //creat sharedMemory
    createSM();
    createSMInMAP();

    //init write sharedMemory
    fromCharVarsToWriteSM();

    //init map shareMemory
    fromMapCharVarsToMapWriteSM();

    xcpMaster->setXcpName(fun->funName);
    xcpMaster->setXcpCanInterface(fun->intfName);
    xcpMaster->setBaudrate(xcpConfig.baudrate);
    xcpMaster->setIDMasterToSlave(xcpConfig.id_CMD);
    xcpMaster->setIDSlaveToMaster(xcpConfig.id_RES);
    xcpMaster->setResistanceEnable(0);

    xcpMaster->setIsCanFd(xcpConfig.isCanFD);
    xcpMaster->setXcpFdBaudrate(xcpConfig.fdBaudrate);
    xcpMaster->setMaxDLC(xcpConfig.maxDLC);

    xcpMaster->setPollMeasList(this->measPamList);

    xcpMaster->setUseSeedKey(xcpConfig.useSeedKey);
    xcpMaster->setSeedKeyDllPath(xcpConfig.seedKeyDllPath);
    xcpMaster->setXcpDeviceType(xcpConfig.xcpDeviceType);

    xcpMaster->setZlgDeviceType(xcpConfig.zlgDeviceType);
    xcpMaster->setZlgDeviceIndex(xcpConfig.zlgDeviceIndex);

    //xcp memory segment
    if(curA2LProject)
    {
        xcpMaster->setXcpMemSegList(curA2LProject->getA2lMemSegList());
    }

    //xcp daq
    if(curA2LProject && !(xcpConfig.useA2lSetFile))
    {
        xcpMaster->setXcpDaq(curA2LProject->getDaqInfo());
        xcpMaster->setXcpDaqList(curA2LProject->getDaqListStructList());
        xcpMaster->setXcpEventList(curA2LProject->getEventList());
        xcpMaster->setXcpEventCanIdList(curA2LProject->getA2lEventCanIdList());
    }
    else
    {
        xcpMaster->setXcpDaq(&(xcpConfig.a2lDaq));
        xcpMaster->setXcpDaqList(xcpConfig.daqList);
        xcpMaster->setXcpEventList(xcpConfig.eventList);
        xcpMaster->setXcpEventCanIdList(xcpConfig.eventCanIdList);

    }

    xcpMaster->setMeasVarListForDAQ(this->daqMeasPamList);

    //XCP CAN Init
    if(!xcpMaster->XCPInit())
    {
        qCritical()<<"XCP功能【CAN初始化】失败";
        emit xcpMsg(fun->funName + ": XCP功能【CAN初始化】失败");
        xcpCanInitOk = false;
        xcpState = -1;
        return;
    }
    xcpCanInitOk = true;
    xcpState = 1;
    emit xcpCanInitStatus(2);
    emit xcpMsg(fun->funName + ": XCP功能【CAN初始化】成功");
    qInfo()<<"XCP功能【CAN初始化】成功！";
    emit xcpFunStateChanged(1, funIndex);

    //XCP  setup connection
    if(!xcpMaster->XCP_Setup_Session())
    {
        qCritical()<<"XCP功能【建立连接】失败";
        emit xcpMsg(fun->funName + ": XCP功能【建立连接】失败");
        xcpState = -1;
        return;
    }
    xcpSetupOk = true;
    xcpState = 2;
    emit xcpConnStatus(2);
    emit xcpMsg(fun->funName + ": XCP功能【建立连接】成功! 准备进行读写");
    qInfo()<<"XCP功能【建立连接】成功! 准备进行读写=";
    emit xcpFunStateChanged(2, funIndex);

    if(curA2LProject && !(xcpConfig.usePamsInExcel))
    {
        if(downloadCalList(curA2LProject, this->calInfoList))
        {
            qInfo()<<"【下载CSV标定清单】成功!";
        }
        else
        {
            qCritical()<<"【下载CSV标定清单】失败";
        }
    }
    else
    {
        if(charVars_Download(csvCharPamList))
        {
            qInfo()<<"【下载CSV标定清单】文件成功!";
        }
        else
        {
            qCritical()<<"【下载CSV标定清单】文件失败";
        }
    }


    //char pams init download
    if(charVars_Download(charPamList))
    {
        qInfo()<<"【下载所选标定】参数值成功!";
    }
    else
    {
        qCritical()<<"【下载所选标定】参数值失败";
    }

    //xcp daq config
    if(!xcpDaqConfig())
    {
        xcpState = -1;
        qCritical()<<"XCP功能【DAQ配置】失败";
        emit xcpMsg("XCP功能【DAQ配置】失败");
    }

    emit xcpMsg(fun->funName + ": XCP功能【DAQ配置】成功!");

    //save pam info to xml
    createPams();

    //map pam check
    mapCharPamCheckThread->setMapCharPamList(charMapPamList);
    mapCharPamCheckThread->setMapCharSMHash(smMapWriteHash);
    mapCharPamCheckThread->setMapCharCheckRunFlag(true);
    mapCharPamCheckThread->start();
    qInfo()<<"XCP功能【MAP参数】校验线程启动";

    //char pam check
    charPamCheckThread->setCharPamList(charPamList);
    charPamCheckThread->setSMKey(smKeyWrite);
    charPamCheckThread->setSmWriteSize(sizeWrite);
    charPamCheckThread->setCharCheckRunFlag(true);
    charPamCheckThread->start();
    qInfo()<<"XCP功能【标定】参数校验线程启动";

    //polling thread
    xcpPollThread->setMeasPamList(measPamList);
    xcpPollThread->setSmRead(this->smRead);
    xcpPollThread->setSMKey(this->smKeyRead);
    xcpPollThread->setSmReadSize(this->sizeRead);
    xcpPollThread->setPollRunFlag(pollingRun);
    xcpPollThread->start();
    qInfo()<<"XCP功能【轮询采集】线程启动";

    emit xcpPollingStatus(2);
    emit xcpCaliStatus(2);
    emit xcpMsg(fun->funName + ": XCP功能【标定】和【轮询采集】线程启动");

    connect(charPamCheckThread, &CharPamCheck::addCaliAction, xcpPollThread, &XCP_Polling_Thread::addCaliActionSlot);
    connect(mapCharPamCheckThread, &MapCharPamCheckThread::addMapCaliAction, xcpPollThread, &XCP_Polling_Thread::addMapCaliActionSlot);

    // xcp daq start
    if(!setXcpDaqStartStop(daqRun))
    {
        xcpState = -1;
        qCritical()<<"XCP功能【DAQ采集】功能启动失败";
    }
    else
    {
        xcpState = 3;
        emit xcpDaqRunStatus(2);
        emit xcpMsg(fun->funName + ": XCP功能【DAQ采集】功能启动成功");
        emit xcpFunStateChanged(3, funIndex);
    }

    xcpStartFinished = true;
    xcpStopFinished = false;
}

void Xcp_Fun_Thread::stopXCP()
{
    xcpStartFinished = false;
    xcpStopFinished = false;

    if(xcpPollThread == NULL || charPamCheckThread == NULL) return;
    if(a2lWin == NULL || xcpMaster == NULL) return;

    if(!setXcpDaqStartStop(false))
    {
        qCritical()<<"XCP功能【DAQ采集】功能停止错误";
    }
    xcpState = 2;
    emit xcpDaqRunStatus(0);
    emit xcpMsg("XCP功能【DAQ采集】功能停止");
    emit xcpFunStateChanged(2, funIndex);

    disconnect(charPamCheckThread, &CharPamCheck::addCaliAction, xcpPollThread, &XCP_Polling_Thread::addCaliActionSlot);
    disconnect(mapCharPamCheckThread, &MapCharPamCheckThread::addMapCaliAction, xcpPollThread, &XCP_Polling_Thread::addMapCaliActionSlot);

    // 使用stop()方法安全停止线程，增加超时和强制终止处理
    charPamCheckThread->setCharCheckRunFlag(false);
    charPamCheckThread->stop();
    if (!charPamCheckThread->wait(3000)) {
        charPamCheckThread->terminate();
        charPamCheckThread->wait();
    }
    charPamCheckThread->deleteLater();
    charPamCheckThread = nullptr;

    mapCharPamCheckThread->setMapCharCheckRunFlag(false);
    mapCharPamCheckThread->stop();
    if (!mapCharPamCheckThread->wait(3000)) {
        mapCharPamCheckThread->terminate();
        mapCharPamCheckThread->wait();
    }
    mapCharPamCheckThread->deleteLater();
    mapCharPamCheckThread = nullptr;

    xcpPollThread->setPollRunFlag(false);
    xcpPollThread->stop();
    if (!xcpPollThread->wait(3000)) {
        xcpPollThread->terminate();
        xcpPollThread->wait();
    }
    xcpPollThread->deleteLater();
    xcpPollThread = nullptr;

    xcpState = 1;
    emit xcpPollingStatus(0);
    emit xcpCaliStatus(0);
    emit xcpMsg(fun->funName + ": XCP功能【标定】和【轮询】功能停止");
    emit xcpFunStateChanged(1, funIndex);

    xcpMaster->XCPStop();
    qInfo()<<"XCP功能停止, 关闭CAN";
    xcpState = 0;
    emit xcpConnStatus(0);
    emit xcpCanInitStatus(0);
    emit xcpMsg(fun->funName + ": XCP功能停止");
    emit xcpFunStateChanged(0, funIndex);

    deleteSM();
    deleteSMInMAP();

    delete xcpMaster;
    xcpMaster = NULL;

    qDeleteAll(measPamList);
    measPamList.clear();
    qDeleteAll(daqMeasPamList);
    daqMeasPamList.clear();
    qDeleteAll(charPamList);
    charPamList.clear();
    qDeleteAll(csvCharPamList);
    csvCharPamList.clear();

    xcpStartFinished = false;
    xcpStopFinished = true;
}

bool Xcp_Fun_Thread::xcpDaqConfig()
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

bool Xcp_Fun_Thread::setXcpDaqStartStop(bool status)
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


void Xcp_Fun_Thread::createSM()
{
    if(smRead == NULL)
    {
        smRead = new QSharedMemory(smKeyRead);
    }
    smRead->create(8+sizeRead);

    if(smReadDAQ == NULL)
    {
        smReadDAQ = new QSharedMemory(smKeyReadDAQ);
    }
    smReadDAQ->create(8+sizeReadDAQ);

    if(smWrite == NULL)
    {
        smWrite = new QSharedMemory(smKeyWrite);
    }
    smWrite->create(8+sizeWrite);
}

void Xcp_Fun_Thread::deleteSM()
{
    delete smRead;
    smRead = NULL;

    delete smReadDAQ;
    smReadDAQ = NULL;

    delete smWrite;
    smWrite = NULL;
}

void Xcp_Fun_Thread::createSMInMAP()
{
    for(int i = 0; i < charMapPamList.count(); i++)
    {
        A2L_VarChar *mapChar = charMapPamList.at(i);

        mapChar->smKey = mapListKey + "_" + mapChar->Name;

        QSharedMemory *sm = new QSharedMemory(mapListKey + "_" + mapChar->Name);
        int size = mapChar->zCount * mapChar->DataSizeAG;
        if(!sm->create(size+8))
        {
            qDebug()<<"create map sharedmemory error, "<<mapChar->Name;
        }

        smMapWriteList.append(sm);
        smMapWriteHash.insert(mapChar, sm);
    }
}

void Xcp_Fun_Thread::deleteSMInMAP()
{
    qDeleteAll(smMapWriteList);
    smMapWriteList.clear();

    smMapWriteHash.clear();
}

void Xcp_Fun_Thread::fromCharVarsToWriteSM()
{
    if(!smWrite) return;

    char *buffer = new char[sizeWrite];
    fromCharVarsToWriteRawData(buffer, sizeWrite);
    quint64 initTime = 0;

    copyToSharedMemory_2Data(smWrite, 0, (char*)&initTime, 8, 8, buffer, sizeWrite);

    emit writePamValueUpdated();

    delete[] buffer;
}

void Xcp_Fun_Thread::fromCharVarsToWriteRawData(char *data, quint64 size)
{
    char *buffer = new char[size];

    for(int i = 0; i < charPamList.count(); i++)
    {
        A2L_VarChar *charVar = charPamList.at(i);

        value2IntelData(buffer, charVar);
    }

    memcpy(data, buffer, size);

    delete[] buffer;
}

void Xcp_Fun_Thread::fromMapCharVarsToMapWriteSM()
{
    for(int i = 0; i < charMapPamList.count(); i++)
    {
        A2L_VarChar *charVar = charMapPamList.at(i);

        int size = (charVar->zCount * charVar->DataSizeAG + 8);

        QSharedMemory *sm = smMapWriteHash.value(charVar);

        int offset = 8;
        for(int j = 0; j < charVar->zCount; j++)
        {

            qreal value = charVar->hexValue_ZList.at(j);
            char *data = new char[charVar->DataSizeAG];
            transferPhyValueToRawData(charVar, value, data);

            copyToSharedMemory(sm, offset, data, charVar->DataSizeAG);
            /*
            sm->lock();
            memcpy((char*)sm->data()+offset, data, charVar->DataSizeAG);
            sm->unlock();
            */

            delete[] data;

            offset += charVar->DataSizeAG;
        }
    }
}

void Xcp_Fun_Thread::transferPhyValueToRawData(A2L_VarChar *charVar, double value, char *data)
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

void Xcp_Fun_Thread::value2IntelData(char *data, A2L_VarChar *charVar)
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

QHash<quint16, QList<PARAM *> > Xcp_Fun_Thread::getPamHashFromMeasHash(QHash<quint16, QList<A2L_VarMeas *> > measHash)
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

QList<PARAM *> Xcp_Fun_Thread::transferMeasToPams(QList<A2L_VarMeas *> measPamList)
{
    QList<PARAM*> pamList;

    if(measPamList.isEmpty())
        return pamList;

    quint32 startBitIndex = 0;
    quint32 startByteIndex = 0;

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

void Xcp_Fun_Thread::updateMapValueInSM(A2L_VarChar *map, quint16 index_x, quint16 index_y, char *data)
{
    //qDebug()<<"In update MapValue in SM."<<map->Name<<",index_x="<<index_x<<",index_y="<<index_y;
    if(!map)
        return;

    QSharedMemory sm;
    sm.setKey(map->smKey);

    if (!sm.isAttached())
    {
        if(!sm.attach())
        {
            qDebug()<<"unable attach to map sharedMemory :"<<sm.key();
            return;
        }
    }

    int index = 8;
    if(map->yCount == 0 && map->xCount != 0)
    {
        index += (map->DataSizeAG * index_x);
    }
    else
    {
        index += map->DataSizeAG * (map->yCount * index_x + index_y);
    }

    qDebug()<<"index offset="<<index;

    sm.lock();
    memcpy((char*)(sm.data())+index, data, map->DataSizeAG);
    sm.unlock();

    //qDebug()<<"map Value updated."<<map->Name<<",index_x="<<index_x<<", index_y="<<index_y;

}


int Xcp_Fun_Thread::getXcpState() const
{
    return xcpState;
}

QHash<A2L_VarChar *, QSharedMemory *> Xcp_Fun_Thread::getSmMapWriteHash() const
{
    return smMapWriteHash;
}


bool Xcp_Fun_Thread::getXcpStartFinished() const
{
    return xcpStartFinished;
}

QString Xcp_Fun_Thread::getSmKeyWrite() const
{
    return smKeyWrite;
}

QString Xcp_Fun_Thread::getSmKeyReadDAQ() const
{
    return smKeyReadDAQ;
}

QString Xcp_Fun_Thread::getSmKeyRead() const
{
    return smKeyRead;
}


QString Xcp_Fun_Thread::getMapListKey() const
{
    return mapListKey;
}

QList<A2L_VarChar *> Xcp_Fun_Thread::getCharMapPamList() const
{
    return charMapPamList;
}

QList<A2L_VarChar *> Xcp_Fun_Thread::getCharPamList() const
{
    return charPamList;
}

QList<A2L_VarMeas *> Xcp_Fun_Thread::getDaqMeasPamList() const
{
    return daqMeasPamList;
}

QList<PARAM *> Xcp_Fun_Thread::getPamList() const
{
    QList<PARAM*> pamList;
    foreach (A2L_VarMeas *meas, measPamList) {
        pamList.append((PARAM*)meas);
    }

    return pamList;
}

QList<PARAM *> Xcp_Fun_Thread::getDaqPamList() const
{
    QList<PARAM*> pamList;
    foreach (A2L_VarMeas *meas, daqMeasPamList) {
        pamList.append((PARAM*)meas);
    }

    return pamList;
}

QList<PARAM *> Xcp_Fun_Thread::getCharValuePamList() const
{
    QList<PARAM*> pamList;
    foreach (A2L_VarChar *charVar, charPamList) {
        PARAM *charPam = (PARAM*)charVar;

        connect(charPam, QOverload<PARAM*, double>::of(&PARAM::valueUpdated), this, &Xcp_Fun_Thread::updatePamValueInSM);

        pamList.append(charPam);
    }

    return pamList;
}

QList<PARAM *> Xcp_Fun_Thread::getCharCurveMapPamList() const
{
    QList<PARAM*> pamList;
    foreach (A2L_VarChar *charVar, charMapPamList) {
        PARAM *charPam = (PARAM*)charVar;

        connect(charPam, &PARAM::mapValueUpdated, this, &Xcp_Fun_Thread::updatePamMapValueInSM);

        pamList.append(charPam);
    }

    return pamList;
}

QList<A2L_VarMeas *> Xcp_Fun_Thread::getMeasPamList() const
{
    return measPamList;
}

void Xcp_Fun_Thread::setFunInfo(FunStruct *value, int index)
{
    fun = value;
    funIndex = index;
}

void Xcp_Fun_Thread::setXcpConfig(const XcpConfig &value)
{
    xcpConfig = value;
}

