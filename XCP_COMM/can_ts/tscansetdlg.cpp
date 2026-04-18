#include "tscansetdlg.h"
#include "ui_tscansetdlg.h"
#include <QDir>
#include <QDesktopWidget>
#include "dsbdelegate.h"

TsCanSetDlg::TsCanSetDlg(QWidget *parent, QString work_name, QString fun_name, QString intf_name, QString set_file_path) :
    QDialog(parent),
    workName(work_name), funName(fun_name),
    intfName(intf_name), canSetFilePath(set_file_path),
    ui(new Ui::TsCanSetDlg)
{
    ui->setupUi(this);
    setWindowTitle("CAN/CANFD Setting");

    QDesktopWidget *deskTop =  QApplication::desktop();
    resize(deskTop->width()*3/4, deskTop->height()*3/4);

    QStringList header;
    header<<"Message"<<"Data"<<"ID"<<"TimeStamp";

    readModel = new QStandardItemModel();
    readModel->setColumnCount(4);
    readModel->setHorizontalHeaderLabels(header);
    ui->traceTreeView_Read->setModel(readModel);
    ui->traceTreeView_Read->setColumnWidth(0, 240);
    ui->traceTreeView_Read->setColumnWidth(1, 240);

    writeModel = new QStandardItemModel();
    writeModel->setColumnCount(4);
    writeModel->setHorizontalHeaderLabels(header);
    ui->traceTreeView_Write->setModel(writeModel);
    ui->traceTreeView_Write->setColumnWidth(0, 240);
    ui->traceTreeView_Write->setColumnWidth(1, 240);

    DSBDelegate *delegate = new DSBDelegate(this);
    ui->traceTreeView_Write->setItemDelegate(delegate);
    ui->traceTreeView_Write->setEditTriggers(QAbstractItemView::DoubleClicked);
    connect(delegate, SIGNAL(modelDataUpdated(int, int, double)), this, SLOT(Slt_writeModelDataUpdated(int, int, double)));

    ui->traceTreeView_Read->header()->setSectionsMovable(false);
    ui->traceTreeView_Write->header()->setSectionsMovable(false);
    ui->dbcTreeView->header()->setSectionsMovable(false);

    dbcModel = new QStandardItemModel();
    ui->dbcTreeView->setModel(dbcModel);
    ui->dbcTreeView->setHeaderHidden(false);
    ui->dbcTreeView->setEditTriggers(QAbstractItemView::NoEditTriggers);

    initDeviceTypeNumHash();

    //canSetFilePath = QApplication::applicationDirPath() + canSetFilePath;
    QString fileName = workName.toLower() + "-" + funName.toLower() + "-setting.xml";
    QString filePath_ref = "/work/" + workName + "/" + fileName;
    QString filePath = QApplication::applicationDirPath() + filePath_ref;

    if(canSetFilePath.isEmpty())
    {

        canSetFileName = fileName;
        canSetFilePath = filePath;
    }
    else
    {
        canSetFilePath = QApplication::applicationDirPath() + canSetFilePath;

        QFileInfo fileInfo(canSetFilePath);
        if(canSetFilePath != filePath && fileInfo.isFile())
        {
            QFile file(canSetFilePath);
            bool ok = file.rename(filePath);
            if(ok)
            {
                qDebug()<<"can/canfd fun setting file rename as :"<<fileName;
                canSetFileName = fileName;
                canSetFilePath = filePath;
            }
        }
    }

    connect(&xml_ins, SIGNAL(readyForLoadDBC(QString)), this, SLOT(loadDbc(QString)));
    connect(&xml_ins, SIGNAL(readyForLoadArxml(QString)), this, SLOT(loadArxml(QString)));

    loadXml();
}

TsCanSetDlg::~TsCanSetDlg()
{
    delete ui;
}

void TsCanSetDlg::setDatabaseHeader(QString filePath)
{
    if(filePath.isEmpty()) return;

    QString name;
    QStringList fileList = filePath.split("/", QString::SkipEmptyParts);
    name = fileList.at(fileList.count()-1);
    name = name.split(".", QString::SkipEmptyParts)[0];

    QStringList header;
    header << name << "ID" << "DLC";
    dbcModel->setHorizontalHeaderLabels(header);
}

