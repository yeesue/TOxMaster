#ifndef IMPORT_EXPORT_SERVICE_H
#define IMPORT_EXPORT_SERVICE_H

#include <QObject>
#include <QList>
#include <QString>

class DB_Testbed;
class A2L_VarMeas;
class A2L_VarChar;

class ImportExportService : public QObject
{
    Q_OBJECT

public:
    struct PamSet {
        QString projectName;
        bool secondChannel = false;

        QList<A2L_VarMeas*> measPamList;
        QList<A2L_VarMeas*> daqMeasPamList;
        QList<A2L_VarChar*> charPamList;

        quint64 sizeRead = 0;
        quint64 sizeReadDAQ = 0;
        quint64 sizeWrite = 0;

        QString smKeyRead;
        QString smKeyReadDAQ;
        QString smKeyWrite;
    };

    explicit ImportExportService(QObject *parent = nullptr);

    void clearPamSet(PamSet &set) const;

    bool exportPamSetToExcel(const PamSet &set) const;
    bool loadPamSetFromExcel(PamSet &set) const;
    bool loadPamSetFromDb(PamSet &set, DB_Testbed *&db) const;
    bool savePamSetToDb(const PamSet &set, DB_Testbed *&db) const;

    bool exportMeasToExcel(const QList<A2L_VarMeas*> &meaVars, const QString &excelPath) const;
    bool exportCharsToExcel(const QList<A2L_VarChar*> &charVars, const QString &excelPath) const;
    void importMeasFromExcel(const QString &excelPath, QList<A2L_VarMeas*> &measList, quint64 &byteSize) const;
    void importCharsFromExcel(const QString &excelPath, QList<A2L_VarChar*> &charList, quint64 &byteSize) const;

    bool delExcel(const QString &filePath) const;
    void convertToColName(int data, QString &res) const;
    QString to26AlphabetString(int data) const;
};

#endif // IMPORT_EXPORT_SERVICE_H
