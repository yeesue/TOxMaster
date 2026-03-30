#include "udssetdlg.h"
#include "ui_udssetdlg.h"
#include <QDir>
#include <QDebug>

UdsSetDlg::UdsSetDlg(QWidget *parent, QString work_name, QString fun_name, QString intf_name, QString set_file_path) :
    QDialog(parent), workName(work_name),
    funName(fun_name), intfName(intf_name),
    ui(new Ui::UdsSetDlg)
{
    ui->setupUi(this);

    setWindowTitle("uds setting(" + workName.toLower() + "/" + funName.toLower() + ")");

    QString fileName = workName.toLower() + "-" + funName.toLower() + "-setting.ini";
    QString filePath = QApplication::applicationDirPath() + "/work/" + workName + "/" + fileName;

    if(udsSetFilePath.isEmpty())
    {

        udsSetFileName = fileName;
        udsSetFilePath = filePath;
    }
    else
    {
        QFileInfo fileInfo(udsSetFilePath);
        if(udsSetFilePath != filePath && fileInfo.isFile())
        {
            QFile file(udsSetFilePath);
            bool ok = file.rename(filePath);
            if(ok)
            {
                qDebug()<<"uds fun setting file rename as :"<<fileName;
                udsSetFileName = fileName;
                udsSetFilePath = filePath;
            }
        }
    }

    readSetting();
}

UdsSetDlg::~UdsSetDlg()
{
    delete ui;
}


void UdsSetDlg::accept()
{
    writeSetting();

    QDialog::accept();
}

void UdsSetDlg::readSetting()
{
    QSettings setting(udsSetFilePath, QSettings::IniFormat);

    setting.beginGroup("UDSSetting");

    ui->cb_Intf->setCurrentText(setting.value("CanIntf").toString());
    ui->cb_IOMode->setCurrentIndex(setting.value("CanIoMode").toUInt());

    ui->sb_baudrate->setValue(setting.value("Baudrate").toUInt());
    ui->sb_baudrate_canfd->setValue(setting.value("Baudrate_CANFD").toUInt());

    ui->le_transmitID->setText(setting.value("TransmitID").toString());
    ui->le_receiveID->setText(setting.value("ReceiveID").toString());

    ui->sb_maxDLC->setValue(setting.value("maxDLC").toUInt());
    ui->le_securityConstant->setText(setting.value("SecurityConstant").toString());

    ui->cb_Tester->setChecked(setting.value("TesterActive").toBool());
    ui->sb_TesterCycleTime->setValue(setting.value("TesterCycleTime").toUInt());
    ui->cb_ResponseRequired->setChecked(setting.value("TesterResponseRequired").toBool());

    ui->cb_UseDllForSeedKey->setChecked(setting.value("DllActive").toBool());
    ui->le_dllPath->setText(setting.value("DllPath").toString());
    ui->cb_SeedKey->setCurrentText(setting.value("SeedKeyMethod").toString());

    ui->cb_byteOrder->setCurrentIndex(setting.value("ByteOrder").toUInt());

    ui->cb_autoFreeze->setChecked(setting.value("AutoFreeze").toBool());
    ui->sb_timeBetweenCtrlAndFreeze->setValue(setting.value("TimeBetweenCtrlAndFreeze").toUInt());


    setting.endGroup();
}

void UdsSetDlg::writeSetting()
{
    QSettings setting(udsSetFilePath, QSettings::IniFormat);

    setting.beginGroup("UDSSetting");

    setting.setValue("CanIntf", ui->cb_Intf->currentText());
    setting.setValue("CanIoMode", ui->cb_IOMode->currentIndex());
    setting.setValue("Baudrate", ui->sb_baudrate->value());
    setting.setValue("Baudrate_CANFD", ui->sb_baudrate_canfd->value());
    setting.setValue("TransmitID", ui->le_transmitID->text());
    setting.setValue("ReceiveID", ui->le_receiveID->text());
    setting.setValue("maxDLC", ui->sb_maxDLC->value());
    setting.setValue("SecurityConstant", ui->le_securityConstant->text());

    setting.setValue("TesterActive", ui->cb_Tester->isChecked());
    setting.setValue("TesterCycleTime", ui->sb_TesterCycleTime->value());
    setting.setValue("TesterResponseRequired", ui->cb_ResponseRequired->isChecked());

    setting.setValue("DllActive", ui->cb_UseDllForSeedKey->isChecked());
    setting.setValue("DllPath", ui->le_dllPath->text());
    setting.setValue("SeedKeyMethod", ui->cb_SeedKey->currentText());
    setting.setValue("SeedKeyMethodIndex", ui->cb_SeedKey->currentIndex());
    setting.setValue("ByteOrder", ui->cb_byteOrder->currentIndex());

    setting.setValue("AutoFreeze", ui->cb_autoFreeze->isChecked());
    setting.setValue("TimeBetweenCtrlAndFreeze", ui->sb_timeBetweenCtrlAndFreeze->value());


    setting.endGroup();

    emit udsSetUpdated();
}

QString UdsSetDlg::getUdsSetFilePath() const
{
    return udsSetFilePath;
}


QString UdsSetDlg::getUdsSetFileName() const
{
    return udsSetFileName;
}
