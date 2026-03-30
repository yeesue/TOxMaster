#ifndef CALICSV_H
#define CALICSV_H

#include <QObject>
#include <QFileDialog>
#include <QDir>
#include <QTextStream>

struct CalInfo{
    QString charName;
    QString type;
    QString unit;
    QString xAxisName;
    QStringList xAxisStrArray;
    QString yAxisName;
    QStringList yAxisStrArray;

    QStringList zValueStrArray;
};

class CaliCsv : public QObject
{
    Q_OBJECT
public:
    explicit CaliCsv(QObject *parent = nullptr, QString filePath = "");
    ~CaliCsv();

    QList<CalInfo> getCalList() const;

    bool readFromCsv(QString csvFilePath);

signals:
    void loadingProgress(int percentage);

private:


private:
    QString caliCsvFileName = "";
    QString caliCsvFilePath = "";

    QList<CalInfo> calList;

};

#endif // CALICSV_H
