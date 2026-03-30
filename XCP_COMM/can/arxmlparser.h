#ifndef ARXMLPARSER_H
#define ARXMLPARSER_H

#include <QObject>
#include <QDomDocument>
#include <QFile>
#include <QTextStream>
#include "ar_can_format.h"
#include "config.h"

class ArxmlParser : public QObject
{
    Q_OBJECT
public:
    explicit ArxmlParser(QObject *parent = nullptr, QString filePath = "");
    ~ArxmlParser();

    bool getParserOk() const;

    QString getArxmlName() const;

    AR_XML_PACKAGES *getArXmlPackages() const;

    QList<dbc_Cluster *> getCanClusters() const;

    QStringList getGenDbcFileList() const;

    QHash<QString, QString> getClusterDbcHash() const;

signals:

private slots:
    QString canClusterToDBC(dbc_Cluster *dbcCluster);

private:
    bool parserArxml(QString filePath);

    void arxmlPackagesToClusters(AR_XML_PACKAGES *arXmlPackages);
    void arCanClusterToCluster(AR_CAN_CLUSTER_SIM *arCanCluster, dbc_Cluster *dbcCluster);

    AR_SIGNAL_TRIG* findSignalTrigPtrByName(QList<AR_SIGNAL_TRIG*> sigTrigList, QString name);
    AR_PDU_TRIG* findPDUTrigPtrByName(QList<AR_PDU_TRIG*> pduTrigList, QString name);
    AR_PDU_CONTAINER* findPDUContainerPtrByName(QList<AR_PDU_CONTAINER*> pduList, QString name);
    AR_PDU* findPDUPtrByName(QList<AR_PDU*> pduList, QString name);
    AR_FRAME* findFramePtrByName(QList<AR_FRAME*> frameList, QString name);
    AR_SIGNAL* findSignalPtrByName(QList<AR_SIGNAL*> signalList, QString name);
    AR_SYS_SIGNAL* findSysSignalPtrByName(QList<AR_SYS_SIGNAL*> sysSignalList, QString name);
    AR_BASETYPE* findBaseTypePtrByName(QList<AR_BASETYPE*> baseTypeList, QString name);
    AR_COMPU_METHOD* findCompuMethodPtrByName(QList<AR_COMPU_METHOD*> compuMethodList, QString name);
    AR_CAN_COMM_CONTROLLER* findCanCommControllerByName(QList<AR_CAN_COMM_CONTROLLER*> canCommControllerList, QString name);
    AR_CAN_COMM_CONNECTOR* findCanCommConnectorByName(QList<AR_CAN_COMM_CONNECTOR*> canCommConnectorList, QString name);

    AR_FRAME* findFrameByRefPDU(QList<AR_FRAME*> frameList, AR_PDU *pdu);

    QString textOfFirstElementByTagName(QDomElement elem, QString tagName);
    QString textOfFirstElementByTagNames(QDomElement elem, QString tagName1, QString tagName2);
    QString textOfIndexElementByTagName(QDomElement elem, QString tagName, int index);
    QStringList textListOfElementsByTagName(QDomElement elem, QString tagName_Multi);
    QStringList textListOfElementsByTagNames(QDomElement elem, QString tagName_Single, QString tagName_Multi);

    void writeTextLine(QString textLine);
    void writeText_dbc_Prefix();

    void writeText_dbc_FrameAndSignal(dbc_Cluster *dbcCluster);
    void writeText_dbc_Attrs(dbc_Cluster *dbcCluster);
    void writeText_dbc_Frame_Attrs(dbc_Cluster *dbcCluster);
    void writeText_dbc_Signal_Attrs(dbc_Cluster *dbcCluster);
    void writeText_dbc_Signal_ValTables(dbc_Cluster *dbcCluster);

    QString addQuotes(QString text);

    QString textLineFrame_BO_(dbc_frame *frame);
    QString textLineSignal_SG_(dbc_signal *signal);

    QString attrTextLine_BA_DEF_(QString Category, QString attrName, QString type, QStringList valueList);
    QString attrTextLine_BA_DEF_DEF_(QString attrName, QString initValue);
    QString attrTextLine_BA_(QString attrName, QString Category, QStringList infoList, QString initValue);
    QString valTextLine_VAL_(QStringList infoList, QList<ValTable*> valTableList);

private:
    bool parserOk = false;
    QString arxmlFilePath = "";
    QString arxmlName = "";
    AR_XML_PACKAGES *arXmlPackages = NULL;

    QList<dbc_Cluster*> canClusters;
    QStringList genDbcFileList;
    QHash<QString, QString> clusterDbcHash;

    QFile dbcFile;
    QTextStream out;

    QString NetWorkNode;
};

#endif // ARXMLPARSER_H
