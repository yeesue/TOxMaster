#include "a2l_praser_interface.h"
#include <QLibrary>
#include <QMessageBox>
#include <QDebug>
#include <QApplication>

A2L_PRASER_INTERFACE::A2L_PRASER_INTERFACE(QObject *parent) :
    QObject(parent)
{
    this->appPath = QApplication::applicationDirPath();
    this->a2lPraserDllPath = appPath + "/driver/A2L_Praser_DLL.dll";

    init_A2LPraser();
}

typedef void (*Init)();

void A2L_PRASER_INTERFACE::init_A2LPraser()
{
    QLibrary dllLib(a2lPraserDllPath);

    if (dllLib.load())
    {
        qDebug()<<"load A2LPraser dll init ok!";
        Init init = (Init)dllLib.resolve("Init");
        if (init)
        {
            qDebug()<<"resolve A2LPraser dll init ok!";
            init();

        }
        else
        {
            qDebug()<<"resolve A2LPraser dll init error.";
        }
    }
    else
    {
        QMessageBox::information(NULL, "NO", "A2LPraser Dll is not loaded!");
    }
}


typedef void (*Show)();

void A2L_PRASER_INTERFACE::show_A2LPraserWin()
{
    QLibrary dllLib(a2lPraserDllPath);
    if (dllLib.load())
    {
        qDebug()<<"load A2LPraser dll show ok!";
        Show show = (Show)dllLib.resolve("Show");
        if (show)
        {
            qDebug()<<"resolve A2LPraser dll show ok!";
            show();
        }
        else
        {
            qDebug()<<"resolve show error.";
        }
    }
    else
    {
        QMessageBox::information(NULL, "NO", "A2LPraser Dll is not loaded!");
    }
}


typedef bool (*LoadPraser)(QString, QStringList*);

bool A2L_PRASER_INTERFACE::loadFile_A2LPraser(QString fullFileName, QStringList &infoList)
{
    if (fullFileName.isEmpty())
        return false;

    QLibrary dllLib(a2lPraserDllPath);
    if (dllLib.load())
    {
        qDebug()<<"load loadPraser ok!";
        LoadPraser loadPraser = (LoadPraser)dllLib.resolve("praserA2LFileToTreeModel");
        if (loadPraser)
        {
            qDebug()<<"resolve loadPraser ok!";
            bool status = loadPraser(fullFileName, &infoList);
            if(status)
            {
                qDebug()<<"praser success!";
            }
            else
            {
                qDebug()<<"praser fail!";
            }
            return status;
        }
        else
        {
            qDebug()<<"resolve loadPraser error.";
            return false;
        }
    }
    else
    {
        QMessageBox::information(NULL, "NO", "Dll is not loaded!");
        return false;
    }

}

typedef bool (*AddHex)(QString, QString, QStringList*);

bool A2L_PRASER_INTERFACE::addHexToA2LFile(QString fullFileName, QString fullHexName, QStringList &infoList)
{
    if (fullFileName.isEmpty() || fullHexName.isEmpty())
        return false;

    QLibrary dllLib(a2lPraserDllPath);
    if (dllLib.load())
    {
        //qDebug()<<"load addHex ok!";
        AddHex addHex = (AddHex)dllLib.resolve("addHexFileToA2LFile");
        if (addHex)
        {
            //qDebug()<<"resolve addHex ok!";
            bool status = addHex(fullFileName, fullHexName, &infoList);
            if(status)
            {
                //qDebug()<<"add Hex File success!";

            }
            return status;
        }
        else
        {
            qDebug()<<"resolve addHex error.";
            return false;
        }
    }
    else
    {
        QMessageBox::information(NULL, "NO", "Dll is not loaded!");
        return false;
    }
}


typedef QStringList (*GetMeasNameList)(QString);

QStringList A2L_PRASER_INTERFACE::getMeasNameList(QString fullFileName)
{
   QStringList measNameList;
    QLibrary dllLib(a2lPraserDllPath);
    if (dllLib.load())
    {
        qDebug()<<"load GetMeasNameList ok!";
        GetMeasNameList getMeasNameList = (GetMeasNameList)dllLib.resolve("getAllMeasNamesInA2LFile");
        if (getMeasNameList)
        {
            qDebug()<<"resolve GetMeasNameList ok!";
            measNameList = getMeasNameList(fullFileName);
            qDebug()<<"Meas Count="<<measNameList.count();
        }
        else
        {
            qDebug()<<"resolve GetMeasNameList error.";
        }
    }
    else
    {
        QMessageBox::information(NULL, "NO", "Dll is not loaded!");
    }

    return measNameList;
}


typedef QStringList (*GetCharNameList)(QString);

