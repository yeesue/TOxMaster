#include "incacomm.h"
#include<QDebug>
#include <qscriptengine.h>

QString CLSID_INCA;

INCACOMM::INCACOMM(QObject *parent) : QObject(parent)
{
    measPhyTypeList <<"real64"<<"char";

    caliPhyTypeList <<"real64"<<"char";

}

INCACOMM::~INCACOMM()
{

}

bool INCACOMM::connectToINCA()
{
    if(incaObj == NULL)
    {
        incaObj = new QAxObject("Inca.Inca");
    }

    if(!incaObj)
        return false;

    QString apiVer = incaObj->dynamicCall("APIVersion()").toString();
    qDebug()<<"连接版本INCA"<<apiVer;

    //bool flag = incaObj->dynamicCall("ClearMonitor()").toBool();
    //qDebug()<<"清除Monitor:"<<flag;

    QString progId = incaObj->dynamicCall("ProgId()").toString();
    qDebug()<<"程序ID-"<<progId;

    experimentObj = incaObj->querySubObject("GetOpenedExperiment()");

    if(!experimentObj)
    {
        qDebug()<<"无法获取Experiment!";
        return false;
    }
    else
    {
        qInfo()<<"获取Experiment成功";
    }

    connected = true;

    return true;
}

bool INCACOMM::disconnectFromINCA()
{
    if(experimentObj)
    {
        delete experimentObj;
        experimentObj = NULL;

        qInfo()<<"退出Experiment连接";
    }

    if(incaObj)
    {
        delete incaObj;
        incaObj = NULL;

        qInfo()<<"退出INCA连接";
    }

    return true;
}

bool INCACOMM::isConnected()
{
    return this->connected;
}

QAxObject *INCACOMM::getExp()
{
    return this->experimentObj;
}

bool INCACOMM::startRecording()
{
    if(!incaObj || !experimentObj)
        return false;

    bool startOk = experimentObj->dynamicCall("StartRecording()").toBool();

    qInfo()<<"启动数据记录:"<<startOk;

    return startOk;
}

bool INCACOMM::stopRecording(QString rcdFileName)
{
    if(!incaObj || !experimentObj)
        return false;

    bool stopOk = experimentObj->dynamicCall("StopRecording(string, string)", rcdFileName, "ETASMDF").toBool();

    qInfo()<<"停止数据记录:"<<stopOk;

    return stopOk;
}

bool INCACOMM::getRecordingState()
{
    if(!incaObj || !experimentObj)
        return false;

    bool isRecording = experimentObj->dynamicCall("GetRecordingState()").toBool();

    return isRecording;
}

QStringList INCACOMM::getAllMeasNames()
{
    QStringList nameList;
    if(!incaObj || !experimentObj)
        return nameList;
    qInfo()<<"Get all measure elements...";
    nameList = experimentObj->dynamicCall("GetAllActiveMeasureLabels()").toStringList();
    //qInfo()<<nameList;

    nameList.sort();

    return nameList;
}

QStringList INCACOMM::getAllCaliNames()
{
    int typeId = qRegisterMetaType<QAxObject*>();

    QStringList nameList;
    if(!incaObj || !experimentObj)
        return nameList;
    qInfo()<<"Get all cali elements...";
    QList<QVariant> caliElemList = experimentObj->dynamicCall("GetAllCalibrationElements()").toList();
    qInfo()<<"get cali num:"<<caliElemList.count();

    foreach (QVariant var, caliElemList) {
        bool canConv = var.canConvert<QAxObject*>();
        qDebug()<<"can convert QAxObject?"<<canConv<<", variantType:"<<var.typeName();

        /*
        QAxObject *caliElem = var.value<QAxObject*>();
        if(caliElem)
        {
            QString caliName = caliElem->dynamicCall("GetName()").toString();
            qInfo()<<"get caliName:"<<caliName;

            nameList.append(caliName);
        }
        */
    }

    return nameList;
}

