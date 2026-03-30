#include "xcp_thread.h"
#include <QCoreApplication>
#include <QDebug>

/*
void displayErrorAndExit(nxStatus_t status, nxSessionRef_t session, const char *source)
{
    char statusString[1024];
    nxStatusToString(status, sizeof(statusString), statusString);

    qDebug()<<"\nError at "<<source<<"\n"<<statusString;
    nxClear(session);
}
*/

XCP_R_Thread::XCP_R_Thread(QObject *parent) : QThread(parent)
{

}

XCP_R_Thread::~XCP_R_Thread()
{
    requestInterruption();
    quit();
    wait();
}


void XCP_R_Thread::setReadOnceEnable(bool enable)
{
    readOnceEnable = enable;
}

void XCP_R_Thread::setResDataReady(bool ready)
{
    resDataReady = ready;
}

bool XCP_R_Thread::getResDataReady()
{
    return resDataReady;
}

void XCP_R_Thread::setIsStop(bool value)
{
    isStop = value;
}


void XCP_R_Thread::unpackFrameStream()
{
    for(int i = 0; i < FrameStreamSize; i++)
    {
        quint32 id = (frameBuffer_R_Stream + i)->Identifier;

        id &= 0x1FFFFFFF;

        if(id != this->id_Xcp_SlaveToMaster)
            continue;


        *FrameTimeStampBuffer_Stream = (frameBuffer_R_Stream + i)->Timestamp;
        if(*FrameTimeStampBuffer_Stream <= *FrameTimeStampBuffer_Old_Stream)
            continue;

        quint8 len = sizeof(nxFrameCAN_t);
        quint8 *src = new quint8[len];
        memcpy(src, (quint8*)(frameBuffer_R_Stream + i), len);
        quint8 pid = (frameBuffer_R_Stream + i)->Payload[0];

        if (pid > 0xFB)
        {
            resDataReady = true;
            emit RESDataReady(src, len);
        }
        else
        {
            emit ODTDataReady(src, len);
        }

        *FrameTimeStampBuffer_Old_Stream = *FrameTimeStampBuffer_Stream;

    }

    emit curFrameTimeChanged((quint64)(*FrameTimeStampBuffer_Stream));
}

void XCP_R_Thread::unpackFdFrameStream()
{
    for(int i = 0; i < FrameStreamSize; i++)
    {
        quint32 id = (fdFrameBuffer_R_Stream + i)->Identifier;

        id &= 0x1FFFFFFF;

        if(id != this->id_Xcp_SlaveToMaster)
            continue;


        *FrameTimeStampBuffer_Stream = (fdFrameBuffer_R_Stream + i)->Timestamp;
        if(*FrameTimeStampBuffer_Stream <= *FrameTimeStampBuffer_Old_Stream)
            continue;

        quint8 len = sizeof(nxFrameCANFD_t);
        quint8 *src = new quint8[len];
        memcpy(src, (quint8*)(fdFrameBuffer_R_Stream + i), len);
        quint8 pid = (fdFrameBuffer_R_Stream + i)->Payload[0];

        if (pid > 0xFB)
        {
            resDataReady = true;
            emit RESDataReady(src, len);
        }
        else
        {
            emit ODTDataReady(src, len);
        }

        *FrameTimeStampBuffer_Old_Stream = *FrameTimeStampBuffer_Stream;

    }

    emit curFrameTimeChanged((quint64)(*FrameTimeStampBuffer_Stream));
}

void XCP_R_Thread::setId_Xcp_SlaveToMaster(const quint32 &value)
{
    id_Xcp_SlaveToMaster = value;
}

void XCP_R_Thread::setFrameStreamSize(int value)
{
    FrameStreamSize = value;
}