QStringList A2L_PRASER_INTERFACE::getCharNameList(QString fullFileName)
{
    QStringList charNameList;
    QLibrary dllLib(a2lPraserDllPath);
    if (dllLib.load())
    {
        qDebug()<<"load GetCharNameList ok!";
        GetCharNameList getCharNameList = (GetCharNameList)dllLib.resolve("getAllCharacNamesInA2LFile");
        if (getCharNameList)
        {
            qDebug()<<"resolve GetCharNameList ok!";
            charNameList = getCharNameList(fullFileName);

            qDebug()<<"Char Count="<<charNameList.count();

        }
        else
        {
            qDebug()<<"resolve GetCharNameList error.";
        }
    }
    else
    {
        QMessageBox::information(NULL, "NO", "Dll is not loaded!");
    }

    return charNameList;
}


typedef QMap<QString, QString>* (*GetMeasNodeInfo)(QString, QString);

QMap<QString, QString> *A2L_PRASER_INTERFACE::GetMeasNodeInfoByName(QString fullFileName, QString reqMeasName)
{
    if (fullFileName.isEmpty() || reqMeasName.isEmpty())
        return NULL;

    QLibrary dllLib(a2lPraserDllPath);
    if (dllLib.load())
    {
        qDebug()<<"load GetMeasNodeInfo ok!";
        GetMeasNodeInfo getMeasNodeInfo = (GetMeasNodeInfo)dllLib.resolve("getMeasNodeInfoInA2LFile");
        if (getMeasNodeInfo)
        {
            qDebug()<<"resolve GetMeasNodeInfo ok!";
            QMap<QString, QString> *measParMap = getMeasNodeInfo(fullFileName, reqMeasName);
            if (measParMap == NULL)
                return NULL;
            qDebug()<<"measParMap count="<<measParMap->keys().count();
            for (int i = 0; i < measParMap->keys().count(); i++)
            {
                QString key = measParMap->keys().at(i);
                QString value = measParMap->value(key);
                qDebug()<<"key="<<key<<", value="<<value;
            }
            return measParMap;
        }
        else
        {
            qDebug()<<"resolve GetMeasNodeInfo error.";
            return NULL;
        }
    }
    else
    {
        QMessageBox::information(NULL, "NO", "Dll is not loaded!");
        return NULL;
    }
}


typedef QMap<QString, QString>* (*GetCharNodeInfo)(QString, QString);

QMap<QString, QString> *A2L_PRASER_INTERFACE::GetCharNodeInfoByName(QString fullFileName, QString reqCharName)
{
    if (fullFileName.isEmpty() || reqCharName.isEmpty())
        return NULL;

    QLibrary dllLib(a2lPraserDllPath);
    if (dllLib.load())
    {
        qDebug()<<"load GetCharNodeInfo ok!";
        GetCharNodeInfo getCharNodeInfo = (GetCharNodeInfo)dllLib.resolve("getCharacNodeInfoInA2LFile");
        if (getCharNodeInfo)
        {
            qDebug()<<"resolve GetCharNodeInfo ok!";
            QMap<QString, QString> *charParMap = getCharNodeInfo(fullFileName, reqCharName);
            if (charParMap == NULL)
                return NULL;
            qDebug()<<"charParMap count="<<charParMap->keys().count();
            for (int i = 0; i < charParMap->keys().count(); i++)
            {
                QString key = charParMap->keys().at(i);
                QString value = charParMap->value(key);
                qDebug()<<"key="<<key<<", value="<<value;
            }
            return charParMap;

        }
        else
        {
            qDebug()<<"resolve GetCharacNodeInfo error.";
            return NULL;
        }
    }
    else
    {
        QMessageBox::information(NULL, "NO", "Dll is not loaded!");
        return NULL;
    }
}


typedef QMap<QString, QString>* (*GetCompMNodeInfo)(QString, QString);

QMap<QString, QString> *A2L_PRASER_INTERFACE::GetCompMNodeInfoByName(QString fullFileName, QString reqCompMName)
{
    if (fullFileName.isEmpty() || reqCompMName.isEmpty())
        return NULL;

    QLibrary dllLib(a2lPraserDllPath);
    if (dllLib.load())
    {
        qDebug()<<"load GetCompMNodeInfo ok!";
        GetCompMNodeInfo getCompMNodeInfo = (GetCompMNodeInfo)dllLib.resolve("getCompMNodeInfoInA2LFile");
        if (getCompMNodeInfo)
        {
            qDebug()<<"resolve GetCompMNodeInfo ok!";
            QMap<QString, QString> *compMParMap = getCompMNodeInfo(fullFileName, reqCompMName);
            if (compMParMap == NULL)
                return NULL;
            qDebug()<<"compMParMap count="<<compMParMap->keys().count();
            for (int i = 0; i < compMParMap->keys().count(); i++)
            {
                QString key = compMParMap->keys().at(i);
                QString value = compMParMap->value(key);
                qDebug()<<"key="<<key<<", value="<<value;
            }

            return compMParMap;

        }
        else
        {
            qDebug()<<"resolve GetCompMNodeInfo error.";
            return NULL;
        }
    }
    else
    {
        QMessageBox::information(NULL, "NO", "Dll is not loaded!");
        return NULL;
    }
}

typedef QMap<QString, QString>* (*GetRecordLayoutNodeInfo)(QString, QString);

