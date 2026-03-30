#include "nixnet_thread.h"
#include <QDebug>


void displayErrorAndExit(nxStatus_t status, nxSessionRef_t session, const char *source)
{
    char statusString[1024];
    nxStatusToString(status, sizeof(statusString), statusString);

    qCritical()<<"\nError at "<<source<<"\n"<<statusString;
    nxClear(session);
}


NIXNET_R_Queue_Thread::NIXNET_R_Queue_Thread(QObject *parent)
{

}

NIXNET_R_Queue_Thread::~NIXNET_R_Queue_Thread()
{

}

void NIXNET_R_Queue_Thread::setCanType(const quint32 &value)
{
    canType = value;
}

void NIXNET_R_Queue_Thread::setSm(QSharedMemory *value)
{
    sm = value;
}

void NIXNET_R_Queue_Thread::setReadFrame(dbc_frame *value)
{
    readFrame = value;

    if(readFrame->m_cyclicTime < 1) readFrame->m_cyclicTime = 100;
}

void NIXNET_R_Queue_Thread::setIsStop(bool value)
{
    isStop = value;
}

void NIXNET_R_Queue_Thread::setIsArxml(bool value)
{
    isArxml = value;
}

void NIXNET_R_Queue_Thread::setSession_readFrame_Queue(const nxSessionRef_t &value)
{
    session_readFrame_Queue = value;
}

void NIXNET_R_Queue_Thread::run()
{

    qDebug()<<"In read frame run: "<<readFrame->m_name;
    emit stateChanged(3);

    u32 queueSize = 10000;
    nxStatus_t status_SetQueueSize = nxSetProperty(session_readFrame_Queue, nxPropSession_QueueSize, sizeof(u32), &queueSize);
    if(status_SetQueueSize == nxSuccess)
    {
        qDebug()<<"***Can read session queueSize set as: "<<queueSize;
    }
    else
    {
        qCritical()<<"Can read session queueSize set error:"<<status_SetQueueSize;
    }

    canfd_frameBuffer_Queue = new nxFrameCANFD_t[frameQueueSize];
    u32 numOfBytesForFrames = sizeof(nxFrameCANFD_t) * frameQueueSize;
    f64 timeout = nxTimeout_None;

    while (!isStop)
    {
        msleep(readFrame->m_cyclicTime);
        nxStatus_t status_readFrame_Queue = nxReadFrame(session_readFrame_Queue, canfd_frameBuffer_Queue, numOfBytesForFrames,
                                    timeout, &numOfBytesReturned);

        if (status_readFrame_Queue == nxSuccess)  //ignore warning: "The CAN FD baud rate you supplied exceeds the capabilities the transceiver manufacturer specified"
        {
            //qDebug()<<sel_frame_r->m_name<<"num need:"<<numOfBytesForFrames<<", read queue number:"<<*numOfBytesReturned;
            unpackFrameQueue();

            errMsgShow = false;

        }
        else
        {
            if(errMsgShow) continue;

            qCritical()<<"Read Frame Queue error, Session:"<<session_readFrame_Queue;
            displayErrorAndExit(status_readFrame_Queue, session_readFrame_Queue, "nxReadFrame(Queue Mode in Can Read)");

            nxStop(session_readFrame_Queue, 1);
            nxFlush(session_readFrame_Queue);
            nxStart(session_readFrame_Queue, 1);

            emit stateChanged(-1);

            errMsgShow = true;
        }
    }

}

void NIXNET_R_Queue_Thread::unpackFrameQueue()
{
    nxFrameVar_t *Frame = (nxFrameVar_t *)canfd_frameBuffer_Queue;
    while ((u8*)Frame < ((u8*)canfd_frameBuffer_Queue + numOfBytesReturned))
    {
        quint32 id = Frame->Identifier;

        /*
        if(id != frameId)
        {
            Frame = nxFrameIterate(Frame);
            continue;
        }
        *FrameTimeStampBuffer_Queue = Frame->Timestamp;
        if(*FrameTimeStampBuffer_Queue <= *FrameTimeStampBuffer_Old_Queue)
        {
            Frame = nxFrameIterate(Frame);
            continue;
        }
        */
        //qDebug()<<"In queue thread id:"<<id<<", timestamp:"<<*(quint64*)FrameTimeStampBuffer_Queue;

        quint8 len = nxFrameSize(Frame->PayloadLength);

        emit readFrameRawDataReceived(id, (quint8*)Frame, len);

        Frame = nxFrameIterate(Frame);

    }
}