void XCP_R_Thread::run()
{

    //qDebug()<<"In xcp read thread, id:0x"<<QString::number(id_Xcp_SlaveToMaster, 16).toUpper();;

    *FrameTimeStampBuffer_Stream = 0;
    *FrameTimeStampBuffer_Old_Stream = 0;

    u32 queueSize = 500000;
    nxStatus_t status_SetQueueSize = nxSetProperty(session_R_XNET_Stream, nxPropSession_QueueSize, sizeof(u32), &queueSize);
    if(status_SetQueueSize == nxSuccess)
    {
        qDebug()<<"***** Xcp Read Session QueueSize set as:"<<queueSize;
    }
    else
    {
        qDebug()<<"Xcp Read Session QueueSize set error:"<<status_SetQueueSize;
    }

    while (!isInterruptionRequested())
    {
        usleep(10);

        // changed read mode to queued

        if(!isCanFd)
        {
            status_R_XNET_Stream = nxReadFrame(session_R_XNET_Stream, frameBuffer_R_Stream, numOfBytesForFrames_Stream,
                                        timeout_r_Stream, numOfBytesReturned_Stream);
            if(status_R_XNET_Stream == nxSuccess)
            {

                unpackFrameStream();
            }
            else
            {
                displayErrorAndExit(status_R_XNET_Stream, session_R_XNET_Stream, "nxReadFrame(Queue Mode in Xcp Read)");

                nxStop(session_R_XNET_Stream, 0);
                nxFlush(session_R_XNET_Stream);
                nxStart(session_R_XNET_Stream, 0);

            }
        }
        else
        {
            status_R_XNET_Stream = nxReadFrame(session_R_XNET_Stream, fdFrameBuffer_R_Stream, numOfBytesForFrames_Stream,
                                        timeout_r_Stream, numOfBytesReturned_Stream);
            if(status_R_XNET_Stream == nxSuccess)
            {

                unpackFdFrameStream();
            }
            else
            {
                displayErrorAndExit(status_R_XNET_Stream, session_R_XNET_Stream, "nxReadFrame(Queue Mode in Xcp Read)");

                nxStop(session_R_XNET_Stream, 0);
                nxFlush(session_R_XNET_Stream);
                nxStart(session_R_XNET_Stream, 0);

            }
        }

    }
}
///////////////////////////////////////////////////////

XCP_R_Event_Thread::XCP_R_Event_Thread(QObject *parent)
{

}

XCP_R_Event_Thread::~XCP_R_Event_Thread()
{
    requestInterruption();
    quit();
    wait();
}

void XCP_R_Event_Thread::setIsStop(bool value)
{
    this->isStop = value;
}

void XCP_R_Event_Thread::setFrameStreamSize(int value)
{
    this->FrameStreamSize = value;
}

void XCP_R_Event_Thread::unpackEventFrameStream()
{
    for(int i = 0; i < FrameStreamSize; i++)
    {
        quint32 id = (frameBuffer_R_Event_Stream + i)->Identifier;
        if(eventIdList.indexOf(id) == -1)
            continue;

        *FrameTimeStampBuffer_Stream = (frameBuffer_R_Event_Stream + i)->Timestamp;
        if(*FrameTimeStampBuffer_Stream <= *FrameTimeStampBuffer_Old_Stream)
            continue;


        quint8 len = sizeof(nxFrameCAN_t);
        quint8 *src = new quint8[len];
        memcpy(src, (quint8*)(frameBuffer_R_Event_Stream + i), len);
        quint8 pid = (frameBuffer_R_Event_Stream + i)->Payload[0];

        if (pid <= 0xFB)
        {
            emit ODTDataReady(src, len);
        }

        *FrameTimeStampBuffer_Old_Stream = *FrameTimeStampBuffer_Stream;

    }
    emit curFrameTimeChanged((quint64)(*FrameTimeStampBuffer_Stream));
}

void XCP_R_Event_Thread::unpackEventFdFrameStream()
{
    for(int i = 0; i < FrameStreamSize; i++)
    {
        quint32 id = (fdFrameBuffer_R_Event_Stream + i)->Identifier;
        if(eventIdList.indexOf(id) == -1)
            continue;

        *FrameTimeStampBuffer_Stream = (fdFrameBuffer_R_Event_Stream + i)->Timestamp;
        if(*FrameTimeStampBuffer_Stream <= *FrameTimeStampBuffer_Old_Stream)
            continue;


        quint8 len = sizeof(nxFrameCANFD_t);
        quint8 *src = new quint8[len];
        memcpy(src, (quint8*)(fdFrameBuffer_R_Event_Stream + i), len);
        quint8 pid = (fdFrameBuffer_R_Event_Stream + i)->Payload[0];

        if (pid <= 0xFB)
        {
            emit ODTDataReady(src, len);
        }

        *FrameTimeStampBuffer_Old_Stream = *FrameTimeStampBuffer_Stream;

    }
    emit curFrameTimeChanged((quint64)(*FrameTimeStampBuffer_Stream));
}

