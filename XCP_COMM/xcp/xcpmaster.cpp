#include "xcpmaster.h"
#include <QCoreApplication>
#include <QDebug>
#include "string"


XCPMaster::XCPMaster(QObject *parent, QString name, int canType) :
    QObject(parent), xcpName(name), xcpDeviceType(canType)
{
    for (int i = 0; i < 64; i++)
    {
        CTO_CMDPacket[i] = 0;
        CTO_RESPacket[i] = 0;
    }

    if(xcpDeviceType == 0)
    {
        xcpCanThread = new XCP_Thread(this);
        connect(xcpCanThread->thread_RES, &XCP_R_Thread::RESDataReady, this, &XCPMaster::getCTORESData);
    }
    else if(xcpDeviceType == 1)
    {
        xcpTsCanThread = new XCP_Thread_TS(this);
        connect(xcpTsCanThread->thread_RES, &XCP_R_Thread_TS::RESDataReady_TS, this, &XCPMaster::getCTORESData_TS);
    }
    else if(xcpDeviceType == 2)
    {
        xcpZlgCanThread = new XCP_Thread_ZLG(this);
        connect(xcpZlgCanThread->thread_RES, &XCP_R_Thread_ZLG::RESDataReady_ZLG, this, &XCPMaster::getCTORESData_ZLG);
    }

    varsDAQConfigured = false;

    if(QThread::currentThread() == qApp->thread())
    {
        qDebug()<<"XcpMaster run in Main Thread"<<", MainThread:"<<qApp->thread()<<", Sub Thread:"<<this->thread();

    }
    else if(QThread::currentThread() == this->thread())
    {
        qDebug()<<"XcpMaster run in Sub Thread"<<", MainThread:"<<qApp->thread()<<", Sub Thread:"<<this->thread();
    }
    else
    {
        qDebug()<<"XcpMaster run in other Thread";
    }
}

XCPMaster::~XCPMaster()
{
    if(xcpCanThread)
    {
        delete xcpCanThread;
        xcpdbcRemove();
    }
    if(xcpTsCanThread)
    {
        delete xcpTsCanThread;
    }

}

bool XCPMaster::XCPInit()
{
    //验证daq参数的安排
    /*
    if(!isCanFd)
    {
        arrangeMeasVars();
    }
    else
    {
        arrangeMeasVars_dlc64();
    }
    */

    if(xcpDeviceType == 0)
    {
        qInfo()<<"选择NI-XNET硬件";
        return XCPInit_NI();
    }
    else if(xcpDeviceType == 1)
    {
        qInfo()<<"选择TS_CAN硬件";
        return XCPInit_TS();
    }
    else if(xcpDeviceType == 2)
    {
        qInfo()<<"选择ZLG_CAN硬件";
        return XCPInit_ZLG();
    }

}

void XCPMaster::XCPStop()
{
    if(xcpDeviceType == 0)
    {
        XCPStop_NI();
    }
    else if(xcpDeviceType == 1)
    {
        XCPStop_TS();
    }
    else if(xcpDeviceType == 2)
    {
        XCPStop_ZLG();
    }
}

bool XCPMaster::XCPInit_NI()
{
    //testing setting
    //isCanFd = false;
    //xcpFdBaudrate = 2000000;
    //maxDLC = 64;


    qDebug()<<"XCP初始化:更新XCP dbc信息";
    xcpdbcUpdate();

    xcpCanThread->path = xcpdbc_Path;
    xcpCanThread->port = xcpcan_Interface;
    xcpCanThread->cluster = xcp_Cluster;
    xcpCanThread->frames_r = FrameText_RES;
    xcpCanThread->frames_w = FrameText_CMD;
    xcpCanThread->frames_r_event = FrameText_EVENT;

    xcpCanThread->isCanFd = this->isCanFd;
    xcpCanThread->maxDLC = this->maxDLC;


    xcpCanInitFlag = xcpCanThread->XCP_CAN_Init();
    qDebug()<<"XCP初始化 : 初始化CAN端口状态:"<<xcpCanInitFlag;

    return xcpCanInitFlag;
}

void XCPMaster::XCPStop_NI()
{
    CTO_DISCONNECT();

    xcpCanThread->XCP_CAN_Stop();

    xcpCanInitFlag = false;
}

bool XCPMaster::XCPInit_TS()
{
    qDebug()<<"XCP初始化:TS_CAN CanIndex:"<<this->xcpcan_Interface;

    xcpTsCanThread->id_CMD = this->ID_Master_to_Slave;
    xcpTsCanThread->id_RES = this->ID_Slave_to_Master;
    xcpTsCanThread->canfd_abitBaudrate = this->xcpBaudrate;
    xcpTsCanThread->canfd_dbitBaudrate = this->xcpBaudrate;
    xcpTsCanThread->canType = isCanFd ? 1 : 0; // 0 = can, 1 = canfd
    xcpTsCanThread->canfd_standard = 1; // 0 = can, 1 = iso canfd, 2 = no iso canfd
    xcpTsCanThread->resistanceEnable = this->resistanceEnable;
    if(isCanFd)
    {
        xcpTsCanThread->canfd_dbitBaudrate = this->xcpFdBaudrate;
    }
    xcpTsCanThread->maxDLC = this->maxDLC;

    //从A2L_EventCanId中提取DAQ的event id值
    int eventIdCount = this->xcpEventCanIdList.count();
    idList.clear();
    for(int i = 0; i < eventIdCount; i++)
    {
        A2L_EventCanId *eventCanId = xcpEventCanIdList.at(i);
        quint32 id = eventCanId->Event_CAN_ID;
        idList.append(id);
    }
    xcpTsCanThread->thread_RES->eventIdList = idList;

    int portIndex = this->xcpcan_Interface.replace("CAN", "").toUInt();
    tsCanIndex = (APP_CHANNEL)portIndex; // String "CAN0" == APP_CHANNEL: CAN1
    xcpTsCanThread->canIndex = tsCanIndex;


    xcpCanInitFlag = xcpTsCanThread->XCP_CAN_Init();
    qDebug()<<"XCP初始化 : 初始化CAN端口状态:"<<xcpCanInitFlag;

    return xcpCanInitFlag;
}

void XCPMaster::XCPStop_TS()
{
    CTO_DISCONNECT();

    xcpTsCanThread->XCP_CAN_Stop();

    xcpCanInitFlag = false;
}

bool XCPMaster::XCPInit_ZLG()
{
    qDebug()<<"XCP初始化:ZLG_CAN DeviceType:"<<zlgDeviceType
            <<", DeviceIndex:"<<zlgDeviceIndex<<", CanIndex:"<<this->xcpcan_Interface;

    xcpZlgCanThread->deviceType = zlgDeviceType;  // 41 = ZCAN_USBCANFD_200U
    xcpZlgCanThread->deviceIndex = zlgDeviceIndex;
    xcpZlgCanThread->id_CMD = this->ID_Master_to_Slave;
    xcpZlgCanThread->id_RES = this->ID_Slave_to_Master;
    xcpZlgCanThread->canfd_abitBaudrate = this->xcpBaudrate;
    xcpZlgCanThread->canfd_dbitBaudrate = this->xcpBaudrate;
    xcpZlgCanThread->canType = isCanFd ? 1 : 0; // 0 = can, 1 = canfd
    xcpZlgCanThread->resistanceEnable = this->resistanceEnable;
    if(isCanFd)
    {
        xcpZlgCanThread->canfd_dbitBaudrate = this->xcpFdBaudrate;
    }
    xcpZlgCanThread->maxDLC = this->maxDLC;

    //从A2L_EventCanId中提取DAQ的event id值
    int eventIdCount = this->xcpEventCanIdList.count();
    idList.clear();
    for(int i = 0; i < eventIdCount; i++)
    {
        A2L_EventCanId *eventCanId = xcpEventCanIdList.at(i);
        quint32 id = eventCanId->Event_CAN_ID;
        idList.append(id);
    }
    xcpZlgCanThread->thread_RES->eventIdList = idList;

    int portIndex = this->xcpcan_Interface.replace("CAN", "").toUInt();
    zlgCanIndex = portIndex;
    xcpZlgCanThread->canIndex = zlgCanIndex;


    xcpCanInitFlag = xcpZlgCanThread->XCP_CAN_Init();
    qDebug()<<"XCP初始化 : 初始化CAN端口状态:"<<xcpCanInitFlag;

    return xcpCanInitFlag;
}

void XCPMaster::XCPStop_ZLG()
{
    CTO_DISCONNECT();

    xcpZlgCanThread->XCP_CAN_Stop();

    xcpCanInitFlag = false;
}

void XCPMaster::XCPDemo()
{
    XCP_Setup_Session();
    XCP_GetInfos();
}

bool XCPMaster::XCP_Setup_Session()
{
    if (!xcpCanInitFlag)
        return false;

    //CTO_CONNECT(); //去掉第一次的CONNECT
    CTO_DISCONNECT();

    if (!CTO_CONNECT())
    {
        return false;
    }
    else
    {
        XCPConnected = true;
    }
    CTO_GET_COMM_MODE_INFO();

    if(!CTO_GET_STATUS())
    {
        return false;
    }

    if(useSeedKey)
    {
        //unlock DAQ
        if(!XCP_UnlockEcuResource(0x04))
        {
            return false;
        }
        qDebug()<<"Unlock DAQ Resource OK!";

        //unlock CAL/PAG
        if(!XCP_UnlockEcuResource(0x01))
        {
            return false;
        }
        qDebug()<<"Unlock CAL/PAG Resource OK!";
    }

    CTO_GET_PAG_PROCESSOR_INFO();

    /*
    if(MAX_SEGMENT == 0)
        MAX_SEGMENT = 1;

    for (int i = 0; i < MAX_SEGMENT + 1; i++)
    {
        for(int j = 0; j < 3; j++)
        {
            curSegment = i;
            curPage = j;
            CTO_GET_CAL_PAGE();

            if(!CTO_SET_CAL_PAGE())
                return false;
        }
    }
    */
    //curSegment = 0x00;
    //curPage = 0x01;
    //if(!CTO_SET_CAL_PAGE())
    //    return false;

    XCP_SetMemSeg();


    if(!CTO_GET_STATUS())
    {
        return false;
    }

    return true;
}

void XCPMaster::XCP_SetMemSeg()
{
    if(xcpMemSegList.isEmpty()) return;

    for(int i = 0; i < xcpMemSegList.count(); i++)
    {
        A2L_MemorySegment *memSeg = xcpMemSegList.at(i);

        if(!memSeg) continue;

        QList<A2L_Page*> pageList = memSeg->a2lPageList;

        for(int j = 0; j < memSeg->pageNum; j++)
        {
            A2L_Page *page = pageList.at(j);
            if(page)
            {
                if(page->xcpWriteAccessType == "XCP_WRITE_ACCESS_WITH_ECU_ONLY")
                {
                    if(XCP_SET_CAL_PAGE(memSeg->segIndex, page->pageIndex))
                    {
                        //qInfo()<<"SET CAL PAGE OK: SEGMENT[" + (memSeg->segIndex) + "]-PAGE[" + (page->pageIndex) + "]";
                    }
                }
            }
        }
    }
}

bool XCPMaster::XCP_SET_CAL_PAGE(quint8 memSeg, quint8 page)
{
    curSegment = memSeg;
    curPage = page;
    if(!CTO_SET_CAL_PAGE())
        return false;
    return true;
}

bool XCPMaster::XCP_UnlockEcuResource(quint8 resource)
{
    if (!xcpCanInitFlag) return false;
    if (!XCPConnected) return false;

    if(!useSeedKey || seedKeyDllPath.isEmpty())
    {
        qInfo()<<"未设置加锁或未配置SeedKey文件";
        return false;
    }

    for(int i = 0; i < 16; i++)
    {
        data_SEED[i] = 0x00;
        data_KEY[i] = 0x00;
    }

    SEED_MODE = 0x00;
    SEED_RESOURCE = resource;
    if(!CTO_GET_SEED())
        return false;


    LEN_KEY = 255;
    //seed 2 key
    qint16 status = XCP_Seed2Key(seedKeyDllPath, SEED_RESOURCE, LEN_SEED, data_SEED, &LEN_KEY, data_KEY);
    qDebug()<<"xcp seed2key status:"<<status;

    qDebug()<<"After calculation, Len of key="<<LEN_KEY;
    qDebug()<<"Key="
            <<QString::number(data_KEY[0], 16)<<"-"
            <<QString::number(data_KEY[1], 16)<<"-"
            <<QString::number(data_KEY[2], 16)<<"-"
            <<QString::number(data_KEY[3], 16)<<"-"
            <<QString::number(data_KEY[4], 16)<<"-"
            <<QString::number(data_KEY[5], 16)<<"-";

    if(!CTO_UNLOCK())
        return false;

    return true;
}

bool XCPMaster::XCP_GetInfos()
{
    if (!XCPConnected)
        return false;
    if (!CTO_GET_ID())
        return false;
    numOfDataAG_UPLOAD = length_GET_ID;
    if(CTO_UPLOAD())
        return true;
    else
        return false;
}

bool XCPMaster::XCP_Cal()
{
    if (!xcpCanInitFlag)
        return false;

    if (!CTO_SET_MTA())
        return false;

    if (CTO_DOWNLOAD())
        return true;
    else
        return false;
}

bool XCPMaster::XCP_Poll()
{
    if (!xcpCanInitFlag)
        return false;

    if (!CTO_SET_MTA())
        return false;
    if (CTO_UPLOAD())
        return true;
    else
        return false;
}

bool XCPMaster::XCP_DAQ_Demo()
{
    if (!xcpCanInitFlag)
        return false;
    if (!XCPConnected)
        return false;

    arrangeMeasVars();
    varsDAQConfigured = true;


    if (!XCP_DAQ_ALLOC_DYN())
        return false;
    if (!XCP_DAQ_LIST_CONFIG())
        return false;

    if (!XCP_SET_DAQ_LIST_MODE_FORALL())
        return false;
    if (!CTO_GET_STATUS())
        return false;
    qDebug()<<"DAQ_Running from GET_STATUS :"<<DAQ_Running_GETSTATUS;

    return true;
}

bool XCPMaster::XCP_DAQ_CONFIG()
{

    if (!xcpCanInitFlag)
        return false;
    if (!XCPConnected)
        return false;


    if (xcpDaq == NULL)
        return false;

    if (xcpDaq->DaqType == "STATIC")
    {
        qDebug()<<"Static DAQ Prepare";
        arrangeMeasVars_Static();

        if (!XCP_PREPARE_DAQ_LIST_STATIC())
            return false;
        if (!XCP_CONFIG_DAQ_LIST_STATIC())
            return false;

    }
    else if (xcpDaq->DaqType == "DYNAMIC")
    {
        qDebug()<<"Dynamic DAQ Prepare";
        if(!isCanFd)
        {
            arrangeMeasVars();
        }
        else
        {
            arrangeMeasVars_dlc64();
        }


        varsDAQConfigured = true;

        if (!XCP_PREPARE_DAQ_LIST_DYN())
            return false;
        if (!XCP_CONFIG_DAQ_LIST_DYN())
            return false;
    }
    else
    {
        return false;
    }


    if (!XCP_SET_DAQ_LIST_MODE_FORALL())
        return false;
    if (!CTO_GET_STATUS())
        return false;

    qDebug()<<"DAQ_Running from GET_STATUS :"<<DAQ_Running_GETSTATUS;


    return true;
}

bool XCPMaster::XCP_DAQ_Start_Stop(bool runFlag)
{
    if (!xcpCanInitFlag)
        return false;

    DAQRunning = runFlag ? false : true;
    if (!DAQRunning)
    {
        if (!XCP_STARTSTOP_DAQ_SELECT(true))
        {
            qCritical()<<"DAQ Start Selected DAQList Fail.";
            return false;
        }
        else
        {
            qDebug()<<"DAQ Start Selected DAQList Successfully!";
            qDebug()<<"DAQ_Running from GET_STATUS :"<<DAQ_Running_GETSTATUS;
            qDebug()<<"============================";
            if (DAQ_Running_GETSTATUS)
                DAQRunning = true;

            if(xcpDeviceType == 0)
            {
                connect(xcpCanThread->thread_RES, &XCP_R_Thread::ODTDataReady, this, &XCPMaster::getDAQODTData);
                connect(this, &XCPMaster::ODTDataUpdated, this, &XCPMaster::ODTDataUpdatedSlot);
                if(!idList.isEmpty())
                {
                    connect(xcpCanThread->thread_EVENT, &XCP_R_Event_Thread::ODTDataReady, this, &XCPMaster::getDAQODTData);
                }
            }
            else if(xcpDeviceType == 1)
            {
                bool connOK = connect(xcpTsCanThread->thread_RES, &XCP_R_Thread_TS::ODTDataReady_TS, this, &XCPMaster::getDAQODTData_TS);
                connect(this, &XCPMaster::ODTDataUpdated, this, &XCPMaster::ODTDataUpdatedSlot);
                qDebug()<<"ODT connect for TS:"<<connOK;
                if(!idList.isEmpty())
                {
                    //connect(xcpTsCanThread->thread_EVENT, SIGNAL(ODTDataReady_TS(quint8*, quint32)), this, SLOT(getDAQODTData_TS(quint8*, quint32)));
                }
            }
            else if(xcpDeviceType == 2)
            {
                connect(xcpZlgCanThread->thread_RES, &XCP_R_Thread_ZLG::ODTDataReady_ZLG, this, &XCPMaster::getDAQODTData_ZLG);
                connect(this, &XCPMaster::ODTDataUpdated, this, &XCPMaster::ODTDataUpdatedSlot);
                if(!idList.isEmpty())
                {

                }
            }

            /*
            connect(xcpCanThread->thread_RES, SIGNAL(ODTDataReady(ByteArrayPtr, quint32)), this, SLOT(getDAQODTData(ByteArrayPtr, quint32)));
            connect(this, SIGNAL(ODTDataUpdated(quint16)), this, SLOT(ODTDataUpdatedSlot(quint16)));
            if(!idList.isEmpty())
            {
                connect(xcpCanThread->thread_EVENT, SIGNAL(ODTDataReady(ByteArrayPtr, quint32)), this, SLOT(getDAQODTData(ByteArrayPtr, quint32)));
            }
            */
        }
    }
    else
    {
        if (!XCP_STARTSTOP_DAQ_SELECT(false))
        {
            qCritical()<<"DAQ Stop Selected DAQList Fail.";
            return false;
        }
        else
        {
            qDebug()<<"DAQ Stop Selected DAQList Successfully!";
            qDebug()<<"DAQ_Running from GET_STATUS :"<<DAQ_Running_GETSTATUS;
            qDebug()<<"============================";
            if (!DAQ_Running_GETSTATUS)
                DAQRunning = false;
            if(xcpDeviceType == 0)
            {
                disconnect(xcpCanThread->thread_RES, &XCP_R_Thread::ODTDataReady, this, &XCPMaster::getDAQODTData);
                disconnect(this, &XCPMaster::ODTDataUpdated, this, &XCPMaster::ODTDataUpdatedSlot);

                if(!idList.isEmpty())
                {
                    disconnect(xcpCanThread->thread_EVENT, &XCP_R_Event_Thread::ODTDataReady, this, &XCPMaster::getDAQODTData);
                }
            }
            else if(xcpDeviceType == 1)
            {
                disconnect(xcpTsCanThread->thread_RES, &XCP_R_Thread_TS::ODTDataReady_TS, this, &XCPMaster::getDAQODTData_TS);
                disconnect(this, &XCPMaster::ODTDataUpdated, this, &XCPMaster::ODTDataUpdatedSlot);

                if(!idList.isEmpty())
                {

                }
            }
            /*
            disconnect(xcpCanThread->thread_RES, SIGNAL(ODTDataReady(ByteArrayPtr, quint32)), this, SLOT(getDAQODTData(ByteArrayPtr, quint32)));
            disconnect(this, SIGNAL(ODTDataUpdated(quint16)), this, SLOT(ODTDataUpdatedSlot(quint16)));

            if(!idList.isEmpty())
            {
                disconnect(xcpCanThread->thread_EVENT, SIGNAL(ODTDataReady(ByteArrayPtr, quint32)), this, SLOT(getDAQODTData(ByteArrayPtr, quint32)));
            }
            */
        }
        DAQRunning = false;
    }

    return true;
}

