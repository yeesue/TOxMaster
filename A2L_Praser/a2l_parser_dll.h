#ifndef A2L_PARSER_DLL_H
#define A2L_PARSER_DLL_H

#include "a2l_parser_dll_global.h"
#include "mainwindow.h"
//#include "a2ltreemodel.h"

class A2L_PARSER_DLLSHARED_EXPORT A2L_Parser_DLL
{

public:
    A2L_Parser_DLL();
};

MainWindow *win;

extern "C" A2L_PARSER_DLLSHARED_EXPORT void Init();
extern "C" A2L_PARSER_DLLSHARED_EXPORT void Show();
extern "C" A2L_PARSER_DLLSHARED_EXPORT void Hide();
extern "C" A2L_PARSER_DLLSHARED_EXPORT void Run();
extern "C" A2L_PARSER_DLLSHARED_EXPORT void End();

extern "C" A2L_PARSER_DLLSHARED_EXPORT bool parserA2LFileToTreeModel(QString fullFileName, QStringList *infoList);
extern "C" A2L_PARSER_DLLSHARED_EXPORT QStringList getAllMeasNamesInA2LFile(QString fullFileName);
extern "C" A2L_PARSER_DLLSHARED_EXPORT QStringList getAllCharacNamesInA2LFile(QString fullFileName);

extern "C" A2L_PARSER_DLLSHARED_EXPORT QMap<QString, QString> *getMeasNodeInfoInA2LFile(QString fullFileName, QString reqMeasName);
extern "C" A2L_PARSER_DLLSHARED_EXPORT QMap<QString, QString> *getCharacNodeInfoInA2LFile(QString fullFileName, QString reqCharacName);

extern "C" A2L_PARSER_DLLSHARED_EXPORT QMap<QString, QString> *getCompMNodeInfoInA2LFile(QString fullFileName, QString reqCompMName);
extern "C" A2L_PARSER_DLLSHARED_EXPORT QMap<QString, QString> *getAxisPtsNodeInfoInA2LFile(QString fullFileName, QString reqAxisPtsName);

extern "C" A2L_PARSER_DLLSHARED_EXPORT QMap<QString, QString> *getCompVtabNodeInfoInA2LFile(QString fullFileName, QString reqCompVtabName);

extern "C" A2L_PARSER_DLLSHARED_EXPORT QMap<QString, QString> *getRecordLayoutNodeInfoInA2LFile(QString fullFileName, QString reqRecordLayoutName);

extern "C" A2L_PARSER_DLLSHARED_EXPORT QMap<QString, QString> *getDaqNodeInfoInA2LFile(QString fullFileName, QString reqDaqName);

extern "C" A2L_PARSER_DLLSHARED_EXPORT QMap<QString, QString> *getDaqListNodeInfoInA2LFileByIndex(QString fullFileName, QString reqDaqListName, int index);
extern "C" A2L_PARSER_DLLSHARED_EXPORT QMap<QString, QString> *getDaqListCanIdNodeInfoInA2LFileByIndex(QString fullFileName, QString reqDaqListCanIdName, int index);

extern "C" A2L_PARSER_DLLSHARED_EXPORT QMap<QString, QString> *getDaqMemConsumpNodeInfoInA2LFile(QString fullFileName, QString reqDaqMemConsumpName);

extern "C" A2L_PARSER_DLLSHARED_EXPORT QMap<QString, QString> *getXcpOnCANNodeInfoInA2LFile(QString fullFileName, QString reqXcpOnCANName);
extern "C" A2L_PARSER_DLLSHARED_EXPORT QMap<QString, QString> *getXcpOnCANNodeInfoInA2LFileByIndex(QString fullFileName, QString reqXcpOnCANName, int index);

extern "C" A2L_PARSER_DLLSHARED_EXPORT QMap<QString, QString> *getDaqNodeInfoInXcpOnCanByName(QString fullFileName, QString reqDaqName,int indexOfXcpOnCan);
extern "C" A2L_PARSER_DLLSHARED_EXPORT QMap<QString, QString> *getEventNodeInfoInXcpOnCanByNameAndIndex(QString fullFileName, QString reqEventName, int index, int indexOfXcpOnCan);
extern "C" A2L_PARSER_DLLSHARED_EXPORT QMap<QString, QString> *getDaqMemConsumpNodeInfoInXcpOnCanByName(QString fullFileName, QString reqDaqMemConsumpName,int indexOfXcpOnCan);
extern "C" A2L_PARSER_DLLSHARED_EXPORT QMap<QString, QString> *getCanFdNodeInfoInXcpOnCanByName(QString fullFileName, QString reqCanFdName,int indexOfXcpOnCan);

extern "C" A2L_PARSER_DLLSHARED_EXPORT QMap<QString, QString> *getEventNodeInfoInA2LFileByIndex(QString fullFileName, QString reqEventName, int index);
extern "C" A2L_PARSER_DLLSHARED_EXPORT QMap<QString, QString> *getEventCanIdListNodeInfoInA2LFileByIndex(QString fullFileName, QString reqEventCanIdListName, int index);

extern "C" A2L_PARSER_DLLSHARED_EXPORT bool addHexFileToA2LFile(QString fullFileName, QString fullHexName, QStringList *infoList);

extern "C" A2L_PARSER_DLLSHARED_EXPORT QMap<QString, QStringList> *getCharHexValue(QString fullFileName, QString fullHexName, QString reqCharName, bool *_ok);

extern "C" A2L_PARSER_DLLSHARED_EXPORT QMap<QString, QString> *getMemorySegmentInfoInA2LFileByIndex(QString fullFileName, QString reqMemorySegmentName, int index);
extern "C" A2L_PARSER_DLLSHARED_EXPORT QMap<QString, QString> *getPageNodeInfoInMemorySegmentByNameAndIndex(QString fullFileName, QString reqPageName, int index, int indexOfMemorySegment);

#endif // A2L_PARSER_DLL_H