bool INCACOMM::getMeasurementValue(QString measName, double &min, double &max, double &value, QString &result)
{
    if(!incaObj || !experimentObj || measName.isEmpty())
        return false;

    QAxObject *measElem = experimentObj->querySubObject("GetMeasureElement(string)", measName);
    if(!measElem)
    {
        qDebug()<<"can't find the measurement: "<<measName;
        return false;
    }

    QAxObject *measData = measElem->querySubObject("GetValue()");
    if(!measData)
    {
        qDebug()<<"can't get the value for measurement: "<<measName;
        return false;
    }

    if(!measData->dynamicCall("IsScalar()").toBool())
    {
        qDebug()<<"measurement is not scalar : "<<measName;
        return false;
    }

    QString measPhyType = measData->dynamicCall("GetPhysType()").toString();
    int index = measPhyTypeList.indexOf(measPhyType);

    if(index == -1)
    {
        qDebug()<<"measurement type is not supported.";
        return false;
    }

    switch (index) {
    case 0:
    {
        value = measData->dynamicCall("GetDoublePhysValue()").toDouble();
        break;
    }
    case 1:
    {
        result = measData->dynamicCall("GetCharPhysValue()").toString();
        value = measData->dynamicCall("GetImplValue()").toDouble();
    }
    default:
        break;
    }

    min = measData->dynamicCall("GetMeasureRangeMin()").toDouble();
    max = measData->dynamicCall("GetMeasureRangeMax()").toDouble();

    return true;

}

double INCACOMM::getMeasureValue(QString measName)
{

    if(!incaObj || !experimentObj || measName.isEmpty())
        return 0;

    double value = 0;

    QAxObject *measElem = experimentObj->querySubObject("GetMeasureElement(string)", measName);
    if(!measElem)
    {
        qDebug()<<"can't find the measurement: "<<measName;
        return 0;
    }

    QAxObject *measData = measElem->querySubObject("GetValue()");
    if(!measData)
    {
        qDebug()<<"can't get the value for measurement: "<<measName;
        return 0;
    }

    if(!measData->dynamicCall("IsScalar()").toBool())
    {
        qDebug()<<"measurement is not scalar : "<<measName;
        return 0;
    }

    QString measPhyType = measData->dynamicCall("GetPhysType()").toString();
    int index = measPhyTypeList.indexOf(measPhyType);

    if(index == -1)
    {
        qDebug()<<"measurement type is not supported.";
        return false;
    }

    switch (index) {
    case 0:
    {
        value = measData->dynamicCall("GetDoublePhysValue()").toDouble();
        break;
    }
    case 1:
    {
        value = measData->dynamicCall("GetImplValue()").toDouble();
    }
    default:
        break;
    }

    return value;
}

double INCACOMM::getMeasureMin(QString measName)
{
    if(!incaObj || !experimentObj || measName.isEmpty())
        return 0;

    double min = 0;

    QAxObject *measElem = experimentObj->querySubObject("GetMeasureElement(string)", measName);
    if(!measElem)
    {
        qDebug()<<"can't find the measurement: "<<measName;
        return 0;
    }

    QAxObject *measData = measElem->querySubObject("GetValue()");
    if(!measData)
    {
        qDebug()<<"can't get the value for measurement: "<<measName;
        return 0;
    }

    if(!measData->dynamicCall("IsScalar()").toBool())
    {
        qDebug()<<"measurement is not scalar : "<<measName;
        return 0;
    }

    QString measPhyType = measData->dynamicCall("GetPhysType()").toString();
    //qDebug()<< measName <<"measure phy type:"<<measPhyType;

    int index = measPhyTypeList.indexOf(measPhyType);

    min = measData->dynamicCall("GetMeasureRangeMin()").toDouble();

    return min;
}

double INCACOMM::getMeasureMax(QString measName)
{
    if(!incaObj || !experimentObj || measName.isEmpty())
        return 0;

    double max = 0;

    QAxObject *measElem = experimentObj->querySubObject("GetMeasureElement(string)", measName);
    if(!measElem)
    {
        qDebug()<<"can't find the measurement: "<<measName;
        return 0;
    }

    QAxObject *measData = measElem->querySubObject("GetValue()");
    if(!measData)
    {
        qDebug()<<"can't get the value for measurement: "<<measName;
        return 0;
    }

    if(!measData->dynamicCall("IsScalar()").toBool())
    {
        qDebug()<<"measurement is not scalar : "<<measName;
        return 0;
    }

    QString measPhyType = measData->dynamicCall("GetPhysType()").toString();
    //qDebug()<<"measure phy type:"<<measPhyType;
    int index = measPhyTypeList.indexOf(measPhyType);

    max = measData->dynamicCall("GetMeasureRangeMax()").toDouble();

    return max;
}

