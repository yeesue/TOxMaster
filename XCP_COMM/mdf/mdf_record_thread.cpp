#include "mdf_record_thread.h"
#include <QApplication>

MDF_Record_Thread::MDF_Record_Thread(QObject *parent) :
    QObject(parent)
{

    //timer = new QTimer;
    //timer->setInterval(1000);
    //connect(timer, SIGNAL(timeout()), this, SLOT(recordTimeUpdated()));

    mdf3FileName = "mdf_data";

    QString appPath = QApplication::applicationDirPath();
    mdf3FileStorePath = appPath + "/data/";
    mdf3FileTempPath = appPath + "/temp/";

    cycleRcdTimer = new QTimer(this);
    cycleRcdTimer->setInterval(500);
    connect(cycleRcdTimer, SIGNAL(timeout()), this, SLOT(Slt_cycleRcdMonitor()));
    cycleRcdTimer->start();
}

MDF_Record_Thread::~MDF_Record_Thread()
{

}

void MDF_Record_Thread::addDgPams(QString dgName, QList<PARAM *> pams, quint32 dgBlockSize)
{
    if(dgName.isEmpty() || pams.isEmpty() || dgBlockSize == 0)
        return;
    if(dgNameList.indexOf(dgName) != -1)
        return;
    qDebug()<<"adding DG: dgName"<<dgName<<", pamsNum:"<<pams.count()<<", size:"<<dgBlockSize;

    this->dgPamsHash.insert(dgName, pams);
    this->dgBlockSizeHash.insert(dgName, dgBlockSize);
    dgNameList.append(dgName);
    dgCount++;

    allPams.append(pams);
}

bool MDF_Record_Thread::isRecording()
{
    return record_on;
}

void MDF_Record_Thread::setRecordFileName(QString name)
{
    this->mdf3FileName = name;
}

QString MDF_Record_Thread::getRecordFileFullPath()
{
    return mdf3FileFullName ;
}

void MDF_Record_Thread::setRecordStatus_v2(bool recordFlag)
{
    if(record_on && !recordFlag)
    {
        mdf_record_stop_v2();
    }
    else if(!record_on && recordFlag)
    {
        mdf_record_start_v2();
    }
}

void MDF_Record_Thread::setPollDataBlockSize(int size)
{
    this->sizeOfRecordBlock = size;
}

bool MDF_Record_Thread::mdf_record_start_v2()
{
    QDateTime now = QDateTime::currentDateTime();
    QString date_str = now.date().toString("yyyyMMdd");
    QString time_str = now.time().toString("HHmmss");
    QString starttime_str = date_str + "_" + time_str;

    QString fileName = mdf3FileName + "_"+ starttime_str + ".dat";
    for(int i = 0; i < dgCount; i++)
    {
        QString dgName = dgNameList.at(i);
        QString tempFileName = "Temp_" + dgName + "_" + fileName;
        QString mdf3FileFullTempPath = mdf3FileTempPath +  "" + tempFileName;
        dgTempFilePathHash.insert(dgName, mdf3FileFullTempPath);

        QFile *tempFile = new QFile(mdf3FileFullTempPath);
        if (!tempFile->open(QIODevice::WriteOnly | QIODevice::Append))
        {
            qDebug()<<"Open mdf3 tempFile Error :"<<tempFile->fileName();
            return false;
        }
        dgTempFileHash.insert(dgName, tempFile);

        QDataStream *out = new QDataStream;
        out->setDevice(tempFile);
        dgTempStreamHash.insert(dgName, out);

        quint32 numRecord = 0;
        (*out) << numRecord;

    }


    QString mdf3FileFullPath = mdf3FileStorePath + "" + fileName;
    this->mdf3FileFullName = mdf3FileFullPath;

    mdf3_ins = new MDF3;

    //create mdf file and add ID, HD, TX
    mdf3_ins->mdf3_createAndOpenFile(mdf3FileFullPath);
    mdf3_ins->mdf3_addIDHDToFile(dgCount);

    time.restart();

    timer = new QTimer;
    timer->setInterval(1000);
    connect(timer, SIGNAL(timeout()), this, SLOT(recordTimeUpdated()));
    timer->start();


    timeStamp_start = 0;
    timeStamp_on = 0;
    time_on = 0;

    record_on = true;
    recordFinished = false;
    qDebug()<<"=====Start recording(v2)...=====";

    return true;
}