QMap<QString, QString> *A2L_PRASER_INTERFACE::GetRecordLayoutNodeInfoByName(QString fullFileName, QString reqRecordLayoutName)
{


    if (fullFileName.isEmpty() || reqRecordLayoutName.isEmpty())
        return NULL;

    QLibrary dllLib(a2lPraserDllPath);
    if (dllLib.load())
    {
        qDebug()<<"load GetRecordLayoutNodeInfo ok!";
        GetRecordLayoutNodeInfo getRecordLayoutNodeInfo = (GetRecordLayoutNodeInfo)dllLib.resolve("getRecordLayoutNodeInfoInA2LFile");
        if (getRecordLayoutNodeInfo)
        {
            qDebug()<<"resolve GetRecordLayoutNodeInfo ok!";
            QMap<QString, QString> *recordLayoutParMap = getRecordLayoutNodeInfo(fullFileName, reqRecordLayoutName);
            if (recordLayoutParMap == NULL)
                return NULL;
            qDebug()<<"recordLayoutParMap count="<<recordLayoutParMap->keys().count();
            for (int i = 0; i < recordLayoutParMap->keys().count(); i++)
            {
                QString key = recordLayoutParMap->keys().at(i);
                QString value = recordLayoutParMap->value(key);
                qDebug()<<"key="<<key<<", value="<<value;
            }
            return recordLayoutParMap;

        }
        else
        {
            qDebug()<<"resolve GetRecordLayoutNodeInfo error.";
            return NULL;
        }
    }
    else
    {
        QMessageBox::information(NULL, "NO", "Dll is not loaded!");
        return NULL;
    }
}


typedef QMap<QString, QString>* (*GetAxisPtsNodeInfo)(QString, QString);

QMap<QString, QString> *A2L_PRASER_INTERFACE::GetAxisPtsNodeInfoByName(QString fullFileName, QString reqAxisPtsName)
{
    if (fullFileName.isEmpty() || reqAxisPtsName.isEmpty())
        return NULL;

    QLibrary dllLib(a2lPraserDllPath);
    if (dllLib.load())
    {
        qDebug()<<"load GetAxisPtsNodeInfo ok!";
        GetAxisPtsNodeInfo getAxisPtsNodeInfo = (GetAxisPtsNodeInfo)dllLib.resolve("getAxisPtsNodeInfoInA2LFile");
        if (getAxisPtsNodeInfo)
        {
            qDebug()<<"resolve GetAxisPtsNodeInfo ok!";
            QMap<QString, QString> *axisPtsParMap = getAxisPtsNodeInfo(fullFileName, reqAxisPtsName);
            if (axisPtsParMap == NULL)
                return NULL;
            qDebug()<<"axisPtsParMap count="<<axisPtsParMap->keys().count();
            for (int i = 0; i < axisPtsParMap->keys().count(); i++)
            {
                QString key = axisPtsParMap->keys().at(i);
                QString value = axisPtsParMap->value(key);
                qDebug()<<"key="<<key<<", value="<<value;
            }

            return axisPtsParMap;

        }
        else
        {
            qDebug()<<"resolve GetAxisPtsNodeInfo error.";
            return NULL;
        }
    }
    else
    {
        QMessageBox::information(NULL, "NO", "Dll is not loaded!");
        return NULL;
    }
}


typedef QMap<QString, QString>* (*GetCompVtabNodeInfo)(QString, QString);

QMap<QString, QString> *A2L_PRASER_INTERFACE::GetCompVtabNodeInfoByName(QString fullFileName, QString reqCompVtabName)
{
    if (fullFileName.isEmpty() || reqCompVtabName.isEmpty())
        return NULL;

    QLibrary dllLib(a2lPraserDllPath);
    if (dllLib.load())
    {
        qDebug()<<"load GetCompVtabNodeInfo ok!";
        GetCompVtabNodeInfo getCompVtabNodeInfo = (GetCompVtabNodeInfo)dllLib.resolve("getCompVtabNodeInfoInA2LFile");
        if (getCompVtabNodeInfo)
        {
            qDebug()<<"resolve GetCompVtabNodeInfo ok!";
            QMap<QString, QString> *compVtabParMap = getCompVtabNodeInfo(fullFileName, reqCompVtabName);
            if (compVtabParMap == NULL)
                return NULL;
            qDebug()<<"compVtabParMap count="<<compVtabParMap->keys().count();
            for (int i = 0; i < compVtabParMap->keys().count(); i++)
            {
                QString key = compVtabParMap->keys().at(i);
                QString value = compVtabParMap->value(key);
                qDebug()<<"key="<<key<<", value="<<value;
            }

            return compVtabParMap;

        }
        else
        {
            qDebug()<<"resolve GetCompVtabNodeInfo error.";
            return NULL;
        }
    }
    else
    {
        QMessageBox::information(NULL, "NO", "Dll is not loaded!");
        return NULL;
    }
}


typedef QMap<QString, QString>* (*GetDaqNodeInfo)(QString, QString);