///////////////////////////////////////////////////////////////////////
NIXNET_W_Queue_Thread::NIXNET_W_Queue_Thread(QObject *parent)
{

}

NIXNET_W_Queue_Thread::~NIXNET_W_Queue_Thread()
{

}

void NIXNET_W_Queue_Thread::setCanType(const quint32 &value)
{
    canType = value;
}

void NIXNET_W_Queue_Thread::setIsArxml(bool value)
{
    isArxml = value;
}

void NIXNET_W_Queue_Thread::setIsStop(bool value)
{
    isStop = value;
}

void NIXNET_W_Queue_Thread::setSm(QSharedMemory *value)
{
    sm = value;
}

void NIXNET_W_Queue_Thread::setWriteFrame(dbc_frame *value)
{
    writeFrame = value;
}

void NIXNET_W_Queue_Thread::setSession_writeSigConv_Queue(const nxSessionRef_t &value)
{
    session_writeSigConv_Queue = value;
}

void NIXNET_W_Queue_Thread::setSession_WriteFrame_Queue(const nxSessionRef_t &value)
{
    session_WriteFrame_Queue = value;
}

void NIXNET_W_Queue_Thread::setFrameQueueSize(int value)
{
    frameQueueSize = value;
}

void NIXNET_W_Queue_Thread::run()
{
    emit stateChanged(3);

    init();
    time.start();

    while (!isStop)
    {
        msleep(writeFrame->m_cyclicTime);

        if(!packTransmitData()) continue;

        if(!copySndDataFromSmToBuf()) continue;

        nxStatus_t status_WriteFrame_Queue = nxWriteFrame(session_WriteFrame_Queue, canfd_frameBuffer_Queue,
                                     numOfBytesForFrames, timeout_w);
        if (status_WriteFrame_Queue == nxSuccess)
        {

            emit stateChanged(3);

            updateSndTimeStamp();

            errMsgShow = false;
        }
        else
        {
            if(errMsgShow) continue;

            displayErrorAndExit(status_WriteFrame_Queue, session_WriteFrame_Queue, "nxWriteFrame(Queue Mode in Can Write)");

            nxStop(session_WriteFrame_Queue, 1);
            nxFlush(session_WriteFrame_Queue);
            nxStart(session_WriteFrame_Queue, 1);

            emit stateChanged(-1);

            errMsgShow = true;
        }
    }

    delete[] canfd_frameBuffer_Queue;
    delete[] canfd_frameBuffer_sigConv;
    delete[] sigValueBuf;
}

void NIXNET_W_Queue_Thread::init()
{

    u32 queueSize = 100000;
    nxStatus_t status_SetQueueSize = nxSetProperty(session_WriteFrame_Queue, nxPropSession_QueueSize, sizeof(u32), &queueSize);
    if(status_SetQueueSize == nxSuccess)
    {
        qDebug()<<"***Can queue write session queueSize set as: "<<queueSize;
    }
    else
    {
        qCritical()<<"Can queue write session queueSize set error:"<<status_SetQueueSize;
    }


    if(writeFrame->m_cyclicTime < 1) writeFrame->m_cyclicTime = 100;
    numOfBytesForFrames = frameQueueSize * nxFrameSize(writeFrame->m_dlc);
    canfd_frameBuffer_Queue = new u8[numOfBytesForFrames];
    canfd_frameBuffer_sigConv = new u8[numOfBytesForFrames];

    sigNum = writeFrame->m_sigs.count();
    sizeOfsigValBuf = sigNum * sizeof(f64);
    sigValueBuf = new f64[sigNum];
}

void NIXNET_W_Queue_Thread::sigRC(dbc_signal *sig, int min, int max, int step)
{
    sig->m_value += step;

    if(sig->m_value > max) sig->m_value = min;
}

void NIXNET_W_Queue_Thread::dataCRC(char *data, int startIndex, int endIndex, int crcIndex)
{
    quint8 crcResult = 0;
    for(int i = startIndex; i < endIndex; i++)
    {
        if(i == crcIndex) continue;

        crcResult = crcResult ^ data[i];
    }

    data[crcIndex] = crcResult;
}

