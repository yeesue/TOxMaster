#include "xcp_daq_thread.h"

XCP_DAQ_Thread::XCP_DAQ_Thread(QObject *parent, XCPMaster *master) :
    QObject(parent), xcpMaster(master)
{

}

void XCP_DAQ_Thread::setDaqMeasPamList(const QList<A2L_VarMeas *> &value)
{
    daqMeasPamList = value;
}

void XCP_DAQ_Thread::arrangeMeasVars()
{
        xcpDaqConfigPamReset();

        // 将参数分配到对应的周期-参数列表Hash中
        for (int i = 0; i < measVarList.count(); i++)
        {
            A2L_VarMeas *var = measVarList.at(i);
            quint16 rate = var->rate_ms;                                      // 获取DAQ 参数 采样周期
            sizeDataBuffer_DAQ += var->DataSizeAG;                  // 计算DAQ 参数 总大小

            varValueHash.insert(var, 0.0);                                       // 初始化参数-值 Hash

            // 将参数加入对应的周期-参数列表 Hash
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

        //DAQ 数据存储区
        //DataBuffer_DAQ = new quint8[sizeDataBuffer_DAQ];

        quint8 pid = 0;
        quint32 offset = 0;

        QList<float> keys = rateVarHash.keys();
        qSort(keys.begin(), keys.end());

        if (keys.count() == 0)
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

            quint16 curDAQList = i;
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
                else if ((dataIndex + var->DataSizeAG) < (dataVolumeAG*2))
                {
                    ptr1.PTR_Size = (dataVolumeAG - dataIndex);
                    daqPTRList.append(ptr1);

                    //DataBuffer *buffer1 = new DataBuffer;
                    DataBuffer buffer1 ;
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
                    ptr2.PTR_Size = (var->DataSizeAG - ptr1.PTR_Size);
                    ptr2.PTR_var = var;
                    ptr2.DAQ_PID = pid;
                    daqPTRList.append(ptr2);

                    dataIndex += (var->DataSizeAG - ptr1.PTR_Size);
                    odtEntryIndex += 1;

                }
                else if ((dataIndex + var->DataSizeAG) < (dataVolumeAG*3))
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

void XCP_DAQ_Thread::arrangeMeasVars_Static()
{
    xcpDaqConfigPamReset();

    for (int i = 0; i < measVarList.count(); i++)
    {
        A2L_VarMeas *var = measVarList.at(i);
        quint16 rate = var->rate_ms;
        sizeDataBuffer_DAQ += var->DataSizeAG;

        varValueHash.insert(var, 0.0);

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

    //DAQ 数据存储区
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

A2L_Daq_List *XCP_DAQ_Thread::getDaqListByEventRate(float rate_ms)
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

void XCP_DAQ_Thread::xcpDaqConfigPamReset()
{
    // rate-MeasList Hash
    rateVarHash.clear();

    daqPTRList.clear();

    sizeDataBuffer_DAQ = 0;

    pidHash.clear();

    pidAttrHash.clear();

    daqListDataHash.clear();

    varAttrHash.clear();
    //
    varValueHash.clear();

    daqListVarHash.clear();

    daqListSMHash.clear();

    daqAlloc.clear();
}
