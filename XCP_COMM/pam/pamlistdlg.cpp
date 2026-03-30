#include "pamlistdlg.h"
#include "ui_pamlistdlg.h"
#include <QDebug>
#include "globals.h"

pamListDlg::pamListDlg(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::pamListDlg)
{
    ui->setupUi(this);
    setWindowTitle(tr("参数选择"));

    QStringList keys = Globals::funPamListHash.uniqueKeys();
    //qSort(keys.begin(), keys.end());

    for(int i = 0; i < keys.count(); i++)
    {
        QString key = keys.at(i);

        ui->comboBox_module->addItem(key);

        //QList<PARAM*> pams = Globals::funPamListHash.value(key);

    }

    QStringList keys_w = Globals::funWritePamListHash.uniqueKeys();
    //qSort(keys_w.begin(), keys_w.end());

    for(int i = 0; i < keys_w.count(); i++)
    {
        QString key = keys_w.at(i);

        ui->comboBox_module->addItem(key);

        //QList<PARAM*> pams = Globals::funWritePamListHash.value(key);

    }

    QStringList keys_w_map = Globals::funMapPamListHash.uniqueKeys();
    //qSort(keys_w_map.begin(), keys_w_map.end());

    for(int i = 0; i < keys_w_map.count(); i++)
    {
        QString key = keys_w_map.at(i);

        ui->comboBox_module->addItem(key);

        //QList<PARAM*> pams = Globals::funMapPamListHash.value(key);

    }


    connect(ui->comboBox_module, SIGNAL(currentIndexChanged(int)), this, SLOT(SltModuleChanged(int)));
    connect(ui->listWidget_pams, SIGNAL(itemClicked(QListWidgetItem*)), this, SLOT(pamItemClickedSlot(QListWidgetItem*)));
    connect(ui->listWidget_pamSelected, SIGNAL(itemDoubleClicked(QListWidgetItem*)), this, SLOT(pamSelectedItemDoubleClicked(QListWidgetItem*)));

    ui->comboBox_module->setCurrentIndex(0);
    SltModuleChanged(0);

}

pamListDlg::~pamListDlg()
{
    delete ui;
}

void pamListDlg::clearSelection()
{
    ui->listWidget_pamSelected->clear();
    ui->listWidget_pams->clearSelection();
}

void pamListDlg::SltModuleChanged(int index)
{
    ui->listWidget_pams->clear();

    QString key = ui->comboBox_module->currentText();
    QList<PARAM*> pamList = Globals::funPamListHash.value(key);

    if(pamList.isEmpty())
    {
        pamList = Globals::funWritePamListHash.value(key);
    }

    if(pamList.isEmpty())
    {
        pamList = Globals::funMapPamListHash.value(key);
    }

    foreach(PARAM *pam, pamList) {
        QVariant var;
        var.setValue(pam);
        QListWidgetItem *item = new QListWidgetItem(pam->Name);
        item->setData(Qt::UserRole, var);
        ui->listWidget_pams->addItem(item);
    }

}

void pamListDlg::pamItemClickedSlot(QListWidgetItem *item)
{
    QString name = item->text();
    QString module = ui->comboBox_module->currentText();
    QString selectedName = name + "." + module;

    QList<QListWidgetItem*> items = ui->listWidget_pamSelected->findItems(selectedName, Qt::MatchExactly);

    if (items.count() == 0)
    {
        ui->listWidget_pamSelected->addItem(selectedName);

    }
    else
    {
        foreach (QListWidgetItem *item, items) {
            int row = ui->listWidget_pamSelected->row(item);
            ui->listWidget_pamSelected->takeItem(row);
        }
    }
}

void pamListDlg::pamSelectedItemDoubleClicked(QListWidgetItem *item)
{
    int row = ui->listWidget_pamSelected->row(item);
    ui->listWidget_pamSelected->takeItem(row);
}

void pamListDlg::accept()
{
    QStringList nameList;

    for(int i = 0; i < ui->listWidget_pamSelected->count(); i++)
    {
        QListWidgetItem *item = ui->listWidget_pamSelected->item(i);
        nameList<<item->text();
    }
    pamSelected = getPamsByNames(nameList);

    emit pamListSelected(pamSelected);

    emit pamFirstSelected(pamSelected.first());

    QDialog::accept();
}

QList<PARAM *> pamListDlg::getPamsByNames(QStringList names)
{
    QList<PARAM*> pamList;
    for (int i = 0; i < names.count(); i++)
    {
        QString selectedName = names.at(i);
        QRegExp rx("\\.");
        int pos = rx.indexIn(selectedName, 0);
        QString name = selectedName.left(pos);
        QString module = selectedName.right(selectedName.count()- pos -1);

        PARAM *pam = findPambyName(module, name);
        pamList.append(pam);
    }

    return pamList;
}


PARAM *pamListDlg::findPambyName(QString key, QString name)
{
    if(key.isEmpty() || name.isEmpty())
        return NULL;

    QList<PARAM*> pamList = Globals::funPamListHash.value(key);

    if(pamList.isEmpty())
    {
        pamList = Globals::funWritePamListHash.value(key);
    }

    if(pamList.isEmpty())
    {
        pamList = Globals::funMapPamListHash.value(key);
    }

    foreach (PARAM *pam, pamList) {
        if(pam->Name == name)
            return pam;
    }
    return NULL;
}
