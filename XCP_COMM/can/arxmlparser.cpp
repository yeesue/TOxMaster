#include "arxmlparser.h"
#include "logger.h"
#include <QDebug>
#include <QFile>
#include <QFileInfo>
#include <QApplication>

ArxmlParser::ArxmlParser(QObject *parent, QString filePath) :
    QObject(parent),
    arxmlFilePath(filePath)
{
    QFileInfo fileInfo(arxmlFilePath);
    if(!fileInfo.isFile())
    {
        LOG_WARN_STREAM() << "arxml file is invalid";
    }

    arxmlName = arxmlFilePath.split('/').last().split('.').first();

    parserOk = parserArxml(arxmlFilePath);

    arxmlPackagesToClusters(this->arXmlPackages);
}

ArxmlParser::~ArxmlParser()
{
    if(arXmlPackages)
    {
        delete arXmlPackages;
        arXmlPackages = NULL;
    }
}


QStringList ArxmlParser::getGenDbcFileList() const
{
    return genDbcFileList;
}

QList<dbc_Cluster *> ArxmlParser::getCanClusters() const
{
    return canClusters;
}

QString ArxmlParser::canClusterToDBC(dbc_Cluster *dbcCluster)
{
    if(dbcCluster == NULL)
        return "";

    QString genDBCFileName = dbcCluster->m_clusterName + "_" + arxmlName + ".dbc";

    if(dbcCluster->m_can_io_mode == 2)
    {
        genDBCFileName = "canfd_"+genDBCFileName;
    }
    LOG_DEBUG_STREAM() << "ready to gen dbc file name:" << genDBCFileName;
    QString genDBCFilePath = QApplication::applicationDirPath() + "/arxml_to_dbc/" + genDBCFileName;

    //genDBCFilePath.replace(genFileName, dbcCluster->m_clusterName + "_" + genDBCFileName);
    LOG_DEBUG_STREAM() << "ready to gen dbc file path:" << genDBCFilePath;

    if (genDBCFilePath.isEmpty())
        return "";

    dbcFile.setFileName(genDBCFilePath);
    if (!dbcFile.open(QIODevice::WriteOnly))
    {
        LOG_ERROR_STREAM() << "File Open error.";
        return "";
    }

    out.setDevice(&dbcFile);

    writeText_dbc_Prefix();
    writeText_dbc_FrameAndSignal(dbcCluster);
    writeText_dbc_Attrs(dbcCluster);
    writeText_dbc_Frame_Attrs(dbcCluster);
    writeText_dbc_Signal_Attrs(dbcCluster);
    writeText_dbc_Signal_ValTables(dbcCluster);

    dbcFile.close();

    genDbcFileList.append(genDBCFilePath);

    LOG_INFO_STREAM() << tr("arxml to dbc file created : [%1]Path: %2")
                      .arg(genDBCFileName).arg(genDBCFilePath);

    return genDBCFilePath;
}

QString ArxmlParser::getArxmlName() const
{
    return arxmlName;
}

AR_XML_PACKAGES *ArxmlParser::getArXmlPackages() const
{
    return arXmlPackages;
}


bool ArxmlParser::getParserOk() const
{
    return parserOk;
}