void TsCanSetDlg::showClusterInTreeView(CLUSTER_ *cluster)
{
    if(cluster == NULL)
        return;

    ui->checkBox_isCANFD->setChecked(cluster->bus_type);
    ui->ComBox_CANIOMode->setCurrentIndex(cluster->can_io_mode);

    dbcModel->clear();
    dbcModel->setColumnCount(4);
    ui->dbcTreeView->setColumnWidth(0, 200);

    QList<BO_*> boList = cluster->m_boList;
    qDebug()<<"bolist count:"<<cluster->m_boList.count();
    QStandardItem *root = new QStandardItem(cluster->m_name);

    for(int i = 0; i < boList.count(); i++)
    {
        BO_ *frame = boList.at(i);

        QString frameText = frame->m_name;
        QString frameIdText = "0x"+QString::number(frame->m_id, 16).toUpper();
        QString frameDlcText = QString::number(frame->m_length);
        QString framePeriod = QString::number(frame->m_period);

        QList<QStandardItem*> frameList;

        QStandardItem *frameItem0 = new QStandardItem(frameText);
        QStandardItem *frameItem1 = new QStandardItem(frameIdText);
        QStandardItem *frameItem2 = new QStandardItem(frameDlcText);
        QStandardItem *frameItem3 = new QStandardItem(framePeriod);

        frameList << frameItem0 << frameItem1 << frameItem2 << frameItem3;

        root->appendRow(frameList);

        for(int j = 0; j < frame->m_sgList.count(); j++)
        {
            SG_ *sig = frame->m_sgList.at(j);

            QStandardItem *sigItem = new QStandardItem(sig->m_name);

            frameItem0->appendRow(sigItem);

        }

    }

    dbcModel->appendRow(root);

    ui->dbcTreeView->setExpandsOnDoubleClick(true);
    ui->dbcTreeView->expand(dbcModel->indexFromItem(root));

    ui->dbcTreeView->resizeColumnToContents(1);
    ui->dbcTreeView->resizeColumnToContents(2);
    ui->dbcTreeView->resizeColumnToContents(3);
}

void TsCanSetDlg::initDeviceTypeNumHash()
{
    QStringList keys = deviceTypeNumHash.uniqueKeys();
    //qSort(keys.begin(), keys.end(), caseInsensitiveLessThan);

    ui->ComBox_DeviceType->clear();
    ui->ComBox_DeviceType->addItems(keys);
}

void TsCanSetDlg::setHeader(QString filePath)
{
    QString name;
    QStringList fileList = filePath.split("/", QString::SkipEmptyParts);
    name = fileList.at(fileList.count()-1);
    name = name.split(".", QString::SkipEmptyParts)[0];

    QStringList header;
    header << name << "ID" << "DLC" << "Period";

    //ui->dbcTree->setHeaderLabels(header);

    dbcModel->setHorizontalHeaderLabels(header);
}

void TsCanSetDlg::loadDatabase(QString dbFilePath)
{
    if(dbFilePath.isEmpty())
        return;
    qDebug()<<"Loading database file path:"<<dbFilePath;

    if(!dbFilePath.isEmpty())
    {
        loadDbc(dbFilePath);

        canConfig.sel_cluster = ui->ComBox_Cluster->currentText();
        CLUSTER_ *cluster = clusterHash.value(canConfig.sel_cluster);

        showClusterInTreeView(cluster);
    }

    connect(ui->ComBox_Cluster, SIGNAL(currentTextChanged(QString)), this, SLOT(clusterChangedSlot(QString)));
}

void TsCanSetDlg::loadDbc(QString dbcFile)
{
    if(dbcFile.isEmpty())
        return;

    DbcParser *dbcParser = new DbcParser();
    if(dbcParser->doParsering(dbcFile))
    {
        //qDebug()<<"bo count:"<<dbcParser->getBOList().count();

        CLUSTER_ *cluster =  dbcParser->getCluster();
        if(cluster)
        {
            ui->ComBox_Cluster->clear();
            ui->ComBox_Cluster->addItem(cluster->m_name);
            showClusterInTreeView(cluster);

            dbcFilePath = dbcFile;
            if(dbcFile.contains(QApplication::applicationDirPath()))
            {
                dbcFilePath_ref = dbcFile.replace(QApplication::applicationDirPath(), 0);
            }
            else
            {
                dbcFilePath_ref = "";
            }
            m_cluster = cluster;
        }
        setHeader(dbcFile);

        qDebug()<<"dbc file ref path:"<<dbcFilePath_ref;

    }
    else
    {
        qDebug()<<"dbc parsing error.";
    }
    delete dbcParser;
}


