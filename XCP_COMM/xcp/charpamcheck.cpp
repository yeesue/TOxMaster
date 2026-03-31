#include "charpamcheck.h"

CharPamCheck::CharPamCheck(QObject *parent, XCPMaster *master) :
    QThread(parent), xcpMaster(master)
{

}

void CharPamCheck::setCharPamList(QList<A2L_VarChar *> charList)
{
    this->charPamList = charList;
}

void CharPamCheck::setCharCheckRunFlag(bool runFlag)
{
    this->charCheckRunFlag = runFlag;
}

void CharPamCheck::setSMKey(QString smKeyWrite)
{
    this->smKeyWrite = smKeyWrite;
}

bool CharPamCheck::charArrayEqual(char *one, char *two, int size)
{
    int offset = 0;
    while (offset < size)
    {
        if(*(one+offset) != *(two+offset))
            return false;
        offset++;
    }
    return true;
}

void CharPamCheck::run()
{
    QTime time;
    time.start();

    QSharedMemory smWrite;
    smWrite.setKey(smKeyWrite);
    smWrite.attach();

    int size = smWriteSize + 8;
    CharArrayPtr data_last = makeCharArray(size);
    CharArrayPtr data_new = makeCharArray(size);

    //init the data memory
    smWrite.lock();
    memcpy(data_last.data(), (char*)smWrite.data(), size);
    memcpy(data_new.data(), (char*)smWrite.data(), size);
    smWrite.unlock();

    while (!isStop)
    {
        msleep(checkRate_ms);

        if(!charCheckRunFlag) continue;
        if(!xcpMaster) continue;
        if(!xcpMaster->getXcpSessionConnectedStatus()) continue;

        copyFromSharedMemory(&smWrite, 0, (quint8*)data_new.data(), size);
        /*
        if(!smWrite.isAttached())
        {
            if(!smWrite.attach())
            {                
                qDebug()<<"Unable attach to write sharedmemory.";
                continue;
            }
        }
        smWrite.lock();
        memcpy(data_new, (char*)smWrite.data(), size);
        smWrite.unlock();
        */

        if(!charArrayEqual(data_last.data(), data_new.data(), 8))
        {
            //qDebug()<<"SM Write time memory, find diff.";
        }

        for(int i = 0; i < charPamList.count(); i++)
        {
            A2L_VarChar *charVar = charPamList.at(i);

            int startByte = charVar->StartByte+8;
            int dataSize = charVar->DataSizeAG;

            if(!charArrayEqual(data_last.data()+startByte, data_new.data() + startByte, dataSize))
            {
                //find diff, need to make a calibration
                //qDebug()<<"Char Var :"<<charVar->Name<<", find diff.";

                Cali_Pair pair;
                pair.charVar = charVar;
                pair.size = dataSize;
                pair.data = new char[dataSize];
                memcpy(pair.data, data_new.data()+startByte, dataSize);

                QVariant caliData;
                caliData.setValue(pair);

                emit addCaliAction(caliData);

                //replace old data with new data
                memcpy(data_last.data()+startByte, data_new.data()+startByte, dataSize);
            }
        }        

    }

    // 智能指针自动释放，无需手动delete
}

void CharPamCheck::setIsStop(bool value)
{
    isStop = value;
}

void CharPamCheck::setSmWriteSize(const quint32 &value)
{
    smWriteSize = value;
}

//Map Characteristics value check thread

MapCharPamCheckThread::MapCharPamCheckThread(QObject *parent, XCPMaster *master) :
    QThread(parent), xcpMaster(master)
{

}

void MapCharPamCheckThread::setMapCharPamList(QList<A2L_VarChar *> mapCharList)
{
    this->mapCharPamList = mapCharList;
}

void MapCharPamCheckThread::setMapCharSMHash(QHash<A2L_VarChar *, QSharedMemory *> mapSMHash)
{
    this->mapSMHash = mapSMHash;
}

void MapCharPamCheckThread::setMapCharCheckRunFlag(bool runFlag)
{
    this->mapCharCheckRunFlag = runFlag;
}


