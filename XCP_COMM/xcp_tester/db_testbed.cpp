#include "db_testbed.h"

DB_Testbed::DB_Testbed(QObject *parent) : QObject(parent)
{
    openDB();

}

DB_Testbed::~DB_Testbed()
{
    closeDB();
}

bool DB_Testbed::openDB()
{
    db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName("TestBed.db");
    if(!db.open())
    {
        qDebug()<<"Open database error.";

        return false;
    }
    else
    {
        qDebug()<<"Open database ok.";

        return true;
    }
}

void DB_Testbed::closeDB()
{
    db.close();
    QString name = QSqlDatabase::database().connectionName();
    QSqlDatabase::removeDatabase(name);
}



QList<A2L_VarChar*> DB_Testbed::getCharPams(QString smKeyWrite, quint64 &size)
{
    QList<A2L_VarChar*> charPamList;
    charPamList.clear();

    QSqlQuery query(db);
    QString selQuery = "SELECT * FROM CharsTable";
    query.prepare(selQuery);
    if(!query.exec())
    {
        qDebug()<<"exec error:"<<query.lastError();
        return charPamList;
    }
    int startBitIndex = 0;
    int startByteIndex = 0;
    startBitIndex += 0;

    while(query.next())
    {
        A2L_VarChar *charVar = new A2L_VarChar();
        charVar->Name = query.value("pamName").toString();
        charVar->charType = query.value("type").toString();
        charVar->DataType = query.value("dataType").toString();
        charVar->ScalingFactor = query.value("scalingFactor").toDouble();
        charVar->ScalingOffset = query.value("scalingOffset").toDouble();
        charVar->Unit = query.value("unit").toString();
        charVar->ECUAddress = (quint32)(query.value("ecuAddress").toInt());
        charVar->DataSizeAG = (quint32)(query.value("dataSize").toInt());
        charVar->LowerLimit = query.value("lowerLimit").toDouble();
        charVar->UpperLimit = query.value("upperLimit").toDouble();
        charVar->setValue(query.value("defaultValue").toDouble());
        charVar->ConversionType = query.value("conversionType").toString();
        charVar->COEFFS = query.value("coeffs").toString();

        QStringList coefStrList = charVar->COEFFS.split(",");
        for (int i = 0; i < coefStrList.count(); i++)
        {
            QString str = coefStrList.at(i);
            charVar->CoefList.append(str.toDouble());
        }

        charVar->StartByte = startByteIndex;
        charVar->smKey = smKeyWrite;

        startBitIndex += (charVar->DataSizeAG * 8);
        startByteIndex = startBitIndex/8;

        charPamList.append(charVar);
    }

    size = startBitIndex/8;

    return charPamList;
}

QList<A2L_VarMeas*> DB_Testbed::getMeasPams(QString smKeyRead, quint64 &size)
{
    QList<A2L_VarMeas*> measPamList;
    measPamList.clear();

    QSqlQuery query(db);
    QString selQuery = "SELECT * FROM MeasTable";
    query.prepare(selQuery);
    if(!query.exec())
    {
        qDebug()<<"exec error:"<<query.lastError();
        return measPamList;
    }
    int startBitIndex = 0;
    int startByteIndex = 0;
    startBitIndex += 0;

    while(query.next())
    {
        A2L_VarMeas *measVar = new A2L_VarMeas();
        measVar->Name = query.value("pamName").toString();

        measVar->DataType = query.value("dataType").toString();
        measVar->ScalingFactor = query.value("scalingFactor").toDouble();
        measVar->ScalingOffset = query.value("scalingOffset").toDouble();
        measVar->Unit = query.value("unit").toString();
        measVar->ECUAddress = (quint32)(query.value("ecuAddress").toInt());
        measVar->DataSizeAG = (quint32)(query.value("dataSize").toInt());
        measVar->LowerLimit = query.value("lowerLimit").toDouble();
        measVar->UpperLimit = query.value("upperLimit").toDouble();

        measVar->ConversionType = query.value("conversionType").toString();
        measVar->COEFFS = query.value("coeffs").toString();

        QStringList coefStrList = measVar->COEFFS.split(",");
        for (int i = 0; i < coefStrList.count(); i++)
        {
            QString str = coefStrList.at(i);
            measVar->CoefList.append(str.toDouble());
        }

        measVar->StartByte = startByteIndex;
        measVar->smKey = smKeyRead;

        startBitIndex += (measVar->DataSizeAG * 8);
        startByteIndex = startBitIndex/8;

        measPamList.append(measVar);

    }

    size = startBitIndex/8;

    return measPamList;
}

