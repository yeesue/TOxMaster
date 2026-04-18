#include "scriptobj.h"
#include <QEventLoop>
#include <QCoreApplication>
#include <QFile>
#include <QDir>
#include <QTextStream>
#include <cstdlib>

ScriptTimer::ScriptTimer(QObject *parent) :
    QTimer(parent)
{

}

ScriptTimer::~ScriptTimer()
{

}

void ScriptTimer::setInterval(int msec)
{
    QTimer::setInterval(msec);
}


void ScriptTimer::start(int msec)
{
    QTimer::start(msec);
}

void ScriptTimer::start()
{
    QTimer::start();
}

ScriptObj::ScriptObj(QObject *parent, QScriptEngine *scriptEngine) :
    QObject(parent), engine(scriptEngine)
{

}

ScriptObj::~ScriptObj()
{

}

void ScriptObj::setScriptFilePath(QString filePath)
{
    this->scriptFilePath = filePath;

}


QScriptValue ScriptObj::createTimer()
{
    ScriptTimer *timer = new ScriptTimer();
    return (engine->newQObject(timer));
}

void ScriptObj::log(QString msg)
{
    emit logMsg(msg);
}

void ScriptObj::logDoubleValue(double value)
{
    QString valueTxt = QString::number(value, 'g', 6);
    log(valueTxt);
}

bool ScriptObj::setSMPamValue(QString smKey, QString pamName, qreal value)
{
    if(smKey.isEmpty() || pamName.isEmpty())
        return false;
    PARAM *pam = Globals::pamHash.value(pamName + "." + smKey);
    if(!pam)
        return false;

    pam->setValue(value);

    return true;
}

qreal ScriptObj::getSMPamValue(QString smKey, QString pamName)
{
    if(smKey.isEmpty() || pamName.isEmpty())
        return 0;
    PARAM *pam = Globals::pamHash.value(pamName + "." + smKey);
    if(!pam)
        return 0;

    return pam->getValue();
}

QString ScriptObj::getSMPamValueStr(QString smKey, QString pamName)
{
    if(smKey.isEmpty() || pamName.isEmpty())
        return "None";
    PARAM *pam = Globals::pamHash.value(pamName + "." + smKey);
    if(!pam)
        return "None";

    return QString::number(pam->getValue());
}

bool ScriptObj::setPamValue(QString modName, QString pamName, qreal value)
{
    //qDebug()<<"in script obj setPamValue";
    if(modName.isEmpty() || pamName.isEmpty())
        return false;

    PARAM *pam = Globals::pamHash.value(pamName + "." + modName);
    if(!pam)
        return false;

    pam->setValue(value);

    return true;
}

qreal ScriptObj::getPamValue(QString modName, QString pamName)
{
    if(modName.isEmpty() || pamName.isEmpty())
        return 0;
    PARAM *pam = Globals::pamHash.value(pamName + "." + modName);
    if(!pam)
        return 0;

    return pam->getValue();
}

QString ScriptObj::getPamValueStr(QString modName, QString pamName)
{
    if(modName.isEmpty() || pamName.isEmpty())
        return "None";

    PARAM *pam = Globals::pamHash.value(pamName + "." + modName);
    if(!pam)
        return "None";

    return QString::number(pam->getValue(), 'g', 8);
}

bool ScriptObj::setMapPamValue(QString modName, QString pamName, int index_x, int index_y, qreal value)
{
    //qDebug()<<"in script obj setPamValue";
    if(modName.isEmpty() || pamName.isEmpty())
        return false;

    PARAM *pam = Globals::mapPamHash.value(pamName + "." + modName);
    if(!pam)
        return false;

    //pam->setValue(value);

    A2L_VarChar *mapChar = (A2L_VarChar*)pam;

    if(index_x == 0)
    {
        qDebug()<<"Axis value updated."<<pamName;
    }
    else if (index_y == 0)
    {
        qDebug()<<"Yxis value updated."<<pamName;
    }
    else
    {
        //qDebug()<<"Z map value updated."<<pamName;

        char *data = new char[mapChar->DataSizeAG];
        transferPhyValueToRawData(mapChar, value, data);

        updateMapValueInSM(mapChar, index_y - 1, index_x - 1, data);
    }

    return true;
}


void ScriptObj::wait(quint32 msec)
{
    QEventLoop loop;//????????????????
    QTimer::singleShot(msec, &loop, SLOT(quit()));//???????��???????????????????????????
    loop.exec();//???????????��??????????????????????????????????
}

void ScriptObj::msleep(quint32 msec)
{
    QTime dieTime = QTime::currentTime().addMSecs(msec);
    while( QTime::currentTime() < dieTime )
        QCoreApplication::processEvents(QEventLoop::AllEvents, 100);
}

