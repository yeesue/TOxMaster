#include "auto_win.h"
#include "ui_auto_win.h"
#include <QDebug>

Auto_Win::Auto_Win(QWidget *parent, QString name) :
    QMainWindow(parent),
    workName(name),
    ui(new Ui::Auto_Win)
{
    ui->setupUi(this);
    setWindowTitle("Auto");

    autoModel = new QStandardItemModel();
    ui->fileList->setModel(autoModel);
    ui->fileList->setAlternatingRowColors(true);
    autoModel->setHeaderData(0, Qt::Horizontal, tr("Automation"));


    jsModel = new QStandardItemModel();
    ui->jsScriptList->setModel(jsModel);
    ui->jsScriptList->setAlternatingRowColors(true);
    jsModel->setHeaderData(0, Qt::Horizontal, tr("JS_Script"));

    QString appPath = QApplication::applicationDirPath();
    notePadPath = appPath + "/notepad++/notepad++.exe";
    notePadProcess = new QProcess(this);

    getAutoFiles();
    getjsScriptFiles();

    ui->fileList->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(ui->fileList, SIGNAL(doubleClicked(QModelIndex)), this, SLOT(fileListDoubleClickedSlot(QModelIndex)));

    ui->fileTable->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(ui->fileTable, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(showMenuInFTable(QPoint)));

    ui->jsScriptList->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(ui->jsScriptList, SIGNAL(doubleClicked(QModelIndex)), this, SLOT(jsFileListDoubleClickedSlot(QModelIndex)));


}

Auto_Win::~Auto_Win()
{
    delete autoModel;
    delete ui;
}

void Auto_Win::showMenuInFTable(QPoint pos)
{
    QMenu *menu = new QMenu(ui->fileTable);

    QAction *actionAdd = new QAction("Add", ui->fileTable);
    QAction *actionInsert = new QAction("Insert Before", ui->fileTable);
    QAction *actionRemove = new QAction("Remove", ui->fileTable);

    connect(actionAdd, SIGNAL(triggered()), this, SLOT(insertFunSlot()));
    connect(actionInsert, SIGNAL(triggered()), this, SLOT(insertFunBeforeSlot()));
    connect(actionRemove, SIGNAL(triggered()), this, SLOT(deleteFunSlot()));

    menu->addAction(actionAdd);
    menu->addAction(actionInsert);
    menu->addAction(actionRemove);

    menu->move(cursor().pos());
    menu->show();

    int x = pos.x();
    int y = pos.y();
    QModelIndex index = ui->fileTable->indexAt(QPoint(x, y));
    int row = index.row();
    int col = index.column();
    selRowInFile = row;
    selColInFile = col;
}

void Auto_Win::addRowSlot()
{
/*
    ui->table->insertRow(ui->table->rowCount());
    QString funName = scriptModel->itemFromIndex(ui->list->currentIndex())->text();
    int curRow = ui->table->rowCount()-1;

    int funIndex = funList.indexOf(funName);
    switch (funIndex) {
    case 0:
    {
        break;
    }
    case 1:
    {

        ui->table->setItem(curRow, 0, new QTableWidgetItem("0"));
        ui->table->setItem(curRow, 1, new QTableWidgetItem("0"));
        ui->table->setItem(curRow, 2, new QTableWidgetItem("5"));
        ui->table->setItem(curRow, 3, new QTableWidgetItem("0"));
        ui->table->setItem(curRow, 4, new QTableWidgetItem("5"));
        ui->table->setItem(curRow, 5, new QTableWidgetItem("0"));
        ui->table->setItem(curRow, 6, new QTableWidgetItem("300"));
        ui->table->setItem(curRow, 7, new QTableWidgetItem("1500"));
        break;
    }
    case 2:
    {
        ui->table->setItem(curRow, 0, new QTableWidgetItem("1000"));

        break;
    }
    default:
        break;
    }
*/
}

void Auto_Win::removeRowSlot()
{
    //ui->table->removeRow(ui->table->rowCount()-1);
}

void Auto_Win::pamChooseSlot()
{
    /*
    QString funName = scriptModel->itemFromIndex(ui->list->currentIndex())->text();

    if(funName != "PAM_SET" && !(funName.contains("CONDITION")))
        return;

    pamListDlg *pamDlg = new pamListDlg();
    pamDlg->setAttribute(Qt::WA_DeleteOnClose);
    connect(pamDlg, SIGNAL(pamListSelected(QList<PARAM*>)), this, SLOT(pamChooseHandle(QList<PARAM*>)));

    pamDlg->show();
    */

}

