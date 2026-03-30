#include "xml_gen.h"
#include <QDebug>
#include <QApplication>

XML_GEN::XML_GEN(QObject *parent) : QObject(parent)
{

}

bool XML_GEN::genXML()
{
    QString xmlPath = xmlFilePath;
    QString xmlPath_ref = "";

    qDebug()<<"WriteXmlPath:"<<xmlPath;
    qDebug()<<"RefXmlPath:"<<xmlPath_ref;

    QFileInfo fileInfo(xmlPath);
    if(fileInfo.isFile())
    {
        QFile file(xmlPath);
        file.remove();
    }

    if (xmlPath.isEmpty())
        return false;

    QFile xmlFile(xmlPath);
    if (!xmlFile.open(QFile::WriteOnly | QFile::Text))
    {

        qDebug()<<tr("Cannot write file %1:\n%2.").arg(xmlPath).arg(xmlFile.errorString());
        return false;
    }

    QDomDocument xml("CAN_TB");

    QDomElement root = xml.createElement("Root");
    root.setAttribute("arxmlFile", sel_arxmlPath);
    root.setAttribute("refArxmlFile", sel_arxmlPath_ref);
    root.setAttribute("arCluster", sel_arCluster);
    root.setAttribute("dbcFile", sel_dbcPath);
    root.setAttribute("refDbcFile", sel_dbcPath_ref);

    sel_appDir = QCoreApplication::applicationDirPath();
    root.setAttribute("appDir", sel_appDir);
    qDebug()<<"[DBC PATH]appDir saved:"<<sel_appDir;

    root.setAttribute("isArxml", isArxml ? 1 : 0);
    root.setAttribute("cluster", sel_cluster);
    root.setAttribute("interface", sel_interface);
    root.setAttribute("Baudrate", sel_baudrate);
    root.setAttribute("CANIO_Mode", sel_canio_mode);
    root.setAttribute("CANFD_Baudrate", sel_canfd_baudrate);
    root.setAttribute("CAN_Termination", canTermination);
    root.setAttribute("CustomBD", customBD);
    root.setAttribute("CustomFDBD", customFDBD);
    root.setAttribute("SamplePoint_BD", samplePoint_BD);
    root.setAttribute("SamplePoint_FDBD", samplePoint_FDBD);
    root.setAttribute("Sel_Frames_r", sel_frames_r);
    root.setAttribute("Sel_Frames_w", sel_frames_w);
    root.setAttribute("Sel_Sigs_r", sel_sigs_r);
    root.setAttribute("Sel_Sigs_w", sel_sigs_w);

    xml.appendChild(root);

    for(int i=0; i < dbc_frames_r.count(); i++)
    {
        //qDebug()<<"frame Read index = "<<i;
        dbc_frame *frame = dbc_frames_r[i];
        u8 dlc = frame->m_dlc;
        u32 sizeFrame = nxFrameSize(dlc);
        QDomElement ReadElem = xml.createElement(frame->m_name + "_" + sel_interface);
        root.appendChild(ReadElem);
        ReadElem.setAttribute("Size_Bytes", sizeFrame);
        ReadElem.setAttribute("isRead", 1);
        ReadElem.setAttribute("ID", "0x"+QString::number(frame->m_id, 16));
        ReadElem.setAttribute("isExtented", (frame->m_extented==false)?0:1);
        ReadElem.setAttribute("DLC", frame->m_dlc);
        ReadElem.setAttribute("TimingType", frame->m_timingType);
        ReadElem.setAttribute("Sample_Rate", frame->m_cyclicTime);
        ReadElem.setAttribute("MSG", INDEX_READ_SM_CAN+sel_interface.at(3).digitValue());

        QDomElement timeElem = xml.createElement("Param_"+QString::number(0));
        ReadElem.appendChild(timeElem);
        timeElem.setAttribute("Unit", "s");
        timeElem.setAttribute("DefaultValue", "0");
        timeElem.setAttribute("Minimum", "0");
        timeElem.setAttribute("Maximum", "0");
        timeElem.setAttribute("ScalingOffset", "0");
        timeElem.setAttribute("ScalingFactor", "1");
        timeElem.setAttribute("LengthBit", "64");
        timeElem.setAttribute("StartBit", "0");
        timeElem.setAttribute("StartBit_Oth", "0");
        timeElem.setAttribute("ByteOrder", "0");
        timeElem.setAttribute("Type", "3");
        timeElem.setAttribute("Param_Name", "Time");

        for (int j=1; j < frame->m_sigs.count()+1; j++)
        {
            //qDebug()<<"signal read inde = "<<j;
            dbc_signal *sig = frame->m_sigs[j-1];
            QDomElement Elem = xml.createElement("Param_"+QString::number(j));
            ReadElem.appendChild(Elem);

            Elem.setAttribute("Unit", sig->m_unit);
            Elem.setAttribute("DefaultValue", QString::number(sig->m_value_default));
            Elem.setAttribute("Minimum", QString::number(sig->m_minimum));
            Elem.setAttribute("Maximum", QString::number(sig->m_maximum));
            Elem.setAttribute("ScalingOffset", QString::number(sig->m_offset));
            Elem.setAttribute("ScalingFactor", QString::number(sig->m_factor));
            Elem.setAttribute("LengthBit", QString::number(sig->m_bitLen, 10));
            Elem.setAttribute("StartBit", QString::number(sig->m_startBit + 64, 10));
            Elem.setAttribute("StartBit_Oth", QString::number(sig->m_startBit_oth + 64, 10));
            Elem.setAttribute("ByteOrder", QString::number(sig->m_byteOrder,10));
            Elem.setAttribute("Type", QString::number(sig->m_valType, 10));
            Elem.setAttribute("Param_Name", sig->m_name);


        }
    }

    int sigIndex_r = 0;
    QDomElement ReadSigElem = xml.createElement("RP_" + sel_interface);
    root.appendChild(ReadSigElem);

    // add time channel
    QDomElement timeElem_r = xml.createElement("Param_"+QString::number(sigIndex_r));
    ReadSigElem.appendChild(timeElem_r);
    timeElem_r.setAttribute("Unit", "s");
    timeElem_r.setAttribute("DefaultValue", QString::number(0,10));
    timeElem_r.setAttribute("Minimum", QString::number(0, 10));
    timeElem_r.setAttribute("Maximum", QString::number(0, 10));
    timeElem_r.setAttribute("ScalingOffset", QString::number(0));
    timeElem_r.setAttribute("ScalingFactor", QString::number(1));
    timeElem_r.setAttribute("LengthBit", QString::number(64, 10));
    timeElem_r.setAttribute("StartBit", QString::number(sigIndex_r*64, 10));
    timeElem_r.setAttribute("StartBit_Oth", QString::number(0, 10));
    timeElem_r.setAttribute("ByteOrder", QString::number(0,10));
    timeElem_r.setAttribute("Type", QString::number(3, 10));
    timeElem_r.setAttribute("Param_Name", "Time");

    sigIndex_r++;


    for(int i=0; i < dbc_frames_r.count(); i++)
    {
        dbc_frame *frame = dbc_frames_r[i];
        for (int j=0; j < frame->m_sigs.count(); j++)
        {
            dbc_signal *sig = frame->m_sigs[j];
            QDomElement sigElem = xml.createElement("Param_"+QString::number(sigIndex_r));
            ReadSigElem.appendChild(sigElem);
            sigElem.setAttribute("Unit", sig->m_unit);
            sigElem.setAttribute("DefaultValue", QString::number(sig->m_value_default));
            sigElem.setAttribute("Minimum", QString::number(sig->m_minimum));
            sigElem.setAttribute("Maximum", QString::number(sig->m_maximum));
            sigElem.setAttribute("ScalingOffset", QString::number(0));
            sigElem.setAttribute("ScalingFactor", QString::number(1));
            sigElem.setAttribute("LengthBit", QString::number(64, 10));
            sigElem.setAttribute("StartBit", QString::number(sigIndex_r*64, 10));
            sigElem.setAttribute("StartBit_Oth", QString::number(0, 10));
            sigElem.setAttribute("ByteOrder", QString::number(0,10));
            sigElem.setAttribute("Type", QString::number(3, 10));
            sigElem.setAttribute("Param_Name", sig->m_name);


            sigIndex_r++;
        }
    }
    ReadSigElem.setAttribute("Size_Bytes", sigIndex_r*sizeof(f64));
    ReadSigElem.setAttribute("Sample_Rate", QString::number(10,10));
    ReadSigElem.setAttribute("isRead", QString::number(1,10));


    for(int i=0; i < dbc_frames_w.count(); i++)
    {
        dbc_frame *frame = dbc_frames_w[i];
        u8 dlc = frame->m_dlc;
        u32 sizeFrame = nxFrameSize(dlc);
        QDomElement WriteElem = xml.createElement(frame->m_name + "_" + sel_interface);
        root.appendChild(WriteElem);
        WriteElem.setAttribute("Size_Bytes", sizeFrame);
        WriteElem.setAttribute("isRead", 0);
        WriteElem.setAttribute("ID", "0x"+QString::number(frame->m_id, 16));
        WriteElem.setAttribute("isExtented", (frame->m_extented==false)?0:1);
        WriteElem.setAttribute("DLC", frame->m_dlc);
        WriteElem.setAttribute("TimingType", frame->m_timingType);
        WriteElem.setAttribute("Sample_Rate", frame->m_cyclicTime);
        WriteElem.setAttribute("MSG", INDEX_WRITE_SM_CAN+sel_interface.at(3).digitValue());        


        QDomElement timeElem = xml.createElement("Param_"+QString::number(0));
        WriteElem.appendChild(timeElem);
        timeElem.setAttribute("Unit", "s");
        timeElem.setAttribute("DefaultValue", "0");
        timeElem.setAttribute("Minimum", "0");
        timeElem.setAttribute("Maximum", "0");
        timeElem.setAttribute("ScalingOffset", "0");
        timeElem.setAttribute("ScalingFactor", "1");
        timeElem.setAttribute("LengthBit", "64");
        timeElem.setAttribute("StartBit", "0");
        timeElem.setAttribute("StartBit_Oth", "0");
        timeElem.setAttribute("ByteOrder", "0");
        timeElem.setAttribute("Type", "3");
        timeElem.setAttribute("Param_Name", "Time");

        for (int j=1; j < frame->m_sigs.count()+1; j++)
        {
            dbc_signal *sig = frame->m_sigs[j-1];
            //qDebug()<<"sig in gen xml name:"<<sig->m_name<<",startbit:"<<sig->m_startBit;
            QDomElement Elem = xml.createElement("Param_"+QString::number(j));
            WriteElem.appendChild(Elem);

            Elem.setAttribute("Unit", sig->m_unit);
            Elem.setAttribute("DefaultValue", QString::number(sig->m_value_default));
            Elem.setAttribute("Minimum", QString::number(sig->m_minimum));
            Elem.setAttribute("Maximum", QString::number(sig->m_maximum));
            Elem.setAttribute("ScalingOffset", QString::number(sig->m_offset));
            Elem.setAttribute("ScalingFactor", QString::number(sig->m_factor));
            Elem.setAttribute("LengthBit", QString::number(sig->m_bitLen, 10));
            Elem.setAttribute("StartBit", QString::number(sig->m_startBit + 64, 10));
            Elem.setAttribute("StartBit_Oth", QString::number(sig->m_startBit_oth + 64, 10));
            Elem.setAttribute("ByteOrder", QString::number(sig->m_byteOrder,10));
            Elem.setAttribute("Type", QString::number(sig->m_valType, 10));
            Elem.setAttribute("Param_Name", sig->m_name);
            Elem.setAttribute("SetValue", QString::number(sig->m_value));
            Elem.setAttribute("AssignMode", QString::number(sig->assignMode));

            if(sig->assignMode == CRC)
            {
                Elem.setAttribute("sigPam0", sig->checkSumInfos.at(0));
                Elem.setAttribute("sigPam1", sig->checkSumInfos.at(1));
                Elem.setAttribute("sigPam2", sig->checkSumInfos.at(2));
                Elem.setAttribute("sigPam3", sig->checkSumInfos.at(3));
            }
            else
            {
                Elem.setAttribute("sigPam0", sig->sigAssignModePams.at(0));
                Elem.setAttribute("sigPam1", sig->sigAssignModePams.at(1));
                Elem.setAttribute("sigPam2", sig->sigAssignModePams.at(2));
                Elem.setAttribute("sigPam3", sig->sigAssignModePams.at(3));
            }


        }
    }

    int sigIndex_w = 0;
    QDomElement WriteSigElem = xml.createElement("WP_" + sel_interface);
    root.appendChild(WriteSigElem);

    QDomElement timeElem_w = xml.createElement("Param_"+QString::number(sigIndex_w));
    WriteSigElem.appendChild(timeElem_w);
    timeElem_w.setAttribute("Unit", "s");
    timeElem_w.setAttribute("DefaultValue", QString::number(0, 10));
    timeElem_w.setAttribute("Minimum", QString::number(0, 10));
    timeElem_w.setAttribute("Maximum", QString::number(0, 10));
    timeElem_w.setAttribute("ScalingOffset", QString::number(0));
    timeElem_w.setAttribute("ScalingFactor", QString::number(1));
    timeElem_w.setAttribute("LengthBit", QString::number(64, 10));
    timeElem_w.setAttribute("StartBit", QString::number(sigIndex_w*64, 10));
    timeElem_w.setAttribute("StartBit_Oth", QString::number(0, 10));
    timeElem_w.setAttribute("ByteOrder", QString::number(0,10));
    timeElem_w.setAttribute("Type", QString::number(3, 10));
    timeElem_w.setAttribute("Param_Name", "Time");

    sigIndex_w++;


    for(int i=0; i < dbc_frames_w.count(); i++)
    {
        dbc_frame *frame = dbc_frames_w[i];
        for (int j=0; j < frame->m_sigs.count(); j++)
        {
            dbc_signal *sig = frame->m_sigs[j];
            //qDebug()<<"sig in gen xml name:"<<sig->m_name<<",startbit:"<<sig->m_startBit;
            QDomElement sigElem = xml.createElement("Param_"+QString::number(sigIndex_w));
            WriteSigElem.appendChild(sigElem);
            sigElem.setAttribute("Unit", sig->m_unit);
            sigElem.setAttribute("DefaultValue", QString::number(sig->m_value_default));
            sigElem.setAttribute("Minimum", QString::number(sig->m_minimum));
            sigElem.setAttribute("Maximum", QString::number(sig->m_maximum));
            sigElem.setAttribute("ScalingOffset", QString::number(0));
            sigElem.setAttribute("ScalingFactor", QString::number(1));
            sigElem.setAttribute("LengthBit", QString::number(64, 10));
            sigElem.setAttribute("StartBit", QString::number(sigIndex_w*64, 10));
            sigElem.setAttribute("StartBit_Oth", QString::number(0, 10));
            sigElem.setAttribute("ByteOrder", QString::number(0,10));
            sigElem.setAttribute("Type", QString::number(3, 10));
            sigElem.setAttribute("Param_Name", sig->m_name);
            sigElem.setAttribute("SetValue", QString::number(sig->m_value));
            sigElem.setAttribute("AssignMode", sig->assignMode);

            if(sig->assignMode == CRC)
            {
                sigElem.setAttribute("sigPam0", sig->checkSumInfos.at(0));
                sigElem.setAttribute("sigPam1", sig->checkSumInfos.at(1));
                sigElem.setAttribute("sigPam2", sig->checkSumInfos.at(2));
                sigElem.setAttribute("sigPam3", sig->checkSumInfos.at(3));
            }
            else
            {
                sigElem.setAttribute("sigPam0", sig->sigAssignModePams.at(0));
                sigElem.setAttribute("sigPam1", sig->sigAssignModePams.at(1));
                sigElem.setAttribute("sigPam2", sig->sigAssignModePams.at(2));
                sigElem.setAttribute("sigPam3", sig->sigAssignModePams.at(3));
            }

            sigIndex_w++;
        }
    }
    WriteSigElem.setAttribute("Size_Bytes", sigIndex_w*sizeof(f64));
    WriteSigElem.setAttribute("Sample_Rate", QString::number(10,10));
    WriteSigElem.setAttribute("isRead", QString::number(0,10));

    QString xmlString = xml.toString();
    QTextStream out(&xmlFile);
    out << xmlString;

    xmlFile.close();

    return true;
}

