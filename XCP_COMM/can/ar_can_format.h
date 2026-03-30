#ifndef AR_CAN_FORMAT_H
#define AR_CAN_FORMAT_H

#include <QString>

enum compuMethodCategory{
    LINEAR,
    TEXTABLE
};

struct AR_SCALE_TEXTTABLE{
    QString shortLabel;
    quint32 lowerLimit;
    quint32 upperLimit;
    QString compuConstant;
};

struct AR_BASETYPE{
    QString shortName;
    QString category;
    quint32 baseTypeSize;
    QString baseTypeEncoding;
};

struct AR_COMPU_METHOD{
    QString shortName;
    QString category;

    qreal p1;
    qreal p2;
    qreal denominator;
    qreal lowerLimit;
    qreal upperLimit;

    QList<AR_SCALE_TEXTTABLE*> scaleTextTableList;
};

struct AR_SYS_SIGNAL{
    QString shortName;
    AR_COMPU_METHOD *refCompuMethod;
};

struct AR_SIGNAL{
    QString shortName;
    quint32 sigLen;
    quint32 startpos;
    QString packingOrder;
    QString transferProperty;
    qreal initValue;

    AR_BASETYPE *refBaseType;
    AR_COMPU_METHOD *refCompuMethod;

};

struct AR_SIGNAL_TRIG{
    QString shortName;
    AR_SIGNAL *refSig;
};

struct AR_PDU{
    QString shortName;
    quint32 pduLen;
    qreal timing_s;
    quint32 headerID;

    QList<AR_SIGNAL*> refSigList;
};

struct AR_PDU_TRIG;

struct AR_PDU_CONTAINER{
    QString shortName;
    quint32 pduContainerLen;
    QString headerType;
    QStringList refPDUTrigStrList;
    QList<AR_PDU_TRIG*> refPDUTrigList;
};


struct AR_PDU_TRIG{
    QString shortName;

    QString refPDUDest;
    AR_PDU* refPDU;
    AR_PDU_CONTAINER* refPDUContainer;

    QList<AR_SIGNAL_TRIG*> refSigTrigList;
};


struct AR_FRAME{
    QString shortName;
    quint32 id;
    quint32 frameLen;
    qreal timing_s = 0;
    bool isExtented;
    bool CAN_FD_supported;
    QString FrameRXBehavior;
    QString FrameTXBehavior;

    quint32 startBitOffset = 0;

    QString refPDUDest;
    AR_PDU *refPDU = NULL;
    AR_PDU_CONTAINER *refPDUContainer = NULL;
};

struct AR_FRAME_TRIG{
    QString shortName;
    quint32 id;
    QString frameAddressMode;
    QString CANFDSupported;
    QString FrameRXBehavior;
    QString FrameTXBehavior;

    quint32 startBitOffset = 0;

    AR_FRAME *refFrame = NULL;
    AR_PDU_TRIG *refPDUTrig = NULL;
};

struct AR_CAN_CLUSTER{
    QString shortName;
    quint32 Baudrate;
    quint32 CANFD_Baudrate;

    quint8 ProgSeg;
    quint8 SJW;
    quint8 TSeg1;
    quint8 TSeg2;
    qreal SamplePoint;

    bool FD_BRS;
    quint8 FD_ProgSeg;
    quint8 FD_SJW;
    quint8 FD_TSeg1;
    quint8 FD_TSeg2;
    qreal FD_SamplePoint;

};

struct AR_CAN_COMM_CONTROLLER{
    QString canCommControllerName;

    quint8 ProgSeg;
    quint8 SJW;
    quint8 TSeg1;
    quint8 TSeg2;
    qreal SamplePoint;

    bool FD_BRS;
    quint8 FD_ProgSeg;
    quint8 FD_SJW;
    quint8 FD_TSeg1;
    quint8 FD_TSeg2;
    qreal FD_SamplePoint;
};

struct AR_CAN_COMM_CONNECTOR{
    QString canCommConnectorName;

    QString refCanCommControllerName;
    AR_CAN_COMM_CONTROLLER *refCanCommConntroller = NULL;
};

struct AR_CAN_CLUSTER_SIM{
    QString canClusterName;
    quint32 Baudrate;
    quint32 CANFD_Baudrate;

    AR_CAN_COMM_CONTROLLER *refCanCommController = NULL;

    QList<AR_FRAME_TRIG*> ar_FrameTrigList;
    QList<AR_SIGNAL_TRIG*> ar_SignalTrigList;
    QList<AR_PDU_TRIG*> ar_PDUTrigList;

    QString postfix_FD;
    QString postfix_nonFD;
};


struct AR_PACKAGES{
    QList<AR_BASETYPE*> ar_BaseTypeList;
    QList<AR_COMPU_METHOD*> ar_CompuMethodList;
    QList<AR_SYS_SIGNAL*> ar_SysSignalList;
    QList<AR_SIGNAL*> ar_SignalList;
    QList<AR_SIGNAL_TRIG*> ar_SignalTrigList;
    QList<AR_PDU*> ar_PDUList;
    QList<AR_PDU_TRIG*> ar_PDUTrigList;
    QList<AR_PDU_CONTAINER*> ar_PDUContainerList;
    QList<AR_FRAME*> ar_FrameList;
    QList<AR_FRAME_TRIG*> ar_FrameTrigList;

    AR_CAN_CLUSTER *ar_CAN_Clusster;
};


struct AR_XML_PACKAGES{
    QString ar_ECU_Instance;
    QList<AR_BASETYPE*> ar_BaseTypeList;
    QList<AR_COMPU_METHOD*> ar_CompuMethodList;
    QList<AR_SYS_SIGNAL*> ar_SysSignalList;
    QList<AR_SIGNAL*> ar_SignalList;
    QList<AR_PDU*> ar_PDUList;
    QList<AR_PDU_CONTAINER*> ar_PDUContainerList;
    QList<AR_FRAME*> ar_FrameList;

    QList<AR_CAN_CLUSTER_SIM*> ar_CANClusterList;
    QList<AR_CAN_COMM_CONTROLLER*> ar_CANCommControllerList;
};

struct GEN_DBC_INFO{
    QString genDBCName;
    QString genDBCPath;
};


#endif // AR_CAN_FORMAT_H
