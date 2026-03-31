#include "tablewin.h"
#include "ui_tablewin.h"
#include "delegate.h"
#include <QLineEdit>
#include <QMenu>

TableWin::TableWin(QWidget *parent, QString name, int mode) :
    QMainWindow(parent),
    winName(name),
    winMode(mode),
    ui(new Ui::TableWin)
{
    ui->setupUi(this);
    setWindowTitle(winName);

    //setAttribute(Qt::WA_TranslucentBackground, true);

    QStringList headers;
    headers << "参数" << "值" << "单位";

    QTableView *table = ui->tableView;

    model = new QStandardItemModel();
    table->setModel(model);
    table->setContextMenuPolicy(Qt::CustomContextMenu);
    table->setEditTriggers(winMode ? QTableView::DoubleClicked : QTableView::NoEditTriggers);
    table->setSelectionBehavior(QTableView::SelectRows);
    table->setSelectionMode(QTableView::ExtendedSelection);
    table->setAlternatingRowColors(true);
    table->setFocusPolicy(Qt::NoFocus);

    table->horizontalHeader()->setStretchLastSection(true);
    table->horizontalHeader()->setDefaultAlignment(Qt::AlignCenter);

    model->setHorizontalHeaderLabels(headers);
    model->setColumnCount(3);
    table->horizontalHeader()->resizeSection(0, 200);
    table->horizontalHeader()->resizeSection(1, 120);

    DoubleSpinBoxDelegate *dsbDelegate = new DoubleSpinBoxDelegate();
    table->setItemDelegate(dsbDelegate);

    connect(table, &QTableView::customContextMenuRequested, this, &TableWin::Slt_ContextMenuRequest);
    connect(dsbDelegate, &DoubleSpinBoxDelegate::modelDataUpdated, this, &TableWin::Slt_ModelDataUpdated);
    //connect(table, SIGNAL(doubleClicked(QModelIndex)), this, SLOT(Slt_DoubleClicked(QModelIndex)));

    if(winMode == 0)
    {
        updateTimer = new QTimer();
        updateTimer->setInterval(200);
        connect(updateTimer, &QTimer::timeout, this, &TableWin::Slt_Updated);

        updateTimer->start();

        setWindowIcon(QIcon(":/icon/icon/tableWin.png"));
    }
    else if(winMode == 1)
    {
        setWindowIcon(QIcon(":/icon/icon/tableWinWrite.png"));
    }
}

TableWin::~TableWin()
{
    delete ui;
}

void TableWin::setWinName(const QString &value)
{
    winName = value;
}

void TableWin::Slt_ContextMenuRequest(QPoint pos)
{
    QMenu *menu = new QMenu(this);
    menu->setAttribute(Qt::WA_DeleteOnClose);

    menu->addAction("Add Pams", this, SLOT(Slt_AddPams()));
    menu->addAction("Del Pams", this, SLOT(Slt_DelPams()));
    menu->addAction("Rename", this, SLOT(Slt_Rename()));
    menu->addAction("Up", this, SLOT(Slt_UpPam()));
    menu->addAction("Down", this, SLOT(Slt_DownPam()));


    menu->popup(ui->tableView->mapToGlobal(pos));
}

void TableWin::Slt_ModelDataUpdated(int index, double value)
{
    /*
    A2L_VarChar *charVar = charPamList.at(index);
    if(charVar != NULL)
    {
        charVar->setValue(value);
        emit charValueUpdated(charVar, value);
    }
    */

    PARAM *pam = writePamList.at(index);
    PamInfo pamInfo = writePams.at(index);
    if(pam != NULL)
    {
        pam->setValue(value);

        pamInfo.pamSetValue = value;
        writePams.replace(index, pamInfo);

        //emit pamValueUpdated(pam, value);
    }
}


void TableWin::Slt_Updated()
{
    if(pamList.isEmpty() || model->rowCount() != pamList.count())
        return;

    for(int i = 0; i < pamList.count(); i++)
    {
        PARAM *pam = pamList.at(i);
        if(!pam)
            continue;
        PamInfo pamInfo = readPams.at(i);

        QStandardItem *valueItem = model->item(i, 1);
        valueItem->setText(QString::number(pam->getValue(), 'f', pamInfo.decimal));
    }
}

