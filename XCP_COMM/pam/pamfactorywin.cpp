#include "pamfactorywin.h"
#include "ui_pamfactorywin.h"

PamFactoryWin::PamFactoryWin(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::PamFactoryWin)
{
    ui->setupUi(this);
    setWindowTitle("PamFactory");

    pamModel = new QStandardItemModel();
    ui->treeView_Pam->setModel(pamModel);
    ui->treeView_Pam->setAlternatingRowColors(true);

    updateTimer = new QTimer(this);
    updateTimer->setInterval(100);

    updatePamView(true);

    startPamUpdate();
}

PamFactoryWin::~PamFactoryWin()
{
    delete ui;
}

void PamFactoryWin::updatePamView(bool runFlag)
{
    //pamActiveFlag = runFlag;

    qDebug()<<"update pam view:"<<runFlag;

    pamModel->clear();

    QStringList header;
    header << "Parameter" << "Value";
    pamModel->setHorizontalHeaderLabels(header);
    ui->treeView_Pam->setColumnWidth(0, 250);

    if(!runFlag)
        return;

    QStringList keys = Globals::funPamListHash.uniqueKeys();
    qSort(keys.begin(), keys.end());

    for(int i = 0; i < keys.count(); i++)
    {
        QString key = keys.at(i);

        QStandardItem *keyItem = new QStandardItem(key);

        QList<PARAM*> pams = Globals::funPamListHash.value(key);

        pamModel->appendRow(keyItem);

        foreach (PARAM* pam, pams) {
            QList<QStandardItem*> pamItemList;
            QStandardItem *pamItem = new QStandardItem(pam->Name);
            QStandardItem *pamValueItem = new QStandardItem(QString::number(pam->getValue(), 'g', 8));
            pamItemList.append(pamItem);
            pamItemList.append(pamValueItem);

            keyItem->appendRow(pamItemList);
        }
    }

    QStringList keys_w = Globals::funWritePamListHash.uniqueKeys();
    qSort(keys_w.begin(), keys_w.end());

    for(int i = 0; i < keys_w.count(); i++)
    {
        QString key = keys_w.at(i);

        QStandardItem *keyItem = new QStandardItem(key);

        QList<PARAM*> pams = Globals::funWritePamListHash.value(key);

        pamModel->appendRow(keyItem);

        foreach (PARAM* pam, pams) {
            QList<QStandardItem*> pamItemList;
            QStandardItem *pamItem = new QStandardItem(pam->Name);
            QStandardItem *pamValueItem = new QStandardItem(QString::number(pam->getValue(), 'g', 8));
            pamItemList.append(pamItem);
            pamItemList.append(pamValueItem);

            keyItem->appendRow(pamItemList);
        }
    }

    QStringList keys_w_map = Globals::funMapPamListHash.uniqueKeys();
    qSort(keys_w_map.begin(), keys_w_map.end());

    for(int i = 0; i < keys_w_map.count(); i++)
    {
        QString key = keys_w_map.at(i);

        QStandardItem *keyItem = new QStandardItem(key);

        QList<PARAM*> pams = Globals::funMapPamListHash.value(key);

        pamModel->appendRow(keyItem);

        foreach (PARAM* pam, pams) {
            QList<QStandardItem*> pamItemList;
            QStandardItem *pamItem = new QStandardItem(pam->Name);
            QStandardItem *pamValueItem = new QStandardItem(QString::number(pam->getValue(), 'g', 8));
            pamItemList.append(pamItem);
            pamItemList.append(pamValueItem);

            keyItem->appendRow(pamItemList);
        }
    }

}

void PamFactoryWin::stopPamUpdate()
{
    if(updateTimer == NULL)
    {
        updateTimer = new QTimer(this);
    }

    updateTimer->stop();
    disconnect(updateTimer, SIGNAL(timeout()), this, SLOT(updatePamValueView()));
    disconnect(updateTimer, SIGNAL(timeout()), this, SLOT(updateWritePamValueView()));
    disconnect(updateTimer, SIGNAL(timeout()), this, SLOT(updateMapPamValueView()));
}

void PamFactoryWin::startPamUpdate()
{
    if(updateTimer == NULL)
    {
        updateTimer = new QTimer(this);
    }
    connect(updateTimer, SIGNAL(timeout()), this, SLOT(updatePamValueView()));
    connect(updateTimer, SIGNAL(timeout()), this, SLOT(updateWritePamValueView()));
    connect(updateTimer, SIGNAL(timeout()), this, SLOT(updateMapPamValueView()));
    updateTimer->setInterval(100);

    updateTimer->start();
}

void PamFactoryWin::updatePamValueView()
{
    //if(!pamActiveFlag)
    //    return;

    QStringList keys = Globals::funPamListHash.uniqueKeys();

    for(int i = 0; i < keys.count(); i++)
    {
        QString key = keys.at(i);
        QList<PARAM*> pams = Globals::funPamListHash.value(key);

        QStandardItem *keyItem = pamModel->findItems(key).at(0);

        for(int j = 0; j < pams.count(); j++)
        {
            PARAM *pam = pams.at(j);
            qreal value = pam->getValue();
            QStandardItem *valueItem = keyItem->child(j,1);
            valueItem->setText(QString::number(value, 'g', 6));
        }
    }

}

void PamFactoryWin::updateWritePamValueView()
{

    //if(!pamActiveFlag)
    //    return;

    QStringList keys = Globals::funWritePamListHash.uniqueKeys();

    for(int i = 0; i < keys.count(); i++)
    {
        QString key = keys.at(i);
        QList<PARAM*> pams = Globals::funWritePamListHash.value(key);

        QStandardItem *keyItem = pamModel->findItems(key).at(0);

        for(int j = 0; j < pams.count(); j++)
        {
            PARAM *pam = pams.at(j);
            qreal value = pam->getValue();
            QStandardItem *valueItem = keyItem->child(j,1);
            valueItem->setText(QString::number(value, 'g', 6));
        }
    }
}

void PamFactoryWin::updateMapPamValueView()
{
    //if(!pamActiveFlag)
    //    return;

    QStringList keys = Globals::funMapPamListHash.uniqueKeys();

    for(int i = 0; i < keys.count(); i++)
    {
        QString key = keys.at(i);
        QList<PARAM*> pams = Globals::funMapPamListHash.value(key);

        QStandardItem *keyItem = pamModel->findItems(key).at(0);

        for(int j = 0; j < pams.count(); j++)
        {
            PARAM *pam = pams.at(j);
            qreal value = pam->getValue();
            QStandardItem *valueItem = keyItem->child(j,1);
            valueItem->setText(QString::number(value, 'g', 6));
        }
    }
}

void PamFactoryWin::on_actionExpand_triggered()
{
    if(isAllExpanded)
    {
        ui->treeView_Pam->collapseAll();
        isAllExpanded = false;
    }
    else
    {
        ui->treeView_Pam->expandAll();
        isAllExpanded = true;
    }
}