bool NIXNET_W_Queue_Thread::packTransmitData()
{
    if(!writeFrame) return false;

    for(int i = 0; i < sigNum; i++)
    {
        dbc_signal *sig = writeFrame->m_sigs.at(i);

        // RC handle
        if(sig->m_name.contains("_RC"))
        {
            sigRC(sig, 0, 15, 1);
        }

        sigValueBuf[i] = sig->m_value;
    }

    u32 numOfBytesReturned = 0;
    nxStatus_t status_writeSigConv = nxConvertSignalsToFramesSinglePoint(session_writeSigConv_Queue, sigValueBuf,
                                                           sizeOfsigValBuf, canfd_frameBuffer_sigConv, numOfBytesForFrames,
                                                           &numOfBytesReturned);
    if (status_writeSigConv == nxSuccess)
    {
        //qDebug()<<"Write signal conversion succeed!"<<session_writeSigConv_Queue;

        char* data = (char*)canfd_frameBuffer_sigConv + 16;
        copyToSharedMemory(sm, 8, data, writeFrame->m_dlc);

        return true;
    }
    else
    {
        //displayErrorAndExit(status_writeSigConv, session_writeSigConv_Queue, "nxConvertSignalstoFrames");

        return false;
    }
}

void NIXNET_W_Queue_Thread::copyFromSigConvBufToFrameBuf()
{
    if(!canfd_frameBuffer_Queue || !canfd_frameBuffer_sigConv) return;

    memcpy(canfd_frameBuffer_Queue, canfd_frameBuffer_sigConv, numOfBytesForFrames);
}

bool NIXNET_W_Queue_Thread::copySndDataFromSmToBuf()
{
    if(!writeFrame || !sm || !canfd_frameBuffer_Queue) return false;

    /*
    ((nxFrameVar_t*)canfd_frameBuffer_Queue)->Identifier = writeFrame->m_id;
    ((nxFrameVar_t*)canfd_frameBuffer_Queue)->PayloadLength = writeFrame->m_dlc;
    ((nxFrameVar_t*)canfd_frameBuffer_Queue)->Type = 0;
    */

    copyFromSigConvBufToFrameBuf();

    char* data = (char*)canfd_frameBuffer_Queue + 16;
    copyFromSharedMemory(sm, 8, data, writeFrame->m_dlc);

    return true;
}

void NIXNET_W_Queue_Thread::updateSndTimeStamp()
{
    sndAbsTime = (quint64)time.elapsed();  // timeStamp(ms)

    copyToSharedMemory(sm, 0, (char*)&sndAbsTime, 8);
}


/////////////////////////////////////////////////////////////////////////////////////

NIXNET_Thread::NIXNET_Thread(QObject *parent) : QObject(parent)
{

}

NIXNET_Thread::~NIXNET_Thread()
{

}

QHash<QString, QSharedMemory *> NIXNET_Thread::getReadFrameSmHash() const
{
    return readFrameSmHash;
}

QHash<QString, QSharedMemory *> NIXNET_Thread::getWriteFrameSmHash() const
{
    return writeFrameSmHash;
}


void NIXNET_Thread::setFrames(QList<dbc_frame *> frames)
{
    dbc_frames = frames;
}

void NIXNET_Thread::setFrames_r(QList<dbc_frame *> frames)
{
    this->dbc_frames_r = frames;
}

void NIXNET_Thread::setFrames_w(QList<dbc_frame *> frames)
{
    this->dbc_frames_w = frames;
}