void TableWin::Slt_AddMeas(QList<A2L_VarMeas *> pams)
{
    if(pams.isEmpty())
        return;

    measPamList.append(pams);

    foreach (A2L_VarMeas *pam, pams) {
        if(!pam)
            continue;

        QStandardItem *nameItem = new QStandardItem(pam->Name);

        QStandardItem *valueItem = new QStandardItem(pam->getValue());
        valueItem->setTextAlignment(Qt::AlignCenter);

        QStandardItem *unitItem = new QStandardItem(pam->Unit);
        unitItem->setTextAlignment(Qt::AlignCenter);

        QList<QStandardItem*> itemList;
        itemList << nameItem << valueItem << unitItem;

        model->appendRow(itemList);

        PamInfo pamInfo;
        pamInfo.modName = pam->smKey;
        pamInfo.pamName = pam->Name;
        measPams.append(pamInfo);
    }
    ui->tableView->resizeColumnToContents(0);
    //ui->tableView->resizeColumnToContents(1);

    emit winUpdated();
}

void TableWin::Slt_AddChar(QList<A2L_VarChar *> pams)
{
    if(pams.isEmpty())
        return;

    charPamList.append(pams);

    foreach (A2L_VarChar *pam, pams) {
        if(!pam)
            continue;

        QStandardItem *nameItem = new QStandardItem(pam->Name);
        nameItem->setData(QVariant::fromValue(pam), Qt::UserRole);

        QStandardItem *valueItem = new QStandardItem(QString::number(pam->getValue(), 'g', 8));
        valueItem->setData(QVariant::fromValue(pam), Qt::UserRole);
        valueItem->setTextAlignment(Qt::AlignCenter);

        QStandardItem *unitItem = new QStandardItem(pam->Unit);
        unitItem->setData(QVariant::fromValue(pam), Qt::UserRole);
        unitItem->setTextAlignment(Qt::AlignCenter);

        QList<QStandardItem*> itemList;
        itemList << nameItem << valueItem << unitItem;

        model->appendRow(itemList);

        PamInfo pamInfo;
        pamInfo.modName = pam->smKey;
        pamInfo.pamName = pam->Name;
        charPams.append(pamInfo);
    }

    ui->tableView->resizeColumnToContents(0);

    emit winUpdated();
}

void TableWin::Slt_AddParams(QList<PARAM *> pams)
{
    if(pams.isEmpty())
        return;

    pamList.append(pams);

    foreach (PARAM *pam, pams) {
        if(!pam)
            continue;

        QStandardItem *nameItem = new QStandardItem(pam->Name);

        QStandardItem *valueItem = new QStandardItem(pam->getValue());
        valueItem->setTextAlignment(Qt::AlignCenter);

        QStandardItem *unitItem = new QStandardItem(pam->Unit);
        unitItem->setTextAlignment(Qt::AlignCenter);

        QList<QStandardItem*> itemList;
        itemList << nameItem << valueItem << unitItem;

        model->appendRow(itemList);

        PamInfo pamInfo;
        pamInfo.modName = pam->smKey;
        pamInfo.pamName = pam->Name;
        readPams.append(pamInfo);
    }
    ui->tableView->resizeColumnToContents(0);

    emit winUpdated();
}

void TableWin::Slt_AddWriteParams(QList<PARAM *> pams)
{
    if(pams.isEmpty())
        return;

    writePamList.append(pams);

    foreach (PARAM *pam, pams) {
        if(!pam) continue;
        //qDebug()<<"add pam item:"<<pam->Name;

        QStandardItem *nameItem = new QStandardItem(pam->Name);
        //nameItem->setData(QVariant::fromValue(pam), Qt::UserRole);

        QStandardItem *valueItem = new QStandardItem(QString::number(pam->getValue(), 'g', 8));
        valueItem->setData(QVariant::fromValue(pam), Qt::UserRole);
        valueItem->setTextAlignment(Qt::AlignCenter);

        QStandardItem *unitItem = new QStandardItem(pam->Unit);
        //unitItem->setData(QVariant::fromValue(pam), Qt::UserRole);
        unitItem->setTextAlignment(Qt::AlignCenter);

        QList<QStandardItem*> itemList;
        itemList << nameItem << valueItem << unitItem;

        model->appendRow(itemList);

        PamInfo pamInfo;
        pamInfo.modName = pam->smKey;
        pamInfo.pamName = pam->Name;
        pamInfo.pamSetValue = pam->getValue();
        writePams.append(pamInfo);

    }
    ui->tableView->resizeColumnToContents(0);

    emit winUpdated();
}

void TableWin::Slt_AddPams()
{
    FunPamDlg *funPamDlg = new FunPamDlg(nullptr, winMode);
    funPamDlg->setAttribute(Qt::WA_DeleteOnClose);
    funPamDlg->show();

    connect(funPamDlg, SIGNAL(pamListSelected(QList<PARAM*>)), this, SLOT(Slt_AddParams(QList<PARAM*>)));
    connect(funPamDlg, SIGNAL(writePamListSelected(QList<PARAM*>)), this, SLOT(Slt_AddWriteParams(QList<PARAM*>)));

}

