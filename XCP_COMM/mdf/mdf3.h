#ifndef MDF3_H
#define MDF3_H

#include <QtGlobal>
#include <QtCore>
#include <QDateTime>
#include "mdf3block.h"
#include "param.h"
#include "a2l_varmeas.h"


class MDF3
{

public:
    MDF3();

    bool mdf3_Demo(QString fileName);
    bool demoDataGen(char *data, int *size, qreal rate);

    //QFile *mdf3_createFile(QString fileName);
    bool mdf3_createAndOpenFile(QString fileName);
    bool mdf3_closeFile();
    bool mdf3_addIDHDToFile(int numDG);
    bool mdf3_addDGToFile(quint64 curFilePos, char *data, int size, QList<SigAttr*> sigList, bool lastDG);
    bool mdf3_addDGToFile(quint64 curFilePos, char *data, int size, QList<PARAM*> pamList, bool lastDG);
    bool mdf3_addDGToFile(quint64 curFilePos, int size, int sizeBlock, QList<PARAM*> pamList, bool lastDG);
    bool mdf3_addDGToFile(quint64 curFilePos, int size, QList<A2L_VarMeas*> measPamList, bool lastDG);
    bool mdf3_addRawDataToFile(char *data, int size);

    //bool mdf3_readFile(QString fileName);
    quint64 mdf3_getFileCurPos();


private:
    QString mdf3_FileName;
    QString mdf3_FilePath;
    QFile mdf3_File;

    QDateTime DateTime_StartRecord;




};

#endif // MDF3_H
