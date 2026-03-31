#include "mapwin.h"
#include "ui_mapwin.h"
#include <QDebug>



MapWin::MapWin(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MapWin)
{
    ui->setupUi(this);
    setWindowTitle("MAP_Win");

    model = new QStandardItemModel();
    delegate = new DSBDelegate();
    connect(delegate, &MapDelegate::modelDataUpdated, this, &MapWin::curMapDataUpdatedSlot);

}

MapWin::~MapWin()
{
    delete ui;
}


void MapWin::setMapList(QList<A2L_VarChar *> list)
{
    this->mapList = list;

    ui->comboBox_mapName->clear();

    for (int i = 0; i < mapList.count(); i++)
    {
        A2L_VarChar *map = mapList.at(i);
        ui->comboBox_mapName->addItem(map->Name);
    }

    ui->comboBox_mapName->setCurrentIndex(0);
}

void MapWin::setXcpConnected(bool connected)
{
    this->xcpConnected = connected;
}

void MapWin::currentFunTextChangedSlot(QString text)
{

    QList<A2L_VarChar*> charMapList = xcpFunMapListHash.value(text);

    updateMapList(charMapList);

    QHash<A2L_VarChar*, QSharedMemory*> mapCharSmHash = xcpFunMapSmHash.value(text);

    this->mapSmHash = mapCharSmHash;
}

void MapWin::currentMapIndexChangedSlot(int index)
{
    if(index >= mapList.count())
        return;

    A2L_VarChar *map = mapList.at(index);
    curMap = map;

    model->clear();

    int rowCount = (map->yCount == 0 ? map->yCount+2 : map->yCount+1);
    int colCount = map->xCount+1;

    model->setColumnCount(colCount);
    model->setRowCount(rowCount);

    int rowWidth = 20;
    int columnWidth = 50;

    ui->tableView_mapContent->setItemDelegate(delegate);

    QModelIndex curIndex = model->index(0, 0, QModelIndex());
    model->setData(curIndex, QVariant("Y/X"));
    model->item(0, 0)->setTextAlignment(Qt::AlignCenter);
    model->item(0, 0)->setBackground(Qt::gray);

    ui->tableView_mapContent->setColumnWidth(0, columnWidth);

    //qDebug()<<"yAix set";
    if(map->yCount > 0)
    {
        for(int i = 1; i < rowCount; i++)
        {
            curIndex = model->index(i, 0, QModelIndex());
            model->setData(curIndex, QVariant(map->hexValue_YList.at(i-1)));
            model->setItem(i, 0, new QStandardItem());
            model->item(i, 0)->setData(QVariant::fromValue(map), Qt::UserRole);
            model->item(i, 0)->setText(QString::number(map->hexValue_YList.at(i-1)));
            model->item(i, 0)->setTextAlignment(Qt::AlignCenter);
            model->item(i, 0)->setBackground(Qt::gray);
        }
    }

    //qDebug()<<"xAix set";
    for(int i = 1; i < colCount; i++)
    {
        curIndex = model->index(0, i, QModelIndex());
        model->setData(curIndex, QVariant(map->hexValue_XList.at(i-1)));
        model->setItem(0, i, new QStandardItem());
        model->item(0, i)->setData(QVariant::fromValue(map), Qt::UserRole);
        model->item(0, i)->setText(QString::number(map->hexValue_XList.at(i-1)));
        model->item(0, i)->setTextAlignment(Qt::AlignCenter);
        model->item(0, i)->setBackground(Qt::gray);

        ui->tableView_mapContent->setColumnWidth(i, columnWidth);

        //tableView->setColumnWidth(i, columnWidth);

    }

    //qDebug()<<"zVal set";
    for(int i = 1; i < rowCount; i++)
    {
        for(int j = 1; j < colCount; j++)
        {
            curIndex = model->index(i, j, QModelIndex());
            model->setData(curIndex, QVariant(map->hexValue_ZList.at((j-1)*map->yCount + i - 1)));
            model->setItem(i, j, new QStandardItem());
            model->item(i, j)->setData(QVariant::fromValue(map), Qt::UserRole);
            model->item(i, j)->setText(QString::number(map->hexValue_ZList.at((j-1)*map->yCount + i - 1)));
            model->item(i, j)->setTextAlignment(Qt::AlignCenter);
        }
    }



    ui->tableView_mapContent->setModel(model);
    ui->tableView_mapContent->setAlternatingRowColors(true);
    //resize(QSize(columnWidth*colCount + 100, rowWidth*rowCount + 200));
    //ui->tableView_mapContent->resizeColumnsToContents();

    ui->lineEdit_CharacType->setText(map->charType);

    ui->lineEdit_startAddress->setText("0x" + QString::number(map->ECUAddress, 16));
    ui->lineEdit_startAddress_AxisX->setText("0x" + QString::number(map->Axis_X_ECUAddress, 16));
    ui->lineEdit_startAddress_AxisY->setText("0x" + QString::number(map->Axis_Y_ECUAddress, 16));

    ui->lineEdit_Type->setText(map->ConversionType);
    ui->lineEdit_Type_AxisX->setText(map->Axis_X_ConversionType);
    ui->lineEdit_Type_AxisY->setText(map->Axis_Y_ConversionType);

    ui->lineEdit_DataType->setText(map->DataType);
    ui->lineEdit_DataType_AxisX->setText(map->Axis_X_DataType);
    ui->lineEdit_DataType_AxisY->setText(map->Axis_Y_DataType);

    ui->lineEdit_sizeX->setText(QString::number(map->Axis_X_DataSizeAG));
    ui->lineEdit_sizeY->setText(QString::number(map->Axis_Y_DataSizeAG));
    ui->lineEdit_sizeZ->setText(QString::number(map->DataSizeAG));

    ui->lineEdit_min->setText(QString::number(map->LowerLimit, 'g', 6));
    ui->lineEdit_max->setText(QString::number(map->UpperLimit, 'g', 6));

    //tableView->setWindowTitle(map->Name);
    //tableView->resizeColumnsToContents();
    //tableView->setAlternatingRowColors(true);
    //tableView->resize(QSize(columnWidth*(colCount+1), rowWidth*(rowCount+2)));
    //tableView->show();
    //mapTableViewHash.insert(map, tableView);



}