void TableWin::Slt_DelPams()
{
    QModelIndexList indexList =  ui->tableView->selectionModel()->selectedIndexes();
    QList<int> selectedRows;
    for(int i = 0; i < indexList.count(); i++)
    {
        selectedRows.append(indexList.at(i).row());
    }

    qSort(selectedRows.begin(), selectedRows.end(), qGreater<int>());

    for(int i = 0; i < selectedRows.count(); i++)
    {
        if(winMode == 0)
        {
            //measPamList.removeAt(i);
            pamList.removeAt(i);
            readPams.removeAt(i);
        }
        else if(winMode == 1)
        {
            //charPamList.removeAt(i);
            //charPams.removeAt(i);

            writePamList.removeAt(i);
            writePams.removeAt(i);
        }
        model->removeRow(selectedRows.at(i));
    }

    emit winUpdated();
}

void TableWin::Slt_Rename()
{

}

void TableWin::Slt_UpPam()
{
    QModelIndex curIndex = ui->tableView->currentIndex();
    int curRow = curIndex.row();
    if(curRow == 0) return;



    if(winMode == 0)
    {
        updateTimer->stop(); // 停止数值刷新

        QList<QStandardItem*> itemList = model->takeRow(curRow);  //提取表格当前行
        model->insertRow(curRow - 1, itemList);  //插入上一行

        PARAM *pam = pamList.takeAt(curRow);
        pamList.insert(curRow - 1, pam);

        PamInfo pamInfo = readPams.takeAt(curRow);
        readPams.insert(curRow - 1, pamInfo);

        updateTimer->start();  // 重新开始数值刷新
    }
    else if(winMode == 1)
    {
        QList<QStandardItem*> itemList = model->takeRow(curRow);  //提取表格当前行
        model->insertRow(curRow - 1, itemList);  //插入上一行

        /*
        PARAM *curPam = writePamList.at(curRow);

        model->removeRow(curRow);  //删除表格当前行

        QStandardItem *nameItem = new QStandardItem(curPam->Name);

        QStandardItem *valueItem = new QStandardItem(QString::number(curPam->getValue(), 'g', 8));
        valueItem->setData(QVariant::fromValue(curPam), Qt::UserRole);
        valueItem->setTextAlignment(Qt::AlignCenter);

        QStandardItem *unitItem = new QStandardItem(curPam->Unit);
        unitItem->setTextAlignment(Qt::AlignCenter);

        QList<QStandardItem*> itemList;
        itemList << nameItem << valueItem << unitItem;

        model->insertRow(curRow - 1, itemList);  //插入上一行
        */

        PARAM *pam = writePamList.takeAt(curRow);
        writePamList.insert(curRow - 1, pam);

        PamInfo pamInfo = writePams.takeAt(curRow);
        writePams.insert(curRow - 1, pamInfo);

    }

}

void TableWin::Slt_DownPam()
{
    QModelIndex curIndex = ui->tableView->currentIndex();
    int curRow = curIndex.row();
    if(curRow == (model->rowCount() - 1)) return;


    if(winMode == 0)
    {
        updateTimer->stop(); // 停止数值刷新

        QList<QStandardItem*> itemList = model->takeRow(curRow);  //提取表格当前行
        model->insertRow(curRow + 1, itemList);  //插入下一行

        PARAM *pam = pamList.takeAt(curRow);
        pamList.insert(curRow + 1, pam);

        PamInfo pamInfo = readPams.takeAt(curRow);
        readPams.insert(curRow + 1, pamInfo);

        updateTimer->start();  // 重新开始数值刷新
    }
    else if(winMode == 1)
    {
        QList<QStandardItem*> itemList = model->takeRow(curRow);  //提取表格当前行
        model->insertRow(curRow + 1, itemList);  //插入下一行

        /*
        PARAM *curPam = writePamList.at(curRow);

        model->removeRow(curRow);  //删除表格当前行

        QStandardItem *nameItem = new QStandardItem(curPam->Name);

        QStandardItem *valueItem = new QStandardItem(QString::number(curPam->getValue(), 'g', 8));
        valueItem->setData(QVariant::fromValue(curPam), Qt::UserRole);
        valueItem->setTextAlignment(Qt::AlignCenter);

        QStandardItem *unitItem = new QStandardItem(curPam->Unit);
        unitItem->setTextAlignment(Qt::AlignCenter);

        QList<QStandardItem*> itemList;
        itemList << nameItem << valueItem << unitItem;

        model->insertRow(curRow + 1, itemList);  //插入下一行
        */

        PARAM *pam = writePamList.takeAt(curRow);
        writePamList.insert(curRow + 1, pam);

        PamInfo pamInfo = writePams.takeAt(curRow);
        writePams.insert(curRow + 1, pamInfo);

    }
}