QMap<QString, QString> *A2L_PRASER_INTERFACE::GetDaqNodeInfoByName(QString fullFileName, QString reqDaqName)
{
    if (fullFileName.isEmpty() || reqDaqName.isEmpty())
        return NULL;

    QLibrary dllLib(a2lPraserDllPath);
    if (dllLib.load())
    {
        //qDebug()<<"load GetDaqNodeInfo ok!";
        GetDaqNodeInfo getDaqNodeInfo = (GetDaqNodeInfo)dllLib.resolve("getDaqNodeInfoInA2LFile");
        if (getDaqNodeInfo)
        {
            //qDebug()<<"resolve GetDaqNodeInfo ok!";
            QMap<QString, QString> *daqParMap = getDaqNodeInfo(fullFileName, reqDaqName);
            if (daqParMap == NULL)
                return NULL;
            //qDebug()<<"daqParMap count="<<daqParMap->keys().count();
            for (int i = 0; i < daqParMap->keys().count(); i++)
            {
                QString key = daqParMap->keys().at(i);
                QString value = daqParMap->value(key);
                qDebug()<<"key="<<key<<", value="<<value;
            }

            return daqParMap;

        }
        else
        {
            qDebug()<<"resolve GetDaqNodeInfo error.";
            return NULL;
        }
    }
    else
    {
        QMessageBox::information(NULL, "NO", "Dll is not loaded!");
        return NULL;
    }
}


typedef QMap<QString, QString>* (*GetDaqMemConsumpNodeInfo)(QString, QString);

QMap<QString, QString> *A2L_PRASER_INTERFACE::GetDaqMemConsumpNodeInfoByName(QString fullFileName, QString reqDaqMemConsumpName)
{
    if (fullFileName.isEmpty() || reqDaqMemConsumpName.isEmpty())
        return NULL;

    QLibrary dllLib(a2lPraserDllPath);
    if (dllLib.load())
    {
        qDebug()<<"load GetDaqNodeInfo ok!";
        GetDaqMemConsumpNodeInfo getDaqMemConsumpNodeInfo = (GetDaqMemConsumpNodeInfo)dllLib.resolve("getDaqMemConsumpNodeInfoInA2LFile");
        if (getDaqMemConsumpNodeInfo)
        {
            qDebug()<<"resolve GetDaqMemConsumpNodeInfo ok!";
            QMap<QString, QString> *daqMemConsumpParMap = getDaqMemConsumpNodeInfo(fullFileName, reqDaqMemConsumpName);
            if (daqMemConsumpParMap == NULL)
                return NULL;
            qDebug()<<"daqMemConsumpParMap count="<<daqMemConsumpParMap->keys().count();
            for (int i = 0; i < daqMemConsumpParMap->keys().count(); i++)
            {
                QString key = daqMemConsumpParMap->keys().at(i);
                QString value = daqMemConsumpParMap->value(key);
                qDebug()<<"key="<<key<<", value="<<value;
            }

            return daqMemConsumpParMap;

        }
        else
        {
            qDebug()<<"resolve GetDaqMemConsumpNodeInfo error.";
            return NULL;
        }
    }
    else
    {
        QMessageBox::information(NULL, "NO", "Dll is not loaded!");
        return NULL;
    }
}


typedef QMap<QString, QString>* (*GetXcpOnCANNodeInfo)(QString, QString, int);

QMap<QString, QString> *A2L_PRASER_INTERFACE::GetXcpOnCanNodeInfoByNameAndIndex(QString fullFileName, QString reqXcpOnCanName, int index)
{
    if (fullFileName.isEmpty() || reqXcpOnCanName.isEmpty())
        return NULL;

    QLibrary dllLib(a2lPraserDllPath);
    if (dllLib.load())
    {
        //qDebug()<<"load GetXcpOnCANNodeInfo ok!";
        GetXcpOnCANNodeInfo getXcpOnCANNodeInfo = (GetXcpOnCANNodeInfo)dllLib.resolve("getXcpOnCANNodeInfoInA2LFileByIndex");
        if (getXcpOnCANNodeInfo)
        {
            //qDebug()<<"resolve GetXcpOnCANNodeInfo ok!";
            QMap<QString, QString> *xcpOnCANParMap = getXcpOnCANNodeInfo(fullFileName, reqXcpOnCanName, index);
            if (xcpOnCANParMap == NULL)
                return NULL;
            //qDebug()<<"xcpOnCANParMap count="<<xcpOnCANParMap->keys().count();
            for (int i = 0; i < xcpOnCANParMap->keys().count(); i++)
            {
                QString key = xcpOnCANParMap->keys().at(i);
                QString value = xcpOnCANParMap->value(key);
                qDebug()<<"key="<<key<<", value="<<value;
            }

            return xcpOnCANParMap;

        }
        else
        {
            qDebug()<<"resolve GetXcpOnCANNodeInfo error.";
            return NULL;
        }
    }
    else
    {
        QMessageBox::information(NULL, "NO", "Dll is not loaded!");
        return NULL;
    }
}

typedef QMap<QString, QString>* (*GetEventNodeInfo)(QString, QString, int);

