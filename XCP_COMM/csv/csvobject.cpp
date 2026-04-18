#include "csvobject.h"
#include <QApplication>
#include <QDebug>
#include "globals.h"

CSVObject::CSVObject(QObject *parent) :
    QObject(parent)
{

}

CSVObject::~CSVObject()
{

}


bool CSVObject::getRcdOn() const
{
    return rcdOn;
}

void CSVObject::setRcdOn(bool value)
{
    rcdOn = value;
}

quint32 CSVObject::getRate_ms() const
{
    return rate_ms;
}

void CSVObject::setRate_ms(const quint32 &value)
{
    rate_ms = value;
}

QString CSVObject::getCsv_FileName() const
{
    return csv_FileName;
}

void CSVObject::setCsv_FileName(const QString &value)
{
    csv_FileName = value;
}

QString CSVObject::getCsv_StorePath() const
{
    return csv_StorePath;
}

void CSVObject::setCsv_StorePath(const QString &value)
{
    csv_StorePath = value;
}

QString CSVObject::getCsv_FilePath() const
{
    return csv_FilePath;
}

void CSVObject::setCsv_FilePath(const QString &value)
{
    csv_FilePath = value;
}

bool CSVObject::csv_record_start()
{
    if(rcdOn) return true;

    readSetting();

    if (pamNameList.isEmpty())
        return false;

    QDateTime now = QDateTime::currentDateTime();
    QString date_str = now.date().toString("yyyyMMdd");
    QString time_str = now.time().toString("HHmmss");
    QString starttime_str = date_str + "_" + time_str;

    csv_FileName += "_"+ starttime_str + ".csv";

    csv_StorePath = csv_StorePath.isEmpty() ? (QApplication::applicationDirPath() + "/data/csv/") : csv_StorePath;
    csv_FilePath = csv_StorePath + "/" + csv_FileName;

    qInfo()<<"csv file path:"<<csv_FilePath;

    csv_File.reset(new QFile(csv_FilePath));

    if(!csv_File->open(QIODevice::ReadWrite | QIODevice::Append))
    {
        qDebug()<<"Open CSV File "<<csv_FileName<<" Error.";
        qDebug()<<csv_File->errorString();
        csv_File.reset();

        return false;
    }

    out.setDevice(csv_File.get());
    out.setCodec("GB2312");

    out << "Record Time,";
    for(int i = 0; i < pamCount; i++)
    {
        QString pamName = pamNameList.at(i);

        if(i == pamCount - 1)
        {
            out << pamName + "\n";
        }
        else
        {
            out << pamName + ",";
        }
    }
    out.flush();

    rcdOn = true;

    qInfo()<<"[CSV]Start csv record";

    return true;
}

void CSVObject::csv_record_stop()
{
    if(!rcdOn) return;

    rcdOn = false;

    if(csv_File)
    {
        csv_File->close();
        csv_File.reset();
    }

    qInfo()<<"[CSV]Stop csv record";
}

void CSVObject::csv_record_once()
{
    if(!rcdOn)
        return;

    QString curTimeStr = QDateTime::currentDateTime().toString("HH:mm:ss.z");

    out << curTimeStr + ",";
    for(int i = 0; i < pamCount; i++)
    {
        double value = pamValueList[i];

        if(i == pamCount - 1)
        {
            out << QString::number(value, 'g', 6) + "\n";
        }
        else
        {
            out << QString::number(value, 'g', 6) + ",";
        }
    }
    out.flush();
}

void CSVObject::csv_record_once(double valueList[], int size)
{
    if(!rcdOn)
        return;

    //qDebug()<<"in record_once(valueList)";
    QString curTimeStr = QDateTime::currentDateTime().toString("HH:mm:ss.z");

    out << curTimeStr + ",";

    for(int i = 0; i < size; i++)
    {
        double value = valueList[i];

        if(i == pamCount - 1)
        {
            out << QString::number(value, 'g', 6) + "\n";
        }
        else
        {
            out << QString::number(value, 'g', 6) + ",";
        }
    }
    out.flush();

}