bool NIXNET_Thread::deviceConnect()
{

    initSession();

    //////////////////////////////////////////////
    for(int i = 0; i < dbc_frames_w.count(); i++)
    {
        dbc_frame *frame = dbc_frames_w.at(i);

        QString frameSigStr;
        foreach (dbc_signal *sig, frame->m_sigs) {
            frameSigStr += sig->m_name;
            if(sig != frame->m_sigs.last())
            {
                frameSigStr += ",";
            }
        }
        QByteArray tempFrameSig = frameSigStr.toLocal8Bit();
        const char* sigStr = tempFrameSig.data();
        nxSessionRef_t sigConvFrameSession;

        QByteArray tempFrame = frame->m_name.toLocal8Bit();
        const char* frameStr = tempFrame.data();
        nxSessionRef_t queueWriteSession;

        qDebug()<<"=======Write Frame:"<<frame->m_name;
        NIXNET_W_Queue_Thread *queueWriteThread = queueWriteThreadList.at(i);

        nxStatus_t status_sigConvFrame = nxCreateSession(dbcName, dbcCluster, sigStr,
                                                         can_interface, nxMode_SignalConversionSinglePoint,
                                                         &sigConvFrameSession);
        if(status_sigConvFrame == nxSuccess)
        {
            qDebug()<<"Create Sig Conv Session in Queue Write Succeed!"<<sigConvFrameSession;

            queueWriteThread->setSession_writeSigConv_Queue(sigConvFrameSession);
            sessionRefList_queueWriteSigConv.append(sigConvFrameSession);

        }
        else
        {
            displayErrorAndExit(status_sigConvFrame, sigConvFrameSession, "nxCreateSession queue write sigConvFrame");
        }

        //try out frameOut single point mode and queued mode
        nxStatus_t status_writeFrameQueue = nxCreateSession(dbcName, dbcCluster, frameStr,
                                                            can_interface, nxMode_FrameOutSinglePoint,
                                                            &queueWriteSession);
        if (status_writeFrameQueue == nxSuccess)
        {
            qDebug()<<"Create Queue Write Session Succeed!"<<queueWriteSession;

            queueWriteThread->setSession_WriteFrame_Queue(queueWriteSession);

            sessionRefList_queueWriteFrame.append(queueWriteSession);

            if(i == 0)
            {
                nxStatus_t status_dbc;
                status_dbc = nxSetProperty(queueWriteSession, nxPropSession_IntfCanFdBaudRate64, sizeof(u64), &canfd_baudrate);
                qDebug()<<"Status of set fd baudrate:"<<status_dbc;
                status_dbc = nxSetProperty(queueWriteSession, nxPropSession_IntfCANTerm, sizeof(u32), &canTermination);
                qDebug()<<"Status of set can termination:"<<status_dbc;

                //Customize the Baudrate and FD Baudrate for Write Session
                if (customBD_on && queueWriteSession != 0)
                {
                    if (setSessionIntfCustomBaudrate(queueWriteSession, baudrate, samplePoint_BD*100))  //Sample Point = 70.00%
                    {
                        qDebug()<<"Queue write session baudrate sample point set ok!"<<samplePoint_BD;
                        samplePointError = false;
                    }
                    else
                    {
                        qCritical()<<"Queue write session baudrate sample point set Error.";
                        samplePointError = true;
                        emit stateChanged(-1);
                    }
                }

                if (customFDBD_on && queueWriteSession != 0)
                {
                    queueWriteThread->setIsArxml(true);
                    queueWriteThread->setCanType(1);
                    qDebug()<<"Queue write thread in [FD Mode]";

                    if(setSessionIntfCustomFDBaudrate(queueWriteSession, canfd_baudrate, samplePoint_FDBD*100))
                    {
                        qDebug()<<"Queue write session FD baudrate sample point set ok!"<<samplePoint_FDBD;
                        samplePointError = false;
                    }
                    else
                    {
                        qCritical()<<"Queue write session FD baudrate sample point set Error.";
                        samplePointError = true;
                        emit stateChanged(-1);
                    }
                }

            }

            if(!samplePointError)
            {
                emit stateChanged(2);
            }

        }
        else
        {
            displayErrorAndExit(status_writeFrameQueue, queueWriteSession, "nxCreateSession queue write");

            writeSessionCreateError = true;
            break;
        }
    }
    //////////////////////////////////////////////
    for(int i = 0; i < dbc_frames_r.count(); i++)
    {
        dbc_frame *frame = dbc_frames_r.at(i);

        QByteArray tempFrame = frame->m_name.toLocal8Bit();
        const char* frameStr = tempFrame.data();
        nxSessionRef_t queueReadSession;

        qDebug()<<"=======Read Frame:"<<frameStr<<", dlc:"<<frame->m_dlc;
        NIXNET_R_Queue_Thread *queueReadThread = queueReadThreadList.at(i);

        nxStatus_t status_readFrameQueue = nxCreateSession(dbcName, dbcCluster, frameStr,
                                        can_interface, nxMode_FrameInQueued,
                                        &queueReadSession);
        if (status_readFrameQueue == nxSuccess)
        {
            qDebug()<<"Create Queue Read Session Succeed!"<<queueReadSession;

            queueReadThread->setSession_readFrame_Queue(queueReadSession);

            sessionRefList_queueReadFrame.append(queueReadSession);

            if(i == 0 && frames_w.isEmpty())
            {
                nxStatus_t status_dbc;
                status_dbc = nxSetProperty(queueReadSession, nxPropSession_IntfCanFdBaudRate64, sizeof(u64), &canfd_baudrate);
                qDebug()<<"Status of set fd baudrate:"<<status_dbc;
                status_dbc = nxSetProperty(queueReadSession, nxPropSession_IntfCANTerm, sizeof(u32), &canTermination);
                qDebug()<<"Status of set can termination:"<<status_dbc;

                //Customize the Baudrate and FD Baudrate for Read Session
                if (customBD_on && queueReadSession != 0)
                {
                    if (setSessionIntfCustomBaudrate(queueReadSession, baudrate, samplePoint_BD*100))  //Sample Point = 70.00%
                    {
                        qDebug()<<"Queue read session baudrate sample point set ok!"<<samplePoint_BD;
                        samplePointError = false;
                    }
                    else
                    {
                        qCritical()<<"Queue read session baudrate sample point set Error.";
                        samplePointError = true;
                        emit stateChanged(-1);
                    }
                }

                if (customFDBD_on && queueReadSession != 0)
                {
                    queueReadThread->setIsArxml(true);
                    queueReadThread->setCanType(1);
                    qDebug()<<"Queue read thread in [FD Mode]";

                    if(setSessionIntfCustomFDBaudrate(queueReadSession, canfd_baudrate, samplePoint_FDBD*100))
                    {
                        qDebug()<<"Queue read session FD baudrate sample point set ok!"<<samplePoint_FDBD;
                        samplePointError = false;
                    }
                    else
                    {
                        qCritical()<<"Queue read session FD baudrate sample point set Error.";
                        samplePointError = true;
                        emit stateChanged(-1);
                    }
                }

            }

            if(!samplePointError)
            {
                //queueReadThread->start();

                emit stateChanged(2);
            }

        }
        else
        {
            displayErrorAndExit(status_readFrameQueue, queueReadSession, "nxCreateSession queue read");

            readSessionCreateError = true;
            break;
        }
    }

    // after all read session have been created then start
    foreach (NIXNET_R_Queue_Thread *queueReadThread, queueReadThreadList) {
        queueReadThread->start();
    }
    // after all write session have been created then start
    foreach (NIXNET_W_Queue_Thread *queueWriteThread, queueWriteThreadList) {
        queueWriteThread->start();
    }

    if(samplePointError || readSessionCreateError || writeSessionCreateError)
        return false;
    else
        return true;

}