void XCP_R_Event_Thread::setEventIdList(const QList<quint32> &value)
{
    eventIdList = value;
}

void XCP_R_Event_Thread::run()
{

    *FrameTimeStampBuffer_Stream = 0;
    *FrameTimeStampBuffer_Old_Stream = 0;
    while (!isInterruptionRequested())
    {
        usleep(10);

        if(!isCanFd)
        {
            status_R_Event_XNET_Stream = nxReadFrame(session_R_Event_XNET_Stream, frameBuffer_R_Event_Stream, numOfBytesForFrames_Stream,
                                        timeout_r_Stream, numOfBytesReturned_Stream);
            if(status_R_Event_XNET_Stream == nxSuccess)
            {

                unpackEventFrameStream();
            }
            else
            {
                displayErrorAndExit(status_R_Event_XNET_Stream, session_R_Event_XNET_Stream, "nxReadFrame(Event Stream)");
            }
        }
        else
        {
            status_R_Event_XNET_Stream = nxReadFrame(session_R_Event_XNET_Stream, fdFrameBuffer_R_Event_Stream, numOfBytesForFrames_Stream,
                                        timeout_r_Stream, numOfBytesReturned_Stream);
            if(status_R_Event_XNET_Stream == nxSuccess)
            {

                unpackEventFdFrameStream();
            }
            else
            {
                displayErrorAndExit(status_R_Event_XNET_Stream, session_R_Event_XNET_Stream, "nxReadFrame(Event Stream)");
            }
        }


    }
}


/////////////////////////////////////////////////////////////
XCP_W_Thread::XCP_W_Thread(QObject *parent) : QThread(parent)
{

}

XCP_W_Thread::~XCP_W_Thread()
{
    requestInterruption();
    quit();
    wait();
}


void XCP_W_Thread::setWriteOnceEnable(bool enable)
{
    writeOnceEnable = enable;
    writeSucceed = false;
}

bool XCP_W_Thread::writeOnceSucceed()
{
    return writeSucceed;
}

void XCP_W_Thread::setCMDPayLoad(quint8 *data, u32 numByte)
{
    if(!isCanFd)
    {
        quint8 *dst = frameBuffer_W->Payload;
        memcpy(dst, data, numByte);
        frameBuffer_W->PayloadLength = numByte;
    }
    else
    {
        quint8 *dst = fdFrameBuffer_W->Payload;
        memcpy(dst, data, numByte);
        fdFrameBuffer_W->PayloadLength = numByte;
    }

}

void XCP_W_Thread::run()
{
    while (!isInterruptionRequested())
    {
        usleep(10);

        if (writeOnceEnable)
        {
            if(!isCanFd)
            {
                status_W_XNET = nxWriteFrame(session_W_XNET, frameBuffer_W,
                                             numOfBytesForFrames, timeout_w);
            }
            else
            {
                status_W_XNET = nxWriteFrame(session_W_XNET, fdFrameBuffer_W,
                                             numOfBytesForFrames, timeout_w);
            }

            if (status_W_XNET == nxSuccess)
            {
               writeSucceed = true;
               //qDebug()<<"CTO write once";
            }
            else
            {
                displayErrorAndExit(status_W_XNET, session_W_XNET, "nxWriteFrame");
            }
            writeOnceEnable = false;
        }
    }
}

//////////////////////////////////////////////////////////////////////
XCP_Thread::XCP_Thread(QObject *parent) : QObject(parent)
{

    thread_RES = new XCP_R_Thread(this);
    thread_CMD = new XCP_W_Thread(this);
    thread_EVENT = new XCP_R_Event_Thread(this);

}

XCP_Thread::~XCP_Thread()
{
    delete thread_EVENT;
    delete thread_RES;
    delete thread_CMD;
}