void TsCanSetDlg::loadMsgToModel()
{
    if (canConfig.dbc_frames_r.isEmpty() && canConfig.dbc_frames_w.isEmpty())
        return;

    qDebug()<<"show read frames to tree";
    for (int i = 0; i < canConfig.dbc_frames_r.count(); i++)
    {
        BO_ *frame = canConfig.dbc_frames_r[i];

        QStandardItem *msgItem0 = new QStandardItem(frame->m_name);
        QStandardItem *msgItem1 = new QStandardItem("-");
        QStandardItem *msgItem2 = new QStandardItem("0x" + QString::number(frame->m_id, 16).toUpper());
        QStandardItem *msgItem3 = new QStandardItem("-");
        msgItem0->setData(QVariant::fromValue(frame), Qt::UserRole);
        msgItem2->setTextAlignment(Qt::AlignCenter);
        QList<QStandardItem*> items;
        items << msgItem0 << msgItem1 << msgItem2 << msgItem3;

        for(int k = 0; k < items.count(); k++)
        {
            QStandardItem *item = items.at(k);
            QBrush brush(QColor(0xFFEC8B));
            item->setBackground(brush);
        }

        for (int j = 0; j < frame->m_sgList.count(); j++)
        {
            SG_* signal = frame->m_sgList[j];

            QStandardItem *sigItem0 = new QStandardItem(signal->m_name);
            QStandardItem *sigItem1 = new QStandardItem(QString::number(signal->m_value, 'g', 6));
            sigItem0->setData(QVariant::fromValue(signal), Qt::UserRole);
            sigItem1->setTextAlignment(Qt::AlignCenter);
            QList<QStandardItem*> sigItems;
            sigItems << sigItem0 << sigItem1;

            msgItem0->appendRow(sigItems);
        }

        readModel->appendRow(items);
    }

    qDebug()<<"show write frames to tree";
    for (int i = 0; i < canConfig.dbc_frames_w.count(); i++)
    {
        BO_ *frame = canConfig.dbc_frames_w[i];

        QStandardItem *msgItem0 = new QStandardItem(frame->m_name);
        QStandardItem *msgItem1 = new QStandardItem("-");
        QStandardItem *msgItem2 = new QStandardItem("0x" + QString::number(frame->m_id, 16).toUpper());
        QStandardItem *msgItem3 = new QStandardItem("-");
        msgItem0->setData(QVariant::fromValue(frame), Qt::UserRole);
        msgItem2->setTextAlignment(Qt::AlignCenter);
        QList<QStandardItem*> items;
        items << msgItem0 << msgItem1 << msgItem2 << msgItem3;

        for(int k = 0; k < items.count(); k++)
        {
            QStandardItem *item = items.at(k);
            QBrush brush(QColor(0xC1FFC1));
            item->setBackground(brush);
        }

        for (int j = 0; j < frame->m_sgList.count(); j++)
        {
            SG_* signal = frame->m_sgList[j];

            QStandardItem *sigItem0 = new QStandardItem(signal->m_name);
            QStandardItem *sigItem1 = new QStandardItem(QString::number(signal->m_value, 'g', 6));
            sigItem1->setTextAlignment(Qt::AlignCenter);
            sigItem0->setData(QVariant::fromValue(signal), Qt::UserRole);
            sigItem1->setData(QVariant::fromValue(signal), Qt::UserRole);
            QList<QStandardItem*> sigItems;
            sigItems << sigItem0 << sigItem1;

            msgItem0->appendRow(sigItems);
        }

        writeModel->appendRow(items);
    }
}

QStringList TsCanSetDlg::getMsgNameList(QTreeView *treeView)
{
    QStringList msgList;
    if(!treeView) return msgList;
    QStandardItemModel *model = (QStandardItemModel*)(treeView->model());

    if(!model) return msgList;

    int count = model->rowCount();

    for (int i = 0; i < count; i++)
    {
        msgList<<(model->item(i, 0)->text());
    }
    return msgList;
}

