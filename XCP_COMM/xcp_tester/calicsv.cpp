#include "calicsv.h"
#include <QDebug>

CaliCsv::CaliCsv(QObject *parent, QString filePath) :
    QObject(parent),
    caliCsvFilePath(filePath)
{
    readFromCsv(caliCsvFilePath);
}

CaliCsv::~CaliCsv()
{

}

bool CaliCsv::readFromCsv(QString csvFilePath)
{
    if(csvFilePath.isEmpty())
    {
        return false;
    }

    QFile calCsvFile(csvFilePath);
    if(!calCsvFile.open(QIODevice::ReadOnly))
    {
        qDebug()<<"Cal csv file open error.";
        return false;
    }
    QTextStream *out = new QTextStream(&calCsvFile);
    QStringList lines = out->readAll().split("\r\n");
    calCsvFile.close();

    int lineCount = lines.count();
    qDebug()<<"cal csv file lines count:"<<lines.count();

    calList.clear();
    int index = 0;
    while(index < lineCount)
    {
        QString line = lines.at(index);
        line.replace("\"", "");

        if(line.isEmpty() || line == "\r")
        {
            index++;
            continue;
        }

        if(line.at(0) == "*")
        {
            index++;
            continue;
        }

        QStringList strList = line.split(",");
        if(strList.count() != 2 || strList.at(0) != "")
        {
            index++;
            continue;
        }

        QString charName = strList.at(1);
        index += 2;
        QString valueLine = lines.at(index);
        valueLine.replace("\"", "");
        valueLine.replace(":", "");
        QStringList valueStrList = valueLine.split(",");
        if(valueStrList.count() < 3)
        {
            index++;
            continue;
        }

        QString type = valueStrList.at(0);
        //qDebug()<<"=====CharName:"<<charName<<", type:"<<type;
        CalInfo calIns;
        calIns.charName = charName;
        calIns.type = type;

        if(type == "VALUE")
        {
            QString unitStr = valueStrList.at(1);
            QString valueStr = valueStrList.at(2);
            //qDebug()<<"unit:"<<unitStr<<", setValue:"<<valueStr;
            calIns.unit = unitStr;
            calIns.zValueStrArray.append(valueStr);
        }
        else if(type == "VAL_BLK")
        {
            QString unitStr = valueStrList.at(1);

            QStringList blkValueStrs;
            for(int i = 2; i < valueStrList.count(); i++)
            {
                blkValueStrs.append(valueStrList.at(i));
            }
            //qDebug()<<"unit:"<<unitStr <<", setValues:"<< blkValueStrs;
            calIns.unit = unitStr;
            calIns.zValueStrArray.append(blkValueStrs);
        }
        else if(type == "AXIS_PTS")
        {
            QString unitStr = valueStrList.at(1);

            QStringList ptsValueStrs;
            for(int i = 2; i < valueStrList.count(); i++)
            {
                ptsValueStrs.append(valueStrList.at(i));
            }
            //qDebug()<<"unit:"<<unitStr <<", aXisPts:"<< ptsValueStrs;
            calIns.unit = unitStr;
            calIns.zValueStrArray.append(ptsValueStrs);
        }
        else if(type == "CURVE")
        {
            QString xAxisName = valueStrList.at(1);

            QStringList xAxisValueStrs;
            for(int i = 2; i < valueStrList.count(); i++)
            {
               xAxisValueStrs.append(valueStrList.at(i));
            }
            //qDebug()<<"xAxis:"<<xAxisName << ", xAxisValues:"<<xAxisValueStrs;

            index++;
            QString zValueLine = lines.at(index);
            zValueLine.replace("\"", "");
            zValueLine.replace(":", "");
            QStringList zValueStrList = zValueLine.split(",");
            QString unitStr = zValueStrList.at(0);
            QStringList zValueStrs;
            for(int i = 2; i < zValueStrList.count(); i++)
            {
                zValueStrs.append(zValueStrList.at(i));
            }
            //qDebug()<<"unit:"<<unitStr<<", zValues:"<<zValueStrs;

            calIns.unit = unitStr;
            calIns.xAxisName = xAxisName;
            calIns.xAxisStrArray.append(xAxisValueStrs);
            calIns.zValueStrArray.append(zValueStrs);
        }
        else if(type == "MAP")
        {

        }
        else
        {

        }

        int percentage = index / lineCount * 100;
        emit loadingProgress(percentage);

        //qDebug()<<"=====CharName:"<<calIns.charName<<", type:"<<calIns.type<<", unit:"<<calIns.unit
        //       <<", xAxis:"<<calIns.xAxisStrArray
        //      <<", zValues:"<<calIns.zValueStrArray;
        calList.append(calIns);
        index++;
    }

    qDebug()<<"======cal csv praser end!======";

    return true;

}

QList<CalInfo> CaliCsv::getCalList() const
{
    return calList;
}
