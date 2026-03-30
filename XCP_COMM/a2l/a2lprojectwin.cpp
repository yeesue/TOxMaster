#include "a2lprojectwin.h"
#include "ui_a2lprojectwin.h"
#include <QFileDialog>
#include <QDir>
#include <QFileInfo>


A2LProjectWin::A2LProjectWin(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::A2LProjectWin)
{
    ui->setupUi(this);
    setWindowTitle("A2L_Project");

    a2lParser = new A2L_PARSER_INTERFACE(this);

    connect(ui->listWidget_A2L, SIGNAL(itemDoubleClicked(QListWidgetItem*)), this, SLOT(listItemDoubleClickedSlot(QListWidgetItem*)));


}

A2LProjectWin::~A2LProjectWin()
{
    delete ui;
}

QStringList A2LProjectWin::getProjectNameList()
{
    return a2lProjectMap.uniqueKeys();
}

A2L_Project *A2LProjectWin::getProjectByName(QString projectName)
{
    return a2lProjectMap.value(projectName);
}


void A2LProjectWin::addA2LProject(QString fullA2LName, QString fullHexName)
{
    //add a2l file
    if (fullA2LName.isEmpty() || fullA2LName.right(4).toLower() != ".a2l")
        return;

    QFileInfo a2lFileInfo(fullA2LName);
    if(!a2lFileInfo.isFile())
        return;

    QString shortA2LName = fullA2LName.split("/").last();
    shortA2LName = shortA2LName.replace(".a2l", "");

    if (a2lProjectMap.keys().indexOf(shortA2LName) != -1)
    {
        shortA2LName += "_2";
    }

    if (a2lParser == NULL)
        return;

    A2L_Project *a2lProject = new A2L_Project(fullA2LName, a2lParser);

    if (!a2lProject->getParsedStatus())
    {
        qDebug()<<"A2L File Parsed Error.";
        return;
    }
    a2lProjectMap.insert(shortA2LName, a2lProject);
    curA2LProject = a2lProject;

    a2lFileMap.insert(shortA2LName, fullA2LName);
    ui->listWidget_A2L->addItem(shortA2LName);
    currentA2LFileName = a2lFileMap.value(shortA2LName);

    QTreeWidgetItem *a2lItem = new QTreeWidgetItem();
    a2lItem->setText(0, shortA2LName);
    a2lItem->setIcon(0, QIcon(":/icons/A2L.png"));
    ui->treeWidget_A2LHex->addTopLevelItem(a2lItem);

    QStringList infoList = a2lProject->getParsedInfo();
    qDebug()<<"A2L Parser Info:==========================";
    for(int i = 0; i < infoList.count(); i++)
        qDebug()<<infoList.at(i);

    //add hex file
    if(a2lFileMap.keys().indexOf(shortA2LName) == -1)
        return;

    if (fullHexName.isEmpty() || fullHexName.right(4).toLower() != ".hex")
        return;

    QString shortHexName = fullHexName.split("/").last();
    shortHexName = shortHexName.replace(".hex", "");

    A2L_Project *curProject = a2lProjectMap.value(shortA2LName);

    bool status = curProject->addHexFile(fullHexName);
    if (!status)
        return;

    QTreeWidgetItem *hexItem = new QTreeWidgetItem();
    hexItem->setText(0, shortHexName);
    hexItem->setIcon(0, QIcon(":/icons/Hex.png"));
    a2lItem->addChild(hexItem);

    ui->treeWidget_A2LHex->expandAll();
}

void A2LProjectWin::on_actionAddA2LFile_triggered()
{
    const QString fullFileName =
        QFileDialog::getOpenFileName(this,
                                      tr("select an Asap file (A2L)"), QDir::currentPath(),
                                      tr("ASAP files (*.a2l);;all files (*.*)"));
    if (fullFileName.isEmpty())
        return;
    QString shortFileName = fullFileName.split("/").last();
    shortFileName = shortFileName.replace(".a2l", "");

    if (a2lProjectMap.keys().indexOf(shortFileName) != -1)
    {
        shortFileName += "_2";
    }

    if (a2lParser == NULL)
        return;
    A2L_Project *a2lProject = new A2L_Project(fullFileName, a2lParser);

    if (!a2lProject->getParsedStatus())
    {
        qDebug()<<"A2L File Parsed Error.";
        return;
    }
    a2lProjectMap.insert(shortFileName, a2lProject);
    curA2LProject = a2lProject;

    a2lFileMap.insert(shortFileName, fullFileName);
    ui->listWidget_A2L->addItem(shortFileName);
    currentA2LFileName = a2lFileMap.value(shortFileName);

    QTreeWidgetItem *a2lItem = new QTreeWidgetItem();
    a2lItem->setText(0, shortFileName);
    a2lItem->setIcon(0, QIcon(":/icons/A2L.png"));
    //root->addChild(a2lItem);
    ui->treeWidget_A2LHex->addTopLevelItem(a2lItem);

    QStringList infoList = a2lProject->getParsedInfo();
    qDebug()<<"A2L Parser Info:==========================";
    for(int i = 0; i < infoList.count(); i++)
        qDebug()<<infoList.at(i);
}

void A2LProjectWin::on_actionShowA2LWin_triggered()
{
    if (a2lParser)
        a2lParser->show_A2LParserWin();
}

void A2LProjectWin::closeEvent(QCloseEvent *event)
{
    emit winClosed();

    QMainWindow::closeEvent(event);
}

void A2LProjectWin::on_actionDelA2LFile_triggered()
{
    QListWidgetItem *item = ui->listWidget_A2L->selectedItems().at(0);
    if (item == NULL)
        return;
    int index = ui->listWidget_A2L->currentRow();
    QString selA2LProject =  item->text();
    QString fullFileName = a2lFileMap.value(selA2LProject);
    ui->listWidget_A2L->takeItem(index);
    a2lProjectMap.remove(selA2LProject);
    delete(item);

    emit delA2LFile(fullFileName);
}

void A2LProjectWin::listItemDoubleClickedSlot(QListWidgetItem *item)
{
    QString shortName = item->text();
    A2L_Project *a2lProject = a2lProjectMap.value(shortName);
    if (a2lProject == NULL)
        return;

    //XCPWIN *xcpWin = new XCPWIN(this, a2lProject);
    //xcpWin->show();
}


void A2LProjectWin::on_actionAddHEXFile_triggered()
{
    QTreeWidgetItem *item = ui->treeWidget_A2LHex->currentItem();
    if (item == NULL)
        return;

    QString selA2LShortName = item->text(0);
    if(a2lFileMap.keys().indexOf(selA2LShortName) == -1)
        return;

    const QString fullHexName =
        QFileDialog::getOpenFileName(this,
                                      tr("select an Code file (Hex)"), QDir::currentPath(),
                                      tr("Hex files (*.hex);;all files (*.*)"));
    if (fullHexName.isEmpty())
        return;

    QString shortHexName = fullHexName.split("/").last();
    shortHexName = shortHexName.replace(".hex", "");

    A2L_Project *curProject = a2lProjectMap.value(selA2LShortName);

    bool status = curProject->addHexFile(fullHexName);
    if (!status)
        return;

    QTreeWidgetItem *hexItem = new QTreeWidgetItem();
    hexItem->setText(0, shortHexName);
    hexItem->setIcon(0, QIcon(":/icons/Hex.png"));
    item->addChild(hexItem);

    ui->treeWidget_A2LHex->expandAll();
}