void TsCanSetDlg::loadFramesToReadTreeView(QList<BO_ *> boList)
{
    qDebug()<<"load frames to read tree view";

    readModel->clear();
    QStringList header;
    header<<"Message"<<"Data"<<"ID"<<"TimeStamp";
    readModel->setHorizontalHeaderLabels(header);
    readModel->setColumnCount(4);
    ui->traceTreeView_Read->setColumnWidth(0, 240);
    ui->traceTreeView_Read->setColumnWidth(1, 240);

    for (int i = 0; i < boList.count(); i++)
    {
        BO_* frame = boList[i];

        QStandardItem *msgItem0 = new QStandardItem(frame->m_name);
        QStandardItem *msgItem1 = new QStandardItem("-");
        QStandardItem *msgItem2 = new QStandardItem("0x" + QString::number(frame->m_id, 16).toUpper());
        QStandardItem *msgItem3 = new QStandardItem("-");
        msgItem0->setData(QVariant::fromValue(frame), Qt::UserRole);
        msgItem2->setTextAlignment(Qt::AlignCenter);
        QList<QStandardItem*> items;
        items << msgItem0 << msgItem1 << msgItem2 << msgItem3;

        for(int k = 0; k < items.count(); k++)
        {
            QStandardItem *item = items.at(k);
            QBrush brush(QColor(0xFFEC8B));
            item->setBackground(brush);
        }

        for (int j = 0; j < frame->m_sgList.count(); j++)
        {
            SG_* signal = frame->m_sgList[j];

            QStandardItem *sigItem0 = new QStandardItem(signal->m_name);
            QStandardItem *sigItem1 = new QStandardItem(QString::number(signal->m_value, 'g', 6));
            sigItem0->setData(QVariant::fromValue(signal), Qt::UserRole);
            sigItem1->setTextAlignment(Qt::AlignCenter);
            QList<QStandardItem*> sigItems;
            sigItems << sigItem0 << sigItem1;

            msgItem0->appendRow(sigItems);
        }

        readModel->appendRow(items);
    }
}

void TsCanSetDlg::loadFramesToWriteTreeView(QList<BO_ *> boList)
{
    qDebug()<<"load frames to write tree view";

    writeModel->clear();
    QStringList header;
    header<<"Message"<<"Data"<<"ID"<<"TimeStamp";
    writeModel->setHorizontalHeaderLabels(header);
    writeModel->setColumnCount(4);
    ui->traceTreeView_Write->setColumnWidth(0, 240);
    ui->traceTreeView_Write->setColumnWidth(1, 240);

    for (int i = 0; i < boList.count(); i++)
    {
        BO_* frame = boList[i];

        QStandardItem *msgItem0 = new QStandardItem(frame->m_name);
        QStandardItem *msgItem1 = new QStandardItem("-");
        QStandardItem *msgItem2 = new QStandardItem("0x" + QString::number(frame->m_id, 16).toUpper());
        QStandardItem *msgItem3 = new QStandardItem("-");
        msgItem0->setData(QVariant::fromValue(frame), Qt::UserRole);
        msgItem2->setTextAlignment(Qt::AlignCenter);
        QList<QStandardItem*> items;
        items << msgItem0 << msgItem1 << msgItem2 << msgItem3;

        for(int k = 0; k < items.count(); k++)
        {
            QStandardItem *item = items.at(k);
            QBrush brush(QColor(0xC1FFC1));
            item->setBackground(brush);
        }

        for (int j = 0; j < frame->m_sgList.count(); j++)
        {
            SG_* signal = frame->m_sgList[j];

            QStandardItem *sigItem0 = new QStandardItem(signal->m_name);
            QStandardItem *sigItem1 = new QStandardItem(QString::number(signal->m_value, 'g', 6));
            sigItem1->setTextAlignment(Qt::AlignCenter);
            sigItem0->setData(QVariant::fromValue(signal), Qt::UserRole);
            sigItem1->setData(QVariant::fromValue(signal), Qt::UserRole);
            QList<QStandardItem*> sigItems;
            sigItems << sigItem0 << sigItem1;

            msgItem0->appendRow(sigItems);
        }

        writeModel->appendRow(items);
    }
}

BO_ *TsCanSetDlg::getBoById(quint32 id)
{
    if(m_cluster == NULL)
        return NULL;
    if(m_cluster->m_boList.isEmpty())
        return NULL;
    foreach (BO_* bo, m_cluster->m_boList) {
        if(!bo)
            continue;
        if(bo->m_id == id)
            return bo;
    }
    return NULL;
}

BO_ *TsCanSetDlg::getBoByName(QString boName)
{
    if(m_cluster == NULL)
        return NULL;
    if(m_cluster->m_boList.isEmpty())
        return NULL;
    foreach (BO_* bo, m_cluster->m_boList) {
        if(!bo)
            continue;
        if(bo->m_name == boName)
            return bo;
    }
    return NULL;
}

QList<BO_ *> TsCanSetDlg::getBoListByNames(QStringList boNameList)
{
    QList<BO_*> boList;
    for(int i = 0; i < boNameList.count(); i++)
    {
        boList.append(getBoByName(boNameList.at(i)));
    }

    return boList;
}

