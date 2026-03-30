#include "pamseldlg.h"
#include "ui_pamseldlg.h"
#include <QDebug>
#include <QComboBox>

PAMSELDlg::PAMSELDlg(QWidget *parent, A2L_Project *project) :
    QDialog(parent), a2lProject(project),
    ui(new Ui::PAMSELDlg)
{
    ui->setupUi(this);
    setWindowTitle("Variable Select");

    showMaximized();

    pamNameCharSelected.clear();
    pamNameMeasSelected.clear();

    ui->list_MeasPams->setSelectionMode(QAbstractItemView::MultiSelection);
    ui->list_CharPams->setSelectionMode(QAbstractItemView::MultiSelection);

    ui->checkBox_Char->setChecked(true);
    ui->checkBox_Meas->setChecked(true);

    ui->table_Pams_Selected_Rate->setColumnCount(2);
    QStringList labels;
    labels << "Pam" << "Rate_ms";
    ui->table_Pams_Selected_Rate->setHorizontalHeaderLabels(labels);
    ui->table_Pams_Selected_Rate->setColumnWidth(0, 500);
    ui->table_Pams_Selected_Rate->setColumnWidth(1, 150);

    if(a2lProject->getDaqMemoryConsumption())
        ui->progressBar_VolumnUsage->setMaximum(a2lProject->getDaqMemoryConsumption()->Daq_Memory_Limit);

    ui->progressBar_BusLoad->setMaximum(100);


    if (a2lProject)
    {
        setMeasAll(a2lProject->getA2LAllMeasVarNameList());
        setCharAll(a2lProject->getA2LAllCharVarNameList());
        setEventNameList(a2lProject->getEventNameList());

        //setEventCurSizeHash(a2lProject->getEventCurSizeHash());
        //setEventMaxSizeHash(a2lProject->getEventMaxSizeHash());
        //setEventRateHash(a2lProject->getEventRateHash());
    }
    else
    {
        qDebug()<<"a2lProject in pamseldlg is NULL.";
    }


    connect(ui->checkBox_Meas, SIGNAL(stateChanged(int)), this, SLOT(checkStateChangedMeasSlot(int)));
    connect(ui->checkBox_Char, SIGNAL(stateChanged(int)), this, SLOT(checkStateChangedCharSlot(int)));

    connect(ui->lineEdit_Search, SIGNAL(textEdited(QString)), this, SLOT(searchTextChangedSlot(QString)));

    connect(ui->list_MeasPams, SIGNAL(itemClicked(QListWidgetItem*)), this, SLOT(measItemClickedSlot(QListWidgetItem*)));
    connect(ui->list_CharPams, SIGNAL(itemClicked(QListWidgetItem*)), this, SLOT(charItemClickedSlot(QListWidgetItem*)));
    connect(ui->list_MeasPamsMatrix, SIGNAL(itemClicked(QListWidgetItem*)), this, SLOT(measMatrixItemClickedSlot(QListWidgetItem*)));

    connect(ui->list_Pams_Selected, SIGNAL(itemClicked(QListWidgetItem*)), this, SLOT(selectedItemClickedSlot(QListWidgetItem*)));
    connect(ui->table_Pams_Selected_Rate, SIGNAL(itemDoubleClicked(QTableWidgetItem*)), this, SLOT(doubleClickedItemClickedSlot(QTableWidgetItem*)));

    //connect(ui->comboBox_Event, SIGNAL(currentTextChanged(QString)), this, SLOT(eventComboTextChangedSlot(QString)));

    connect(this, SIGNAL(accepted()), this, SLOT(getResultPamNames()));



}

PAMSELDlg::~PAMSELDlg()
{
    delete ui;
}

void PAMSELDlg::setMeasAll(QStringList measNameList)
{
    pamNameMeasAll = measNameList;

    if (ui->checkBox_Meas->isChecked())
    {
        setMeasList(measNameList);
    }

}

void PAMSELDlg::setCharAll(QStringList charNameList)
{
    pamNameCharAll = charNameList;

    if (ui->checkBox_Char->isChecked())
    {
        setCharList(charNameList);
    }

}

