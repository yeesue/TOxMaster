#include "calmanagewin.h"
#include "ui_calmanagewin.h"
#include <QDebug>
#include "exceloperator.h"

CalManageWin::CalManageWin(QWidget *parent, XCPMaster *xcp, A2L_Project *a2lProject) :
    QMainWindow(parent),
    xcpMaster(xcp),
    curA2lProject(a2lProject),
    ui(new Ui::CalManageWin)
{
    ui->setupUi(this);

    setWindowTitle("Calibration Management");

    ui->tableWidget->setColumnCount(7);
    QStringList header;
    header << "Cal Name"<<"Type"<<"Unit"<<"xAxis"<<"yAxis"<<"zValue"<<"Check";
    ui->tableWidget->setHorizontalHeaderLabels(header);

    ui->progressBar->setValue(0);
    ui->progressBar->setMaximum(100);
    ui->progressBar->setVisible(false);
    ui->progressBar->setTextVisible(false);

}

CalManageWin::~CalManageWin()
{
    delete ui;
}

void CalManageWin::on_actionLoad_triggered()
{
    QFileDialog* fd = new QFileDialog(this);
    QString appPath = QApplication::applicationDirPath();
    QString filePath = fd->getOpenFileName(this,
                                           tr("Open File"),appPath,tr("Excel(*.csv)"));
    if(filePath.isEmpty())
        return;

    CaliCsv *caliCsv = new CaliCsv(this, filePath);

    this->calList = caliCsv->getCalList();
    //qDebug()<<"cal pam count:"<<calList.count();

    showCalListInTable();
}

void CalManageWin::SltLoadingProgress(int percentage)
{

    ui->progressBar->setValue(percentage);
    if(percentage >= 100)
        ui->progressBar->setVisible(false);
}

void CalManageWin::showCalListInTable()
{
    ui->tableWidget->clearContents();
    ui->tableWidget->setRowCount(calList.count());
    for(int i = 0; i < calList.count(); i++)
    {
        CalInfo calIns = calList.at(i);
        QTableWidgetItem *nameItem = new QTableWidgetItem(calIns.charName);
        QTableWidgetItem *typeItem = new QTableWidgetItem(calIns.type);
        QTableWidgetItem *unitItem = new QTableWidgetItem(calIns.unit);

        QTableWidgetItem *xAxisItem = new QTableWidgetItem(calIns.xAxisName);
        QTableWidgetItem *yAxisItem = new QTableWidgetItem(calIns.yAxisName);

        QString xAxis = calIns.xAxisStrArray.join(",");
        QString yAxis = calIns.yAxisStrArray.join(",");
        QString zValue = calIns.zValueStrArray.join(",");
        QTableWidgetItem *zValueItem = new QTableWidgetItem(zValue);

        ui->tableWidget->setItem(i, 0, nameItem);
        ui->tableWidget->setItem(i, 1, typeItem);
        ui->tableWidget->setItem(i, 2, unitItem);
        ui->tableWidget->setItem(i, 3, xAxisItem);
        ui->tableWidget->setItem(i, 4, yAxisItem);
        ui->tableWidget->setItem(i, 5, zValueItem);
    }
    ui->tableWidget->resizeColumnsToContents();
}

