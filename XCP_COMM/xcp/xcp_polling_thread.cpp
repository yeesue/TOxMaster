#include "xcp_polling_thread.h"

XCP_Polling_Thread::XCP_Polling_Thread(QObject *parent, XCPMaster *master):
    QThread(parent), xcpMaster(master)
{
    connect(this, &XCP_Polling_Thread::pollingSucceed, this, &XCP_Polling_Thread::pollingSucceedSlot);
}

XCP_Polling_Thread::~XCP_Polling_Thread()
{
    stop();
    wait();
}

void XCP_Polling_Thread::stop()
{
    QMutexLocker locker(&m_mutex);
    m_running = false;
    isStop = true;
    m_condition.wakeAll();
}

void XCP_Polling_Thread::setMeasPamList(QList<A2L_VarMeas *> measList)
{
    this->measPamList = measList;
}

void XCP_Polling_Thread::setCharPamList(QList<A2L_VarChar *> charList)
{
    this->charPamList = charList;
}

void XCP_Polling_Thread::setPollRunFlag(bool runFlag)
{
    this->pollRunFlag = runFlag;
}

void XCP_Polling_Thread::setSMKey(QString smKeyRead)
{
    this->smKeyRead = smKeyRead;
}

void XCP_Polling_Thread::setMeasRate(int rate_ms)
{
    this->measRate_ms = rate_ms;
}

void XCP_Polling_Thread::run()
{
    QElapsedTimer timer;
    timer.start();

    QSharedMemory sm;
    sm.setKey(smKeyRead);

    if(!xcpMaster || !xcpMaster->getXcpSessionConnectedStatus())
    {
        qDebug()<<"XCP init or setup error, not ready to establish communication. Try reconnect!";
    }


    bool cali_last, polling_last;
    while (m_running)
    {
        // execute the diff calibration
        if(caliPairList.count() > 0)
        {
            cali_last = caliOk;
            Cali_Pair pair = caliPairList.first();
            if(xcpMaster->XCP_Cal_VALUE(pair.charVar, pair.data, pair.size))
            {
                //qDebug()<<"In pool thread, cal value:"<<pair.charVar->Name;
                caliPairList.removeFirst();
                delete pair.data;

                caliOk = true;
            }
            else
            {
                caliOk = false;
            }
            if(cali_last != caliOk)
            {
                emit caliStatusChanged(caliOk);
            }
        }

        // execute the diff map calibration
        if(mapCaliPairList.count() > 0)
        {
            cali_last = caliOk;
            Cali_Pair pair = mapCaliPairList.first();

            if(xcpMaster->XCP_Cal_VALUE(pair.charVar, pair.data, pair.size, pair.offset))
            {
                mapCaliPairList.removeFirst();
                delete pair.data;

                caliOk = true;
            }
            else
            {
                caliOk = false;
            }
            if(cali_last != caliOk)
            {
                emit caliStatusChanged(caliOk);
            }
        }

        //qreal time_abs = time.elapsed() * 0.001;
        if(!pollRunFlag)
        {
            // 等待轮询标志变为true，避免忙等待
            QMutexLocker locker(&m_mutex);
            m_condition.wait(&m_mutex, 10);
            continue;
        }

        // 高精度定时器控制轮询间隔
        qint64 elapsed = timer.elapsed();
        qint64 targetInterval = measRate_ms;
        
        if (elapsed < targetInterval) {
            msleep(targetInterval - elapsed);
        }
        
        timer.restart();

        for(int i = 0; i < measPamList.count(); i++)
        {
            polling_last =pollingOk;
            pollingOk = xcpMaster->XCP_Poll_Meas(measPamList.at(i));

            if(polling_last != pollingOk)
            {
                emit pollingStatusChanged(pollingOk);
            }
        }

        emit measPamsValueUpdated();

        // mdf record
        if(recordOn && !measPamList.isEmpty())
        {
            quint8 *buf = new quint8[smReadSize + 8];

            copyFromSharedMemory(&sm, 0, (char*)buf, smReadSize + 8);

            /*
            if(!sm.isAttached())
            {
                if(!sm.attach())
                {
                    continue;
                }
            }
            sm.lock();
            memcpy(buf, (quint8*)sm.data(), smReadSize + 8);
            sm.unlock();
            */

            emit pollDataForRecord(buf, smReadSize + 8, -1);
            emit pollDataForRecord(buf, smReadSize + 8, "RP_POLL_" + xcpName);
        }

    }
}

void XCP_Polling_Thread::addCaliActionSlot(QVariant caliData)
{
    Cali_Pair pair = caliData.value<Cali_Pair>();
    caliPairList.append(pair);
}

void XCP_Polling_Thread::addMapCaliActionSlot(QVariant mapCaliData)
{
    Cali_Pair pair = mapCaliData.value<Cali_Pair>();
    mapCaliPairList.append(pair);
    //qDebug()<<"add map cali pair offset:"<<pair.offset;
}

void XCP_Polling_Thread::pollingSucceedSlot(qreal absTime)
{
    QSharedMemory sm;
    sm.setKey(smKeyRead);

    if(smKeyRead.isEmpty())
        return;

    if(!sm.isAttached())
    {
        if(!sm.attach())
        {
            qDebug()<<"Unable attach to XCP polling read sharedmemory.";
            return;
        }
    }
    sm.lock();
    memcpy((char*)sm.data(), (char*)&absTime, 8);
    sm.unlock();

    emit measPamsValueUpdated();
}

void XCP_Polling_Thread::sltPollingStatusActive(bool status)
{
    this->pollRunFlag = status;
}

void XCP_Polling_Thread::setIsStop(bool value)
{
    isStop = value;
}

void XCP_Polling_Thread::setXcpName(const QString &value)
{
    xcpName = value;
}

void XCP_Polling_Thread::setRecordOn(bool value)
{
    recordOn = value;
}

void XCP_Polling_Thread::setSmRead(QSharedMemory *value)
{
    smRead = value;
}

void XCP_Polling_Thread::setSmReadSize(int value)
{
    smReadSize = value;
}

