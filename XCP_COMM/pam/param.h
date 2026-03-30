#ifndef PARAM_H
#define PARAM_H

#include <QObject>
#include <QSharedMemory>

class PARAM : public QObject
{
    Q_OBJECT
public:
    explicit PARAM(QObject *parent = 0);
    ~PARAM();

    /*
    QString Param_Name = "";
    QString Param_Text = "";
    int Type = 0;                      // 0 = unsigned, 1 = signed, 2 = float, 3 = double
    int StartBit = 0;
    int LengthBit = 0;
    int ByteOrder = 0;
    qreal DefaultValue = 0;
    qreal ScalingFactor = 0;
    qreal ScalingOffset = 0;
    qreal Maximum = 0;
    qreal Minimum = 0;
    qreal SampleRate = 0;
    QString Unit = "";
    QString Comment = "";

    int StartByte = 0;
    bool isRead = false;

    QString smKey = "";
    int smSize = 0;

    // param display config
    int Param_NumDec = 2;
    bool Param_Write_Allow = false;

    // reference pam for new define pam
    PARAM *refPam = NULL;
    bool nedPam = false;
    */

    QString Name = "";
    QString ShortName = "";
    QString LongIdentifier = "";

    QString Unit = "None";

    qreal ScalingOffset = 0;
    qreal ScalingFactor = 1;

    qreal DefaultValue = 0;
    qreal LowerLimit = 0;
    qreal UpperLimit = 0;

    //type value: ref mdf CNBLOCK signal data type:
    //0=uint, 1=int, 2=float, 3=double(default byteorder from idblock),
    //9=uint(moto), 10=int, 11=float,12=double  (moto)
    //13=uint(intel), 14=int, 15=float,16=double (intel)

    int Type = 0;                      // 0 = unsigned, 1 = signed, 2 = float, 3 = double
    int StartBit = 0;
    int LengthBit = 0;
    int ByteOrder = 0;

    int StartByte = 0;
    QString smKey = "";
    int smSize = 0;

    float rate_ms = 100;            // ms

    int funIndex = -1;
    int pamModule = 0;              // 0 = common pam, 1 = xcp char pam, 2 = can send pam

signals:
    void valueUpdated(qreal);
    void valueUpdated(QString);
    void valueUpdated(PARAM*, double);

    void mapValueUpdated(PARAM*, int, int, double);

public slots:
    void setValue(qreal value);
    qreal getValue();

    void setMapValue(int index_x, int index_y, qreal value);

    void setXcpCharValue(qreal value);

    void updatePamValueInSM(qreal value);
    qreal getPamValueInSM();

    bool isNedPam();

private:
    void value2IntelData(char *data, qreal value);
    quint8 rotateLeft(quint8 val, quint8 n);

private:
    qreal Value = 0.0;
    bool nedPam = false;


};

#endif // PARAM_H