QMap<QString, QString> *A2L_PRASER_INTERFACE::GetEventNodeInfoByNameAndIndex(QString fullFileName, QString reqEventName, int index)
{
    if (fullFileName.isEmpty() || reqEventName.isEmpty())
        return NULL;

    QLibrary dllLib(a2lPraserDllPath);
    if (dllLib.load())
    {
        //qDebug()<<"load GetEventNodeInfo ok!";
        GetEventNodeInfo getEventNodeInfo = (GetEventNodeInfo)dllLib.resolve("getEventNodeInfoInA2LFileByIndex");
        if (getEventNodeInfo)
        {
            //qDebug()<<"resolve GetEventNodeInfo ok!";
            QMap<QString, QString> *eventParMap = getEventNodeInfo(fullFileName, reqEventName, index);
            if (eventParMap == NULL)
                return NULL;
            //qDebug()<<"eventParMap count="<<eventParMap->keys().count();
            for (int i = 0; i < eventParMap->keys().count(); i++)
            {
                QString key = eventParMap->keys().at(i);
                QString value = eventParMap->value(key);
                qDebug()<<"key="<<key<<", value="<<value;
            }

            return eventParMap;
        }
        else
        {
            qDebug()<<"resolve GetEventNodeInfo error.";
            return NULL;
        }
    }
    else
    {
        QMessageBox::information(NULL, "NO", "Dll is not loaded!");
        return NULL;
    }

}

typedef QMap<QString, QString>* (*GetEventCanIdNodeInfo)(QString, QString, int);

QMap<QString, QString> *A2L_PRASER_INTERFACE::GetEventCanIdNodeInfoByNameAndIndex(QString fullFileName, QString reqEventCanIdName, int index)
{
    if (fullFileName.isEmpty() || reqEventCanIdName.isEmpty())
        return NULL;

    QLibrary dllLib(a2lPraserDllPath);
    if (dllLib.load())
    {
        //qDebug()<<"load GetEventCanIdNodeInfo ok!";
        GetEventCanIdNodeInfo getEventCanIdNodeInfo = (GetEventCanIdNodeInfo)dllLib.resolve("getEventCanIdListNodeInfoInA2LFileByIndex");
        if (getEventCanIdNodeInfo)
        {
            //qDebug()<<"resolve GetEventCanIdNodeInfo ok!";
            QMap<QString, QString> *eventCanIdParMap = getEventCanIdNodeInfo(fullFileName, reqEventCanIdName, index);
            if (eventCanIdParMap == NULL)
                return NULL;
            //qDebug()<<"eventCanIdParMap count="<<eventCanIdParMap->keys().count();
            for (int i = 0; i < eventCanIdParMap->keys().count(); i++)
            {
                QString key = eventCanIdParMap->keys().at(i);
                QString value = eventCanIdParMap->value(key);
                qDebug()<<"key="<<key<<", value="<<value;
            }

            return eventCanIdParMap;
        }
        else
        {
            qDebug()<<"resolve GetEventCanIdNodeInfo error.";
            return NULL;
        }
    }
    else
    {
        QMessageBox::information(NULL, "NO", "Dll is not loaded!");
        return NULL;
    }
}


typedef QMap<QString, QString>* (*GetDaqNodeInfoInXcpOnCan)(QString, QString,int);

QMap<QString, QString> *A2L_PRASER_INTERFACE::GetDaqNodeInfoInXcpOnCanByName(QString fullFileName, QString reqDaqName, int indexXcpOnCan)
{
    if (fullFileName.isEmpty() || reqDaqName.isEmpty())
        return NULL;

    QLibrary dllLib(a2lPraserDllPath);
    if (dllLib.load())
    {
        //qDebug()<<"load GetDaqNodeInfoInfoInXcpOnCan ok!";
        GetDaqNodeInfoInXcpOnCan getDaqNodeInfoInXcpOnCan = (GetDaqNodeInfoInXcpOnCan)dllLib.resolve("getDaqNodeInfoInXcpOnCanByName");
        if (getDaqNodeInfoInXcpOnCan)
        {
            //qDebug()<<"resolve GetDaqNodeInfoInXcpOnCan ok!";
            QMap<QString, QString> *daqParMap = getDaqNodeInfoInXcpOnCan(fullFileName, reqDaqName,indexXcpOnCan);
            if (daqParMap == NULL)
                return NULL;
            //qDebug()<<"daqParMapInXcpOnCan count="<<daqParMap->keys().count();
            for (int i = 0; i < daqParMap->keys().count(); i++)
            {
                QString key = daqParMap->keys().at(i);
                QString value = daqParMap->value(key);
                qDebug()<<"key="<<key<<", value="<<value;
            }

            return daqParMap;

        }
        else
        {
            qDebug()<<"resolve GetDaqNodeInfoInfoInXcpOnCan error.";
            return NULL;
        }
    }
    else
    {
        QMessageBox::information(NULL, "NO", "Dll is not loaded!");
        return NULL;
    }
}


typedef QMap<QString, QString>* (*GetDaqMemConsumpNodeInfoInXcpOnCan)(QString, QString, int);