QString INCACOMM::getMeasureChar(QString measName)
{
    QString result = "";

    if(!incaObj || !experimentObj || measName.isEmpty())
        return result;



    QAxObject *measElem = experimentObj->querySubObject("GetMeasureElement(string)", measName);
    if(!measElem)
    {
        qDebug()<<"can't find the measurement: "<<measName;
        return result;
    }

    QAxObject *measData = measElem->querySubObject("GetValue()");
    if(!measData)
    {
        qDebug()<<"can't get the value for measurement: "<<measName;
        return result;
    }

    if(!measData->dynamicCall("IsScalar()").toBool())
    {
        qDebug()<<"measurement is not scalar : "<<measName;
        return result;
    }

    QString measPhyType = measData->dynamicCall("GetPhysType()").toString();
    //qDebug()<< measName <<" measure phy type:"<<measPhyType;
    int index = measPhyTypeList.indexOf(measPhyType);

    if(index == -1)
    {
        qDebug()<<"measurement type is not supported.";
        return result;
    }

    switch (index) {
    case 0:
    {
        //value = measData->dynamicCall("GetDoublePhysValue()").toDouble();
        break;
    }
    case 1:
    {
        result = measData->dynamicCall("GetCharPhysValue()").toString();
        break;

    }
    default:
        break;
    }

    return result;
}

QString INCACOMM::getMeasurePhyType(QString measName)
{
    QString type = "";

    if(!incaObj || !experimentObj || measName.isEmpty())
        return type;

    QAxObject *measElem = experimentObj->querySubObject("GetMeasureElement(string)", measName);
    if(!measElem)
    {
        qDebug()<<"can't find the measurement: "<<measName;
        return type;
    }

    QAxObject *measData = measElem->querySubObject("GetValue()");
    if(!measData)
    {
        qDebug()<<"can't get the value for measurement: "<<measName;
        return type;
    }

    if(!measData->dynamicCall("IsScalar()").toBool())
    {
        qDebug()<<"measurement is not scalar : "<<measName;
        return type;
    }

    type = measData->dynamicCall("GetPhysType()").toString();
    qDebug()<< measName <<" measure phy type:"<<type;

    return type;
}

bool INCACOMM::getScalarCaliValue(QString caliName, double &min, double &max, double &value, QString &result)
{
    if(!incaObj || !experimentObj || caliName.isEmpty())
        return false;

    QAxObject *caliElem = experimentObj->querySubObject("GetCalibrationElement(string)", caliName);
    if(!caliElem)
    {
        qDebug()<<"can't find the calibration: "<<caliName;
        return false;
    }

    QAxObject *caliData = caliElem->querySubObject("GetValue()");
    if(!caliData)
    {
        qDebug()<<"can't get the value for calibration: "<<caliName;
        return false;
    }

    if(!caliData->dynamicCall("IsScalar()").toBool())
    {
        qDebug()<<"calibration is not scalar : "<<caliName;
        return false;
    }

    QString caliPhyType = caliData->dynamicCall("GetPhysType()").toString();
    //qDebug()<< caliName <<" caliure phy type:"<<caliPhyType;
    int index = caliPhyTypeList.indexOf(caliPhyType);

    if(index == -1)
    {
        qDebug()<<"calibration type is not supported.";
        return false;
    }

    switch (index) {
    case 0:
    {
        value = caliData->dynamicCall("GetDoublePhysValue()").toDouble();
        break;
    }
    case 1:
    {
        result = caliData->dynamicCall("GetCharPhysValue()").toString();
        value = caliData->dynamicCall("GetImplValue()").toDouble();
    }
    default:
        break;
    }

    min = caliData->dynamicCall("GetCalibrationWeakBoundMin()").toDouble();
    max = caliData->dynamicCall("GetCalibrationWeakBoundMax()").toDouble();

    return true;
}

double INCACOMM::getScalarCaliValue(QString caliName)
{
    if(!incaObj || !experimentObj || caliName.isEmpty())
        return 0;

    double value = 0;

    QAxObject *caliElem = experimentObj->querySubObject("GetCalibrationElement(string)", caliName);
    if(!caliElem)
    {
        qDebug()<<"can't find the calibration: "<<caliName;
        return 0;
    }

    QAxObject *caliData = caliElem->querySubObject("GetValue()");
    if(!caliData)
    {
        qDebug()<<"can't get the value for calibration: "<<caliName;
        return 0;
    }

    if(!caliData->dynamicCall("IsScalar()").toBool())
    {
        qDebug()<<"calibration is not scalar : "<<caliName;
        return 0;
    }

    QString caliPhyType = caliData->dynamicCall("GetPhysType()").toString();
    int index = caliPhyTypeList.indexOf(caliPhyType);

    if(index == -1)
    {
        qDebug()<<"calibration type is not supported.";
        return false;
    }

    switch (index) {
    case 0:
    {
        value = caliData->dynamicCall("GetDoublePhysValue()").toDouble();
        break;
    }
    case 1:
    {
        //result = caliData->dynamicCall("GetCharPhysValue()").toString();
        value = caliData->dynamicCall("GetImplValue()").toDouble();
    }
    default:
        break;
    }

    return value;
}