void XCP_Thread::preSession()
{
    temp_path = path.toLocal8Bit();
    xcp_dbcPath = temp_path.data();

    QStringList fileList = path.split("/", QString::SkipEmptyParts);
    if (fileList.count() == 0)
        return;
    name = fileList.at(fileList.count()-1);
    name = name.split(".", QString::SkipEmptyParts)[0];
    temp_name = name.toLocal8Bit();
    xcp_dbcName = temp_name.data();

    temp_port = port.toLocal8Bit();
    xcp_can_interface = temp_port.data();
    qDebug()<<xcp_can_interface;

    temp_cluster = cluster.toLocal8Bit();
    xcp_dbcCluster = temp_cluster.data();

    temp_frames_r = frames_r.toLocal8Bit();
    Frame_RES = temp_frames_r.data();
    qDebug()<<Frame_RES;

    temp_frames_w = frames_w.toLocal8Bit();
    Frame_CMD = temp_frames_w.data();
    qDebug()<<Frame_CMD;

    temp_frames_r_event = frames_r_event.toLocal8Bit();
    Frames_EVENT = temp_frames_r_event.data();
    qDebug()<<Frames_EVENT;

    QStringList frameList_r;
    int numFrame_R = 0;
    if (!frames_r.isEmpty())
    {
        frameList_r = frames_r.split(',');
        thread_RES->numOfFrame = frames_r.split(',').count();
        numFrame_R = thread_RES->numOfFrame;

        thread_RES->FrameTimeStampBuffer_Stream = new nxTimestamp_t[numFrame_R];
        thread_RES->FrameTimeStampBuffer_Old_Stream = new nxTimestamp_t[numFrame_R];
        thread_RES->isCanFd = isCanFd;

        if(!isCanFd)
        {
            thread_RES->frameBuffer_R_Stream = new nxFrameCAN_t[frameStreamSize];
            thread_RES->numOfBytesForFrames_Stream = frameStreamSize * sizeof(nxFrameCAN_t);
        }
        else
        {
            thread_RES->fdFrameBuffer_R_Stream = new nxFrameCANFD_t[frameStreamSize];
            thread_RES->numOfBytesForFrames_Stream = frameStreamSize * sizeof(nxFrameCANFD_t);
        }


        thread_RES->setFrameStreamSize(this->frameStreamSize);

        for (int i = 0; i < numFrame_R; i++)
        {
            thread_RES->FrameTimeStampBuffer_Stream[i] = 0;
            thread_RES->FrameTimeStampBuffer_Old_Stream[i] = 0;
        }
    }

    QStringList frameList_w;
    if (!frames_w.isEmpty())
    {
        frameList_w = frames_w.split(',');
        thread_CMD->numOfFrame = frames_w.split(',').count();
        thread_CMD->isCanFd = isCanFd;

        if(!isCanFd)
        {
            thread_CMD->numOfBytesForFrames = thread_CMD->numOfFrame * sizeof(nxFrameCAN_t);
            thread_CMD->frameBuffer_W = new nxFrameCAN_t[thread_CMD->numOfFrame];
            thread_CMD->frameBuffer_W_temp = new nxFrameCAN_t[thread_CMD->numOfFrame];
        }
        else
        {
            thread_CMD->numOfBytesForFrames = thread_CMD->numOfFrame * sizeof(nxFrameCANFD_t);
            thread_CMD->fdFrameBuffer_W = new nxFrameCANFD_t[thread_CMD->numOfFrame];
            thread_CMD->fdFrameBuffer_W_temp = new nxFrameCANFD_t[thread_CMD->numOfFrame];
        }

    }

    QStringList frameList_r_event;
    if (!frames_r_event.isEmpty())
    {
        frameList_r_event = frames_r_event.split(',');
        thread_EVENT->numOfFrame = frames_r_event.split(',').count();
        quint32 numFrame_R_EVENT = thread_EVENT->numOfFrame;
        thread_EVENT->isCanFd = isCanFd;

        if(!isCanFd)
        {
            thread_EVENT->frameBuffer_R_Event_Stream = new nxFrameCAN_t[frameStreamSize_EVENT];
            thread_EVENT->numOfBytesForFrames_Stream = frameStreamSize_EVENT * sizeof(nxFrameCAN_t);
        }
        else
        {
            thread_EVENT->fdFrameBuffer_R_Event_Stream = new nxFrameCANFD_t[frameStreamSize_EVENT];
            thread_EVENT->numOfBytesForFrames_Stream = frameStreamSize_EVENT * sizeof(nxFrameCANFD_t);
        }
        thread_EVENT->FrameTimeStampBuffer_Stream = new nxTimestamp_t[numFrame_R_EVENT];
        thread_EVENT->FrameTimeStampBuffer_Old_Stream = new nxTimestamp_t[numFrame_R_EVENT];

        thread_EVENT->setFrameStreamSize(this->frameStreamSize_EVENT);

        for (int i = 0; i < numFrame_R_EVENT; i++)
        {
            thread_EVENT->FrameTimeStampBuffer_Stream[i] = 0;
            thread_EVENT->FrameTimeStampBuffer_Old_Stream[i] = 0;
        }
    }


    if ((status_RES_R_Stream == 0) || (status_CMD_W == 0) || (status_EVENT_R_Stream == 0))
    {
        nxClear(sessionRef_RES_R);
        nxClear(sessionRef_RES_R_Stream);
        nxClear(sessionRef_CMD_W);
        nxClear(sessionRef_EVENT_R_Stream);
    }

    //status_RES_R = nxdbRemoveAlias(dbcName);
    //qDebug()<<"Remove Alias Status = "<<status_RES_R;

    status_RES_R = nxdbAddAlias64(xcp_dbcName, xcp_dbcPath, baudrate);
    qDebug()<<"Add Alias Status = "<<status_RES_R;

}