void PAMSELDlg::setList(QStringList measList, QStringList charList)
{
    setMeasList(measList);
    setCharList(charList);
}

void PAMSELDlg::setMeasList(QStringList measList)
{
    ui->list_MeasPams->clear();
    if (!ui->checkBox_Meas->isChecked())
        return;
    ui->list_MeasPams->addItems(measList);

}

void PAMSELDlg::setCharList(QStringList charList)
{
    ui->list_CharPams->clear();
    if (!ui->checkBox_Char->isChecked())
        return;
    ui->list_CharPams->addItems(charList);
}

void PAMSELDlg::setEventNameList(QStringList eventList)
{
    if(eventList.isEmpty())
        return;

    this->eventNameList = eventList;
    ui->comboBox_Event->addItems(eventList);
    ui->comboBox_Event->setCurrentIndex(0);
    //quint32 size = eventNameMaxSizeHash.value(ui->comboBox_Event->currentText());
    //ui->lineEdit_sizeMax->setText(QString::number(size));

}

void PAMSELDlg::setEventCurSizeHash(QHash<QString, quint32> curSizeHash)
{
    eventNameCurSizeHash = curSizeHash;
    eventNameTempCurSizeHash = curSizeHash;

}

void PAMSELDlg::setEventMaxSizeHash(QHash<QString, quint32> maxSizeHash)
{
    eventNameMaxSizeHash = maxSizeHash;
    quint32 size = eventNameMaxSizeHash.value(ui->comboBox_Event->currentText());
    ui->lineEdit_sizeMax->setText(QString::number(size));
}

void PAMSELDlg::setEventRateHash(QHash<QString, qreal> rateHash)
{
    eventNameRateHash = rateHash;
}

void PAMSELDlg::checkStateChangedMeasSlot(int state)
{
    if (state == Qt::Checked)
    {
        pamNameMeasFilted = getPamsMatchExp(pamNameMeasAll, ui->lineEdit_Search->text());
        setMeasList(pamNameMeasFilted);
    }
    else if (state == Qt::Unchecked)
    {
        pamNameMeasFilted.clear();
        setMeasList(pamNameMeasFilted);
    }
}

void PAMSELDlg::checkStateChangedCharSlot(int state)
{
    if (state == Qt::Checked)
    {
        pamNameCharFilted = getPamsMatchExp(pamNameCharAll, ui->lineEdit_Search->text());
        setCharList(pamNameCharFilted);
    }
    else if (state == Qt::Unchecked)
    {
        pamNameCharFilted.clear();
        setCharList(pamNameCharFilted);
    }
}

