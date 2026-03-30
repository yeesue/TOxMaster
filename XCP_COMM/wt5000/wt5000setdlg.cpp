#include "wt5000setdlg.h"
#include "ui_wt5000setdlg.h"
#include <QFile>
#include <QFileInfo>
#include <QDebug>

WT5000SetDlg::WT5000SetDlg(QWidget *parent, QString work_name, QString fun_name, QString intf_name, QString set_file_path) :
    QDialog(parent),
    workName(work_name), funName(fun_name),
    intfName(intf_name), setFilePath(set_file_path),
    ui(new Ui::WT5000SetDlg)
{
    ui->setupUi(this);
    setWindowTitle("wt5000 setting(" + workName.toLower() + "/" + funName.toLower() + ")");


    setFilePath = QApplication::applicationDirPath() + setFilePath;
    QString fileName = workName.toLower() + "-" + funName.toLower() + "-setting.ini";
    QString filePath = QApplication::applicationDirPath() + "/work/" + workName + "/" + fileName;

    if(set_file_path.isEmpty())
    {
        setFileName = fileName;
        setFilePath = filePath;

        QFileInfo fileInfo(setFilePath);
        if(!fileInfo.isFile())
        {
            QFile file(setFilePath);
            qInfo()<<"wt3000 fun setting file new created:"<<fileName;
        }
    }
    else
    {
        QFileInfo fileInfo(setFilePath);
        if(setFilePath != filePath && fileInfo.isFile())
        {
            QFile file(setFilePath);
            bool ok = file.rename(filePath);
            if(ok)
            {
                qDebug()<<"wt3000 fun setting file rename as :"<<fileName;
                setFileName = fileName;
                setFilePath = filePath;
            }
        }
    }

    readSetting();
}

WT5000SetDlg::~WT5000SetDlg()
{
    delete ui;
}

void WT5000SetDlg::accept()
{
    writeSetting();
    QDialog::accept();
}

void WT5000SetDlg::writeSetting()
{
    //QString appPath = QApplication::applicationDirPath();
    //QString setFilePath = appPath + "/setting/wt3000_setting.ini";
    //QSettings setting(setFilePath, QSettings::IniFormat);

    QSettings setting(setFilePath, QSettings::IniFormat);

    setting.setValue("IP_Address", ui->IPAddressEdit->text());
}

void WT5000SetDlg::readSetting()
{
    //QString appPath = QApplication::applicationDirPath();
    //QString setFilePath = appPath + "/setting/wt3000_setting.ini";

    //QSettings setting(setFilePath, QSettings::IniFormat);

    QSettings setting(setFilePath, QSettings::IniFormat);

    ui->IPAddressEdit->setText(setting.value("IP_Address").toString());
}

QString WT5000SetDlg::getSetFilePath() const
{
    return setFilePath;
}
