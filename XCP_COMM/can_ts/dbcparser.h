#ifndef DBCPARSER_H
#define DBCPARSER_H

#include <QObject>
#include <QHash>

class VAL{
public:
    QString name;
    quint64 val;
};

class PERIOD{
public:
    quint16 m_id;
    quint16 m_period;
};

class TX_TYPE{
public:
    quint16 m_id;
    quint16 m_tx_type;
};

class SG_START_VALUE{
public:
    quint16 m_id;
    QString sgName;
    double sgStartValue;
};

class VAL_TABLE{
public:
    QString m_name;
    QList<VAL> m_valList;
};

class SG_ {
public:
    QString m_name;
    QString m_multiplexer;
    quint16 m_startBit;
    quint16 m_startBit_oth;
    quint8 m_bitLen;
    quint8 m_type;        // 0 = motorola, 1 = intel    according to dbc file parsing synax
    quint8 m_valType;    // 0 = signed, 1 = unsigned, 2 = float, 3 = double   according to dbc editor++
    quint8 m_byteorder;  // = m_type
    double m_factor;
    double m_offset;
    double m_min;
    double m_max;
    double m_value_default;
    double m_value;
    QString m_unit;
    QString m_recv;
    QList<VAL> m_valList;

};

Q_DECLARE_METATYPE(SG_*)

class BO_ {
public:
    QString m_name;
    quint32 m_id;
    quint8 m_extented;
    quint8 m_length;
    quint16 m_period;
    quint8 m_send_type;   //"0=cyclic","1=Spontaneous","2=cyclicIfActive","3=spontaneousWithDelay","4=Immediate";
    QString m_trans;
    QList<SG_*> m_sgList;

    quint8 can_io_mode;
    QList<SG_*> pdu_header_id_sgList;
    QHash<quint32, int> headerId_index_hash;

    QString smKey = "";
    //quint8 m_data[8];
};

Q_DECLARE_METATYPE(BO_*)

class CLUSTER_ {
public:
    QString m_name;
    quint64 m_baudrate;
    quint64 m_baudrate_fd;
    float m_sp_can;
    float m_sp_canfd;

    quint8 bus_type;     // 0 = can, 1 = can fd
    quint8 can_io_mode;

    QList<BO_*> m_boList;
    QList<BO_*> m_pduList;
};

class CPos{
public:
    int byteIndex;
    int bitIndex;
};

class DbcParser:public QObject
{
    Q_OBJECT
public:
    DbcParser();
    ~DbcParser();

    bool doParsering(const QString& file);

    QList<BO_*>& getBOList();
    CLUSTER_ *getCluster() const;

    static CPos posCalc(quint8 start, quint8 offset, quint8 format);
    static CPos offsetToIndex(quint8 start, quint8 offset, quint8 format);
    static CPos indexToPos(int index); //绝对位置转pos结构

private:
    QString m_version;
    QList<BO_*> m_boList;
    CLUSTER_ *m_cluster;

    int findValTblIndex(QString name,const QList<VAL_TABLE>& valList);
    int findStartValueIndex(QString name, QList<SG_START_VALUE>& startValueList);
    int findPeriodIndex(quint16 id,const QList<PERIOD>& pdList);
    int findSendTypeIndex(quint16 id,const QList<TX_TYPE>& txList);

    quint8 sbMotoConv(quint8 sb, quint8 lb);
    quint16 sbMotoConv_FD(quint16 sb, quint8 lb);
};

#endif // DBCPARSER_H