void CalManageWin::showCharVarsInTable()
{
    ui->tableWidget->clearContents();
    ui->tableWidget->setRowCount(charVarList.count());
    for(int i = 0; i < charVarList.count(); i++)
    {
        A2L_VarChar *charVar = charVarList.at(i);
        QTableWidgetItem *nameItem = new QTableWidgetItem(charVar->Name);
        QTableWidgetItem *typeItem = new QTableWidgetItem(charVar->Type);
        QTableWidgetItem *unitItem = new QTableWidgetItem(charVar->Unit);

        QTableWidgetItem *xAxisItem = new QTableWidgetItem(charVar->Axis_X_Ref);
        QTableWidgetItem *yAxisItem = new QTableWidgetItem(charVar->Axis_Y_Ref);

        QString xAxis = "";
        for(int i = 0; i < charVar->xCount; i++)
        {
            xAxis += QString::number(charVar->hexValue_XList.at(i)) + ((i+1) == charVar->xCount ? "" : ",");
        }
        QString yAxis = "";
        for(int i = 0; i < charVar->yCount; i++)
        {
            yAxis += QString::number(charVar->hexValue_YList.at(i)) + ((i+1) == charVar->yCount ? "" : ",");
        }

        QString zValues = "";
        for(int i = 0; i < charVar->zCount; i++)
        {
            zValues += QString::number(charVar->hexValue_ZList.at(i)) + ((i+1) == charVar->zCount ? "" : ",");
        }

        QString blkValues = "";
        for(int i = 0; i < charVar->number_BLK; i++)
        {
            blkValues += QString::number(charVar->Blk_Values.at(i)) + ((i+1) == charVar->number_BLK ? "" : ",");
        }

        QString zValue = "";
        if(charVar->charType == "VALUE")
        {
            zValue = QString::number(charVar->getValue());
        }
        else if(charVar->charType == "VAL_BLK")
        {
            zValue = blkValues;
        }
        else
        {
            zValue = zValues;
        }
        QTableWidgetItem *zValueItem = new QTableWidgetItem(zValue);

        ui->tableWidget->setItem(i, 0, nameItem);
        ui->tableWidget->setItem(i, 1, typeItem);
        ui->tableWidget->setItem(i, 2, unitItem);
        ui->tableWidget->setItem(i, 3, xAxisItem);
        ui->tableWidget->setItem(i, 4, yAxisItem);
        ui->tableWidget->setItem(i, 5, zValueItem);
    }
    ui->tableWidget->resizeColumnsToContents();
}

QList<A2L_VarChar*> CalManageWin::getCharVarListFromCalInfoList(QList<CalInfo> calList)
{
    QList<A2L_VarChar*> charVarList;
    if(curA2lProject == NULL)
        return charVarList;

    for(int i = 0; i < calList.count(); i++)
    {
        CalInfo calInfo = calList.at(i);

        qDebug()<<"cal index:"<<i;
        if(charTypeList.indexOf(calInfo.type) == -1)
            continue;

        A2L_VarChar *charVar = curA2lProject->getCharVarByName(calInfo.charName);

        if(!charVar)
            continue;

        if(charVar->charType == "VALUE")
        {
            QString valueStr = calInfo.zValueStrArray.at(0);
            bool ok = false;
            double value = valueStr.toDouble(&ok);

            charVar->setValue(ok ? value : 0);

            if(charVar->ConversionType == "TAB_VERB" && !charVar->ConversionVTab.isEmpty())
            {
                QString valuePairStr = charVar->ConversionVTab;
                QStringList valuePairList = valuePairStr.split(",");
                foreach (QString valuePair, valuePairList) {
                    int index = valuePair.indexOf(valueStr);
                    if(index != -1)
                    {
                        QString str = valuePair.split("-").first();
                        value = str.toDouble();
                        charVar->setValue(value);
                    }
                }
            }
        }
        else if(charVar->charType == "VAL_BLK")
        {
            QStringList blkValueStr = calInfo.zValueStrArray;
            bool ok = false;
            for(int i = 0; i < blkValueStr.count(); i++)
            {
                QString valueStr = blkValueStr.at(i);
                double value = valueStr.toDouble(&ok);
                if(i < charVar->number_BLK && ok)
                {
                    charVar->setBlkValue(i, 1, value);
                }
            }

            qDebug()<<"~~~Set Blk value[0]" << charVar->Blk_Values[0] << ", blk count:"<<charVar->Blk_Values.count();
        }
        else if(charVar->charType == "CURVE")
        {
            qDebug()<<"~~~from hex:curve xAxis Value:"<<charVar->hexValue_XList;
            qDebug()<<"~~~from hex:curve zValue:"<<charVar->hexValue_ZList;
            qDebug()<<"~~~from calInfo:curve xAxis Value:"<<calInfo.xAxisStrArray;
            qDebug()<<"~~~from calInfo:curve zValue:"<<calInfo.zValueStrArray;

            QStringList zValueStrList = calInfo.zValueStrArray;
            QStringList xValueStrList = calInfo.xAxisStrArray;
            bool ok1 = false;
            bool ok2 = false;
            for(int i = 0; i < zValueStrList.count(); i++)
            {
                QString valueStr = zValueStrList.at(i);
                QString xAxisStr = xValueStrList.at(i);
                double value = valueStr.toDouble(&ok1);
                if(i < charVar->zCount && ok1)
                {
                    charVar->hexValue_ZList.replace(i, value);
                }
                double xValue = xAxisStr.toDouble(&ok2);
                if(i < charVar->xCount && ok2)
                {
                    charVar->hexValue_XList.replace(i, xValue);
                }
            }

            qDebug()<<"~~~from cal csv:curve xAxis Value:"<<charVar->hexValue_XList;
            qDebug()<<"~~~from cal csv:curve zValue:"<<charVar->hexValue_ZList;
        }
        else if(charVar->charType == "MAP")
        {

        }
        else
        {

        }

        charVarList.append(charVar);
    }
    return charVarList;
}