bool XCPMaster::XCP_Poll_Meas(A2L_VarMeas *measVar)
{
    if(measVar == NULL)
        return false;

    int size = measVar->DataSizeAG;

    setAddressMTA(measVar->ECUAddress, 0);
    setnumOfDataUPLOAD(size);

    if(!XCP_Poll())
    {
        //qDebug()<<measVar->Name<<" poll error.";
        return false;
    }
    //char data[size];
    char *data = new char[size];

    memcpy(data, (char*)data_UPLOAD, size);

    if(ByteOrder_XCPAddr == 1)
        reverse(data, size);

    return copyPollRawDataToSM(data, size, measVar);

}

bool XCPMaster::copyPollRawDataToSM(char* data, int size, A2L_VarMeas *measVar)
{
    if(measVar == NULL)
        return false;
    if(measVar->smKey == "")
        return false;
    QSharedMemory sm;
    sm.setKey(measVar->smKey);

    if(!sm.isAttached())
    {
        if(!sm.attach())
        {
            qDebug()<<"Unable attach to read sharedmemory.";
            return false;
        }
    }

    sm.lock();
    memcpy((quint8*)sm.data(), CTO_RESTime, 8);
    memcpy((char*)sm.data()+8+measVar->StartByte, data, size);
    sm.unlock();

    delete[] data;

    return true;
}

bool XCPMaster::XCP_Cal_VALUE(A2L_VarChar *varChar)
{
    if (varChar == NULL)
        return false;

    curCharVar = varChar;
    //qDebug()<<"Current charVar:"<<curCharVar->Name;

    if (varChar->ECUAddress <= 0)
        return false;

    int dataSize = varChar->DataSizeAG;
    QString dataType = varChar->DataType;

    qreal value = 0;
    if (varChar->ConversionType == "RAT_FUNC")
    {
         value = RatFuncConvFromPhysToInt(varChar->getValue(), varChar->CoefList);
    }
    else if (varChar->ConversionType == "TAB_VERB")
    {
        value = varChar->getValue();
    }
    else
    {
        value = varChar->getValue();
    }

    quint8 *data = new quint8[8];
    switch (dataSize) {
    case 1:
    {
        if(dataType == "UBYTE")
        {
            quint8 valueInt = (quint8)value;
            memcpy(data, (quint8*)&valueInt, dataSize);
        }
        else if(dataType == "SBYTE")
        {
            qint8 valueInt = (qint8)value;
            memcpy(data, (quint8*)&valueInt, dataSize);
        }
    }
        break;
    case 2:
    {
        if(dataType == "UWORD")
        {
            quint16 valueInt = (quint16)value;
            memcpy(data, (quint8*)&valueInt, dataSize);
        }
        else if(dataType == "SWORD")
        {
            qint16 valueInt = (qint16)value;
            memcpy(data, (quint8*)&valueInt, dataSize);
        }
    }
        break;
    case 4:
    {
        if(dataType == "ULONG")
        {
            quint32 valueInt = (quint32)value;
            memcpy(data, (quint8*)&valueInt, dataSize);
        }
        else if(dataType == "SLONG")
        {
            qint32 valueInt = (qint32)value;
            memcpy(data, (quint8*)&valueInt, dataSize);
        }
        else if(dataType == "FLOAT32_IEEE")
        {
            float valueInt = (float)value;
            memcpy(data, (quint8*)&valueInt, dataSize);
        }
    }
        break;
    case 8:
    {
        if(dataType == "A_UINT64")
        {
            quint64 valueInt = (quint64)value;
            memcpy(data, (quint8*)&valueInt, dataSize);
        }
        else if(dataType == "A_UINT64")
        {
            qint64 valueInt = (qint64)value;
            memcpy(data, (quint8*)&valueInt, dataSize);
        }
        else if(dataType == "FLOAT64_IEEE")
        {
            qreal valueInt = (qreal)value;
            memcpy(data, (quint8*)&valueInt, dataSize);
        }
    }
        break;
    default:
        break;
    }


    //qDebug()<<"char variable value="<<value;
    //qDebug()<<"data ptr="<<data<<", dataSize="<<varChar->DataSizeAG<<",dataType"<<varChar->DataType;

    setAddressMTA(varChar->ECUAddress, 0);
    setDataDOWNLOAD(data, varChar->DataSizeAG);

    bool calStatus = false;
    calStatus = XCP_Cal();
    if(!calStatus)
    {
        qCritical()<<varChar->Name<<"In XCP_Cal Calirate error.";
    }

    delete[] data;

    return calStatus;
}

bool XCPMaster::XCP_Cal_VALUE(A2L_VarChar *varChar, char *data, int size)
{
    if (varChar == NULL)
        return false;

    if (varChar->ECUAddress <= 0)
        return false;

    setAddressMTA(varChar->ECUAddress, 0);
    setDataDOWNLOAD((quint8*)data, varChar->DataSizeAG);

    bool calStatus = false;
    calStatus = XCP_Cal();
    if(!calStatus)
    {
        qCritical()<<varChar->Name<<"In XCP_Cal Calibrate error";
    }
    else
    {
        //qDebug()<<varChar->Name<<" In XCP_Cal Calibrate ok!";
        updateCalTimeInSM(varChar);
    }

    return calStatus;
}

bool XCPMaster::XCP_Cal_VALUE(A2L_VarChar *varChar, char *data, int size, quint32 offset)
{
    if (varChar == NULL)
        return false;

    if (varChar->ECUAddress <= 0)
        return false;


    setAddressMTA(varChar->ECUAddress + offset, 0);
    setDataDOWNLOAD((quint8*)data, varChar->DataSizeAG);

    bool calStatus = false;
    calStatus = XCP_Cal();
    if(!calStatus)
    {
        qCritical()<<varChar->Name<<"In XCP_Cal Calibrate error";
    }
    else
    {
        updateCalTimeInSM(varChar);
    }
    return calStatus;
}

bool XCPMaster::XCP_Cal_CURVE(A2L_VarChar *curveChar, int index, qreal phys)
{
    if (curveChar == NULL)
        return false;

    //curCharVar = curveChar;
    qDebug()<<"Current curveChar:"<<curveChar->Name;

    if (curveChar->ECUAddress <= 0)
        return false;

    qreal value = 0;
    if (curveChar->ConversionType == "RAT_FUNC")
    {
         value = RatFuncConvFromPhysToInt(phys, curveChar->CoefList);
    }
    else if (curveChar->ConversionType == "TAB_VERB")
    {
        value = phys;
    }
    else
    {
        value = phys;
    }


    quint8 *data = NULL;
    if (curveChar->DataSizeAG == 1 && curveChar->DataType == "UBYTE")
    {
        quint8 valueInt = (quint8)value;
        data = (quint8*)&valueInt;
        //qDebug()<<"active 1";
    }
    else if (curveChar->DataSizeAG == 4 && curveChar->DataType == "FLOAT32_IEEE")
    {
        float valueInt = (float)value;
        data = (quint8*)&valueInt;
    }
    else if (curveChar->DataSizeAG == 4 && curveChar->DataType == "ULONG")
    {
        quint32 valueInt = (quint32)value;
        data = (quint8*)&valueInt;
    }

    if(data == NULL)
        return false;

    qDebug()<<"char variable value="<<value;
    qDebug()<<"data ptr="<<data<<", dataSize="<<curveChar->DataSizeAG<<",dataType"<<curveChar->DataType;

    quint32 mtaAddress = curveChar->ECUAddress + curveChar->DataSizeAG * index;
    setAddressMTA(mtaAddress, 0);
    setDataDOWNLOAD(data, curveChar->DataSizeAG);

    qDebug()<<"char variable value="<<value;
    qDebug()<<"data ptr="<<data<<", dataSize="<<curveChar->DataSizeAG<<",dataType"<<curveChar->DataType;
    qDebug()<<"MTA Address:"<<QString::number(mtaAddress, 16);

    bool calStatus = false;
    calStatus = XCP_Cal();

    return calStatus;
}

bool XCPMaster::updateCalTimeInSM(A2L_VarChar *varChar)
{
    if(!varChar)
        return false;

    QSharedMemory sm;
    sm.setKey(varChar->smKey);

    if(!sm.isAttached())
    {
        if(!sm.attach())
        {
            //qDebug()<<"Unable attach to read sharedmemory.";
            return false;
        }
    }
    sm.lock();
    memcpy((quint8*)sm.data(), CTO_RESTime, 8);
    sm.unlock();

    return true;
}

bool XCPMaster::XCP_Cal_VAL_BLK(A2L_VarChar *varChar)
{
    if(varChar == NULL)
        return false;
    if(varChar->charType != "VAL_BLK" || varChar->ECUAddress <= 0)
        return false;

    int dataSize = varChar->DataSizeAG;
    QString dataType = varChar->DataType;

    for(int i = 0; i < varChar->number_BLK; i++)
    {
        qreal value = 0;
        if (varChar->ConversionType == "RAT_FUNC")
        {
             value = RatFuncConvFromPhysToInt(varChar->Blk_Values.at(i), varChar->CoefList);
        }
        else if (varChar->ConversionType == "TAB_VERB")
        {
            value = varChar->Blk_Values.at(i);
        }
        else
        {
            value = varChar->Blk_Values.at(i);
        }

        quint8 *data = new quint8[8];
        switch (dataSize) {
        case 1:
        {
            if(dataType == "UBYTE")
            {
                quint8 valueInt = (quint8)value;
                memcpy(data, (quint8*)&valueInt, dataSize);
            }
            else if(dataType == "SBYTE")
            {
                qint8 valueInt = (qint8)value;
                memcpy(data, (quint8*)&valueInt, dataSize);
            }
        }
            break;
        case 2:
        {
            if(dataType == "UWORD")
            {
                quint16 valueInt = (quint16)value;
                memcpy(data, (quint8*)&valueInt, dataSize);
            }
            else if(dataType == "SWORD")
            {
                qint16 valueInt = (qint16)value;
                memcpy(data, (quint8*)&valueInt, dataSize);
            }
        }
            break;
        case 4:
        {
            if(dataType == "ULONG")
            {
                quint32 valueInt = (quint32)value;
                memcpy(data, (quint8*)&valueInt, dataSize);
            }
            else if(dataType == "SLONG")
            {
                qint32 valueInt = (qint32)value;
                memcpy(data, (quint8*)&valueInt, dataSize);
            }
            else if(dataType == "FLOAT32_IEEE")
            {
                float valueInt = (float)value;
                memcpy(data, (quint8*)&valueInt, dataSize);
            }
        }
            break;
        case 8:
        {
            if(dataType == "A_UINT64")
            {
                quint64 valueInt = (quint64)value;
                memcpy(data, (quint8*)&valueInt, dataSize);
            }
            else if(dataType == "A_UINT64")
            {
                qint64 valueInt = (qint64)value;
                memcpy(data, (quint8*)&valueInt, dataSize);
            }
            else if(dataType == "FLOAT64_IEEE")
            {
                qreal valueInt = (qreal)value;
                memcpy(data, (quint8*)&valueInt, dataSize);
            }
        }
            break;
        default:
            break;
        }

        quint32 mtaAddress = varChar->ECUAddress + varChar->DataSizeAG * i;
        setAddressMTA(mtaAddress, 0);
        setDataDOWNLOAD(data, varChar->DataSizeAG);
        //qDebug()<<"MTA Address:"<<QString::number(mtaAddress, 16);

        bool calStatus = false;
        calStatus = XCP_Cal();
        if(!calStatus)
        {
            qDebug()<<varChar->Name<<" In XCP_Cal Calibrate error. blk index:"<<i;
            return false;
        }

        delete[] data;
    }

    return true;
}

bool XCPMaster::XCP_Cal_CURVE(A2L_VarChar *varChar)
{
    if(varChar == NULL)
        return false;
    if(varChar->charType != "CURVE" || varChar->ECUAddress <= 0)
        return false;

    int dataSize = varChar->DataSizeAG;
    QString dataType = varChar->DataType;

    for(int i = 0; i < varChar->zCount; i++)
    {
        qreal value = 0;
        if (varChar->ConversionType == "RAT_FUNC")
        {
             value = RatFuncConvFromPhysToInt(varChar->hexValue_ZList.at(i), varChar->CoefList);
        }
        else if (varChar->ConversionType == "TAB_VERB")
        {
            value = varChar->hexValue_ZList.at(i);
        }
        else
        {
            value = varChar->hexValue_ZList.at(i);
        }

        quint8 *data = new quint8[8];
        switch (dataSize) {
        case 1:
        {
            if(dataType == "UBYTE")
            {
                quint8 valueInt = (quint8)value;
                memcpy(data, (quint8*)&valueInt, dataSize);
            }
            else if(dataType == "SBYTE")
            {
                qint8 valueInt = (qint8)value;
                memcpy(data, (quint8*)&valueInt, dataSize);
            }
        }
            break;
        case 2:
        {
            if(dataType == "UWORD")
            {
                quint16 valueInt = (quint16)value;
                memcpy(data, (quint8*)&valueInt, dataSize);
            }
            else if(dataType == "SWORD")
            {
                qint16 valueInt = (qint16)value;
                memcpy(data, (quint8*)&valueInt, dataSize);
            }
        }
            break;
        case 4:
        {
            if(dataType == "ULONG")
            {
                quint32 valueInt = (quint32)value;
                memcpy(data, (quint8*)&valueInt, dataSize);
            }
            else if(dataType == "SLONG")
            {
                qint32 valueInt = (qint32)value;
                memcpy(data, (quint8*)&valueInt, dataSize);
            }
            else if(dataType == "FLOAT32_IEEE")
            {
                float valueInt = (float)value;
                memcpy(data, (quint8*)&valueInt, dataSize);
            }
        }
            break;
        case 8:
        {
            if(dataType == "A_UINT64")
            {
                quint64 valueInt = (quint64)value;
                memcpy(data, (quint8*)&valueInt, dataSize);
            }
            else if(dataType == "A_UINT64")
            {
                qint64 valueInt = (qint64)value;
                memcpy(data, (quint8*)&valueInt, dataSize);
            }
            else if(dataType == "FLOAT64_IEEE")
            {
                qreal valueInt = (qreal)value;
                memcpy(data, (quint8*)&valueInt, dataSize);
            }
        }
            break;
        default:
            break;
        }

        quint32 mtaAddress = varChar->ECUAddress + varChar->DataSizeAG * i;
        setAddressMTA(mtaAddress, 0);
        setDataDOWNLOAD(data, varChar->DataSizeAG);
        //qDebug()<<"Curve zValue MTA Address:"<<QString::number(mtaAddress, 16);

        bool calStatus = false;
        calStatus = XCP_Cal();
        if(!calStatus)
        {
            qDebug()<<varChar->Name<<" In XCP_Cal Calibrate error. curve z index:"<<i;
            return false;
        }

        delete[] data;
    }

    int xDataSize = varChar->Axis_X_DataSizeAG;
    QString xDataType = varChar->Axis_X_DataType;
    for(int i = 0; i < varChar->xCount; i++)
    {
        qreal value = 0;
        if (varChar->Axis_X_ConversionType == "RAT_FUNC")
        {
             value = RatFuncConvFromPhysToInt(varChar->hexValue_XList.at(i), varChar->CoefList);
        }
        else if (varChar->Axis_X_ConversionType == "TAB_VERB")
        {
            value = varChar->hexValue_XList.at(i);
        }
        else
        {
            value = varChar->hexValue_XList.at(i);
        }

        quint8 *data = new quint8[8];
        switch (xDataSize) {
        case 1:
        {
            if(xDataType == "UBYTE")
            {
                quint8 valueInt = (quint8)value;
                memcpy(data, (quint8*)&valueInt, dataSize);
            }
            else if(xDataType == "SBYTE")
            {
                qint8 valueInt = (qint8)value;
                memcpy(data, (quint8*)&valueInt, dataSize);
            }
        }
            break;
        case 2:
        {
            if(xDataType == "UWORD")
            {
                quint16 valueInt = (quint16)value;
                memcpy(data, (quint8*)&valueInt, dataSize);
            }
            else if(xDataType == "SWORD")
            {
                qint16 valueInt = (qint16)value;
                memcpy(data, (quint8*)&valueInt, dataSize);
            }
        }
            break;
        case 4:
        {
            if(xDataType == "ULONG")
            {
                quint32 valueInt = (quint32)value;
                memcpy(data, (quint8*)&valueInt, dataSize);
            }
            else if(xDataType == "SLONG")
            {
                qint32 valueInt = (qint32)value;
                memcpy(data, (quint8*)&valueInt, dataSize);
            }
            else if(xDataType == "FLOAT32_IEEE")
            {
                float valueInt = (float)value;
                memcpy(data, (quint8*)&valueInt, dataSize);
            }
        }
            break;
        case 8:
        {
            if(xDataType == "A_UINT64")
            {
                quint64 valueInt = (quint64)value;
                memcpy(data, (quint8*)&valueInt, dataSize);
            }
            else if(xDataType == "A_UINT64")
            {
                qint64 valueInt = (qint64)value;
                memcpy(data, (quint8*)&valueInt, dataSize);
            }
            else if(xDataType == "FLOAT64_IEEE")
            {
                qreal valueInt = (qreal)value;
                memcpy(data, (quint8*)&valueInt, dataSize);
            }
        }
            break;
        default:
            break;
        }

        quint32 mtaAddress = varChar->Axis_X_ECUAddress + xDataSize * i;
        setAddressMTA(mtaAddress, 0);
        setDataDOWNLOAD(data, xDataSize);
        //qDebug()<<"Curve xAxis MTA Address:"<<QString::number(mtaAddress, 16);

        bool calStatus = false;
        calStatus = XCP_Cal();
        if(!calStatus)
        {
            qDebug()<<varChar->Name<<" In XCP_Cal Calirate error. curve x index:"<<i;
            return false;
        }

        delete[] data;
    }

}


void XCPMaster::setXcpDaq(A2L_Daq *daq)
{
    xcpDaq = daq;
    qDebug()<<"xcp daq:"<<xcpDaq->Name;
}

void XCPMaster::setXcpDaqList(QList<A2L_Daq_List *> daqList)
{
    xcpDaqList = daqList;
    qDebug()<<"xcp daq List count:"<<xcpDaqList.count();
}

void XCPMaster::setXcpEventList(QList<A2L_Event *> eventList)
{
    xcpEventList = eventList;
    qDebug()<<"xcp event List count:"<<xcpEventList.count();
}

void XCPMaster::setXcpEventCanIdList(const QList<A2L_EventCanId *> &value)
{
    xcpEventCanIdList = value;

    idList.clear();
    foreach (A2L_EventCanId *eventCanId, xcpEventCanIdList) {
        if(eventCanId)
        {
            if(eventCanId->Event_CAN_ID > 0)
                idList.append(eventCanId->Event_CAN_ID);
        }
    }

    if(xcpCanThread)
    {
        xcpCanThread->thread_EVENT->setEventIdList(idList);
    }
}

void XCPMaster::initVars()
{
    varInfo *var1 = new varInfo;
    var1->varName = "VACU_EOPMotSpdAct_rpm";
    var1->varAddress = 0x60002830;
    var1->varSizeAG = 4;
    var1->rate_ms = 10;

    varInfo *var2 = new varInfo;
    var2->varName = "VINP_EDUCooltTemp_C";
    var2->varAddress = 0x70007FB0;
    var2->varSizeAG = 4;
    var2->rate_ms = 10;

    varInfo *var3 = new varInfo;
    var3->varName = "VOMC_EOPMotVoltTgt_V";
    var3->varAddress = 0x600027C8;
    var3->varSizeAG = 4;
    var3->rate_ms = 10;

    varInfo *var4  = new varInfo;
    var4->varName = "VINP_EDUOilTemp_C";
    var4->varAddress = 0x70007FA8;
    var4->varSizeAG = 4;
    var4->rate_ms = 10;

    varInfo *var5 = new varInfo;
    var5->varName = "VINP_LVBatVolt_V";
    var5->varAddress = 0x70007F70;
    var5->varSizeAG = 4;
    var5->rate_ms = 10;

}