void NIXNET_Thread::deviceDisconnect()
{
    qDebug()<<"stop read thread.";

    for(int i = 0; i < queueReadThreadList.count(); i++)
    {
        NIXNET_R_Queue_Thread *queueReadThread = queueReadThreadList.at(i);
        if(queueReadThread->isRunning())
        {
            queueReadThread->setIsStop(true);
            queueReadThread->quit();
            queueReadThread->wait();
        }
    }

    qDebug()<<"stop write thread.";

    for(int i = 0; i < queueWriteThreadList.count(); i++)
    {
        NIXNET_W_Queue_Thread *queueWriteThread = queueWriteThreadList.at(i);
        if(queueWriteThread->isRunning())
        {
            queueWriteThread->setIsStop(true);
            queueWriteThread->quit();
            queueWriteThread->wait();
        }
    }

    qDebug()<<"stop sessions.";
    for(int i = 0; i < sessionRefList_queueReadFrame.count(); i++)
    {
        nxSessionRef_t queueReadSession = sessionRefList_queueReadFrame.at(i);
        nxStop(queueReadSession, 1);
    }

    nxStatus_t status_queueReadFrame = 0, status_queueWriteFrame = 0, status_writeSigConv = 0;
    qDebug()<<"clear sessions.";

    for(int i = 0; i < sessionRefList_queueReadFrame.count(); i++)
    {
        nxSessionRef_t queueReadSession = sessionRefList_queueReadFrame.at(i);
        status_queueReadFrame  = nxClear(queueReadSession);
    }

    for(int i = 0; i < sessionRefList_queueWriteSigConv.count(); i++)
    {
        nxSessionRef_t writeSigConvSession = sessionRefList_queueWriteSigConv.at(i);
        status_writeSigConv  = nxClear(writeSigConvSession);
    }

    for(int i = 0; i < sessionRefList_queueWriteFrame.count(); i++)
    {
        nxSessionRef_t queueWriteSession = sessionRefList_queueWriteFrame.at(i);
        status_queueWriteFrame  = nxClear(queueWriteSession);
    }

    handleAfterSessionClosed();

    emit stateChanged(0);

}

