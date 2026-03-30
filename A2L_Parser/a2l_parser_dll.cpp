#include "a2l_parser_dll.h"


A2L_Parser_DLL::A2L_Parser_DLL()
{
}

void Init()
{
    if(win == NULL)
    {
        win = new MainWindow;
    }

}

void Show()
{
    win->show();
}

void Hide()
{
    win->hide();
}

void Run()
{

}

void End()
{
    if(win)
    {
        delete win;
    }
}

bool parserA2LFileToTreeModel(QString fullFileName, QStringList *infoList)
{
    if (fullFileName.isEmpty())
        return false;

   bool parserStatus =  win->loadAndParserA2lFile(fullFileName, *infoList);
   return parserStatus;
}

QStringList getAllMeasNamesInA2LFile(QString fullFileName)
{
    QStringList measNameList;
    if (!win)
        return measNameList;

    measNameList = win->getAllMeasNamesInA2LFile(fullFileName);
    return measNameList;
}

QStringList getAllCharacNamesInA2LFile(QString fullFileName)
{
    QStringList characNameList;
    if (!win)
        return characNameList;

    characNameList = win->getAllCharacNamesInA2LFile(fullFileName);
    return characNameList;
}

QMap<QString, QString> *getMeasNodeInfoInA2LFile(QString fullFileName, QString reqMeasName)
{
    if (!win)
        return NULL;
    QMap<QString, QString> *measInfoMap = win->getMeasNodeInfoByName(fullFileName, reqMeasName);

    return measInfoMap;
}

QMap<QString, QString> *getCharacNodeInfoInA2LFile(QString fullFileName, QString reqCharacName)
{
    if (!win)
        return NULL;
    QMap<QString, QString> *characInfoMap = win->getCharacNodeInfoByName(fullFileName, reqCharacName);

    return characInfoMap;
}

QMap<QString, QString> *getCompMNodeInfoInA2LFile(QString fullFileName, QString reqCompMName)
{
    if (!win)
        return NULL;
    QMap<QString, QString> *compMInfoMap = win->getCompMNodeInfoByName(fullFileName, reqCompMName);

    return compMInfoMap;
}

QMap<QString, QString> *getAxisPtsNodeInfoInA2LFile(QString fullFileName, QString reqAxisPtsName)
{
    if (!win)
        return NULL;
    QMap<QString, QString> *axisPtsInfoMap = win->getAxisPtsNodeInfoByName(fullFileName, reqAxisPtsName);

    return axisPtsInfoMap;
}

QMap<QString, QString> *getDaqNodeInfoInA2LFile(QString fullFileName, QString reqDaqName)
{
    if (!win)
        return NULL;
    QMap<QString, QString> *daqInfoMap = win->getDaqNodeInfoByName(fullFileName, reqDaqName);

    return daqInfoMap;
}

QMap<QString, QString> *getXcpOnCANNodeInfoInA2LFile(QString fullFileName, QString reqXcpOnCANName)
{
    if (!win)
        return NULL;
    QMap<QString, QString> *xcpOnCANInfoMap = win->getXcpOnCANNodeInfoByName(fullFileName, reqXcpOnCANName);

    return xcpOnCANInfoMap;
}

QMap<QString, QString> *getXcpOnCANNodeInfoInA2LFileByIndex(QString fullFileName, QString reqXcpOnCANName, int index)
{
    if (!win)
        return NULL;
    QMap<QString, QString> *xcpOnCANInfoMap = win->getXcpOnCANNodeInfoByNameAndIndex(fullFileName, reqXcpOnCANName, index);

    return xcpOnCANInfoMap;
}


QMap<QString, QString> *getDaqMemConsumpNodeInfoInA2LFile(QString fullFileName, QString reqDaqMemConsumpName)
{
    if (!win)
        return NULL;
    QMap<QString, QString> *daqMemConsumpInfoMap = win->getDaqMemConsumpNodeInfoByName(fullFileName, reqDaqMemConsumpName);

    return daqMemConsumpInfoMap;
}

QMap<QString, QString> *getEventNodeInfoInA2LFileByIndex(QString fullFileName, QString reqEventName, int index)
{
    if (!win)
        return NULL;
    QMap<QString, QString> *eventInfoMap = win->getEventNodeInfoByNameAndIndex(fullFileName, reqEventName, index);

    return eventInfoMap;
}

QMap<QString, QString> *getEventCanIdListNodeInfoInA2LFileByIndex(QString fullFileName, QString reqEventCanIdListName, int index)
{
    if (!win)
        return NULL;
    QMap<QString, QString> *eventCanIdListInfoMap = win->getEventCanIdListNodeInfoByNameAndIndex(fullFileName, reqEventCanIdListName, index);

    return eventCanIdListInfoMap;
}