void PAMSELDlg::measItemClickedSlot(QListWidgetItem *item)
{
    if (!a2lProject)
        return;

    QString clickedItemStr = item->text();
    A2L_VarMeas *measVar = a2lProject->getMeasVarByNameAndRate(clickedItemStr, eventNameList.at(0));

    if (!measVar)
        return;

    qDebug()<<"Clicked meas:"<<measVar->Name<<", arraySize:"<<measVar->ArraySize;
    QStringList measVarMatrixList;

    ui->list_MeasPamsMatrix->clear();
    if(measVar->ArraySize > 1)
    {
        for (int i = 0; i < measVar->ArraySize; i++)
        {
            QString measNameArray = clickedItemStr + "_["+QString::number(i)+"]";
            measVarMatrixList.append(measNameArray);
        }

        ui->list_MeasPamsMatrix->addItems(measVarMatrixList);
        return;
    }
    else if (measVar->xDim > 1 || measVar->yDim > 1 || measVar->zDim > 1)
    {
        for (int i = 0; i < measVar->xDim; i++)
        {

            QString measNameX = clickedItemStr + "_["+QString::number(i)+"]";
            if (measVar->yDim <= 1 && measVar->zDim <= 1)
            {
                measVarMatrixList.append(measNameX);
                continue;
            }
            for (int j = 0; j < measVar->yDim; j++)
            {

                QString measNameY = measNameX + "["+QString::number(j)+"]";
                if (measVar->zDim <= 1)
                {
                    measVarMatrixList.append(measNameY);
                    continue;
                }
                for (int z = 0; z < measVar->zDim; z++)
                {

                    QString measNameZ = measNameY + "["+QString::number(z)+"]";
                    measVarMatrixList.append(measNameZ);
                }
            }
        }
        ui->list_MeasPamsMatrix->addItems(measVarMatrixList);
        return;
    }

    if (pamNameMeasSelected.indexOf(clickedItemStr) == -1)
    {
        pamNameMeasSelected.append(clickedItemStr);
        ui->list_Pams_Selected->addItem(clickedItemStr);

        int rowCount = ui->table_Pams_Selected_Rate->rowCount();
        ui->table_Pams_Selected_Rate->insertRow(rowCount);
        QTableWidgetItem *item = new QTableWidgetItem(clickedItemStr);
        ui->table_Pams_Selected_Rate->setItem(rowCount, 0, item);

        QComboBox *box = new QComboBox();
        box->addItems(eventNameList);
        ui->table_Pams_Selected_Rate->setCellWidget(rowCount, 1, box);

        connect(box, SIGNAL(currentTextChanged(QString)), this, SLOT(selMeasRateChangedSlot(QString)));

        //tempMeasVarList.append(measVar);
        //updateTempEventCurSize(measVar);
    }
    else
    {
        pamNameMeasSelected.removeOne(clickedItemStr);

        if (!takeItemByName(ui->list_Pams_Selected, clickedItemStr))
            qDebug()<<"Take item fail.";

        int row = findSelMeasTableRowByName(clickedItemStr);
        if (row >= 0)
            ui->table_Pams_Selected_Rate->removeRow(row);

        pamNameCharSelected.removeOne(clickedItemStr);

    }

}

void PAMSELDlg::charItemClickedSlot(QListWidgetItem *item)
{
    QString clickedItemStr = item->text();
    if (pamNameCharSelected.indexOf(clickedItemStr) == -1)
    {
        pamNameCharSelected.append(clickedItemStr);
        ui->list_Pams_Selected->addItem(clickedItemStr);
    }
    else
    {
        pamNameCharSelected.removeOne(clickedItemStr);

        if (!takeItemByName(ui->list_Pams_Selected, clickedItemStr))
            qDebug()<<"Take item fail.";
    }
}

void PAMSELDlg::measMatrixItemClickedSlot(QListWidgetItem *item)
{
    if (!a2lProject)
        return;

    QString clickedItemStr = item->text();
    QRegExp rx("\\[\\d\\]");
    int pos = 0;
    QStringList list;
    QList<quint8> dimList = {0,0,0};
    int index = 0;
    while ((pos = rx.indexIn(clickedItemStr, pos)) != -1)
    {
        list << rx.cap(0);
        dimList.replace(index, rx.cap(0).remove(0,1).remove(-1,1).toInt());
        pos += rx.matchedLength();
        index++;
    }
    QString originVarStr = clickedItemStr;
    originVarStr.replace(QRegExp("_(\\[\\d\\])+"), "");
    qDebug()<<"Origin meas:"<<originVarStr;

    A2L_VarMeas *measVar = a2lProject->getMeasVarByNameAndRate(originVarStr, eventNameList.at(0));
    measVar->Name = clickedItemStr;
    measVar->ShortName = originVarStr;

    if(measVar->ArraySize > 1)
    {
        measVar->ECUAddress += (measVar->DataSizeAG * dimList.at(0));
    }
    else if(measVar->xDim >1 || measVar->yDim > 1 || measVar->zDim > 1)
    {
        measVar->ECUAddress += (measVar->DataSizeAG *
                                (dimList.at(0) * measVar->yDim * measVar->zDim +
                                 dimList.at(1) * measVar->zDim +
                                 dimList.at(2)
                                 ));
    }

    qDebug()<<"Meas name="<<measVar->Name<<", short name:"<<measVar->ShortName<<", Address=0x"<<QString::number(measVar->ECUAddress, 16);


    if (pamNameMeasSelected.indexOf(clickedItemStr) == -1)
    {
        pamNameMeasSelected.append(clickedItemStr);
        ui->list_Pams_Selected->addItem(clickedItemStr);

        int rowCount = ui->table_Pams_Selected_Rate->rowCount();
        ui->table_Pams_Selected_Rate->insertRow(rowCount);
        QTableWidgetItem *item = new QTableWidgetItem(clickedItemStr);
        ui->table_Pams_Selected_Rate->setItem(rowCount, 0, item);

        QComboBox *box = new QComboBox();
        box->addItems(eventNameList);
        ui->table_Pams_Selected_Rate->setCellWidget(rowCount, 1, box);

        connect(box, SIGNAL(currentTextChanged(QString)), this, SLOT(selMeasRateChangedSlot(QString)));

        //tempMeasVarList.append(measVar);
        //updateTempEventCurSize(measVar);
    }
    else
    {
        pamNameMeasSelected.removeOne(clickedItemStr);

        if (!takeItemByName(ui->list_Pams_Selected, clickedItemStr))
            qDebug()<<"Take item fail.";

        int row = findSelMeasTableRowByName(clickedItemStr);
        if (row >= 0)
            ui->table_Pams_Selected_Rate->removeRow(row);

        pamNameCharSelected.removeOne(clickedItemStr);

    }
}