QMap<QString, QString> *A2L_PRASER_INTERFACE::GetDaqMemConsumpNodeInfoInXcpOnCanByName(QString fullFileName, QString reqDaqMemConsumpName, int indexXcpOnCan)
{
    if (fullFileName.isEmpty() || reqDaqMemConsumpName.isEmpty())
        return NULL;

    QLibrary dllLib(a2lPraserDllPath);
    if (dllLib.load())
    {
        qDebug()<<"load GetDaqNodeInfoInXcpOnCan ok!";
        GetDaqMemConsumpNodeInfoInXcpOnCan getDaqMemConsumpNodeInfoInXcpOnCan = (GetDaqMemConsumpNodeInfoInXcpOnCan)dllLib.resolve("getDaqMemConsumpNodeInfoInXcpOnCanByName");
        if (getDaqMemConsumpNodeInfoInXcpOnCan)
        {
            qDebug()<<"resolve GetDaqMemConsumpNodeInfoInXcpOnCan ok!";
            QMap<QString, QString> *daqMemConsumpParMap = getDaqMemConsumpNodeInfoInXcpOnCan(fullFileName, reqDaqMemConsumpName, indexXcpOnCan);
            if (daqMemConsumpParMap == NULL)
                return NULL;
            qDebug()<<"daqMemConsumpParMapInXcpOnCan count="<<daqMemConsumpParMap->keys().count();
            for (int i = 0; i < daqMemConsumpParMap->keys().count(); i++)
            {
                QString key = daqMemConsumpParMap->keys().at(i);
                QString value = daqMemConsumpParMap->value(key);
                qDebug()<<"key="<<key<<", value="<<value;
            }

            return daqMemConsumpParMap;

        }
        else
        {
            qDebug()<<"resolve GetDaqMemConsumpNodeInfoInXcpOnCan error.";
            return NULL;
        }
    }
    else
    {
        QMessageBox::information(NULL, "NO", "Dll is not loaded!");
        return NULL;
    }
}


typedef QMap<QString, QString>* (*GetEventNodeInfoInXcpOnCan)(QString, QString, int, int);

QMap<QString, QString> *A2L_PRASER_INTERFACE::GetEventNodeInfoInXcpOnCanByNameAndIndex(QString fullFileName, QString reqEventName, int index, int indexXcpOnCan)
{
    if (fullFileName.isEmpty() || reqEventName.isEmpty())
        return NULL;

    QLibrary dllLib(a2lPraserDllPath);
    if (dllLib.load())
    {
        //qDebug()<<"load GetEventNodeInfoInXcpOnCan ok!";
        GetEventNodeInfoInXcpOnCan getEventNodeInfoInXcpOnCan = (GetEventNodeInfoInXcpOnCan)dllLib.resolve("getEventNodeInfoInXcpOnCanByNameAndIndex");
        if (getEventNodeInfoInXcpOnCan)
        {
            //qDebug()<<"resolve GetEventNodeInfoInfoInXcpOnCan ok!";
            QMap<QString, QString> *eventParMap = getEventNodeInfoInXcpOnCan(fullFileName, reqEventName, index, indexXcpOnCan);
            if (eventParMap == NULL)
                return NULL;
            //qDebug()<<"eventParMapInXcpOnCan count="<<eventParMap->keys().count();
            for (int i = 0; i < eventParMap->keys().count(); i++)
            {
                QString key = eventParMap->keys().at(i);
                QString value = eventParMap->value(key);
                qDebug()<<"key="<<key<<", value="<<value;
            }

            return eventParMap;
        }
        else
        {
            qDebug()<<"resolve GetEventNodeInfoInfoInXcpOnCan error.";
            return NULL;
        }
    }
    else
    {
        QMessageBox::information(NULL, "NO", "Dll is not loaded!");
        return NULL;
    }
}

typedef QMap<QString, QString>* (*GetCanFdNodeInfoInXcpOnCan)(QString, QString,int);

QMap<QString, QString> *A2L_PRASER_INTERFACE::GetCanFdNodeInfoInXcpOnCanByName(QString fullFileName, QString reqCanFdName, int indexXcpOnCan)
{
    if (fullFileName.isEmpty() || reqCanFdName.isEmpty())
        return NULL;

    QLibrary dllLib(a2lPraserDllPath);
    if (dllLib.load())
    {
        //qDebug()<<"load GetCanFdNodeInfoInfoInXcpOnCan ok!";
        GetCanFdNodeInfoInXcpOnCan getCanFdNodeInfoInXcpOnCan = (GetCanFdNodeInfoInXcpOnCan)dllLib.resolve("getCanFdNodeInfoInXcpOnCanByName");
        if (getCanFdNodeInfoInXcpOnCan)
        {
            //qDebug()<<"resolve GetCanFdNodeInfoInXcpOnCan ok!";
            QMap<QString, QString> *canfdParMap = getCanFdNodeInfoInXcpOnCan(fullFileName, reqCanFdName,indexXcpOnCan);
            if (canfdParMap == NULL)
                return NULL;
            //qDebug()<<"canfdParMapInXcpOnCan count="<<canfdParMap->keys().count();
            for (int i = 0; i < canfdParMap->keys().count(); i++)
            {
                QString key = canfdParMap->keys().at(i);
                QString value = canfdParMap->value(key);
                qDebug()<<"key="<<key<<", value="<<value;
            }

            return canfdParMap;

        }
        else
        {
            qDebug()<<"resolve GetCanFdNodeInfoInfoInXcpOnCan error.";
            return NULL;
        }
    }
    else
    {
        QMessageBox::information(NULL, "NO", "Dll is not loaded!");
        return NULL;
    }
}