bool ArxmlParser::parserArxml(QString filePath)
{
    LOG_INFO_STREAM() << "===========Start of Arxml Parsing==========";
    LOG_INFO_STREAM() << "ARXML File:" << filePath;
    if (filePath.isEmpty())
        return false;

    QFile arxmlFile(filePath);
    if (!arxmlFile.open(QFile::ReadOnly | QFile::Text))
    {
        LOG_ERROR_STREAM() << tr("Cannot read file %1:\n%2.").arg(filePath).arg(arxmlFile.errorString());
        return false;
    }

    QDomDocument arxml("ARXML_INFO");

    if (!arxml.setContent(&arxmlFile))
    {
        arxmlFile.close();
        return false;
    }
    arxmlFile.close();

    LOG_INFO_STREAM() << "==========Begin to parse the file============";
    QDomElement root = arxml.documentElement();
    QString schemaLocation = root.attribute("xsi:schemaLocation");
    //LOG_DEBUG() << "Tag=" << root.tagName() << ",schemalocation=" << schemaLocation;

    /*-----------ECU Instance-------------*/

    QDomElement ECUInstanceElem = root.elementsByTagName("ECU-INSTANCE").at(0).toElement();

    QString ECUInstanceName = textOfFirstElementByTagName(ECUInstanceElem, "SHORT-NAME");
    //LOG_DEBUG() << "ECU Instance:" << ECUInstanceName;

    LOG_INFO_STREAM() << "===============CAN COMMUNICATION CONTROLLER parser===============";
    QList<AR_CAN_COMM_CONTROLLER*> arCanCommControllerList;
    QDomNodeList canCommConNodeList = ECUInstanceElem.elementsByTagName("CAN-COMMUNICATION-CONTROLLER");
    for(int i = 0; i < canCommConNodeList.count(); i++)
    {
        AR_CAN_COMM_CONTROLLER *ar_CanCommController = new AR_CAN_COMM_CONTROLLER;

        QDomElement canCommConElem = canCommConNodeList.at(i).toElement();

        QString canCommConName = textOfFirstElementByTagName(canCommConElem, "SHORT-NAME");
        //qDebug()<<i<<"-CAN_Comm_Controller:"<<canCommConName;
        ar_CanCommController->canCommControllerName = canCommConName;

        QDomElement CANCtrlConfigElem = canCommConElem.elementsByTagName("CAN-CONTROLLER-CONFIGURATION").at(0).toElement();

        QDomNodeList CANFDAttrNodeList = CANCtrlConfigElem.elementsByTagName("CAN-CONTROLLER-FD-ATTRIBUTES");

        QString FD_PropSeg = textOfFirstElementByTagNames(CANCtrlConfigElem, "CAN-CONTROLLER-FD-ATTRIBUTES", "PROP-SEG");
        QString FD_SJW = textOfFirstElementByTagNames(CANCtrlConfigElem, "CAN-CONTROLLER-FD-ATTRIBUTES", "SYNC-JUMP-WIDTH");
        QString FD_TSEG1 = textOfFirstElementByTagNames(CANCtrlConfigElem, "CAN-CONTROLLER-FD-ATTRIBUTES", "TIME-SEG-1");
        QString FD_TSEG2 = textOfFirstElementByTagNames(CANCtrlConfigElem, "CAN-CONTROLLER-FD-ATTRIBUTES", "TIME-SEG-2");
        QString FD_TDCO = textOfFirstElementByTagNames(CANCtrlConfigElem, "CAN-CONTROLLER-FD-ATTRIBUTES", "TRCV-DELAY-COMPENSATION-OFFSET");
        QString FD_BRS = textOfFirstElementByTagNames(CANCtrlConfigElem, "CAN-CONTROLLER-FD-ATTRIBUTES", "TX-BIT-RATE-SWITCH");
        //qDebug()<<"FD_PropSeg="<<FD_PropSeg<<", FD_SJW="<<FD_SJW<<", FD_TSEG1="<<FD_TSEG1<<", FD_TSEG="<<FD_TSEG2<<", FD_TDCO="<<FD_TDCO<<", FD_BRS="<<FD_BRS;

        ar_CanCommController->FD_BRS = (FD_BRS == "True") ? true : false;
        ar_CanCommController->FD_ProgSeg = FD_PropSeg.toInt();
        ar_CanCommController->FD_SJW = FD_SJW.toInt();
        ar_CanCommController->FD_TSeg1 = FD_TSEG1.toInt();
        ar_CanCommController->FD_TSeg2 = FD_TSEG2.toInt();
        ar_CanCommController->FD_SamplePoint = (qreal)(1 + ar_CanCommController->FD_ProgSeg + ar_CanCommController->FD_TSeg1)/(qreal)(1 + ar_CanCommController->FD_ProgSeg + ar_CanCommController->FD_TSeg1 + ar_CanCommController->FD_TSeg2);

        int index = 0;
        if(!CANFDAttrNodeList.isEmpty())
        {
            index = 1;
        }

        QString PropSeg = textOfIndexElementByTagName(CANCtrlConfigElem, "PROP-SEG", index);
        QString SJW = textOfIndexElementByTagName(CANCtrlConfigElem, "SYNC-JUMP-WIDTH", index);
        QString TSEG1 = textOfIndexElementByTagName(CANCtrlConfigElem, "TIME-SEG-1", index);
        QString TSEG2 = textOfIndexElementByTagName(CANCtrlConfigElem, "TIME-SEG-2", index);
        //qDebug()<<"PropSeg="<<PropSeg<<", SJW="<<SJW<<", TSEG1="<<TSEG1<<", TSEG="<<TSEG2;

        ar_CanCommController->ProgSeg = PropSeg.toInt();
        ar_CanCommController->SJW = SJW.toInt();
        ar_CanCommController->TSeg1 = TSEG1.toInt();
        ar_CanCommController->TSeg2 = TSEG2.toInt();
        ar_CanCommController->SamplePoint = (qreal)(1 + ar_CanCommController->ProgSeg + ar_CanCommController->TSeg1)/(qreal)(1 + ar_CanCommController->ProgSeg + ar_CanCommController->TSeg1 + ar_CanCommController->TSeg2);

        arCanCommControllerList.append(ar_CanCommController);
    }

    qDebug()<<"===============CAN COMMUNICATION CONNECTOR parser===============";
    QList<AR_CAN_COMM_CONNECTOR*> arCanCommConnectorList;
    QDomNodeList canCommConnectorNodeList = ECUInstanceElem.elementsByTagName("CAN-COMMUNICATION-CONNECTOR");
    for(int i = 0; i < canCommConnectorNodeList.count(); i++)
    {
        AR_CAN_COMM_CONNECTOR *ar_CanCommConnector = new AR_CAN_COMM_CONNECTOR;

        QDomElement canCommConnectorElem = canCommConnectorNodeList.at(i).toElement();

        QString canCommConnectorName = textOfFirstElementByTagName(canCommConnectorElem, "SHORT-NAME");
        //qDebug()<<i<<"-CAN_Comm_Connector:"<<canCommConnectorName;

        QString refCommControllerName = textOfFirstElementByTagName(canCommConnectorElem, "COMM-CONTROLLER-REF").split('/').last();
        AR_CAN_COMM_CONTROLLER *refCanCommController = findCanCommControllerByName(arCanCommControllerList, refCommControllerName);
        //qDebug()<<"-CAN_Comm_Controller:"<<refCommControllerName;
        //qDebug()<<"-CAN_Comm_Controller from controller:"<<refCanCommController->canCommControllerName;

        ar_CanCommConnector->canCommConnectorName = canCommConnectorName;
        ar_CanCommConnector->refCanCommControllerName = refCommControllerName;
        ar_CanCommConnector->refCanCommConntroller = refCanCommController;

        arCanCommConnectorList.append(ar_CanCommConnector);
    }

    /*----------------BASE-TYPE Parser-----------------*/
    qDebug()<<"===============BASE-TYPE parser===============";
    QList<AR_BASETYPE*> arBaseTypeList;
    QStringList swBaseTypeList;
    QDomNodeList swBaseTypeNodeList = root.elementsByTagName("SW-BASE-TYPE");
    //qDebug()<<"BASETYPE number="<<swBaseTypeNodeList.count();
    for (int i = 0; i < swBaseTypeNodeList.count(); i++)
    {
        QDomElement elem = swBaseTypeNodeList.at(i).toElement();
        QString swBaseTypeName = elem.elementsByTagName("SHORT-NAME").at(0).toElement().text();
        swBaseTypeList<<swBaseTypeName;

        QString category = elem.elementsByTagName("CATEGORY").at(0).toElement().text();
        QString baseTypeSize = elem.elementsByTagName("BASE-TYPE-SIZE").at(0).toElement().text();
        QString baseTypeEncoding = elem.elementsByTagName("BASE-TYPE-ENCODING").at(0).toElement().text();

        AR_BASETYPE *baseType = new AR_BASETYPE;
        baseType->shortName = swBaseTypeName;
        baseType->category = category;
        baseType->baseTypeSize = baseTypeSize.toInt();
        baseType->baseTypeEncoding = baseTypeEncoding;

        arBaseTypeList.append(baseType);

        //qDebug()<<i<<":baseType:"<<swBaseTypeName<<",Category="<<category<<", baseTypeSize="<<baseTypeSize<<",encoding="<<baseTypeEncoding;
    }

    /*---------------COMPU-METHOD Parser---------------*/
    qDebug()<<"===============COMPU-METHOD parser===============";
    QList<AR_COMPU_METHOD*> arCompuMethodList;
    QStringList compuMethodList;
    QDomNodeList compuMethodNodeList = root.elementsByTagName("COMPU-METHOD");
    //qDebug()<<"COMPU-METHOD number="<<compuMethodNodeList.count();
    for (int i = 0; i < compuMethodNodeList.count(); i++)
    {
        QDomElement elem = compuMethodNodeList.at(i).toElement();
        QString compuMethodName = textOfFirstElementByTagName(elem, "SHORT-NAME");
        compuMethodList<<compuMethodName;

        QString category = textOfFirstElementByTagName(elem, "CATEGORY");

        AR_COMPU_METHOD *compuMethod = new AR_COMPU_METHOD;
        compuMethod->shortName = compuMethodName;
        compuMethod->category = category;
        compuMethod->p1 = 0;
        compuMethod->p2 = 1;

        if (category == "LINEAR")
        {
            QString lowerLim = textOfFirstElementByTagNames(elem, "COMPU-SCALES", "LOWER-LIMIT");
            QString upperLim = textOfFirstElementByTagNames(elem, "COMPU-SCALES", "UPPER-LIMIT");
            QStringList numerList = textListOfElementsByTagNames(elem, "COMPU-NUMERATOR", "V");
            QString numerator1, numerator2;
            if (numerList.count()==2)
            {
                numerator1 = numerList.at(0);
                numerator2 = numerList.at(1);
            }
            QString denominator = textOfFirstElementByTagNames(elem, "COMPU-DENOMINATOR", "V");

            compuMethod->p1 = numerator1.toDouble();
            compuMethod->p2 = numerator2.toDouble();
            compuMethod->denominator = denominator.toDouble();
            compuMethod->lowerLimit = lowerLim.toDouble();
            compuMethod->upperLimit = upperLim.toDouble();

            //qDebug()<<i<<":compuMethod:"<<compuMethodName<<",Category="<<category<<",numer1="<<numerator1
                   //<<",numer2="<<numerator2<<",denominator="<<denominator;
        }

        if (category == "")
        {
            QString lowerLim = textOfFirstElementByTagNames(elem, "COMPU-SCALES", "LOWER-LIMIT");
            QString upperLim = textOfFirstElementByTagNames(elem, "COMPU-SCALES", "UPPER-LIMIT");
            QStringList numerList = textListOfElementsByTagNames(elem, "COMPU-NUMERATOR", "V");
            QString numerator1, numerator2;
            if (numerList.count()==2)
            {
                numerator1 = numerList.at(0);
                numerator2 = numerList.at(1);
            }
            QString denominator = textOfFirstElementByTagNames(elem, "COMPU-DENOMINATOR", "V");

            compuMethod->p1 = numerator1.toDouble();
            compuMethod->p2 = numerator2.toDouble();
            compuMethod->denominator = denominator.toDouble();
            compuMethod->lowerLimit = lowerLim.toDouble();
            compuMethod->upperLimit = upperLim.toDouble();

            //qDebug()<<i<<":compuMethod:"<<compuMethodName<<",Category="<<category<<",numer1="<<numerator1
                   //<<",numer2="<<numerator2<<",denominator="<<denominator;
        }

        QList<AR_SCALE_TEXTTABLE*> arScaleTextTableList;
        if (category == "TEXTTABLE")
        {
            QDomNodeList nodeList = elem.elementsByTagName("COMPU-SCALE");

            //qDebug()<<i<<":compuMethod:"<<compuMethodName<<",Category="<<category<<"TextTable Count="<<nodeList.count();
            for (int j = 0; j < nodeList.count(); j++)
            {
                QDomElement elem1 = nodeList.at(j).toElement();
                QString shortLabel = textOfFirstElementByTagName(elem1, "SHORT-LABEL");
                QString lowerLim = textOfFirstElementByTagName(elem1, "LOWER-LIMIT");
                QString upperLim = textOfFirstElementByTagName(elem1, "UPPER-LIMIT");
                QString scaleConst = textOfFirstElementByTagNames(elem1, "COMPU-CONST", "VT");

                AR_SCALE_TEXTTABLE *scaleTextTable = new AR_SCALE_TEXTTABLE;
                scaleTextTable->shortLabel = shortLabel;
                scaleTextTable->lowerLimit = lowerLim.toInt();
                scaleTextTable->upperLimit = upperLim.toInt();
                scaleTextTable->compuConstant = scaleConst;

                arScaleTextTableList.append(scaleTextTable);

                //qDebug()<<j<<":TextTableScale:"<<shortLabel<<",lowerLim="<<lowerLim<<",upperLim="<<upperLim
                       //<<", scaleConst="<<scaleConst;

            }
        }

        if (category == "SCALE_LINEAR_AND_TEXTTABLE")
        {
            QString lowerLim = textOfFirstElementByTagNames(elem, "COMPU-SCALES", "LOWER-LIMIT");
            QString upperLim = textOfFirstElementByTagNames(elem, "COMPU-SCALES", "UPPER-LIMIT");
            QStringList numerList = textListOfElementsByTagNames(elem, "COMPU-NUMERATOR", "V");
            QString numerator1, numerator2;
            if (numerList.count()==2)
            {
                numerator1 = numerList.at(0);
                numerator2 = numerList.at(1);
            }
            QString denominator = textOfFirstElementByTagNames(elem, "COMPU-DENOMINATOR", "V");

            compuMethod->p1 = numerator1.toDouble();
            compuMethod->p2 = numerator2.toDouble();
            compuMethod->denominator = denominator.toDouble();
            compuMethod->lowerLimit = lowerLim.toDouble();
            compuMethod->upperLimit = upperLim.toDouble();

            //qDebug()<<i<<":compuMethod:"<<compuMethodName<<",Category="<<category<<",numer1="<<numerator1
                   //<<",numer2="<<numerator2<<",denominator="<<denominator;


            QDomNodeList nodeList = elem.elementsByTagName("COMPU-SCALE");

            //qDebug()<<i<<":compuMethod:"<<compuMethodName<<",Category="<<category<<"TextTable Count="<<nodeList.count();
            for (int j = 0; j < nodeList.count(); j++)
            {
                QDomElement elem1 = nodeList.at(j).toElement();
                QString shortLabel = textOfFirstElementByTagName(elem1, "SHORT-LABEL");
                QString lowerLim = textOfFirstElementByTagName(elem1, "LOWER-LIMIT");
                QString upperLim = textOfFirstElementByTagName(elem1, "UPPER-LIMIT");
                QString scaleConst = textOfFirstElementByTagNames(elem1, "COMPU-CONST", "VT");

                if(scaleConst.isEmpty())
                    continue;

                AR_SCALE_TEXTTABLE *scaleTextTable = new AR_SCALE_TEXTTABLE;
                scaleTextTable->shortLabel = shortLabel;
                scaleTextTable->lowerLimit = lowerLim.toInt();
                scaleTextTable->upperLimit = upperLim.toInt();
                scaleTextTable->compuConstant = scaleConst;

                arScaleTextTableList.append(scaleTextTable);

                //qDebug()<<j<<":TextTableScale:"<<shortLabel<<",lowerLim="<<lowerLim<<",upperLim="<<upperLim
                       //<<", scaleConst="<<scaleConst;

            }
        }

        compuMethod->scaleTextTableList = arScaleTextTableList;

        arCompuMethodList.append(compuMethod);
    }


    /*--------------SYSTEM-SIGNAL Parser---------------*/
    qDebug()<<"===============SYSTEM-SIGNAL parser===============";
    QList<AR_SYS_SIGNAL*> arSysSignalList;
    QStringList sysSigList;
    QDomNodeList sysSigNodeList = root.elementsByTagName("SYSTEM-SIGNAL");
    //qDebug()<<"SYSTEM-SIGNAL number :"<<sysSigNodeList.count();
    for (int i = 0; i < sysSigNodeList.count(); i++)
    {
        QDomElement elem = sysSigNodeList.at(i).toElement();
        QString sysSigName = textOfFirstElementByTagName(elem, "SHORT-NAME");
        sysSigList<<sysSigName;

        QString compuMethodRef = textOfFirstElementByTagName(elem, "COMPU-METHOD-REF").split('/').last();

        AR_SYS_SIGNAL *sysSignal = new AR_SYS_SIGNAL;
        sysSignal->shortName = sysSigName;
        sysSignal->refCompuMethod = findCompuMethodPtrByName(arCompuMethodList, compuMethodRef);

        arSysSignalList.append(sysSignal);
    }

    /*--------------I-SIGNAL Parser---------------*/
    qDebug()<<"===============I_SIGNAL parser===============";
    QList<AR_SIGNAL*> arSignalList;
    QStringList sigList;
    QDomNodeList sigNodeList = root.elementsByTagName("I-SIGNAL");
    //qDebug()<<"I-SIGNAL number :"<<sigNodeList.count();
    for (int i = 0; i < sigNodeList.count(); i++)
    {
        QDomElement elem = sigNodeList.at(i).toElement();
        QString sigName = textOfFirstElementByTagName(elem, "SHORT-NAME");
        sigList<<sigName;

        QString initValue = textOfFirstElementByTagNames(elem, "INIT-VALUE", "VALUE");
        QString sigLen = textOfFirstElementByTagName(elem, "LENGTH");
        QString baseTypeRef = textOfFirstElementByTagName(elem,"BASE-TYPE-REF").split('/').last();
        QString compuMethodRef = textOfFirstElementByTagName(elem, "COMPU-METHOD-REF").split('/').last();

        QString systemSignalRef = textOfFirstElementByTagName(elem, "SYSTEM-SIGNAL-REF").split('/').last();
        AR_SYS_SIGNAL *sysSignal = findSysSignalPtrByName(arSysSignalList, systemSignalRef);

        AR_SIGNAL *signal = new AR_SIGNAL;
        signal->shortName = sigName;
        signal->sigLen = sigLen.toInt();
        signal->refBaseType = findBaseTypePtrByName(arBaseTypeList, baseTypeRef);
        signal->refCompuMethod = findCompuMethodPtrByName(arCompuMethodList, compuMethodRef);
        if(signal->refCompuMethod == NULL && sysSignal != NULL)
        {
            signal->refCompuMethod = sysSignal->refCompuMethod;
        }
        signal->initValue = initValue.toDouble();

        arSignalList.append(signal);

        //qDebug()<<i<<":signal:"<<sigName<<",len="<<sigLen<<",baseType:"<<baseTypeRef<<", compuMethod:"<<compuMethodRef;
    }


    /*---------------PDU Parser-------------*/
    qDebug()<<"===============PDU parser===============";
    QList<AR_PDU*> arPDUList;
    QStringList pduList;
    QDomNodeList pduNodeList = root.elementsByTagName("I-SIGNAL-I-PDU");
    //qDebug()<<"PDU number:"<<pduNodeList.count();
    for (int i = 0; i < pduNodeList.count(); i++)
    {
        QDomElement elem = pduNodeList.at(i).toElement();
        QString pduName = elem.elementsByTagName("SHORT-NAME").at(0).toElement().text();
        pduList<<pduName;

        QString pduLen = elem.elementsByTagName("LENGTH").at(0).toElement().text();
        QString pduCycleTiming = elem.elementsByTagName("TIME-PERIOD").at(0).toElement().elementsByTagName("VALUE").at(0).toElement().text();
        QString pduHeaderID = textOfFirstElementByTagName(elem, "HEADER-ID-SHORT-HEADER");

        AR_PDU *pdu = new AR_PDU;
        pdu->shortName = pduName;
        pdu->pduLen = pduLen.toInt();
        pdu->timing_s = pduCycleTiming.toDouble();
        pdu->headerID = pduHeaderID.toInt();

        //如果PDU中没有周期值，从名称中提取周期字符进行替代
        QRegExp rx("(\\d+(ms))");
        if(rx.indexIn(pduName, 0) != -1)
        {
            QString timeStrInPduName = rx.cap(1);
            //qDebug()<<"PDU:"<<pduName<<",Contain timeStr:"<<timeStrInPduName;
            timeStrInPduName.replace("ms", "");
            //qDebug()<<"PDU:"<<pduName<<",Contain timeStr:"<<timeStrInPduName;
            if(pdu->timing_s == 0)
            {
                pdu->timing_s = timeStrInPduName.toDouble()/1000;
                //qDebug()<<"PDU:"<<pduName<<",time:"<<pdu->timing_s;
            }
        }

        //qDebug()<<i<<":PDU:"<<pduName<<",len="<<pduLen<<",Timing="<<pdu->timing_s<<", heraerID="<<pdu->headerID;

        QList<AR_SIGNAL*> refSignalList;
        QDomNodeList sigToPduMapList = elem.elementsByTagName("I-SIGNAL-TO-I-PDU-MAPPING");
        //qDebug()<<"refSignal number="<<sigToPduMapList.count();
        if (sigToPduMapList.count() != 0)
        {
            for(int j = 0; j < sigToPduMapList.count(); j++)
            {
                QDomElement elemMap = sigToPduMapList.at(j).toElement();
                QString sigToPduMapName = textOfFirstElementByTagName(elemMap, "I-SIGNAL-REF").split('/').last();
                QString sigToPduMapStartPos = textOfFirstElementByTagName(elemMap, "START-POSITION");
                QString packingByteOrder = textOfFirstElementByTagName(elemMap, "PACKING-BYTE-ORDER");
                QString transferProp = textOfFirstElementByTagName(elemMap, "TRANSFER-PROPERTY");

                AR_SIGNAL *signal = findSignalPtrByName(arSignalList, sigToPduMapName);
                if(signal == NULL)
                    continue;

                signal->startpos =sigToPduMapStartPos.toInt();
                signal->packingOrder = packingByteOrder;
                signal->transferProperty = transferProp;

                refSignalList.append(signal);

                //qDebug()<<"sigToPduMap : "<<sigToPduMapName<<", StartPos="<<sigToPduMapStartPos<<", PackingOrder="<<packingByteOrder;
            }
        }

        pdu->refSigList = refSignalList;
        arPDUList.append(pdu);
    }


    /*-------------PDU CONTAINER Parser------------*/
    qDebug()<<"===============PDU CONTAINER parser===============";
    QList<AR_PDU_CONTAINER*> arPDUContainerList;
    QStringList pduContainerList;
    QDomNodeList pduContainerNodeList = root.elementsByTagName("CONTAINER-I-PDU");
    //qDebug()<<"PDU-CONTAINER number:"<<pduContainerNodeList.count();
    for (int i = 0; i < pduContainerNodeList.count(); i++)
    {
        QDomElement elem = pduContainerNodeList.at(i).toElement();
        QString pduContainerName = textOfFirstElementByTagName(elem, "SHORT-NAME");
        pduContainerList<<pduContainerName;

        QString pduContainerLen = textOfFirstElementByTagName(elem, "LENGTH");
        QString headerType = textOfFirstElementByTagName(elem, "HEADER-TYPE");

        QList<AR_PDU_TRIG*> refPDUTrigList;
        QStringList refPDUTrigStrList;
        QDomNodeList containedPDUTrigList = elem.elementsByTagName("CONTAINED-PDU-TRIGGERING-REF");
        if (containedPDUTrigList.count() != 0)
        {

            for (int j = 0; j < containedPDUTrigList.count(); j++)
            {
                QDomElement elemRefPDUTrig = containedPDUTrigList.at(j).toElement();
                QString refPDUTrigName =  elemRefPDUTrig.text().split('/').last();
                refPDUTrigStrList<<refPDUTrigName;

                //AR_PDU_TRIG *pduTrig = findPDUTrigPtrByName(arPDUTrigList, refPDUTrigName);
                //refPDUTrigList.append(pduTrig);
            }
        }

        qSort(refPDUTrigStrList.begin(), refPDUTrigStrList.end());

        AR_PDU_CONTAINER *pduContainer = new AR_PDU_CONTAINER;
        pduContainer->shortName = pduContainerName;
        pduContainer->pduContainerLen = pduContainerLen.toInt();
        pduContainer->refPDUTrigStrList = refPDUTrigStrList;
        pduContainer->refPDUTrigList = refPDUTrigList;
        pduContainer->headerType = headerType;

        arPDUContainerList.append(pduContainer);

        //qDebug()<<i<<":pdu Container:"<<pduContainerName<<",len="<<pduContainerLen;
    }


    /*----------CAN-FRAME Parser--------*/
    qDebug()<<"===============CAN-FRAME parser===============";
    QList<AR_FRAME*> arFrameList;
    QStringList frameList;
    QDomNodeList frameNodeList = root.elementsByTagName("CAN-FRAME");
    //qDebug()<<"FRAME number :"<<frameNodeList.count();

    for (int i = 0; i < frameNodeList.count(); i++)
    {
        QDomElement elem = frameNodeList.at(i).toElement();
        QString frameName = elem.elementsByTagName("SHORT-NAME").at(0).toElement().text();
        frameList<<frameName;

        QString frameLen = elem.elementsByTagName("FRAME-LENGTH").at(0).toElement().text();

        QString packingByteOrder = textOfFirstElementByTagName(elem, "PACKING-BYTE-ORDER");
        //qDebug()<<i<<":frame:"<<frameName<<",len="<<frameLen<<",packingOrder="<<packingByteOrder;

        AR_FRAME *frame = new AR_FRAME;
        frame->shortName = frameName;
        frame->frameLen = frameLen.toInt();

        QList<AR_PDU*> refPDUList;
        QDomNodeList pduToFrameMapList = elem.elementsByTagName("PDU-TO-FRAME-MAPPING");
        if (pduToFrameMapList.count() != 0)
        {
            for(int j = 0; j < pduToFrameMapList.count(); j++)
            {
                QDomElement elemMap = pduToFrameMapList.at(j).toElement();
                QString pduToFrameMapName = textOfFirstElementByTagName(elemMap, "PDU-REF").split('/').last();
                QString destOfRefPDU = elemMap.elementsByTagName("PDU-REF").at(0).toElement().attribute("DEST");

                frame->refPDUDest = destOfRefPDU;

                AR_PDU *refPDU = NULL;
                AR_PDU_CONTAINER *refPDUContainer = NULL;
                if (destOfRefPDU == "I-SIGNAL-I-PDU")
                {
                    refPDU = findPDUPtrByName(arPDUList, pduToFrameMapName);
                    //frame->refPDU = refPDU;
                }
                if(refPDU)
                {
                    frame->refPDU = refPDU;
                    frame->timing_s = refPDU->timing_s;
                    frame->id = refPDU->headerID;
                }

                if (destOfRefPDU == "CONTAINER-I-PDU")
                {
                    refPDUContainer = findPDUContainerPtrByName(arPDUContainerList, pduToFrameMapName);
                    //frame->refPDUContainer = refPDUContainer;
                }
                if(refPDUContainer)
                {
                    frame->refPDUContainer = refPDUContainer;
                }

                //qDebug()<<"pduMapToFrame : "<<pduToFrameMapName;
            }
        }

        //frame->refPDUList = refPDUList;
        arFrameList.append(frame);
    }

    /*-----------CAN Cluster Info-------------*/
    qDebug()<<"===============CAN-CLUSTER parser===============";
    QList<AR_CAN_CLUSTER_SIM*> arCanClusterList;
    QDomNodeList clusterNodeList = root.elementsByTagName("CAN-CLUSTER");
    for(int i = 0; i < clusterNodeList.count(); i++)
    {
        AR_CAN_CLUSTER_SIM *ar_CanCluster = new AR_CAN_CLUSTER_SIM;

        QDomElement canClusterElem = clusterNodeList.at(i).toElement();
        QString cluName = canClusterElem.elementsByTagName("SHORT-NAME").at(0).toElement().text();
        qDebug()<<i<<"------------CAN-ClusterName:"<<cluName<<"-----------------";

        QString Baudrate = canClusterElem.elementsByTagName("BAUDRATE").at(0).toElement().text();
        qDebug()<<"BAUDRATE="<<Baudrate;
        ar_CanCluster->Baudrate = Baudrate.toInt();


        QString CANFD_Baudrate = canClusterElem.elementsByTagName("CAN-FD-BAUDRATE").at(0).toElement().text();
        qDebug()<<"CANFD_BAUDRATE="<<CANFD_Baudrate;
        ar_CanCluster->CANFD_Baudrate = CANFD_Baudrate.toInt();

        QString CommConnectorName = textOfFirstElementByTagName(canClusterElem, "COMMUNICATION-CONNECTOR-REF").split('/').last();
        //qDebug()<<"CAN Comm Connector:="<<CommConnectorName;
        AR_CAN_COMM_CONNECTOR *canCommConnector = findCanCommConnectorByName(arCanCommConnectorList, CommConnectorName);

        if(canCommConnector)
        {
            AR_CAN_COMM_CONTROLLER *canCommController = canCommConnector->refCanCommConntroller;
            if(canCommController)
            {
                //qDebug()<<"refCanCommController ="<<canCommController->canCommControllerName;
                ar_CanCluster->refCanCommController = canCommController;
            }
        }


        /*-------------SIG-TRIGGERINNG Parser-------------*/
        QList<AR_SIGNAL_TRIG*> arSigTrigList;
        QDomNodeList sigTrigNodeList = canClusterElem.elementsByTagName("I-SIGNAL-TRIGGERING");
        qDebug()<<cluName<<"/ I-SIGNAL TRIGGERING number :"<<sigTrigNodeList.count();
        for (int i = 0; i < sigTrigNodeList.count(); i++)
        {
            QDomElement elem = sigTrigNodeList.at(i).toElement();
            QString sigTrigName = textOfFirstElementByTagName(elem, "SHORT-NAME");
            QString refSigName = textOfFirstElementByTagName(elem, "I-SIGNAL-REF").split('/').last();

            AR_SIGNAL *refSig = findSignalPtrByName(arSignalList, refSigName);

            AR_SIGNAL_TRIG *sigTrig = new AR_SIGNAL_TRIG;
            sigTrig->shortName = sigTrigName;
            sigTrig->refSig = refSig;

            arSigTrigList.append(sigTrig);
        }

        ar_CanCluster->ar_SignalTrigList = arSigTrigList;

        /*-----------------PDU-TRIGGERING Parser---------------*/
        QList<AR_PDU_TRIG*> arPDUTrigList;
        QDomNodeList pduTrigNodeList = canClusterElem.elementsByTagName("PDU-TRIGGERING");
        qDebug()<<cluName<<"/ PDU-TRIGGERING number :"<<pduTrigNodeList.count();
        for (int i = 0; i < pduTrigNodeList.count(); i++)
        {
            QDomElement elem = pduTrigNodeList.at(i).toElement();
            QString pduTrigName = textOfFirstElementByTagName(elem, "SHORT-NAME");
            QString refPDUName = textOfFirstElementByTagName(elem, "I-PDU-REF").split('/').last();

            QDomElement elemRefPDU = elem.elementsByTagName("I-PDU-REF").at(0).toElement();
            QString refPDUDest = elemRefPDU.attribute("DEST");

            //qDebug()<<i<<": PDU TRIGGERING :"<<pduTrigName<<", refPDUName="<<refPDUName<<", refPDUDest="<<refPDUDest;

            AR_PDU *refPDU = NULL;
            AR_PDU_CONTAINER *refPDUContainer = NULL;
            if (refPDUDest == "I-SIGNAL-I-PDU" || refPDUDest == "DCM-I-PDU" || refPDUDest == "N-PDU" || refPDUDest == "NM-PDU")
            {
                refPDU = findPDUPtrByName(arPDUList, refPDUName);
                refPDUContainer = NULL;
            }

            if (refPDUDest == "CONTAINER-I-PDU")
            {
                refPDU = NULL;
                refPDUContainer = findPDUContainerPtrByName(arPDUContainerList, refPDUName);
            }

            QList<AR_SIGNAL_TRIG*> refSigTrigList;
            QDomNodeList refSigTrigNodeList = elem.elementsByTagName("I-SIGNAL-TRIGGERING-REF");
            //qDebug()<<"refSigTrig Number="<<refSigTrigNodeList.count();
            if (!refSigTrigNodeList.isEmpty())
            {
                for(int j = 0; j < refSigTrigNodeList.count(); j++)
                {
                    QDomElement elemRefSigTrig = refSigTrigNodeList.at(j).toElement();
                    QString refSigTrigName = elemRefSigTrig.text().split('/').last();

                    AR_SIGNAL_TRIG *refSigTrig = findSignalTrigPtrByName(arSigTrigList, refSigTrigName);

                    refSigTrigList.append(refSigTrig);
                }
            }

            AR_PDU_TRIG *pduTrig = new AR_PDU_TRIG;
            pduTrig->shortName = pduTrigName;
            pduTrig->refPDUDest = refPDUDest;
            pduTrig->refPDU = refPDU;
            pduTrig->refPDUContainer = refPDUContainer;
            pduTrig->refSigTrigList = refSigTrigList;

            arPDUTrigList.append(pduTrig);
        }

        ar_CanCluster->ar_PDUTrigList = arPDUTrigList;

        /*--------FRAME TRIGGERING Parser----------*/
        QList<AR_FRAME_TRIG*> arFrameTrigList;
        QStringList frameTrigList;
        QDomNodeList frameTrigNodeList = canClusterElem.elementsByTagName("CAN-FRAME-TRIGGERING");
        //qDebug()<<cluName<<"/ CAN-FRAME-TRIGGERING number :"<<frameTrigNodeList.count();
        for (int i = 0; i < frameTrigNodeList.count(); i++)
        {
            QDomElement elem = frameTrigNodeList.at(i).toElement();
            QString frameTrigName = elem.elementsByTagName("SHORT-NAME").at(0).toElement().text();
            frameTrigList<<frameTrigName;

            QString frameID = elem.elementsByTagName("IDENTIFIER").at(0).toElement().text();
            QString frameAddressMode = elem.elementsByTagName("CAN-ADDRESSING-MODE").at(0).toElement().text();
            QString CANFDSupported = elem.elementsByTagName("CAN-FD-FRAME-SUPPORT").at(0).toElement().text();
            QString FrameRXBehavior = elem.elementsByTagName("CAN-FRAME-RX-BEHAVIOR").at(0).toElement().text();
            QString FrameTXBehavior = elem.elementsByTagName("CAN-FRAME-TX-BEHAVIOR").at(0).toElement().text();

            //qDebug()<<i<<":frameTrig:"<<frameTrigName<<",id="<<frameID<<",CAN Address Mode:"<<frameAddressMode
                   //<<"CAN FD Supported:"<<CANFDSupported<<"FrameRXBehavior"<<FrameRXBehavior<<"FrameTXBehavior"<<FrameTXBehavior;

            QString FrameNameRef = elem.elementsByTagName("FRAME-REF").at(0).toElement().text().split('/').last();
            //qDebug()<<"ref Frame:"<<FrameNameRef;

            QString refPDUTrigName = textOfFirstElementByTagName(elem, "PDU-TRIGGERING-REF").split('/').last();
            AR_PDU_TRIG *refPDUTrig = findPDUTrigPtrByName(arPDUTrigList, refPDUTrigName);
            AR_FRAME* frame = findFramePtrByName(arFrameList, FrameNameRef);
            if (frame)
            {
                frame->id = frameID.toInt();
                frame->isExtented = (frameAddressMode=="STANDARD"? false : true);
                frame->CAN_FD_supported = (CANFDSupported == "true"? true : false);
                frame->FrameRXBehavior = FrameRXBehavior;
                frame->FrameTXBehavior = FrameTXBehavior;
                //qDebug()<<"find the frame"<<frame->m_id;
            }

            AR_FRAME_TRIG *frameTrig = new AR_FRAME_TRIG;
            frameTrig->shortName = frameTrigName;
            frameTrig->id = frameID.toInt();
            frameTrig->frameAddressMode = frameAddressMode;
            frameTrig->CANFDSupported = CANFDSupported;
            frameTrig->FrameRXBehavior = FrameRXBehavior;
            frameTrig->FrameTXBehavior = FrameTXBehavior;
            frameTrig->refFrame = frame;
            frameTrig->refPDUTrig = refPDUTrig;

            arFrameTrigList.append(frameTrig);

        }

        ar_CanCluster->ar_FrameTrigList = arFrameTrigList;
        ar_CanCluster->canClusterName = cluName;
        arCanClusterList.append(ar_CanCluster);
    }

    AR_XML_PACKAGES *arXML_Packages_new = new AR_XML_PACKAGES;
    arXML_Packages_new->ar_ECU_Instance = ECUInstanceName;
    arXML_Packages_new->ar_BaseTypeList = arBaseTypeList;
    arXML_Packages_new->ar_CompuMethodList = arCompuMethodList;
    arXML_Packages_new->ar_SysSignalList = arSysSignalList;
    arXML_Packages_new->ar_SignalList = arSignalList;
    arXML_Packages_new->ar_PDUList = arPDUList;
    arXML_Packages_new->ar_PDUContainerList = arPDUContainerList;
    arXML_Packages_new->ar_FrameList = arFrameList;
    arXML_Packages_new->ar_CANCommControllerList = arCanCommControllerList;
    arXML_Packages_new->ar_CANClusterList = arCanClusterList;


    this->arXmlPackages = arXML_Packages_new;
    qDebug()<<"===========End of Arxml Parsing==========";

    return true;
}