void Auto_Win::pamChooseHandle(QList<PARAM *> pams)
{
/*
    int pamIndex = 0;

    int pamCount = pams.count();
    if(pamCount == 0)
        return;

    int curRow = ui->table->currentIndex().row();

    for(int i = curRow; i < ui->table->rowCount(); i++)
    {
        if(pamCount <= pamIndex)
            return;

        PARAM *pam = pams.at(pamIndex);
        ui->table->setItem(i, 0, new QTableWidgetItem(pam->smKey));
        ui->table->setItem(i, 1, new QTableWidgetItem(pam->Name));
        ui->table->setItem(i, 2, new QTableWidgetItem(0));
        pamIndex++;
    }

*/
}

void Auto_Win::treePressedSlot(QModelIndex index)
{
    //ui->tree->resizeColumnToContents(index.row());
    //qDebug()<<"row="<<index.row();
    //QString selectedRowTxt = ui->tree->model()->itemData(index).values()[0].toString();
    //qDebug()<<"result=="<<selectedRowTxt;

    selRowInTree = index.row();
    selIndexInTree = index;
}

void Auto_Win::fileListDoubleClickedSlot(QModelIndex index)
{
    if(fileModel == NULL)
    {
        on_actionNew_triggered();

    }
    QString autoName = autoModel->itemFromIndex(index)->text();

    QString autoFilePath = autoFilePathHash.value(autoName);
    if(autoFilePath == curAutoFilePath)
        return;

    openAutoFilePath(autoFilePath);

}

void Auto_Win::openAutoFilePath(QString openFilePath)
{
    if(openFilePath.isEmpty())
        return;
    QString openFileName = openFilePath.split("/").last();
    qDebug()<<"open file name:"<<openFileName;

    on_actionNew_triggered();

    curAutoFilePath = openFilePath;

    QFile *csvAutoFile = new QFile(openFilePath);
    if(!csvAutoFile->open(QIODevice::ReadOnly))
    {
        qDebug()<<"Open csv auto file "<<openFileName<<" Error.";
        qDebug()<<csvAutoFile->errorString();
        delete csvAutoFile;

        return;
    }

    QTextStream in;
    in.setDevice(csvAutoFile);
    in.setCodec("GB2312");

    //int lineIndex = 0;
    while(!in.atEnd())
    {
        QString lineText = in.readLine();
        if(lineText.isEmpty())
        {
            continue;
        }

        QStringList dataText = lineText.split(",");
        QList<QStandardItem*> itemList;
        int index = 0;
        foreach (QString str, dataText) {
            QString data = str.trimmed();
            QStandardItem *dataItem = new QStandardItem(data);
            dataItem->setTextAlignment(Qt::AlignCenter);
            if(index == 0)
                dataItem->setFont(QFont("Microsoft YaHei", 8));
            itemList.append(dataItem);
            index++;
        }
        fileModel->appendRow(itemList);
    }


    csvAutoFile->close();

    delete csvAutoFile;
}

void Auto_Win::jsFileListDoubleClickedSlot(QModelIndex index)
{
    QString jsName = jsModel->itemFromIndex(index)->text();

    QString jsFilePath = jsFilePathHash.value(jsName);

    if (jsFilePath.isEmpty())
        return;
    //qDebug()<<"Script File Name:"<<scriptFilePath;

    QStringList args;
    args.append(jsFilePath);

    if(!notePadProcess)
    {
        notePadProcess = new QProcess(this);
    }
    notePadProcess->startDetached(notePadPath, args);

    if(!notePadProcess->waitForStarted())
    {
        return;
    }
}

void Auto_Win::cmdInfoSlot(QStringList info)
{
    qDebug()<< "insert cmd: " << info;

    QList<QStandardItem*> items;

    for(int i = 0; i < info.count(); i++)
    {
        QStandardItem *item = new QStandardItem(info.at(i));
        item->setFont(QFont("Microsoft YaHei", 8));
        item->setTextAlignment(Qt::AlignCenter);

        items.append(item);
    }

    if(insertRow == fileModel->rowCount())
    {
        fileModel->appendRow(items);
    }
    else
    {
        fileModel->insertRow(insertRow, items);
    }
}

