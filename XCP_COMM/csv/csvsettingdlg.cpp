#include "csvsettingdlg.h"
#include "ui_csvsettingdlg.h"
#include <QDebug>
#include <QCheckBox>

CsvSettingDlg::CsvSettingDlg(QWidget *parent, QString name) :
    QDialog(parent),
    workName(name),
    ui(new Ui::CsvSettingDlg)
{
    ui->setupUi(this);

    setWindowTitle("CsvSetting");
    //showMaximized();

    ui->lineEdit_fileStorePath->setProperty("borderless", "true");
    ui->lineEdit_fileStorePath->setReadOnly(true);
    ui->lineEdit_fullFilePath->setProperty("borderless", "true");
    ui->lineEdit_fullFilePath->setReadOnly(true);

    ui->table->setAlternatingRowColors(true);

    workRecordDir = QApplication::applicationDirPath() + "/work/" + workName + "/record";

    pamModel = new QStandardItemModel;
    ui->table->setModel(pamModel);
    QStringList header;
    header << "PamName" << "ModName";
    pamModel->setHorizontalHeaderLabels(header);

    selPamModel = new QStandardItemModel;
    ui->table_selected->setModel(selPamModel);
    selPamModel->setHorizontalHeaderLabels(header);

    showAllPamsInTable();

    readSetting();

}

CsvSettingDlg::~CsvSettingDlg()
{
    delete ui;
}

void CsvSettingDlg::accept()
{
    writeSetting();
    QDialog::accept();
}

void CsvSettingDlg::readSetting()
{
    QString recordSetFilePath = workRecordDir + "/csv_setting.ini";
    QSettings setting(recordSetFilePath, QSettings::IniFormat);

    qInfo()<<"read csv setting:" + setting.fileName();

    ui->lineEdit_fileName->setText(setting.value("CsvFileName").toString());
    ui->lineEdit_fileStorePath->setText(setting.value("CsvFileDir").toString());
    ui->lineEdit_fullFilePath->setText(setting.value("CsvFullFilePath").toString());

    QString pamNames = setting.value("CsvPamNames").toString();
    QString modNames = setting.value("CsvModNames").toString();

    QStringList pamNameList, modNameList;

    pamNameList = pamNames.split("|");
    modNameList = modNames.split("|");

    if(pamNameList.count() != modNameList.count())
        return;

    for(int i = 0; i < pamNameList.count(); i++)
    {
        QList<QStandardItem*> itemList;

        QStandardItem *nameItem = new QStandardItem(pamNameList.at(i));
        QStandardItem *modItem = new QStandardItem(modNameList.at(i));

        itemList << nameItem << modItem;

        selPamModel->appendRow(itemList);
    }

    ui->table_selected->resizeColumnsToContents();

}

void CsvSettingDlg::writeSetting()
{

    csvFileName = ui->lineEdit_fileName->text() + ".csv";
    QString csvFileDir = ui->lineEdit_fileStorePath->text();
    csvFileDir = csvFileDir == "" ? QApplication::applicationDirPath() + "/data/csv" : csvFileDir;
    csvFilePath = csvFileDir + "/" + csvFileName;
    ui->lineEdit_fullFilePath->setText(csvFilePath);

    QString recordSetFilePath = workRecordDir + "/csv_setting.ini";
    QSettings setting(recordSetFilePath, QSettings::IniFormat);

    setting.clear();

    qInfo()<<"write csv setting:" + setting.fileName();

    setting.setValue("CsvFileName", ui->lineEdit_fileName->text());
    setting.setValue("CsvFileDir", ui->lineEdit_fileStorePath->text());
    setting.setValue("CsvFullFilePath", ui->lineEdit_fullFilePath->text());

    int rowCount = selPamModel->rowCount();
    QString pamNames, modNames;
    for(int i = 0; i < rowCount; i++)
    {

        if(i == (rowCount - 1))
        {
            pamNames += selPamModel->item(i, 0)->text();
            modNames += selPamModel->item(i, 1)->text();
        }
        else
        {
            pamNames += (selPamModel->item(i, 0)->text() + "|");
            modNames += (selPamModel->item(i, 1)->text() + "|");
        }
    }

    setting.setValue("CsvPamNames", pamNames);
    setting.setValue("CsvModNames", modNames);

}