bool CalManageWin::downloadCalList(QList<CalInfo> calList)
{
    if(calList.isEmpty() || curA2lProject == NULL || xcpMaster == NULL)
        return false;

    QList<A2L_VarChar*> charVarList = getCharVarListFromCalInfoList(calList);

    if(!xcpMaster->getXcpSessionConnectedStatus())
        return false;

    qDebug()<<"=====Begine to download cal list.=====";
    ui->progressBar->setVisible(true);
    ui->progressBar->setMaximum(100);
    int max = charVarList.count();
    for(int i = 0; i < max; i++)
    {
        A2L_VarChar *charVar = charVarList.at(i);
        if(charVar->charType == "VALUE")
        {
            if(!xcpMaster->XCP_Cal_VALUE(charVar))
            {
                qDebug()<<"VALUE Charac:"<<charVar->Name<<", calibrate error.";
            }
            else
            {
                qDebug()<<"VALUE Charac:"<<charVar->Name<<", download ok: "<<charVar->getValue();
            }
        }
        else if(charVar->charType == "VAL_BLK")
        {
            if(!xcpMaster->XCP_Cal_VAL_BLK(charVar))
            {
                qDebug()<<"VAL_BLK Charac:"<<charVar->Name<<", calibrate error.";
            }
            else
            {
                qDebug()<<"VAL_BLK Charac:"<<charVar->Name<<", download ok: "<<charVar->Blk_Values;
            }
        }
        else if(charVar->charType == "CURVE")
        {
            if(!xcpMaster->XCP_Cal_CURVE(charVar))
            {
                qDebug()<<"CURVE Charac:"<<charVar->Name<<", calibrate error.";
            }
            else
            {
                qDebug()<<"CURVE Charac:"<<charVar->Name<<", download ok. zValues:"<<charVar->hexValue_ZList;
            }
        }
        else
        {

        }

        int percentage = ((float)(i+1))/max*100;
        ui->progressBar->setValue(percentage);
    }

    return true;
}

bool CalManageWin::downloadCalChars(QList<A2L_VarChar *> charVarList)
{
    if(xcpMaster == NULL || charVarList.isEmpty())
        return false;
    if(!xcpMaster->getXcpSessionConnectedStatus())
        return false;

    qDebug()<<"=====Begine to download charVar list.=====";
    ui->progressBar->setVisible(true);
    ui->progressBar->setMaximum(100);
    int max = charVarList.count();
    for(int i = 0; i < max; i++)
    {
        A2L_VarChar *charVar = charVarList.at(i);
        if(charVar->charType == "VALUE")
        {
            if(!xcpMaster->XCP_Cal_VALUE(charVar))
            {
                qDebug()<<"VALUE Charac:"<<charVar->Name<<", calibrate error.";
            }
            else
            {
                qDebug()<<"VALUE Charac:"<<charVar->Name<<", download ok: "<<charVar->getValue();
            }
        }
        else if(charVar->charType == "VAL_BLK")
        {
            if(!xcpMaster->XCP_Cal_VAL_BLK(charVar))
            {
                qDebug()<<"VAL_BLK Charac:"<<charVar->Name<<", calibrate error.";
            }
            else
            {
                qDebug()<<"VAL_BLK Charac:"<<charVar->Name<<", download ok: "<<charVar->Blk_Values;
            }
        }
        else if(charVar->charType == "CURVE")
        {
            if(!xcpMaster->XCP_Cal_CURVE(charVar))
            {
                qDebug()<<"CURVE Charac:"<<charVar->Name<<", calibrate error.";
            }
            else
            {
                qDebug()<<"CURVE Charac:"<<charVar->Name<<", download ok. zValues:"<<charVar->hexValue_ZList;
            }
        }
        else
        {

        }

        int percentage = ((float)(i+1))/max*100;
        ui->progressBar->setValue(percentage);
    }

    return true;
}

