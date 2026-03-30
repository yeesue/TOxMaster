#ifndef A2L_VARMEAS_H
#define A2L_VARMEAS_H

#include <QObject>
#include "xcp_common.h"
#include "param.h"
#include <QTimer>

struct A2L_Event;

class A2L_VarMeas : public PARAM
{
    Q_OBJECT
public:
    A2L_VarMeas();
    ~A2L_VarMeas();

    QString DataType = "";             //DataType: UBYTE, SBYTE, UWORD, SWORD, ULONG, SLONG, A_UINT64, A_INT64, FLOAT32_IEEE, FLOAT64_IEEE
    QString Conversion = "";
    QString ConversionType ="";
    QString COEFFS = "";
    QString ConversionTable = "";

    QString Format = "";

    quint32 Resolution = 0; //smallest possible change in bits
    qreal Accuracy = 0;     //possible variation from exact value in %

    quint32 DataSizeAG = 0;
    quint32 ECUAddress = 0;

    QList<qreal> CoefList;   // INT = f(PHYS) = (aX^2 + bX + c)/(dX^2 + eX + f)

    quint16 ArraySize = 1;
    quint8 xDim = 1;
    quint8 yDim = 1;
    quint8 zDim = 1;

    //float rate_ms = 100;            // ms
    A2L_Event *daqEventRef = NULL;

    //qreal getValue();
    //void setValue(qreal value);

    int getPrecision();

public slots:
    void valueUpdatedTimerSlot();

signals:
    //void valueUpdated(qreal);

private:
    //qreal Value = 0;


};

#endif // A2L_VARMEAS_H