SG_ *TsCanSetDlg::getSgByName(BO_ *bo, QString sgName)
{
    if(!bo)
        return NULL;
    if(bo->m_sgList.isEmpty())
        return NULL;
    foreach (SG_* sg, bo->m_sgList) {
        if(!sg)
            continue;
        if(sg->m_name == sgName)
            return sg;
    }

    return NULL;
}


bool TsCanSetDlg::saveXml()
{

    QStringList readFrameList = getMsgNameList(ui->traceTreeView_Read);
    QStringList writeFrameList = getMsgNameList(ui->traceTreeView_Write);

    QString deviceType = ui->ComBox_DeviceType->currentText();
    quint8 deviceIndex = ui->ComBox_DeviceIndex->currentIndex();
    quint8 canIndex = ui->ComBox_CanIndex->currentIndex();

    QString modName = "TSCAN" + QString::number(canIndex);

    quint8 can_io_mode = ui->ComBox_CANIOMode->currentIndex();
    quint8 canfd_standard = ui->ComBox_CanFDStandard->currentIndex();

    quint64 can_baudrate = (quint64)(ui->ComBox_Baudrate->currentText().toInt());
    quint64 canfd_baudrate = (quint64)(ui->ComBox_CANFD_Baudrate->currentText().toLongLong());

    quint8 termination_enable = ui->checkBox_Termination->isChecked() ? 1 : 0;
    quint8 isCanFD = ui->checkBox_isCANFD->isChecked() ? 1 : 0;

    double samplePoint_abit = ui->doubleSpinBox_SamplePointBD->value();
    double samplePoint_dbit = ui->doubleSpinBox_SamplePointFDBD->value();
    bool sp_abit_enable = ui->checkBox_CustomBD->isChecked();
    bool sp_dbit_enable = ui->checkBox_CustomFDBD->isChecked();

    if(can_io_mode == 0)
    {
        if(can_baudrate == 0)
            return false;
    }
    else
    {
        if(canfd_baudrate == 0)
            return false;
    }

    QString appPath = QApplication::applicationDirPath();
    //QString xmlPath_ref = "/pamFactory/TS_" + deviceType + "_DEV" + QString::number(deviceIndex) + "_CAN" + QString::number(canIndex) + ".xml";
    //QString xmlPath = appPath + xmlPath_ref;
    QString xmlPath = this->canSetFilePath;
    qDebug()<<"Save xml file:"<<xmlPath;

    QFile xmlFile(xmlPath);
    if (!xmlFile.open(QFile::WriteOnly | QFile::Text))
    {
        qDebug()<<tr("Cannot write file %1:\n%2.").arg(xmlPath)
                  .arg(xmlFile.errorString());
        return false;
    }

    QDomDocument xml("TS_CAN");

    QDomElement root = xml.createElement("Root");
    root.setAttribute("dbcfile", dbcFilePath);
    root.setAttribute("dbcfile_ref", dbcFilePath_ref);
    root.setAttribute("cluster_name", m_cluster->m_name);
    root.setAttribute("device_type", deviceType);
    root.setAttribute("device_index", deviceIndex);
    root.setAttribute("can_index", canIndex);
    root.setAttribute("can_io_mode", can_io_mode);
    root.setAttribute("canfd_standard", canfd_standard);
    root.setAttribute("can_baudrate", can_baudrate);
    root.setAttribute("canfd_baudrate", canfd_baudrate);
    root.setAttribute("termination_enable", termination_enable);
    root.setAttribute("custom_abit", sp_abit_enable);
    root.setAttribute("custom_dbit", sp_dbit_enable);
    root.setAttribute("samplepoint_abit", samplePoint_abit);
    root.setAttribute("samplepoint_dbit", samplePoint_dbit);
    xml.appendChild(root);

    QList<BO_*> readBOList = getBoListByNames(readFrameList);

    for(int i=0; i < readBOList.count(); i++)
    {
        BO_* frame = readBOList[i];
        //qDebug()<<"frame:"<<frame->m_name;
        quint32 sizeFrame = frame->m_length + 8;
        //qDebug()<<"size of frame:"<<sizeFrame;
        //QDomElement ReadElem = xml.createElement("RP_" + frame->m_name + "_" + moduleName);
        QDomElement ReadElem = xml.createElement("RP_0x" + QString::number(frame->m_id, 16).toUpper() + "_" + modName);
        root.appendChild(ReadElem);
        ReadElem.setAttribute("size_bytes", sizeFrame);
        ReadElem.setAttribute("is_read", 1);
        ReadElem.setAttribute("id", "0x"+QString::number(frame->m_id, 16));
        ReadElem.setAttribute("extented", frame->m_extented);
        ReadElem.setAttribute("dlc", frame->m_length);
        ReadElem.setAttribute("sample_rate", frame->m_period);

        QDomElement timeElem = xml.createElement("param_"+QString::number(0));
        ReadElem.appendChild(timeElem);
        timeElem.setAttribute("unit", "s");
        timeElem.setAttribute("value_default", "0");
        timeElem.setAttribute("minimum", "0");
        timeElem.setAttribute("maximum", "0");
        timeElem.setAttribute("scalingoffset", "0");
        timeElem.setAttribute("scalingfactor", "1");
        timeElem.setAttribute("lengthbit", "64");
        timeElem.setAttribute("startbit", "0");
        timeElem.setAttribute("startbit_oth", "0");
        timeElem.setAttribute("byteorder", "0");
        timeElem.setAttribute("type", "3");
        timeElem.setAttribute("param_name", "Time");

        for (int j=1; j < frame->m_sgList.count()+1; j++)
        {
            //qDebug()<<"signal read inde = "<<j;
            SG_* sig = frame->m_sgList[j-1];
            QDomElement Elem = xml.createElement("param_"+QString::number(j));
            ReadElem.appendChild(Elem);

            Elem.setAttribute("unit", sig->m_unit);
            Elem.setAttribute("value_default", QString::number(sig->m_value_default));
            Elem.setAttribute("minimum", QString::number(sig->m_min));
            Elem.setAttribute("maximum", QString::number(sig->m_max));
            Elem.setAttribute("scalingoffset", QString::number(sig->m_offset));
            Elem.setAttribute("scalingfactor", QString::number(sig->m_factor));
            Elem.setAttribute("lengthbit", QString::number(sig->m_bitLen, 10));
            Elem.setAttribute("startbit", QString::number(sig->m_startBit+64, 10));
            Elem.setAttribute("startbit_oth", QString::number(sig->m_startBit+64, 10));

            int byteOrder = 0;
            if(sig->m_byteorder == 0)
            {
                byteOrder = 1;
            }
            else if(sig->m_byteorder == 1)
            {
                byteOrder = 0;
            }

            Elem.setAttribute("byteorder", QString::number(byteOrder,10));

            int type = sig->m_valType;
            if(type == 0)
                type = 10;       // can type 0(signed) transfer to mdf type 10(moto signed)
            else if(type == 1)
                type = 9;        // can type 1(unsigned) transfer to mdf type 9(moto unsigned)
            Elem.setAttribute("type", QString::number(type, 10));

            Elem.setAttribute("param_name", sig->m_name);

        }
    }

    QList<BO_*> writeBOList = getBoListByNames(writeFrameList);

    for(int i=0; i < writeBOList.count(); i++)
    {
        BO_* frame = writeBOList[i];
        //qDebug()<<"frame:"<<frame->m_name;
        quint32 sizeFrame = frame->m_length + 8;
        //qDebug()<<"size of frame:"<<sizeFrame;
        //QDomElement WriteElem = xml.createElement("WP_" + frame->m_name + "_" + moduleName);
        QDomElement WriteElem = xml.createElement("WP_0x" + QString::number(frame->m_id, 16).toUpper() + "_" + modName);
        root.appendChild(WriteElem);
        WriteElem.setAttribute("size_bytes", sizeFrame);
        WriteElem.setAttribute("is_read", 0);
        WriteElem.setAttribute("id", "0x"+QString::number(frame->m_id, 16));
        WriteElem.setAttribute("extented", frame->m_extented);
        WriteElem.setAttribute("dlc", frame->m_length);
        WriteElem.setAttribute("sample_rate", frame->m_period);

        QDomElement timeElem = xml.createElement("param_"+QString::number(0));
        WriteElem.appendChild(timeElem);
        timeElem.setAttribute("unit", "s");
        timeElem.setAttribute("value_default", "0");
        timeElem.setAttribute("minimum", "0");
        timeElem.setAttribute("maximum", "0");
        timeElem.setAttribute("scalingoffset", "0");
        timeElem.setAttribute("scalingfactor", "1");
        timeElem.setAttribute("lengthbit", "64");
        timeElem.setAttribute("startbit", "0");
        timeElem.setAttribute("startbit_oth", "0");
        timeElem.setAttribute("byteorder", "0");
        timeElem.setAttribute("type", "3");
        timeElem.setAttribute("param_name", "Time");

        for (int j=1; j < frame->m_sgList.count()+1; j++)
        {
            SG_* sig = frame->m_sgList[j-1];
            //qDebug()<<"sig in gen xml name:"<<sig->Name<<",startbit:"<<sig->StartBit;
            QDomElement Elem = xml.createElement("param_"+QString::number(j));
            WriteElem.appendChild(Elem);

            Elem.setAttribute("unit", sig->m_unit);
            Elem.setAttribute("value_default", QString::number(sig->m_value_default));
            Elem.setAttribute("minimum", QString::number(sig->m_min));
            Elem.setAttribute("maximum", QString::number(sig->m_max));
            Elem.setAttribute("scalingoffset", QString::number(sig->m_offset));
            Elem.setAttribute("scalingfactor", QString::number(sig->m_factor));
            Elem.setAttribute("lengthbit", QString::number(sig->m_bitLen, 10));
            Elem.setAttribute("startbit", QString::number(sig->m_startBit+64, 10));
            Elem.setAttribute("startbit_oth", QString::number(sig->m_startBit+64, 10));

            int byteOrder = 0;
            if(sig->m_byteorder == 0)
            {
                byteOrder = 1;
            }
            else if(sig->m_byteorder == 1)
            {
                byteOrder = 0;
            }
            Elem.setAttribute("byteorder", QString::number(byteOrder,10));

            int type = sig->m_valType;
            if(type == 0)
                type = 10;       // can type 0(signed) transfer to mdf type 10(moto signed)
            else if(type == 1)
                type = 9;        // can type 1(unsigned) transfer to mdf type 9(moto unsigned)
            Elem.setAttribute("type", QString::number(type, 10));

            Elem.setAttribute("param_name", sig->m_name);
            Elem.setAttribute("set_value", QString::number(sig->m_value));

        }
    }

    QString xmlString = xml.toString();
    QTextStream out(&xmlFile);
    out << xmlString;

    xmlFile.close();

    return true;
}

