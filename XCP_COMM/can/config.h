#ifndef CONFIG_H
#define CONFIG_H

#include <QString>
#include <QHash>
#include <QMetaType>


enum frm_TimingType
{
    CyclicData,
    EventData,
    CyclicRemote,
    EventRemote
};

enum Sig_ByteOrder
{
    Intel_Order,
    Motorola_Order
};

enum Sig_Val_Type
{
    Signed_Int,
    Unsigned_Int,
    IEEE_Float,
    IEEE_Double
};

enum Sig_Val_Assign_Mode
{
    Constant = 0,
    Variable = 1,
    RC = 2,
    CRC = 3
};

struct ValTable
{
    quint32 value;
    QString enumText;
};

struct CheckSumInfo
{
    quint8 startIndex_CS_data = 0;
    quint8 endIndex_CS_data = 0;
    quint8 index_CS = 0;
};

struct dbc_signal
{
    QString m_name;
    QString m_unit;

    quint32 m_byteOrder;  // 0 = Little Endian(Intel), 1 = Big Endian(Moto)
    quint32 m_valType;    // 0 = signed, 1 = unsigned, 2 = float, 3 = double
    quint32 m_bitLen = 0;
    quint32 m_startBit = 0;
    quint32 m_startBit_oth = 0;

    double m_value_default = 0;
    double m_factor = 1.0;
    double m_offset = 0.0;
    double m_maximum = 0;
    double m_minimum = 0;
    double m_value = 0;

    QList<ValTable*> valTableList;
    Sig_Val_Assign_Mode assignMode = Constant;
    QStringList sigAssignModePams;
    QStringList checkSumInfos;
};

Q_DECLARE_METATYPE(dbc_signal*)

struct dbc_frame
{
    QString m_name;
    bool m_extented = false;
    quint32 m_id = 0;
    quint32 m_dlc = 0;
    quint32 m_timingType;
    quint32 m_cyclicTime = 0;
    quint32 m_can_io_mode = 0;
    QList<dbc_signal*> m_sigs;

    QList<dbc_signal*> pduHeaderID_sigs;
    QHash<quint32, int> headerIDIndexHash;

    bool flagChecksum = false;
    quint8 indexChecksum = 0;
    QList<CheckSumInfo> checkSumInfoList;

    QString smKey = "";
};

Q_DECLARE_METATYPE(dbc_frame*)

struct dbc_Cluster
{
    QString m_clusterName;
    quint64 m_can_baudrate;
    quint64 m_canfd_baudrate;

    qreal m_can_samplePoint;
    qreal m_canfd_samplePoint;

    QString m_io_mode;
    quint32 m_can_io_mode;

    QList<dbc_frame*> m_frames;
    QList<dbc_frame*> m_pdus;

    QString postfix_signal;

};


#endif // CONFIG_H