bool MDF_Record_Thread::mdf_record_stop_v2()
{
    qDebug()<<"=====Stop recording(v2)...=====";

    record_on = false;

    timer->stop();
    delete timer;
    timer = NULL;

    timeStamp_start = 0;
    timeStamp_on = 0;
    time_on = 0;

    quint64 curPos = mdf3_ins->mdf3_getFileCurPos();

    for(int i = 0; i < dgCount; i++)
    {
        QString dgName = dgNameList.at(i);
        qDebug()<<"In DG :"<<dgName;
        QFile *tempFile = dgTempFileHash.value(dgName);
        QString tempFilePath = dgTempFilePathHash.value(dgName);
        quint32 numRecord = dgNumRecordHash.value(dgName);
        QDataStream* out = dgTempStreamHash.value(dgName);
        quint32 sizeBlock = dgBlockSizeHash.value(dgName);
        QList<PARAM*> pamList = dgPamsHash.value(dgName);

        qDebug()<<"current temp file path:"<<tempFilePath;

        if(tempFile->seek(0))
        {
            (*out) << numRecord;
        }

        tempFile->flush();
        tempFile->close();

        if(tempFile->open(QIODevice::ReadOnly))
        {
            qDebug()<<"Open temp file error.";
        }

        QDataStream in(tempFile);
        quint32 numOfRcd = 0;
        in >> numOfRcd;

        qDebug()<<"Num of record:"<<numOfRcd<<", size of block:"<<sizeBlock;

        quint64 sizeAllRecord = numOfRcd * sizeBlock;
        bool lastDG = (i == (dgCount - 1)) ? true : false;
        mdf3_ins->mdf3_addDGToFile(curPos, sizeAllRecord, sizeBlock, pamList, lastDG);

        qDebug()<<"stop rcd: add dg done";
        int sizeOnce = 1024 * 100;
        char *data = new char[sizeOnce];

        quint64 leftSize = sizeAllRecord;
        quint64 finishedSize = 0;
        if (numOfRcd)
        {
            while (leftSize >=  (quint64)sizeOnce)
            {
                int readSize = in.readRawData(data, sizeOnce);
                //qDebug()<<"add raw data..."<<readSize;

                //if(readSize <= 0)
                //    break;

                mdf3_ins->mdf3_addRawDataToFile(data, sizeOnce);

                leftSize -= sizeOnce;
                finishedSize += readSize;
            }

            int lastSize = in.readRawData(data, leftSize);
            //qDebug()<<"read last raw data size:"<<lastSize;

            mdf3_ins->mdf3_addRawDataToFile(data, leftSize);

            leftSize -= lastSize;
            finishedSize += lastSize;

            qDebug()<<"read from temp file size="<<finishedSize;
        }

        curPos = mdf3_ins->mdf3_getFileCurPos();
        qDebug()<<"current mdf file pos="<<curPos;

        delete[] data;

        //temp file close and remove
        tempFile->flush();
        tempFile->close();
        tempFile->remove();

        delete out;
        delete tempFile;
    }

    mdf3_ins->mdf3_closeFile();

    delete mdf3_ins;

    recordFinished = true;

    dgTempFileHash.clear();
    dgTempFileHash.clear();
    dgTempStreamHash.clear();
    dgNumRecordHash.clear();

    return true;
}

void MDF_Record_Thread::mdf_record_slot_v2(quint8 *buf, quint32 size, QString dgName)
{
    if(!record_on)
        return;

    if(dgNameList.indexOf(dgName) == -1)
        return;

    if(timeStamp_start == 0)
    {
        timeStamp_start = *(quint64*)buf;
        time_on = 0;
    }
    else
    {
        if(*(quint64*)buf < timeStamp_start)
            return;

        timeStamp_on = *(quint64*)buf - timeStamp_start;
        time_on = (qreal)timeStamp_on / 10000000.0;            //nxFrame中的timestamp单位为100ns
    }
    //qDebug()<<"dg:"<<dgName<< ",time:"<<time_on<<", timeStamp:"<<timeStamp_on;

    QDataStream* out = dgTempStreamHash.value(dgName);

    //write time and data to temp file
    out->writeRawData((char*)&time_on, 8);
    out->writeRawData((char*)(buf+8), size-8);

    quint32 numRecord_last = dgNumRecordHash.value(dgName);
    dgNumRecordHash.insert(dgName, numRecord_last + 1);

    if(size < dgBlockSizeHash.value(dgName))
    {
        dgBlockSizeHash.insert(dgName, size);
    }

    delete[] buf;
}