void CsvSettingDlg::showAllPamsInTable()
{
    pamModel->clear();
    pamModel->setColumnCount(2);

    QStringList header;
    header << "PamName" << "ModName";
    pamModel->setHorizontalHeaderLabels(header);

    int index = 0;
    QStringList keys_sel = recordPamHash.keys();


    QStringList keys = Globals::funPamListHash.keys();
    qSort(keys.begin(), keys.end());

    for(int i = 0; i < keys.count(); i++)
    {
        QString key = keys.at(i);

        QList<PARAM*> pamList = Globals::funPamListHash.value(key);

        for(int j = 0; j < pamList.count(); j++)
        {
            PARAM *pam = pamList.at(j);

            QString refName = pam->Name + "." + pam->smKey;
            if(keys_sel.indexOf(refName) != -1)
                continue;

            QStandardItem *nameItem = new QStandardItem(pam->Name);
            QStandardItem *modItem = new QStandardItem(pam->smKey);

            QList<QStandardItem*> itemList;
            itemList << nameItem << modItem;
            pamModel->appendRow(itemList);

            index++;
        }
    }

    QStringList keys_w = Globals::funWritePamListHash.keys();
    qSort(keys_w.begin(), keys_w.end());

    for(int i = 0; i < keys_w.count(); i++)
    {
        QString key = keys_w.at(i);

        QList<PARAM*> pamList = Globals::funWritePamListHash.value(key);

        for(int j = 0; j < pamList.count(); j++)
        {
            PARAM *pam = pamList.at(j);

            QString refName = pam->Name + "." + pam->smKey;
            if(keys_sel.indexOf(refName) != -1)
                continue;

            QStandardItem *nameItem = new QStandardItem(pam->Name);
            QStandardItem *modItem = new QStandardItem(pam->smKey);

            QList<QStandardItem*> itemList;
            itemList << nameItem << modItem;
            pamModel->appendRow(itemList);

            index++;
        }
    }

    ui->table->resizeColumnsToContents();

}

void CsvSettingDlg::getSelectedPamsInTable()
{
    recordPamList.clear();
    recordPamHash.clear();

    int rowCount = selPamModel->rowCount();

    qDebug()<<"row count="<<rowCount;

    for(int i = 0; i < rowCount; i++)
    {
        QString paramName = selPamModel->item(i, 0)->text();
        QString moduleName = selPamModel->item(i, 1)->text();
        PARAM *pam = Globals::pamHash.value(paramName + "." + moduleName);
        recordPamList.append(pam);

        recordPamHash.insert(paramName + "." + moduleName, pam);
    }

    qDebug()<<"selected pam count="<<recordPamList.count();
}

void CsvSettingDlg::on_pb_config_released()
{
    QString storePath =
            QFileDialog::getExistingDirectory(this, tr("Choose file store path"),
                                         QDir::currentPath());

    ui->lineEdit_fileStorePath->setText(storePath);
}

void CsvSettingDlg::on_pb_filter_released()
{
    QString str = ui->le_startWith->text();

    for(int i = 0; i < pamModel->rowCount(); i++)
    {
        if(!pamModel->item(i, 0)->text().startsWith(str, Qt::CaseInsensitive))
        {
            ui->table->hideRow(i);
        }
        else
        {
            ui->table->showRow(i);
        }
    }
}

void CsvSettingDlg::on_pb_refresh_released()
{

}

void CsvSettingDlg::on_pb_selected_released()
{
    QModelIndex curIndex = ui->table->currentIndex();
    int curRow = curIndex.row();

    QList<QStandardItem*> itemList;

    QStandardItem *nameItem = new QStandardItem(pamModel->item(curRow, 0)->text());
    QStandardItem *modItem = new QStandardItem(pamModel->item(curRow, 1)->text());

    itemList << nameItem << modItem;

    selPamModel->appendRow(itemList);

    ui->table_selected->resizeColumnsToContents();

}

void CsvSettingDlg::on_pb_unselected_released()
{
    QModelIndex curIndex = ui->table_selected->currentIndex();
    int curRow = curIndex.row();

    //pamModel->appendRow(selPamModel->takeRow(curRow));
    selPamModel->takeRow(curRow);
}

void CsvSettingDlg::on_pb_up_released()
{
    QModelIndex curIndex = ui->table_selected->currentIndex();
    int curRow = curIndex.row();
    //qDebug()<< "curRow:"<<curRow;
    if(curRow < 1) return;

    QList<QStandardItem*> itemList = selPamModel->takeRow(curRow);
    selPamModel->insertRow(curRow - 1, itemList);

    QModelIndex nextIndex = selPamModel->index(curRow - 1, 0);
    ui->table_selected->setCurrentIndex(nextIndex);

    //qDebug()<<"After curRow:"<<ui->table_selected->currentIndex().row();
}

void CsvSettingDlg::on_pb_down_released()
{
    int rowCount = selPamModel->rowCount();
    QModelIndex curIndex = ui->table_selected->currentIndex();
    int curRow = curIndex.row();
    //qDebug()<< "curRow:"<<curRow;
    if(curRow >= (rowCount - 1)) return;

    QList<QStandardItem*> itemList = selPamModel->takeRow(curRow);
    selPamModel->insertRow(curRow + 1, itemList);

    QModelIndex nextIndex = selPamModel->index(curRow + 1, 0);
    ui->table_selected->setCurrentIndex(nextIndex);

    //qDebug()<<"After curRow:"<<ui->table_selected->currentIndex().row();
}