void TableWin::reLoadPams()
{

    if(winMode == 0)
    {
        //qDebug()<<"reload pams in read table win"<<readPams.count();
        if(readPams.isEmpty()) return;

        addPamsByPamInfo();

    }
    else if(winMode == 1)
    {
        //qDebug()<<"reload pams in write table win"<<writePams.count();
        if(writePams.isEmpty()) return;

        addWritePamsByPamInfo();
    }
}

void TableWin::addMeasByPamInfo()
{
    QList<A2L_VarMeas*> pamList;
    for(int i = 0; i < measPams.count(); i++)
    {
        PamInfo pamInfo = measPams.at(i);

        QList<A2L_VarMeas*> pams = Globals::funMeasListHash.value(pamInfo.modName);

        foreach (A2L_VarMeas *pam, pams) {
            if(pam->Name == pamInfo.pamName)
            {
                pamList.append(pam);
                continue;
            }
        }
    }

    measPamList.clear();
    measPams.clear();
    model->removeRows(0, model->rowCount());

    Slt_AddMeas(pamList);
}

void TableWin::addCharsByPamInfo()
{
    QList<A2L_VarChar*> pamList;
    for(int i = 0; i < charPams.count(); i++)
    {
        PamInfo pamInfo = charPams.at(i);

        QList<A2L_VarChar*> pams = Globals::funCharListHash.value(pamInfo.modName);

        foreach (A2L_VarChar *pam, pams) {
            if(pam->Name == pamInfo.pamName)
            {
                pamList.append(pam);
                continue;
            }
        }
    }

    charPamList.clear();
    charPams.clear();
    model->removeRows(0, model->rowCount());

    Slt_AddChar(pamList);
}

void TableWin::addPamsByPamInfo()
{
    QList<PARAM*> pamList;
    for(int i = 0; i < readPams.count(); i++)
    {
        PamInfo pamInfo = readPams.at(i);

        QList<PARAM*> pams = Globals::funPamListHash.value(pamInfo.modName);

        foreach (PARAM *pam, pams) {
            if(pam->Name == pamInfo.pamName)
            {
                pamList.append(pam);
                break;
            }
        }
    }

    this->pamList.clear();
    readPams.clear();
    model->removeRows(0, model->rowCount());

    Slt_AddParams(pamList);
}

void TableWin::addWritePamsByPamInfo()
{
    QList<PARAM*> pamList;
    for(int i = 0; i < writePams.count(); i++)
    {
        PamInfo pamInfo = writePams.at(i);

        QList<PARAM*> pams = Globals::funWritePamListHash.value(pamInfo.modName);

        foreach (PARAM *pam, pams) {
            if(pam->Name == pamInfo.pamName)
            {
                pam->setValue(pamInfo.pamSetValue);
                pamList.append(pam);
                break;
            }
        }
    }

    this->writePamList.clear();
    writePams.clear();
    model->removeRows(0, model->rowCount());

    Slt_AddWriteParams(pamList);
}

void TableWin::on_actionExitAndDel_triggered()
{
    emit winDeleted(this);

    //setAttribute(Qt::WA_DeleteOnClose);
    //this->close();
}

QString TableWin::getWinName() const
{
    return winName;
}

void TableWin::Slt_Trigger(bool active)
{
    this->show();
}

void TableWin::Slt_RunActive(bool runActive)
{
    if(runActive)
    {
        reLoadPams();
    }
}

void TableWin::on_actionRename_triggered()
{

}

QList<PamInfo> TableWin::getWritePams() const
{

    return writePams;
}

void TableWin::setWritePams(const QList<PamInfo> &value)
{
    writePams = value;
}

QList<PamInfo> TableWin::getReadPams() const
{
    return readPams;
}

void TableWin::setReadPams(const QList<PamInfo> &value)
{
    readPams = value;
}

void TableWin::setCharPams(const QList<PamInfo> &value)
{
    charPams = value;
}

void TableWin::setMeasPams(const QList<PamInfo> &value)
{
    measPams = value;
}

QList<PamInfo> TableWin::getCharPams() const
{
    return charPams;
}

QList<PamInfo> TableWin::getMeasPams() const
{
    return measPams;
}


void TableWin::setFunCharListHash(const QHash<QString, QList<A2L_VarChar *> > &value)
{
    funCharListHash = value;
}

void TableWin::hideMenu()
{
    ui->menuBar->hide();
}

void TableWin::setFunMeasListHash(const QHash<QString, QList<A2L_VarMeas *> > &value)
{
    funMeasListHash = value;
}