void ScriptObj::transferPhyValueToRawData(A2L_VarChar *charVar, double value, char *data)
{
    if(charVar == NULL)
        return;
    QString dataType = charVar->DataType;
    int dataSize = charVar->DataSizeAG;

    int dataTypeIndex = dataTypeEnum.indexOf(dataType);

    switch (dataTypeIndex) {
    case 0:
    {
        quint8 val = (quint8)value;
        memcpy(data, (char*)&val, dataSize);
        break;
    }
    case 1:
    {
        qint8 val = (qint8)value;
        memcpy(data, (char*)&val, dataSize);
        break;
    }
    case 2:
    {
        quint16 val = (quint16)value;
        memcpy(data, (char*)&val, dataSize);
        break;
    }
    case 3:
    {
        qint16 val = (qint16)value;
        memcpy(data, (char*)&val, dataSize);
        break;
    }
    case 4:
    {
        quint32 val = (quint32)value;
        memcpy(data, (char*)&val, dataSize);
        break;
    }
    case 5:
    {
        qint32 val = (qint32)value;
        memcpy(data, (char*)&val, dataSize);
        break;
    }
    case 6:
    {
        quint64 val = (quint64)value;
        memcpy(data, (char*)&val, dataSize);
        break;
    }
    case 7:
    {
        qint64 val = (qint64)value;
        memcpy(data, (char*)&val, dataSize);
        break;
    }
    case 8:
    {
        float val = (float)value;
        memcpy(data, (char*)&val, dataSize);
        break;
    }
    case 9:
    {
        double val = (double)value;
        memcpy(data, (char*)&val, dataSize);
        break;
    }
    default:
        break;
    }
}

void ScriptObj::updateMapValueInSM(A2L_VarChar *map, quint16 index_x, quint16 index_y, char *data)
{
    //qDebug()<<"in update MapValue in SM."<<map->Name<<",index_x="<<index_x<<",index_y="<<index_y;

    QSharedMemory sm;
    sm.setKey(map->smKey);

    if (!sm.isAttached())
    {
        if(!sm.attach())
        {
            qDebug()<<"unable attach to map sharedMemory :"<<sm.key();
            return;
        }
    }

    int index = 8;
    if(map->yCount == 0 && map->xCount != 0)
    {
        index += (map->DataSizeAG * index_x);
    }
    else
    {
        index += map->DataSizeAG * (map->yCount * index_x + index_y);
    }

    //qDebug()<<"index offset="<<index;

    sm.lock();
    memcpy((char*)sm.data()+index, data, map->DataSizeAG);
    sm.unlock();

    //qDebug()<<"map Value updated."<<map->Name<<",index_x="<<index_x<<", index_y="<<index_y;

}

// File operations
QString ScriptObj::readFile(QString filePath)
{
    QFile file(filePath);
    if (!file.exists()) {
        return "";
    }
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        return "";
    }
    QTextStream in(&file);
    QString content = in.readAll();
    file.close();
    return content;
}

bool ScriptObj::writeFile(QString filePath, QString content)
{
    QFile file(filePath);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        return false;
    }
    QTextStream out(&file);
    out << content;
    file.close();
    return true;
}

bool ScriptObj::appendFile(QString filePath, QString content)
{
    QFile file(filePath);
    if (!file.open(QIODevice::Append | QIODevice::Text)) {
        return false;
    }
    QTextStream out(&file);
    out << content;
    file.close();
    return true;
}

bool ScriptObj::fileExists(QString filePath)
{
    QFile file(filePath);
    return file.exists();
}

bool ScriptObj::removeFile(QString filePath)
{
    QFile file(filePath);
    return file.remove();
}

QStringList ScriptObj::getFilesInDirectory(QString directoryPath)
{
    QDir directory(directoryPath);
    return directory.entryList(QDir::Files);
}

// System operations
QString ScriptObj::getCurrentDirectory()
{
    return QDir::currentPath();
}

bool ScriptObj::setCurrentDirectory(QString directoryPath)
{
    return QDir::setCurrent(directoryPath);
}

QString ScriptObj::getEnvironmentVariable(QString name)
{
    return qgetenv(name.toUtf8());
}

bool ScriptObj::setEnvironmentVariable(QString name, QString value)
{
    qputenv(name.toUtf8(), value.toUtf8());
    return true;
}

// Math operations
double ScriptObj::random(double min, double max)
{
    return min + (max - min) * qrand() / (RAND_MAX + 1.0);
}

double ScriptObj::sin(double angle)
{
    return qSin(angle);
}

double ScriptObj::cos(double angle)
{
    return qCos(angle);
}

double ScriptObj::tan(double angle)
{
    return qTan(angle);
}

double ScriptObj::sqrt(double value)
{
    return qSqrt(value);
}

double ScriptObj::pow(double base, double exponent)
{
    return qPow(base, exponent);
}

// String operations
QString ScriptObj::toUpper(QString str)
{
    return str.toUpper();
}

QString ScriptObj::toLower(QString str)
{
    return str.toLower();
}

QString ScriptObj::trim(QString str)
{
    return str.trimmed();
}

int ScriptObj::indexOf(QString str, QString substring)
{
    return str.indexOf(substring);
}

QString ScriptObj::substring(QString str, int start, int length)
{
    if (length == -1) {
        return str.mid(start);
    } else {
        return str.mid(start, length);
    }
}
