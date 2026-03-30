#ifndef A2L_PARSER_INTERFACE_H
#define A2L_PARSER_INTERFACE_H

#include <QObject>

class A2L_PARSER_INTERFACE : public QObject
{
    Q_OBJECT
public:
    A2L_PARSER_INTERFACE(QObject* parent = 0);

    void init_A2LParser();
    void show_A2LParserWin();

    bool loadFile_A2LParser(QString fullFileName, QStringList &infoList);
    bool addHexToA2LFile(QString fullFileName, QString fullHexName, QStringList &infoList);

    QStringList getMeasNameList(QString fullFileName);
    QStringList getCharNameList(QString fullFileName);

    QMap<QString, QString>* GetMeasNodeInfoByName(QString fullFileName, QString reqMeasName);
    QMap<QString, QString>* GetCharNodeInfoByName(QString fullFileName, QString reqCharName);
    QMap<QString, QString>* GetCompMNodeInfoByName(QString fullFileName, QString reqCompMName);
    QMap<QString, QString>* GetRecordLayoutNodeInfoByName(QString fullFileName, QString reqRecordLayoutName);
    QMap<QString, QString>* GetAxisPtsNodeInfoByName(QString fullFileName, QString reqAxisPtsName);
    QMap<QString, QString>* GetCompVtabNodeInfoByName(QString fullFileName, QString reqCompVtabName);
    QMap<QString, QString>* GetDaqNodeInfoByName(QString fullFileName, QString reqDaqName);
    QMap<QString, QString>* GetDaqMemConsumpNodeInfoByName(QString fullFileName, QString reqDaqMemConsumpName);
    QMap<QString, QString>* GetXcpOnCanNodeInfoByNameAndIndex(QString fullFileName, QString reqXcpOnCanName, int index);
    QMap<QString, QString>* GetEventNodeInfoByNameAndIndex(QString fullFileName, QString reqEventName, int index);
    QMap<QString, QString>* GetEventCanIdNodeInfoByNameAndIndex(QString fullFileName, QString reqEventCanIdName, int index);

    QMap<QString, QString>* GetDaqNodeInfoInXcpOnCanByName(QString fullFileName, QString reqDaqName, int indexXcpOnCan);
    QMap<QString, QString>* GetDaqMemConsumpNodeInfoInXcpOnCanByName(QString fullFileName, QString reqDaqMemConsumpName, int indexXcpOnCan);
    QMap<QString, QString>* GetEventNodeInfoInXcpOnCanByNameAndIndex(QString fullFileName, QString reqEventName, int index, int indexXcpOnCan);
    QMap<QString, QString>* GetCanFdNodeInfoInXcpOnCanByName(QString fullFileName, QString reqCanFdName, int indexXcpOnCan);

    QMap<QString, QString>* GetMemorySegmentNodeInfoByNameAndIndex(QString fullFileName, QString reqMemorySegmentName, int index);
    QMap<QString, QString>* GetPageNodeInfoInMemorySegmentByNameAndIndex(QString fullFileName, QString reqPageName, int index, int indexMemorySegment);

    QMap<QString, QString>* GetDaqListNodeInfoByNameAndIndex(QString fullFileName, QString reqDaqListName, int index);
    QMap<QString, QString>* GetDaqListCanIdNodeInfoByNameAndIndex(QString fullFileName, QString reqDaqListCanIdName, int index);

    QMap<QString, QStringList>* GetCharHexValueByName(QString fullFileName, QString fullHexName, QString reqCharName, bool *ok);

private:
    QString appPath;
    QString a2lParserDllPath;
};

#endif // A2L_PARSER_INTERFACE_H