typedef QMap<QString, QString>* (*GetMemorySegmentNodeInfo)(QString, QString, int);

QMap<QString, QString> *A2L_PRASER_INTERFACE::GetMemorySegmentNodeInfoByNameAndIndex(QString fullFileName, QString reqMemorySegmentName, int index)
{
    if (fullFileName.isEmpty() || reqMemorySegmentName.isEmpty())
        return NULL;

    QLibrary dllLib(a2lPraserDllPath);
    if (dllLib.load())
    {
        //qDebug()<<"load GetMemorySegmentNodeInfo ok!";
        GetMemorySegmentNodeInfo getMemorySegmentNodeInfo = (GetMemorySegmentNodeInfo)dllLib.resolve("getMemorySegmentInfoInA2LFileByIndex");
        if (getMemorySegmentNodeInfo)
        {
            //qDebug()<<"resolve getMemorySegmentNodeInfo ok!";
            QMap<QString, QString> *memorySegmentParMap = getMemorySegmentNodeInfo(fullFileName, reqMemorySegmentName, index);
            if (memorySegmentParMap == NULL)
                return NULL;
            //qDebug()<<"memorySegmentParMap count="<<memorySegmentParMap->keys().count();
            for (int i = 0; i < memorySegmentParMap->keys().count(); i++)
            {
                QString key = memorySegmentParMap->keys().at(i);
                QString value = memorySegmentParMap->value(key);
                qDebug()<<"key="<<key<<", value="<<value;
            }

            return memorySegmentParMap;

        }
        else
        {
            qDebug()<<"resolve getMemorySegmentNodeInfo error.";
            return NULL;
        }
    }
    else
    {
        QMessageBox::information(NULL, "NO", "Dll is not loaded!");
        return NULL;
    }
}

typedef QMap<QString, QString>* (*GetPageNodeInfoInMemorySegment)(QString, QString, int, int);

QMap<QString, QString> *A2L_PRASER_INTERFACE::GetPageNodeInfoInMemorySegmentByNameAndIndex(QString fullFileName, QString reqPageName, int index, int indexMemorySegment)
{
    if (fullFileName.isEmpty() || reqPageName.isEmpty())
        return NULL;

    QLibrary dllLib(a2lPraserDllPath);
    if (dllLib.load())
    {
        //qDebug()<<"load GetPageNodeInfoInMemorySegment ok!";
        GetPageNodeInfoInMemorySegment GetPageNodeInfo = (GetPageNodeInfoInMemorySegment)dllLib.resolve("getPageNodeInfoInMemorySegmentByNameAndIndex");
        if (GetPageNodeInfo)
        {
            //qDebug()<<"resolve GetPageNodeInfoInMemorySegment ok!";
            QMap<QString, QString> *pageParMap = GetPageNodeInfo(fullFileName, reqPageName, index, indexMemorySegment);
            if (pageParMap == NULL)
                return NULL;
            //qDebug()<<"pageParMapInMemorySegment count="<<pageParMap->keys().count();
            for (int i = 0; i < pageParMap->keys().count(); i++)
            {
                QString key = pageParMap->keys().at(i);
                QString value = pageParMap->value(key);
                qDebug()<<"key="<<key<<", value="<<value;
            }

            return pageParMap;
        }
        else
        {
            qDebug()<<"resolve GetPageNodeInfoInMemorySegment error.";
            return NULL;
        }
    }
    else
    {
        QMessageBox::information(NULL, "NO", "Dll is not loaded!");
        return NULL;
    }
}


typedef QMap<QString, QString>* (*GetDaqListNodeInfo)(QString, QString, int);

QMap<QString, QString> *A2L_PRASER_INTERFACE::GetDaqListNodeInfoByNameAndIndex(QString fullFileName, QString reqDaqListName, int index)
{
    if (fullFileName.isEmpty() || reqDaqListName.isEmpty())
        return NULL;

    QLibrary dllLib(a2lPraserDllPath);
    if (dllLib.load())
    {
        //qDebug()<<"load GetDaqListNodeInfo ok!";
        GetDaqListNodeInfo getDaqListNodeInfo = (GetDaqListNodeInfo)dllLib.resolve("getDaqListNodeInfoInA2LFileByIndex");
        if (getDaqListNodeInfo)
        {
            //qDebug()<<"resolve GetDaqListNodeInfo ok!";
            QMap<QString, QString> *daqListParMap = getDaqListNodeInfo(fullFileName, reqDaqListName, index);
            if (daqListParMap == NULL)
                return NULL;
            //qDebug()<<"daqListParMap count="<<daqListParMap->keys().count();
            for (int i = 0; i < daqListParMap->keys().count(); i++)
            {
                QString key = daqListParMap->keys().at(i);
                QString value = daqListParMap->value(key);
                qDebug()<<"key="<<key<<", value="<<value;
            }
            return daqListParMap;

        }
        else
        {
            qDebug()<<"resolve GetDaqListNodeInfo error.";
            return NULL;
        }
    }
    else
    {
        QMessageBox::information(NULL, "NO", "Dll is not loaded!");
        return NULL;
    }
}