bool TsCanSetDlg::loadXml()
{
    QString appPath = QApplication::applicationDirPath();

    QString deviceType = ui->ComBox_DeviceType->currentText();
    quint8 deviceIndex = ui->ComBox_DeviceIndex->currentIndex();
    quint8 canIndex = ui->ComBox_CanIndex->currentIndex();

    //QString fileName = "TS_" + deviceType + "_DEV" + QString::number(deviceIndex) + "_CAN" + QString::number(canIndex) + ".xml";
    //QString xmlPath = appPath + "/pamFactory/" + fileName;
    QString xmlPath = this->canSetFilePath;
    qDebug()<<"Load xml file:"<<xmlPath;

    if (xmlPath.isEmpty())
        return false;

    QFile xmlFile(xmlPath);
    if (!xmlFile.open(QFile::ReadOnly | QFile::Text))
    {
        qDebug()<<tr("Cannot read file %1:\n%2.").arg(xmlPath).arg(xmlFile.errorString());
        return false;
    }

    QDomDocument xml("TS_CAN");

    if (!xml.setContent(&xmlFile))
    {
        xmlFile.close();
        return false;
    }
    xmlFile.close();

    QDomElement root = xml.documentElement();

    QString dbcPath = root.attribute("dbcfile");
    QString dbcPath_ref = root.attribute("dbcfile_ref");
    QString clusterName = root.attribute("cluster_name");

    if(!dbcPath_ref.isEmpty())
    {
        loadDbc(QApplication::applicationDirPath() + dbcPath_ref);

    }
    ui->ComBox_DeviceType->setCurrentText(root.attribute("device_type"));
    ui->ComBox_DeviceIndex->setCurrentIndex(root.attribute("device_index").toInt());
    ui->ComBox_CanIndex->setCurrentIndex(root.attribute("can_index").toInt());
    ui->ComBox_CANIOMode->setCurrentIndex(root.attribute("can_io_mode").toInt());
    ui->ComBox_CanFDStandard->setCurrentIndex(root.attribute("canfd_standard").toInt());

    ui->ComBox_Baudrate->setCurrentText(root.attribute("can_baudrate"));
    ui->ComBox_CANFD_Baudrate->setCurrentText(root.attribute("canfd_baudrate")); // toLongLong

    ui->checkBox_Termination->setChecked(root.attribute("termination_enable").toInt() > 0 ? true : false);
    ui->checkBox_CustomBD->setChecked(root.attribute("custom_abit").toInt() > 0 ? true : false);
    ui->checkBox_CustomFDBD->setChecked(root.attribute("custom_dbit").toInt() > 0 ? true : false);
    ui->doubleSpinBox_SamplePointBD->setValue(root.attribute("samplepoint_abit").toDouble());
    ui->doubleSpinBox_SamplePointFDBD->setValue(root.attribute("samplepoint_dbit").toDouble());

    //QString modName = deviceType + "_DEV" + QString::number(deviceIndex)
    //                    + "_CAN" + QString::number(canIndex);
    QString modName = "TSCAN" + QString::number(canIndex);

    QList<BO_*> readFrames;
    QList<BO_*> writeFrames;

    QDomElement msgElem = root.firstChildElement();
    while (!msgElem.isNull())
    {

        QString boName = msgElem.tagName();
        //BO_* frame = getBoByName(boName);

        quint32 id = msgElem.attribute("id").toInt(0,16);
        BO_* frame = getBoById(id);

        if(!frame)
        {
            msgElem = msgElem.nextSiblingElement();
            continue;
        }

        //qDebug()<<"frame Name ="<<frame->m_name<<",id="<<frame->m_id;

        QDomElement sigElem = msgElem.firstChildElement();
        while (!sigElem.isNull())
        {
            QString sigName = sigElem.attribute("param_name");
            SG_* signal = getSgByName(frame, sigName);
            if(!signal)
            {
                sigElem = sigElem.nextSiblingElement();
                continue;
            }

            signal->m_value = sigElem.attribute("set_value").toDouble();

            //qDebug()<<"sigName="<<signal->m_name<<", setValue="<<signal->m_value;

            //if (signal->m_name != "Time")
            //    frame->m_sgList.append(signal);

            sigElem = sigElem.nextSiblingElement();
        }


        if (frame->m_id != 0)
        {
            int isRead = msgElem.attribute("is_read").toInt();
            if (isRead)
            {
                readFrames.append(frame);
            }
            else
            {
                writeFrames.append(frame);
            }
        }

        msgElem = msgElem.nextSiblingElement();

    }

    loadFramesToReadTreeView(readFrames);
    loadFramesToWriteTreeView(writeFrames);

    return true;
}