void ArxmlParser::arxmlPackagesToClusters(AR_XML_PACKAGES *arXmlPackages)
{
    qDebug()<<"{===========Start of Arxml to Clusters dbcs===========}";
    if(arXmlPackages == NULL)
    {
        return;
    }
    int clusterCount = arXmlPackages->ar_CANClusterList.count();
    canClusters.clear();

    for(int i = 0; i < clusterCount; i++)
    {
        AR_CAN_CLUSTER_SIM *arCanCluster = arXmlPackages->ar_CANClusterList.at(i);
        if (arCanCluster == NULL)
            continue;
        dbc_Cluster *dbcCluster = new dbc_Cluster();
        arCanClusterToCluster(arCanCluster, dbcCluster);

        canClusters.append(dbcCluster);        

        QString filePath = canClusterToDBC(dbcCluster);
        qDebug()<<"arxml cluster name:"<<dbcCluster->m_clusterName<<", gen dbc file path:"<<filePath;
        if(!filePath.isEmpty())
        {
            genDbcFileList.append(filePath);
            clusterDbcHash.insert(dbcCluster->m_clusterName, filePath);
        }
    }
    qDebug()<<"{===========End of Arxml to Clusters dbcs===========}";
}

void ArxmlParser::arCanClusterToCluster(AR_CAN_CLUSTER_SIM *arCanCluster, dbc_Cluster *dbcCluster)
{
    qDebug()<<"[===========Start of ArCluster to Cluster===========]";
    if(arCanCluster == NULL || dbcCluster == NULL)
    {
        return;
    }

    QString postfix = arCanCluster->postfix_FD;
    //qDebug()<<"Postfix for FD dbc signal:"<<postfix;


    AR_CAN_COMM_CONTROLLER *arCanCommController = arCanCluster->refCanCommController;

    dbcCluster->m_clusterName = arCanCluster->canClusterName;
    qDebug()<<"arCluster to Cluster name:"<<dbcCluster->m_clusterName;
    dbcCluster->m_can_baudrate = arCanCluster->Baudrate;
    qDebug()<<"arCluster to Cluster baudrate:"<<dbcCluster->m_can_baudrate;
    dbcCluster->m_canfd_baudrate = arCanCluster->CANFD_Baudrate;
    qDebug()<<"arCluster to Cluster canfd baudrate:"<<dbcCluster->m_canfd_baudrate;

    int canMode = (dbcCluster->m_canfd_baudrate == 0 ? 0 : 2);
    dbcCluster->m_can_io_mode = canMode;
    qDebug()<<"arCluster to Cluster can mode:"<<dbcCluster->m_can_io_mode;

    dbcCluster->postfix_signal = arCanCluster->postfix_FD;

    if(arCanCommController != NULL)
    {
        dbcCluster->m_can_samplePoint = arCanCommController->SamplePoint;
        dbcCluster->m_canfd_samplePoint = arCanCommController->FD_SamplePoint;
        qDebug()<<"arCluster to Cluster samplePoint:"<<dbcCluster->m_can_samplePoint;
        qDebug()<<"arCluster to Cluster fd samplePoint:"<<dbcCluster->m_canfd_samplePoint;
    }

    if (arCanCluster->ar_FrameTrigList.isEmpty())
        return;

    QList<dbc_frame*> dbcFrames;
    for (int i = 0; i < arCanCluster->ar_FrameTrigList.count(); i++)
    {
        dbc_frame *frame = new dbc_frame;

        AR_FRAME_TRIG *arFrameTrig = arCanCluster->ar_FrameTrigList.at(i);
        AR_FRAME *refFrame = arFrameTrig->refFrame;
        if(refFrame == NULL)
            continue;

        frame->m_name = refFrame->shortName;
        frame->m_dlc = refFrame->frameLen;
        frame->m_id = arFrameTrig->id;
        frame->m_extented = refFrame->isExtented;
        frame->m_timingType = 0;

        frame->m_cyclicTime = (refFrame->timing_s * 1000);

        frame->m_can_io_mode = canMode;

        quint32 startBitOffset = arFrameTrig->startBitOffset;
        QList<dbc_signal*> dbcSignals;

        if(arFrameTrig->refPDUTrig != NULL)
        {
            QString refPDUDest = arFrameTrig->refPDUTrig->refPDUDest;
            if (refPDUDest == "I-SIGNAL-I-PDU")
            {
                AR_PDU *arpdu = arFrameTrig->refPDUTrig->refPDU;
                AR_PDU_TRIG *arpduTrig = arFrameTrig->refPDUTrig;

                if(arpdu != NULL)
                {

                    if(frame->m_cyclicTime == 0)
                    {
                        frame->m_cyclicTime = arpdu->timing_s * 1000;
                    }

                    for (int z = 0; z < arpduTrig->refSigTrigList.count(); z++)
                    {
                        AR_SIGNAL *arSignal = arpduTrig->refSigTrigList.at(z)->refSig;
                        if(arSignal == NULL)
                            continue;
                        dbc_signal *signal = new dbc_signal;
                        signal->m_name = arSignal->shortName;

                        if(signal->m_name.left(2) == "is")
                        {
                            signal->m_name.remove(0, 2);
                        }
                        signal->m_name += (postfix);

                        signal->m_startBit = arSignal->startpos + startBitOffset;
                        signal->m_bitLen = arSignal->sigLen;
                        quint32 byteOrder;
                        if (arSignal->packingOrder == "MOST-SIGNIFICANT-BYTE-FIRST")
                            byteOrder = 1;
                        else if ((arSignal->packingOrder == "LAST-SIGNIFICANT-BYTE-FIRST"))
                            byteOrder = 0;
                        else
                            byteOrder = 1;

                        signal->m_byteOrder = byteOrder;
                        signal->m_valType = 1;
                        signal->m_value_default = arSignal->initValue;

                        AR_COMPU_METHOD *refCompuMethod = arSignal->refCompuMethod;

                        if(refCompuMethod != NULL)
                        {
                            signal->m_minimum = arSignal->refCompuMethod->lowerLimit;
                            signal->m_maximum = arSignal->refCompuMethod->upperLimit;
                            signal->m_factor = arSignal->refCompuMethod->p2;
                            signal->m_offset = arSignal->refCompuMethod->p1;

                            QList<ValTable*> sigValTableList;
                            if (arSignal->refCompuMethod->category == "TEXTTABLE" || arSignal->refCompuMethod->category == "SCALE_LINEAR_AND_TEXTTABLE")
                            {
                                int count = arSignal->refCompuMethod->scaleTextTableList.count();
                                for (int m = 0; m < count; m++)
                                {
                                    //qDebug()<<"refPDU 8";
                                    AR_SCALE_TEXTTABLE *textTable = arSignal->refCompuMethod->scaleTextTableList.at(m);

                                    ValTable *valTable = new ValTable;
                                    valTable->value = textTable->lowerLimit;
                                    valTable->enumText = textTable->compuConstant;
                                    sigValTableList.append(valTable);
                                    //qDebug()<<"refPDU 9";
                                }
                            }
                            signal->valTableList = sigValTableList;
                        }
                        else
                        {
                            signal->m_minimum = -10000;
                            signal->m_maximum = 10000;
                            signal->m_factor = 1;
                            signal->m_offset = 0;
                        }

                        dbcSignals.append(signal);

                    }
                    quint32 currentPDULen = arpdu->pduLen;
                    startBitOffset += currentPDULen*8;
                    //qDebug()<<"refPDU 10";
                }
            }
            if (arFrameTrig->refPDUTrig->refPDUDest == "CONTAINER-I-PDU")
            {
                QStringList refPDUTrigStrList = arFrameTrig->refPDUTrig->refPDUContainer->refPDUTrigStrList;
                QString headerType = arFrameTrig->refPDUTrig->refPDUContainer->headerType;

                if (frame->m_id == 0xB2)
                {
                    //startBitOffset += 12*8;
                }

                for(int j = 0; j < refPDUTrigStrList.count(); j++)
                {
                    QString refPDUTrigStr = refPDUTrigStrList.at(j);

                    AR_PDU_TRIG *pduTrig = findPDUTrigPtrByName(arCanCluster->ar_PDUTrigList, refPDUTrigStr);

                    AR_PDU *arpdu = pduTrig->refPDU;

                    //如果refFrame中的周期为0，使用refPDU中的周期值
                    if(frame->m_cyclicTime == 0 && arpdu != NULL)
                    {
                        frame->m_cyclicTime = arpdu->timing_s * 1000;
                    }
                    //qDebug()<<"ref PDU:"<<arpdu->shortName<<", Short-Header-ID:"<<arpdu->headerID;

                    if(frame->m_can_io_mode == 2 && frame->m_dlc > 8)
                    {
                        dbc_signal *signal_headerID = new dbc_signal;
                        signal_headerID->m_name = "headerID_"+arpdu->shortName;
                        signal_headerID->m_name += (postfix);

                        signal_headerID->m_startBit = 7 + startBitOffset;
                        signal_headerID->m_bitLen = 24;
                        signal_headerID->m_byteOrder = 1;
                        signal_headerID->m_factor = 1;
                        signal_headerID->m_offset = 0;
                        signal_headerID->m_valType = 1;
                        signal_headerID->m_value_default = arpdu->headerID;
                        dbcSignals.append(signal_headerID);

                        dbc_signal *signal_headerDLC = new dbc_signal;
                        signal_headerDLC->m_name = "headerDLC_"+arpdu->shortName;
                        signal_headerDLC->m_name += (postfix);

                        signal_headerDLC->m_startBit = signal_headerID->m_bitLen + 7 + startBitOffset;
                        signal_headerDLC->m_bitLen = 8;
                        signal_headerDLC->m_byteOrder = 1;
                        signal_headerDLC->m_factor = 1;
                        signal_headerDLC->m_offset = 0;
                        signal_headerDLC->m_valType = 1;
                        signal_headerDLC->m_value_default = 8;
                        dbcSignals.append(signal_headerDLC);
                        startBitOffset += 32;
                    }


                    for (int z = 0; z < arpdu->refSigList.count(); z++)
                    {
                        AR_SIGNAL *arSignal = arpdu->refSigList.at(z);

                        dbc_signal *signal = new dbc_signal;
                        signal->m_name = arSignal->shortName;

                        if(signal->m_name.left(2) == "is")
                        {
                            signal->m_name.remove(0, 2);
                        }

                        signal->m_name += (postfix);

                        signal->m_startBit = arSignal->startpos + startBitOffset;
                        signal->m_bitLen = arSignal->sigLen;
                        quint32 byteOrder;
                        if (arSignal->packingOrder == "MOST-SIGNIFICANT-BYTE-FIRST")
                            byteOrder = 1;
                        else if ((arSignal->packingOrder == "LAST-SIGNIFICANT-BYTE-FIRST"))
                            byteOrder = 0;
                        else
                            byteOrder = 1;

                        signal->m_byteOrder = byteOrder;
                        signal->m_minimum = arSignal->refCompuMethod->lowerLimit;
                        signal->m_maximum = arSignal->refCompuMethod->upperLimit;
                        signal->m_factor = arSignal->refCompuMethod->p2;
                        signal->m_offset = arSignal->refCompuMethod->p1;
                        signal->m_valType = 1;
                        signal->m_value_default = arSignal->initValue;

                        QList<ValTable*> sigValTableList;
                        //sigValTableList.clear();
                        if (arSignal->refCompuMethod->category == "TEXTTABLE" || arSignal->refCompuMethod->category == "SCALE_LINEAR_AND_TEXTTABLE")
                        {

                            int count = arSignal->refCompuMethod->scaleTextTableList.count();
                            for (int m = 0; m < count; m++ )
                            {
                                AR_SCALE_TEXTTABLE *textTable = arSignal->refCompuMethod->scaleTextTableList.at(m);

                                ValTable *valTable = new ValTable;
                                valTable->value = textTable->lowerLimit;
                                valTable->enumText = textTable->compuConstant;

                                sigValTableList.append(valTable);
                            }
                        }
                        signal->valTableList = sigValTableList;

                        dbcSignals.append(signal);

                    }
                    quint32 currentPDULen = arpdu->pduLen;
                    startBitOffset += currentPDULen*8;
                }
            }
        }

        frame->m_sigs = dbcSignals;
        dbcFrames.append(frame);
    }


    QList<dbc_frame*> dbcPDUs;

    dbcCluster->m_frames = dbcFrames;
    dbcCluster->m_pdus = dbcPDUs;

    qDebug()<<"[===========End of ArCluster to Cluster==========]";

}