QMap<QString, QString> *getRecordLayoutNodeInfoInA2LFile(QString fullFileName, QString reqRecordLayoutName)
{
    if (!win)
        return NULL;
    QMap<QString, QString> *recordLayoutInfoMap = win->getRecordLayoutNodeInfoByName(fullFileName, reqRecordLayoutName);

    return recordLayoutInfoMap;
}

bool addHexFileToA2LFile(QString fullFileName, QString fullHexName, QStringList *infoList)
{
    if (fullFileName.isEmpty())
        return false;
    if (fullHexName.isEmpty())
        return false;

   bool Status =  win->addHexFile(fullFileName, fullHexName, *infoList);
   return Status;
}

QMap<QString, QStringList>* getCharHexValue(QString fullFileName, QString fullHexName, QString reqCharName, bool *_ok)
{
    if (fullFileName.isEmpty())
        return NULL;
    if (fullHexName.isEmpty())
        return NULL;

   QMap<QString, QStringList> *charHexInfoMap =  win->getCharHexValueByName(fullFileName, fullHexName, reqCharName, _ok);
   return charHexInfoMap;
}

QMap<QString, QString> *getDaqListNodeInfoInA2LFileByIndex(QString fullFileName, QString reqDaqListName, int index)
{
    if (!win)
        return NULL;
    QMap<QString, QString> *daqListInfoMap = win->getDaqListNodeInfoByNameAndIndex(fullFileName, reqDaqListName, index);

    return daqListInfoMap;
}

QMap<QString, QString> *getDaqListCanIdNodeInfoInA2LFileByIndex(QString fullFileName, QString reqDaqListCanIdName, int index)
{
    if (!win)
        return NULL;
    QMap<QString, QString> *daqListCanIdInfoMap = win->getDaqListCanIdNodeInfoByNameAndIndex(fullFileName, reqDaqListCanIdName, index);

    return daqListCanIdInfoMap;
}

QMap<QString, QString> *getDaqNodeInfoInXcpOnCanByName(QString fullFileName, QString reqDaqName, int indexOfXcpOnCan)
{
    if (!win)
        return NULL;
    QMap<QString, QString> *daqInfoMap = win->getDaqNodeInfoInXcpOnCanByName(fullFileName, reqDaqName, indexOfXcpOnCan);

    return daqInfoMap;
}

QMap<QString, QString> *getEventNodeInfoInXcpOnCanByNameAndIndex(QString fullFileName, QString reqEventName, int index, int indexOfXcpOnCan)
{
    if (!win)
        return NULL;
    QMap<QString, QString> *eventInfoMap = win->getEventNodeInfoInXcpOnCanByNameAndIndex(fullFileName, reqEventName, index, indexOfXcpOnCan);

    return eventInfoMap;
}

QMap<QString, QString> *getDaqMemConsumpNodeInfoInXcpOnCanByName(QString fullFileName, QString reqDaqMemConsumpName, int indexOfXcpOnCan)
{
    if (!win)
        return NULL;
    QMap<QString, QString> *daqMemConsumpInfoMap = win->getDaqMemConsumpNodeInfoInXcpOnCanByName(fullFileName, reqDaqMemConsumpName, indexOfXcpOnCan);

    return daqMemConsumpInfoMap;
}


QMap<QString, QString> *getCanFdNodeInfoInXcpOnCanByName(QString fullFileName, QString reqCanFdName, int indexOfXcpOnCan)
{
    if (!win)
        return NULL;
    QMap<QString, QString> *canfdInfoMap = win->getCanFdNodeInfoInXcpOnCanByName(fullFileName, reqCanFdName, indexOfXcpOnCan);

    return canfdInfoMap;
}

QMap<QString, QString> *getCompVtabNodeInfoInA2LFile(QString fullFileName, QString reqCompVtabName)
{
    if (!win)
        return NULL;
    QMap<QString, QString> *compVtabInfoMap = win->getCompVtabNodeInfoByName(fullFileName, reqCompVtabName);

    return compVtabInfoMap;
}

QMap<QString, QString> *getMemorySegmentInfoInA2LFileByIndex(QString fullFileName, QString reqMemorySegmentName, int index)
{
    if (!win)
        return NULL;
    QMap<QString, QString> *memorySegmentInfoMap = win->getMemorySegmentNodeInfoInA2LFileByIndex(fullFileName, reqMemorySegmentName, index);

    return memorySegmentInfoMap;
}

QMap<QString, QString> *getPageNodeInfoInMemorySegmentByNameAndIndex(QString fullFileName, QString reqPageName, int index, int indexOfMemorySegment)
{
    if (!win)
        return NULL;
    QMap<QString, QString> *pageInfoMap = win->getPageNodeInfoInMemorySegmentByNameAndIndex(fullFileName, reqPageName, index, indexOfMemorySegment);

    return pageInfoMap;
}