bool CalManageWin::delExcel(QString filePath)
{
    QFileInfo excelFile(filePath);
    if(excelFile.isFile())
    {
        if(!QFile::remove(filePath))
        {
            qDebug()<<"Delete excel file error: "<<filePath;
            return false;
        }
    }

    return excelFile.isFile();
}

void CalManageWin::convertToColName(int data, QString &res)
{
    Q_ASSERT(data>0 && data<65535);
    int tempData = data / 26;
    if(tempData > 0)
    {
        int mode = data % 26;
        convertToColName(mode,res);
        convertToColName(tempData,res);
    }
    else
    {
        res=(to26AlphabetString(data)+res);
    }
}

QString CalManageWin::to26AlphabetString(int data)
{
    QChar ch = data + 0x40;//A???0x41
    return QString(ch);
}

bool CalManageWin::exportCalsToExcel(QList<A2L_VarChar *> charVars, QString excelPath)
{
    ExcelOperator *charExcel = new ExcelOperator();
    charExcel->open(excelPath);
    QAxObject *sheet = charExcel->getSheet("Sheet1");
    if(sheet == NULL)
    {
        return false;
    }

    if(charVars.isEmpty())
        return false;

    qDebug()<<"export project cal pams to excel......";
    ui->le_Log->setText("export project cal pams to excel......");
    QList<QList<QVariant>> charVariants;
    for(int i = 0; i < charVars.count(); i++)
    {
        QList<QVariant> variant;
        A2L_VarChar *charVar = charVars.at(i);

        variant.append(QVariant::fromValue(charVar->Name));
        variant.append(QVariant::fromValue(charVar->Type));
        variant.append(QVariant::fromValue(charVar->DataType));
        variant.append(QVariant::fromValue(charVar->ScalingFactor));
        variant.append(QVariant::fromValue(charVar->ScalingOffset));
        variant.append(QVariant::fromValue(charVar->Unit));
        variant.append(QVariant::fromValue(charVar->ECUAddress));
        variant.append(QVariant::fromValue(charVar->DataSizeAG));
        variant.append(QVariant::fromValue(charVar->LowerLimit));
        variant.append(QVariant::fromValue(charVar->UpperLimit));
        variant.append(QVariant::fromValue(charVar->ConversionType));
        variant.append(QVariant::fromValue(charVar->COEFFS));

        if(charVar->charType == "VALUE")
        {
            variant.append(QVariant::fromValue(charVar->getValue()));
        }
        else if(charVar->charType == "VAL_BLK")
        {
            QString str;
            for(int i = 0; i < charVar->number_BLK; i++)
            {
                //qDebug()<<QString::number(charVar->Blk_Values.at(i));
                str += (QString::number(charVar->Blk_Values.at(i), 'f', 6) + ((i+1) == charVar->number_BLK ? "" : ","));
            }
            variant.append(QVariant::fromValue(str));
        }
        else if(charVar->charType == "CURVE")
        {
            QString str;
            for(int i = 0; i < charVar->zCount; i++)
            {
                str += (QString::number(charVar->hexValue_ZList.at(i)) + ( (i+1) == charVar->zCount ? "" : ","));
            }
            variant.append(QVariant::fromValue(str));
        }
        else if(charVar->charType == "MAP")
        {
            QString str;
            for(int i = 0; i < charVar->zCount; i++)
            {
                str += (QString::number(charVar->hexValue_ZList.at(i)) + ( (i+1) == charVar->zCount ? "" : ","));
            }
            variant.append(QVariant::fromValue(str));
        }
        else
        {
            variant.append(QVariant::fromValue(charVar->getValue()));
        }

        variant.append(QVariant::fromValue(charVar->Axis_X_Ref));
        variant.append(QVariant::fromValue(charVar->Axis_X_Type));
        variant.append(QVariant::fromValue(charVar->Axis_X_DataType));
        variant.append(QVariant::fromValue(charVar->Axis_X_ScalingFactor));
        variant.append(QVariant::fromValue(charVar->Axis_X_ScalingOffset));
        variant.append(QVariant::fromValue(charVar->Axis_X_Unit));
        variant.append(QVariant::fromValue(charVar->Axis_X_ECUAddress));
        variant.append(QVariant::fromValue(charVar->Axis_X_DataSizeAG));
        variant.append(QVariant::fromValue(charVar->Axis_X_LowerLimit));
        variant.append(QVariant::fromValue(charVar->Axis_X_UpperLimit));
        variant.append(QVariant::fromValue(charVar->Axis_X_ConversionType));
        variant.append(QVariant::fromValue(charVar->Axis_X_COEFFS));

        QString xStr;
        for(int i = 0; i < charVar->xCount; i++)
        {
            xStr += QString::number(charVar->hexValue_XList.at(i)) + ( (i+1) == charVar->xCount ? "" : ",");
        }
        variant.append(QVariant::fromValue(xStr));

        variant.append(QVariant::fromValue(charVar->Axis_Y_Ref));
        variant.append(QVariant::fromValue(charVar->Axis_Y_Type));
        variant.append(QVariant::fromValue(charVar->Axis_Y_DataType));
        variant.append(QVariant::fromValue(charVar->Axis_Y_ScalingFactor));
        variant.append(QVariant::fromValue(charVar->Axis_Y_ScalingOffset));
        variant.append(QVariant::fromValue(charVar->Axis_Y_Unit));
        variant.append(QVariant::fromValue(charVar->Axis_Y_ECUAddress));
        variant.append(QVariant::fromValue(charVar->Axis_Y_DataSizeAG));
        variant.append(QVariant::fromValue(charVar->Axis_Y_LowerLimit));
        variant.append(QVariant::fromValue(charVar->Axis_Y_UpperLimit));
        variant.append(QVariant::fromValue(charVar->Axis_Y_ConversionType));
        variant.append(QVariant::fromValue(charVar->Axis_Y_COEFFS));

        QString yStr = "";
        for(int i = 0; i < charVar->yCount; i++)
        {
            yStr += QString::number(charVar->hexValue_YList.at(i)) + ( (i+1) == charVar->yCount ? "" : ",");
        }
        variant.append(QVariant::fromValue(yStr));

        QString endStr = "end";
        variant.append(QVariant::fromValue(endStr));

        charVariants.append(variant);
    }

    int row = charVariants.size();
    int col = charVariants.at(0).size();

    QString rangStr;
    convertToColName(col,rangStr);
    rangStr += QString::number(row);
    rangStr = "A1:" + rangStr;
    qDebug()<<rangStr;

    QAxObject *range = sheet->querySubObject("Range(const QString&)",rangStr);
    if(NULL == range || range->isNull())
    {
        return false;
    }
    bool succ = false;
    QVariant var;
    QVariantList vars;
    for(int i = 0; i < row; i++)
    {
        vars.append(QVariant(charVariants[i]));
    }
    var = QVariant(vars);
    succ = range->setProperty("Value", var);
    delete range;

    charExcel->close();
    qDebug()<<"export project cal pams to excel end";
    ui->le_Log->setText("export project cal pams to excel end");
    return true;
}

