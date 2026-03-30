#include "win_wt5000.h"
#include "ui_win_wt5000.h"
#include "wt5000thread.h"
#include <QDir>
#include <QDebug>
//#include <QSqlDatabase>
//#include <QSqlError>
//#include <QSqlQuery>
#include <QSettings>


WIN_WT5000::WIN_WT5000(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::WIN_WT5000),
    thread(new WT5000Thread)
{
    ui->setupUi(this);

    setWindowTitle("WT5000");

    init();

    //connectDevice();
}

WIN_WT5000::~WIN_WT5000()
{
    disconnectDevice();
    deleteSM();

    delete ui;
}

void WIN_WT5000::dataUpdatedSlot()
{

    if(!sm_wt5000) return;

    char buf[200];

    copyFromSharedMemory(sm_wt5000, 0, buf, 200);

    QString dataHexStr = "";
    for (int i = 0; i < 200; i++)
    {
        QString hexStr = QString::number(static_cast<int>(buf[i]), 16).right(2).toUpper().leftJustified(2, '0') + " ";
        dataHexStr += hexStr;
    }
    QString rcvStr = QString::number(*(quint64*)buf);

    ui->receiveBufText->setText(dataHexStr);

    fromRawDataToPamValue(buf);
}

void WIN_WT5000::init()
{
    /*
    if(loadDefaultConfig())
    {
        writeXML();
        createShareMemory();
    }
    */
    initTable();

    createSM();

    if(!timer)
    {
        timer = new QTimer();
    }
    timer->setInterval(100);
    connect(timer, &QTimer::timeout,this, &WIN_WT5000::dataUpdatedSlot);
    timer->start();

    readSetting();
    thread->setIPAddress(ui->IPAddressEdit->text());
    createConnections();

}

void WIN_WT5000::createConnections()
{
    //connect(ui->action_Connect,&QAction::triggered,this,&WIN_WT5000::saveDefaultConfig);
    connect(ui->action_Connect,&QAction::triggered,this,&WIN_WT5000::connectDevice);
    connect(ui->action_Disconnect,&QAction::triggered,this,&WIN_WT5000::disconnectDevice);
    connect(thread,&WT5000Thread::reconnect,this,&WIN_WT5000::reconnectDevice);

}


void WIN_WT5000::createSM()
{
    //shareMemorySendToConsole=FN_CreateShareMemory("WT5000Commu_SendToConsole",200);
    //thread->sharedMemorySendToConsole=shareMemorySendToConsole;

    if(!sm_wt5000)
    {
        sm_wt5000 = new QSharedMemory("RP_WT5000");
    }
    sm_wt5000->create(200);

    thread->sm_wt5000 = sm_wt5000;

    //qDebug()<<"create sm done";
}

void WIN_WT5000::deleteSM()
{
    if(sm_wt5000)
    {
        sm_wt5000->detach();

        delete sm_wt5000;
        sm_wt5000 = NULL;
    }
}

void WIN_WT5000::initTable()
{
    pamNameList << "I_1" << "U_1" << "I_2" << "U_2" << "I_3" << "U_3"
                << "P_1" << "Q_1" << "P_2" << "Q_2" << "P_3" << "Q_3"
                << "ETA_1" << "ETA_2" << "ETA_3" << "P_Sigma" << "I_4" << "U_4"
                << "P_4" << "Q_4" << "S_1" << "S_2" << "S_3" << "S_4"
                << "PF_1" << "PF_2" << "PF_3" << "PF_4" << "PHI_1" << "PHI_2"
                << "PHI_3" << "PHI_4" << "S_4" << "S_4" << "S_4" << "S_4"
                << "S_4" << "S_4" << "S_4" << "S_4" << "S_4" << "S_4"
                << "S_4" << "S_4" << "S_4" << "S_4" << "S_4" << "S_4";


    if(!pamModel)
    {
        pamModel = new QStandardItemModel();
    }

    ui->tableView->setModel(pamModel);

    QStringList header;
    header << "参数" << "值";
    pamModel->setHorizontalHeaderLabels(header);

    pamModel->setColumnCount(2);
    ui->tableView->setColumnWidth(0, 200);
    ui->tableView->setColumnWidth(1, 150);

    for(int i = 0; i < pamNameList.count(); i++)
    {
        QString pamName = pamNameList.at(i);

        QList<QStandardItem*> itemList;
        QStandardItem *nameItem = new QStandardItem(pamName);
        QStandardItem *valueItem = new QStandardItem("0.00");
        nameItem->setTextAlignment(Qt::AlignCenter);
        valueItem->setTextAlignment(Qt::AlignCenter);

        itemList.append(nameItem);
        itemList.append(valueItem);

        pamModel->appendRow(itemList);

    }

}

void WIN_WT5000::fromRawDataToPamValue(char *data)
{
    if(!data) return;

    if(pamModel->rowCount() != pamNameList.count()) return;

    for(int i = 0; i < pamNameList.count(); i++)
    {
        char dataIntel[4];
        reverseData_4bytes(data + 8 + i * 4, dataIntel);

        float value = *(float*)dataIntel;

        //qDebug()<<pamNameList.at(i)<<":"<<value;

        QStandardItem *valueItem = pamModel->item(i, 1);
        valueItem->setText(QString::number(value, 'g', 6));
    }
}

void WIN_WT5000::reverseData_4bytes(char *src, char *dst)
{
    if(!src || !dst) return;

    dst[0] = src[3];
    dst[1] = src[2];
    dst[2] = src[1];
    dst[3] = src[0];
}


void WIN_WT5000::connectDevice()
{
    if(thread->connectDevice())
    {
        ui->action_Connect->setDisabled(true);
        ui->action_Disconnect->setEnabled(true);
    }
    timer->start();

    writeSetting();
}

void WIN_WT5000::disconnectDevice()
{

    if(thread->disconnectDevice())
    {
        ui->action_Connect->setEnabled(true);
        ui->action_Disconnect->setDisabled(true);
    }

}

void WIN_WT5000::reconnectDevice()
{
    disconnectDevice();
    connectDevice();
}

void WIN_WT5000::writeSetting()
{
    QString appPath = QApplication::applicationDirPath();
    QString setFilePath = appPath + "/setting/wt5000_setting.ini";

    QSettings setting(setFilePath, QSettings::IniFormat);

    setting.setValue("IP_Address", ui->IPAddressEdit->text());
}

void WIN_WT5000::readSetting()
{
    QString appPath = QApplication::applicationDirPath();
    QString setFilePath = appPath + "/setting/wt5000_setting.ini";

    QSettings setting(setFilePath, QSettings::IniFormat);

    ui->IPAddressEdit->setText(setting.value("IP_Address").toString());
}
