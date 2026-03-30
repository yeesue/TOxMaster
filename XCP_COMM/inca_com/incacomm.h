#ifndef INCACOMM_H
#define INCACOMM_H

#include <QObject>
#include <QAxObject>
#include <QTime>

class INCACOMM : public QObject
{
    Q_OBJECT
public:
    explicit INCACOMM(QObject *parent = 0);
    ~INCACOMM();

signals:

public slots:

    bool connectToINCA();
    bool disconnectFromINCA();

    bool isConnected();
    QAxObject *getExp();

    bool startRecording();
    bool stopRecording(QString rcdFileName);

    bool getRecordingState();

    QStringList getAllMeasNames();
    QStringList getAllCaliNames();

    bool getMeasurementValue(QString measName, double &min, double &max, double &value, QString &result);
    double getMeasureValue(QString measName);
    double getMeasureMin(QString measName);
    double getMeasureMax(QString measName);

    QString getMeasureChar(QString measName);
    QString getMeasurePhyType(QString measName);

    bool getScalarCaliValue(QString caliName, double &min, double &max, double &value, QString &result);
    double getScalarCaliValue(QString caliName);
    double getScalarCaliMin(QString caliName);
    double getScalarCaliMax(QString caliName);

    bool setScalarCaliValue(QString caliName, double value);

    bool setCurveCaliValueRange(QString caliName, int startIndex, int endIndex, QList<double> valueList);
    bool setCurveCaliValueSinglePoint(QString caliName, int index, double value);
    bool setCurveCaliSameValueRange(QString caliName, int startIndex, int endIndex, double value);

    bool setMapCaliValueRange(QString caliName, int startXIndex, int startYIndex, int endXIndex, int endYIndex, QList<QList<double>> valueMatrix);
    bool setMapCaliValueSinglePoint(QString caliName, int xIndex, int yIndex, double value);
    bool setMapCaliSameValueRange(QString caliName, int startXIndex, int startYIndex, int endXIndex, int endYIndex, double value);

private:
    bool connected = false;

    QAxObject *incaObj = NULL;
    QAxObject *experimentObj = NULL;

    QStringList measPhyTypeList;
    QStringList caliPhyTypeList;


};

extern QString CLSID_INCA;

#endif // INCACOMM_H