void PAMSELDlg::selectedItemClickedSlot(QListWidgetItem *item)
{
    QString clickedItemStr = item->text();

    if (!takeItemByName(ui->list_Pams_Selected, clickedItemStr))
        qDebug()<<"Take item fail.";

    pamNameMeasSelected.removeOne(clickedItemStr);

    int row = findSelMeasTableRowByName(clickedItemStr);
    if (row >= 0)
        ui->table_Pams_Selected_Rate->removeRow(row);

    //removeMeasVarByName(clickedItemStr);

    pamNameCharSelected.removeOne(clickedItemStr);
}

void PAMSELDlg::doubleClickedItemClickedSlot(QTableWidgetItem *item)
{
    int column = item->column();
    if (column == 0)
    {
        QString pamName = item->text();
        ui->table_Pams_Selected_Rate->removeRow(item->row());
        pamNameMeasSelected.removeOne(pamName);
        removeMeasVarByName(pamName);
        takeItemByName(ui->list_Pams_Selected, pamName);
    }
}

QListWidgetItem *PAMSELDlg::findItemByName(QListWidget *list, QString name)
{
    int itemCount = list->count();
    if (itemCount == 0)
        return NULL;
    for(int i = 0; i < itemCount; i++)
    {
        QListWidgetItem *item = list->item(i);
        if (item->text() == name)
            return item;
    }
    return NULL;
}

bool PAMSELDlg::takeItemByName(QListWidget *list, QString name)
{
    int itemCount = list->count();
    if (itemCount == 0)
        return false;
    for(int i = 0; i < itemCount; i++)
    {
        QListWidgetItem *item = list->item(i);
        if (item->text() == name)
        {
            list->takeItem(i);
            delete item;
            return true;
        }
    }
    return false;
}

QStringList PAMSELDlg::getPamsMatchExp(QStringList pamList, QString searchExp)
{
    //QString searchExp = ui->lineEdit_Search->text();

    if (searchExp.isEmpty())
        return pamList;

    QRegExp exp("^" + searchExp);
    exp.setCaseSensitivity(Qt::CaseInsensitive);

    QStringList filtedPamList;

    for (int i = 0; i < pamList.count(); i++)
    {
        QString pam = pamList.at(i);
        if (exp.indexIn(pam) != -1)
            filtedPamList.append(pam);
    }
    return filtedPamList;
}

void PAMSELDlg::searchTextChangedSlot(QString searchText)
{
    //QString searchText = ui->lineEdit_Search->text();
    pamNameMeasFilted = getPamsMatchExp(pamNameMeasAll, searchText);
    pamNameCharFilted = getPamsMatchExp(pamNameCharAll, searchText);

    setList(pamNameMeasFilted, pamNameCharFilted);

}