//arrange DAQ measurement variables
void XCPMaster::arrangeMeasVars()
{

    xcpDaqConfigPamReset();

    // ???????????????????-?????б?Hash??
    for (int i = 0; i < daqMeasVarList.count(); i++)
    {
        A2L_VarMeas *var = daqMeasVarList.at(i);
        quint16 rate = var->rate_ms;                                      // ???DAQ ???? ????????
        sizeDataBuffer_DAQ += var->DataSizeAG;                  // ????DAQ ???? ???С

        //varValueHash.insert(var, 0.0);                                       // ?????????-? Hash

        // ???????????????????-?????б? Hash
        if (!rateVarHash.keys().contains(rate))
        {
            QList<A2L_VarMeas*> varList;
            varList.append(var);
            rateVarHash.insert(rate,varList);
        }
        else
        {
            QList<A2L_VarMeas*> varList = rateVarHash.value(rate);
            varList.append(var);
            rateVarHash.insert(rate, varList);
        }
    }

    //DAQ ???????
    //DataBuffer_DAQ = new quint8[sizeDataBuffer_DAQ];

    quint8 pid = 0;
    quint32 offset = 0;

    QList<float> keys = rateVarHash.keys();
    std::sort(keys.begin(), keys.end());

    if (keys.count() == 0)
        return;

    //对参数配置后DAQ按照EVENT的序号来排序，因为有可能出现周期大的EVENT序号反而小
    //这种情况下不重新排序的话会导致EVENT不起作用，报FD 06(DAQ overload)

    for(int i = 0; i < keys.count() - 1; i++)
    {

        float rate = keys.at(i);
        float rate_next = keys.at(i+1);

        A2L_Event *event = getEventChannelByEventRate(rate);
        A2L_Event *event_next = getEventChannelByEventRate(rate_next);

        if(!event || !event_next) continue;

        if(event->ChannelNumber > event_next->ChannelNumber)
        {
            float temp = keys.at(i);
            keys[i] = keys[i+1];
            keys[i+1] = temp;
        }
    }


    DAQ_COUNT = keys.count();
    for (int i = 0; i < keys.count(); i++)
    {
        QList<A2L_VarMeas*> vars = rateVarHash.value(keys.at(i));
        quint8 odtIndex = 0;
        quint8 odtEntryIndex = 0;
        quint8 dataIndex = 0;
        quint8 dataVolumeAG = 7;
        quint32 varIndex = 0;

        quint16 curDAQList = i;
        daqListVarHash.insert(curDAQList, vars);

        XCP_DAQ_ALLOC alloc;
        alloc.DAQ_LIST_NUMBER = curDAQList;
        QList<quint8> odtEntryCountList;

        quint32 dataSize = 0;

        for (int j = 0; j < vars.count(); j++)
        {
            A2L_VarMeas *var = vars.at(j);

            bool lastVar = (vars.count() - 1) == j ? true : false;

            dataSize += var->DataSizeAG;

            //var_Attr* varAttr = new var_Attr;
            var_Attr varAttr;
            varAttr.DAQ_List = curDAQList;
            varAttr.var_Offset = varIndex;
            varAttrHash.insert(var, varAttr);
            varIndex += var->DataSizeAG;

            XCP_DAQ_PTR ptr1, ptr2, ptr3;

            //ptr1 = new XCP_DAQ_PTR;
            ptr1.DAQ_LIST_INDEX = MIN_DAQ + i;
            ptr1.DAQ_ODT_INDEX = odtIndex;
            ptr1.DAQ_ODT_ENTRY_INDEX = odtEntryIndex;
            ptr1.PTR_Address = var->ECUAddress;
            ptr1.PTR_var = var;
            ptr1.DAQ_PID = pid;
            ptr1.offsetDataBuffer = offset;

            odtEntryIndex += 1;

            if ((dataIndex + var->DataSizeAG) < dataVolumeAG)
            {
                ptr1.PTR_Size = var->DataSizeAG;
                daqPTRList.append(ptr1);

                dataIndex += var->DataSizeAG;

            }
            else if((dataIndex + var->DataSizeAG) == dataVolumeAG)
            {
                ptr1.PTR_Size = var->DataSizeAG;
                daqPTRList.append(ptr1);

                DataBuffer buffer1;
                buffer1.data = new quint8[7];
                buffer1.numData = 7;
                pidHash.insert(pid, buffer1);

                PID_Attr pidAttr1;
                pidAttr1.DAQ_List = curDAQList;
                pidAttr1.ODT_Offset = odtIndex*7;
                pidAttr1.biggestPID = lastVar ? true : false;
                pidAttrHash.insert(pid, pidAttr1);

                odtIndex += lastVar ? 0 : 1;
                pid += lastVar ? 0 : 1;
                dataIndex = 0;
                odtEntryCountList.append(odtEntryIndex);
                odtEntryIndex = 0;

            }
            else if ((dataIndex + var->DataSizeAG) < (dataVolumeAG*2) && (dataIndex + var->DataSizeAG) > dataVolumeAG)
            {
                ptr1.PTR_Size = (dataVolumeAG - dataIndex);
                daqPTRList.append(ptr1);

                //DataBuffer *buffer1 = new DataBuffer;
                DataBuffer buffer1;
                buffer1.data = new quint8[7];
                buffer1.numData = 7;
                pidHash.insert(pid, buffer1);


                //PID_Attr* pidAttr1 = new PID_Attr;
                PID_Attr pidAttr1;
                pidAttr1.DAQ_List = curDAQList;
                pidAttr1.ODT_Offset = odtIndex*7;
                pidAttr1.biggestPID = false;
                pidAttrHash.insert(pid, pidAttr1);

                odtIndex += 1;
                pid += 1;
                dataIndex = 0;
                odtEntryCountList.append(odtEntryIndex);
                odtEntryIndex = 0;

                //ptr2 = new XCP_DAQ_PTR;
                ptr2.DAQ_LIST_INDEX = MIN_DAQ + i;
                ptr2.DAQ_ODT_INDEX = odtIndex;
                ptr2.DAQ_ODT_ENTRY_INDEX = odtEntryIndex;
                ptr2.PTR_Address = var->ECUAddress + ptr1.PTR_Size;
                ptr2.PTR_Size = (var->DataSizeAG - ptr1.PTR_Size);
                ptr2.PTR_var = var;
                ptr2.DAQ_PID = pid;
                daqPTRList.append(ptr2);

                dataIndex += (var->DataSizeAG - ptr1.PTR_Size);
                odtEntryIndex += 1;

            }
            else if((dataIndex + var->DataSizeAG) == (dataVolumeAG*2))
            {
                ptr1.PTR_Size = (dataVolumeAG - dataIndex);
                daqPTRList.append(ptr1);

                DataBuffer buffer1;
                buffer1.data = new quint8[7];
                buffer1.numData = 7;
                pidHash.insert(pid, buffer1);

                PID_Attr pidAttr1;
                pidAttr1.DAQ_List = curDAQList;
                pidAttr1.ODT_Offset = odtIndex*7;
                pidAttr1.biggestPID = false;
                pidAttrHash.insert(pid, pidAttr1);

                odtIndex += 1;
                pid += 1;
                dataIndex = 0;
                odtEntryCountList.append(odtEntryIndex);
                odtEntryIndex = 0;

                ptr2.DAQ_LIST_INDEX = MIN_DAQ + i;
                ptr2.DAQ_ODT_INDEX = odtIndex;
                ptr2.DAQ_ODT_ENTRY_INDEX = odtEntryIndex;
                ptr2.PTR_Address = var->ECUAddress + ptr1.PTR_Size;
                ptr2.PTR_Size = (var->DataSizeAG - ptr1.PTR_Size);
                ptr2.PTR_var = var;
                ptr2.DAQ_PID = pid;
                daqPTRList.append(ptr2);

                dataIndex += (var->DataSizeAG - ptr1.PTR_Size);
                odtEntryIndex += 2;
            }
            else if ((dataIndex + var->DataSizeAG) < (dataVolumeAG*3)  && (dataIndex + var->DataSizeAG) > (dataVolumeAG*2))
            {
                ptr1.PTR_Size = (dataVolumeAG - dataIndex);
                daqPTRList.append(ptr1);

                //DataBuffer *buffer1 = new DataBuffer;
                DataBuffer buffer1;
                buffer1.data = new quint8[7];
                buffer1.numData = 7;
                pidHash.insert(pid, buffer1);

                //PID_Attr* pidAttr1 = new PID_Attr;
                PID_Attr pidAttr1;
                pidAttr1.DAQ_List = curDAQList;
                pidAttr1.ODT_Offset = odtIndex*7;
                pidAttr1.biggestPID = false;
                pidAttrHash.insert(pid, pidAttr1);

                odtIndex +=1;
                pid += 1;
                dataIndex = 0;
                odtEntryCountList.append(odtEntryIndex);
                odtEntryIndex = 0;

                //ptr2 = new XCP_DAQ_PTR;
                ptr2.DAQ_LIST_INDEX = MIN_DAQ + i;
                ptr2.DAQ_ODT_INDEX = odtIndex;
                ptr2.DAQ_ODT_ENTRY_INDEX = odtEntryIndex;
                ptr2.PTR_Address = var->ECUAddress + ptr1.PTR_Size;
                ptr2.PTR_Size = dataVolumeAG;
                ptr2.PTR_var = var;
                ptr2.DAQ_PID = pid;
                daqPTRList.append(ptr2);

                //DataBuffer *buffer2 = new DataBuffer;
                DataBuffer buffer2;
                buffer2.data = new quint8[7];
                buffer2.numData = 7;
                pidHash.insert(pid, buffer2);

                //PID_Attr* pidAttr2 = new PID_Attr;
                PID_Attr pidAttr2;
                pidAttr2.DAQ_List = curDAQList;
                pidAttr2.ODT_Offset = odtIndex*7;
                pidAttr2.biggestPID = false;
                pidAttrHash.insert(pid, pidAttr2);

                odtIndex += 1;
                pid += 1;
                dataIndex = 0;
                odtEntryCountList.append(odtEntryIndex);
                odtEntryIndex = 0;

                //ptr3 = new XCP_DAQ_PTR;
                ptr3.DAQ_LIST_INDEX = MIN_DAQ + i;
                ptr3.DAQ_ODT_INDEX = odtIndex;
                ptr3.DAQ_ODT_ENTRY_INDEX = odtEntryIndex;
                ptr3.PTR_Address = var->ECUAddress + ptr1.PTR_Size + dataVolumeAG;
                ptr3.PTR_Size = var->DataSizeAG - ptr1.PTR_Size - dataVolumeAG;
                ptr3.PTR_var = var;
                ptr3.DAQ_PID = pid;
                daqPTRList.append(ptr3);

                dataIndex += (var->DataSizeAG - ptr1.PTR_Size - dataVolumeAG);
                odtEntryIndex += 1;
            }

            if (lastVar && dataIndex != 0)
            {
                DataBuffer buffer;
                buffer.data = new quint8[dataIndex];
                buffer.numData = dataIndex;
                pidHash.insert(pid, buffer);

                PID_Attr pidAttr;
                pidAttr.DAQ_List = curDAQList;
                pidAttr.ODT_Offset = odtIndex*7;
                pidAttr.biggestPID = true;
                pidAttrHash.insert(pid, pidAttr);

                odtEntryCountList.append(odtEntryIndex);
                pid += 1;
            }

        }

        alloc.ODT_COUNT = odtIndex + 1;
        alloc.ODT_ENTRY_COUNT = odtEntryCountList;
        qDebug()<<"DAQ List="<<alloc.DAQ_LIST_NUMBER<<", ODT Count="<<alloc.ODT_COUNT<<", ODT Entry List Count="<<alloc.ODT_ENTRY_COUNT.count()<<", rate="<<keys.at(i);
        for (int i = 0; i < alloc.ODT_ENTRY_COUNT.count(); i++)
        {
            qDebug()<<"ODT Index "<<i<<", ODT Entry Count="<<alloc.ODT_ENTRY_COUNT.at(i);
        }
        daqAlloc.append(alloc);

        DataBuffer buffer;
        buffer.data = new quint8[dataSize];
        buffer.numData = dataSize;

        daqListDataHash.insert(curDAQList, buffer);

        QSharedMemory *smDAQ = new QSharedMemory("RP_DAQ_" + xcpName + "_" +QString::number(curDAQList));
        smDAQ->create(dataSize + 8);   //add time pam space in each daq data buffer
        daqListSMHash.insert(curDAQList, smDAQ);

        daqListBlockSizeHash.insert(curDAQList, dataSize + 8); // add 8 bytes for time pam used for record

    }

    QList<quint8> pidKeys = pidHash.keys();
    std::sort(pidKeys.begin(), pidKeys.end());

    for (int i = 0; i < pidKeys.count(); i++)
    {
        quint8 key = pidKeys.at(i);
        qDebug()<<"pid "<<key<<", dataBuffer Size"<<pidHash.value(key).numData;
    }

    for (int i = 0; i < pidKeys.count(); i++)
    {
        quint8 key = pidKeys.at(i);
        qDebug()<<"pid "<<key<<", pidAttr_DAQList="<<pidAttrHash.value(key).DAQ_List
               <<", pidAttr_Offset="<<pidAttrHash.value(key).ODT_Offset
              <<", pidAttr biggestPID ="<<pidAttrHash.value(key).biggestPID;
    }

    QList<A2L_VarMeas*> varKeys = varAttrHash.keys();
    for (int i = 0; i < daqMeasVarList.count(); i++)
    {
        A2L_VarMeas *var = daqMeasVarList.at(i);

        qDebug()<<"var "<<var->Name<<", var_DAQList="<<varAttrHash.value(var).DAQ_List
               <<", var_offset="<<varAttrHash.value(var).var_Offset;
    }

    for (int i = 0; i < daqPTRList.count(); i++)
    {
        XCP_DAQ_PTR ptr = daqPTRList.at(i);

        qDebug()<<"PTR "<<i<<"PID "<<ptr.DAQ_PID<<" DAQ List="<<ptr.DAQ_LIST_INDEX<<", ODT Index="<<ptr.DAQ_ODT_INDEX<<", ODT Entry Index="
               <<ptr.DAQ_ODT_ENTRY_INDEX<<", PTR_Address="<<QString::number(ptr.PTR_Address, 16)<<", PTR_Size="<<ptr.PTR_Size;
    }
    qDebug()<<"==================================";
}