bool XML_GEN::readXML()
{

    QString xmlPath = xmlFilePath;
    qDebug()<<"ReadXmlPath:"<<xmlPath;

    if (xmlPath.isEmpty())
        return false;

    QFile xmlFile(xmlPath);
    if (!xmlFile.open(QFile::ReadOnly | QFile::Text))
    {
        qDebug()<<tr("Cannot read file %1:\n%2.").arg(xmlPath).arg(xmlFile.errorString());
        return false;
    }

    QDomDocument xml("CAN_TB");

    if (!xml.setContent(&xmlFile))
    {
        xmlFile.close();
        return false;
    }
    xmlFile.close();

    QDomElement root = xml.documentElement();

    sel_arxmlPath = root.attribute("arxmlFile");
    sel_arxmlPath_ref = root.attribute("refArxmlFile");
    sel_arCluster = root.attribute("arCluster");
    sel_dbcPath = root.attribute("dbcFile");
    sel_dbcPath_ref = root.attribute("refDbcFile");
    sel_appDir = root.attribute("appDir");
    QString appDir = QCoreApplication::applicationDirPath();
    if(sel_appDir != appDir)
    {
        qDebug()<<"[DBC PATH]Origin app dir:"<<sel_appDir;
        qDebug()<<"[DBC PATH]New app dir:"<<appDir;
        if(sel_appDir == "")
        {
            sel_dbcPath = (sel_dbcPath_ref == "") ? sel_dbcPath : appDir + sel_dbcPath_ref;
        }
        else
        {
            sel_dbcPath.replace(sel_appDir, appDir);
        }

        qDebug()<<"[DBC PATH]New dbc path:"<<sel_dbcPath;
    }
    int flagValue = root.attribute("isArxml").toInt();
    isArxml = flagValue ? true : false;
    sel_cluster = root.attribute("cluster");
    sel_interface = root.attribute("interface");
    sel_baudrate = root.attribute("Baudrate").toInt();
    sel_canio_mode = root.attribute("CANIO_Mode").toInt();
    sel_canfd_baudrate = root.attribute("CANFD_Baudrate").toLongLong();
    canTermination = root.attribute("CAN_Termination").toInt();    
    customBD = root.attribute("CustomBD").toInt();
    customFDBD = root.attribute("CustomFDBD").toInt();
    samplePoint_BD = root.attribute("SamplePoint_BD").toDouble();
    samplePoint_FDBD = root.attribute("SamplePoint_FDBD").toDouble();

    sel_frames_r = root.attribute("Sel_Frames_r");
    sel_frames_w = root.attribute("Sel_Frames_w");
    sel_sigs_r = root.attribute("Sel_Sigs_r");
    sel_sigs_w = root.attribute("Sel_Sigs_w");

    dbc_frames_r.clear();
    dbc_frames_w.clear();

    qDebug()<<"load from xml arxml path="<<sel_arxmlPath;
    qDebug()<<"load from xml dbc path="<<sel_dbcPath;
    qDebug()<<"load from xml interface="<<sel_interface;

    QDomElement msgElem = root.firstChildElement();
    while (!msgElem.isNull())
    {
        dbc_frame *frame = new dbc_frame;

        int index = msgElem.tagName().lastIndexOf('_');
        int len = msgElem.tagName().length();
        frame->m_name = msgElem.tagName().remove(index, len-index);
        if(msgElem.attribute("isExtented").toInt())
            frame->m_extented = true;
        else
            frame->m_extented = false;

        frame->m_id = msgElem.attribute("ID").toInt(0,16);
        frame->m_dlc = msgElem.attribute("DLC").toInt();
        frame->m_timingType = msgElem.attribute("TimingType").toInt();
        frame->m_cyclicTime = msgElem.attribute("Sample_Rate").toInt();

        qDebug()<<"ID="<<frame->m_id<<"Frame Name ="<<frame->m_name;

        QDomElement sigElem = msgElem.firstChildElement();
        while (!sigElem.isNull())
        {
            dbc_signal *signal = new dbc_signal;
            signal->m_name = sigElem.attribute("Param_Name");
            signal->m_startBit = sigElem.attribute("StartBit").toInt();
            signal->m_startBit_oth = sigElem.attribute("StartBit_Oth").toInt();
            signal->m_bitLen = sigElem.attribute("LengthBit").toInt();
            signal->m_valType = sigElem.attribute("Type").toInt();
            signal->m_factor = sigElem.attribute("ScalingFactor").toDouble();
            signal->m_offset = sigElem.attribute("ScalingOffset").toDouble();
            signal->m_maximum = sigElem.attribute("Maximum").toDouble();
            if(signal->m_maximum == 0)
            {
                signal->m_maximum = qPow(2, signal->m_bitLen) - 1;
            }
            signal->m_minimum = sigElem.attribute("Minimum").toDouble();
            signal->m_unit = sigElem.attribute("Unit");
            signal->m_byteOrder = sigElem.attribute("ByteOrder").toInt();
            signal->m_value_default  = sigElem.attribute("DefaultValue").toDouble();

            signal->m_value = sigElem.attribute("SetValue").toDouble();

            signal->assignMode = Sig_Val_Assign_Mode(sigElem.attribute("AssignMode").toInt());

            QStringList sigPams;
            sigPams << sigElem.attribute("sigPam0");
            sigPams << sigElem.attribute("sigPam1");
            sigPams << sigElem.attribute("sigPam2");
            sigPams << sigElem.attribute("sigPam3");

            if(signal->assignMode == CRC)
            {
                signal->checkSumInfos = sigPams;

                frame->flagChecksum = true;

                CheckSumInfo checkSumInfo;
                checkSumInfo.startIndex_CS_data = QString(signal->checkSumInfos.at(0)).toInt();
                checkSumInfo.endIndex_CS_data = QString(signal->checkSumInfos.at(1)).toInt();
                checkSumInfo.index_CS = QString(signal->checkSumInfos.at(2)).toInt();
                frame->checkSumInfoList.append(checkSumInfo);
            }
            else
            {
                signal->sigAssignModePams = sigPams;
            }


            if (signal->m_name != "Time")
            {
                signal->m_startBit -= 64;
                signal->m_startBit_oth -= 64;
                frame->m_sigs.append(signal);
            }

            //qDebug()<<"sigName="<<signal->m_name<<", sigPams RC="<<signal->sigAssignModePams<<", sigPams CRC="<<signal->checkSumInfos;
            sigElem = sigElem.nextSiblingElement();
        }

        if (frame->m_id != 0)
        {
            int isRead = msgElem.attribute("isRead").toInt();
            if (isRead)
            {
                dbc_frames_r.append(frame);
            }
            else
            {
                dbc_frames_w.append(frame);
            }
        }

        msgElem = msgElem.nextSiblingElement();

    }

    //emit readyForLoadStoredMsg();

    return true;
}