void MDF_Record_Thread::mdf_record_slot_poll(quint8 *buf, quint32 size, int pollIndex)
{
    if(!record_on || pollIndex != -1 || pollPams.isEmpty())
        return;

    quint64 timeStamp_poll = 0;
    qreal timeOn_poll = 0;
    if(timeStamp_start == 0)
    {
        timeStamp_start = *(quint64*)buf;
    }
    else
    {
        timeStamp_poll = *(quint64*)buf - timeStamp_start;
        timeOn_poll = (qreal)timeStamp_poll / 10000000.0;
    }

    //write time and data to poll temp file
    out_poll->writeRawData((char*)&timeOn_poll, 8);
    out_poll->writeRawData((char*)(buf+8), size-8);

    numOfRecord_poll++;
    sizeOfRecordBlock = size;

    delete[] buf;
}

void MDF_Record_Thread::recordTimeUpdated()
{
    int secs = time_on;
    int hour = secs / 3600;
    int min = (secs % 3600) / 60;
    int second = secs % 60 ;
    QString recordTimeStr = QString("%1:%2:%3")
                            .arg(hour,2,10,QLatin1Char('0'))
                            .arg(min,2,10,QLatin1Char('0'))
            .arg(second,2,10,QLatin1Char('0'));
    //qDebug()<<"record Time.";

    emit recordTime(recordTimeStr);
}

void MDF_Record_Thread::Slt_cycleRcdMonitor()
{
    if(!record_on || !rcdSet.cyclicRecord || rcdSet.cycleNum < 1 | rcdSet.singleCycleTime_s < 3) return;

    if(time_on >= rcdSet.singleCycleTime_s)
    {
        cycleTransition = true;

        setRecordStatus_v2(false);

        curCycle += 1;
        emit cycleNumUpdated(curCycle);

        if(curCycle > rcdSet.cycleNum)
        {
            cycleTransition = false;
            curCycle = 1;
            return;
        }

        setRecordStatus_v2(true);

        cycleTransition = false;
    }
}

void MDF_Record_Thread::loadRecordSetting(QString workName)
{
    QString workRecordDir = QApplication::applicationDirPath() + "/work/" + workName + "/record";
    QString recordSetFilePath = workRecordDir + "/record_setting.ini";
    QSettings setting(recordSetFilePath, QSettings::IniFormat);

    qInfo()<<"read record setting:" + setting.fileName();

    rcdSet.recordFileName = setting.value("RecordDataFileName").toString();
    rcdSet.cyclicRecord = setting.value("CycleRecord").toBool();
    rcdSet.cycleNum = setting.value("NumberOfCycle").toUInt();
    rcdSet.singleCycleTime_s = setting.value("SingleCycleTime").toUInt();

    mdf3FileName = rcdSet.recordFileName;
}

void MDF_Record_Thread::setRcdSet(const RecordStruct &value)
{
    rcdSet = value;
}

void MDF_Record_Thread::setWorkName(const QString &value)
{
    workName = value;

    loadRecordSetting(workName);
}

bool MDF_Record_Thread::getRecordFinished() const
{
    return recordFinished;
}

bool MDF_Record_Thread::getRecord_on() const
{
    return record_on;
}


void MDF_Record_Thread::setPollPams(const QList<PARAM *> &value)
{
    pollPams = value;
}


void MDF_Record_Thread::setPamsBlockSizeHash(const QHash<quint16, quint32> &value)
{
    recordBlockSizeHash = value;
}

void MDF_Record_Thread::setDaqSmHash(const QHash<quint16, QSharedMemory *> &value)
{
    daqSmHash = value;

}

void MDF_Record_Thread::setPamsHash(const QHash<quint16, QList<PARAM *> > &value)
{
    pamsHash = value;
    dgList = pamsHash.uniqueKeys();
    dgCount = dgList.count();
}