void PAMSELDlg::getResultPamNames()
{
    emit measPamSelAccepted(pamNameMeasSelected);
    emit charPamSelAccepted(pamNameCharSelected);

    /*
    getPamRateMeasList();
    if (pamRateMeasSelected.count() == pamNameMeasSelected.count())
    {
        emit measPamSelAccepted(pamNameMeasSelected);
        emit charPamSelAccepted(pamNameCharSelected);
    }
    */
}

void PAMSELDlg::getPamRateMeasList()
{
    pamRateMeasSelected.clear();
    for (int i = 0; i < pamNameMeasSelected.count(); i++)
    {
        QComboBox *box = (QComboBox*)(ui->table_Pams_Selected_Rate->cellWidget(i, 1));
        QString curText = box->currentText();
        pamRateMeasSelected.append(curText);
    }
}

void PAMSELDlg::eventComboTextChangedSlot(QString text)
{

    ui->lineEdit_sizeMax->setText(QString::number(eventNameMaxSizeHash.value(text)));
    qDebug()<<"DAQ Type = "<<a2lProject->getDaqInfo()->DaqType;

    quint64 baudrate = 0;
    A2L_Xcp_On_Can *xcpOnCan = a2lProject->getCurUseXcpOnCan();
    if(xcpOnCan)
    {
        baudrate = xcpOnCan->BAUDRATE;
        qDebug()<<"Baudrate in xcp_on_can:"<<QString::number(baudrate);
    }


    if (a2lProject->getDaqInfo()->DaqType == "STATIC")
    {
        qreal curEventSizePerc = 0;
        if (eventNameMaxSizeHash.value(text) != 0)
        {
            qDebug()<<"curSize="<<eventNameTempCurSizeHash.value(text)<<", maxSize="<<eventNameMaxSizeHash.value(text);
            curEventSizePerc = (qreal)(eventNameTempCurSizeHash.value(text)) / (qreal)(eventNameMaxSizeHash.value(text)) * 100;
            qDebug()<<"curPerc"<<curEventSizePerc;

        }
        ui->progressBar_VolumnUsage->setValue((int)curEventSizePerc);

        qreal curBusLoadSum = 0;
        for(int i = 0; i < eventNameTempCurSizeHash.keys().count(); i++)
        {
            QString key = eventNameTempCurSizeHash.keys().at(i);
            int curEventSize = eventNameTempCurSizeHash.value(key);

            qreal rate_ms = eventNameRateHash.value(key);
            curBusLoadSum += 1000/rate_ms * 111 / baudrate * 100 * (curEventSize/7 + (curEventSize%7>0 ? 1 : 0));
        }
        ui->progressBar_BusLoad->setValue(curBusLoadSum);

    }
    else if (a2lProject->getDaqInfo()->DaqType == "DYNAMIC")
    {
        qreal curEventSizePerc = 0;
        qreal curAllEventSizePerc = 0;
        if (eventNameMaxSizeHash.value(text) != 0)
        {
            qDebug()<<"curSize="<<eventNameTempCurSizeHash.value(text)<<", maxSize="<<eventNameMaxSizeHash.value(text);
            curEventSizePerc = (qreal)(eventNameTempCurSizeHash.value(text)) / (qreal)(eventNameMaxSizeHash.value(text)) * 100;
            qDebug()<<"curPerc"<<curEventSizePerc;

        }
        else
        {
            qreal curSizeSum = 0;
            qreal curBusLoadSum = 0;
            for(int i = 0; i < eventNameTempCurSizeHash.keys().count(); i++)
            {
                QString key = eventNameTempCurSizeHash.keys().at(i);
                int curEventSize = eventNameTempCurSizeHash.value(key);
                curSizeSum += (qreal)curEventSize;

                qreal rate_ms = eventNameRateHash.value(key);

                curBusLoadSum += 1000/rate_ms * 111 / baudrate * 100 * (curEventSize/7+ (curEventSize%7 > 0 ? 1 : 0));
                qDebug()<<"current Key="<<key<<", curEventSize="<<curEventSize<<"cur BusLoadSum="<<curBusLoadSum;
            }
            //curAllEventSizePerc = curSizeSum / (qreal)(a2lProject->getDaqMemoryConsumption()->Daq_Memory_Limit) * 100;
            qDebug()<<"curSizeSum="<<curSizeSum<<", SizeLimit="<<a2lProject->getDaqMemoryConsumption()->Daq_Memory_Limit;
            ui->progressBar_VolumnUsage->setValue(curSizeSum);

            ui->progressBar_BusLoad->setValue(curBusLoadSum);

        }

    }


}