QString XML_GEN::getSel_frames_r() const
{
    return sel_frames_r;
}

void XML_GEN::setSel_frames_r(const QString &value)
{
    sel_frames_r = value;
}

QString XML_GEN::getSel_frames_w() const
{
    return sel_frames_w;
}

void XML_GEN::setSel_frames_w(const QString &value)
{
    sel_frames_w = value;
}

QString XML_GEN::getSel_sigs_r() const
{
    return sel_sigs_r;
}

void XML_GEN::setSel_sigs_r(const QString &value)
{
    sel_sigs_r = value;
}

QString XML_GEN::getSel_sigs_w() const
{
    return sel_sigs_w;
}

void XML_GEN::setSel_sigs_w(const QString &value)
{
    sel_sigs_w = value;
}

void XML_GEN::setXmlFilePath(const QString &value)
{
    xmlFilePath = value;
}

void XML_GEN::setIsArxml(bool value)
{
    isArxml = value;
}

void XML_GEN::setArxmlPath(QString path)
{
    sel_arxmlPath = path;
}

void XML_GEN::setArxmlRefPath(QString refPath)
{
    sel_arxmlPath_ref = refPath;
}

void XML_GEN::setArCluster(QString cluster)
{
    sel_arCluster = cluster;
}

void XML_GEN::setDbcPath(QString path)
{
    sel_dbcPath = path;
}