double INCACOMM::getScalarCaliMin(QString caliName)
{
    if(!incaObj || !experimentObj || caliName.isEmpty())
        return 0;

    double min = 0;

    QAxObject *caliElem = experimentObj->querySubObject("GetCalibrationElement(string)", caliName);
    if(!caliElem)
    {
        qDebug()<<"can't find the calibration: "<<caliName;
        return 0;
    }

    QAxObject *caliData = caliElem->querySubObject("GetValue()");
    if(!caliData)
    {
        qDebug()<<"can't get the value for calibration: "<<caliName;
        return 0;
    }

    if(!caliData->dynamicCall("IsScalar()").toBool())
    {
        qDebug()<<"calibration is not scalar : "<<caliName;
        return 0;
    }

    QString caliPhyType = caliData->dynamicCall("GetPhysType()").toString();
    //qDebug()<<"caliure phy type:"<<caliPhyType;

    int index = caliPhyTypeList.indexOf(caliPhyType);

    min = caliData->dynamicCall("GetCalibrationWeakBoundMin()").toDouble();

    return min;
}

double INCACOMM::getScalarCaliMax(QString caliName)
{
    if(!incaObj || !experimentObj || caliName.isEmpty())
        return 0;

    double max = 0;

    QAxObject *caliElem = experimentObj->querySubObject("GetCalibrationElement(string)", caliName);
    if(!caliElem)
    {
        qDebug()<<"can't find the calibration: "<<caliName;
        return 0;
    }

    QAxObject *caliData = caliElem->querySubObject("GetValue()");
    if(!caliData)
    {
        qDebug()<<"can't get the value for calibration: "<<caliName;
        return 0;
    }

    if(!caliData->dynamicCall("IsScalar()").toBool())
    {
        qDebug()<<"calibration is not scalar : "<<caliName;
        return 0;
    }

    QString caliPhyType = caliData->dynamicCall("GetPhysType()").toString();
    //qDebug()<<"caliure phy type:"<<caliPhyType;
    int index = caliPhyTypeList.indexOf(caliPhyType);

    max = caliData->dynamicCall("GetCalibrationWeakBoundMin()").toDouble();

    return max;
}

bool INCACOMM::setScalarCaliValue(QString caliName, double value)
{
    if(!incaObj || !experimentObj || caliName.isEmpty())
        return false;

    QAxObject *caliData = experimentObj->querySubObject("GetCalibrationValue(string)", caliName);
    if(!caliData)
    {
        qDebug()<<"can't find the calibration: "<<caliName;
        return false;
    }

    if(!caliData->dynamicCall("IsScalar()").toBool())
    {
        qDebug()<<"calibration is not scalar: "<<caliName;
        return false;
    }


    QString caliPhyType = caliData->dynamicCall("GetPhysType()").toString();
    //qDebug()<< caliName <<" calibration phy type:"<<caliPhyType;
    int index = caliPhyTypeList.indexOf(caliPhyType);


    bool caliOK = false;
    switch (index) {
    case 0:
    {
        caliOK = caliData->dynamicCall("SetDoublePhysValue(double)", value).toBool();
        break;
    }
    case 1:
    {
        //QString valueChar = (value > 0 ? "True" : "False");
        //caliOK = caliData->dynamicCall("SetCharPhysValue(String)", valueChar).toBool();
        caliOK = caliData->dynamicCall("SetImplValue(Double)", value).toBool();
        break;
    }
    default:
        break;
    }

    return caliOK;
}

//startIndex=1...size, endIndex = startIndex + size - 1,
bool INCACOMM::setCurveCaliValueRange(QString caliName, int startIndex, int endIndex, QList<double> valueList)
{
    if(!incaObj || !experimentObj || caliName.isEmpty())
        return false;

    QAxObject *caliElem = experimentObj->querySubObject("GetCalibrationElement(string)", caliName);
    if(!caliElem)
    {
        qDebug()<<"未发现CURVE标定参数: "<<caliName;
        return false;
    }

    bool isOneDTable = caliElem->dynamicCall("IsOneDTable()").toBool();

    if(isOneDTable)
    {
        QAxObject *caliData_Curve = caliElem->querySubObject("GetValue()");
        if(!caliData_Curve)
            return false;

        //bool oneDTable = caliData_Curve->dynamicCall("IsOneDTable()").toBool();
        //qDebug()<<"CaliElementDataItem isOneDTable:"<<oneDTable;

        QAxObject *caliData = caliData_Curve->querySubObject("GetValue()");
        if(!caliData) return false;

        //bool isArray = caliData->dynamicCall("isArray()").toBool();
        //qDebug()<<"isArray:"<<isArray;

        //bool IsWriteProtected = caliData->dynamicCall("IsWriteProtected()").toBool();
        //qDebug()<<"IsWriteProtected="<<IsWriteProtected;

        //int dataSize = caliData->dynamicCall("GetSize()").toInt();
        //qDebug()<<"dataSize="<<dataSize;

        //bool isReadOnly = caliData->dynamicCall("isReadOnly()").toBool();
         //qDebug()<<"isReadOnly="<<isReadOnly;

        QVariantList list;
        for(int i = 0; i < valueList.count(); i++)
        {
            list.append(valueList.at(i));
        }
        QVariant listVar = list;

        bool caliOK = caliData->dynamicCall("SetDoublePhysValuesRange(Double[], Int32, Int32)", listVar, startIndex, endIndex).toBool();

        return caliOK;

    }

    return false;

}

