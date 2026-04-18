#include "import_export_service.h"

#include <QApplication>
#include <QAxObject>
#include <QDebug>
#include <QFile>
#include <QFileInfo>
#include <QVariant>

#include "a2l_varmeas.h"
#include "a2l_varchar.h"
#include "db_testbed.h"
#include "exceloperator.h"
#include "xcp_error.h"

ImportExportService::ImportExportService(QObject *parent)
    : QObject(parent)
{
}

void ImportExportService::clearPamSet(PamSet &set) const
{
    qDeleteAll(set.measPamList);
    set.measPamList.clear();

    qDeleteAll(set.daqMeasPamList);
    set.daqMeasPamList.clear();

    qDeleteAll(set.charPamList);
    set.charPamList.clear();

    set.sizeRead = 0;
    set.sizeReadDAQ = 0;
    set.sizeWrite = 0;
    set.smKeyRead.clear();
    set.smKeyReadDAQ.clear();
    set.smKeyWrite.clear();
}

bool ImportExportService::exportPamSetToExcel(const PamSet &set) const
{
    const QString basePath = QApplication::applicationDirPath() + "/xcp_pams/";
    const QString suffix = set.secondChannel ? "_2nd" : "";

    const QString measExcelPathPoll = basePath + "measurements_poll" + suffix + ".xlsx";
    delExcel(measExcelPathPoll);
    exportMeasToExcel(set.measPamList, measExcelPathPoll);

    const QString measExcelPathDaq = basePath + "measurements_daq" + suffix + ".xlsx";
    delExcel(measExcelPathDaq);
    exportMeasToExcel(set.daqMeasPamList, measExcelPathDaq);

    const QString charExcelPath = basePath + "characteristics" + suffix + ".xlsx";
    delExcel(charExcelPath);
    exportCharsToExcel(set.charPamList, charExcelPath);

    return true;
}

bool ImportExportService::loadPamSetFromExcel(PamSet &set) const
{
    clearPamSet(set);

    const QString basePath = QApplication::applicationDirPath() + "/xcp_pams/";
    const QString suffix = set.secondChannel ? "_2nd" : "";

    importMeasFromExcel(basePath + "measurements_poll" + suffix + ".xlsx", set.measPamList, set.sizeRead);
    set.smKeyRead = "RP_POLL_" + set.projectName;

    importMeasFromExcel(basePath + "measurements_daq" + suffix + ".xlsx", set.daqMeasPamList, set.sizeReadDAQ);
    set.smKeyReadDAQ = "RP_DAQ_" + set.projectName;

    importCharsFromExcel(basePath + "characteristics" + suffix + ".xlsx", set.charPamList, set.sizeWrite);
    set.smKeyWrite = "WP_" + set.projectName;

    return true;
}

bool ImportExportService::loadPamSetFromDb(PamSet &set, DB_Testbed *&db) const
{
    if (db == nullptr) {
        db = new DB_Testbed();
    }

    qDeleteAll(set.measPamList);
    set.measPamList.clear();
    set.sizeRead = 0;

    qDeleteAll(set.charPamList);
    set.charPamList.clear();
    set.sizeWrite = 0;

    set.measPamList = db->getMeasPams(set.smKeyRead, set.sizeRead);
    set.charPamList = db->getCharPams(set.smKeyWrite, set.sizeWrite);
    return true;
}

bool ImportExportService::savePamSetToDb(const PamSet &set, DB_Testbed *&db) const
{
    if (db == nullptr) {
        db = new DB_Testbed();
    }

    return db->saveMeasPams(set.measPamList) && db->saveCharPams(set.charPamList);
}