void XML_GEN::setDbcRefPath(QString refPath)
{
    sel_dbcPath_ref = refPath;
}

void XML_GEN::setCluster(QString cluster)
{
    sel_cluster = cluster;
}

void XML_GEN::setInterface(QString intf)
{
    sel_interface = intf;
}

void XML_GEN::setBaudrate(u64 baudrate)
{
    sel_baudrate = baudrate;
}

void XML_GEN::setCANIOMode(u32 canioMode)
{
    sel_canio_mode = canioMode;
}

void XML_GEN::setCANFDBaudrate(u64 canfdBaudrate)
{
    sel_canfd_baudrate = canfdBaudrate;
}

void XML_GEN::setCANTermination(u32 Termination)
{
    canTermination = Termination;
}

void XML_GEN::setCustomBD(u32 customBD)
{
    this->customBD = customBD;
}

void XML_GEN::setCustomFDBD(u32 customFDBD)
{
    this->customFDBD = customFDBD;
}

void XML_GEN::setSamplePoint_BD(qreal samplePoint)
{
    this->samplePoint_BD = samplePoint;
}

void XML_GEN::setSamplePoint_FDBD(qreal samplePoint)
{
    this->samplePoint_FDBD = samplePoint;
}

void XML_GEN::setFramesForRead(QList<dbc_frame *> frames)
{
    dbc_frames_r.clear();
    dbc_frames_r = frames;
}