void MapWin::updateMapList(QList<A2L_VarChar*> list)
{
    //qDebug()<<"update map list.";

    disconnect(ui->comboBox_mapName, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &MapWin::currentMapIndexChangedSlot);

    //qDeleteAll(mapList);      //加了这句在第二次进行connect时程序会异常退出？

    this->mapList.clear();

    this->mapList = list;

    ui->comboBox_mapName->clear();

    for (int i = 0; i < mapList.count(); i++)
    {
        A2L_VarChar *map = mapList.at(i);
        ui->comboBox_mapName->addItem(map->Name);
    }
    ui->comboBox_mapName->setCurrentIndex(-1);

    connect(ui->comboBox_mapName, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &MapWin::currentMapIndexChangedSlot);

    //QComboBox 的connect函数需要在addItem等函数之后进行连接
    //connect(ui->comboBox_mapName, SIGNAL(currentIndexChanged(int)), this, SLOT(currentIndexChangedSlot(int)));

    //qDebug()<<"map list update done.";
}

void MapWin::setMapSmHash(QHash<A2L_VarChar *, QSharedMemory *> mapSmHash)
{
    this->mapSmHash = mapSmHash;

}

void MapWin::updateMapValueInSM(A2L_VarChar *map, quint16 index_x, quint16 index_y, char *data)
{

    //qDebug()<<"In update MapValue in SM."<<map->Name<<",index_x="<<index_x<<",index_y="<<index_y;
    if(!mapSmHash.keys().contains(map))
        return;

    QSharedMemory *sm = mapSmHash.value(map);

    if (!sm->isAttached())
    {
        if(!sm->attach())
        {
            qDebug()<<"unable attach to map sharedMemory :"<<sm->key();
            return;
        }
    }

    int index = 8;
    if(map->yCount == 0 && map->xCount != 0)
    {
        index += (map->DataSizeAG * index_x);
    }
    else
    {
        index += map->DataSizeAG * (map->yCount * index_x + index_y);
    }

    qDebug()<<"index offset="<<index;

    sm->lock();
    memcpy((char*)(sm->data())+index, data, map->DataSizeAG);
    sm->unlock();

    //qDebug()<<"map Value updated."<<map->Name<<",index_x="<<index_x<<", index_y="<<index_y;

}