AR_SIGNAL_TRIG *ArxmlParser::findSignalTrigPtrByName(QList<AR_SIGNAL_TRIG *> sigTrigList, QString name)
{
    if (sigTrigList.isEmpty())
        return NULL;
    AR_SIGNAL_TRIG *sigTrig;
    for(int i = 0; i < sigTrigList.count(); i++)
    {
        sigTrig = sigTrigList.at(i);
        if (name == sigTrig->shortName)
            return sigTrig;
    }
    return NULL;
}

AR_PDU_TRIG *ArxmlParser::findPDUTrigPtrByName(QList<AR_PDU_TRIG *> pduTrigList, QString name)
{
    if (pduTrigList.isEmpty())
        return NULL;
    AR_PDU_TRIG *pduTrig;
    for(int i = 0; i < pduTrigList.count(); i++)
    {
        pduTrig = pduTrigList.at(i);
        if (name == pduTrig->shortName)
            return pduTrig;
    }
    return NULL;
}

AR_PDU_CONTAINER *ArxmlParser::findPDUContainerPtrByName(QList<AR_PDU_CONTAINER*> pduContainerList, QString name)
{
    if (pduContainerList.isEmpty())
        return NULL;
    AR_PDU_CONTAINER *pduContainer;
    for(int i = 0; i < pduContainerList.count(); i++)
    {
        pduContainer = pduContainerList.at(i);
        if (name == pduContainer->shortName)
            return pduContainer;
    }
    return NULL;
}