void XML_GEN::setFramesForWrite(QList<dbc_frame *> frames)
{
    dbc_frames_w.clear();
    dbc_frames_w = frames;
}

bool XML_GEN::getIsArxml()
{
    return isArxml;
}

QString XML_GEN::getArxmlPath()
{
    return sel_arxmlPath;
}

QString XML_GEN::getArxmlRefPath()
{
    return sel_arxmlPath_ref;
}

QString XML_GEN::getArCluster()
{
    return sel_arCluster;
}

QString XML_GEN::getDbcRefPath()
{
    return sel_dbcPath_ref;
}

QString XML_GEN::getDbcPath()
{
    return sel_dbcPath;
}

QString XML_GEN::getCluster()
{
    return sel_cluster;
}

QString XML_GEN::getInterface()
{
    return sel_interface;
}

u64 XML_GEN::getBaudrate()
{
    return sel_baudrate;
}

u32 XML_GEN::getCANIOMode()
{
    return sel_canio_mode;
}

u64 XML_GEN::getCANFDBaudrate()
{
    return sel_canfd_baudrate;
}

u32 XML_GEN::getCANTermination()
{
    return canTermination;
}

u32 XML_GEN::getCustomBD()
{
    return customBD;
}

u32 XML_GEN::getCustomFDBD()
{
    return customFDBD;
}

qreal XML_GEN::getSamplePoint_BD()
{
    return samplePoint_BD;
}

qreal XML_GEN::getSamplePoint_FDBD()
{
    return samplePoint_FDBD;
}

QList<dbc_frame *> XML_GEN::getFramesForRead()
{
    return dbc_frames_r;
}

QList<dbc_frame *> XML_GEN::getFramesForWrite()
{
    return dbc_frames_w;
}