bool ImportExportService::exportMeasToExcel(const QList<A2L_VarMeas *> &meaVars, const QString &excelPath) const
{
    ExcelOperator *measExcel = new ExcelOperator();
    measExcel->open(excelPath);
    QAxObject *sheet = measExcel->getSheet("Sheet1");
    if (sheet == nullptr || meaVars.isEmpty()) {
        delete measExcel;
        return false;
    }

    QList<QList<QVariant>> measVariants;
    for (A2L_VarMeas *measVar : meaVars) {
        QList<QVariant> variant;
        variant.append(QVariant::fromValue(measVar->Name));
        variant.append(QVariant::fromValue(measVar->ShortName));
        variant.append(QVariant::fromValue(measVar->DataType));
        variant.append(QVariant::fromValue(measVar->ScalingFactor));
        variant.append(QVariant::fromValue(measVar->ScalingOffset));
        variant.append(QVariant::fromValue(measVar->Unit));
        variant.append(QVariant::fromValue(measVar->ECUAddress));
        variant.append(QVariant::fromValue(measVar->DataSizeAG));
        variant.append(QVariant::fromValue(measVar->LowerLimit));
        variant.append(QVariant::fromValue(measVar->UpperLimit));
        variant.append(QVariant::fromValue(measVar->getValue()));
        variant.append(QVariant::fromValue(measVar->ConversionType));
        variant.append(QVariant::fromValue(measVar->COEFFS));
        variant.append(QVariant::fromValue(measVar->StartByte));
        variant.append(QVariant::fromValue(measVar->rate_ms));
        variant.append(QVariant::fromValue(measVar->smKey));
        measVariants.append(variant);
    }

    const int row = measVariants.size();
    const int col = measVariants.at(0).size();

    QString rangeStr;
    convertToColName(col, rangeStr);
    rangeStr += QString::number(row);
    rangeStr = "A1:" + rangeStr;

    QAxObject *range = sheet->querySubObject("Range(const QString&)", rangeStr);
    if (range == nullptr || range->isNull()) {
        measExcel->close();
        delete measExcel;
        return false;
    }

    QVariantList vars;
    for (int i = 0; i < row; ++i) {
        vars.append(QVariant(measVariants[i]));
    }
    range->setProperty("Value", QVariant(vars));

    delete range;
    measExcel->close();
    delete measExcel;
    return true;
}

bool ImportExportService::exportCharsToExcel(const QList<A2L_VarChar *> &charVars, const QString &excelPath) const
{
    ExcelOperator *charExcel = new ExcelOperator();
    charExcel->open(excelPath);
    QAxObject *sheet = charExcel->getSheet("Sheet1");
    if (sheet == nullptr || charVars.isEmpty()) {
        delete charExcel;
        return false;
    }

    QList<QList<QVariant>> charVariants;
    for (A2L_VarChar *charVar : charVars) {
        QList<QVariant> variant;
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
        variant.append(QVariant::fromValue(charVar->getValue()));
        variant.append(QVariant::fromValue(charVar->ConversionType));
        variant.append(QVariant::fromValue(charVar->COEFFS));
        variant.append(QVariant::fromValue(charVar->StartByte));
        variant.append(QVariant::fromValue(charVar->smKey));
        charVariants.append(variant);
    }

    const int row = charVariants.size();
    const int col = charVariants.at(0).size();

    QString rangeStr;
    convertToColName(col, rangeStr);
    rangeStr += QString::number(row);
    rangeStr = "A1:" + rangeStr;

    QAxObject *range = sheet->querySubObject("Range(const QString&)", rangeStr);
    if (range == nullptr || range->isNull()) {
        charExcel->close();
        delete charExcel;
        return false;
    }

    QVariantList vars;
    for (int i = 0; i < row; ++i) {
        vars.append(QVariant(charVariants[i]));
    }
    range->setProperty("Value", QVariant(vars));

    delete range;
    charExcel->close();
    delete charExcel;
    return true;
}