AR_PDU *ArxmlParser::findPDUPtrByName(QList<AR_PDU *> pduList, QString name)
{
    if (pduList.isEmpty())
        return NULL;
    AR_PDU *pdu;
    for(int i = 0; i < pduList.count(); i++)
    {
        pdu = pduList.at(i);
        if (name == pdu->shortName)
            return pdu;
    }
    return NULL;
}

AR_FRAME *ArxmlParser::findFramePtrByName(QList<AR_FRAME*> frameList, QString name)
{
    if (frameList.isEmpty())
        return NULL;
    AR_FRAME *frame;
    for(int i = 0; i < frameList.count(); i++)
    {
        frame = frameList.at(i);
        if (name == frame->shortName)
            return frame;
    }
    return NULL;
}

AR_SIGNAL *ArxmlParser::findSignalPtrByName(QList<AR_SIGNAL *> signalList, QString name)
{
    if (signalList.isEmpty())
        return NULL;
    AR_SIGNAL *signal;
    for(int i = 0; i < signalList.count(); i++)
    {
        signal = signalList.at(i);
        if (name == signal->shortName)
            return signal;
    }
    return NULL;
}

AR_SYS_SIGNAL *ArxmlParser::findSysSignalPtrByName(QList<AR_SYS_SIGNAL *> sysSignalList, QString name)
{
    if (sysSignalList.isEmpty())
        return NULL;
    AR_SYS_SIGNAL *sysSignal;
    for(int i = 0; i < sysSignalList.count(); i++)
    {
        sysSignal = sysSignalList.at(i);
        if (name == sysSignal->shortName)
            return sysSignal;
    }
    return NULL;
}

