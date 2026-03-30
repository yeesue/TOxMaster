#include "configwin.h"
#include "ui_configwin.h"
#include <QFileDialog>
#include <QFileDevice>
#include <QFile>
#include <QMessageBox>

#include <QDebug>

ConfigWin::ConfigWin(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::ConfigWin)
{
    ui->setupUi(this);

    setWindowTitle("Configuration");
}

ConfigWin::~ConfigWin()
{
    delete ui;
}

void ConfigWin::on_pb_select_MeasPamFile_clicked()
{
    QFileDialog* fd = new QFileDialog(this);
    QString appPath = QApplication::applicationDirPath();
    QString filePath = fd->getOpenFileName(this,
                                           tr("Open Meas File"),appPath,tr("Excel(*.xlsx *.xls)"));
    if(filePath.isEmpty())
        return;
    qDebug()<<"Selected a2l meas pam file:"<<filePath;

    ui->le_MeasPamFile->setText(filePath);
}

void ConfigWin::on_pb_select_CharPamFile_clicked()
{
    QFileDialog* fd = new QFileDialog(this);
    QString appPath = QApplication::applicationDirPath();
    QString filePath = fd->getOpenFileName(this,
                                           tr("Open Char File"),appPath,tr("Excel(*.xlsx *.xls)"));
    if(filePath.isEmpty())
        return;
    qDebug()<<"Selected a2l char pam file:"<<filePath;

    ui->le_CharPamFile->setText(filePath);
}

void ConfigWin::on_pb_select_CalPamFile_clicked()
{
    QFileDialog* fd = new QFileDialog(this);
    QString appPath = QApplication::applicationDirPath();
    QString filePath = fd->getOpenFileName(this,
                                           tr("Open Cal File"),appPath,tr("Excel(*.xlsx *.xls)"));
    if(filePath.isEmpty())
        return;
    qDebug()<<"Selected cal pam file:"<<filePath;

    ui->le_CalPamFile->setText(filePath);
}

void ConfigWin::on_pb_select_CanPamFile_clicked()
{
    QFileDialog* fd = new QFileDialog(this);
    QString appPath = QApplication::applicationDirPath();
    QString filePath = fd->getOpenFileName(this,
                                           tr("Open Can File"),appPath,tr("xml(*.xml)"));
    if(filePath.isEmpty())
        return;
    qDebug()<<"Selected can pam file:"<<filePath;

    ui->le_CanPamFile->setText(filePath);
}

void ConfigWin::on_pb_select_ScriptFile_clicked()
{
    QFileDialog* fd = new QFileDialog(this);
    QString appPath = QApplication::applicationDirPath();
    QString filePath = fd->getOpenFileName(this,
                                           tr("Open Script File"),appPath,tr("JS(*.js)"));
    if(filePath.isEmpty())
        return;
    qDebug()<<"Selected script file:"<<filePath;

    ui->le_ScriptFile->setText(filePath);
}

bool ConfigWin::readSetting(QString configFilePath)
{
    if(configFilePath.isEmpty())
        return false;
    QFileInfo fileInfo(configFilePath);
    if(!fileInfo.isFile() || fileInfo.suffix() != "ini")
    {
        return false;
    }

    QSettings setting(configFilePath, QSettings::IniFormat);

    QString a2lMeasPamFile = setting.value("a2lMeasPamFile").toString();
    QString a2lCharPamFile = setting.value("a2lCharPamFile").toString();
    QString calPamFile = setting.value("calPamFile").toString();
    QString canPamFile = setting.value("canPamFile").toString();
    QString scriptFile = setting.value("scriptFile").toString();

    ui->le_MeasPamFile->setText(a2lMeasPamFile);
    ui->le_CharPamFile->setText(a2lCharPamFile);
    ui->le_CalPamFile->setText(calPamFile);
    ui->le_CanPamFile->setText(canPamFile);
    ui->le_ScriptFile->setText(scriptFile);

    return true;
}

bool ConfigWin::writeSetting(QString configFilePath)
{
    if(configFilePath.isEmpty())
        return false;
    QFileInfo fileInfo(configFilePath);

    if(!fileInfo.isFile() || fileInfo.suffix() != "ini")
    {
        return false;
    }

    QSettings setting(configFilePath, QSettings::IniFormat);

    setting.setValue("a2lMeasPamFile", ui->le_MeasPamFile->text());
    setting.setValue("a2lCharPamFile", ui->le_CharPamFile->text());
    setting.setValue("calPamFile", ui->le_CalPamFile->text());
    setting.setValue("canPamFile", ui->le_CanPamFile->text());
    setting.setValue("scriptFile", ui->le_ScriptFile->text());

    return true;
}

