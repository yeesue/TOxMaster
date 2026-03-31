#include "csv_win.h"
#include "ui_csv_win.h"
#include <QDebug>
#include <QCheckBox>

CSV_Win::CSV_Win(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::CSV_Win)
{
    ui->setupUi(this);

    setWindowTitle("Set_CsvRecorder");

    ui->lineEdit_fileStorePath->setProperty("borderless", "true");
    ui->lineEdit_fileStorePath->setReadOnly(true);
    ui->lineEdit_fullFilePath->setProperty("borderless", "true");
    ui->lineEdit_fullFilePath->setReadOnly(true);

    ui->tableWidget->setAlternatingRowColors(true);

    showAllPamsInTableWidget();

}

CSV_Win::~CSV_Win()
{

    delete pamModel;
    delete ui;
}


void CSV_Win::showAllPamsInTableWidget()
{

    ui->tableWidget->clear();
    ui->tableWidget->setColumnCount(3);

    QStringList header;
    header << "PamName" << "ModName"<< "Record Check";
    ui->tableWidget->setHorizontalHeaderLabels(header);

    QStringList keys = Globals::funPamListHash.keys();
    qSort(keys.begin(), keys.end());

    int index = 0;
    for(int i = 0; i < keys.count(); i++)
    {
        QString key = keys.at(i);

        QList<PARAM*> pamList = Globals::funPamListHash.value(key);

        for(int j = 0; j < pamList.count(); j++)
        {
            PARAM *pam = pamList.at(j);

            ui->tableWidget->insertRow(index);

            ui->tableWidget->setItem(index, 0, new QTableWidgetItem(pam->Name));
            ui->tableWidget->setColumnWidth(0, 200);

            ui->tableWidget->setItem(index, 1, new QTableWidgetItem(pam->smKey));

            QCheckBox *box = new QCheckBox(this);
            QWidget *w = new QWidget(this);
            QHBoxLayout *layout = new QHBoxLayout();
            layout->addWidget(box);
            layout->setMargin(0);
            layout->setAlignment(box, Qt::AlignCenter);
            w->setLayout(layout);

            ui->tableWidget->setCellWidget(index, 2, w);

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

            ui->tableWidget->insertRow(index);

            ui->tableWidget->setItem(index, 0, new QTableWidgetItem(pam->Name));
            ui->tableWidget->setColumnWidth(0, 200);

            ui->tableWidget->setItem(index, 1, new QTableWidgetItem(pam->smKey));

            QCheckBox *box = new QCheckBox(this);
            QWidget *w = new QWidget(this);
            QHBoxLayout *layout = new QHBoxLayout();
            layout->addWidget(box);
            layout->setMargin(0);
            layout->setAlignment(box, Qt::AlignCenter);
            w->setLayout(layout);

            ui->tableWidget->setCellWidget(index, 2, w);

            index++;
        }
    }

    ui->tableWidget->resizeColumnToContents(0);
    ui->tableWidget->resizeColumnToContents(1);
}

void CSV_Win::getSelectedPamsInTable()
{
    recordPamList.clear();

    int rowCount = ui->tableWidget->rowCount();

    qDebug()<<"row count="<<rowCount;

    for(int i = 0; i < rowCount; i++)
    {
        QWidget *w = ui->tableWidget->cellWidget(i, 2);
        QLayoutItem *item = w->layout()->itemAt(0);
        QCheckBox *box  = (QCheckBox*)(item->widget());
        if(!box)
            continue;

        bool checked = box->isChecked();

        if(checked)
        {
            QString paramName = ui->tableWidget->item(i, 0)->text();
            QString moduleName = ui->tableWidget->item(i, 1)->text();
            PARAM *pam = Globals::pamHash.value(paramName + "." + moduleName);
            recordPamList.append(pam);
        }
    }

    qDebug()<<"selected pam count="<<recordPamList.count();
}


QList<PARAM *> CSV_Win::getRecordPamList() const
{
    return recordPamList;
}

void CSV_Win::setRecordPamList(const QList<PARAM *> &value)
{
    recordPamList = value;
}

void CSV_Win::on_pb_Save_clicked()
{
    getSelectedPamsInTable();
}

void CSV_Win::on_pb_rcd_path_config_clicked()
{
    QString storePath =
            QFileDialog::getExistingDirectory(this, tr("Choose file store path"),
                                         QDir::currentPath());


    ui->lineEdit_fileStorePath->setText(storePath);
}

void CSV_Win::on_pb_Filter_clicked()
{
    QString str = ui->le_startWith->text();

    for(int i = 0; i < ui->tableWidget->rowCount(); i++)
    {
        if(!ui->tableWidget->item(i, 0)->text().startsWith(str, Qt::CaseInsensitive))
        {
            ui->tableWidget->hideRow(i);
        }
        else
        {
            ui->tableWidget->showRow(i);
        }
    }
}
