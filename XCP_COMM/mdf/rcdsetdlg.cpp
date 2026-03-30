#include "rcdsetdlg.h"
#include "ui_rcdsetdlg.h"
#include <QSettings>
#include <QDebug>

RcdSetDlg::RcdSetDlg(QWidget *parent, QString name) :
    QDialog(parent),
    workName(name),
    ui(new Ui::RcdSetDlg)
{
    ui->setupUi(this);

    setWindowTitle("Record setting");

    workRecordDir = QApplication::applicationDirPath() + "/work/" + workName + "/record";

    readSetting();
}

RcdSetDlg::~RcdSetDlg()
{
    delete ui;
}

void RcdSetDlg::accept()
{
    writeSetting();
    QDialog::accept();
}

void RcdSetDlg::readSetting()
{
    QString recordSetFilePath = workRecordDir + "/record_setting.ini";
    QSettings setting(recordSetFilePath, QSettings::IniFormat);

    qInfo()<<"read record setting:" + setting.fileName();

    ui->le_rcdDataFileName->setText(setting.value("RecordDataFileName").toString());
    ui->cb_CycleRcd->setChecked(setting.value("CycleRecord").toBool());
    ui->sb_NumOfCycle->setValue(setting.value("NumberOfCycle").toUInt());
    ui->sb_SingleCycleTime->setValue(setting.value("SingleCycleTime").toUInt());
}

void RcdSetDlg::writeSetting()
{
    QString recordSetFilePath = workRecordDir + "/record_setting.ini";
    QSettings setting(recordSetFilePath, QSettings::IniFormat);

    setting.clear();

    qInfo()<<"write record setting:" + setting.fileName();

    setting.setValue("RecordDataFileName", ui->le_rcdDataFileName->text());
    setting.setValue("CycleRecord", ui->cb_CycleRcd->isChecked());
    setting.setValue("NumberOfCycle", ui->sb_NumOfCycle->value());
    setting.setValue("SingleCycleTime", ui->sb_SingleCycleTime->value());
}

void RcdSetDlg::setWorkName(const QString &value)
{
    workName = value;
    workRecordDir = QApplication::applicationDirPath() + "/work/" + workName + "/record";
}