void XCP_Thread::afterSession()
{
    if(!frames_r.isEmpty())
    {
        delete[] thread_RES->FrameTimeStampBuffer_Stream;
        thread_RES->FrameTimeStampBuffer_Stream = NULL;

        delete[] thread_RES->FrameTimeStampBuffer_Old_Stream;
        thread_RES->FrameTimeStampBuffer_Old_Stream = NULL;

        if(!isCanFd)
        {
            delete[] thread_RES->frameBuffer_R_Stream;
            thread_RES->frameBuffer_R_Stream = NULL;
        }
        else
        {
            delete[] thread_RES->fdFrameBuffer_R_Stream;
            thread_RES->fdFrameBuffer_R_Stream = NULL;
        }

    }

    if(!frames_w.isEmpty())
    {
        if(!isCanFd)
        {
            delete[] thread_CMD->frameBuffer_W;
            thread_CMD->frameBuffer_W = NULL;

            delete[] thread_CMD->frameBuffer_W_temp;
            thread_CMD->frameBuffer_W_temp = NULL;
        }
        else
        {
            delete[] thread_CMD->fdFrameBuffer_W;
            thread_CMD->fdFrameBuffer_W = NULL;

            delete[] thread_CMD->fdFrameBuffer_W_temp;
            thread_CMD->fdFrameBuffer_W_temp = NULL;
        }

    }

    if(!frames_r_event.isEmpty())
    {
        delete[] thread_EVENT->FrameTimeStampBuffer_Stream;
        thread_EVENT->FrameTimeStampBuffer_Stream = NULL;

        delete[] thread_EVENT->FrameTimeStampBuffer_Old_Stream;
        thread_EVENT->FrameTimeStampBuffer_Old_Stream = NULL;

        if(!isCanFd)
        {
            delete[] thread_EVENT->frameBuffer_R_Event_Stream;
            thread_EVENT->frameBuffer_R_Event_Stream = NULL;
        }
        else
        {
            delete[] thread_EVENT->fdFrameBuffer_R_Event_Stream;
            thread_EVENT->fdFrameBuffer_R_Event_Stream = NULL;
        }

    }
}


QStringList XCP_Thread::getCANInterface()
{
    QStringList interfaceList;
    nxSessionRef_t sessionSys;
    nxStatus_t status = nxSystemOpen(&sessionSys);

    u32 intfRefs[DEFAULT_INTF_NUM];

    if (status == nxSuccess)
    {
        status = nxGetProperty(sessionSys, nxPropSys_IntfRefs, sizeof(u32)*DEFAULT_INTF_NUM, intfRefs);
        if (status == nxSuccess)
        {
            for (int i = 0; i < DEFAULT_INTF_NUM; i++)
            {
                u32 propSize = 0;
                if (intfRefs[i])
                {
                    status = nxGetPropertySize(intfRefs[i], nxPropIntf_Name, &propSize);
                    if(propSize <= 0 || propSize > 6)
                        continue;
                    char *intfNameChar = new char[propSize];
                    status = nxGetProperty(intfRefs[i], nxPropIntf_Name, propSize, intfNameChar);
                    QString intfName =  QString(intfNameChar);
                    //qDebug()<<intfNameStr;
                    if (!intfName.isEmpty() && intfName.contains("CAN"))
                    {
                        interfaceList << intfName;
                    }
                    delete[] intfNameChar;
                }

            }
        }
    }
    return interfaceList;
}