void NIXNET_Thread::initSession()
{
    emit stateChanged(1);

    qDebug()<<" NI XNET Thread in Pre Session...";

    temp_path = path.toLocal8Bit();
    dbcPath = temp_path.data();

    QStringList fileList = path.split("/", QString::SkipEmptyParts);
    if (fileList.count() == 0) return;
    name = fileList.at(fileList.count()-1);
    name = name.split(".", QString::SkipEmptyParts)[0];
    temp_name = name.toLocal8Bit();
    dbcName = temp_name.data();

    temp_port = port.toLocal8Bit();
    can_interface = temp_port.data();
    //qDebug()<<can_interface;

    temp_cluster = cluster.toLocal8Bit();
    dbcCluster = temp_cluster.data();


    //////////////////////////////////////////////////////
    if (frames_r != "")
    {
        frames_r_List = frames_r.split(',');
        int numFrame_R = frames_r_List.count();

        for(int i = 0; i < numFrame_R; i++)
        {
            QSharedMemory *sm = new QSharedMemory(this);
            dbc_frame *frame = dbc_frames_r.at(i);
            if(!frame) continue;

            NIXNET_R_Queue_Thread *queueReadThread = new NIXNET_R_Queue_Thread();
            queueReadThread->setReadFrame(frame);
            queueReadThread->setSm(sm);

            connect(queueReadThread, SIGNAL(readFrameRawDataReceived(quint32,quint8*,quint8)), this, SIGNAL(readFrameRawDataReceived(quint32,quint8*,quint8)));
            connect(queueReadThread, SIGNAL(stateChanged(int)), this, SIGNAL(stateChanged(int)));

            QString keyName = "RP_" + port + "_" + QString::number(frame->m_id);
            sm->setKey(keyName);
            sm->create((frame->m_dlc < 8 ? 8 : frame->m_dlc) + 8);

            frame->smKey = keyName;
            readFrameSmHash.insert(keyName, sm);

            queueReadThreadList.append(queueReadThread);

        }
    }
    ////////////////////////////////////////////////////
    if (frames_w != "")
    {
        frames_w_List = frames_w.split(',');
        int frameNum  = frames_w_List.count();

        for(int i = 0; i < frameNum; i++)
        {
            QSharedMemory *sm = new QSharedMemory(this);
            dbc_frame *frame = dbc_frames_w.at(i);
            if(!frame) continue;

            QString keyName = "WP_" + port + "_" + QString::number(frame->m_id);
            sm->setKey(keyName);
            sm->create((frame->m_dlc < 8 ? 8 : frame->m_dlc) + 8);

            frame->smKey = keyName;
            writeFrameSmHash.insert(keyName, sm);

            u8 dlc = frame->m_dlc;
            u32 sizeFrame = nxFrameSize(dlc);
            qDebug()<<"writeFrame="<<frame->m_name<<"dlc="<<frame->m_dlc<<",sizeFrame="<<sizeFrame
                   <<", cyclicTime:"<<frame->m_cyclicTime;

            NIXNET_W_Queue_Thread *queueWriteThread = new NIXNET_W_Queue_Thread();
            queueWriteThread->setWriteFrame(frame);
            queueWriteThread->setSm(sm);
            queueWriteThread->setFrameQueueSize(writeFrameQueueSize);

            connect(queueWriteThread, SIGNAL(stateChanged(int)), this, SIGNAL(stateChanged(int)));

            queueWriteThreadList.append(queueWriteThread);
        }
    }

    xnetDbcModify();

}


void NIXNET_Thread::handleAfterSessionClosed()
{
    qDebug()<<"clear read thread.";
    for(int i = 0; i < queueReadThreadList.count(); i++)
    {
        NIXNET_R_Queue_Thread *queueReadThread = queueReadThreadList.at(i);
        if(queueReadThread)
        {

        }
    }
    qDeleteAll(queueReadThreadList);
    queueReadThreadList.clear();

    qDebug()<<"clear write thread.";
    for(int i = 0; i < queueWriteThreadList.count(); i++)
    {
        NIXNET_W_Queue_Thread *queueWriteThread = queueWriteThreadList.at(i);
        if(queueWriteThread)
        {

        }
    }
    qDeleteAll(queueWriteThreadList);
    queueWriteThreadList.clear();

}