void ConfigWin::on_pb_load_clicked()
{
    QFileDialog* fd = new QFileDialog(this);
    QString appPath = QApplication::applicationDirPath() + "/config";
    QString filePath = fd->getOpenFileName(this,
                                           tr("Open File"),appPath,tr("Configuration(*.ini)"));
    if(filePath.isEmpty())
        return;

    qDebug()<<"Selected read config file:"<<filePath;

    if(!readSetting(filePath))
    {
        qDebug()<<"read config file error";
    }

}

void ConfigWin::on_pb_save_clicked()
{
    QFileDialog* fd = new QFileDialog(this);
    QString appPath = QApplication::applicationDirPath() + "/config";
    QString filePath = fd->getSaveFileName(this,
                                           tr("Save File"), appPath, tr("Configuration(*.ini)"));

    if(filePath.isEmpty())
        return;
    qDebug()<<"Save config file:"<<filePath;

    QFile file(filePath);
    if(!file.open(QIODevice::WriteOnly | QIODevice::Text))
    {
            return;
    }
    file.close();

    if(!writeSetting(filePath))
    {
        qDebug()<<"save config file error";
    }
}

void ConfigWin::on_actionNew_Configuration_triggered()
{
    ui->le_MeasPamFile->clear();
    ui->le_CharPamFile->clear();
    ui->le_CalPamFile->clear();
    ui->le_CanPamFile->clear();
    ui->le_ScriptFile->clear();

    flag_isNew = 1;
    flag_isOpen = 1;
}

void ConfigWin::on_actionOpen_Configuration_triggered()
{
    QFileDialog* fd = new QFileDialog(this);
    QString path = QApplication::applicationDirPath() + "/config";

    QString fileName;
    fileName = fd->getOpenFileName(this,
                                   tr("Open configuration"), path ,"INI File(*.ini)");
    if(fileName == "")
    {
        return ;
    }
    else
    {
        QFile file(fileName);
        if(!file.open(QIODevice::ReadOnly | QIODevice::Text))
        {
            QMessageBox::warning(this,"error","open file error!");
            return;
        }
        else
        {
            if(!file.isReadable())
                QMessageBox::warning(this,"error","this file is not readable!");
            else
            {

                file.close();

                readSetting(fileName);
                flag_isOpen = 1;
                Last_FileName = fileName;
            }
        }
    }

    qDebug()<<"file is new: " << flag_isNew << ", file is open:" << flag_isOpen << ",open file:"<<Last_FileName;

}

void ConfigWin::on_actionSave_triggered()
{
    if(flag_isNew)
    {
        QFileDialog fileDialog;
        QString fileName = fileDialog.getSaveFileName(this,"Open File","","Ini File(*.ini)");
        if(fileName == "")
        {
            return;
        }
        QFile file(fileName);
        if(!file.open(QIODevice::WriteOnly | QIODevice::Text))
        {
            QMessageBox::warning(this,"error","Open File Error!");
            return;
        }
        else
        {

            Last_FileContent =fileName;
        }
        QMessageBox::information(this,"Save File","Save File Success",QMessageBox::Ok);
        file.close();
        writeSetting(fileName);
        flag_isNew = 0;
        Last_FileName = fileName;
    }
        else
        {
            if(flag_isOpen)
            {
                QFile file(Last_FileName);
                if(!file.open(QIODevice::WriteOnly | QIODevice::Text))
                {
                    QMessageBox::warning(this,"error","Open File Faile");
                    return;
                }
                else
                {
                    file.close();

                    writeSetting(Last_FileName);
                }
            }
            else
            {
                QMessageBox::warning(this,"Warning","Please new or open a file");
                return;
            }

        }

}

void ConfigWin::on_actionSave_As_triggered()
{
    QFileDialog fileDialog;
    QString fileName = fileDialog.getSaveFileName(this,"Open File","","Ini File(*.ini)");
    if(fileName == "")
    {
        return;
    }
    QFile file(fileName);
    if(!file.open(QIODevice::WriteOnly | QIODevice::Text))
    {
        QMessageBox::warning(this,"error","open file failure!");
        return;
    }
    else
    {

        QMessageBox::warning(this,"tip","Save File Success!");

        file.close();

        Last_FileName = fileName;
        flag_isNew = 0;
        writeSetting(fileName);
    }

}