bool INCACOMM::setCurveCaliValueSinglePoint(QString caliName, int index, double value)
{
    if(caliName.isEmpty()) return false;

    QList<double> listValue;
    listValue.append(value);

    return setCurveCaliValueRange(caliName, index, index, listValue);
}

bool INCACOMM::setCurveCaliSameValueRange(QString caliName, int startIndex, int endIndex, double value)
{
    if(caliName.isEmpty()) return false;

    QList<double> listValue;

    for(int i = startIndex; i< endIndex + 1; i++)
    {
        listValue.append(value);
    }

    return setCurveCaliValueRange(caliName, startIndex, endIndex, listValue);
}

bool INCACOMM::setMapCaliValueRange(QString caliName, int startXIndex, int startYIndex, int endXIndex, int endYIndex, QList<QList<double> > valueMatrix)
{
    if(!incaObj || !experimentObj || caliName.isEmpty())
        return false;

    QAxObject *caliElem = experimentObj->querySubObject("GetCalibrationElement(string)", caliName);
    if(!caliElem)
    {
        qDebug()<<"未发现MAP标定参数: "<<caliName;
        return false;
    }

    bool isTwoDTable = caliElem->dynamicCall("IsTwoDTable()").toBool();

    if(isTwoDTable)
    {
        QAxObject *caliData_Map = caliElem->querySubObject("GetValue()");
        if(!caliData_Map) return false;

        //bool twoDTable = caliData_Map->dynamicCall("IsTwoDTable()").toBool();
        //qDebug()<<"CaliElementDataItem isTwoDTable:"<<twoDTable;

        QAxObject *caliData = caliData_Map->querySubObject("GetValue()");
        if(!caliData) return false;

        //bool isMatrix = caliData->dynamicCall("isMatrix()").toBool();
        //qDebug()<<"isMatrix:"<<isMatrix;

        QVariantList matrix;
        for(int i = 0; i < valueMatrix.count(); i++)
        {
            QVariantList list;
            QList<double> valueList = valueMatrix.at(i);
            for(int j = 0; j < valueList.count(); j++)
            {
                list.append(valueList.at(j));
            }
            QVariant listVar = list;
            matrix.append(listVar);
        }
        QVariant matrixVar = matrix;

        //qInfo()<<"startX:"<<startXIndex<<", startY:"<<startYIndex<<", endX:"<<endXIndex<<", endY:"<<endYIndex<<", valueMatrix:"<<valueMatrix;
        bool caliOK = caliData->dynamicCall("SetDoublePhysValuesRange(Double[][], Int32, Int32, Int32, Int32)", matrixVar, startXIndex, startYIndex, endXIndex, endYIndex).toBool();

        return caliOK;

        return true;
    }

    return false;
}

bool INCACOMM::setMapCaliValueSinglePoint(QString caliName, int xIndex, int yIndex, double value)
{
    if(caliName.isEmpty()) return false;

    QList<QList<double>> matrixValue;
    QList<double> listValue;
    listValue.append(value);
    matrixValue.append(listValue);

    return setMapCaliValueRange(caliName, xIndex, yIndex, xIndex, yIndex, matrixValue);
}

bool INCACOMM::setMapCaliSameValueRange(QString caliName, int startXIndex, int startYIndex, int endXIndex, int endYIndex, double value)
{
    if(caliName.isEmpty()) return false;

    QList<QList<double>> matrixValue;
    QList<double> listValue;
    for(int i = startXIndex; i < endXIndex + 1; i++)
    {
        for(int j = startYIndex; j < endYIndex + 1; j++)
        {
            listValue.append(value);
        }
        matrixValue.append(listValue);
    }

    return setMapCaliValueRange(caliName, startXIndex, startYIndex, endXIndex, endYIndex, matrixValue);
}