typedef QMap<QString, QString>* (*GetDaqListCanIdNodeInfo)(QString, QString, int);

QMap<QString, QString> *A2L_PRASER_INTERFACE::GetDaqListCanIdNodeInfoByNameAndIndex(QString fullFileName, QString reqDaqListCanIdName, int index)
{
    if (fullFileName.isEmpty() || reqDaqListCanIdName.isEmpty())
        return NULL;

    QLibrary dllLib(a2lPraserDllPath);
    if (dllLib.load())
    {
        //qDebug()<<"load GetDaqListCanIdNodeInfo ok!";
        GetDaqListCanIdNodeInfo getDaqListCanIdNodeInfo = (GetDaqListCanIdNodeInfo)dllLib.resolve("getDaqListCanIdNodeInfoInA2LFileByIndex");
        if (getDaqListCanIdNodeInfo)
        {
            //qDebug()<<"resolve GetDaqListCanIdNodeInfo ok!";
            QMap<QString, QString> *daqListCanIdParMap = getDaqListCanIdNodeInfo(fullFileName, reqDaqListCanIdName, index);
            if (daqListCanIdParMap == NULL)
                return NULL;
            //qDebug()<<"daqListCanIdParMap count="<<daqListCanIdParMap->keys().count();
            for (int i = 0; i < daqListCanIdParMap->keys().count(); i++)
            {
                QString key = daqListCanIdParMap->keys().at(i);
                QString value = daqListCanIdParMap->value(key);
                qDebug()<<"key="<<key<<", value="<<value;
            }
            return daqListCanIdParMap;
        }
        else
        {
            qDebug()<<"resolve GetDaqListCanIdNodeInfo error.";
            return NULL;
        }
    }
    else
    {
        QMessageBox::information(NULL, "NO", "Dll is not loaded!");
        return NULL;
    }
}

typedef QMap<QString, QStringList>* (*GetCharHexValue)(QString, QString, QString, bool*);

QMap<QString, QStringList> *A2L_PRASER_INTERFACE::GetCharHexValueByName(QString fullFileName, QString fullHexName, QString reqCharName, bool *ok)
{
    if (fullFileName.isEmpty() || fullHexName.isEmpty() || reqCharName.isEmpty())
        return NULL;

    QLibrary dllLib(a2lPraserDllPath);
    if (dllLib.load())
    {
        qDebug()<<"load getCharHexValue ok!";
        GetCharHexValue getCharHexValue = (GetCharHexValue)dllLib.resolve("getCharHexValue");
        if (getCharHexValue)
        {
            qDebug()<<"resolve GetCharHexValue ok!";
            QMap<QString, QStringList>* charHexInfoMap = getCharHexValue(fullFileName, fullHexName, reqCharName, ok);
            qDebug()<<"ok="<<*ok;
            if (charHexInfoMap != NULL)
            {
                QStringList xyzCountList = charHexInfoMap->value("xyzCountList");
                QStringList xList = charHexInfoMap->value("charXList");
                QStringList yList = charHexInfoMap->value("charYList");
                QStringList zList = charHexInfoMap->value("charZList");
                QString xCount, yCount, zCount;
                xCount = xyzCountList.at(0);
                yCount = xyzCountList.at(1);
                zCount = xyzCountList.at(2);

                //qDebug()<<"xList Count="<<xCount;
                //qDebug()<<"yList Count="<<yCount;
                //qDebug()<<"zList Count="<<zCount;

                QString valueStr;
                for (int i = 0; i < xCount.toInt(); i++)
                {
                    valueStr += xList.at(i);
                    valueStr += "-";
                }
                //qDebug()<<"xList:"<<valueStr;
                valueStr.clear();

                for (int i = 0; i < yCount.toInt(); i++)
                {
                    valueStr += yList.at(i);
                    valueStr += "-";
                }
                //qDebug()<<"yList:"<<valueStr;
                valueStr.clear();

                for (int i = 0; i < zCount.toInt(); i++)
                {
                    valueStr += zList.at(i);
                    valueStr += "-";
                }
                //qDebug()<<"zList:"<<valueStr;
                valueStr.clear();
            }
            return charHexInfoMap;

        }
        else
        {
            qDebug()<<"resolve GetCharHexValue error.";
            return NULL;
        }
    }
    else
    {
        QMessageBox::information(NULL, "NO", "Dll is not loaded!");
        return NULL;
    }
}