AR_BASETYPE *ArxmlParser::findBaseTypePtrByName(QList<AR_BASETYPE *> baseTypeList, QString name)
{
    if (baseTypeList.isEmpty())
        return NULL;
    AR_BASETYPE *baseType;
    for(int i = 0; i < baseTypeList.count(); i++)
    {
        baseType = baseTypeList.at(i);
        if (name == baseType->shortName)
            return baseType;
    }
    return NULL;
}

AR_COMPU_METHOD *ArxmlParser::findCompuMethodPtrByName(QList<AR_COMPU_METHOD *> compuMethodList, QString name)
{
    if (compuMethodList.isEmpty())
        return NULL;
    AR_COMPU_METHOD *compuMethod;
    for(int i = 0; i < compuMethodList.count(); i++)
    {
        compuMethod = compuMethodList.at(i);
        if (name == compuMethod->shortName)
            return compuMethod;
    }
    return NULL;
}

AR_CAN_COMM_CONTROLLER *ArxmlParser::findCanCommControllerByName(QList<AR_CAN_COMM_CONTROLLER *> canCommControllerList, QString name)
{
    if (canCommControllerList.isEmpty())
        return NULL;
    AR_CAN_COMM_CONTROLLER *canCommController = NULL;
    for(int i = 0; i < canCommControllerList.count(); i++)
    {
        canCommController = canCommControllerList.at(i);
        if (name == canCommController->canCommControllerName)
            return canCommController;
    }
    return NULL;
}