void XCPMaster::arrangeMeasVars_Static()
{

    xcpDaqConfigPamReset();

    for (int i = 0; i < daqMeasVarList.count(); i++)
    {
        A2L_VarMeas *var = daqMeasVarList.at(i);
        float rate = var->rate_ms;
        sizeDataBuffer_DAQ += var->DataSizeAG;

        //varValueHash.insert(var, 0.0);

        if (!rateVarHash.keys().contains(rate))
        {
            QList<A2L_VarMeas*> varList;
            varList.append(var);
            rateVarHash.insert(rate,varList);
        }
        else
        {
            QList<A2L_VarMeas*> varList = rateVarHash.value(rate);
            varList.append(var);
            rateVarHash.insert(rate, varList);
        }
    }

    //DAQ ???????
    //DataBuffer_DAQ = new quint8[sizeDataBuffer_DAQ];

    //quint8 pid = 0;
    quint32 offset = 0;

    QList<float> keys = rateVarHash.keys();
    qSort(keys.begin(), keys.end());

    if (keys.count() == 0)
        return;

    if (keys.count() > (xcpDaq->Max_Daq - xcpDaq->Min_Daq))
        return;

    DAQ_COUNT = keys.count();

    for (int i = 0; i < keys.count(); i++)
    {
        QList<A2L_VarMeas*> vars = rateVarHash.value(keys.at(i));
        quint8 odtIndex = 0;
        quint8 odtEntryIndex = 0;
        quint8 dataIndex = 0;
        quint8 dataVolumeAG = 7;
        quint8 varIndex = 0;

        A2L_Daq_List *refDaqList = getDaqListByEventRate(keys.at(i));
        quint16 curDAQList = refDaqList->DaqListNumber;
        quint8 pid = refDaqList->First_Pid;
        dataVolumeAG = refDaqList->Max_Odt_Entries;

        daqListVarHash.insert(curDAQList, vars);

        //XCP_DAQ_ALLOC *alloc = new XCP_DAQ_ALLOC;
        XCP_DAQ_ALLOC alloc;
        alloc.DAQ_LIST_NUMBER = curDAQList;
        QList<quint8> odtEntryCountList;

        quint32 dataSize = 0;

        for (int j = 0; j < vars.count(); j++)
        {
            A2L_VarMeas *var = vars.at(j);

            dataSize += var->DataSizeAG;

            //var_Attr* varAttr = new var_Attr;
            var_Attr varAttr;
            varAttr.DAQ_List = curDAQList;
            varAttr.var_Offset = varIndex;
            varAttrHash.insert(var, varAttr);
            varIndex += var->DataSizeAG;

            XCP_DAQ_PTR ptr1, ptr2, ptr3;

            //ptr1 = new XCP_DAQ_PTR;

            ptr1.DAQ_LIST_INDEX = curDAQList;
            ptr1.DAQ_ODT_INDEX = odtIndex;
            ptr1.DAQ_ODT_ENTRY_INDEX = odtEntryIndex;
            ptr1.PTR_Address = var->ECUAddress;
            ptr1.PTR_var = var;
            ptr1.DAQ_PID = pid;
            ptr1.offsetDataBuffer = offset;

            odtEntryIndex += 1;

            if ((dataIndex + var->DataSizeAG) < dataVolumeAG)
            {
                ptr1.PTR_Size = var->DataSizeAG;
                daqPTRList.append(ptr1);

                dataIndex += var->DataSizeAG;
            }
            else if((dataIndex + var->DataSizeAG) == dataVolumeAG)
            {
                ptr1.PTR_Size = var->DataSizeAG;
                daqPTRList.append(ptr1);

                DataBuffer buffer1;
                buffer1.data = new quint8[7];
                buffer1.numData = 7;
                pidHash.insert(pid, buffer1);

                PID_Attr pidAttr1;
                pidAttr1.DAQ_List = curDAQList;
                pidAttr1.ODT_Offset = odtIndex*7;
                pidAttr1.biggestPID = false;
                pidAttrHash.insert(pid, pidAttr1);

                odtIndex += 1;
                pid += 1;
                dataIndex = 0;
                odtEntryCountList.append(odtEntryIndex);
                odtEntryIndex = 0;
            }
            else if ((dataIndex + var->DataSizeAG) < (dataVolumeAG*2))
            {
                ptr1.PTR_Size = (dataVolumeAG - dataIndex);
                daqPTRList.append(ptr1);

                //DataBuffer *buffer1 = new DataBuffer;
                DataBuffer buffer1;
                buffer1.data = new quint8[dataVolumeAG];
                buffer1.numData = dataVolumeAG;
                pidHash.insert(pid, buffer1);


                //PID_Attr* pidAttr1 = new PID_Attr;
                PID_Attr pidAttr1;
                pidAttr1.DAQ_List = curDAQList;
                pidAttr1.ODT_Offset = odtIndex*7;
                pidAttr1.biggestPID = false;
                pidAttrHash.insert(pid, pidAttr1);

                odtIndex +=1;
                pid += 1;
                dataIndex = 0;
                odtEntryCountList.append(odtEntryIndex);
                odtEntryIndex = 0;

                //ptr2 = new XCP_DAQ_PTR;
                ptr2.DAQ_LIST_INDEX = curDAQList;
                ptr2.DAQ_ODT_INDEX = odtIndex;
                ptr2.DAQ_ODT_ENTRY_INDEX = odtEntryIndex;
                ptr2.PTR_Address = var->ECUAddress + ptr1.PTR_Size;
                ptr2.PTR_Size = (var->DataSizeAG - ptr1.PTR_Size);
                ptr2.PTR_var = var;
                ptr2.DAQ_PID = pid;
                daqPTRList.append(ptr2);

                dataIndex += (var->DataSizeAG - ptr1.PTR_Size);
                odtEntryIndex += 1;

            }
            else if ((dataIndex + var->DataSizeAG) == (dataVolumeAG*2))
            {
                ptr1.PTR_Size = (dataVolumeAG - dataIndex);
                daqPTRList.append(ptr1);

                DataBuffer buffer1;
                buffer1.data = new quint8[7];
                buffer1.numData = 7;
                pidHash.insert(pid, buffer1);

                PID_Attr pidAttr1;
                pidAttr1.DAQ_List = curDAQList;
                pidAttr1.ODT_Offset = odtIndex*7;
                pidAttr1.biggestPID = false;
                pidAttrHash.insert(pid, pidAttr1);

                odtIndex += 1;
                pid += 1;
                dataIndex = 0;
                odtEntryCountList.append(odtEntryIndex);
                odtEntryIndex = 0;

                ptr2.DAQ_LIST_INDEX = MIN_DAQ + i;
                ptr2.DAQ_ODT_INDEX = odtIndex;
                ptr2.DAQ_ODT_ENTRY_INDEX = odtEntryIndex;
                ptr2.PTR_Address = var->ECUAddress + ptr1.PTR_Size;
                ptr2.PTR_Size = (var->DataSizeAG - ptr1.PTR_Size);
                ptr2.PTR_var = var;
                ptr2.DAQ_PID = pid;
                daqPTRList.append(ptr2);

                dataIndex += (var->DataSizeAG - ptr1.PTR_Size);
                odtEntryIndex += 2;
            }
            else if ((dataIndex + var->DataSizeAG) < (dataVolumeAG*3))
            {
                ptr1.PTR_Size = (dataVolumeAG - dataIndex);
                daqPTRList.append(ptr1);

                //DataBuffer *buffer1 = new DataBuffer;
                DataBuffer buffer1;
                buffer1.data = new quint8[dataVolumeAG];
                buffer1.numData = dataVolumeAG;
                pidHash.insert(pid, buffer1);

                //PID_Attr* pidAttr1 = new PID_Attr;
                PID_Attr pidAttr1;
                pidAttr1.DAQ_List = curDAQList;
                pidAttr1.ODT_Offset = odtIndex*7;
                pidAttr1.biggestPID = false;
                pidAttrHash.insert(pid, pidAttr1);

                odtIndex +=1;
                pid += 1;
                dataIndex = 0;
                odtEntryCountList.append(odtEntryIndex);
                odtEntryIndex = 0;

                //ptr2 = new XCP_DAQ_PTR;
                ptr2.DAQ_LIST_INDEX = curDAQList;
                ptr2.DAQ_ODT_INDEX = odtIndex;
                ptr2.DAQ_ODT_ENTRY_INDEX = odtEntryIndex;
                ptr2.PTR_Address = var->ECUAddress + ptr1.PTR_Size;
                ptr2.PTR_Size = dataVolumeAG;
                ptr2.PTR_var = var;
                ptr2.DAQ_PID = pid;
                daqPTRList.append(ptr2);

                //DataBuffer *buffer2 = new DataBuffer;
                DataBuffer buffer2;
                buffer2.data = new quint8[7];
                buffer2.numData = 7;
                pidHash.insert(pid, buffer2);

                //PID_Attr* pidAttr2 = new PID_Attr;
                PID_Attr pidAttr2;
                pidAttr2.DAQ_List = curDAQList;
                pidAttr2.ODT_Offset = odtIndex*7;
                pidAttr2.biggestPID = false;
                pidAttrHash.insert(pid, pidAttr2);

                odtIndex += 1;
                pid += 1;
                dataIndex = 0;
                odtEntryCountList.append(odtEntryIndex);
                odtEntryIndex = 0;

                //ptr3 = new XCP_DAQ_PTR;
                ptr3.DAQ_LIST_INDEX = curDAQList;
                ptr3.DAQ_ODT_INDEX = odtIndex;
                ptr3.DAQ_ODT_ENTRY_INDEX = odtEntryIndex;
                ptr3.PTR_Address = var->ECUAddress + ptr1.PTR_Size + dataVolumeAG;
                ptr3.PTR_Size = var->DataSizeAG - ptr1.PTR_Size - dataVolumeAG;
                ptr3.PTR_var = var;
                ptr3.DAQ_PID = pid;
                daqPTRList.append(ptr3);

                dataIndex += (var->DataSizeAG - ptr1.PTR_Size - dataVolumeAG);
                odtEntryIndex += 1;
            }

            if (j == (vars.count()-1))
            {
                //DataBuffer *buffer = new DataBuffer;
                DataBuffer buffer;
                buffer.data = new quint8[dataIndex];
                buffer.numData = dataIndex;
                pidHash.insert(pid, buffer);

                //PID_Attr* pidAttr = new PID_Attr;
                PID_Attr pidAttr;
                pidAttr.DAQ_List = curDAQList;
                pidAttr.ODT_Offset = odtIndex*7;
                pidAttr.biggestPID = true;
                pidAttrHash.insert(pid, pidAttr);

                odtEntryCountList.append(odtEntryIndex);
                pid += 1;
            }

        }
        alloc.ODT_COUNT = odtIndex+1;
        alloc.ODT_ENTRY_COUNT = odtEntryCountList;
        qDebug()<<"DAQ List="<<alloc.DAQ_LIST_NUMBER<<", ODT Count="<<alloc.ODT_COUNT;
        for (int i = 0; i < alloc.ODT_ENTRY_COUNT.count(); i++)
        {
            qDebug()<<"ODT Index "<<i<<", ODT Entry Count="<<alloc.ODT_ENTRY_COUNT.at(i);
        }
        daqAlloc.append(alloc);

        //DataBuffer *buffer = new DataBuffer;
        DataBuffer buffer;
        buffer.data = new quint8[dataSize];
        buffer.numData = dataSize;

        daqListDataHash.insert(curDAQList, buffer);

        QSharedMemory *smDAQ = new QSharedMemory("SM_DAQ_List_"+QString::number(curDAQList));
        smDAQ->create(dataSize);
        daqListSMHash.insert(curDAQList, smDAQ);
    }

    QList<quint8> pidKeys = pidHash.keys();
    qSort(pidKeys.begin(), pidKeys.end());

    for (int i = 0; i < pidKeys.count(); i++)
    {
        quint8 key = pidKeys.at(i);
        qDebug()<<"pid "<<key<<", dataBuffer Size"<<pidHash.value(key).numData;
    }

    for (int i = 0; i < pidKeys.count(); i++)
    {
        quint8 key = pidKeys.at(i);
        qDebug()<<"pid "<<key<<", pidAttr_DAQList="<<pidAttrHash.value(key).DAQ_List
               <<", pidAttr_Offset="<<pidAttrHash.value(key).ODT_Offset
              <<", pidAttr biggestPID ="<<pidAttrHash.value(key).biggestPID;
    }

    QList<A2L_VarMeas*> varKeys = varAttrHash.keys();
    for (int i = 0; i < varKeys.count(); i++)
    {
        A2L_VarMeas *var = varKeys.at(i);
        qDebug()<<"var "<<var->Name<<", var_DAQList="<<varAttrHash.value(var).DAQ_List
               <<", var_offset="<<varAttrHash.value(var).var_Offset;
    }

    for (int i = 0; i < daqPTRList.count(); i++)
    {
        XCP_DAQ_PTR ptr = daqPTRList.at(i);

        qDebug()<<"PTR "<<i<<"PID "<<ptr.DAQ_PID<<" DAQ List="<<ptr.DAQ_LIST_INDEX<<", ODT Index="<<ptr.DAQ_ODT_INDEX<<", ODT Entry Index="
               <<ptr.DAQ_ODT_ENTRY_INDEX<<", PTR_Address="<<QString::number(ptr.PTR_Address, 16)<<", PTR_Size="<<ptr.PTR_Size;
    }
    qDebug()<<"==================================";
}

void XCPMaster::arrangeMeasVars_dlc64()
{
    xcpDaqConfigPamReset();

    // ???????????????????-?????б?Hash??
    for (int i = 0; i < daqMeasVarList.count(); i++)
    {
        A2L_VarMeas *var = daqMeasVarList.at(i);
        quint16 rate = var->rate_ms;                                      // ???DAQ ???? ????????
        sizeDataBuffer_DAQ += var->DataSizeAG;                  // ????DAQ ???? ???С

        //varValueHash.insert(var, 0.0);                                       // ?????????-? Hash

        // ???????????????????-?????б? Hash
        if (!rateVarHash.keys().contains(rate))
        {
            QList<A2L_VarMeas*> varList;
            varList.append(var);
            rateVarHash.insert(rate,varList);
        }
        else
        {
            QList<A2L_VarMeas*> varList = rateVarHash.value(rate);
            varList.append(var);
            rateVarHash.insert(rate, varList);
        }
    }

    //DAQ ???????
    //DataBuffer_DAQ = new quint8[sizeDataBuffer_DAQ];

    quint8 pid = 0;
    quint32 offset = 0;

    QList<float> keys = rateVarHash.keys();
    std::sort(keys.begin(), keys.end());

    if (keys.count() == 0)
        return;

    //对参数配置后DAQ按照EVENT的序号来排序，因为有可能出现周期大的EVENT序号反而小
    //这种情况下不重新排序的话会导致EVENT不起作用，报FD 06(DAQ overload)

    for(int i = 0; i < keys.count() - 1; i++)
    {

        float rate = keys.at(i);
        float rate_next = keys.at(i+1);

        A2L_Event *event = getEventChannelByEventRate(rate);
        A2L_Event *event_next = getEventChannelByEventRate(rate_next);

        if(!event || !event_next) continue;

        if(event->ChannelNumber > event_next->ChannelNumber)
        {
            float temp = keys.at(i);
            keys[i] = keys[i+1];
            keys[i+1] = temp;
        }
    }


    DAQ_COUNT = keys.count();
    for (int i = 0; i < keys.count(); i++)
    {
        QList<A2L_VarMeas*> vars = rateVarHash.value(keys.at(i));
        quint8 odtIndex = 0;
        quint8 odtEntryIndex = 0;
        quint8 dataIndex = 0;
        quint8 dataVolumeAG = 63;
        quint32 varIndex = 0;

        quint16 curDAQList = i;
        daqListVarHash.insert(curDAQList, vars);

        XCP_DAQ_ALLOC alloc;
        alloc.DAQ_LIST_NUMBER = curDAQList;
        QList<quint8> odtEntryCountList;

        quint32 dataSize = 0;

        for (int j = 0; j < vars.count(); j++)
        {
            A2L_VarMeas *var = vars.at(j);

            bool lastVar = (vars.count() - 1) == j ? true : false;

            dataSize += var->DataSizeAG;

            //var_Attr* varAttr = new var_Attr;
            var_Attr varAttr;
            varAttr.DAQ_List = curDAQList;
            varAttr.var_Offset = varIndex;
            varAttrHash.insert(var, varAttr);
            varIndex += var->DataSizeAG;

            XCP_DAQ_PTR ptr1, ptr2, ptr3;

            //ptr1 = new XCP_DAQ_PTR;
            ptr1.DAQ_LIST_INDEX = MIN_DAQ + i;
            ptr1.DAQ_ODT_INDEX = odtIndex;
            ptr1.DAQ_ODT_ENTRY_INDEX = odtEntryIndex;
            ptr1.PTR_Address = var->ECUAddress;
            ptr1.PTR_var = var;
            ptr1.DAQ_PID = pid;
            ptr1.offsetDataBuffer = offset;

            odtEntryIndex += 1;

            if ((dataIndex + var->DataSizeAG) < dataVolumeAG)
            {
                ptr1.PTR_Size = var->DataSizeAG;
                daqPTRList.append(ptr1);

                dataIndex += var->DataSizeAG;

            }
            else if((dataIndex + var->DataSizeAG) == dataVolumeAG)
            {
                ptr1.PTR_Size = var->DataSizeAG;
                daqPTRList.append(ptr1);

                DataBuffer buffer1;
                buffer1.data = new quint8[63];
                buffer1.numData = 63;
                pidHash.insert(pid, buffer1);

                PID_Attr pidAttr1;
                pidAttr1.DAQ_List = curDAQList;
                pidAttr1.ODT_Offset = odtIndex * 63;
                pidAttr1.biggestPID = lastVar ? true : false;
                pidAttrHash.insert(pid, pidAttr1);

                odtIndex += lastVar ? 0 : 1;
                pid += lastVar ? 0 : 1;
                dataIndex = 0;
                odtEntryCountList.append(odtEntryIndex);
                odtEntryIndex = 0;

            }
            else if ((dataIndex + var->DataSizeAG) < (dataVolumeAG*2) && (dataIndex + var->DataSizeAG) > dataVolumeAG)
            {
                ptr1.PTR_Size = (dataVolumeAG - dataIndex);
                daqPTRList.append(ptr1);

                //DataBuffer *buffer1 = new DataBuffer;
                DataBuffer buffer1;
                buffer1.data = new quint8[63];
                buffer1.numData = 63;
                pidHash.insert(pid, buffer1);


                //PID_Attr* pidAttr1 = new PID_Attr;
                PID_Attr pidAttr1;
                pidAttr1.DAQ_List = curDAQList;
                pidAttr1.ODT_Offset = odtIndex * 63;
                pidAttr1.biggestPID = false;
                pidAttrHash.insert(pid, pidAttr1);

                odtIndex += 1;
                pid += 1;
                dataIndex = 0;
                odtEntryCountList.append(odtEntryIndex);
                odtEntryIndex = 0;

                //ptr2 = new XCP_DAQ_PTR;
                ptr2.DAQ_LIST_INDEX = MIN_DAQ + i;
                ptr2.DAQ_ODT_INDEX = odtIndex;
                ptr2.DAQ_ODT_ENTRY_INDEX = odtEntryIndex;
                ptr2.PTR_Address = var->ECUAddress + ptr1.PTR_Size;
                ptr2.PTR_Size = (var->DataSizeAG - ptr1.PTR_Size);
                ptr2.PTR_var = var;
                ptr2.DAQ_PID = pid;
                daqPTRList.append(ptr2);

                dataIndex += (var->DataSizeAG - ptr1.PTR_Size);
                odtEntryIndex += 1;

            }

            if (lastVar && dataIndex != 0)
            {
                DataBuffer buffer;
                buffer.data = new quint8[dataIndex];
                buffer.numData = dataIndex;
                pidHash.insert(pid, buffer);

                PID_Attr pidAttr;
                pidAttr.DAQ_List = curDAQList;
                pidAttr.ODT_Offset = odtIndex * 63;
                pidAttr.biggestPID = true;
                pidAttrHash.insert(pid, pidAttr);

                odtEntryCountList.append(odtEntryIndex);
                pid += 1;
            }

        }

        alloc.ODT_COUNT = odtIndex + 1;
        alloc.ODT_ENTRY_COUNT = odtEntryCountList;
        qDebug()<<"DAQ List="<<alloc.DAQ_LIST_NUMBER<<", ODT Count="<<alloc.ODT_COUNT<<", ODT Entry List Count="<<alloc.ODT_ENTRY_COUNT.count()<<", rate="<<keys.at(i);
        for (int i = 0; i < alloc.ODT_ENTRY_COUNT.count(); i++)
        {
            qDebug()<<"ODT Index "<<i<<", ODT Entry Count="<<alloc.ODT_ENTRY_COUNT.at(i);
        }
        daqAlloc.append(alloc);

        DataBuffer buffer;
        buffer.data = new quint8[dataSize];
        buffer.numData = dataSize;

        daqListDataHash.insert(curDAQList, buffer);

        QSharedMemory *smDAQ = new QSharedMemory("RP_DAQ_" + xcpName + "_" +QString::number(curDAQList));
        smDAQ->create(dataSize + 8);   //add time pam space in each daq data buffer
        daqListSMHash.insert(curDAQList, smDAQ);

        daqListBlockSizeHash.insert(curDAQList, dataSize + 8); // add 8 bytes for time pam used for record

    }

    QList<quint8> pidKeys = pidHash.keys();
    std::sort(pidKeys.begin(), pidKeys.end());

    for (int i = 0; i < pidKeys.count(); i++)
    {
        quint8 key = pidKeys.at(i);
        qDebug()<<"pid "<<key<<", dataBuffer Size"<<pidHash.value(key).numData;
    }

    for (int i = 0; i < pidKeys.count(); i++)
    {
        quint8 key = pidKeys.at(i);
        qDebug()<<"pid "<<key<<", pidAttr_DAQList="<<pidAttrHash.value(key).DAQ_List
               <<", pidAttr_Offset="<<pidAttrHash.value(key).ODT_Offset
              <<", pidAttr biggestPID ="<<pidAttrHash.value(key).biggestPID;
    }

    QList<A2L_VarMeas*> varKeys = varAttrHash.keys();
    for (int i = 0; i < daqMeasVarList.count(); i++)
    {
        A2L_VarMeas *var = daqMeasVarList.at(i);

        qDebug()<<"var "<<var->Name<<", var_DAQList="<<varAttrHash.value(var).DAQ_List
               <<", var_offset="<<varAttrHash.value(var).var_Offset;
    }

    for (int i = 0; i < daqPTRList.count(); i++)
    {
        XCP_DAQ_PTR ptr = daqPTRList.at(i);

        qDebug()<<"PTR "<<i<<"PID "<<ptr.DAQ_PID<<" DAQ List="<<ptr.DAQ_LIST_INDEX<<", ODT Index="<<ptr.DAQ_ODT_INDEX<<", ODT Entry Index="
               <<ptr.DAQ_ODT_ENTRY_INDEX<<", PTR_Address="<<QString::number(ptr.PTR_Address, 16)<<", PTR_Size="<<ptr.PTR_Size;
    }
    qDebug()<<"==================================";
}

bool XCPMaster::XCP_DAQ_ALLOC_DYN()
{
    //dynamic Configuration
    if (!CTO_FREE_DAQ())
        return false;

    DAQ_COUNT = daqAlloc.count();

    if (!CTO_ALLOC_DAQ())
        return false;
    for (int i = MIN_DAQ; i < DAQ_COUNT; i++)
    {
        XCP_DAQ_ALLOC alloc = daqAlloc.at(i);
        DAQ_LIST_NUMBER = i;
        ODT_COUNT = alloc.ODT_COUNT;

        if (!CTO_ALLOC_ODT())
            return false;
    }
    for (int i = MIN_DAQ; i < DAQ_COUNT; i++)
    {
        XCP_DAQ_ALLOC alloc = daqAlloc.at(i);
        DAQ_LIST_NUMBER = i;
        ODT_COUNT = alloc.ODT_COUNT;
        for (int j = 0; j < ODT_COUNT; j++)
        {
            ODT_NUMBER = j;
            ODT_ENTRIES_COUNT = alloc.ODT_ENTRY_COUNT.at(j);
            if (!CTO_ALLOC_ODT_ENTRY())
                return false;
        }
    }
    return true;
}

bool XCPMaster::XCP_PREPARE_DAQ_LIST_DYN()
{

    if (!CTO_FREE_DAQ())
        return false;

    DAQ_COUNT = daqAlloc.count();

    if (!CTO_ALLOC_DAQ())
        return false;

    for (int i = MIN_DAQ; i < DAQ_COUNT; i++)
    {
        XCP_DAQ_ALLOC alloc = daqAlloc.at(i);
        DAQ_LIST_NUMBER = i;
        ODT_COUNT = alloc.ODT_COUNT;

        if (!CTO_ALLOC_ODT())
            return false;
    }
    for (int i = MIN_DAQ; i < DAQ_COUNT; i++)
    {
        XCP_DAQ_ALLOC alloc = daqAlloc.at(i);
        DAQ_LIST_NUMBER = i;
        ODT_COUNT = alloc.ODT_COUNT;
        for (int j = 0; j < ODT_COUNT; j++)
        {
            ODT_NUMBER = j;
            ODT_ENTRIES_COUNT = alloc.ODT_ENTRY_COUNT.at(j);
            if (!CTO_ALLOC_ODT_ENTRY())
                return false;
        }
    }
    return true;
}

bool XCPMaster::XCP_CONFIG_DAQ_LIST_DYN()
{
    for (int i = 0; i < daqPTRList.count(); i++)
    {
        XCP_DAQ_PTR ptr = daqPTRList.at(i);

        DAQLISTNUMBER_SETPTR = ptr.DAQ_LIST_INDEX;
        ODTNUMBER_SETPTR = ptr.DAQ_ODT_INDEX;
        ODTENTRYNUMBER_SETPTR = ptr.DAQ_ODT_ENTRY_INDEX;
        if (!CTO_SET_DAQ_PTR())
            return false;

        ADDRESSEXT_WRITEDAQ = 0x00;
        SIZE_WRITEDAQ = ptr.PTR_Size;
        ADDRESS_WRITEDAQ = ptr.PTR_Address;
        if (!CTO_WRITE_DAQ())
            return false;
    }
    return true;
}


bool XCPMaster::XCP_PREPARE_DAQ_LIST_STATIC()
{
    if (xcpDaq == NULL)
        return false;
    if (xcpDaqList.isEmpty())
        return false;

    for (int i = xcpDaq->Min_Daq; i < xcpDaq->Max_Daq; i++)
    {
        DAQLISTNUMBER_CLEARDAQLIST = (quint16)i;
        if (!CTO_CLEAR_DAQ_LIST())
            return false;
    }

    return true;
}