void ImportExportService::importMeasFromExcel(const QString &excelPath, QList<A2L_VarMeas *> &measList, quint64 &byteSize) const
{
    ExcelOperator *measExcel = new ExcelOperator();
    measExcel->open(excelPath);
    QAxObject *sheet = measExcel->getSheet("Sheet1");
    if (sheet == nullptr) {
        qDebug() << "import meas: pams excel sheet not found!";
        delete measExcel;
        return;
    }

    QAxObject *usedRange = sheet->querySubObject("UsedRange");
    QVariant var = usedRange->dynamicCall("value");

    int startBitIndex = 0;
    QVariantList varRowContents = var.toList();
    const int rowCount = varRowContents.size();

    for (int i = 0; i < rowCount; ++i) {
        const QVariantList tmp = varRowContents[i].toList();

        A2L_VarMeas *measVar = new A2L_VarMeas();
        measVar->Name = tmp[0].toString();
        measVar->ShortName = tmp[1].toString();
        measVar->DataType = tmp[2].toString();
        measVar->ScalingFactor = tmp[3].toDouble();
        measVar->ScalingOffset = tmp[4].toDouble();
        measVar->Unit = tmp[5].toString();
        measVar->ECUAddress = static_cast<quint32>(tmp[6].toInt());
        measVar->DataSizeAG = static_cast<quint32>(tmp[7].toInt());
        measVar->LowerLimit = tmp[8].toDouble();
        measVar->UpperLimit = tmp[9].toDouble();
        measVar->ConversionType = tmp[11].toString();
        measVar->COEFFS = tmp[12].toString();
        measVar->StartByte = tmp[13].toInt();
        measVar->rate_ms = tmp[14].toFloat();
        measVar->smKey = tmp[15].toString();

        const QStringList coefStrList = measVar->COEFFS.split(",");
        for (const QString &str : coefStrList) {
            measVar->CoefList.append(str.toDouble());
        }

        startBitIndex += (measVar->DataSizeAG * 8);
        measList.append(measVar);
    }

    byteSize = startBitIndex / 8;

    measExcel->close();
    delete measExcel;
}

void ImportExportService::importCharsFromExcel(const QString &excelPath, QList<A2L_VarChar *> &charList, quint64 &byteSize) const
{
    ExcelOperator *charExcel = new ExcelOperator();
    charExcel->open(excelPath);
    QAxObject *sheet = charExcel->getSheet("Sheet1");
    if (sheet == nullptr) {
        qDebug() << "import char: pams excel sheet not found!";
        delete charExcel;
        return;
    }

    QAxObject *usedRange = sheet->querySubObject("UsedRange");
    QVariant var = usedRange->dynamicCall("value");

    int startBitIndex = 0;
    QVariantList varRowContents = var.toList();
    const int rowCount = varRowContents.size();

    for (int i = 0; i < rowCount; ++i) {
        const QVariantList tmp = varRowContents[i].toList();

        A2L_VarChar *charVar = new A2L_VarChar();
        charVar->Name = tmp[0].toString();
        charVar->charType = tmp[1].toString();
        charVar->DataType = tmp[2].toString();
        charVar->ScalingFactor = tmp[3].toDouble();
        charVar->ScalingOffset = tmp[4].toDouble();
        charVar->Unit = tmp[5].toString();
        charVar->ECUAddress = static_cast<quint32>(tmp[6].toInt());
        charVar->DataSizeAG = static_cast<quint32>(tmp[7].toInt());
        charVar->LowerLimit = tmp[8].toDouble();
        charVar->UpperLimit = tmp[9].toDouble();
        charVar->setValue(tmp[10].toDouble());
        charVar->ConversionType = tmp[11].toString();
        charVar->COEFFS = tmp[12].toString();
        charVar->StartByte = tmp[13].toInt();
        charVar->smKey = tmp[14].toString();

        const QStringList coefStrList = charVar->COEFFS.split(",");
        for (const QString &str : coefStrList) {
            charVar->CoefList.append(str.toDouble());
        }

        startBitIndex += (charVar->DataSizeAG * 8);
        charList.append(charVar);
    }

    byteSize = startBitIndex / 8;

    charExcel->close();
    delete charExcel;
}

bool ImportExportService::delExcel(const QString &filePath) const
{
    QFileInfo excelFile(filePath);
    if (excelFile.isFile() && !QFile::remove(filePath)) {
        qDebug() << XCPErrorRegistry::instance().formatError(
            ERR_COMMUNICATION,
            QString("Delete excel file error: %1").arg(filePath));
        return false;
    }

    return true;
}

void ImportExportService::convertToColName(int data, QString &res) const
{
    Q_ASSERT(data > 0 && data < 65535);
    const int tempData = data / 26;
    if (tempData > 0) {
        const int mode = data % 26;
        convertToColName(mode, res);
        convertToColName(tempData, res);
    } else {
        res = (to26AlphabetString(data) + res);
    }
}

QString ImportExportService::to26AlphabetString(int data) const
{
    const QChar ch = data + 0x40;
    return QString(ch);
}
