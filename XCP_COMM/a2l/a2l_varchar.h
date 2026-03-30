#ifndef A2L_VARCHAR_H
#define A2L_VARCHAR_H

#include <QObject>
#include "xcp_common.h"
#include "param.h"

class A2L_VarChar;

struct Cali_Pair{
    A2L_VarChar *charVar;
    char *data;
    int size;
    int offset = 0;
};
Q_DECLARE_METATYPE(Cali_Pair)

class A2L_VarChar : public PARAM
{
    Q_OBJECT
public:
    A2L_VarChar();
    ~A2L_VarChar();

    QString DataType = "";        //DataType: UBYTE, SBYTE, UWORD, SWORD, ULONG, SLONG, A_UINT64, A_INT64, FLOAT32_IEEE, FLOAT64_IEEE
    QString charType = "";
    QString Conversion = "";
    QString ConversionType = "";
    QString COEFFS = "";
    QString ConversionTable = "";
    QString ConversionVTab = "";

    QString Deposit = "";
    QString Format = "";

    quint32 DataSizeAG = 0;
    quint32 ECUAddress = 0;

    qreal MaxDiff = 0;

    QList<qreal> CoefList;

    quint8 Fnc_Values_Position;
    QString Fnc_Values_DataType = "";
    QString Fnc_Values_IndexMode = "";
    QString Fnc_Values_AddressType = "";


    QString Axis_X_Ref = "";
    QString Axis_X_Attribute = "";
    QString Axis_X_InputQuantity = "";
    QString Axis_X_Conversion = "";
    quint8 Axis_X_MaxAxisPoints = 0;
    qreal Axis_X_LowerLimit = 0;
    qreal Axis_X_UpperLimit = 0;
    QString Axis_X_Format = "";
    QString Axis_X_Deposit = "";
    QList<qreal> Axis_X_CoefList;
    qreal Axis_X_ScalingOffset = 0;
    qreal Axis_X_ScalingFactor = 1;

    quint8 Axis_Pts_X_Position = 0;
    QString Axis_Pts_X_DataType = "";
    QString Axis_Pts_X_IndexIncr = "";
    QString Axis_Pts_X_Addessing = "";

    QString Axis_X_Type = "";
    QString Axis_X_ConversionType = "";
    QString Axis_X_COEFFS = "";
    QString Axis_X_ConversionTable = "";
    QString Axis_X_Unit = "";
    quint32 Axis_X_DataSizeAG = 0;
    quint32 Axis_X_ECUAddress = 0;
    QString Axis_X_DataType = "";
    QString Axis_X_ConversionVTab = "";


    QString Axis_Y_Ref = "";
    QString Axis_Y_Attribute = "";
    QString Axis_Y_InputQuantity = "";
    QString Axis_Y_Conversion = "";
    quint8 Axis_Y_MaxAxisPoints = 0;
    qreal Axis_Y_LowerLimit = 0;
    qreal Axis_Y_UpperLimit = 0;
    QString Axis_Y_Format = "";
    QString Axis_Y_Deposit = "";
    QList<qreal> Axis_Y_CoefList;
    qreal Axis_Y_ScalingOffset = 0;
    qreal Axis_Y_ScalingFactor = 1;

    quint8 Axis_Pts_Y_Position;
    QString Axis_Pts_Y_DataType;
    QString Axis_Pts_Y_IndexIncr;
    QString Axis_Pts_Y_Addessing;

    QString Axis_Y_Type = "";
    QString Axis_Y_ConversionType = "";
    QString Axis_Y_COEFFS = "";
    QString Axis_Y_ConversionTable = "";
    QString Axis_Y_Unit = "";
    quint32 Axis_Y_DataSizeAG = 0;
    quint32 Axis_Y_ECUAddress = 0;
    QString Axis_Y_DataType = "";
    QString Axis_Y_ConversionVTab = "";

    int xDim_BLK = 0;
    int yDim_BLK = 0;
    int zDim_BLK = 0;
    int number_BLK = 0;

    QList<qreal> Blk_Values;

    QList<qreal> getBlkValueList();

    int zCount = 0;
    int xCount = 0;
    int yCount = 0;
    QList<qreal> hexValue_ZList;
    QList<qreal> hexValue_XList;
    QList<qreal> hexValue_YList;

    //qreal getValue();

    QList<qreal> getZValueList();
    QList<qreal> getXValueList();
    QList<qreal> getYValueList();

    //int funIndex = -1;

public slots:
    //void setValue(qreal value);

    void setBlkValue(int startIndex, int count, qreal value);

    void curveCharValueChangedSlot(int startIndex, int count, qreal data);

signals:
    void charVarValueChanged(qreal);
    void charVarValueChanged(A2L_VarChar*);

    void curveCharZValueChanged(A2L_VarChar*, int, qreal);

private:

    //qreal Value;


};

#endif // A2L_VARCHAR_H
