#include "funpamdlg.h"
#include "ui_funpamdlg.h"

FunPamDlg::FunPamDlg(QWidget *parent, int mode) :
    QDialog(parent),
    pamMode(mode),
    ui(new Ui::FunPamDlg)
{
    ui->setupUi(this);

    QStringList keys;
    if(mode == 0)
    {
        keys = Globals::funPamListHash.uniqueKeys();

    }
    else if(mode == 1)
    {
        keys = Globals::funWritePamListHash.uniqueKeys();
    }
    //qDebug()<<keys;
    ui->comboBox_module->addItems(keys);

    connect(ui->comboBox_module, SIGNAL(currentTextChanged(QString)), this, SLOT(Slt_ModuleChanged(QString)));
    connect(ui->listWidget_pams, SIGNAL(itemClicked(QListWidgetItem*)), this, SLOT(pamItemClickedSlot(QListWidgetItem*)));
    connect(ui->listWidget_pamSelected, SIGNAL(itemDoubleClicked(QListWidgetItem*)), this, SLOT(pamSelectedItemDoubleClicked(QListWidgetItem*)));

    ui->comboBox_module->setCurrentIndex(0);
    if(keys.count() > 0)
        Slt_ModuleChanged(keys.at(0));

}

FunPamDlg::~FunPamDlg()
{
    delete ui;
}

void FunPamDlg::clearSelection()
{
    ui->listWidget_pamSelected->clear();
    ui->listWidget_pams->clearSelection();
}

void FunPamDlg::Slt_ModuleChanged(QString text)
{
    ui->listWidget_pams->clear();

    if(pamMode == 0)
    {
        QList<PARAM*> pamList;

        pamList = Globals::funPamListHash.value(text);

        foreach (PARAM *pam, pamList) {
            QVariant var;
            var.setValue(pam);
            QListWidgetItem *item = new QListWidgetItem(pam->Name);
            item->setData(Qt::UserRole, var);
            ui->listWidget_pams->addItem(item);
        }

    }
    else if(pamMode == 1)
    {
        /*
        QList<A2L_VarChar*> pamList;
        pamList = Globals::funCharListHash.value(text);
        foreach (A2L_VarChar *pam, pamList) {
            QVariant var;
            var.setValue(pam);
            QListWidgetItem *item = new QListWidgetItem(pam->Name);
            item->setData(Qt::UserRole, var);
            ui->listWidget_pams->addItem(item);
        }
        */

        QList<PARAM*> pamList;
        pamList = Globals::funWritePamListHash.value(text);
        foreach (PARAM *pam, pamList) {
            QVariant var;
            var.setValue(pam);
            QListWidgetItem *item = new QListWidgetItem(pam->Name);
            item->setData(Qt::UserRole, var);
            ui->listWidget_pams->addItem(item);
        }

    }

}

void FunPamDlg::pamItemClickedSlot(QListWidgetItem *item)
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

void FunPamDlg::pamSelectedItemDoubleClicked(QListWidgetItem *item)
{
    int row = ui->listWidget_pamSelected->row(item);
    ui->listWidget_pamSelected->takeItem(row);
}

void FunPamDlg::accept()
{
    QStringList nameList;

    for(int i = 0; i < ui->listWidget_pamSelected->count(); i++)
    {
        QListWidgetItem *item = ui->listWidget_pamSelected->item(i);
        nameList<<item->text();
    }

    if(pamMode == 0)
    {
        pamSelected = getPamsByNames(nameList);

        emit pamListSelected(pamSelected);
        emit pamFirstSelected(pamSelected.first());
    }
    else if(pamMode == 1)
    {

        writePamSelected = getWritePamsByNames(nameList);

        emit writePamListSelected(writePamSelected);
        emit writePamFirstSelected(writePamSelected.first());
    }

    QDialog::accept();
}

QList<PARAM *> FunPamDlg::getPamsByNames(QStringList names)
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


PARAM *FunPamDlg::findPambyName(QString module, QString name)
{
    if(module.isEmpty() || name.isEmpty())
        return NULL;
    QList<PARAM*> pamList;

    pamList = Globals::funPamListHash.value(module);

    foreach (PARAM *pam, pamList) {
        if(pam->Name == name)
            return pam;
    }
    return NULL;
}

QList<PARAM *> FunPamDlg::getWritePamsByNames(QStringList names)
{
    QList<PARAM*> pamList;
    for (int i = 0; i < names.count(); i++)
    {
        QString selectedName = names.at(i);
        QRegExp rx("\\.");
        int pos = rx.indexIn(selectedName, 0);
        QString name = selectedName.left(pos);
        QString module = selectedName.right(selectedName.count()- pos -1);

        PARAM *pam = findWritePambyName(module, name);

        pamList.append(pam);
    }

    return pamList;
}

PARAM *FunPamDlg::findWritePambyName(QString module, QString name)
{
    if(module.isEmpty() || name.isEmpty())
        return NULL;
    QList<PARAM*> pamList;

    pamList = Globals::funWritePamListHash.value(module);

    foreach (PARAM *pam, pamList) {
        if(pam->Name == name)
            return pam;
    }
    return NULL;
}

QList<A2L_VarChar *> FunPamDlg::getCharPamsByNames(QStringList names)
{
    QList<A2L_VarChar*> pamList;
    for (int i = 0; i < names.count(); i++)
    {
        QString selectedName = names.at(i);
        QRegExp rx("\\.");
        int pos = rx.indexIn(selectedName, 0);
        QString name = selectedName.left(pos);
        QString module = selectedName.right(selectedName.count()- pos -1);

        A2L_VarChar *pam = findCharPambyName(module, name);
        pamList.append(pam);
    }

    return pamList;
}

A2L_VarChar *FunPamDlg::findCharPambyName(QString module, QString name)
{
    if(module.isEmpty() || name.isEmpty())
        return NULL;
    QList<A2L_VarChar*> pamList;

    pamList = Globals::funCharListHash.value(module);

    foreach (A2L_VarChar *pam, pamList) {
        if(pam->Name == name)
            return pam;
    }
    return NULL;
}