AR_CAN_COMM_CONNECTOR *ArxmlParser::findCanCommConnectorByName(QList<AR_CAN_COMM_CONNECTOR *> canCommConnectorList, QString name)
{
    if (canCommConnectorList.isEmpty())
        return NULL;
    AR_CAN_COMM_CONNECTOR *canCommConnector;
    for(int i = 0; i < canCommConnectorList.count(); i++)
    {
        canCommConnector = canCommConnectorList.at(i);
        if (name == canCommConnector->canCommConnectorName)
            return canCommConnector;
    }
    return NULL;
}

AR_FRAME *ArxmlParser::findFrameByRefPDU(QList<AR_FRAME*> frameList, AR_PDU *pdu)
{
    if(frameList.isEmpty() || pdu == NULL)
        return NULL;
    foreach (AR_FRAME *frame, frameList) {
        if(frame->refPDUDest == "I-SIGNAL-I-PDU" && frame->refPDU == pdu)
        {
            return frame;
        }
    }
    return NULL;
}

QString ArxmlParser::textOfFirstElementByTagName(QDomElement elem, QString tagName)
{
    QDomNodeList nodeList = elem.elementsByTagName(tagName);
    QString elemText = "";
    if (!nodeList.isEmpty())
    {
        elemText = nodeList.at(0).toElement().text();
    }
    return elemText;

}

QString ArxmlParser::textOfFirstElementByTagNames(QDomElement elem, QString tagName1, QString tagName2)
{
    QDomNodeList nodeList1 = elem.elementsByTagName(tagName1);
    QString elemText ="";
    if(!nodeList1.isEmpty())
    {
        QDomElement elem1 = nodeList1.at(0).toElement();
        elemText = textOfFirstElementByTagName(elem1, tagName2);
    }
    return elemText;
}

QString ArxmlParser::textOfIndexElementByTagName(QDomElement elem, QString tagName, int index)
{
    QDomNodeList nodeList = elem.elementsByTagName(tagName);
    if (index >= nodeList.count())
        return "";
    QString elemText = "";
    if (!nodeList.isEmpty())
    {
        elemText = nodeList.at(index).toElement().text();
    }
    return elemText;
}

QStringList ArxmlParser::textListOfElementsByTagName(QDomElement elem, QString tagName_Multi)
{
    QDomNodeList nodeList = elem.elementsByTagName(tagName_Multi);
    QStringList elemTextList;
    if (!nodeList.isEmpty())
    {
        for (int i = 0; i < nodeList.count(); i++)
        {
            QString elemText = nodeList.at(i).toElement().text();
            elemTextList << elemText;
        }

    }
    return elemTextList;
}

QStringList ArxmlParser::textListOfElementsByTagNames(QDomElement elem, QString tagName_Single, QString tagName_Multi)
{
    QDomNodeList nodeList1 = elem.elementsByTagName(tagName_Single);
    QStringList elemTextList;
    if(!nodeList1.isEmpty())
    {
        QDomElement elem1 = nodeList1.at(0).toElement();
        elemTextList = textListOfElementsByTagName(elem1, tagName_Multi);
    }
    return elemTextList;
}

void ArxmlParser::writeTextLine(QString textLine)
{
    if (!out.device()->isWritable())
        return;

    out << textLine << "\n";
}

void ArxmlParser::writeText_dbc_Prefix()
{
    writeTextLine("VERSION " "");

    writeTextLine("");
    writeTextLine("");

    writeTextLine("NS_ :");
    writeTextLine("  NS_DESC_");
    writeTextLine("  CM_");
    writeTextLine("  BA_DEF_");
    writeTextLine("  BA_");
    writeTextLine("  VAL_");
    writeTextLine("  CAT_DEF_");
    writeTextLine("  CAT_");
    writeTextLine("  FILTER");
    writeTextLine("  BA_DEF_DEF_");
    writeTextLine("  EV_DATA_");
    writeTextLine("  ENVVAR_DATA_");
    writeTextLine("  SGTYPE_");
    writeTextLine("  SGTYPE_VAL_");
    writeTextLine("  BA_DEF_SGTYPE_");
    writeTextLine("  BA_SGTYPE_");
    writeTextLine("  SIG_TYPE_REF_");
    writeTextLine("  VAL_TABLE_");
    writeTextLine("  SIG_GROUP_");
    writeTextLine("  SIG_VALTYPE_");
    writeTextLine("  SIGTYPE_VALTYPE_");
    writeTextLine("  BO_TX_BU_");
    writeTextLine("  BA_DEF_REL_");
    writeTextLine("  BA_REL_");
    writeTextLine("  BA_DEF_DEF_REL_");
    writeTextLine("  BU_SG_REL_");
    writeTextLine("  BU_EV_REL_");
    writeTextLine("  BU_BO_REL_");
    writeTextLine("  SG_MUL_VAL_");

    writeTextLine("");

    writeTextLine("BS_ :");

    writeTextLine("");

    NetWorkNode = "TC ISC HCU VCU BMS CCU ECM SCU CCP IPD ICC BCM SAS "
                  "IBS AC SAC EOPC ESSPTC EPMCU WLC SCS SDM IMCU Bench Tester";

    writeTextLine("BU_ : " + NetWorkNode);

    writeTextLine("");
}

void ArxmlParser::writeText_dbc_FrameAndSignal(dbc_Cluster *dbcCluster)
{
    int frameCount = dbcCluster->m_frames.count();
    for (int i = 0; i < frameCount; i++)
    {
        dbc_frame *frame = dbcCluster->m_frames.at(i);

        writeTextLine("");
        writeTextLine(textLineFrame_BO_(frame));

        if (frame->m_sigs.isEmpty())
            continue;
        int sigCount = frame->m_sigs.count();
        for (int j = 0; j < sigCount; j++)
        {
            dbc_signal *signal = frame->m_sigs.at(j);

            writeTextLine(textLineSignal_SG_(signal));
        }
    }

    writeTextLine("");
}