void PAMSELDlg::setProject(A2L_Project *project)
{
    a2lProject = project;
}

void PAMSELDlg::updateAllTempEventCurSize()
{
    eventNameTempCurSizeHash = eventNameCurSizeHash;  //reset

    for(int i = 0; i < tempMeasVarList.count(); i++)
    {
        A2L_VarMeas *measVar = tempMeasVarList.at(i);
        A2L_Event *event = measVar->daqEventRef;
        QString eventName = event->ShortName;
        quint32 size = measVar->DataSizeAG;
        quint32 sizeSum = eventNameTempCurSizeHash.value(eventName);
        quint32 newSizeSum = sizeSum + size;
        eventNameTempCurSizeHash.insert(eventName, newSizeSum);
    }
}

void PAMSELDlg::updateTempEventCurSize(A2L_VarMeas *measVar)
{
    A2L_Event *event = measVar->daqEventRef;
    QString eventName = event->ShortName;
    quint32 size = measVar->DataSizeAG;
    quint32 sizeSum = eventNameTempCurSizeHash.value(eventName);
    quint32 newSizeSum = sizeSum + size;
    eventNameTempCurSizeHash.insert(eventName, newSizeSum);

    ui->comboBox_Event->setCurrentText(eventName);
    eventComboTextChangedSlot(eventName);
}

void PAMSELDlg::removeMeasVarByName(QString measVarName)
{
    for(int i = 0; i < tempMeasVarList.count(); i++)
    {
        A2L_VarMeas *measVar = tempMeasVarList.at(i);
        if (measVar->Name == measVarName)
        {
            tempMeasVarList.removeOne(measVar);

            A2L_Event *event = measVar->daqEventRef;
            if(event)
            {
                QString eventName = event->ShortName;
                quint32 size = measVar->DataSizeAG;
                quint32 sizeSum = eventNameTempCurSizeHash.value(eventName);
                quint32 newSizeSum = sizeSum - size;
                eventNameTempCurSizeHash.insert(eventName, newSizeSum);

                ui->comboBox_Event->setCurrentText(eventName);
                eventComboTextChangedSlot(eventName);
            }

            return;
        }
    }
}

int PAMSELDlg::findSelMeasTableRowByName(QString pamName)
{
    int rowIndex = -1;
    for (int i = 0; i < ui->table_Pams_Selected_Rate->rowCount(); i++)
    {
        QTableWidgetItem *item = ui->table_Pams_Selected_Rate->item(i, 0);
        if (item->text() == pamName)
        {
            rowIndex = i;
            return rowIndex;
        }
    }
    return rowIndex;
}

void PAMSELDlg::selMeasRateChangedSlot(QString text)
{
    if (tempMeasVarList.count() != pamNameMeasSelected.count())
        return;

    QStringList tempPamRateSelected;
    for (int i = 0; i < pamNameMeasSelected.count(); i++)
    {
        QComboBox *box = (QComboBox*)(ui->table_Pams_Selected_Rate->cellWidget(i, 1));
        QString curText = box->currentText();
        tempPamRateSelected.append(curText);

        A2L_VarMeas *measVar = tempMeasVarList.at(i);
        if (measVar->daqEventRef->ShortName != curText)
        {
            A2L_Event *event = a2lProject->findEventByShortName(curText);
            measVar->daqEventRef = event;
        }
    }
    //updateAllTempEventCurSize();

    ui->comboBox_Event->setCurrentText(text);

    eventComboTextChangedSlot(text);

}