QString TsCanSetDlg::getCanSetFilePath() const
{
    return canSetFilePath;
}

void TsCanSetDlg::accept()
{
    saveXml();
    QDialog::accept();
}

QString TsCanSetDlg::getCanSetFileName() const
{
    return canSetFileName;
}

void TsCanSetDlg::on_pb_loadDatabase_clicked()
{
    QString filePath = QFileDialog::getOpenFileName(this,tr("Load dbc file..."),"D:", tr("dbc file(*.dbc *.DBC *.xml *.arxml *.ARXML)"));
    if(filePath.isEmpty())
        return;
    qDebug()<<"Selected database file path = "<<filePath;

    QString filePath_ref = filePath;
    filePath_ref.replace(QCoreApplication::applicationDirPath(), "");
    qDebug()<<"Selected database file ref path = "<<filePath_ref;

    setDatabaseHeader(filePath);

    if(filePath.contains(".arxml") || filePath.contains(".ARXML"))
    {

    }
    else if (!filePath.isEmpty())
    {
        loadDbc(filePath);

        canConfig.sel_cluster = ui->ComBox_Cluster->currentText();
        CLUSTER_ *cluster = clusterHash.value(canConfig.sel_cluster);
        showClusterInTreeView(cluster);
    }

    //connect(ui->ComBox_Cluster, SIGNAL(currentTextChanged(QString)), this, SLOT(clusterChangedSlot(QString)));
}

void TsCanSetDlg::on_pb_reset_clicked()
{
    int result = QMessageBox::warning(this,
                         QString::fromLocal8Bit("Reset"),
                         QString::fromLocal8Bit("Confirm to clear selection? \n "),
                         QMessageBox::Ok | QMessageBox::Cancel, QMessageBox::NoButton
                         );
    if (result == QMessageBox::Cancel)
    {

    }
    else
    {

        readModel->clear();
        writeModel->clear();

        readModel->setColumnCount(4);
        writeModel->setColumnCount(4);
        QStringList headerLabels;
        headerLabels<<"Message"<<"Data"<<"Pam0"<<"Pam1";

        readModel->setHorizontalHeaderLabels(headerLabels);
        writeModel->setHorizontalHeaderLabels(headerLabels);

        ui->traceTreeView_Read->setColumnWidth(0, 240);
        ui->traceTreeView_Write->setColumnWidth(0, 240);
    }
}