void ArxmlParser::writeText_dbc_Attrs(dbc_Cluster *dbcCluster)
{
    QStringList strList;
    strList.clear();

    writeTextLine(attrTextLine_BA_DEF_("", "DBName", "STRING", strList));

    writeTextLine(attrTextLine_BA_DEF_("", "BusType", "STRING", strList));

    strList.clear();
    strList << "0" << "1";
    writeTextLine(attrTextLine_BA_DEF_("BO_", "CANFD_BRS", "ENUM", strList));




    strList.clear();
    strList << "StandardCAN"<<"ExtendedCAN"<<"reserved" <<"J1939PG"
            << "reserved" << "reserved" << "reserved" << "reserved" << "reserved"
            << "reserved" << "reserved" << "reserved" << "reserved" << "reserved"
            << "StandardCAN_FD" << "ExtendedCAN_FD";
    writeTextLine(attrTextLine_BA_DEF_("BO_", "VFrameFormat", "ENUM", strList));

    strList.clear();
    strList << "Cyclic"<<"Event"<<"CyclicIfActive" <<"SpontanWithDelay"
            << "CyclicAndSpontan" << "CyclicAndSpontanWithDelay";
    writeTextLine(attrTextLine_BA_DEF_("BO_", "GenMsgSendType", "ENUM", strList));

    strList.clear();
    strList << "0"<<"0";
    writeTextLine(attrTextLine_BA_DEF_("BO_", "GenMsgCycleTime", "INT", strList));

    strList.clear();
    strList << "0"<<"0";
    writeTextLine(attrTextLine_BA_DEF_("BO_", "GenMsgDelayTime", "INT", strList));

    strList.clear();
    strList << "0"<<"0";
    writeTextLine(attrTextLine_BA_DEF_("SG_", "GenSigStartValue", "FLOAT", strList));

    writeTextLine(attrTextLine_BA_DEF_DEF_("DBName", addQuotes(dbcCluster->m_clusterName)));
    if(dbcCluster->m_can_io_mode == 0)
    {
        writeTextLine(attrTextLine_BA_DEF_DEF_("CANFD_BRS", addQuotes("0")));
        writeTextLine(attrTextLine_BA_DEF_DEF_("VFrameFormat", addQuotes("StandardCAN")));
        writeTextLine(attrTextLine_BA_DEF_DEF_("BusType", addQuotes("CAN")));
    }
    else if(dbcCluster->m_can_io_mode == 2)
    {
        writeTextLine(attrTextLine_BA_DEF_DEF_("CANFD_BRS", addQuotes("1")));
        writeTextLine(attrTextLine_BA_DEF_DEF_("VFrameFormat", addQuotes("StandardCAN")));
        writeTextLine(attrTextLine_BA_DEF_DEF_("BusType", addQuotes("CAN FD")));
    }
    //writeTextLine(attrTextLine_BA_DEF_DEF_("BusType", addQuotes("CAN FD")));
    //writeTextLine(attrTextLine_BA_DEF_DEF_("CANFD_BRS", addQuotes("1")));
    //writeTextLine(attrTextLine_BA_DEF_DEF_("VFrameFormat", addQuotes("StandardCAN")));
    writeTextLine(attrTextLine_BA_DEF_DEF_("GenMsgSendType", addQuotes("Cyclic")));
    writeTextLine(attrTextLine_BA_DEF_DEF_("GenMsgCycleTime", "0"));
    writeTextLine(attrTextLine_BA_DEF_DEF_("GenMsgDelayTime", "0"));
    writeTextLine(attrTextLine_BA_DEF_DEF_("GenSigStartValue", "0"));
}

void ArxmlParser::writeText_dbc_Frame_Attrs(dbc_Cluster *dbcCluster)
{
    int frameCount = dbcCluster->m_frames.count();
    for (int i = 0; i < frameCount; i++)
    {
        dbc_frame *frame = dbcCluster->m_frames.at(i);

        QStringList infoList;
        infoList << QString::number(frame->m_id);

        if (frame->m_can_io_mode == 0)
        {
            writeTextLine(attrTextLine_BA_("VFrameFormat", "BO_", infoList, "0"));
        }
        else
        {
            writeTextLine(attrTextLine_BA_("VFrameFormat", "BO_", infoList, "14"));
        }

        int msgSendType = frame->m_timingType;
        writeTextLine(attrTextLine_BA_("GenMsgSendType", "BO_", infoList, QString::number(msgSendType)));

        int cyclicTime = frame->m_cyclicTime;
        writeTextLine(attrTextLine_BA_("GenMsgCycleTime", "BO_", infoList, QString::number(cyclicTime)));
    }
}

void ArxmlParser::writeText_dbc_Signal_Attrs(dbc_Cluster *dbcCluster)
{
    int frameCount = dbcCluster->m_frames.count();
    for (int i = 0; i < frameCount; i++)
    {
        dbc_frame *frame = dbcCluster->m_frames.at(i);

        if (frame->m_sigs.isEmpty())
            continue;
        int sigCount = frame->m_sigs.count();
        for (int j = 0; j < sigCount; j++)
        {
            dbc_signal *signal = frame->m_sigs.at(j);

            QStringList infoList;
            infoList << QString::number(frame->m_id)<<signal->m_name;

            qreal defaultValue = signal->m_value_default;
            writeTextLine(attrTextLine_BA_("GenSigStartValue", "SG_", infoList, QString::number(defaultValue, 'g', 6)));

        }
    }
}

void ArxmlParser::writeText_dbc_Signal_ValTables(dbc_Cluster *dbcCluster)
{
    int frameCount = dbcCluster->m_frames.count();
    for (int i = 0; i < frameCount; i++)
    {
        dbc_frame *frame = dbcCluster->m_frames.at(i);

        if (frame->m_sigs.isEmpty())
            continue;
        int sigCount = frame->m_sigs.count();
        for (int j = 0; j < sigCount; j++)
        {
            dbc_signal *signal = frame->m_sigs.at(j);

            QStringList infoList;
            infoList << QString::number(frame->m_id)<<signal->m_name;

            if (signal->valTableList.isEmpty())
                continue;

            writeTextLine(valTextLine_VAL_(infoList, signal->valTableList));
        }
    }
}


QString ArxmlParser::addQuotes(QString text)
{
    return ("\"" + text + "\"");
}

QString ArxmlParser::textLineFrame_BO_(dbc_frame *frame)
{
    if (!frame)
        return "";

    QString txNodeName = "Tester";

    QStringList nodes = QString(NetWorkNode).split(" ");

    QString head = frame->m_name.left(frame->m_name.indexOf("_"));
    if(nodes.indexOf(head) != -1)
    {
        txNodeName = head;
    }

    QString frameText = "";
    frameText = "BO_ "+ QString::number(frame->m_id) + " "+ frame->m_name + ": " + QString::number(frame->m_dlc) + " " + txNodeName;
    return frameText;
}

QString ArxmlParser::textLineSignal_SG_(dbc_signal *signal)
{
    if (!signal)
        return "";
    QString valTypeStr = "";
    if (signal->m_valType == 0)
        valTypeStr = "-";
    else if (signal->m_valType == 1)
        valTypeStr = "+";

    QString signalText = "  SG_ " + signal->m_name + " : " + QString::number(signal->m_startBit) + "|" + QString::number(signal->m_bitLen)
                        + "@" + QString::number((signal->m_byteOrder == 1) ? 0 : 1) + valTypeStr
                        + " (" + QString::number(signal->m_factor, 'g', 6) + "," + QString::number(signal->m_offset, 'g', 6) + ")"
                        + " [" + QString::number(signal->m_minimum, 'g', 6) + "|" + QString::number(signal->m_maximum, 'g', 6) + "] "
                        + "\" \" " + "Tester";

    return signalText;
}

QString ArxmlParser::attrTextLine_BA_DEF_(QString Category, QString attrName, QString type, QStringList valueList)
{
    QString valueStr = "";
    for (int i = 0; i < valueList.count(); i++)
    {
        if (type == "ENUM")
        {
            valueStr += (addQuotes(valueList.at(i)));
            if (i < (valueList.count()-1))
                valueStr += ",";
        }
        else
        {
            valueStr += (valueList.at(i) + " ");
        }
    }

    QString Line_BA_DEF_ = "BA_DEF_ " + Category + " " + addQuotes(attrName) + " " + type + " " + valueStr + ";";

    return Line_BA_DEF_;
}

QString ArxmlParser::attrTextLine_BA_DEF_DEF_(QString attrName, QString initValue)
{
    QString Line_BA_DEF_DEF_ = "BA_DEF_DEF_ " + addQuotes(attrName) + " " + initValue + ";";

    return Line_BA_DEF_DEF_;
}

QString ArxmlParser::attrTextLine_BA_(QString attrName, QString Category, QStringList infoList, QString initValue)
{
    QString infoStr = "";
    for (int i = 0; i < infoList.count(); i++)
    {
        infoStr += (infoList.at(i) + " ");
    }

    QString Line_BA_ = "BA_ " + addQuotes(attrName) + " " + Category + " " + infoStr + " " + initValue + ";";

    return Line_BA_;
}

QString ArxmlParser::valTextLine_VAL_(QStringList infoList, QList<ValTable *> valTableList)
{
    QString infoStr = "";
    for (int i = 0; i < infoList.count(); i++)
    {
        infoStr += (infoList.at(i) + " ");
    }

    QString valTableStr = "";
    for (int i = 0; i < valTableList.count(); i++)
    {
        ValTable* valPair = valTableList.at(i);
        valTableStr += (QString::number(valPair->value) + " " + addQuotes(valPair->enumText) + " ");
    }
    QString Line_VAL_ = "VAL_ " + infoStr + " " + valTableStr + ";";

    return Line_VAL_;
}

QHash<QString, QString> ArxmlParser::getClusterDbcHash() const
{
    return clusterDbcHash;
}