void Auto_Win::insertFunSlot()
{

    if(!fileModel)
        return;

    insertRow = fileModel->rowCount();

    CMD_DLG *cmdDlg = new CMD_DLG(this);
    connect(cmdDlg, SIGNAL(cmdInfo(QStringList)), this, SLOT(cmdInfoSlot(QStringList)));
    cmdDlg->setAttribute(Qt::WA_DeleteOnClose);

    cmdDlg->show();

}

void Auto_Win::insertFunBeforeSlot()
{

    if(!fileModel)
        return;

    insertRow = selRowInFile;

    CMD_DLG *cmdDlg = new CMD_DLG(this);
    connect(cmdDlg, SIGNAL(cmdInfo(QStringList)), this, SLOT(cmdInfoSlot(QStringList)));
    cmdDlg->setAttribute(Qt::WA_DeleteOnClose);

    cmdDlg->show();

}

void Auto_Win::deleteFunSlot()
{

    if(!fileModel)
        return;
    int row = selRowInFile;
    int result = QMessageBox::warning(this,
                         tr("Delete Fun"),
                         QString::fromLocal8Bit("Confirm to delete fun? \n "
                            ""),
                         QMessageBox::Ok | QMessageBox::Cancel, QMessageBox::NoButton
                         );
    if (result == QMessageBox::Cancel || QMessageBox::NoButton)
    {
        return;
    }

    fileModel->removeRow(row);
}


void Auto_Win::getAutoFiles()
{
    QString appPath = QApplication::applicationDirPath();
    QString autoFilePath = appPath + "//work//" + workName + "//AutoFile";
    QDir *dir = new QDir(autoFilePath);

    QStringList nameFilter;
    nameFilter << "*.csv";
    QFileInfoList fileInfo = dir->entryInfoList(nameFilter);
    QStringList autoFileList;
    for (int i = 0; i < fileInfo.count(); i++)
    {
        QString filePath = fileInfo.at(i).filePath();
        QString shortFile = filePath.split('/').last();
        QString autoName = shortFile;
        autoName.replace(".csv", "");
        autoFilePathHash.insert(autoName, filePath);

        if(autoName.isEmpty())
            continue;

        autoFileList << filePath;

        QStandardItem *autoItem = new QStandardItem(autoName);
        autoItem->setIcon(QIcon(":/icon/icon/auto.png"));
        //autoItem->setCheckable(true);

        QList<QStandardItem*> itemList;
        itemList<<autoItem;

        autoModel->appendRow(itemList);

    }
}

void Auto_Win::getjsScriptFiles()
{
    QString appPath = QApplication::applicationDirPath();
    QString jsFilePath = appPath + "//work//" + workName + "//AutoFile//jsFile";
    QDir *dir = new QDir(jsFilePath);

    QStringList nameFilter;
    nameFilter << "*.js";
    QFileInfoList fileInfo = dir->entryInfoList(nameFilter);
    QStringList jsFileList;
    for (int i = 0; i < fileInfo.count(); i++)
    {
        QString filePath = fileInfo.at(i).filePath();
        QString shortFile = filePath.split('/').last();
        QString jsName = shortFile;
        jsName.replace(".js", "");
        jsFilePathHash.insert(jsName, filePath);

        if(jsName.isEmpty())
            continue;

        jsFileList << filePath;

        QStandardItem *jsItem = new QStandardItem(jsName);
        jsItem->setIcon(QIcon(":/icon/icon/fun.png"));
        //jsItem->setCheckable(true);

        QList<QStandardItem*> itemList;
        itemList<<jsItem;

        jsModel->appendRow(itemList);

    }
}

void Auto_Win::setWorkName(const QString &value)
{
    workName = value;
}

void Auto_Win::on_actionReload_triggered()
{

    jsModel->clear();
    jsFilePathHash.clear();
    getjsScriptFiles();

    autoModel->clear();
    autoFilePathHash.clear();
    getAutoFiles();
}

void Auto_Win::on_actionAdd_triggered()
{
    insertFunSlot();
}

void Auto_Win::on_actionDelete_triggered()
{
    if(!fileModel)
        return;

    QModelIndex curIndex = ui->fileTable->currentIndex();
    int row = curIndex.row();

    int result = QMessageBox::warning(this,
                         tr("删除命令"),
                         tr("确认删除所选命令? \n "
                            ""),
                         QMessageBox::Ok | QMessageBox::Cancel, QMessageBox::NoButton
                         );
    if (result == QMessageBox::Cancel || QMessageBox::NoButton)
    {
        return;
    }

    fileModel->removeRow(row);

}