bool XCPMaster::XCP_CONFIG_DAQ_LIST_STATIC()
{
    if (xcpDaq == NULL)
        return false;
    if (xcpDaqList.isEmpty())
        return false;

    for (int i = 0; i < daqPTRList.count(); i++)
    {
        XCP_DAQ_PTR ptr = daqPTRList.at(i);

        DAQLISTNUMBER_SETPTR = ptr.DAQ_LIST_INDEX;
        ODTNUMBER_SETPTR = ptr.DAQ_ODT_INDEX;
        ODTENTRYNUMBER_SETPTR = ptr.DAQ_ODT_ENTRY_INDEX;
        if (!CTO_SET_DAQ_PTR())
            return false;

        ADDRESSEXT_WRITEDAQ = 0x00;
        SIZE_WRITEDAQ = ptr.PTR_Size;
        ADDRESS_WRITEDAQ = ptr.PTR_Address;
        if (!CTO_WRITE_DAQ())
            return false;
    }
    return true;
}

bool XCPMaster::XCP_DAQ_LIST_CONFIG()
{
    for (int i = 0; i < daqPTRList.count(); i++)
    {
        XCP_DAQ_PTR ptr = daqPTRList.at(i);

        DAQLISTNUMBER_SETPTR = ptr.DAQ_LIST_INDEX;
        ODTNUMBER_SETPTR = ptr.DAQ_ODT_INDEX;
        ODTENTRYNUMBER_SETPTR = ptr.DAQ_ODT_ENTRY_INDEX;
        if (!CTO_SET_DAQ_PTR())
            return false;

        ADDRESSEXT_WRITEDAQ = 0x00;
        SIZE_WRITEDAQ = ptr.PTR_Size;
        ADDRESS_WRITEDAQ = ptr.PTR_Address;
        if (!CTO_WRITE_DAQ())
            return false;
    }
    return true;
}

bool XCPMaster::XCP_SET_DAQ_LIST_MODE_FORALL()
{
    QList<float> keys = rateVarHash.keys();
    std::sort(keys.begin(), keys.end());

    //对参数配置后DAQ按照EVENT的序号来排序，因为有可能出现周期大的EVENT序号反而小
    //这种情况下不重新排序的话会导致EVENT不起作用，报FD 06(DAQ overload)

    for(int i = 0; i < keys.count() - 1; i++)
    {

        float rate = keys.at(i);
        float rate_next = keys.at(i+1);

        A2L_Event *event = getEventChannelByEventRate(rate);
        A2L_Event *event_next = getEventChannelByEventRate(rate_next);

        if(!event || !event_next) continue;

        if(event->ChannelNumber > event_next->ChannelNumber)
        {
            float temp = keys.at(i);
            keys[i] = keys[i+1];
            keys[i+1] = temp;
        }
    }


    for (int i = 0; i < keys.count(); i++)
    {
        float rate = keys.at(i);

        DAQLISTNUMBER_SETDAQLISTMODE = i;

        A2L_Event *event = getEventChannelByEventRate(rate);
        if(event)
        {
            EVENTCHANNEL_SETDAQLISTMODE = (quint16)(event->ChannelNumber);
        }

        qInfo()<<"SET_DAQ_LIST_MODE: DAQ List Number- "<<DAQLISTNUMBER_SETDAQLISTMODE
               <<", EVENT Channel Number- "<<EVENTCHANNEL_SETDAQLISTMODE<<", rate(ms): "<<rate;

        if (!CTO_SET_DAQ_LIST_MODE())
            return false;
    }

    return true;
}

bool XCPMaster::XCP_STARTSTOP_DAQ_SELECT(bool Flag)
{
    QList<float> keys = rateVarHash.keys();
    std::sort(keys.begin(), keys.end());

    for (int i = 0; i < keys.count(); i++)
    {
        DAQLISTNUMBER_STARTSTOP = i;
        MODE_STARTSTOPDAQLIST = 0x02;  //0x02=Select
        if (!CTO_START_STOP_DAQ_LIST())
            return false;
    }

    if (Flag)
    {
        MODE_STARTSTOPSYNCH = 0x01; //Start Select
    }
    else
    {
        MODE_STARTSTOPSYNCH = 0x02; //Stop Select
    }

    if (!CTO_START_STOP_SYNCH())
        return false;
    if (!CTO_GET_STATUS())
        return false;

    return true;
}

void XCPMaster::setAddressMTA(quint32 Address, quint8 Address_Ext)
{
    MTA_Address = Address;
    MTA_Address_Extension = Address_Ext;
}

void XCPMaster::setDataDOWNLOAD(quint8 *data, quint8 numDataAG)
{
    if(numDataAG > 6)
        return;
    if(ByteOrder_XCPAddr == 1)
        reverse((char*)data, numDataAG);

    ClearDataDownload();
    memcpy(data_DOWNLOAD, data, numDataAG);
    numOfDataAG_DOWNLOAD = numDataAG;

    //qDebug()<<"set dataDownload data0"
           //<<data_DOWNLOAD[0]<<",data1"<<data_DOWNLOAD[1]<<",data2"
           //<<data_DOWNLOAD[2]<<",data3"<<data_DOWNLOAD[3];
}

void XCPMaster::setnumOfDataUPLOAD(quint8 numOfData)
{
    numOfDataAG_UPLOAD = numOfData;
}

void XCPMaster::getDataUPLOAD(quint8 *data, quint8 numDataAG)
{
    memcpy(data, data_UPLOAD, numDataAG);
}

void XCPMaster::setMeasVarListForDAQ(QList<A2L_VarMeas *> varList)
{
    daqMeasVarList = varList;
}

void XCPMaster::addMeasVarListForDAQ(QList<A2L_VarMeas *> varList)
{
    daqMeasVarList.append(varList);
}

void XCPMaster::setBaudrate(quint64 baudrate)
{
    this->xcpBaudrate = baudrate;
}

void XCPMaster::setIDMasterToSlave(quint32 id)
{
    this->ID_Master_to_Slave = id;
}

void XCPMaster::setIDSlaveToMaster(quint32 id)
{
    this->ID_Slave_to_Master = id;
    if(xcpCanThread)
    {
        xcpCanThread->thread_RES->setId_Xcp_SlaveToMaster(this->ID_Slave_to_Master);
    }
}

void XCPMaster::setXcpCanInterface(QString canInterface)
{
    this->xcpcan_Interface = canInterface;
}

void XCPMaster::setResistanceEnable(int value)
{
    resistanceEnable = value;
}

void XCPMaster::setMaxDLC(const quint8 &value)
{
    maxDLC = value;
}

void XCPMaster::setXcpFdBaudrate(const quint64 &value)
{
    xcpFdBaudrate = value;
}

void XCPMaster::setIsCanFd(bool value)
{
    isCanFd = value;
}

void XCPMaster::getCTORESData(ByteArrayPtr data, quint32 numBytes)
{
    quint8 *rawData = data.data();
    if(isCanFd && (maxDLC == 64))
    {
        nxFrameCANFD_t *resFrame = (nxFrameCANFD_t*)rawData;
        quint8 *buf = resFrame->Payload;
        quint8 len = resFrame->PayloadLength;
        memcpy(CTO_RESPacket, buf, len);

        quint8 *time = (quint8*)&(resFrame->Timestamp);
        memcpy(CTO_RESTime, time, 8);

        CTORESDataUpdated = true;
    }
    else
    {
        nxFrameCAN_t *resFrame = (nxFrameCAN_t*)rawData;
        quint8 *buf = resFrame->Payload;
        quint8 len = resFrame->PayloadLength;
        memcpy(CTO_RESPacket, buf, len);

        quint8 *time = (quint8*)&(resFrame->Timestamp);
        memcpy(CTO_RESTime, time, 8);

        CTORESDataUpdated = true;
    }

    //qDebug()<<"CTO-RES[0]="<<QString::number(*CTO_RESPacket, 16)
    //        <<", RES[1]="<<QString::number(*(CTO_RESPacket+1), 16);
}

void XCPMaster::getDAQODTData(ByteArrayPtr data, quint32 numData)
{    

    quint8 *rawData = data.data();
    if(isCanFd && (maxDLC == 64))
    {
        nxFrameCANFD_t *daqFrame = (nxFrameCANFD_t*)rawData;
        quint8 *buf = daqFrame->Payload;
        quint32 len = daqFrame->PayloadLength;
        memcpy(DAQ_ODTPacket, buf, len);

        quint8 pid = *DAQ_ODTPacket;

        PID_Attr attr = pidAttrHash.value(pid);
        quint16 DAQList = attr.DAQ_List;

        QSharedMemory *smDAQ = daqListSMHash.value(DAQList);
        quint32 smSize = daqListBlockSizeHash.value(DAQList);
        if(!smDAQ)
            return;

        if (!smDAQ->isAttached())
        {
            smDAQ->attach();
        }

        quint8 *time = (quint8*)&(daqFrame->Timestamp);
        quint8 *to = static_cast<quint8*>(smDAQ->data());
        quint8 *from = DAQ_ODTPacket+1;
        smDAQ->lock();
        memcpy(to, time, 8);
        memcpy(to+8+attr.ODT_Offset, from, (len-1));
        smDAQ->unlock();

        if (attr.biggestPID)
        {
            emit ODTDataUpdated(DAQList);

            ByteArrayPtr buf_rcd = makeByteArray(smSize);
            smDAQ->lock();
            memcpy(buf_rcd.data(), to, smSize);
            smDAQ->unlock();

            emit ODTDataForRecord(buf_rcd, smSize, DAQList);
            emit ODTDataForRecord(buf_rcd, smSize, "RP_DAQ_" + xcpName + "_" + QString::number(DAQList));
        }
    }
    else
    {
        nxFrameCAN_t *daqFrame = (nxFrameCAN_t*)rawData;
        quint8 *buf = daqFrame->Payload;
        quint32 len = daqFrame->PayloadLength;
        memcpy(DAQ_ODTPacket, buf, len);

        //memcpy(DAQ_ODTPacket, data, numData);

        quint8 pid = *DAQ_ODTPacket;

        //if(pid == 0)
            //qDebug()<<"pid:"<<pid<<", "<<*(DAQ_ODTPacket+1);

        PID_Attr attr = pidAttrHash.value(pid);
        quint16 DAQList = attr.DAQ_List;

        QSharedMemory *smDAQ = daqListSMHash.value(DAQList);
        quint32 smSize = daqListBlockSizeHash.value(DAQList);
        if(!smDAQ)
            return;

        if (!smDAQ->isAttached())
        {
            smDAQ->attach();
        }

        //if(pid == 0)
            //qDebug()<<"ODT_Offset:"<<attr.ODT_Offset;

        quint8 *time = (quint8*)&(daqFrame->Timestamp);
        quint8 *to = static_cast<quint8*>(smDAQ->data());
        quint8 *from = DAQ_ODTPacket+1;
        smDAQ->lock();
        memcpy(to, time, 8);
        memcpy(to+8+attr.ODT_Offset, from, (len-1));
        smDAQ->unlock();

        if (attr.biggestPID)
        {
            emit ODTDataUpdated(DAQList);

            ByteArrayPtr buf_rcd = makeByteArray(smSize);
            smDAQ->lock();
            memcpy(buf_rcd.data(), to, smSize);
            smDAQ->unlock();

            emit ODTDataForRecord(buf_rcd, smSize, DAQList);
            emit ODTDataForRecord(buf_rcd, smSize, "RP_DAQ_" + xcpName + "_" + QString::number(DAQList));
        }
    }


}

void XCPMaster::getCTORESData_TS(quint8 *data, quint32 numBytes)
{
    quint8 *buf = data + 8;
    quint8 len = numBytes - 8;
    memcpy(CTO_RESPacket, buf, len);

    quint8 *time = data;
    memcpy(CTO_RESTime, time, 8);

    CTORESDataUpdated = true;

    //qDebug()<<"CTO-RES[0]="<<QString::number(*CTO_RESPacket, 16)
    //       <<", RES[1]="<<QString::number(*(CTO_RESPacket+1), 16);
}

void XCPMaster::getDAQODTData_TS(quint8 *data, quint32 numData)
{
    if(numData <= 8) return;

    quint8 *buf = data + 8;
    quint32 len = numData - 8;
    memcpy(DAQ_ODTPacket, buf, len);

    quint8 pid = *DAQ_ODTPacket;
    //qDebug()<<"pid:"<<pid<<", num:"<<numData;

    PID_Attr attr = pidAttrHash.value(pid);
    quint16 DAQList = attr.DAQ_List;

    QSharedMemory *smDAQ = daqListSMHash.value(DAQList);
    quint32 smSize = daqListBlockSizeHash.value(DAQList);
    if(!smDAQ) return;

    if (!smDAQ->isAttached())
    {
        smDAQ->attach();
    }

    quint8 *time = data;
    quint8 *to = static_cast<quint8*>(smDAQ->data());
    quint8 *from = DAQ_ODTPacket+1;
    smDAQ->lock();
    memcpy(to, time, 8);
    memcpy(to+8+attr.ODT_Offset, from, (len-1));
    smDAQ->unlock();

    if (attr.biggestPID)
    {
        emit ODTDataUpdated(DAQList);

        ByteArrayPtr buf_rcd = makeByteArray(smSize);
        smDAQ->lock();
        memcpy(buf_rcd.data(), to, smSize);
        smDAQ->unlock();

        emit ODTDataForRecord(buf_rcd, smSize, DAQList);
        emit ODTDataForRecord(buf_rcd, smSize, "RP_DAQ_" + xcpName + "_" + QString::number(DAQList));
    }
}

void XCPMaster::getCTORESData_ZLG(quint8 *data, quint32 numBytes)
{
    quint8 *buf = data + 8;
    quint8 len = numBytes - 8;
    memcpy(CTO_RESPacket, buf, len);

    quint8 *time = data;
    memcpy(CTO_RESTime, time, 8);

    CTORESDataUpdated = true;

    //qDebug()<<"CTO-RES[0]="<<QString::number(*CTO_RESPacket, 16)
    //       <<", RES[1]="<<QString::number(*(CTO_RESPacket+1), 16);
}

void XCPMaster::getDAQODTData_ZLG(quint8 *data, quint32 numData)
{
    quint8 *buf = data + 8;
    quint32 len = numData - 8;
    memcpy(DAQ_ODTPacket, buf, len);

    quint8 pid = *DAQ_ODTPacket;

    PID_Attr attr = pidAttrHash.value(pid);
    quint16 DAQList = attr.DAQ_List;

    QSharedMemory *smDAQ = daqListSMHash.value(DAQList);
    quint32 smSize = daqListBlockSizeHash.value(DAQList);
    if(!smDAQ)
        return;

    if (!smDAQ->isAttached())
    {
        smDAQ->attach();
    }

    quint8 *time = data;
    quint8 *to = static_cast<quint8*>(smDAQ->data());
    quint8 *from = DAQ_ODTPacket+1;
    smDAQ->lock();
    memcpy(to, time, 8);
    memcpy(to+8+attr.ODT_Offset, from, (len-1));
    smDAQ->unlock();

    if (attr.biggestPID)
    {
        emit ODTDataUpdated(DAQList);

        ByteArrayPtr buf_rcd = makeByteArray(smSize);
        smDAQ->lock();
        memcpy(buf_rcd.data(), to, smSize);
        smDAQ->unlock();

        emit ODTDataForRecord(buf_rcd, smSize, DAQList);
        emit ODTDataForRecord(buf_rcd, smSize, "RP_DAQ_" + xcpName + "_" + QString::number(DAQList));
    }
}

void XCPMaster::ODTDataUpdatedSlot(quint16 DAQList)
{
    QList<A2L_VarMeas*> vars = daqListVarHash.value(DAQList);
    QSharedMemory *smDAQ = daqListSMHash.value(DAQList);
    for(int i = 0; i < vars.count(); i++)
    {
        A2L_VarMeas *var = vars.at(i);
        var_Attr attr = varAttrHash.value(var);

        quint64 data = 0;
        float value = 0;

        if (!smDAQ->isAttached())
            smDAQ->attach();

        quint8 *from = static_cast<quint8*>(smDAQ->data());
        quint8 *to = (quint8*)&data;
        smDAQ->lock();
        memcpy(to, from + 8 + attr.var_Offset,  var->DataSizeAG);
        smDAQ->unlock();

        if (var->DataSizeAG == 4)
        {

            if(var->DataType == "ULONG")
            {
                quint32 valueULONG = 0;
                memcpy((quint8*)&valueULONG, (quint8*)&data, 4);
                value = (float)valueULONG;
            }
            else if(var->DataType == "FLOAT32_IEEE")
            {
                memcpy((quint8*)&value, (quint8*)&data, 4);
            }
            else if(var->DataType == "SLONG")
            {
                qint32 valueSLONG = 0;
                memcpy((quint8*)&valueSLONG, (quint8*)&data, 4);
                value = (float)valueSLONG;
            }

        }
        else if (var->DataSizeAG == 2)
        {
            if(var->DataType == "UWORD")
            {
                quint16 valueUWORD = 0;
                memcpy((quint8*)&valueUWORD, (quint8*)&data, 2);
                value = (float)valueUWORD;
            }
            else if(var->DataType == "SWORD")
            {
                qint16 valueSWORD = 0;
                memcpy((quint8*)&valueSWORD, (quint8*)&data, 2);
                value = (float)valueSWORD;
            }

        }
        else if (var->DataSizeAG == 1)
        {
            //quint8 valueUBYTE = 0;
            //memcpy(&valueUBYTE, (quint8*)&data, 1);
            //value = (float)valueUBYTE;

            if(var->DataType == "UBYTE")
            {
                quint8 valueUBYTE = 0;
                memcpy(&valueUBYTE, (quint8*)&data, 1);
                value = (float)valueUBYTE;
            }
            else if(var->DataType == "SBYTE")
            {
                qint8 valueSBYTE = 0;
                memcpy(&valueSBYTE, (quint8*)&data, 1);
                value = (float)valueSBYTE;
            }
        }
        //qDebug()<<"var:"<<var->Name;
        //qDebug()<<"data:"<<*(quint8*)&data<<",valueInt="<<value;

        qreal valuePhys;
        if(var->ConversionType == "RAT_FUNC")
        {
            valuePhys = RatFuncConvFromIntToPhys(value, var->CoefList);
        }
        else if(var->ConversionType == "TAB_VERB")
        {
            valuePhys = value;
        }
        else
        {
            valuePhys = value;
        }

        //qDebug()<<"valuePhy="<<valuePhys;

        /*
        if(var->Name == "VeTIPR_b_IgnRaw")
        {
            qDebug()<<"var:"<<var->Name<<", varOffset:"<<attr.var_Offset;
            qDebug()<<"data:"<<*(quint8*)&data<<",valueInt="<<value<<", hex:"<<QString::number(data, 16);
            qDebug()<<"valuePhy="<<valuePhys;
        }
        */

        var->setValue(valuePhys);
    }
}

qreal XCPMaster::RatFuncConvFromIntToPhys(qreal valueInt, QList<qreal> coefList)
{
    //qDebug()<<"coef Count="<<coefList.count();

    if (coefList.count() != 6)
        return 0;

    qreal y = valueInt;
    qreal a = coefList.at(0);
    qreal b = coefList.at(1);
    qreal c = coefList.at(2);
    qreal d = coefList.at(3);
    qreal e = coefList.at(4);
    qreal f = coefList.at(5);


    if (a == 0 && d == 0)
    {
        qreal valuePhys = (c + f*y)/(b - e*y);
        //qDebug()<<"b="<<b<<", c="<<c;
        return valuePhys;
    }
    return 0;

}

qreal XCPMaster::RatFuncConvFromPhysToInt(qreal valuePhys, QList<qreal> coefList)
{
    if (coefList.count() != 6)
        return 0;
    qreal x = valuePhys;
    qreal a = coefList.at(0);
    qreal b = coefList.at(1);
    qreal c = coefList.at(2);
    qreal d = coefList.at(3);
    qreal e = coefList.at(4);
    qreal f = coefList.at(5);
    //qDebug()<<"valuePhys="<<valuePhys<<", a="<<a<<",b="<<b<<",c="<<c<<",d="<<d<<",e="<<e<<",f="<<f;


    qreal valueInt = (a*x*x + b*x + c)/(d*x*x + e*x+ f);
    //qDebug()<<"valueInt="<<valueInt;

    return valueInt;
}