void NIXNET_Thread::xnetDbcModify()
{
    nxStatus_t status_dbc;
    status_dbc = nxdbRemoveAlias(dbcName);
    qDebug()<<"Remove Alias Status = "<<status_dbc;

    status_dbc = nxdbAddAlias64(dbcName, dbcPath, baudrate);
    if (status_dbc == nxSuccess)
    {
        qDebug()<<"Add DBC Alias Succeed!"<<dbcName;
    }
    else
    {
        qCritical()<<"Add DBC Alias Error."<<status_dbc<<"\n";
        char statusString[1024];
        nxStatusToString(status_dbc, sizeof(statusString), statusString);
        qDebug()<<statusString;
    }

    status_dbc = nxdbOpenDatabase(dbcName, &databaseRef);
    if (status_dbc == nxSuccess)
    {
        qDebug()<<"Open Database Succeed!";
    }

    status_dbc = nxdbGetPropertySize(databaseRef, nxPropDatabase_ClstRefs, &sizeOfClusterRef);
    numOfClusterRef = sizeOfClusterRef/sizeof(nxDatabaseRef_t);
    clusterRef = new nxDatabaseRef_t[numOfClusterRef];
    status_dbc = nxdbGetProperty(databaseRef, nxPropDatabase_ClstRefs, sizeOfClusterRef, clusterRef);
    if (status_dbc == nxSuccess)
    {
        qDebug()<<"Get Cluster Ref Succeed! ClusterRef = "<<clusterRef[0];
    }

    status_dbc = nxdbSetProperty(clusterRef[0], nxPropClst_BaudRate64, sizeof(u64), &baudrate);
    if (status_dbc == nxSuccess)
    {
        qDebug()<<"Set Cluster CAN Standard Baudrate Succeed! ";

    }
    else
    {
        qCritical()<<"Set CAN Standard Baudrate Error."<<status_dbc<<"\n";
        char statusString[1024];
        nxStatusToString(status_dbc, sizeof(statusString), statusString);
        qCritical()<<statusString;
    }

    u64 Baudrate64 = 0;
    status_dbc = nxdbGetProperty(clusterRef[0], nxPropClst_BaudRate64, sizeof(u64), &Baudrate64);
    if (status_dbc == nxSuccess)
    {
        qDebug()<<"Get Cluster CAN Standard Baudrate Succeed! Value = "<<Baudrate64;
    }
    else
    {
        qCritical()<<"Get CAN Standard Baudrate Error."<<status_dbc<<"\n";
        char statusString[1024];
        nxStatusToString(status_dbc, sizeof(statusString), statusString);
        qDebug()<<statusString;
    }

    status_dbc = nxdbSetProperty(clusterRef[0], nxPropClst_CanIoMode, sizeof(u32), &canio_mode);
    if (status_dbc == nxSuccess)
    {
        qDebug()<<"Set Cluster CAN I/O Mode Succeed!";
    }
    else
    {
        qCritical()<<"Set CAN I/O Mode Error."<<status_dbc<<"\n";
        char statusString[1024];
        nxStatusToString(status_dbc, sizeof(statusString), statusString);
        qCritical()<<statusString;
    }

    u32 CANIOMode = 0;
    status_dbc = nxdbGetProperty(clusterRef[0], nxPropClst_CanIoMode, sizeof(u32), &CANIOMode);
    if (status_dbc == nxSuccess)
    {
        qDebug()<<"Get Cluster CAN I/O Mode Succeed! Value = "<<CANIOMode;

    }
    else
    {
        qCritical()<<"Get CAN I/O Mode Error."<<status_dbc<<"\n";
        char statusString[1024];
        nxStatusToString(status_dbc, sizeof(statusString), statusString);
        qCritical()<<statusString;
    }


    status_dbc = nxdbSetProperty(clusterRef[0], nxPropClst_CanFdBaudRate64, sizeof(u64), &canfd_baudrate);
    if (status_dbc == nxSuccess)
    {
        qDebug()<<"Set Cluster CAN FD Baudrate Succeed! ";

    }
    else
    {
        qCritical()<<"Set CAN FD Baudrate Error."<<status_dbc<<"\n";
        char statusString[1024];
        nxStatusToString(status_dbc, sizeof(statusString), statusString);
        qCritical()<<statusString;
    }

    u64 CANFDBaudrate64 = 0;
    status_dbc = nxdbGetProperty(clusterRef[0], nxPropClst_CanFdBaudRate64, sizeof(u64), &CANFDBaudrate64);
    if (status_dbc == nxSuccess)
    {
        qDebug()<<"Get Cluster CAN FD Baudrate Succeed! Value = "<<CANFDBaudrate64;
    }
    else
    {
        qCritical()<<"Get CAN FD Baudrate Error."<<status_dbc<<"\n";
        char statusString[1024];
        nxStatusToString(status_dbc, sizeof(statusString), statusString);
        qCritical()<<statusString;
    }

    //change write frame io mode as can fd
    nxDatabaseRef_t writeFrameRef;
    u32 writeFrameIOMode = nxCANioMode_CAN_FD;

    foreach (QString frame, frames_w_List) {

        char *frameName = frame.toLocal8Bit().data();

        status_dbc = nxdbFindObject(clusterRef[0], nxClass_Frame, frameName, &writeFrameRef);
        if(status_dbc != nxSuccess)
        {
            qCritical()<<"Find Frame object in dbc error, "<<frameName;
            continue;
        }

        u32 currentIOMode = 0;
        status_dbc = nxdbGetProperty(writeFrameRef, nxPropFrm_CANioMode, sizeof(u32), &currentIOMode);
        if(status_dbc != nxSuccess)
        {
            qCritical()<<"Get current frame CAN IO Mode fail."<<frameName;
            continue;
        }
        if(currentIOMode == 0)
        {
            qDebug()<<"Write frame current CAN IO Mode is CAN."<<frameName;
            continue;
        }

        status_dbc = nxdbSetProperty(writeFrameRef, nxPropFrm_CANioMode, sizeof(u32), &writeFrameIOMode);
        if(status_dbc == nxSuccess)
        {
            qDebug()<<"Change frame CAN IO Mode to CAN FD succeed, "<<frameName;
        }
    }

}