bool XCP_Thread::XCP_CAN_Init()
{

    preSession();


    status_RES_R_Stream = nxCreateSession(xcp_dbcName, xcp_dbcCluster, Frame_RES,
                                    xcp_can_interface, nxMode_FrameInQueued,
                                    &sessionRef_RES_R_Stream);
    if (status_RES_R_Stream == nxSuccess)
    {
        qDebug()<<"Create XCP RES Queue Session Succeed!"<<sessionRef_RES_R_Stream;
        thread_RES->session_R_XNET_Stream = sessionRef_RES_R_Stream;
        thread_RES->start();

    }
    else
    {
        displayErrorAndExit(status_RES_R_Stream, sessionRef_RES_R_Stream, "nxCreateSession(XCP RES Queued)");

    }


    status_CMD_W = nxCreateSession(xcp_dbcName, xcp_dbcCluster, Frame_CMD,
                              xcp_can_interface, nxMode_FrameOutSinglePoint,
                              &sessionRef_CMD_W);

    if (status_CMD_W == nxSuccess)
    {
        qDebug()<<"Create XCP CMD Session Succeed!"<<sessionRef_CMD_W;
        thread_CMD->session_W_XNET = sessionRef_CMD_W;
        thread_CMD->start();

        //thread_CMD->time_w.start();
    }
    else
    {
        displayErrorAndExit(status_CMD_W, sessionRef_CMD_W, "nxCreateSession");

    }

    if(!frames_r_event.isEmpty())
    {
        status_EVENT_R_Stream = nxCreateSession(xcp_dbcName, xcp_dbcCluster, Frames_EVENT,
                                        xcp_can_interface, nxMode_FrameInStream,
                                        &sessionRef_EVENT_R_Stream);
        if (status_EVENT_R_Stream == nxSuccess)
        {
            qDebug()<<"Create XCP Event Stream Session Succeed!"<<sessionRef_EVENT_R_Stream;
            thread_EVENT->session_R_Event_XNET_Stream = sessionRef_EVENT_R_Stream;
            thread_EVENT->start();

        }
        else
        {
            displayErrorAndExit(status_EVENT_R_Stream, sessionRef_EVENT_R_Stream, "nxCreateSession(Event Stream)");
        }

    }


    if ((status_RES_R_Stream == nxSuccess ) || (status_CMD_W == nxSuccess))
        return true;
    else
        return false;

}

void XCP_Thread::XCP_CAN_Stop()
{
    qDebug()<<"XCP CAN Stop.";

    thread_EVENT->requestInterruption();
    thread_EVENT->setIsStop(true);
    thread_EVENT->quit();
    status_EVENT_R_Stream = nxClear(sessionRef_EVENT_R_Stream);
    if (status_EVENT_R_Stream != nxSuccess)
    {
        displayErrorAndExit(status_EVENT_R_Stream, sessionRef_EVENT_R_Stream, "nxClearSession");
    }
    else
    {
        qDebug()<<"XCP CAN Read Event Stream Session Clear Succeed!";
    }

    thread_RES->requestInterruption();
    thread_RES->setIsStop(true);
    thread_RES->quit();
    //status_RES_R = nxClear(sessionRef_RES_R);
    status_RES_R_Stream = nxClear(sessionRef_RES_R_Stream);
    if (status_RES_R_Stream != nxSuccess)
    {
        displayErrorAndExit(status_RES_R_Stream, sessionRef_RES_R_Stream, "nxClearSession");
    }
    else
    {
        qDebug()<<"XCP CAN Read Stream Session Clear Succeed!";
    }

    thread_CMD->requestInterruption();
    thread_CMD->quit();
    status_CMD_W = nxClear(sessionRef_CMD_W);
    if (status_CMD_W != nxSuccess)
    {
        displayErrorAndExit(status_CMD_W, sessionRef_CMD_W, "nxClearSession");
    }
    else
    {
        qDebug()<<"XCP CAN Write Session Clear Succeed!";
    }

    afterSession();

}