void XCPMaster::charVarValueChangedSlot(A2L_VarChar *charVar)
{
    qDebug()<<"In xcpMaster value changed slot"<<charVar->Name<<",Value="<<charVar->getValue();
    XCP_Cal_VALUE(charVar);
}

void XCPMaster::curveCharZValueChangedSlot(A2L_VarChar *curveChar, int index, qreal data)
{
    qDebug()<<"In xcpMaster curvevalue changed slot"<<curveChar->Name<<",Z index="<<index<<", data="<<data;
    XCP_Cal_CURVE(curveChar, index, data);
}

bool XCPMaster::getXcpCanInitStatus()
{
    return xcpCanInitFlag;
}

bool XCPMaster::getXcpSessionConnectedStatus()
{
    return XCPConnected;
}

bool XCPMaster::getXcpDaqRunningStatus()
{
    //if (!xcpCanInitFlag)
        //return false;
    //if (!CTO_GET_STATUS())
        //return false;

    return DAQ_Running_GETSTATUS;
}

void XCPMaster::downloadCals()
{
    if(!xcpCanInitFlag || !XCPConnected)
        return;
    if(DAQ_Running_GETSTATUS)
    {
        qDebug()<<"Please stoping daq before downloading.";
        return;
    }

    int max = charList.count();
    for(int i = 0; i < max; i++)
    {
        A2L_VarChar *charVar = charList.at(i);
        if(charVar->charType == "VALUE")
        {
            if(!XCP_Cal_VALUE(charVar))
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
            if(!XCP_Cal_VAL_BLK(charVar))
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
            if(!XCP_Cal_CURVE(charVar))
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


}

void XCPMaster::uploadCals()
{

}

void XCPMaster::sltDaqRunStatusActive(bool status)
{
    if(!xcpCanInitFlag || !XCPConnected)
        return;

    int count = 0;
    while ((DAQ_Running_GETSTATUS != status) && count < 3)
    {
        XCP_DAQ_Start_Stop(status);
        count++;
    }
}

int XCPMaster::getPollMeasIndexInList(A2L_VarMeas *measVar)
{
    if(!measVar || pollMeasList.isEmpty())
        return -1;
    return pollMeasList.indexOf(measVar);
}

bool XCPMaster::copyFileToPath(QString sourceDir, QString toDir, bool coverFileIfExist)
{
    toDir.replace("\\","/");
    if (sourceDir == toDir){
        return true;
    }
    if (!QFile::exists(sourceDir)){
        return false;
    }
    QDir *createfile     = new QDir;
    bool exist = createfile->exists(toDir);
    if (exist){
        if(coverFileIfExist){
            createfile->remove(toDir);
        }
    }//end if

    if(!QFile::copy(sourceDir, toDir))
    {
        return false;
    }
    return true;
}



void XCPMaster::setXcpDeviceType(int value)
{
    xcpDeviceType = value;
}


void XCPMaster::setZlgDeviceIndex(const quint32 &value)
{
    zlgDeviceIndex = value;
}

void XCPMaster::setZlgDeviceType(const quint32 &value)
{
    zlgDeviceType = value;
}

void XCPMaster::setXcpName(const QString &value)
{
    xcpName = value;
}


void XCPMaster::xcpdbcUpdate()
{
    QString appPath = QApplication::applicationDirPath();

    QString filePath;
    QString dbcFilePath;

    QString sourceXmlFile = appPath + "/xcp_dbc/xcp.xml";
    QString sourceDbcFile = appPath + "/xcp_dbc/xcp.xml";
    QString dstXmlFile = appPath + "/xcp_dbc/xcp_" + xcpName +".xml";
    QString dstDbcFile = appPath + "/xcp_dbc/xcp_" + xcpName +".dbc";
    if(copyFileToPath(sourceXmlFile, dstXmlFile, true) && copyFileToPath(sourceDbcFile, dstDbcFile, true))
    {
        filePath = dstXmlFile;
        dbcFilePath = dstDbcFile;
    }
    else
    {
        filePath = appPath + "/xcp_dbc/xcp.xml";
        dbcFilePath = appPath + "/xcp_dbc/xcp.dbc";
    }

    //QString filePath = appPath + "/xcp_dbc/xcp.xml";
    //QString dbcFilePath = appPath + "/xcp_dbc/xcp.dbc";

    xcpdbc_Path = filePath;

    qDebug()<<"xcp dbc path:"<<filePath;

    QFile dbcFile(dbcFilePath);
    if (!dbcFile.open(QIODevice::WriteOnly))
        dbcFile.close();

    if (dbcFile.exists())
    {
        QByteArray temp_path = filePath.toLocal8Bit();
        const char *dbcPath = temp_path.data();

        QString name;
        QStringList fileList = filePath.split("/", QString::SkipEmptyParts);
        //qDebug()<<fileList.count();
        name = fileList.at(fileList.count()-1);
        name = name.split(".", QString::SkipEmptyParts)[0];
        QByteArray temp_name = name.toLocal8Bit();
        char *dbcName = temp_name.data();

        u64 sel_baudrate = xcpBaudrate;
        u64 sel_fdBaudrate = xcpFdBaudrate;
        u32 canIoMode = isCanFd ? 2 : 0;
        u32 maxDlc = isCanFd ? 64 : 8;
        u32 fdIsoMode = 0;


        nxStatus_t status_dbc;

        status_dbc = nxdbAddAlias64(dbcName, dbcPath, sel_baudrate);
        if (status_dbc == nxSuccess)
        {
            qDebug()<<"Add XCP DBC Alias Succeed!"<<dbcName;
        }
        else
        {
            qDebug()<<"Add XCP DBC Alias Error."<<status_dbc<<"\n";
            char statusString[1024];
            nxStatusToString(status_dbc, sizeof(statusString), statusString);
            qDebug()<<statusString;
        }

        /*===================Open database======================*/
        nxDatabaseRef_t databaseRef;
        status_dbc = nxdbOpenDatabase(dbcName, &databaseRef);
        if (status_dbc == nxSuccess)
        {
            qDebug()<<"Open Database Succeed!";
        }
        /*===================Get ClusterRef and Name======================*/
        u32 numOfClusterRef;
        u32 sizeOfClusterRef;
        nxDatabaseRef_t *clusterRef;
        status_dbc = nxdbGetPropertySize(databaseRef, nxPropDatabase_ClstRefs, &sizeOfClusterRef);
        numOfClusterRef = sizeOfClusterRef/sizeof(nxDatabaseRef_t);
        clusterRef = new nxDatabaseRef_t[numOfClusterRef];
        status_dbc = nxdbGetProperty(databaseRef, nxPropDatabase_ClstRefs, sizeOfClusterRef, clusterRef);
        if (status_dbc == nxSuccess)
        {
            qDebug()<<"Get Cluster Ref Succeed! ClusterRef = "<<clusterRef[0];
        }

        char* clusterName;
        u32 sizeOfClusterName;
        QString sel_cluster;
        status_dbc = nxdbGetPropertySize(clusterRef[0], nxPropClst_Name, &sizeOfClusterName);
        clusterName = new char[sizeOfClusterName];
        status_dbc = nxdbGetProperty(clusterRef[0], nxPropClst_Name, sizeOfClusterName, clusterName);
        if (status_dbc == nxSuccess)
        {
            qDebug()<<"Get Cluster Name Succeed! ClusterName = "<<clusterName;
            sel_cluster = QString(QLatin1String(clusterName));
            xcp_Cluster = sel_cluster;
        }
        else
        {
            qDebug()<<"Get Cluster Name Error."<<status_dbc<<"\n";
            char statusString[1024];
            nxStatusToString(status_dbc, sizeof(statusString), statusString);
            qDebug()<<statusString;
        }

        /*===================Set can io mode======================*/
        status_dbc = nxdbSetProperty(clusterRef[0], nxPropClst_CanIoMode, sizeof(u32), &canIoMode);
        if (status_dbc == nxSuccess)
        {
            qDebug()<<"Cluster set can io mode ok! can_io_mode:"<<canIoMode;
        }
        else
        {
            qDebug()<<"Cluster set can io mode error! ";
        }

        /*===================Set Baudrate======================*/
        status_dbc = nxdbSetProperty(clusterRef[0], nxPropClst_BaudRate64, sizeof(u64), &sel_baudrate);
        if (status_dbc == nxSuccess)
        {
            qDebug()<<"Cluster set baudrate ok! "<<sel_baudrate;
        }
        else
        {
            qDebug()<<"Cluster set baudrate error! ";
        }

        if(isCanFd)
        {

            /*===================Set FD Baudrate======================*/
            status_dbc = nxdbSetProperty(clusterRef[0], nxPropClst_CanFdBaudRate64, sizeof(u64), &sel_fdBaudrate);
            if (status_dbc == nxSuccess)
            {
                qDebug()<<"Cluster set fd baudrate ok! "<<sel_fdBaudrate;
            }
            else
            {
                qDebug()<<"Cluster set fd baudrate error! ";
            }

            /*===================Set ISO Mode======================*/
            status_dbc = nxdbSetProperty(clusterRef[0], nxPropClst_CanFdIsoMode, sizeof(u32), &fdIsoMode);
            if (status_dbc == nxSuccess)
            {
                qDebug()<<"Cluster set fd ISO mode ok! "<<fdIsoMode;
            }
            else
            {
                qDebug()<<"Cluster set fd ISO mode error! ";
            }
        }

        nxDatabaseRef_t *frameRef;
        u32 numOfFrameRef;
        u32 sizeOfFrameRef;
        status_dbc = nxdbGetPropertySize(clusterRef[0], nxPropClst_FrmRefs, &sizeOfFrameRef);
        qDebug()<<"Size of Frame Ref = "<<sizeOfFrameRef;
        numOfFrameRef = sizeOfFrameRef/sizeof(nxDatabaseRef_t);
        frameRef = new nxDatabaseRef_t[numOfFrameRef];

        status_dbc = nxdbGetProperty(clusterRef[0], nxPropClst_FrmRefs, sizeOfFrameRef, frameRef);
        if (status_dbc == nxSuccess)
        {
            qDebug()<<"Get Frame Ref Succeed!";
        }

        char* frameName;
        u32 sizeOfFrameName;
        u32 frameId;
        QStringList frameList;
        FrameText_EVENT = "";
        for (int i = 0; i < numOfFrameRef; i++)
        {
            QString frameText, frameIdText;
            u32 payLoadLength = 0;
            u32 frameCanIoMode = 0;
            u32 frameSetCanIoMode = canIoMode > 0 ? 1 : 0;
            boolean idExtented;
            u32 timingType;
            double transmitTime;
            dbc_frame cur_frame;

            nxdbSetProperty(frameRef[i], nxPropFrm_PayloadLen, sizeof(u32), &maxDlc);
            nxdbSetProperty(frameRef[i], nxPropFrm_CANioMode, sizeof(u32), &frameSetCanIoMode);

            status_dbc = nxdbGetPropertySize(frameRef[i], nxPropFrm_Name, &sizeOfFrameName);

            frameName = new char[sizeOfFrameName];
            status_dbc = nxdbGetProperty(frameRef[i], nxPropFrm_Name, sizeOfFrameName, frameName);
            status_dbc = nxdbGetProperty(frameRef[i], nxPropFrm_ID, sizeof(u32), &frameId);
            status_dbc = nxdbGetProperty(frameRef[i], nxPropFrm_CANExtID, sizeof(boolean), &idExtented);
            status_dbc = nxdbGetProperty(frameRef[i], nxPropFrm_CANTimingType, sizeof(u32), &timingType);
            status_dbc = nxdbGetProperty(frameRef[i], nxPropFrm_CANTxTime, sizeof(double), &transmitTime);
            status_dbc = nxdbGetProperty(frameRef[i], nxPropFrm_PayloadLen, sizeof(u32), &payLoadLength);
            status_dbc = nxdbGetProperty(frameRef[i], nxPropFrm_CANioMode, sizeof(u32), &frameCanIoMode);
            if (status_dbc == nxSuccess)
            {
                //qDebug()<<"No."<<i<<"Get Frame Name : "<<frameName<<", ID : "<<frameIdText;
                frameText = QString(QLatin1String(frameName));
                frameList << frameText;
                frameIdText = "0x"+QString::number(frameId, 16).toUpper();
                qDebug()<<"No."<<i<<"Get Frame Name : "<<frameName<<", ID : "<<frameIdText<<", dlc:"<<payLoadLength<<", frameCanIoMode:"<<frameCanIoMode;

                cur_frame.m_name = frameText;
                cur_frame.m_dlc = payLoadLength;
                cur_frame.m_id = frameId;
                cur_frame.m_extented = idExtented;
                cur_frame.m_timingType = timingType;
                cur_frame.m_cyclicTime = transmitTime*1000;
            }

            if (frameText == "CMD")
            {
                if(ID_Master_to_Slave > 0x7FF)
                {
                    bool canExtendedId = true;
                    status_dbc = nxdbSetProperty(frameRef[i], nxPropFrm_CANExtID, sizeof(u8), &canExtendedId);
                    qDebug()<<"Set CMD ID extented: "<<status_dbc;
                }
                else
                {
                    bool canExtendedId = false;
                    status_dbc = nxdbSetProperty(frameRef[i], nxPropFrm_CANExtID, sizeof(bool), &canExtendedId);
                }

                status_dbc = nxdbSetProperty(frameRef[i], nxPropFrm_ID, sizeof(u32), &ID_Master_to_Slave);
                FrameText_CMD = frameText;
            }
            else if (frameText == "RES")
            {
                if(ID_Master_to_Slave > 0x7FF)
                {
                    bool canExtendedId = true;
                    status_dbc = nxdbSetProperty(frameRef[i], nxPropFrm_CANExtID, sizeof(bool), &canExtendedId);
                    qDebug()<<"Set RES ID extented: "<<status_dbc;
                }
                else
                {
                    bool canExtendedId = false;
                    status_dbc = nxdbSetProperty(frameRef[i], nxPropFrm_CANExtID, sizeof(bool), &canExtendedId);
                }

                status_dbc = nxdbSetProperty(frameRef[i], nxPropFrm_ID, sizeof(u32), &ID_Slave_to_Master);
                FrameText_RES = frameText;
            }            
            else if (frameText.contains("EVENT"))
            {
                quint8 eventIndex = (quint8)frameText.right(1).toInt();
                A2L_EventCanId *eventCanId = findEventCanIdByEventNumber(eventIndex);
                if(eventCanId)
                {
                    quint32 id = eventCanId->Event_CAN_ID;

                    if(id > 0x7FF)
                    {
                        bool canExtendedId = true;
                        status_dbc = nxdbSetProperty(frameRef[i], nxPropFrm_CANExtID, sizeof(bool), &canExtendedId);
                        qDebug()<<"Set CMD ID extented: "<<status_dbc;
                    }
                    else
                    {
                        bool canExtendedId = false;
                        status_dbc = nxdbSetProperty(frameRef[i], nxPropFrm_CANExtID, sizeof(bool), &canExtendedId);
                    }

                    status_dbc = nxdbSetProperty(frameRef[i], nxPropFrm_ID, sizeof(u32), &id);
                    FrameText_EVENT += (frameText + ",");
                }
                else
                {
                    quint32 id = 0;
                    status_dbc = nxdbSetProperty(frameRef[i], nxPropFrm_ID, sizeof(u32), &id);
                }
            }

            status_dbc = nxdbGetProperty(frameRef[i], nxPropFrm_ID, sizeof(u32), &frameId);
            qDebug()<<frameName<<" current ID: 0x"<<QString::number(frameId, 16).toUpper();

        }
        int lastIndex = FrameText_EVENT.length() - 1;
        FrameText_EVENT.remove(lastIndex, 1);
        qDebug()<<"Event frames:"<<FrameText_EVENT;

        status_dbc = nxdbSaveDatabase(databaseRef, dbcPath);
        status_dbc = nxdbCloseDatabase(databaseRef, 1);
        if (status_dbc == nxSuccess)
        {
            qDebug()<<"Close database all ref Succeed!";
        }
    }
}

void XCPMaster::xcpdbcRemove()
{
    QString filePath = xcpdbc_Path;
    QString dbcFilePath = filePath;
    dbcFilePath.replace(".xml", ".dbc");

    QFile dbcFile(dbcFilePath);
    if(dbcFile.exists())
    {
        QByteArray temp_path = filePath.toLocal8Bit();
        const char *dbcPath = temp_path.data();

        QString name;
        QStringList fileList = filePath.split("/", QString::SkipEmptyParts);
        name = fileList.at(fileList.count()-1);
        name = name.split(".", QString::SkipEmptyParts)[0];
        QByteArray temp_name = name.toLocal8Bit();
        char *dbcName = temp_name.data();

        nxStatus_t status_dbc;

        status_dbc = nxdbRemoveAlias(dbcName);
        if (status_dbc == nxSuccess)
        {
            qDebug()<<"Remove XCP DBC Alias Succeed!"<<dbcName;
        }
        else
        {
            qDebug()<<"Remove XCP DBC Alias Error."<<status_dbc<<"\n";
            char statusString[1024];
            nxStatusToString(status_dbc, sizeof(statusString), statusString);
            qDebug()<<statusString;
        }
    }

    QFileInfo fileInfo(filePath);
    if(fileInfo.isFile())
    {
        QFile::remove(filePath);
    }
    QFileInfo dbcFileInfo(dbcFilePath);
    if(dbcFileInfo.isFile())
    {
        QFile::remove(dbcFilePath);
    }

}

void XCPMaster::reverse(char *data, int size)
{
    if(size <= 1)
        return;

    char temp;
    int start = 0;
    int end = size-1;
    while(start < end)
    {
        temp = *(data+start);
        *(data+start) = *(data+end);
        *(data+end) = temp;
        start++;
        end--;
    }
}

bool XCPMaster::CTO_CMD_RES_Comm()
{
    if (!xcpCanInitFlag)
        return false;

    QTime time;
    showDataInHex(CTO_CMDPacket, 8);

    if (xcpDeviceType == 0) // NI XNET
    {
        if(!isCanFd)
        {
            xcpCanThread->thread_CMD->setCMDPayLoad(CTO_CMDPacket, 8);
        }
        else
        {
            xcpCanThread->thread_CMD->setCMDPayLoad(CTO_CMDPacket, 64);
        }
        //xcpCanThread->thread_CMD->setCMDPayLoad(CTO_CMDPacket, 8);
        CTORESDataUpdated = false;
        xcpCanThread->thread_CMD->setWriteOnceEnable(true);
        time.start();

        //以下2条代码可以模拟所有报文均正确应答后，可用以调试发送命令
        //CTO_RESPacket[0] = 0xFF;
        //return true;

        while ((!xcpCanThread->thread_CMD->writeOnceSucceed()) || (!CTORESDataUpdated))
        {
            QCoreApplication::processEvents(QEventLoop::AllEvents, 100);
            if (time.elapsed() > 1000)
            {
                qDebug()<<"CTO timeout > 1000ms";
                return false;
            }
        }
        xcpCanThread->thread_RES->setResDataReady(false);

    }
    else if (xcpDeviceType == 1) // TSCAN同星
    {
        if(!isCanFd)
        {
            xcpTsCanThread->thread_CMD->setCMDPayLoad(CTO_CMDPacket, 8);
        }
        else
        {
            xcpTsCanThread->thread_CMD->setCMDPayLoad(CTO_CMDPacket, 64);
        }
        //xcpTsCanThread->thread_CMD->setCMDPayLoad(CTO_CMDPacket, 8);

        CTORESDataUpdated = false;
        xcpTsCanThread->thread_CMD->setWriteOnceEnable(true);
        time.start();

        //以下2条代码可以模拟所有报文均正确应答后，可用以调试发送命令
        //CTO_RESPacket[0] = 0xFF;
        //return true;

        while ((!xcpTsCanThread->thread_CMD->writeOnceSucceed()) || (!CTORESDataUpdated))
        {
            QCoreApplication::processEvents(QEventLoop::AllEvents, 100);
            if (time.elapsed() > 1000)
            {
                qDebug()<<"CTO timeout > 1000ms, TSCAN";
                return false;
            }
        }
        xcpTsCanThread->thread_RES->setResDataReady(false);

    }
    else if (xcpDeviceType == 2) // ZLG_CAN
    {
        if(!isCanFd)
        {
            xcpZlgCanThread->thread_CMD->setCMDPayLoad(CTO_CMDPacket, 8);
        }
        else
        {
            xcpZlgCanThread->thread_CMD->setCMDPayLoad(CTO_CMDPacket, 64);
        }
        //xcpZlgCanThread->thread_CMD->setCMDPayLoad(CTO_CMDPacket, 8);
        CTORESDataUpdated = false;
        xcpZlgCanThread->thread_CMD->setWriteOnceEnable(true);
        time.start();

        //以下2条代码可以模拟所有报文均正确应答后，可用以调试发送命令
        //CTO_RESPacket[0] = 0xFF;
        //return true;

        while ((!xcpZlgCanThread->thread_CMD->writeOnceSucceed()) || (!CTORESDataUpdated))
        {
            QCoreApplication::processEvents(QEventLoop::AllEvents, 100);
            if (time.elapsed() > 1000)
            {
                qDebug()<<"CTO timeout > 1000ms, ZLG_CAN";
                return false;
            }
        }
        xcpZlgCanThread->thread_RES->setResDataReady(false);

    }

    return true;
}

void XCPMaster::CTO_ClearData()
{
    for (int i = 0; i < 64; i++)
    {
        CTO_CMDPacket[i] = 0x00;
        CTO_RESPacket[i] = 0x00;
    }
}

void XCPMaster::showDataInHex(quint8 *data, quint16 size)
{
    if(data == NULL || size == 0)
        return;
    QString hexStr = "";
    for(int i = 0; i < size; i++)
    {
        hexStr += QString::number(*(data+i), 16);
        if(i == size-1)
            continue;
        hexStr += " ";
    }
    //qDebug()<<hexStr.toUpper();
}

bool XCPMaster::CTO_CONNECT()
{
    CTO_ClearData();

    CTO_CMDPacket[0] = CONNECT;
    CTO_CMDPacket[1] = 0x00;
    if (!CTO_CMD_RES_Comm())
    {
        qDebug()<<"CONNECT CTO Comm error";
        return false;
    }

    if (CTO_RESPacket[0] == 0xFF)
    {
        qDebug()<<"CONNECT OK";

        quint8 RESOURCE = CTO_RESPacket[1];
        bool CAL_PAG = RESOURCE & 0x01;
        bool DAQ = (RESOURCE >> 2) & 0x01;
        bool STIM = (RESOURCE >> 3) & 0x01;
        bool PGM = (RESOURCE >> 4) & 0x01;

        quint8 COMM_MODE_BASIC = CTO_RESPacket[2];
        quint8 BYTE_ORDER = COMM_MODE_BASIC & 0x01;
        ByteOrder_XCPAddr = BYTE_ORDER;
        QString byteOrderStr = BYTE_ORDER?"MOTOROLA":"INTEL";
        quint8 ADDRESS_GRANULARITY = (COMM_MODE_BASIC >> 1) & 0x03;
        QString sizeAddress;
        if (ADDRESS_GRANULARITY == 0)
            sizeAddress = "BYTE";
        else if (ADDRESS_GRANULARITY == 1)
            sizeAddress = "WORD";
        else if (ADDRESS_GRANULARITY == 2)
            sizeAddress = "DWORD";
        bool SLAVE_BLOCK_MODE = (COMM_MODE_BASIC >> 6) & 0x01;
        bool OPTIONAL_RES_CONNECT = (COMM_MODE_BASIC >> 7) & 0x01;

        quint8 MAX_CTO = CTO_RESPacket[3];
        quint16 MAX_DTO = MAKEWORD(CTO_RESPacket[4], CTO_RESPacket[5]);
        quint8 ProtocolLayerVersion = CTO_RESPacket[6];
        quint8 TransportLayerVersion = CTO_RESPacket[7];

        qDebug()<<"CONNECT Response OK!";
        qDebug()<<"CAL_PAG="<<CAL_PAG<<", DAQ="<<DAQ<<",STIM="<<STIM<<",PGM="<<PGM;
        qDebug()<<"BYTE_ORDER="<<byteOrderStr<<",ADDRESS_GRANULARITY="<<sizeAddress;
        qDebug()<<"MAX_CTO="<<MAX_CTO<<",MAX_DTO="<<MAX_DTO;
        qDebug()<<"===================================";

        return true;
    }
    else if (CTO_RESPacket[0] == 0xFE)
    {
        qDebug()<<"CONNECT Error."<<"ErrorCode=0x"<<QString::number(CTO_RESPacket[1], 16);
        return false;
    }

}

bool XCPMaster::CTO_DISCONNECT()
{
    CTO_ClearData();
    XCPConnected = false;
    CTO_CMDPacket[0] = DISCONNECT;
    if (!CTO_CMD_RES_Comm())
    {
        qDebug()<<"DISCONNECT CTO Comm error";
        return false;
    }
    if (CTO_RESPacket[0] == 0xFF)
    {
        qDebug()<<"DISCONNECT Response OK";
        qDebug()<<"===================================";

        return true;
    }
    else if (CTO_RESPacket[0] == 0xFE)
    {
        qDebug()<<"DISCONNECT Error."<<"ErrorCode=0x"<<QString::number(CTO_RESPacket[1], 16);

        return false;
    }
}

bool XCPMaster::CTO_GET_STATUS()
{
    CTO_ClearData();

    CTO_CMDPacket[0] = GET_STATUS;
    if (!CTO_CMD_RES_Comm())
    {
        qDebug()<<"CTO Comm error";
        return false;
    }
    if (CTO_RESPacket[0] == 0xFF)
    {
        qDebug()<<"GET_STATUS OK";

        quint8 CurSessStatus = CTO_RESPacket[1];
        bool STORE_CAL_REQ = CurSessStatus & 0x01;
        bool STORE_DAQ_REQ = (CurSessStatus >> 2) & 0x01;
        bool CLEAR_DAQ_REQ = (CurSessStatus >> 3) & 0x01;
        DAQ_Running_GETSTATUS = (CurSessStatus >> 6) & 0x01;
        bool RESUME = (CurSessStatus >> 7) & 0x01;

        quint8 CurResoureProtectStatus = CTO_RESPacket[2];
        CAL_PAG_LOCK_STATUS = CurResoureProtectStatus & 0x01;
        DAQ_LOCK_STATUS = (CurResoureProtectStatus >> 2) & 0x01;
        STIM_LOCK_STATUS = (CurResoureProtectStatus >> 3) & 0x01;
        PGM_LOCK_STATUS = (CurResoureProtectStatus >> 4) & 0x01;

        quint16 SessionConfigId = MAKEWORD(CTO_RESPacket[4], CTO_RESPacket[5]);
        qDebug()<<"GET_STATUS Response OK!";
        qDebug()<<"STORE_CAL_REQ="<<STORE_CAL_REQ<<", STORE_DAQ_REQ="<<STORE_DAQ_REQ<<",CLEAR_DAQ_REQ="<<CLEAR_DAQ_REQ<<",DAQ_Running_GETSTATUS="<<DAQ_Running_GETSTATUS;
        qDebug()<<"CAL_PAG_LOCK ="<<CAL_PAG_LOCK_STATUS<<",DAQ_LOCK = "<<DAQ_LOCK_STATUS<<"STIM_LOCK = "<<STIM_LOCK_STATUS<<",PGM_LOCK = "<<PGM_LOCK_STATUS;
        qDebug()<<"SessionConfigId="<<SessionConfigId;
        qDebug()<<"===================================";

        return true;
    }
    else if (CTO_RESPacket[0] == 0xFE)
    {
        qDebug()<<"GET_STATUS Error."<<"ErrorCode=0x"<<QString::number(CTO_RESPacket[1], 16);
        return false;
    }
}

bool XCPMaster::CTO_GET_SEED()
{
    CTO_ClearData();

    CTO_CMDPacket[0] = GET_SEED;
    CTO_CMDPacket[1] = SEED_MODE;
    CTO_CMDPacket[2] = SEED_RESOURCE;
    if (!CTO_CMD_RES_Comm())
    {
        qDebug()<<"CTO Comm error";
        return false;
    }
    if (CTO_RESPacket[0] == 0xFF)
    {
        qDebug()<<"GET_SEED OK";

        LEN_SEED = CTO_RESPacket[1];
        memcpy(data_SEED, CTO_RESPacket+2, LEN_SEED);

        qDebug()<<"GET_SEED Response OK!";
        qDebug()<<"Len of seed="<<LEN_SEED;
        qDebug()<<"Seed="
                <<QString::number(data_SEED[0], 16)<<"-"
                <<QString::number(data_SEED[1], 16)<<"-"
                <<QString::number(data_SEED[2], 16)<<"-"
                <<QString::number(data_SEED[3], 16)<<"-"
                <<QString::number(data_SEED[4], 16)<<"-"
                <<QString::number(data_SEED[5], 16)<<"-";
        qDebug()<<"===================================";

        return true;
    }
    else if (CTO_RESPacket[0] == 0xFE)
    {
        qDebug()<<"GET_SEED Error."<<"ErrorCode=0x"<<QString::number(CTO_RESPacket[1], 16);
        return false;
    }

    return false;
}

void XCPMaster::setUseSeedKey(bool value)
{
    useSeedKey = value;
}

void XCPMaster::setSeedKeyDllPath(const QString &value)
{
    seedKeyDllPath = value;
}

bool XCPMaster::CTO_UNLOCK()
{
    CTO_ClearData();

    CTO_CMDPacket[0] = UNLOCK;
    CTO_CMDPacket[1] = LEN_KEY;
    memcpy(CTO_CMDPacket+2, data_KEY, LEN_KEY);
    if (!CTO_CMD_RES_Comm())
    {
        qDebug()<<"CTO Comm error";
        return false;
    }
    if (CTO_RESPacket[0] == 0xFF)
    {
        qDebug()<<"UNLOCK OK";

        quint8 CurResoureProtectStatus = CTO_RESPacket[1];
        CAL_PAG_PRO_STATUS = CurResoureProtectStatus & 0x01;
        DAQ_PRO_STATUS = (CurResoureProtectStatus >> 2) & 0x01;
        STIM_PRO_STATUS = (CurResoureProtectStatus >> 3) & 0x01;
        PGM_PRO_STATUS = (CurResoureProtectStatus >> 4) & 0x01;

        qDebug()<<"UNLOCK Response OK!";
        qDebug()<<"CAL_PAG_PRO ="<<CAL_PAG_PRO_STATUS<<",DAQ_PRO = "<<DAQ_PRO_STATUS
                <<"STIM_PRO = "<<STIM_PRO_STATUS<<",PGM_PRO = "<<PGM_PRO_STATUS;
        qDebug()<<"===================================";

        return true;
    }
    else if (CTO_RESPacket[0] == 0xFE)
    {
        qDebug()<<"UNLOCK Error."<<"ErrorCode=0x"<<QString::number(CTO_RESPacket[1], 16);
        return false;
    }

    return false;
}

typedef qint16 (*XCP_ComputeKeyFromSeed)(quint8, quint8, quint8*, quint8*, quint8*);

qint16 XCPMaster::XCP_Seed2Key(QString dllPath, quint8 privilege, quint8 lenOfSeed, quint8 *seed, quint8 *lenOfKey, quint8 *key)
{
    if(dllPath.isEmpty()) return -1;
    if(!lenOfSeed || !seed || !lenOfKey || !key) return -1;

    QLibrary lib(dllPath);

    if(lib.load())
    {
        //qDebug()<<"load XCP SeedKey dll ok.";

        XCP_ComputeKeyFromSeed seed2Key = (XCP_ComputeKeyFromSeed)lib.resolve("XCP_ComputeKeyFromSeed");

        if(seed2Key)
        {
            //qDebug()<<"resolve XCP SeedKey ok";

            return seed2Key(privilege, lenOfSeed, seed, lenOfKey, key);
        }
        else
        {
            qDebug()<<"resolve XCP SeedKey error.";
            qDebug()<<lib.errorString();
        }
    }
    else
    {
        qDebug()<<"load XCP SeedKey error:" + lib.errorString();
    }
}

bool XCPMaster::CTO_SYNCH()
{
    CTO_ClearData();

    CTO_CMDPacket[0] = SYNCH;
    if (!CTO_CMD_RES_Comm())
    {
        qDebug()<<"CTO Comm error";
        return false;
    }
    if (CTO_RESPacket[0] == 0xFF)
    {
        qDebug()<<"SYNCH Response OK";
        qDebug()<<"===================================";
        return true;
    }
    else if (CTO_RESPacket[0] == 0xFE)
    {
        qDebug()<<"SYNCH Error."<<"ErrorCode=0x"<<QString::number(CTO_RESPacket[1], 16);
        return false;
    }
}

bool XCPMaster::CTO_GET_COMM_MODE_INFO()
{
    CTO_ClearData();

    CTO_CMDPacket[0] = GET_COMM_MODE_INFO;
    if (!CTO_CMD_RES_Comm())
    {
        qDebug()<<"CTO Comm error";
        return false;
    }
    if (CTO_RESPacket[0] == 0xFF)
    {
        qDebug()<<"GET_COMM_MODE_INFO OK";

        quint8 CommModeOptional = CTO_RESPacket[2];
        bool MASTER_BLOCK_MODE = CommModeOptional & 0x01;
        bool INTERLEAVED_MODE = (CommModeOptional >> 1) & 0x01;

        quint8 MAX_BS = CTO_RESPacket[4];
        quint8 MIN_ST = CTO_RESPacket[5];
        quint8 QUEUE_SIZE = CTO_RESPacket[6];
        quint8 XCPDriverVersion = CTO_RESPacket[7];

        qDebug()<<"GET_COMM_MODE_INFO Response OK!";
        qDebug()<<"MASTER_BLOCK_MODE="<<MASTER_BLOCK_MODE<<", INTERLEAVED_MODE="<<INTERLEAVED_MODE;
        qDebug()<<"MAX_BS="<<MAX_BS<<",MIN_ST="<<MIN_ST<<"QUEUE_SIZE="<<QUEUE_SIZE;
        qDebug()<<"XCPDriverVersion="<<XCPDriverVersion;
        qDebug()<<"===================================";

        return true;
    }
    else if (CTO_RESPacket[0] == 0xFE)
    {
        qDebug()<<"GET_COMM_MODE_INFO Error."<<"ErrorCode=0x"<<QString::number(CTO_RESPacket[1], 16);
        return false;
    }
}

bool XCPMaster::CTO_GET_ID()
{
    CTO_ClearData();

    CTO_CMDPacket[0] = 0xFA;  // GET_ID 与zlg canframe里的关键字重复,所以用原始数值
    CTO_CMDPacket[1] = 0x01;
    if (!CTO_CMD_RES_Comm())
    {
        qDebug()<<"CTO Comm error";
        return false;
    }
    if (CTO_RESPacket[0] == 0xFF)
    {
        qDebug()<<"GET_ID OK";

        quint8 Mode = CTO_RESPacket[1];

        length_GET_ID = MAKELONG(MAKEWORD(CTO_RESPacket[4], CTO_RESPacket[5]), MAKEWORD(CTO_RESPacket[6], CTO_RESPacket[7]));
        qDebug()<<"GET_ID Response OK!";
        qDebug()<<"Mode="<<Mode;
        qDebug()<<"Length="<<length_GET_ID;
        qDebug()<<"===================================";

        return true;
    }
    else if (CTO_RESPacket[0] == 0xFE)
    {
        qDebug()<<"GET_ID Error."<<"ErrorCode=0x"<<QString::number(CTO_RESPacket[1], 16);
        return false;
    }
}

bool XCPMaster::CTO_SET_MTA()
{
    CTO_ClearData();
    //qDebug()<<"In SET_MTA 0";

    CTO_CMDPacket[0] = SET_MTA;
    CTO_CMDPacket[3] = MTA_Address_Extension;
    if(ByteOrder_XCPAddr == 0)
    {
        memcpy(CTO_CMDPacket+4, (quint8*)(&MTA_Address), 4);
    }
    else if(ByteOrder_XCPAddr == 1)
    {
        quint32 MTA_Address_Motorola = MTA_Address;

        fromIntelToMoto(&MTA_Address_Motorola);

        memcpy(CTO_CMDPacket+4, (quint8*)(&MTA_Address_Motorola), 4);
    }
    //memcpy(CTO_CMDPacket+4, (quint8*)(&MTA_Address), sizeof(typeof(MTA_Address)));

    //qDebug()<<"In SET_MTA 1";
    if (!CTO_CMD_RES_Comm())
    {
        qDebug()<<"SET_MTA CTO Comm error";
        return false;
    }
    if (CTO_RESPacket[0] == 0xFF)
    {
        //qDebug()<<"SET_MTA OK";
        //qDebug()<<"===================================";
        return true;
    }
    else if (CTO_RESPacket[0] == 0xFE)
    {
        qDebug()<<"SET_MTA Error."<<"ErrorCode=0x"<<QString::number(CTO_RESPacket[1], 16);
        return false;
    }
}

void XCPMaster::fromIntelToMoto(quint32 *address)
{
    quint8 temp = 0;
    int count = sizeof(address);
    for(int i = 0; i < count/2; i++)
    {
        temp = *((quint8*)address+i);
        *((quint8*)address+i) = *((quint8*)address+count-1-i);
        *((quint8*)address+count-1-i) = temp;
    }
}

bool XCPMaster::CTO_UPLOAD()
{
    CTO_ClearData();

    CTO_CMDPacket[0] = UPLOAD;
    CTO_CMDPacket[1] = numOfDataAG_UPLOAD;
    if (!CTO_CMD_RES_Comm())
    {
        qDebug()<<"UPLOAD CTO Comm error";
        return false;
    }
    if (CTO_RESPacket[0] == 0xFF)
    {
        //qDebug()<<"UPLOAD OK";

        //char *dataAscii = new char[numOfDataAG_UPLOAD];
        //memcpy(dataAscii, (char*)(CTO_RESPacket+1), numOfDataAG_UPLOAD);
        memcpy(data_UPLOAD, CTO_RESPacket+1, numOfDataAG_UPLOAD);

        //qDebug()<<"UPLOAD Response OK!";
        //qDebug()<<"data="<<QString(dataAscii);
        //qDebug()<<"===================================";

        return true;
    }
    else if (CTO_RESPacket[0] == 0xFE)
    {
        qDebug()<<"UPLOAD Error."<<"ErrorCode=0x"<<QString::number(CTO_RESPacket[1], 16);
        return false;
    }
}

bool XCPMaster::CTO_SHORT_UPLOAD()
{
    CTO_ClearData();

    CTO_CMDPacket[0] = SHORT_UPLOAD;
    quint8 numOfDataAG = 0;
    CTO_CMDPacket[1] = numOfDataAG;
    MTA_Address = 0x00000000;
    MTA_Address_Extension = 0x00;
    CTO_CMDPacket[3] = MTA_Address_Extension;
    *(CTO_CMDPacket+4) = MTA_Address;
    if (!CTO_CMD_RES_Comm())
    {
        qDebug()<<"CTO Comm error";
        return false;
    }
    if (CTO_RESPacket[0] == 0xFF)
    {
        qDebug()<<"SHORT_UPLOAD OK";

        //char *dataAscii = new char[numOfDataAG];
        //memcpy(dataAscii, (char*)(CTO_RESPacket+1), numOfDataAG);

        //qDebug()<<"data="<<QString(dataAscii);

        return true;
    }
    else if (CTO_RESPacket[0] == 0xFE)
    {
        qDebug()<<"SHORT_UPLOAD Error."<<"ErrorCode=0x"<<QString::number(CTO_RESPacket[1], 16);
        return false;
    }
}

bool XCPMaster::CTO_BUILD_CHECKSUM()
{
    CTO_ClearData();

    CTO_CMDPacket[0] = BUILD_CHECKSUM;
    quint32 BlockSize = 0x400;
    *(CTO_CMDPacket+4) = BlockSize;

    if(ByteOrder_XCPAddr == 0)
    {
        memcpy(CTO_CMDPacket+4, (quint8*)(&BlockSize), 4);
    }
    else if(ByteOrder_XCPAddr == 1)
    {
        quint32 BlockSize_Motorola = BlockSize;

        fromIntelToMoto(&BlockSize_Motorola);

        memcpy(CTO_CMDPacket+4, (quint8*)(&BlockSize_Motorola), 4);
    }

    if (!CTO_CMD_RES_Comm())
    {
        qDebug()<<"CTO Comm error";
        return false;
    }
    if (CTO_RESPacket[0] == 0xFF)
    {
        qDebug()<<"BUILD_CHECKSUM OK";

        quint8 checksumType = CTO_RESPacket[1];
        quint32 checksum = MAKELONG(MAKEWORD(CTO_RESPacket[4], CTO_RESPacket[5]), MAKEWORD(CTO_RESPacket[6], CTO_RESPacket[7]));

        qDebug()<<"BUILD_CHECKSUM Response OK!";
        qDebug()<<"checksumType="<<checksumType;
        qDebug()<<"checksum="<<checksum;
        qDebug()<<"===================================";

        return true;
    }
    else if (CTO_RESPacket[0] == 0xFE)
    {
        qDebug()<<"BUILD_CHECKSUM Error."<<"ErrorCode=0x"<<QString::number(CTO_RESPacket[1], 16);
        return false;
    }

    return true;
}

bool XCPMaster::CTO_DOWNLOAD()
{
    CTO_ClearData();

    CTO_CMDPacket[0] = DOWNLOAD;
    CTO_CMDPacket[1] = numOfDataAG_DOWNLOAD;
    //qDebug()<<"numofData Download = "<<numOfDataAG_DOWNLOAD;

    memcpy(CTO_CMDPacket+2, data_DOWNLOAD, numOfDataAG_DOWNLOAD);

    if (!CTO_CMD_RES_Comm())
    {
        qDebug()<<"CTO Comm error";
        return false;
    }
    if (CTO_RESPacket[0] == 0xFF)
    {
        //qDebug()<<"DOWNLOAD OK";
        //qDebug()<<"===================================";
        return true;
    }
    else if (CTO_RESPacket[0] == 0xFE)
    {
        qDebug()<<"DOWNLOAD Error."<<"ErrorCode=0x"<<QString::number(CTO_RESPacket[1], 16);
        return false;
    }
}

void XCPMaster::ClearDataDownload()
{
    for (int i = 0; i < 6; i++)
    {
        data_DOWNLOAD[i] = 0x00;
    }
}

bool XCPMaster::CTO_SET_CAL_PAGE()
{
    CTO_ClearData();

    CTO_CMDPacket[0] = SET_CAL_PAGE;
    quint8 Mode = 0x83;
    CTO_CMDPacket[1] = Mode;
    //quint8 dataSegmentNumber = 0;
    CTO_CMDPacket[2] = curSegment;
    //quint8 dataPageNumber = 0;
    CTO_CMDPacket[3] = curPage;

    if (!CTO_CMD_RES_Comm())
    {
        qDebug()<<"CTO Comm error";
        return false;
    }
    if (CTO_RESPacket[0] == 0xFF)
    {
        qDebug()<<"SET_CAL_PAGE OK";
        qDebug()<<"===================================";
        return true;
    }
    else if (CTO_RESPacket[0] == 0xFE)
    {
        qDebug()<<"SET_CAL_PAGE Error."<<"ErrorCode=0x"<<QString::number(CTO_RESPacket[1], 16);
        return false;
    }
}

bool XCPMaster::CTO_GET_CAL_PAGE()
{
    CTO_ClearData();

    CTO_CMDPacket[0] = GET_CAL_PAGE;
    quint8 accessMode = 0x02;  // 0x01=ECU access, 0x02=XCP access
    CTO_CMDPacket[1] = accessMode;
    CTO_CMDPacket[2] = curSegment;

    if (!CTO_CMD_RES_Comm())
    {
        qDebug()<<"CTO Comm error";
        return false;
    }
    if (CTO_RESPacket[0] == 0xFF)
    {
        qDebug()<<"GET_CAL_PAGE OK";

        quint8 curActivePageNumber = CTO_RESPacket[3];
        qDebug()<<"GET_CAL_PAGE Response OK!";
        qDebug()<<"Current Segment"<<curSegment<<", Active Page Number="<<curActivePageNumber;
        qDebug()<<"===================================";

        return true;
    }
    else if (CTO_RESPacket[0] == 0xFE)
    {
        qDebug()<<"GET_CAL_PAGE Error."<<"ErrorCode=0x"<<QString::number(CTO_RESPacket[1], 16);
        return false;
    }
}

bool XCPMaster::CTO_GET_PAG_PROCESSOR_INFO()
{
    CTO_ClearData();

    CTO_CMDPacket[0] = GET_PAG_PROCESSOR_INFO;
    if (!CTO_CMD_RES_Comm())
    {
        qDebug()<<"CTO Comm error";
        return false;
    }
    if (CTO_RESPacket[0] == 0xFF)
    {
        qDebug()<<"GET_PAG_PROCESSOR_INFO OK";

        MAX_SEGMENT = CTO_RESPacket[1];
        quint8 PAGE_PROPERTIES = CTO_RESPacket[2];

        qDebug()<<"GET_PAG_PROCESSOR_INFO Response OK!";
        qDebug()<<"MAX_SEGMENT="<<MAX_SEGMENT;
        qDebug()<<"===================================";

        return true;
    }
    else if (CTO_RESPacket[0] == 0xFE)
    {
        qDebug()<<"GET_PAG_PROCESSOR_INFO Error."<<"ErrorCode=0x"<<QString::number(CTO_RESPacket[1], 16);
        MAX_SEGMENT = 0x02;
        return false;
    }
}

bool XCPMaster::CTO_CLEAR_DAQ_LIST()
{
    CTO_ClearData();

    CTO_CMDPacket[0] = CLEAR_DAQ_LIST;

    memcpy(CTO_CMDPacket+2, (quint8*)&DAQLISTNUMBER_CLEARDAQLIST, 2);

    if (!CTO_CMD_RES_Comm())
    {
        qDebug()<<"CTO Comm error";
        return false;
    }
    if (CTO_RESPacket[0] == 0xFF)
    {
        qDebug()<<"CLEAR_DAQ_LIST OK";

        qDebug()<<"CLEAR_DAQ_LIST Response OK!";
        qDebug()<<"===================================";

        return true;
    }
    else if (CTO_RESPacket[0] == 0xFE)
    {
        qDebug()<<"CLEAR_DAQ_LIST Error."<<"ErrorCode=0x"<<QString::number(CTO_RESPacket[1], 16);
        return false;
    }
}

bool XCPMaster::CTO_SET_DAQ_PTR()
{
    CTO_ClearData();

    CTO_CMDPacket[0] = SET_DAQ_PTR;

    memcpy(CTO_CMDPacket+2, (quint8*)&DAQLISTNUMBER_SETPTR, 2);

    CTO_CMDPacket[4] = ODTNUMBER_SETPTR;
    CTO_CMDPacket[5] = ODTENTRYNUMBER_SETPTR;

    if (!CTO_CMD_RES_Comm())
    {
        qDebug()<<"CTO Comm error";
        return false;
    }
    if (CTO_RESPacket[0] == 0xFF)
    {
        qDebug()<<"SET_DAQ_PTR OK";

        qDebug()<<"SET_DAQ_PTR Response OK!";
        qDebug()<<"===================================";

        return true;
    }
    else if (CTO_RESPacket[0] == 0xFE)
    {
        qDebug()<<"SET_DAQ_PTR Error."<<"ErrorCode=0x"<<QString::number(CTO_RESPacket[1], 16);
        return false;
    }
}

bool XCPMaster::CTO_WRITE_DAQ()
{
    CTO_ClearData();

    CTO_CMDPacket[0] = WRITE_DAQ;
    CTO_CMDPacket[1] = 0xFF;

    memcpy(CTO_CMDPacket+4, (quint8*)&ADDRESS_WRITEDAQ, sizeof(quint32));

    CTO_CMDPacket[2] = SIZE_WRITEDAQ;
    CTO_CMDPacket[3] = ADDRESSEXT_WRITEDAQ;

    if (!CTO_CMD_RES_Comm())
    {
        qDebug()<<"CTO Comm error";
        return false;
    }
    if (CTO_RESPacket[0] == 0xFF)
    {
        qDebug()<<"WRITE_DAQ OK";

        qDebug()<<"WRITE_DAQ Response OK!";
        qDebug()<<"===================================";

        return true;
    }
    else if (CTO_RESPacket[0] == 0xFE)
    {
        qDebug()<<"WRITE_DAQ Error."<<"ErrorCode=0x"<<QString::number(CTO_RESPacket[1], 16);
        return false;
    }

    return true;
}

bool XCPMaster::CTO_SET_DAQ_LIST_MODE()
{
    CTO_ClearData();

    CTO_CMDPacket[0] = SET_DAQ_LIST_MODE;
    quint8 Mode = 0x00;
    CTO_CMDPacket[1] = Mode;

    memcpy(CTO_CMDPacket+2, (quint8*)&DAQLISTNUMBER_SETDAQLISTMODE, sizeof(quint16));
    memcpy(CTO_CMDPacket+4, (quint8*)&EVENTCHANNEL_SETDAQLISTMODE, sizeof(quint16));

    CTO_CMDPacket[6] = TransmissionRatePrescaler;
    CTO_CMDPacket[7] = DAQListPriority;

    if (!CTO_CMD_RES_Comm())
    {
        qDebug()<<"CTO Comm error";
        return false;
    }
    if (CTO_RESPacket[0] == 0xFF)
    {
        qDebug()<<"SET_DAQ_LIST_MODE OK";

        qDebug()<<"SET_DAQ_LIST_MODE Response OK!";
        qDebug()<<"===================================";

        return true;
    }
    else if (CTO_RESPacket[0] == 0xFE)
    {
        qDebug()<<"SET_DAQ_LIST_MODE Error."<<"ErrorCode=0x"<<QString::number(CTO_RESPacket[1], 16);
        return false;
    }

    return true;
}

bool XCPMaster::CTO_GET_DAQ_LIST_MODE()
{
    CTO_ClearData();

    CTO_CMDPacket[0] = GET_DAQ_LIST_MODE;
    memcpy(CTO_CMDPacket+2, (quint8*)&DAQLISTNUMBER_GETDAQLISTMODE, sizeof(quint16));

    if (!CTO_CMD_RES_Comm())
    {
        qDebug()<<"CTO Comm error";
        return false;
    }
    if (CTO_RESPacket[0] == 0xFF)
    {
        qDebug()<<"GET_DAQ_LIST_MODE OK";

        quint8 curMode = CTO_RESPacket[1];
        quint16 curEventChannelNumber;
        memcpy((quint8*)&curEventChannelNumber, CTO_RESPacket+4, sizeof(quint16));
        quint8 curPrescaler = CTO_RESPacket[6];
        quint8 curDAQListPriority = CTO_RESPacket[7];

        qDebug()<<"GET_DAQ_LIST_MODE Response OK!";
        qDebug()<<"curMode"<<curMode<<", curEventChannelNumber="<<curEventChannelNumber
               <<"curPrescaler"<<curPrescaler<<", curDAQListPriority="<<curDAQListPriority;
        qDebug()<<"===================================";

        return true;
    }
    else if (CTO_RESPacket[0] == 0xFE)
    {
        qDebug()<<"GET_DAQ_LIST_MODE Error."<<"ErrorCode=0x"<<QString::number(CTO_RESPacket[1], 16);
        return false;
    }

    return true;
}

bool XCPMaster::CTO_START_STOP_DAQ_LIST()
{
    CTO_ClearData();

    CTO_CMDPacket[0] = START_STOP_DAQ_LIST;
    //quint8 Mode = 0x02;  //0x00-stop, 0x01-start, 0x02-select
    CTO_CMDPacket[1] = MODE_STARTSTOPDAQLIST;

    memcpy(CTO_CMDPacket+2, (quint8*)&DAQLISTNUMBER_STARTSTOP, sizeof(quint16));

    if (!CTO_CMD_RES_Comm())
    {
        qDebug()<<"CTO Comm error";
        return false;
    }
    if (CTO_RESPacket[0] == 0xFF)
    {
        qDebug()<<"START_STOP_DAQ_LIST OK";

        qDebug()<<"START_STOP_DAQ_LIST Response OK!";
        qDebug()<<"===================================";

        return true;
    }
    else if (CTO_RESPacket[0] == 0xFE)
    {
        qDebug()<<"START_STOP_DAQ_LIST Error."<<"ErrorCode=0x"<<QString::number(CTO_RESPacket[1], 16);
        return false;
    }
}

bool XCPMaster::CTO_START_STOP_SYNCH()
{
    CTO_ClearData();

    CTO_CMDPacket[0] = START_STOP_SYNCH;
    //quint8 Mode = 0x00;  //0x00-stop all, 0x01-start select, 0x02-stop select
    CTO_CMDPacket[1] = MODE_STARTSTOPSYNCH;

    if (!CTO_CMD_RES_Comm())
    {
        qDebug()<<"CTO Comm error";
        return false;
    }
    if (CTO_RESPacket[0] == 0xFF)
    {
        qDebug()<<"START_STOP_SYNCH OK";

        qDebug()<<"START_STOP_SYNCH Response OK!";
        qDebug()<<"===================================";

        return true;
    }
    else if (CTO_RESPacket[0] == 0xFE)
    {
        qDebug()<<"START_STOP_SYNCH Error."<<"ErrorCode=0x"<<QString::number(CTO_RESPacket[1], 16);
        return false;
    }
}

bool XCPMaster::CTO_FREE_DAQ()
{
    CTO_ClearData();

    CTO_CMDPacket[0] = FREE_DAQ;

    if (!CTO_CMD_RES_Comm())
    {
        qDebug()<<"CTO Comm error";
        return false;
    }
    if (CTO_RESPacket[0] == 0xFF)
    {
        qDebug()<<"FREE_DAQ OK";
        qDebug()<<"===================================";
        return true;
    }
    else if (CTO_RESPacket[0] == 0xFE)
    {
        qDebug()<<"FREE_DAQ Error."<<"ErrorCode=0x"<<QString::number(CTO_RESPacket[1], 16);
        return false;
    }
}

bool XCPMaster::CTO_ALLOC_DAQ()
{
    CTO_ClearData();

    CTO_CMDPacket[0] = ALLOC_DAQ;

    if (DAQ_COUNT < 1)
        return false;

    quint8* daqCount = (quint8*)&DAQ_COUNT;
    memcpy(CTO_CMDPacket+2, daqCount, 2);

    if (!CTO_CMD_RES_Comm())
    {
        qDebug()<<"CTO Comm error";
        return false;
    }
    if (CTO_RESPacket[0] == 0xFF)
    {
        qDebug()<<"ALLOC_DAQ OK";
        qDebug()<<"===================================";
        return true;
    }
    else if (CTO_RESPacket[0] == 0xFE)
    {
        qDebug()<<"ALLOC_DAQ Error."<<"ErrorCode=0x"<<QString::number(CTO_RESPacket[1], 16);
        return false;
    }
}

bool XCPMaster::CTO_ALLOC_ODT()
{
    CTO_ClearData();

    CTO_CMDPacket[0] = ALLOC_ODT;

    if (ODT_COUNT < 1)
        return false;

    quint8* daqListNumber = (quint8*)&DAQ_LIST_NUMBER;
    memcpy(CTO_CMDPacket+2, daqListNumber, 2);

    CTO_CMDPacket[4] = ODT_COUNT;

    if (!CTO_CMD_RES_Comm())
    {
        qDebug()<<"CTO Comm error";
        return false;
    }
    if (CTO_RESPacket[0] == 0xFF)
    {
        qDebug()<<"ALLOC_ODT OK";
        qDebug()<<"===================================";
        return true;
    }
    else if (CTO_RESPacket[0] == 0xFE)
    {
        qDebug()<<"ALLOC_ODT Error."<<"ErrorCode=0x"<<QString::number(CTO_RESPacket[1], 16);
        return false;
    }
}

bool XCPMaster::CTO_ALLOC_ODT_ENTRY()
{
    CTO_ClearData();

    CTO_CMDPacket[0] = ALLOC_ODT_ENTRY;

    if (ODT_COUNT < 1)
        return false;

    quint8* daqListNumber = (quint8*)&DAQ_LIST_NUMBER;
    memcpy(CTO_CMDPacket+2, daqListNumber, 2);

    CTO_CMDPacket[4] = ODT_NUMBER;
    CTO_CMDPacket[5] = ODT_ENTRIES_COUNT;

    if (!CTO_CMD_RES_Comm())
    {
        qDebug()<<"CTO Comm error";
        return false;
    }
    if (CTO_RESPacket[0] == 0xFF)
    {
        qDebug()<<"ALLOC_ODT_ENTRY OK";
        qDebug()<<"===================================";
        return true;
    }
    else if (CTO_RESPacket[0] == 0xFE)
    {
        qDebug()<<"ALLOC_ODT_ENTRY Error."<<"ErrorCode=0x"<<QString::number(CTO_RESPacket[1], 16);
        return false;
    }
}

void XCPMaster::setPollMeasList(const QList<A2L_VarMeas *> &value)
{
    pollMeasList = value;
}

QHash<QString, QList<A2L_VarMeas *> > XCPMaster::getDgNameVarHash()
{
    //QHash<QString, QList<A2L_VarMeas *>> dgNameVarHash;
    dgNameVarHash.clear();

    QList<quint16> keys = daqListVarHash.uniqueKeys();
    if(keys.count() == 0)
        return dgNameVarHash;

    for(int i = 0; i < keys.count(); i++)
    {
        quint16 daqList = keys.at(i);
        QList<A2L_VarMeas*> meas = daqListVarHash.value(daqList);
        if(meas.isEmpty())
            continue;
        QString dgName = "RP_DAQ_" + xcpName + "_" + QString::number(daqList);
        dgNameVarHash.insert(dgName, meas);
    }

    dgNameVarHash.insert("RP_POLL_" + xcpName, this->pollMeasList);

    return dgNameVarHash;
}

QHash<quint16, quint32> XCPMaster::getDaqListBlockSizeHash() const
{
    return daqListBlockSizeHash;
}

QHash<quint16, QSharedMemory *> XCPMaster::getDaqListSMHash() const
{
    return daqListSMHash;
}

QHash<quint16, QList<A2L_VarMeas *> > XCPMaster::getDaqListVarHash() const
{
    return daqListVarHash;
}

void XCPMaster::xcpDaqConfigPamReset()
{
    // rate-MeasList Hash
    rateVarHash.clear();

    daqPTRList.clear();

    sizeDataBuffer_DAQ = 0;

    DataBuffer_DAQ = NULL;

    pidHash.clear();

    pidAttrHash.clear();

    daqListDataHash.clear();

    varAttrHash.clear();
    //
    //varValueHash.clear();

    daqListVarHash.clear();

    daqListSMHash.clear();

    daqAlloc.clear();
}

A2L_EventCanId *XCPMaster::findEventCanIdByEventNumber(quint8 eventNumber)
{
    if(xcpEventCanIdList.isEmpty())
        return NULL;
    foreach (A2L_EventCanId *eventCanId, xcpEventCanIdList) {
        if(eventCanId->EventNumber == eventNumber)
            return eventCanId;
    }
    return NULL;
}

A2L_Event* XCPMaster::getEventChannelByEventRate(float rate_ms)
{
    if (xcpEventList.isEmpty() || rate_ms == 0)
        return NULL;

    for (int i = 0; i < xcpEventList.count(); i++)
    {
        A2L_Event *event = xcpEventList.at(i);
        if (event->Rate_ms == rate_ms)
        {
            return event;
        }
    }
    return NULL;
}


A2L_Daq_List *XCPMaster::getDaqListByEventRate(float rate_ms)
{
    if (xcpDaqList.isEmpty() || xcpEventList.isEmpty())
        return NULL;
    A2L_Event *refEvent = NULL;

    for (int i = 0; i < xcpEventList.count(); i++)
    {
        A2L_Event *event = xcpEventList.at(i);
        if (event->Rate_ms == rate_ms)
        {
            refEvent = event;
        }
    }

    if (refEvent == NULL)
        return NULL;

    for (int i = 0; i < xcpDaqList.count(); i++)
    {
        A2L_Daq_List *daqList = xcpDaqList.at(i);
        if (daqList->Event_Fixed == refEvent->ChannelNumber)
        {
            return daqList;
        }
    }

    return NULL;
}

void XCPMaster::setXcpMemSegList(const QList<A2L_MemorySegment *> &value)
{
    xcpMemSegList = value;
}

void XCPMaster::setCharList(const QList<A2L_VarChar *> &value)
{
    charList = value;
}