void CalManageWin::importCalsFromExcel(QString excelPath)
{
    ExcelOperator *charExcel = new ExcelOperator();
    charExcel->open(excelPath);
    QAxObject *sheet = charExcel->getSheet("Sheet1");
    if(sheet == NULL)
    {
        qDebug()<<"import cals: pams excel sheet not found!";
        ui->le_Log->setText("import cals: pams excel sheet not found!");
        return;
    }

    qDebug()<<"import cal pams from excel......";
    ui->le_Log->setText("import cal pams from excel......");

    QAxObject *usedRange=sheet->querySubObject("UsedRange");
    QVariant var=usedRange->dynamicCall("value");

    int startBitIndex = 0;
    int startByteIndex = 0;
    startBitIndex += 0;

    qDeleteAll(this->charVarList);
    this->charVarList.clear();

    QVariantList varRowContents=var.toList();
    const int rowCount=varRowContents.size();
    QVariantList tmp;
    for(int i=0; i<rowCount; ++i)
    {
        tmp=varRowContents[i].toList();

        A2L_VarChar *charVar = new A2L_VarChar();
        charVar->Name = tmp[0].toString();
        charVar->ShortName = tmp[0].toString();
        charVar->charType = tmp[1].toString();
        charVar->DataType = tmp[2].toString();
        charVar->ScalingFactor = tmp[3].toDouble();
        charVar->ScalingOffset = tmp[4].toDouble();
        charVar->Unit = tmp[5].toString();
        charVar->ECUAddress = (quint32)(tmp[6].toInt());
        charVar->DataSizeAG = (quint32)(tmp[7].toInt());
        charVar->LowerLimit = tmp[8].toDouble();
        charVar->UpperLimit = tmp[9].toDouble();
        charVar->ConversionType = tmp[10].toString();
        charVar->COEFFS = tmp[11].toString();

        if(charVar->charType == "VALUE")
        {
            charVar->setValue(tmp[12].toDouble());
        }
        else if(charVar->charType == "VAL_BLK")
        {
            QStringList blkValuesList = tmp[12].toString().split(",");
            QList<qreal> valueList;
            foreach (QString valueStr, blkValuesList) {
                valueList.append(valueStr.toDouble());
            }
            charVar->Blk_Values = valueList;
            charVar->number_BLK = valueList.count();
        }
        else if(charVar->charType == "CURVE")
        {
            QStringList zValuesList = tmp[12].toString().split(",");
            QList<qreal> valueList;
            foreach (QString valueStr, zValuesList) {
                valueList.append(valueStr.toDouble());
            }
            charVar->hexValue_ZList = valueList;
            charVar->zCount = valueList.count();
        }
        else if(charVar->charType == "MAP")
        {
            QStringList zValuesList = tmp[12].toString().split(",");
            QList<qreal> valueList;
            foreach (QString valueStr, zValuesList) {
                valueList.append(valueStr.toDouble());
            }
            charVar->hexValue_ZList = valueList;
            charVar->zCount = valueList.count();
        }
        else
        {
            charVar->setValue(tmp[12].toDouble());
        }

        charVar->Axis_X_Ref = tmp[13].toString();
        charVar->Axis_X_Type = tmp[14].toString();
        charVar->Axis_X_DataType = tmp[15].toString();
        charVar->Axis_X_ScalingFactor = tmp[16].toDouble();
        charVar->Axis_X_ScalingOffset = tmp[17].toDouble();
        charVar->Axis_X_Unit = tmp[18].toString();
        charVar->Axis_X_ECUAddress = (quint32)(tmp[19].toInt());
        charVar->Axis_X_DataSizeAG = (quint32)(tmp[20].toInt());
        charVar->Axis_X_LowerLimit = tmp[21].toDouble();
        charVar->Axis_X_UpperLimit = tmp[22].toDouble();
        charVar->Axis_X_ConversionType = tmp[23].toString();
        charVar->Axis_X_COEFFS = tmp[24].toString();

        QStringList xValueStrList = tmp[25].toString().split(",");
        QList<qreal> xValueList;
        foreach (QString valueStr, xValueStrList) {
            xValueList.append(valueStr.toDouble());
        }
        charVar->hexValue_XList = xValueList;
        charVar->xCount = xValueList.count();

        charVar->Axis_Y_Ref = tmp[26].toString();
        charVar->Axis_Y_Type = tmp[27].toString();
        charVar->Axis_Y_DataType = tmp[28].toString();
        charVar->Axis_Y_ScalingFactor = tmp[29].toDouble();
        charVar->Axis_Y_ScalingOffset = tmp[30].toDouble();
        charVar->Axis_Y_Unit = tmp[31].toString();
        charVar->Axis_Y_ECUAddress = (quint32)(tmp[32].toInt());
        charVar->Axis_Y_DataSizeAG = (quint32)(tmp[33].toInt());
        charVar->Axis_Y_LowerLimit = tmp[34].toDouble();
        charVar->Axis_Y_UpperLimit = tmp[35].toDouble();
        charVar->Axis_Y_ConversionType = tmp[36].toString();
        charVar->Axis_Y_COEFFS = tmp[37].toString();

        QStringList yValueStrList = tmp[38].toString().split(",");
        QList<qreal> yValueList;
        foreach (QString valueStr, yValueStrList) {
            yValueList.append(valueStr.toDouble());
        }
        charVar->hexValue_YList = yValueList;
        charVar->yCount = yValueList.count();

        QStringList coefStrList = charVar->COEFFS.split(",");
        for (int i = 0; i < coefStrList.count(); i++)
        {
            QString str = coefStrList.at(i);
            charVar->CoefList.append(str.toDouble());
        }

        QStringList xCoefStrList = charVar->Axis_X_COEFFS.split(",");
        for (int i = 0; i < xCoefStrList.count(); i++)
        {
            QString str = xCoefStrList.at(i);
            charVar->Axis_X_CoefList.append(str.toDouble());
        }

        QStringList yCoefStrList = charVar->Axis_Y_COEFFS.split(",");
        for (int i = 0; i < yCoefStrList.count(); i++)
        {
            QString str = yCoefStrList.at(i);
            charVar->Axis_Y_CoefList.append(str.toDouble());
        }
        startBitIndex += (charVar->DataSizeAG * 8);
        startByteIndex = startBitIndex/8;

        charVarList.append(charVar);
        qDebug()<<"i:"<<i<<", ===import char name:"<<charVar->Name<<", type:"<<charVar->Type<<", value:"<<charVar->getValue()
               <<", blkValues:"<<charVar->Blk_Values
              <<", xValues:"<<charVar->hexValue_XList
        <<", zValues:"<<charVar->hexValue_ZList;
    }

    charExcel->close();
    qDebug()<<"import cals pams from excel end.";
    ui->le_Log->setText("import cals pams from excel end");

}

