#include "win_wt3000.h"
#include "ui_win_wt3000.h"
#include "wt3000thread.h"
#include <QDir>
#include <QDebug>
//#include <QSqlDatabase>
//#include <QSqlError>
//#include <QSqlQuery>
#include <QSettings>
#include <QTimer>


WIN_WT3000::WIN_WT3000(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::WIN_WT3000),
    thread(new WT3000Thread)
{
    ui->setupUi(this);
    setWindowTitle("WT3000");
    init();

    //connectDevice();
}

WIN_WT3000::~WIN_WT3000()
{
    disconnectDevice();
    deleteSM();
    delete ui;
}

void WIN_WT3000::dataUpdatedSlot()
{
    if(!sm_wt3000) return;

    char buf[200];

    copyFromSharedMemory(sm_wt3000, 0, buf, 200);

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

void WIN_WT3000::init()
{
    /*
    if(loadDefaultConfig())
    {
        thread->setIPAddress(ui->IPAddressEdit->text());
    }
    */

    initTable();

    createSM();

    if(!timer)
    {
        timer = new QTimer();
    }
    timer->setInterval(200);
    connect(timer, &QTimer::timeout,this, &WIN_WT3000::dataUpdatedSlot);
    timer->start();

    readSetting();
    thread->setIPAddress(ui->IPAddressEdit->text());
    createConnections();
}

void WIN_WT3000::createConnections()
{
    //connect(ui->action_Connect,&QAction::triggered,this,&WIN_WT3000::saveDefaultConfig);
    connect(ui->action_Connect,&QAction::triggered,this,&WIN_WT3000::connectDevice);
    connect(ui->action_Disconnect,&QAction::triggered,this,&WIN_WT3000::disconnectDevice);
    connect(thread,&WT3000Thread::reconnect,this,&WIN_WT3000::reconnectDevice);
}

void WIN_WT3000::connectDevice()
{
    if(thread->connectDevice())
    {
        ui->action_Connect->setDisabled(true);
        ui->action_Disconnect->setEnabled(true);
    }
    writeSetting();
}

void WIN_WT3000::disconnectDevice()
{
    if(thread->disconnectDevice())
    {
        ui->action_Connect->setEnabled(true);
        ui->action_Disconnect->setDisabled(true);
    }
}

void WIN_WT3000::reconnectDevice()
{
    disconnectDevice();
    connectDevice();
}

void WIN_WT3000::writeSetting()
{
    QString appPath = QApplication::applicationDirPath();
    QString setFilePath = appPath + "/setting/wt3000_setting.ini";

    QSettings setting(setFilePath, QSettings::IniFormat);

    setting.setValue("IP_Address", ui->IPAddressEdit->text());
}

void WIN_WT3000::readSetting()
{
    QString appPath = QApplication::applicationDirPath();
    QString setFilePath = appPath + "/setting/wt3000_setting.ini";

    QSettings setting(setFilePath, QSettings::IniFormat);

    ui->IPAddressEdit->setText(setting.value("IP_Address").toString());
}

void WIN_WT3000::createSM()
{
    if(!sm_wt3000)
    {
        sm_wt3000 = new QSharedMemory("RP_WT3000");
    }
    sm_wt3000->create(200);

    thread->sm_wt3000 = sm_wt3000;
}

void WIN_WT3000::deleteSM()
{
    if(sm_wt3000)
    {
        sm_wt3000->detach();

        delete sm_wt3000;
        sm_wt3000 = NULL;
    }
}

void WIN_WT3000::initTable()
{
    pamNameList << "I_1" << "U_1" << "I_2" << "U_2" << "I_3" << "U_3"
                << "P_1" << "Q_1" << "P_2" << "Q_2" << "P_3" << "Q_3"
                << "ETA_1" << "ETA_2" << "ETA_3" << "P_Sigma" << "I_4" << "U_4"
                << "P_4" << "Q_4" << "S_1" << "S_2" << "S_3" << "S_4"
                << "PF_1" << "PF_2" << "PF_3" << "PF_4" << "PHI_1" << "PHI_2"
                << "PHI_3" << "PHI_4" << "SPE" << "TORQ" << "S_4" << "S_4"
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

void WIN_WT3000::fromRawDataToPamValue(char *data)
{
    if(!data) return;

    if(pamModel->rowCount() != pamNameList.count()) return;

    for(int i = 0; i < pamNameList.count(); i++)
    {
        char dataIntel[4];
        reverseData_4bytes(data + 8 + i * 4, dataIntel);

        float value = *(float*)dataIntel;
        QString pamName = pamNameList.at(i);

        //qDebug()<<pamNameList.at(i)<<":"<<value;

        QStandardItem *valueItem = pamModel->item(i, 1);
        valueItem->setText(QString::number(value, 'g', 6));
    }
}

void WIN_WT3000::reverseData_4bytes(char *src, char *dst)
{
    if(!src || !dst) return;

    dst[0] = src[3];
    dst[1] = src[2];
    dst[2] = src[1];
    dst[3] = src[0];
}

/*
bool WIN_WT3000::openDataBase()
{
    QString dbName=qApp->applicationDirPath()+"/config/ConfigDB";
    QSqlDatabase db ;
    if(QSqlDatabase::contains("qt_sql_default_connection"))
      db = QSqlDatabase::database("qt_sql_default_connection");
    else
      db = QSqlDatabase::addDatabase("QSQLITE");
    db.setHostName("host");
    db.setUserName("username");
    db.setPassword("password");
    db.setDatabaseName(dbName);
    if (!db.open())
    {
       qInfo()<<Q_FUNC_INFO<<"Can't open ConfigDB,check the database file.";
        return false;
    }
    db.exec("PRAGMA synchronous = OFF");
    db.exec("PRAGMA journal_mode = MEMORY");
    return true;
}

bool WIN_WT3000::openConfigTable()
{
    if (openDataBase())
    {
        QSqlQuery query;
        //判断表是否已经存在
        query.prepare("select * from sqlite_master where name='WT3000ConfigTable'");
        query.exec();
        if(query.next())
            return true;
        if (!query.exec(QString("CREATE TABLE WT3000ConfigTable("
                        "IPAddress VARCHAR)")))
        {
            qInfo()<<Q_FUNC_INFO<<"Select SQLite WT3000ConfigTable Error: "<<query.lastError().text();
            return false;
        }
     }
    else
        return false;
    return true;
}

void WIN_WT3000::saveDefaultConfig()
{
    if (!openConfigTable())
    {
        qInfo()<<Q_FUNC_INFO<<"WT3000Commu: Can't save WT3000config.";
        return;
    }
    QSqlQuery query;
    query.prepare("select * from WT3000ConfigTable ");
    query.exec();
    if(query.next())
    {//如果存在配置，更新配置
        query.finish();
        query.prepare("update WT3000ConfigTable set IPAddress = :IPAddress where rowid = 1");
        query.bindValue(":IPAddress",ui->IPAddressEdit->text());
        query.exec();
    }
    else
    {//如果不存在配置，添加配置
        query.finish();
        query.prepare("insert into WT3000ConfigTable values(:IPAddress)");
        query.bindValue(":IPAddress",ui->IPAddressEdit->text());
        query.exec();
    }
}

bool WIN_WT3000::loadDefaultConfig()
{
    if(!openConfigTable())
    {
        qInfo()<<Q_FUNC_INFO<<"Can't find WT3000Commu config table!";
        return false;
    }
    else
    {
        QSqlQuery query;
        query.exec("SELECT IPAddress FROM WT3000ConfigTable");
        while (query.next())
        {
            QString IPAddress = query.value(0).toString();
            ui->IPAddressEdit->setText(IPAddress);
        }
        return true;
    }
}
*/