void MapWin::modelDataUpdatedSlot(int row, int col, double value)
{
    qDebug()<<"delegate model update: row="<<row<<", col="<<col<<", value="<<value;
}

void MapWin::curMapDataUpdatedSlot(int row, int col, double value)
{
    if(!xcpConnected)
        return;

    if(mapList.isEmpty() || curMap == NULL)
        return;
    if(row == 0 && col == 0)
        return;

    qDebug()<<"mapName="<<curMap->Name<<", row="<<row<<", col="<<col<<",value="<<value;
    if(row == 0)
    {
        qDebug()<<"Axis value updated.";
    }
    else if (col == 0)
    {
        qDebug()<<"Yxis value updated.";
    }
    else
    {
        qDebug()<<"Z map value updated.";

        char *data = new char[curMap->DataSizeAG];
        transferPhyValueToRawData(curMap, value, data);

        updateMapValueInSM(curMap, col-1, row-1, data);
    }
}

void MapWin::transferPhyValueToRawData(A2L_VarChar *charVar, double value, char *data)
{
    if(charVar == NULL)
        return;
    QString dataType = charVar->DataType;
    int dataSize = charVar->DataSizeAG;

    int dataTypeIndex = dataTypeEnum.indexOf(dataType);

    switch (dataTypeIndex) {
    case 0:
    {
        quint8 val = (quint8)value;
        memcpy(data, (char*)&val, dataSize);
        break;
    }
    case 1:
    {
        qint8 val = (qint8)value;
        memcpy(data, (char*)&val, dataSize);
        break;
    }
    case 2:
    {
        quint16 val = (quint16)value;
        memcpy(data, (char*)&val, dataSize);
        break;
    }
    case 3:
    {
        qint16 val = (qint16)value;
        memcpy(data, (char*)&val, dataSize);
        break;
    }
    case 4:
    {
        quint32 val = (quint32)value;
        memcpy(data, (char*)&val, dataSize);
        break;
    }
    case 5:
    {
        qint32 val = (qint32)value;
        memcpy(data, (char*)&val, dataSize);
        break;
    }
    case 6:
    {
        quint64 val = (quint64)value;
        memcpy(data, (char*)&val, dataSize);
        break;
    }
    case 7:
    {
        qint64 val = (qint64)value;
        memcpy(data, (char*)&val, dataSize);
        break;
    }
    case 8:
    {
        float val = (float)value;
        memcpy(data, (char*)&val, dataSize);
        break;
    }
    case 9:
    {
        double val = (double)value;
        memcpy(data, (char*)&val, dataSize);
        break;
    }
    default:
        break;
    }

}

void MapWin::setXcpFunMapSmHash(const QHash<QString, QHash<A2L_VarChar *, QSharedMemory *> > &value)
{
    xcpFunMapSmHash = value;

    QStringList keys = value.uniqueKeys();

    if(keys.count() == 0)
        return;

    QString key_index0 = keys.at(0);
    currentFunTextChangedSlot(key_index0);
}

void MapWin::setXcpFunMapListHash(const QHash<QString, QList<A2L_VarChar *> > &value)
{
    //qDebug()<<"update fun name list.";
    xcpFunMapListHash = value;

    QStringList keys = value.uniqueKeys();

    ui->comboBox_funName->clear();
    ui->comboBox_mapName->clear();

    if(keys.count() == 0)
        return;

    ui->comboBox_funName->addItems(keys);

    connect(ui->comboBox_funName, &QComboBox::currentTextChanged, this, &MapWin::currentFunTextChangedSlot);

    QString key_index0 = keys.at(0);
    ui->comboBox_funName->setCurrentText(key_index0);
    updateMapList(xcpFunMapListHash.value(key_index0));

}