bool MapCharPamCheckThread::charArrayEqual(char *one, char *two, int size)
{
    int offset = 0;
    while (offset < size)
    {
        if(*(one+offset) != *(two+offset))
        {

            return false;
        }

        offset++;
    }
    return true;
}

void MapCharPamCheckThread::run()
{
    QTime time;
    time.start();

    buffer_last.clear();
    buffer_new.clear();
    bufferHash_last.clear();
    bufferHash_new.clear();

    //建立缓存空间，初始化空间
    foreach (A2L_VarChar* charVar, mapCharPamList)
    {

        QSharedMemory *sm = mapSMHash.value(charVar);
        int size = (charVar->zCount * charVar->DataSizeAG + 8);
        //int size = sm->size();
        if(!sm->isAttached())
        {
            if(!sm->attach())
            {
                qDebug()<<"Unable attach to map write sharedmemory.";
                return;
            }
        }

        CharArrayPtr data_last = makeCharArray(size);
        CharArrayPtr data_new = makeCharArray(size);

        buffer_last.append(data_last);
        buffer_new.append(data_new);

        bufferHash_last.insert(charVar, data_last);
        bufferHash_new.insert(charVar, data_new);

        sm->lock();
        memcpy(data_last.data(), (char*)sm->data(), size);        
        sm->unlock();

        memcpy(data_new.data(), data_last.data(), size);
    }

    //独立线程，周期性比对
    while (!isStop)
    {
        msleep(checkRate_ms);

        if(!mapCharCheckRunFlag) continue;

        if(!xcpMaster)
            continue;
        if(!xcpMaster->getXcpSessionConnectedStatus())
            continue;

        int index = 0;
        foreach (A2L_VarChar* charVar, mapCharPamList)
        {

            QSharedMemory *sm = mapSMHash.value(charVar);
            int size = (charVar->zCount * charVar->DataSizeAG + 8);
            //qDebug()<<"sm key:"<<sm->key();


            CharArrayPtr data_last = bufferHash_last.value(charVar);
            CharArrayPtr data_new = bufferHash_new.value(charVar);

            copyFromSharedMemory(sm, 0, (quint8*)data_new.data(), size);

            /*
            if(!sm->isAttached())
            {
                if(!sm->attach())
                {
                    qDebug()<<"Unable attach to map write sharedmemory.";
                    continue;
                }
            }
            sm->lock();
            memcpy(data_new, (char*)sm->data(), size);
            sm->unlock();
            */

            if(!charArrayEqual(data_last.data(), data_new.data(), 8))
            {
                //qDebug()<<"SM map write time memory, find diff.";

                //qDebug()<<"last time:"<<*((double*)data_last)<<", new time:"<<*((double*)data_new);
            }

            int zCount = charVar->zCount;
            quint32 dataSizeAG = charVar->DataSizeAG;

            int offset = 8;
            for(int i = 0; i < zCount; i++)
            {

                if(!charArrayEqual(data_last.data()+offset, data_new.data()+offset, dataSizeAG))
                {
                    //find diff, need to make a map calibration
                    //qDebug()<<"Map Char Var :"<<charVar->Name<<", find diff at index: "<<i<<", addr:"<<QString::number(charVar->ECUAddress, 16);
                    Cali_Pair pair;
                    pair.charVar = charVar;
                    pair.size = dataSizeAG;
                    pair.offset = (offset - 8);
                    pair.data = new char[dataSizeAG];
                    memcpy(pair.data, data_new.data()+offset, dataSizeAG);

                    QVariant mapCaliData;
                    mapCaliData.setValue(pair);

                    emit addMapCaliAction(mapCaliData);

                    //replace old data with new data
                    memcpy(data_last.data()+offset, data_new.data()+offset, dataSizeAG);
                }

                offset += dataSizeAG;
            }

        }

        index++;
    }

    // 智能指针自动释放，无需手动delete
    buffer_last.clear();
    buffer_new.clear();
    bufferHash_last.clear();
    bufferHash_new.clear();
}

void MapCharPamCheckThread::setIsStop(bool value)
{
    isStop = value;
}
