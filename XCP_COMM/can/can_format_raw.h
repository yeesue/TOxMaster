#ifndef CAN_FORMAT_H
#define CAN_FORMAT_H

#include <QtGlobal>

void byte_switch(quint8 *byte1, quint8 *byte2);

int can_format_motorola2userdata(quint64 srcData,quint8 StartBit,
                                 quint32 LenBit,quint64 *dstData);
int can_format_intel2userdata(quint64 srcData,quint8 StartBit,
                              quint32 LenBit,quint64 *dstData);
int can_format_userdata2motorola(quint64 srcData,quint8 StartBit,
                                 quint32 LenBit,quint64 *dstData);
int can_format_userdata2motorola_2(quint64 srcData,quint8 StartBit,
                                 quint32 LenBit,quint64 *dstData);
int can_format_userdata2intel(quint64 srcData,quint8 StartBit,
                              quint32 LenBit,quint64 *dstData);

void can_format_motorola2intel_rawdata(quint64 srcData, quint64 *dstData);
void can_format_motorola2intel_sb_lb(quint16 *StartBit, quint16 *LenBit);
void can_format_motorola2mdf_sb_lb(quint16 &StartBit, quint16 LenBit);

quint32 CanRawMoto2UIntUserData(quint8 *raw, quint16 sb, quint16 lb);
qint32 CanRawMoto2IntUserData(quint8 *raw, quint16 sb, quint16 lb);
quint32 CanRawIntel2UIntUserData(quint8 *raw, quint16 sb, quint16 lb);
qint32 CanRawIntel2IntUserData(quint8 *raw, quint16 sb, quint16 lb);

#endif // CAN_FORMAT_H