bool NIXNET_Thread::setSessionIntfCustomBaudrate(nxSessionRef_t sessionRef, u64 Baudrate, u32 samplePoint)
{
    if (!sessionRef)
        return false;

    u64 curBaudrate = 0;
    nxStatus_t status =  nxGetProperty(sessionRef, nxPropSession_IntfBaudRate64, sizeof(u64), &curBaudrate);
    qDebug()<<"Before Customize, Baudrate: 0x"<<QString::number(curBaudrate, 16).toUpper();

    if (curBaudrate != Baudrate)
        return false;

    u64 Tq = (u64)((qreal)1000000000/curBaudrate/40);
    qDebug()<<"Tq="<<Tq;

    u8 SyncSeg = 1;
    u8 SJW = 4;
    u8 TSEG1 = 33;
    u8 TSEG2 = 4;
    u32 nibble = 0xA0000000;

    TSEG1 = (u8)((float)samplePoint/10000 * 40) - SyncSeg - 1;
    TSEG2 = 40 - SyncSeg - (TSEG1+1) - 1;
    qDebug()<<"TSEG1="<<TSEG1<<", TSEG2="<<TSEG2;

    u64 customBaudrate = 0x0000000000000000;

    customBaudrate |= (TSEG2);
    customBaudrate |= (TSEG1 << 8);
    customBaudrate |= (SJW << 16);
    customBaudrate |= nibble;
    customBaudrate |= (Tq << 32);

    qDebug()<<"After Customize, Baudrate: 0x"<<QString::number(customBaudrate, 16).toUpper();
    status = nxSetProperty(sessionRef, nxPropSession_IntfBaudRate64, sizeof(u64), &customBaudrate);
    if (status == nxSuccess)
    {
        return true;
    }
    else
    {
        return false;
    }

}

bool NIXNET_Thread::setSessionIntfCustomFDBaudrate(nxSessionRef_t sessionRef, u64 FD_Baudrate, u32 samplePoint)
{
    if (!sessionRef)
        return false;

    u64 curFDBaudrate = 0;
    nxStatus_t status =  nxGetProperty(sessionRef, nxPropSession_IntfCanFdBaudRate64, sizeof(u64), &curFDBaudrate);
    qDebug()<<"Before Customize, FD Baudrate: 0x"<<QString::number(curFDBaudrate, 16).toUpper();

    if (curFDBaudrate != FD_Baudrate)
        return false;

    u8 Tq = (u8)((qreal)1000000000/curFDBaudrate/20);
    qDebug()<<"FD Tq="<<Tq;

    u8 SyncSeg = 1;
    u8 SJW = 3;
    u8 TSEG1 = 14;
    u8 TSEG2 = 3;
    u32 nibble = 0xA0000000;

    u8 TDC = 1;
    u8 TDCO = 15;
    u8 TDCF = 0;

    TSEG1 = (u8)((float)samplePoint/10000 * 20) - SyncSeg - 1;
    TSEG2 = 20 - SyncSeg - (TSEG1+1) - 1;
    qDebug()<<"FD TSEG1="<<TSEG1<<", FD TSEG2="<<TSEG2;

    u64 customFDBaudrate = 0x0000000000000000;

    customFDBaudrate |= (SJW);
    customFDBaudrate |= (TSEG2 << 4);
    customFDBaudrate |= (TSEG1 << 8);
    customFDBaudrate |= (Tq << 13);
    customFDBaudrate |= nibble;
    customFDBaudrate |= ((u64)TDCF << 32);
    customFDBaudrate |= ((u64)TDCO << 40);
    customFDBaudrate |= ((u64)TDC << 55);

    qDebug()<<"After Customize, FD Baudrate: 0x"<<QString::number(customFDBaudrate, 16).toUpper();
    status = nxSetProperty(sessionRef, nxPropSession_IntfCanFdBaudRate64, sizeof(u64), &customFDBaudrate);
    if (status == nxSuccess)
    {
        return true;
    }
    else
    {
        return false;
    }
}
