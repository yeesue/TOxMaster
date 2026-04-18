#ifndef CSVOBJECT_H
#define CSVOBJECT_H

#include <QObject>
#include <QTime>
#include <QTimer>
#include <QFileDialog>
#include <QDir>
#include <QTextStream>
#include "param.h"
#include <QSettings>
#include <memory>

class CSVObject : public QObject
{
    Q_OBJECT
public:
    explicit CSVObject(QObject *parent = 0);
    ~CSVObject();

    bool getRcdOn() const;
    void setRcdOn(bool value);

    quint32 getRate_ms() const;
    void setRate_ms(const quint32 &value);

    QString getCsv_FileName() const;
    void setCsv_FileName(const QString &value);

    QString getCsv_StorePath() const;
    void setCsv_StorePath(const QString &value);

    QString getCsv_FilePath() const;
    void setCsv_FilePath(const QString &value);

    QStringList getPamNameList() const;
    void setPamNameList(const QStringList value);


    void setRecordPamList(const QList<PARAM *> &value);

    void setWorkName(const QString &value);

public slots:
    void setFileName(QString fileName);

    void clearPams();
    void addPamName(QString value);
    void setPamNames(QStringList nameList);

    void clearPamValues();
    void addPamValue(double value);
    void setPamValues(QList<double> valueList);
    void setPamValues(QStringList valueList);

    bool csv_record_start();
    void csv_record_stop();
    void csv_record_once();
    void csv_record_once(double valueList[], int size);

    void csv_record_avg(int avgTime_s, int sampleTime_ms);
    void csv_record_shot();

    void resetPamValue();

    void wait(quint32 msec);

signals:
    void showRecord(QString);

private:
    void readSetting();

private:

    int pamCount = 0;
    QStringList pamNameList;
    QList<PARAM*> recordPamList;

    QList<double> pamValueList;

    bool rcdOn = false;
    quint32 rate_ms = 200;

    QString workName = "";
    QString csv_FileName = "";
    QString csv_StorePath = "";
    QString csv_FilePath = "";
    std::unique_ptr<QFile> csv_File;
    QTextStream out;
};

#endif // CSVOBJECT_H