bool DB_Testbed::saveCharPams(QList<A2L_VarChar *> charPamList)
{
    QSqlQuery query(db);
    query.prepare("DELETE FROM CharsTable");
    if(!query.exec())
    {
        qDebug()<<"Delete Error:"<<query.lastError();
        return false;
    }

    if(charPamList.isEmpty())
        return true;

    //Insert charPamList;
    foreach (A2L_VarChar *charVar, charPamList) {


        query.prepare("INSERT INTO CharsTable"
                      "(pamName, type, dataType, scalingFactor, scalingOffset,"
                      "unit, ecuAddress, dataSize, lowerLimit, upperLimit, defaultValue,"
                      "conversionType, coeffs)"
                      "VALUES"
                      "(:pamName, :type, :dataType, :scalingFactor, :scalingOffset,"
                      ":unit, :ecuAddress, :dataSize, :lowerLimit, :upperLimit, :defaultValue,"
                      ":conversionType, :coeffs)");
        query.bindValue(":pamName", charVar->Name);
        query.bindValue(":type", charVar->Name);
        query.bindValue(":dataType", charVar->DataType);
        query.bindValue(":scalingFactor", charVar->ScalingFactor);
        query.bindValue(":scalingOffset", charVar->ScalingOffset);
        query.bindValue(":unit", charVar->Unit);
        query.bindValue(":ecuAddress", charVar->ECUAddress);
        query.bindValue(":dataSize", charVar->DataSizeAG);
        query.bindValue(":lowerLimit", charVar->LowerLimit);
        query.bindValue(":upperLimit", charVar->UpperLimit);
        query.bindValue(":defaultValue", charVar->getValue());
        query.bindValue(":conversionType", charVar->ConversionType);
        query.bindValue(":coeffs", charVar->COEFFS);

        if(!query.exec())
        {
            qDebug()<<"INSERT Error:"<<query.lastError();
            return false;
        }

    }
    return true;
}

bool DB_Testbed::saveMeasPams(QList<A2L_VarMeas *> measPamList)
{
    QSqlQuery query(db);
    query.prepare("DELETE FROM MeasTable");
    if(!query.exec())
    {
        qDebug()<<"Delete Error:"<<query.lastError();
        return false;
    }

    if(measPamList.isEmpty())
        return true;

    //Insert measPamList;
    foreach (A2L_VarMeas *measVar, measPamList) {


        query.prepare("INSERT INTO MeasTable"
                      "(pamName, type, dataType, scalingFactor, scalingOffset,"
                      "unit, ecuAddress, dataSize, lowerLimit, upperLimit, defaultValue,"
                      "conversionType, coeffs)"
                      "VALUES"
                      "(:pamName, :type, :dataType, :scalingFactor, :scalingOffset,"
                      ":unit, :ecuAddress, :dataSize, :lowerLimit, :upperLimit, :defaultValue,"
                      ":conversionType, :coeffs)");
        query.bindValue(":pamName", measVar->Name);
        query.bindValue(":type", measVar->Name);
        query.bindValue(":dataType", measVar->DataType);
        query.bindValue(":scalingFactor", measVar->ScalingFactor);
        query.bindValue(":scalingOffset", measVar->ScalingOffset);
        query.bindValue(":unit", measVar->Unit);
        query.bindValue(":ecuAddress", measVar->ECUAddress);
        query.bindValue(":dataSize", measVar->DataSizeAG);
        query.bindValue(":lowerLimit", measVar->LowerLimit);
        query.bindValue(":upperLimit", measVar->UpperLimit);
        query.bindValue(":defaultValue", measVar->getValue());
        query.bindValue(":conversionType", measVar->ConversionType);
        query.bindValue(":coeffs", measVar->COEFFS);

        if(!query.exec())
        {
            qDebug()<<"INSERT Error:"<<query.lastError();
            return false;
        }

    }
    return true;
}