void CSVObject::csv_record_avg(int avgTime_s, int sampleTime_ms)
{
    if(!rcdOn)
        return;

    resetPamValue();

    int measCount = avgTime_s * 1000 / sampleTime_ms;

    if(measCount <= 0)
        return;

    int count = measCount;
    //this->sampleTime_ms = sampleTime_ms;

    while(count > 0)
    {
        csv_record_shot();

        count--;
        wait(sampleTime_ms);
    }

    QString curTimeStr = QDateTime::currentDateTime().time().toString("HH:mm:ss.z");

    out << curTimeStr + ", ";

    QString record;
    record.append(curTimeStr + "    |    ");
    for(int i = 0; i < recordPamList.count(); i++)
    {
        if(i == recordPamList.count() - 1)
        {
            out << QString::number(pamValueList.at(i)/measCount, 'g', 6) + "\n";

            record.append(QString::number(pamValueList.at(i)/measCount, 'g', 6) + "\n");
        }
        else
        {
            out << QString::number(pamValueList.at(i)/measCount, 'g', 6) + ",";

            record.append(QString::number(pamValueList.at(i)/measCount, 'g', 6) + "    |    ");
        }
    }
    out.flush();

    emit showRecord(record);
}

void CSVObject::csv_record_shot()
{

    if(pamValueList.count() != recordPamList.count())
        return;

    for(int i = 0; i < recordPamList.count(); i++)
    {

        PARAM *pam = recordPamList.at(i);
        if(!pam)
            continue;

        qreal value = pam->getValue();
        qreal sum = pamValueList.at(i) + value;

        //qDebug()<<"i="<<i<<", sum="<<sum;

        pamValueList.replace(i, sum);
    }

}

void CSVObject::resetPamValue()
{
    pamValueList.clear();

    for(int i = 0; i < recordPamList.count(); i++)
    {
        pamValueList.append(0);
    }

}

void CSVObject::wait(quint32 msec)
{
    QEventLoop loop;//定义一个新的事件循环
    QTimer::singleShot(msec, &loop, SLOT(quit()));//创建单次定时器，槽函数为事件循环的退出函数
    loop.exec();//事件循环开始执行，程序会卡在这里，直到定时时间到，本循环被退出
}

void CSVObject::readSetting()
{
    QString recordSetFilePath = QApplication::applicationDirPath() + "/work/" + workName + "/record/csv_setting.ini";
    QSettings setting(recordSetFilePath, QSettings::IniFormat);

    qInfo()<<"read csv setting:" + setting.fileName();

    csv_FileName = (setting.value("CsvFileName").toString());
    csv_StorePath = (setting.value("CsvFileDir").toString());
    csv_FilePath = (setting.value("CsvFullFilePath").toString());

    QString pamNames = setting.value("CsvPamNames").toString();
    QString modNames = setting.value("CsvModNames").toString();

    QStringList pamNameList, modNameList;

    pamNameList = pamNames.split("|");
    modNameList = modNames.split("|");

    if(pamNameList.count() != modNameList.count())
        return;

    this->recordPamList.clear();
    this->pamNameList.clear();
    for(int i = 0; i < pamNameList.count(); i++)
    {
        QString pamKey = pamNameList.at(i) + "." + modNameList.at(i);

        PARAM *pam = Globals::pamHash.value(pamKey);

        if(pam)
        {
            this->pamNameList.append(pamNameList.at(i));
            this->recordPamList.append(pam);
        }
    }

    this->pamCount = this->recordPamList.count();

}

void CSVObject::setWorkName(const QString &value)
{
    workName = value;
}

void CSVObject::setRecordPamList(const QList<PARAM *> &value)
{
    recordPamList = value;
}

QStringList CSVObject::getPamNameList() const
{
    return pamNameList;
}

void CSVObject::setPamNameList(const QStringList value)
{
    pamNameList = value;
    pamCount = pamNameList.count();
}

void CSVObject::setFileName(QString fileName)
{
    this->csv_FileName = fileName;
}

void CSVObject::clearPams()
{
    pamNameList.clear();
    qDebug()<<"csv pam clear.";
}

void CSVObject::addPamName(QString value)
{
    pamNameList.append(value);
    pamCount += 1;

    qDebug()<<"csv add pam name: "<<value;
}

void CSVObject::setPamNames(QStringList nameList)
{
    pamNameList.clear();

    pamNameList = nameList;
    pamCount = pamNameList.count();
}

void CSVObject::clearPamValues()
{
    pamValueList.clear();

}

void CSVObject::addPamValue(double value)
{
    pamValueList.append(value);
}

void CSVObject::setPamValues(QList<double> valueList)
{
    pamValueList = valueList;
}

void CSVObject::setPamValues(QStringList valueList)
{
    pamValueList.clear();

    for(int i = 0; i < pamCount; i++)
    {
        double value = QString(valueList.at(i)).toDouble();
        pamValueList.append(value);
    }

}