void CalManageWin::setXcpMainThread(XCP_Main_Thread *value)
{
    xcpMainThread = value;
}

QList<A2L_VarChar *> CalManageWin::getDownloadCharList() const
{
    return charVarList;
}

void CalManageWin::setCurA2lProject(A2L_Project *value)
{
    curA2lProject = value;
}

void CalManageWin::setXcpMaster(XCPMaster *value)
{
    xcpMaster = value;
}

void CalManageWin::on_actionDownload_triggered()
{
    //downloadCalList(this->calList);
    QList<A2L_VarChar*> charList = getCharVarListFromCalInfoList(this->calList);
    if(xcpMainThread)
    {
        xcpMainThread->setDownloadCharList(charList);
        xcpMainThread->setCharsDownloadActive();
    }
}

void CalManageWin::on_actionExport_triggered()
{

    if(calList.isEmpty() || curA2lProject == NULL || xcpMaster == NULL)
        return;

    QList<A2L_VarChar*> charVarList = getCharVarListFromCalInfoList(calList);

    QString calExcelPath = QApplication::applicationDirPath() + "/xcp_cals/calibration.xlsx";

    if(calExcelPath.isEmpty())
        return;

    delExcel(calExcelPath);
    exportCalsToExcel(charVarList, calExcelPath);
}

void CalManageWin::on_actionImport_triggered()
{
    QString calExcelPath = QApplication::applicationDirPath() + "/xcp_cals/calibration.xlsx";
    importCalsFromExcel(calExcelPath);

    showCharVarsInTable();
}

void CalManageWin::on_actionDownload_Import_triggered()
{
    downloadCalChars(this->charVarList);
}