void Auto_Win::on_actionNew_triggered()
{

    if(fileModel)
    {
        fileModel->clear();
        delete fileModel;
        fileModel = NULL;
    }
    QStandardItemModel *autoFileModel = new QStandardItemModel();
    autoFileModel->setColumnCount(12);
    ui->fileTable->setModel(autoFileModel);
    ui->fileTable->setShowGrid(true);

    QStringList headers;
    headers<<tr("命令")<<tr("参数0")<<tr("参数1")<<tr("参数2")<<tr("参数3")<<tr("参数4")
                <<tr("参数5")<<tr("参数6")<<tr("参数7")<<tr("参数8")<<tr("参数9")<<tr("说明");
    autoFileModel->setHorizontalHeaderLabels(headers);

    fileModel = autoFileModel;

    curAutoFilePath = "";

}
void Auto_Win::on_actionclearFile_triggered()
{

    if(!fileModel)
        return;
    int result = QMessageBox::warning(this,
                         tr("Clear File"),
                         tr("Confirm to clear all? \n "
                            ""),
                         QMessageBox::Ok | QMessageBox::Cancel, QMessageBox::NoButton
                         );
    if (result == QMessageBox::Cancel || QMessageBox::NoButton)
    {
        return;
    }


    fileModel->clear();
    fileModel->setColumnCount(12);
    ui->fileTable->setModel(fileModel);

    QStringList headers;
    headers<<tr("命令")<<tr("参数0")<<tr("参数1")<<tr("参数2")<<tr("参数3")<<tr("参数4")
                <<tr("参数5")<<tr("参数6")<<tr("参数7")<<tr("参数8")<<tr("参数9")<<tr("说明");
    fileModel->setHorizontalHeaderLabels(headers);
    ui->fileTable->setShowGrid(true);
}

void Auto_Win::on_actionSave_triggered()
{
    if(!fileModel)
        return;

    QString baseDir = QApplication::applicationDirPath() + "//work//" + workName + "//AutoFile";
    QString saveFilePath = QFileDialog::getSaveFileName(this,
                                 tr("Save auto file..."),
                                 baseDir,
                                 tr("CSV files (*.csv)"));
    if(saveFilePath.isEmpty())
        return;
    QFileInfo file(saveFilePath);
    if(file.exists())
    {
        QFile::remove(saveFilePath);
    }

    curAutoFilePath = saveFilePath;
    QString saveFileName = saveFilePath.split("/").last();

    qDebug()<<"save file name:"<<saveFileName;

    QFile *csvAutoFile = new QFile(saveFilePath);
    if(!csvAutoFile->open(QIODevice::ReadWrite))
    {
        qDebug()<<"Save csv auto file "<<saveFileName<<" Error.";
        qDebug()<<csvAutoFile->errorString();
        delete csvAutoFile;

        return;
    }

    QTextStream out;
    out.setDevice(csvAutoFile);
    out.setCodec("GB2312");

    // [Collect model data to QString]
    QString textData;
    int rows = fileModel->rowCount();
    int columns = fileModel->columnCount();

    for (int i = 0; i < rows; i++)
    {
        for (int j = 0; j < columns; j++)
        {

          textData += fileModel->data(fileModel->index(i,j)).toString();
          if(j != columns-1)
              textData += ", "; // for .csv file format
        }
        textData += "\n";    // (optional: for new line segmentation)
    }

    out << textData;

    out.flush();
    csvAutoFile->close();

    delete csvAutoFile;
}

void Auto_Win::on_actionOpen_triggered()
{
    QString baseDir = QApplication::applicationDirPath() + "//work//" + workName + "//AutoFile";
    QString openFilePath = QFileDialog::getOpenFileName(this,
                                 tr("Open auto file..."),
                                 baseDir,
                                 tr("CSV files (*.csv)"));

    openAutoFilePath(openFilePath);

}

void Auto_Win::on_actioncmdEdit_triggered()
{
    if(!cmdWin)
    {
        cmdWin = new CMD_DLG(this);
    }

    cmdWin->show();
}

void Auto_Win::on_actionNotePad_triggered()
{
    QStringList args;

    QString scriptDir = QApplication::applicationDirPath() + "//work//" + workName + "//autoFile//jsFile";
    args << scriptDir;

    if(!notePadProcess)
    {
        notePadProcess = new QProcess(this);
    }
    notePadProcess->startDetached(notePadPath, args);

    if(!notePadProcess->waitForStarted())
    {
        //qInfo()<<"Fail to start "<<program;
        //qInfo()<<"Error:"<<process->errorString();
        return;
    }
}
