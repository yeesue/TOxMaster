#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QFileDialog>
#include <QTextStream>
#include <QDebug>
#include <QMessageBox>
#include <QSettings>
#include <typeinfo>

#include "workproject.h"
#include "a2ltreemodel.h"
#include "treemodelcompleter.h"
#include "charmodel.h"
#include "measmodel.h"
#include "compumethodmodel.h"
#include "compuvtabmodel.h"
#include "axisptsmodel.h"
#include "dialogchoosemodule.h"
#include "sptablemodel.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    ui->splitter->setStretchFactor(0, 2);
    ui->splitter->setStretchFactor(1, 3);
    ui->splitter->setStretchFactor(2, 8);

    ui->splitter_v->setStretchFactor(0, 8);
    ui->splitter_v->setStretchFactor(1, 2);

    //ui->splitter_3->setStretchFactor(0, 8);
    //ui->splitter_3->setStretchFactor(1, 2);

    setCentralWidget(ui->splitter_v);

    // menus / toolbars / statusbar
    createActions();
    createMenus();
    initToolBars();
    progBar = new QProgressBar(this);
    statusBar()->addWidget(progBar, 1);
    statusBar()->hide();

    //initialize Treeview
    ui->treeView->header()->hide();
    ui->treeView->setContextMenuPolicy(Qt::CustomContextMenu);
    //connect(ui->treeView, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(showContextMenu(QPoint)));
    connect(ui->treeView, SIGNAL(expanded(QModelIndex)), this, SLOT(resizeColumn0()));
    connect(ui->treeView, SIGNAL(collapsed(QModelIndex)), this, SLOT(resizeColumn0()));
    connect(ui->treeView, SIGNAL(doubleClicked(QModelIndex)), this, SLOT(doubleClicked(QModelIndex)));
    connect(ui->treeView, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(showContextMenuInWpTree(QPoint)));
    ui->treeView->setDragEnabled(true);
    ui->treeView->setAcceptDrops(true);
    ui->treeView->setDropIndicatorShown(true);
    ui->treeView->setSelectionMode(QAbstractItemView::ExtendedSelection);
    ui->treeView->setSelectionBehavior(QAbstractItemView::SelectItems);

    //initialize treeView2
    ui->treeView_2->header()->hide();
    ui->treeView_2->setContextMenuPolicy(Qt::CustomContextMenu);
    ui->treeView_2->setDragEnabled(true);
    ui->treeView_2->setAcceptDrops(true);
    ui->treeView_2->setDropIndicatorShown(true);
    ui->treeView_2->setSelectionMode(QAbstractItemView::ExtendedSelection);
    ui->treeView_2->setSelectionBehavior(QAbstractItemView::SelectItems);
    connect(ui->treeView_2, SIGNAL(doubleClicked(QModelIndex)), this, SLOT(doubleClicked_treeView2(QModelIndex)));
    connect(ui->treeView_2, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(showContextMenu(QPoint)));

    //initialize pointers
    projectList = new QMap<QString, WorkProject*>;

    //Model
    model = new A2lTreeModel(0);
    model_2 = new A2lTreeModel(0);

    //completer
    completer = new TreeModelCompleter(this);
    completer->setSeparator(QLatin1String("/"));
    completer->setCompletionMode(QCompleter::PopupCompletion);
    completer->setCaseSensitivity(Qt::CaseInsensitive);
    completer->setMaxVisibleItems(20);

    //initialize tableView
    ui->tableView->setVisible(true);

}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::writeOutput(QString str)
{
    ui->listWidget->addItem(str);
    ui->listWidget->scrollToBottom();
}

void MainWindow::writeOutput(QStringList list)
{
    ui->listWidget->addItems(list);
    ui->listWidget->scrollToBottom();
}

void MainWindow::readA2l(WorkProject *wp)
{
    // display status bar
    statusBar()->show();
    progBar->reset();
    connect(wp, SIGNAL(incProgressBar(int,int)), this, SLOT(setValueProgressBar(int,int)), Qt::DirectConnection);

    // parse the a2l file
    wp->parse();

    // hide the statusbar
    statusBar()->hide();
    progBar->reset();

    //messagebox in case of a2l parser error
    if (!wp->isOk())
    {
        QMessageBox::warning(this, "HEXplorer" ,"ASAP file parser error");
    }

    //update output console
    ui->listWidget->addItems(wp->_outputList());
    ui->listWidget->scrollToBottom();
}

bool MainWindow::loadAndParserA2lFile(QString fullFileName, QStringList &infoList)
{

    if (fullFileName.isEmpty())
    {
        statusBar()->showMessage(tr("Loading canceled"), 2000);
        writeOutput("action open new project : no file selected");
        return false;
    }

    //intialize projectList if necessary
    if (projectList == NULL)
        projectList = new QMultiMap<QString, WorkProject*>;

    parserOK =  openProject(fullFileName);

    infoList = getParserInfoList();
    return getParserStatus();

    /*
    A2lTreeModel *a2lModel = new A2lTreeModel();

    // create a new Wp
    WorkProject *wp = new WorkProject(fullFileName, a2lModel);

    // parse the a2l file
    wp->parse();

    a2lModel->addNode2RootNode(wp->a2lFile);

    //messagebox in case of a2l parser error
    if (!wp->isOk())
    {
        QMessageBox::warning(this, "A2L_Parser" ,"ASAP file parser error");
    }

    infoList.clear();
    infoList.append(wp->_outputList());
    */

}


bool MainWindow::getParserStatus()
{
    return parserOK;
}

QStringList MainWindow::getParserInfoList()
{
    return parserInfoList;
}


QStringList MainWindow::getAllMeasNamesInA2LFile(QString fullFileName)
{
    QStringList measNameList;
    measNameList.clear();

    if (projectList->isEmpty())
        return measNameList;
    if (projectList->keys().indexOf(fullFileName) == -1)
        return measNameList;
    WorkProject *wp = projectList->value(fullFileName);

    Node *root = wp->a2lFile;
    Node *measNode = model_2->findChildNodeByName(root, "MEASUREMENT");


    if (measNode == NULL)
        return measNameList;

    int measCount = measNode->childNodes.count();
    qDebug()<<"Measurement Params Count="<<measCount;
    for (int i = 0; i < measCount; i++)
    {
        measNameList.append(measNode->childNodes.at(i)->name);
    }

    return measNameList;
}

QStringList MainWindow::getAllCharacNamesInA2LFile(QString fullFileName)
{
    QStringList characNameList;
    characNameList.clear();

    if (projectList->isEmpty())
        return characNameList;
    if (projectList->keys().indexOf(fullFileName) == -1)
        return characNameList;
    WorkProject *wp = projectList->value(fullFileName);

    Node *node = wp->a2lFile;
    Node *characNode = model_2->findChildNodeByName(node, "CHARACTERISTIC");


    if (characNode == NULL)
        return characNameList;

    int characCount = characNode->childNodes.count();
    qDebug()<<"Characteristic Params Count="<<characCount;
    for (int i = 0; i < characCount; i++)
    {
        characNameList.append(characNode->childNodes.at(i)->name);
    }

    return characNameList;
}

MEASUREMENT *MainWindow::getMeasNodeByName(QString fullFileName, QString reqMeasName)
{

    if (projectList->isEmpty())
        return NULL;
    if (projectList->keys().indexOf(fullFileName) == -1)
        return NULL;
    WorkProject *wp = projectList->value(fullFileName);

    Node *root = wp->a2lFile;
    Node *measNode = model_2->findChildNodeByName(root, "MEASUREMENT");
    //Node *measNode = root->getNode("MEASUREMENT");
    int measCount = measNode->childCount();
    //MEASUREMENT *reqMeasNode = NULL;
    for(int i = 0; i < measCount; i++)
    {
        MEASUREMENT *reqMeasNode = (MEASUREMENT*)measNode->childNodes.at(i);
        if (reqMeasName == reqMeasNode->name)
        {
            return reqMeasNode;
        }
    }
    return NULL;
}

QMap<QString, QString>* MainWindow::getMeasNodeInfoByName(QString fullFileName, QString reqMeasName)
{

    MEASUREMENT *reqMeasNode = getMeasNodeByName(fullFileName, reqMeasName);
    if (reqMeasNode == NULL)
        return NULL;
    QMap<QString, QString>* measParMap = new QMap<QString, QString>;

    QString compu_method = reqMeasNode->getPar("Conversion");

    Node *node = reqMeasNode->getParentNode()->getParentNode();
    COMPU_METHOD *cmp = (COMPU_METHOD*)node->getNode("COMPU_METHOD/" + compu_method);

    ECU_ADDRESS *ecuAddress = (ECU_ADDRESS*)reqMeasNode->getItem("ecu_address");

    MATRIX_DIM *matrixDim = (MATRIX_DIM*)reqMeasNode->getItem("MATRIX_DIM");
    ARRAY_SIZE *arraySize = (ARRAY_SIZE*)reqMeasNode->getItem("array_size");

    measParMap->insert("Name", reqMeasNode->name);
    measParMap->insert("LongIdentifier", reqMeasNode->fixPar("LongIdentifier").c_str());
    measParMap->insert("DataType", reqMeasNode->fixPar("DataType").c_str());
    measParMap->insert("Conversion", reqMeasNode->fixPar("Conversion").c_str());
    measParMap->insert("Resolution", reqMeasNode->fixPar("Resolution").c_str());
    measParMap->insert("Accuracy", reqMeasNode->fixPar("Accuracy").c_str());
    measParMap->insert("LowerLimit", reqMeasNode->fixPar("LowerLimit").c_str());
    measParMap->insert("UpperLimit", reqMeasNode->fixPar("UpperLimit").c_str());

    if(ecuAddress)
    {
        measParMap->insert("ECUAddress", ecuAddress->getPar("Address"));
        //qDebug()<<"Get ECU Address:"<<ecuAddress->getPar("Address");
    }
    if (cmp)
        measParMap->insert("Unit", cmp->getPar("Unit"));
    if (matrixDim)
    {
        measParMap->insert("MatrixDimX", matrixDim->getPar("xDim"));
        measParMap->insert("MatrixDimY", matrixDim->getPar("yDim"));
        measParMap->insert("MatrixDimZ", matrixDim->getPar("zDim"));
    }
    if(arraySize)
    {
        measParMap->insert("ArraySize", arraySize->getPar("Number"));
        //qDebug()<<"Get ArraySize:"<<arraySize->getPar("Number");
    }

    return measParMap;


}

CHARACTERISTIC *MainWindow::getCharacNodeByName(QString fullFileName, QString reqCharacName)
{
    if (projectList->isEmpty())
        return NULL;
    if (projectList->keys().indexOf(fullFileName) == -1)
        return NULL;
    WorkProject *wp = projectList->value(fullFileName);

    Node *root = wp->a2lFile;
    Node *characNode = model_2->findChildNodeByName(root, "CHARACTERISTIC");
    //Node *characNode = root->getNode("CHARACTERISTIC");
    int characCount = characNode->childCount();

    for(int i = 0; i < characCount; i++)
    {
        CHARACTERISTIC *reqCharacNode = (CHARACTERISTIC*)characNode->childNodes.at(i);
        if (reqCharacName == reqCharacNode->name)
        {
            return reqCharacNode;
        }
    }
    return NULL;
}

QMap<QString, QString> *MainWindow::getCharacNodeInfoByName(QString fullFileName, QString reqCharacName)
{
    CHARACTERISTIC *reqCharacNode = getCharacNodeByName(fullFileName, reqCharacName);
    if (reqCharacNode == NULL)
        return NULL;
    QMap<QString, QString>* characParMap = new QMap<QString, QString>;

    QString compu_method = reqCharacNode->getPar("Conversion");
    Node *node = reqCharacNode->getParentNode()->getParentNode();
    COMPU_METHOD *cmp = NULL;
    if(node)
    {
        cmp = (COMPU_METHOD*)(node->getNode("COMPU_METHOD/" + compu_method));
    }
    qDebug()<<"===pack characMap===";

    characParMap->insert("Name", reqCharacNode->name);
    characParMap->insert("LongIdentifier", reqCharacNode->fixPar("LongIdentifier").c_str());
    characParMap->insert("Type", reqCharacNode->fixPar("Type").c_str());
    characParMap->insert("Address", reqCharacNode->fixPar("Address").c_str());
    characParMap->insert("Conversion", reqCharacNode->fixPar("Conversion").c_str());
    characParMap->insert("Deposit", reqCharacNode->fixPar("Deposit").c_str());
    characParMap->insert("MaxDiff", reqCharacNode->fixPar("MaxDiff").c_str());
    characParMap->insert("LowerLimit", reqCharacNode->fixPar("LowerLimit").c_str());
    characParMap->insert("UpperLimit", reqCharacNode->fixPar("UpperLimit").c_str());
    if (cmp)
        characParMap->insert("Unit", cmp->getPar("Unit"));

    if(characParMap->value("Type") == "VAL_BLK")
    {
        //qDebug()<<"===pack val_blk===";
        MATRIX_DIM *matrixDimItem = (MATRIX_DIM*)reqCharacNode->getItem("MATRIX_DIM");
        if(matrixDimItem)
        {
            characParMap->insert("xDim", matrixDimItem->getPar("xDim"));
            characParMap->insert("yDim", matrixDimItem->getPar("yDim"));
            characParMap->insert("zDim", matrixDimItem->getPar("zDim"));
        }
        //qDebug()<<"===pack matrix_dim===";
        NUMBER *numberItem = (NUMBER*)reqCharacNode->getItem("NUMBER");
        if(numberItem)
        {
            characParMap->insert("Number", numberItem->getPar("Number"));
        }
        //qDebug()<<"===end pack val_blk===";
    }

    AXIS_DESCR *axis = (AXIS_DESCR*)reqCharacNode->getNode("AXIS_DESCR");
    if (axis)
    {
        if (axis->childCount() == 1)
        {
            AXIS_DESCR *axis_x = (AXIS_DESCR*)axis->getNode(0);
            if (axis_x)
            {
                characParMap->insert("AXIS_X_Attribute", axis_x->fixPar("Attribute").c_str());
                characParMap->insert("AXIS_X_InputQuantity", axis_x->fixPar("InputQuantity").c_str());
                characParMap->insert("AXIS_X_Conversion", axis_x->fixPar("Conversion").c_str());
                characParMap->insert("AXIS_X_MaxAxisPoints", axis_x->fixPar("MaxAxisPoints").c_str());
                characParMap->insert("AXIS_X_LowerLimit", axis_x->fixPar("LowerLimit").c_str());
                characParMap->insert("AXIS_X_UpperLimit", axis_x->fixPar("UpperLimit").c_str());
                FORMAT *formatItem = (FORMAT*)axis_x->getItem("FORMAT");
                if (formatItem)
                    characParMap->insert("AXIS_X_Format", formatItem->getPar("FormatString"));
                DEPOSIT *depositItem = (DEPOSIT*)axis_x->getItem("DEPOSIT");
                if (depositItem)
                    characParMap->insert("AXIS_X_Deposit", depositItem->getPar("Mode"));

                AXIS_PTS_REF* axis_pt_ref = (AXIS_PTS_REF*)axis_x->getItem("AXIS_PTS_REF");
                if (axis_pt_ref)
                {
                    qDebug()<<"AXIS_X:"<<QString(axis_pt_ref->getPar("AxisPoints"));
                    characParMap->insert("AXIS_X_PTS_REF", axis_pt_ref->getPar("AxisPoints"));

                    /*
                    QString axisPtsName = axis_pt_ref->getPar("AxisPoints");
                    AXIS_PTS *axis_pts = (AXIS_PTS*)(node->getNode("AXIS_PTS/" + axisPtsName));
                    if(axis_pts)
                    {
                        characParMap->insert("AXIS_X_PTS_Name", axis_pts->fixPar("Name").c_str());
                        characParMap->insert("AXIS_X_PTS_LongIdentifier", axis_pts->fixPar("LongIdentifier").c_str());
                        characParMap->insert("AXIS_X_PTS_Type", axis_pts->fixPar("Type").c_str());
                        characParMap->insert("AXIS_X_PTS_Address", axis_pts->fixPar("Address").c_str());
                        characParMap->insert("AXIS_X_PTS_Deposit", axis_pts->fixPar("Deposit").c_str());
                        characParMap->insert("AXIS_X_PTS_MaxDiff", axis_pts->fixPar("MaxDiff").c_str());
                        characParMap->insert("AXIS_X_PTS_Conversion", axis_pts->fixPar("Conversion").c_str());
                        characParMap->insert("AXIS_X_PTS_LowerLimit", axis_pts->fixPar("LowerLimit").c_str());
                        characParMap->insert("AXIS_X_PTS_UpperLimit", axis_pts->fixPar("UpperLimit").c_str());

                        QString compu_method_axispts = axis_pts->getPar("Conversion");
                        COMPU_METHOD *cmp_axispts = (COMPU_METHOD*)(node->getNode("COMPU_METHOD/" + compu_method_axispts));
                        if(cmp_axispts)
                        {
                            characParMap->insert("AXIS_X_PTS_Unit", cmp_axispts->getPar("Unit"));
                        }

                    }
                    */
                }
            }
        }
        else if (axis->childCount() == 2)
        {
            AXIS_DESCR *axis_x = (AXIS_DESCR*)axis->getNode(1);
            if (axis_x)
            {
                characParMap->insert("AXIS_X_Attribute", axis_x->fixPar("Attribute").c_str());
                characParMap->insert("AXIS_X_InputQuantity", axis_x->fixPar("InputQuantity").c_str());
                characParMap->insert("AXIS_X_Conversion", axis_x->fixPar("Conversion").c_str());
                characParMap->insert("AXIS_X_MaxAxisPoints", axis_x->fixPar("MaxAxisPoints").c_str());
                characParMap->insert("AXIS_X_LowerLimit", axis_x->fixPar("LowerLimit").c_str());
                characParMap->insert("AXIS_X_UpperLimit", axis_x->fixPar("UpperLimit").c_str());
                FORMAT *formatItem = (FORMAT*)axis_x->getItem("FORMAT");
                if (formatItem)
                    characParMap->insert("AXIS_X_Format", formatItem->getPar("FormatString"));
                DEPOSIT *depositItem = (DEPOSIT*)axis_x->getItem("DEPOSIT");
                if (depositItem)
                    characParMap->insert("AXIS_X_Deposit", depositItem->getPar("Mode"));

                AXIS_PTS_REF* axis_pt_ref = (AXIS_PTS_REF*)axis_x->getItem("AXIS_PTS_REF");
                if (axis_pt_ref)
                {
                    qDebug()<<"AXIS_X:"<<QString(axis_pt_ref->getPar("AxisPoints"));
                    characParMap->insert("AXIS_X_PTS_REF", axis_pt_ref->getPar("AxisPoints"));

                    /*
                    QString axisPtsName = axis_pt_ref->getPar("AxisPoints");
                    AXIS_PTS *axis_pts = (AXIS_PTS*)(node->getNode("AXIS_PTS/" + axisPtsName));
                    if(axis_pts)
                    {
                        characParMap->insert("AXIS_X_PTS_Name", axis_pts->fixPar("Name").c_str());
                        characParMap->insert("AXIS_X_PTS_LongIdentifier", axis_pts->fixPar("LongIdentifier").c_str());
                        characParMap->insert("AXIS_X_PTS_Type", axis_pts->fixPar("Type").c_str());
                        characParMap->insert("AXIS_X_PTS_Address", axis_pts->fixPar("Address").c_str());
                        characParMap->insert("AXIS_X_PTS_Deposit", axis_pts->fixPar("Deposit").c_str());
                        characParMap->insert("AXIS_X_PTS_MaxDiff", axis_pts->fixPar("MaxDiff").c_str());
                        characParMap->insert("AXIS_X_PTS_Conversion", axis_pts->fixPar("Conversion").c_str());
                        characParMap->insert("AXIS_X_PTS_LowerLimit", axis_pts->fixPar("LowerLimit").c_str());
                        characParMap->insert("AXIS_X_PTS_UpperLimit", axis_pts->fixPar("UpperLimit").c_str());

                        QString compu_method_axispts = axis_pts->getPar("Conversion");
                        COMPU_METHOD *cmp_axispts = (COMPU_METHOD*)(node->getNode("COMPU_METHOD/" + compu_method_axispts));
                        if(cmp_axispts)
                        {
                            characParMap->insert("AXIS_X_PTS_Unit", cmp_axispts->getPar("Unit"));
                        }
                    }
                    */
                }
            }
            AXIS_DESCR *axis_y = (AXIS_DESCR*)axis->getNode(0);
            if (axis_y)
            {
                characParMap->insert("AXIS_Y_Attribute", axis_x->fixPar("Attribute").c_str());
                characParMap->insert("AXIS_Y_InputQuantity", axis_x->fixPar("InputQuantity").c_str());
                characParMap->insert("AXIS_Y_Conversion", axis_x->fixPar("Conversion").c_str());
                characParMap->insert("AXIS_Y_MaxAxisPoints", axis_x->fixPar("MaxAxisPoints").c_str());
                characParMap->insert("AXIS_Y_LowerLimit", axis_x->fixPar("LowerLimit").c_str());
                characParMap->insert("AXIS_Y_UpperLimit", axis_x->fixPar("UpperLimit").c_str());
                FORMAT *formatItem = (FORMAT*)axis_x->getItem("FORMAT");
                if (formatItem)
                    characParMap->insert("AXIS_Y_Format", formatItem->getPar("FormatString"));
                DEPOSIT *depositItem = (DEPOSIT*)axis_x->getItem("DEPOSIT");
                if (depositItem)
                    characParMap->insert("AXIS_Y_Deposit", depositItem->getPar("Mode"));

                AXIS_PTS_REF* axis_pt_ref = (AXIS_PTS_REF*)axis_y->getItem("AXIS_PTS_REF");
                if (axis_pt_ref)
                {
                    qDebug()<<"AXIS_Y:"<<QString(axis_pt_ref->getPar("AxisPoints"));
                    characParMap->insert("AXIS_Y_PTS_REF", axis_pt_ref->getPar("AxisPoints"));

                    /*
                    QString axisPtsName = axis_pt_ref->getPar("AxisPoints");
                    AXIS_PTS *axis_pts = (AXIS_PTS*)(node->getNode("AXIS_PTS/" + axisPtsName));
                    if(axis_pts)
                    {
                        characParMap->insert("AXIS_Y_PTS_Name", axis_pts->fixPar("Name").c_str());
                        characParMap->insert("AXIS_Y_PTS_LongIdentifier", axis_pts->fixPar("LongIdentifier").c_str());
                        characParMap->insert("AXIS_Y_PTS_Type", axis_pts->fixPar("Type").c_str());
                        characParMap->insert("AXIS_Y_PTS_Address", axis_pts->fixPar("Address").c_str());
                        characParMap->insert("AXIS_Y_PTS_Deposit", axis_pts->fixPar("Deposit").c_str());
                        characParMap->insert("AXIS_Y_PTS_MaxDiff", axis_pts->fixPar("MaxDiff").c_str());
                        characParMap->insert("AXIS_Y_PTS_Conversion", axis_pts->fixPar("Conversion").c_str());
                        characParMap->insert("AXIS_Y_PTS_LowerLimit", axis_pts->fixPar("LowerLimit").c_str());
                        characParMap->insert("AXIS_Y_PTS_UpperLimit", axis_pts->fixPar("UpperLimit").c_str());

                        QString compu_method_axispts = axis_pts->getPar("Conversion");
                        COMPU_METHOD *cmp_axispts = (COMPU_METHOD*)(node->getNode("COMPU_METHOD/" + compu_method_axispts));
                        if(cmp_axispts)
                        {
                            characParMap->insert("AXIS_Y_PTS_Unit", cmp_axispts->getPar("Unit"));
                        }
                    }
                    */
                }
            }
        }
    }

    return characParMap;
}

COMPU_METHOD *MainWindow::getCompMNodeByName(QString fullFileName, QString reqCompMName)
{
    if (projectList->isEmpty())
        return NULL;
    if (projectList->keys().indexOf(fullFileName) == -1)
        return NULL;
    WorkProject *wp = projectList->value(fullFileName);

    Node *root = wp->a2lFile;
    Node *compMNode = model_2->findChildNodeByName(root, "COMPU_METHOD");
    int compMCount = compMNode->childCount();

    for(int i = 0; i < compMCount; i++)
    {
        COMPU_METHOD *reqCompMNode = (COMPU_METHOD*)compMNode->childNodes.at(i);
        if (reqCompMName == reqCompMNode->name)
        {
            return reqCompMNode;
        }
    }
    return NULL;
}

QMap<QString, QString> *MainWindow::getCompMNodeInfoByName(QString fullFileName, QString reqCompMName)
{
    COMPU_METHOD *reqCompMNode = getCompMNodeByName(fullFileName, reqCompMName);
    if (reqCompMNode == NULL)
        return NULL;
    QMap<QString, QString>* compMParMap = new QMap<QString, QString>;

    COEFFS *coeffs = (COEFFS*)(reqCompMNode->getItem("COEFFS"));
    QString coeffsStr = "";
    if (coeffs)
    {
        QString comma = ",";
        coeffsStr = QString(coeffs->getPar("float1")) + comma + QString(coeffs->getPar("float2"))
                + comma
                + QString(coeffs->getPar("float3")) + comma + QString(coeffs->getPar("float4"))
                + comma
                + QString(coeffs->getPar("float5")) + comma + QString(coeffs->getPar("float6"));
    }

    COMPU_TAB_REF *compuTabRef = (COMPU_TAB_REF*)(reqCompMNode->getItem("COMPU_TAB_REF"));
    QString compVRef = "";
    if (compuTabRef)
    {
        compVRef = compuTabRef->getPar("ConversionTable");
    }

    compMParMap->insert("Name", reqCompMNode->name);
    compMParMap->insert("LongIdentifier", reqCompMNode->fixPar("LongIdentifier").c_str());
    compMParMap->insert("ConversionType", reqCompMNode->fixPar("ConversionType").c_str());
    compMParMap->insert("Unit", reqCompMNode->fixPar("Unit").c_str());
    compMParMap->insert("Format", reqCompMNode->fixPar("Format").c_str());
    compMParMap->insert("COEFFS", coeffsStr);
    compMParMap->insert("ConversionTable", compVRef);

    return compMParMap;
}

RECORD_LAYOUT *MainWindow::getRecordLayoutNodeByName(QString fullFileName, QString reqRecordLayoutName)
{
    if (projectList->isEmpty())
        return NULL;
    if (projectList->keys().indexOf(fullFileName) == -1)
        return NULL;
    WorkProject *wp = projectList->value(fullFileName);

    Node *root = wp->a2lFile;
    Node *recordLayoutNode = model_2->findChildNodeByName(root, "RECORD_LAYOUT");
    int recordLayoutCount = recordLayoutNode->childCount();

    for(int i = 0; i < recordLayoutCount; i++)
    {
        RECORD_LAYOUT *reqRecordLayoutNode = (RECORD_LAYOUT*)recordLayoutNode->childNodes.at(i);
        if (reqRecordLayoutName == reqRecordLayoutNode->name)
        {
            return reqRecordLayoutNode;
        }
    }
    return NULL;
}

QMap<QString, QString> *MainWindow::getRecordLayoutNodeInfoByName(QString fullFileName, QString reqRecordLayoutName)
{
    RECORD_LAYOUT *reqRecordLayoutNode = getRecordLayoutNodeByName(fullFileName, reqRecordLayoutName);
    if (reqRecordLayoutNode == NULL)
        return NULL;
    QMap<QString, QString>* recordLayoutParMap = new QMap<QString, QString>;


    recordLayoutParMap->insert("Name", reqRecordLayoutNode->name);
    FNC_VALUES *fncItem = (FNC_VALUES*)reqRecordLayoutNode->getItem("FNC_VALUES");
    if (fncItem)
    {
        recordLayoutParMap->insert("FNC_VALUES_Position", fncItem->getPar("Position"));
        recordLayoutParMap->insert("FNC_VALUES_DataType", fncItem->getPar("Datatype"));
        recordLayoutParMap->insert("FNC_VALUES_IndexMode", fncItem->getPar("IndexMode"));
        recordLayoutParMap->insert("FNC_VALUES_Addesstype", fncItem->getPar("Addesstype"));
    }
    AXIS_PTS_X *axisPtsXItem = (AXIS_PTS_X*)reqRecordLayoutNode->getItem("AXIS_PTS_X");
    if (axisPtsXItem)
    {
        recordLayoutParMap->insert("AXIS_PTS_X_Position", axisPtsXItem->getPar("Position"));
        recordLayoutParMap->insert("AXIS_PTS_X_DataType", axisPtsXItem->getPar("Datatype"));
        recordLayoutParMap->insert("AXIS_PTS_X_IndexIncr", axisPtsXItem->getPar("IndexIncr"));
        recordLayoutParMap->insert("AXIS_PTS_X_Addessing", axisPtsXItem->getPar("Addessing"));
    }
    NO_AXIS_PTS_X *noAxisPtsXItem = (NO_AXIS_PTS_X*)reqRecordLayoutNode->getItem("NO_AXIS_PTS_X");
    if (noAxisPtsXItem)
    {
        recordLayoutParMap->insert("NO_AXIS_PTS_X_Position", noAxisPtsXItem->getPar("Position"));
        recordLayoutParMap->insert("NO_AXIS_PTS_X_DataType", noAxisPtsXItem->getPar("Datatype"));
    }
    AXIS_PTS_Y *axisPtsYItem = (AXIS_PTS_Y*)reqRecordLayoutNode->getItem("AXIS_PTS_Y");
    if (axisPtsYItem)
    {
        recordLayoutParMap->insert("AXIS_PTS_Y_Position", axisPtsYItem->getPar("Position"));
        recordLayoutParMap->insert("AXIS_PTS_Y_DataType", axisPtsYItem->getPar("Datatype"));
        recordLayoutParMap->insert("AXIS_PTS_Y_IndexIncr", axisPtsYItem->getPar("IndexIncr"));
        recordLayoutParMap->insert("AXIS_PTS_Y_Addessing", axisPtsYItem->getPar("Addessing"));
    }
    NO_AXIS_PTS_Y *noAxisPtsYItem = (NO_AXIS_PTS_Y*)reqRecordLayoutNode->getItem("NO_AXIS_PTS_Y");
    if (noAxisPtsYItem)
    {
        recordLayoutParMap->insert("NO_AXIS_PTS_Y_Position", noAxisPtsYItem->getPar("Position"));
        recordLayoutParMap->insert("NO_AXIS_PTS_Y_DataType", noAxisPtsYItem->getPar("Datatype"));
    }

    return recordLayoutParMap;
}

AXIS_PTS *MainWindow::getAxisPtsNodeByName(QString fullFileName, QString reqAxisPtsName)
{
    if (projectList->isEmpty())
        return NULL;
    if (projectList->keys().indexOf(fullFileName) == -1)
        return NULL;
    WorkProject *wp = projectList->value(fullFileName);

    Node *root = wp->a2lFile;
    Node *axisPtsNode = model_2->findChildNodeByName(root, "AXIS_PTS");
    int axisPtsCount = axisPtsNode->childCount();

    for(int i = 0; i < axisPtsCount; i++)
    {
        AXIS_PTS *reqAxisPtsNode = (AXIS_PTS*)axisPtsNode->childNodes.at(i);
        if (reqAxisPtsName == reqAxisPtsNode->name)
        {
            return reqAxisPtsNode;
        }
    }
    return NULL;
}

QMap<QString, QString> *MainWindow::getAxisPtsNodeInfoByName(QString fullFileName, QString reqAxisPtsName)
{
    AXIS_PTS *reqAxisPtsNode = getAxisPtsNodeByName(fullFileName, reqAxisPtsName);
    if (reqAxisPtsNode == NULL)
        return NULL;
    QMap<QString, QString>* axisPtsParMap = new QMap<QString, QString>;

    QString compu_method = reqAxisPtsNode->getPar("Conversion");
    Node *node = reqAxisPtsNode->getParentNode()->getParentNode();
    COMPU_METHOD *cmp = (COMPU_METHOD*)node->getNode("COMPU_METHOD/" + compu_method);

    axisPtsParMap->insert("Name", reqAxisPtsNode->name);
    axisPtsParMap->insert("LongIdentifier", reqAxisPtsNode->fixPar("LongIdentifier").c_str());
    axisPtsParMap->insert("Address", reqAxisPtsNode->fixPar("Address").c_str());
    axisPtsParMap->insert("InputQuantity", reqAxisPtsNode->fixPar("InputQuantity").c_str());
    axisPtsParMap->insert("Deposit", reqAxisPtsNode->fixPar("Deposit").c_str());
    axisPtsParMap->insert("MaxDiff", reqAxisPtsNode->fixPar("MaxDiff").c_str());
    axisPtsParMap->insert("Conversion", reqAxisPtsNode->fixPar("Conversion").c_str());
    axisPtsParMap->insert("MaxAxisPoints", reqAxisPtsNode->fixPar("MaxAxisPoints").c_str());
    axisPtsParMap->insert("LowerLimit", reqAxisPtsNode->fixPar("LowerLimit").c_str());
    axisPtsParMap->insert("UpperLimit", reqAxisPtsNode->fixPar("UpperLimit").c_str());

    if (cmp)
        axisPtsParMap->insert("Unit", cmp->getPar("Unit"));

    return axisPtsParMap;
}

DAQ *MainWindow::getDaqNodeByName(QString fullFileName, QString reqDaqName)
{
    if (projectList->isEmpty())
        return NULL;
    if (projectList->keys().indexOf(fullFileName) == -1)
        return NULL;
    WorkProject *wp = projectList->value(fullFileName);

    Node *root = wp->a2lFile;
    Node *node = model_2->findChildNodeByName(root, "DAQ");
    DAQ *daqNode = (DAQ*)node;
    qDebug()<<"get Daq Node";

    return daqNode;
}

QMap<QString, QString> *MainWindow::getDaqNodeInfoByName(QString fullFileName, QString reqDaqName)
{
    DAQ *reqDaqNode = getDaqNodeByName(fullFileName, reqDaqName);
    if (reqDaqNode == NULL)
        return NULL;
    QMap<QString, QString>* daqParMap = new QMap<QString, QString>;
    qDebug()<<"Daq Node is valid";

    daqParMap->insert("Name", reqDaqNode->name);
    daqParMap->insert("DaqType", reqDaqNode->fixPar("DaqType").c_str());
    daqParMap->insert("MAX_DAQ", reqDaqNode->fixPar("MAX_DAQ").c_str());
    daqParMap->insert("MAX_EVENT_CHANNEL", reqDaqNode->fixPar("MAX_EVENT_CHANNEL").c_str());
    daqParMap->insert("MIN_DAQ", reqDaqNode->fixPar("MIN_DAQ").c_str());
    daqParMap->insert("OptimisationType", reqDaqNode->fixPar("OptimisationType").c_str());
    daqParMap->insert("AddressExtType", reqDaqNode->fixPar("AddressExtType").c_str());
    daqParMap->insert("IdentificationField", reqDaqNode->fixPar("IdentificationField").c_str());
    daqParMap->insert("GranularityOdtEntrySize", reqDaqNode->fixPar("GranularityOdtEntrySize").c_str());
    daqParMap->insert("MAX_ODT_ENTRY_SIZE_DAQ", reqDaqNode->fixPar("MAX_ODT_ENTRY_SIZE_DAQ").c_str());
    //daqParMap->insert("OverloadIndication", reqDaqNode->fixPar("OverloadIndication").c_str());
    //daqParMap->insert("MAX_ODT_ENTRY_SIZE_STIM", reqDaqNode->fixPar("MAX_ODT_ENTRY_SIZE_STIM").c_str());

    return daqParMap;
}

QList<DAQ_LIST *> MainWindow::getDaqListNodeListByName(QString fullFileName, QString reqDaqListName)
{
    QList<DAQ_LIST*> daqListNodeList;
    daqListNodeList.clear();

    if (projectList->isEmpty())
        return daqListNodeList;
    if (projectList->keys().indexOf(fullFileName) == -1)
        return daqListNodeList;
    WorkProject *wp = projectList->value(fullFileName);

    Node *root = wp->a2lFile;
    Node *node = model_2->findChildNodeByName(root, "DAQ_LIST");
    if (node == NULL)
        return daqListNodeList;

    Node *parentNode = node->getParentNode();
    int childCount = parentNode->childCount();
    qDebug()<<"DaqListNode Parent childCount="<<childCount;
    for (int i = 0; i < childCount; i++)
    {
        Node *child = parentNode->childNodes.at(i);
        if (QString(child->name) == "DAQ_LIST")
        {
            daqListNodeList.append((DAQ_LIST*)child);
        }
    }
    qDebug()<<"DAQ_LIST Node Count="<<daqListNodeList.count();

    return daqListNodeList;
}

QMap<QString, QString> *MainWindow::getDaqListNodeInfoByNameAndIndex(QString fullFileName, QString reqDaqListName, int index)
{
    QList<DAQ_LIST*> reqDaqListNodeList = getDaqListNodeListByName(fullFileName, reqDaqListName);

    if (reqDaqListNodeList.isEmpty() || reqDaqListNodeList.count() <= index)
        return NULL;

    //qDebug()<<"index="<<index;
    DAQ_LIST *reqDaqListNode = reqDaqListNodeList.at(index);
    if (reqDaqListNode == NULL)
        return NULL;
    QMap<QString, QString>* daqListParMap = new QMap<QString, QString>;

    DAQ_LIST_TYPE *daqListTypeItem = (DAQ_LIST_TYPE*)reqDaqListNode->getItem("DAQ_LIST_TYPE");
    MAX_ODT *maxOdtItem = (MAX_ODT*)reqDaqListNode->getItem("MAX_ODT");
    MAX_ODT_ENTRIES *maxOdtEntriesItem = (MAX_ODT_ENTRIES*)reqDaqListNode->getItem("MAX_ODT_ENTRIES");
    FIRST_PID *firstPidItem = (FIRST_PID*)reqDaqListNode->getItem("FIRST_PID");
    EVENT_FIXED *eventFixedItem = (EVENT_FIXED*)reqDaqListNode->getItem("EVENT_FIXED");

    //qDebug()<<"index="<<index<<", req DaqList node name:"<<reqDaqListNode->name;

    daqListParMap->insert("Name", reqDaqListNode->name);
    daqListParMap->insert("DaqListNumber", reqDaqListNode->fixPar("ListNum").c_str());
    //qDebug()<<"1";
    if (daqListTypeItem)
        daqListParMap->insert("Daq_List_Type", daqListTypeItem->getPar("Daq_list_type"));
    //qDebug()<<"2";
    if (maxOdtItem)
        daqListParMap->insert("MAX_ODT", maxOdtItem->getPar("MAX_ODT"));
    //qDebug()<<"3";
    if (maxOdtEntriesItem)
        daqListParMap->insert("MAX_ODT_ENTRIES", maxOdtEntriesItem->getPar("MAX_ODT_ENTRIES"));
    //qDebug()<<"4";
    if (firstPidItem)
        daqListParMap->insert("FIRST_PID", firstPidItem->getPar("FIRST_PID"));
    //qDebug()<<"5";
    if (eventFixedItem)
        daqListParMap->insert("EVENT_FIXED", eventFixedItem->getPar("EVENT_FIXED"));
    //qDebug()<<"6";

    //qDebug()<<"daqListParMap pair count="<<daqListParMap->keys().count();
    return daqListParMap;
}

DAQ_MEMORY_CONSUMPTION *MainWindow::getDaqMemConsumpNodeByName(QString fullFileName, QString reqDaqMemConsumpName)
{
    if (projectList->isEmpty())
        return NULL;
    if (projectList->keys().indexOf(fullFileName) == -1)
        return NULL;
    WorkProject *wp = projectList->value(fullFileName);

    Node *root = wp->a2lFile;
    Node *node = model_2->findChildNodeByName(root, "DAQ_MEMORY_CONSUMPTION");
    DAQ_MEMORY_CONSUMPTION *daqMemConsumpNode = (DAQ_MEMORY_CONSUMPTION*)node;
    qDebug()<<"get DaqMemConsump Node";

    return daqMemConsumpNode;
}

QMap<QString, QString> *MainWindow::getDaqMemConsumpNodeInfoByName(QString fullFileName, QString reqDaqMemConsumpName)
{
    DAQ_MEMORY_CONSUMPTION *reqDaqMemConsumpNode = getDaqMemConsumpNodeByName(fullFileName, reqDaqMemConsumpName);
    if (reqDaqMemConsumpNode == NULL)
        return NULL;
    QMap<QString, QString>* daqMemConsumpParMap = new QMap<QString, QString>;
    qDebug()<<"DaqMemConsump Node is valid";

    daqMemConsumpParMap->insert("Name", reqDaqMemConsumpNode->name);
    daqMemConsumpParMap->insert("DAQ_MEMORY_LIMIT", reqDaqMemConsumpNode->fixPar("DAQ_MEMORY_LIMIT").c_str());
    daqMemConsumpParMap->insert("DAQ_SIZE", reqDaqMemConsumpNode->fixPar("DAQ_SIZE").c_str());
    daqMemConsumpParMap->insert("ODT_SIZE", reqDaqMemConsumpNode->fixPar("ODT_SIZE").c_str());
    daqMemConsumpParMap->insert("ODT_ENTRY_SIZE", reqDaqMemConsumpNode->fixPar("ODT_ENTRY_SIZE").c_str());
    daqMemConsumpParMap->insert("ODT_DAQ_BUFFER_ELEMENT_SIZE", reqDaqMemConsumpNode->fixPar("ODT_DAQ_BUFFER_ELEMENT_SIZE").c_str());
    daqMemConsumpParMap->insert("ODT_STIM_BUFFER_ELEMENT_SIZE", reqDaqMemConsumpNode->fixPar("ODT_STIM_BUFFER_ELEMENT_SIZE").c_str());

    return daqMemConsumpParMap;
}

XCP_ON_CAN *MainWindow::getXcpOnCANNodeByName(QString fullFileName, QString reqXcpOnCANName)
{
    if (projectList->isEmpty())
        return NULL;
    if (projectList->keys().indexOf(fullFileName) == -1)
        return NULL;
    WorkProject *wp = projectList->value(fullFileName);

    QList<XCP_ON_CAN*> xcpOnCANNodeList;

    Node *root = wp->a2lFile;
    Node *node = model_2->findChildNodeByName(root, "XCP_ON_CAN");
    XCP_ON_CAN *xcpOnCANNode = (XCP_ON_CAN*)node;

    return xcpOnCANNode;
}

QMap<QString, QString> *MainWindow::getXcpOnCANNodeInfoByName(QString fullFileName, QString reqXcpOnCANName)
{
    XCP_ON_CAN *reqXcpOnCANNode = getXcpOnCANNodeByName(fullFileName, reqXcpOnCANName);
    if (reqXcpOnCANNode == NULL)
        return NULL;
    QMap<QString, QString>* xcpOnCANParMap = new QMap<QString, QString>;

    CAN_ID_MASTER *canidmasterItem = (CAN_ID_MASTER*)reqXcpOnCANNode->getItem("CAN_ID_MASTER");
    CAN_ID_SLAVE *canidslaveItem = (CAN_ID_SLAVE*)reqXcpOnCANNode->getItem("CAN_ID_SLAVE");
    BAUDRATE *baudrateItem = (BAUDRATE*)reqXcpOnCANNode->getItem("BAUDRATE");
    SAMPLE_POINT *samplepointItem = (SAMPLE_POINT*)reqXcpOnCANNode->getItem("SAMPLE_POINT");
    SAMPLE_RATE *samplerateItem = (SAMPLE_RATE*)reqXcpOnCANNode->getItem("SAMPLE_RATE");
    BTL_CYCLES *btlcyclesItem = (BTL_CYCLES*)reqXcpOnCANNode->getItem("BTL_CYCLES");
    SJW *sjwItem = (SJW*)reqXcpOnCANNode->getItem("SJW");
    SYNC_EDGE *syncedgeItem = (SYNC_EDGE*)reqXcpOnCANNode->getItem("SYNC_EDGE");
    MAX_BUS_LOAD *maxbusloadItem = (MAX_BUS_LOAD*)reqXcpOnCANNode->getItem("MAX_BUS_LOAD");
    TRANSPORT_LAYER_INSTANCE *transportlayerinstanceItem = (TRANSPORT_LAYER_INSTANCE*)reqXcpOnCANNode->getItem("TRANSPORT_LAYER_INSTANCE");


    xcpOnCANParMap->insert("Name", reqXcpOnCANNode->name);
    if (canidmasterItem)
        xcpOnCANParMap->insert("CAN_ID_MASTER", canidmasterItem->getPar("CAN_ID_MASTER"));
    if (canidslaveItem)
        xcpOnCANParMap->insert("CAN_ID_SLAVE", canidslaveItem->getPar("CAN_ID_SLAVE"));
    if (baudrateItem)
        xcpOnCANParMap->insert("BAUDRATE", baudrateItem->getPar("BAUDRATE"));
    if (samplepointItem)
        xcpOnCANParMap->insert("SAMPLE_POINT", samplepointItem->getPar("SAMPLE_POINT"));
    if (samplerateItem)
        xcpOnCANParMap->insert("SAMPLE_RATE", samplerateItem->getPar("SAMPLE_RATE"));
    if (btlcyclesItem)
        xcpOnCANParMap->insert("BTL_CYCLES", btlcyclesItem->getPar("BTL_CYCLES"));
    if (sjwItem)
        xcpOnCANParMap->insert("SJW", sjwItem->getPar("SJW"));
    if (syncedgeItem)
        xcpOnCANParMap->insert("SYNC_EDGE", syncedgeItem->getPar("SYNC_EDGE"));
    if (maxbusloadItem)
        xcpOnCANParMap->insert("MAX_BUS_LOAD", maxbusloadItem->getPar("MAX_BUS_LOAD"));
    if (transportlayerinstanceItem)
        xcpOnCANParMap->insert("TRANSPORT_LAYER_INSTANCE", transportlayerinstanceItem->getPar("TRANSPORT_LAYER_INSTANCE"));

    CAN_FD *canfdNode = (CAN_FD*)reqXcpOnCANNode->getNode("CAN_FD");
    if(canfdNode)
    {
        MAX_DLC *maxdlcItem = (MAX_DLC*)canfdNode->getItem("MAX_DLC");
        CAN_FD_DATA_TRANSFER_BAUDRATE *canfddatatransferbaudrateItem = (CAN_FD_DATA_TRANSFER_BAUDRATE*)canfdNode->getItem("CAN_FD_DATA_TRANSFER_BAUDRATE");
        SAMPLE_POINT *canfdsamplepointItem = (SAMPLE_POINT*)canfdNode->getItem("SAMPLE_POINT");
        BTL_CYCLES *canfdbtlcyclesItem = (BTL_CYCLES*)canfdNode->getItem("BTL_CYCLES");
        SJW *canfdsjwItem = (SJW*)canfdNode->getItem("SJW");
        SYNC_EDGE *canfdsyncedgeItem = (SYNC_EDGE*)canfdNode->getItem("SYNC_EDGE");
        SECONDARY_SAMPLE_POINT *canfdsecondarysamplepointItem = (SECONDARY_SAMPLE_POINT*)canfdNode->getItem("SECONDARY_SAMPLE_POINT");
        MAX_DLC_REQUIRED *maxdlcrequiredItem = (MAX_DLC_REQUIRED*)canfdNode->getItem("MAX_DLC_REQUIRED");
        TRANSCEIVER_DELAY_COMPENSATION *transceiverdelaycompensationItem = (TRANSCEIVER_DELAY_COMPENSATION*)canfdNode->getItem("TRANSCEIVER_DELAY_COMPENSATION");

        if (maxdlcItem)
            xcpOnCANParMap->insert("MAX_DLC", maxdlcItem->getPar("MAX_DLC"));
        if (canfddatatransferbaudrateItem)
            xcpOnCANParMap->insert("CAN_FD_DATA_TRANSFER_BAUDRATE", canfddatatransferbaudrateItem->getPar("CAN_FD_DATA_TRANSFER_BAUDRATE"));
        if (canfdsamplepointItem)
            xcpOnCANParMap->insert("CAN_FD_SAMPLE_POINT", canfdsamplepointItem->getPar("SAMPLE_POINT"));
        if (canfdbtlcyclesItem)
            xcpOnCANParMap->insert("CAN_FD_BTL_CYCLES", canfdbtlcyclesItem->getPar("BTL_CYCLES"));
        if (canfdsjwItem)
            xcpOnCANParMap->insert("CAN_FD_SJW", canfdsjwItem->getPar("SJW"));
        if (canfdsyncedgeItem)
            xcpOnCANParMap->insert("CAN_FD_SYNC_EDGE", canfdsyncedgeItem->getPar("SYNC_EDGE"));
        if (canfdsecondarysamplepointItem)
            xcpOnCANParMap->insert("CAN_FD_SECONDARY_SAMPLE_POINT", canfdsecondarysamplepointItem->getPar("SAMPLE_POINT"));
        if (maxdlcrequiredItem)
            xcpOnCANParMap->insert("MAX_DLC_REQUIRED", maxdlcrequiredItem->getPar("MAX_DLC_REQUIRED"));
        if (transceiverdelaycompensationItem)
            xcpOnCANParMap->insert("TRANSCEIVER_DELAY_COMPENSATION", transceiverdelaycompensationItem->getPar("TRANSCEIVER_DELAY_COMPENSATION"));
    }

    return xcpOnCANParMap;
}

QList<XCP_ON_CAN *> MainWindow::getXcpOnCANNodeListByName(QString fullFileName, QString reqXcpOnCANName)
{
    QList<XCP_ON_CAN*> xcpOnCANNodeList;
    xcpOnCANNodeList.clear();

    if (projectList->isEmpty())
        return xcpOnCANNodeList;
    if (projectList->keys().indexOf(fullFileName) == -1)
        return xcpOnCANNodeList;
    WorkProject *wp = projectList->value(fullFileName);

    Node *root = wp->a2lFile;
    Node *node = model_2->findChildNodeByName(root, "XCP_ON_CAN");

    Node *parentNode = node->getParentNode();
    int childCount = parentNode->childCount();
    //qDebug()<<"parent childCount="<<childCount;
    for (int i = 0; i < childCount; i++)
    {
        Node *child = parentNode->childNodes.at(i);
        if (QString(child->name) == "XCP_ON_CAN")
        {
            xcpOnCANNodeList.append((XCP_ON_CAN*)child);
        }
    }
    //qDebug()<<"XCP_ON_CAN Node Count="<<xcpOnCANNodeList.count();

    return xcpOnCANNodeList;
}

QMap<QString, QString> *MainWindow::getXcpOnCANNodeInfoByNameAndIndex(QString fullFileName, QString reqXcpOnCANName, int index)
{
    QList<XCP_ON_CAN*> reqXcpOnCANNodeList = getXcpOnCANNodeListByName(fullFileName, reqXcpOnCANName);

    if (reqXcpOnCANNodeList.isEmpty() || reqXcpOnCANNodeList.count() <= index)
        return NULL;

    XCP_ON_CAN *reqXcpOnCANNode = reqXcpOnCANNodeList.at(index);
    if (reqXcpOnCANNode == NULL)
        return NULL;
    QMap<QString, QString>* xcpOnCANParMap = new QMap<QString, QString>;

    CAN_ID_MASTER *canidmasterItem = (CAN_ID_MASTER*)reqXcpOnCANNode->getItem("CAN_ID_MASTER");
    CAN_ID_SLAVE *canidslaveItem = (CAN_ID_SLAVE*)reqXcpOnCANNode->getItem("CAN_ID_SLAVE");
    BAUDRATE *baudrateItem = (BAUDRATE*)reqXcpOnCANNode->getItem("BAUDRATE");
    SAMPLE_POINT *samplepointItem = (SAMPLE_POINT*)reqXcpOnCANNode->getItem("SAMPLE_POINT");
    SAMPLE_RATE *samplerateItem = (SAMPLE_RATE*)reqXcpOnCANNode->getItem("SAMPLE_RATE");
    BTL_CYCLES *btlcyclesItem = (BTL_CYCLES*)reqXcpOnCANNode->getItem("BTL_CYCLES");
    SJW *sjwItem = (SJW*)reqXcpOnCANNode->getItem("SJW");
    SYNC_EDGE *syncedgeItem = (SYNC_EDGE*)reqXcpOnCANNode->getItem("SYNC_EDGE");
    MAX_BUS_LOAD *maxbusloadItem = (MAX_BUS_LOAD*)reqXcpOnCANNode->getItem("MAX_BUS_LOAD");
    TRANSPORT_LAYER_INSTANCE *transportlayerinstanceItem = (TRANSPORT_LAYER_INSTANCE*)reqXcpOnCANNode->getItem("TRANSPORT_LAYER_INSTANCE");

    xcpOnCANParMap->insert("Name", reqXcpOnCANNode->name);
    if (canidmasterItem)
        xcpOnCANParMap->insert("CAN_ID_MASTER", canidmasterItem->getPar("CAN_ID_MASTER"));
    if (canidslaveItem)
        xcpOnCANParMap->insert("CAN_ID_SLAVE", canidslaveItem->getPar("CAN_ID_SLAVE"));
    if (baudrateItem)
        xcpOnCANParMap->insert("BAUDRATE", baudrateItem->getPar("BAUDRATE"));
    if (samplepointItem)
        xcpOnCANParMap->insert("SAMPLE_POINT", samplepointItem->getPar("SAMPLE_POINT"));
    if (samplerateItem)
        xcpOnCANParMap->insert("SAMPLE_RATE", samplerateItem->getPar("SAMPLE_RATE"));
    if (btlcyclesItem)
        xcpOnCANParMap->insert("BTL_CYCLES", btlcyclesItem->getPar("BTL_CYCLES"));
    if (sjwItem)
        xcpOnCANParMap->insert("SJW", sjwItem->getPar("SJW"));
    if (syncedgeItem)
        xcpOnCANParMap->insert("SYNC_EDGE", syncedgeItem->getPar("SYNC_EDGE"));
    if (maxbusloadItem)
        xcpOnCANParMap->insert("MAX_BUS_LOAD", maxbusloadItem->getPar("MAX_BUS_LOAD"));
    if (transportlayerinstanceItem)
        xcpOnCANParMap->insert("TRANSPORT_LAYER_INSTANCE", transportlayerinstanceItem->getPar("TRANSPORT_LAYER_INSTANCE"));

    return xcpOnCANParMap;
}

QMap<QString, QString> *MainWindow::getDaqNodeInfoInXcpOnCanByName(QString fullFileName, QString reqDaqName, int indexOfXcpOnCan)
{
    QList<XCP_ON_CAN*> reqXcpOnCANNodeList = getXcpOnCANNodeListByName(fullFileName, "XCP_ON_CAN");

    if (reqXcpOnCANNodeList.isEmpty() || reqXcpOnCANNodeList.count() <= indexOfXcpOnCan)
        return NULL;

    XCP_ON_CAN *reqXcpOnCANNode = reqXcpOnCANNodeList.at(indexOfXcpOnCan);
    if (reqXcpOnCANNode == NULL)
        return NULL;
    Node* node = model_2->findChildNodeByName(reqXcpOnCANNode, "DAQ");
    DAQ *daqNodeInXcpOnCan = (DAQ*)node;
    qDebug()<<"get DaqNode in XCP_ON_CAN";
    if (daqNodeInXcpOnCan == NULL)
        return NULL;

    QMap<QString, QString>* daqParMap = new QMap<QString, QString>;
    daqParMap->insert("Name", daqNodeInXcpOnCan->name);
    daqParMap->insert("DaqType", daqNodeInXcpOnCan->fixPar("DaqType").c_str());
    daqParMap->insert("MAX_DAQ", daqNodeInXcpOnCan->fixPar("MAX_DAQ").c_str());
    daqParMap->insert("MAX_EVENT_CHANNEL", daqNodeInXcpOnCan->fixPar("MAX_EVENT_CHANNEL").c_str());
    daqParMap->insert("MIN_DAQ", daqNodeInXcpOnCan->fixPar("MIN_DAQ").c_str());
    daqParMap->insert("OptimisationType", daqNodeInXcpOnCan->fixPar("OptimisationType").c_str());
    daqParMap->insert("AddressExtType", daqNodeInXcpOnCan->fixPar("AddressExtType").c_str());
    daqParMap->insert("IdentificationField", daqNodeInXcpOnCan->fixPar("IdentificationField").c_str());
    daqParMap->insert("GranularityOdtEntrySize", daqNodeInXcpOnCan->fixPar("GranularityOdtEntrySize").c_str());
    daqParMap->insert("MAX_ODT_ENTRY_SIZE_DAQ", daqNodeInXcpOnCan->fixPar("MAX_ODT_ENTRY_SIZE_DAQ").c_str());


    return daqParMap;

}

QMap<QString, QString> *MainWindow::getCanFdNodeInfoInXcpOnCanByName(QString fullFileName, QString reqCanFdName, int indexOfXcpOnCan)
{
    QList<XCP_ON_CAN*> reqXcpOnCANNodeList = getXcpOnCANNodeListByName(fullFileName, "XCP_ON_CAN");

    if (reqXcpOnCANNodeList.isEmpty() || reqXcpOnCANNodeList.count() <= indexOfXcpOnCan)
        return NULL;

    XCP_ON_CAN *reqXcpOnCANNode = reqXcpOnCANNodeList.at(indexOfXcpOnCan);
    if (reqXcpOnCANNode == NULL)
        return NULL;
    Node* node = model_2->findChildNodeByName(reqXcpOnCANNode, "CAN_FD");
    CAN_FD *canFdNodeInXcpOnCan = (CAN_FD*)node;
    qDebug()<<"get CanFdNode in XCP_ON_CAN";
    if (canFdNodeInXcpOnCan == NULL)
        return NULL;

    QMap<QString, QString>* canFdParMap = new QMap<QString, QString>;

    MAX_DLC *maxdlcItem = (MAX_DLC*)canFdNodeInXcpOnCan->getItem("MAX_DLC");
    CAN_FD_DATA_TRANSFER_BAUDRATE *canfdbaudrateItem = (CAN_FD_DATA_TRANSFER_BAUDRATE*)canFdNodeInXcpOnCan->getItem("CAN_FD_DATA_TRANSFER_BAUDRATE");
    SAMPLE_POINT *samplepointItem = (SAMPLE_POINT*)canFdNodeInXcpOnCan->getItem("SAMPLE_POINT");
    BTL_CYCLES *btlcyclesItem = (BTL_CYCLES*)canFdNodeInXcpOnCan->getItem("BTL_CYCLES");
    SJW *sjwItem = (SJW*)canFdNodeInXcpOnCan->getItem("SJW");
    SYNC_EDGE *syncedgeItem = (SYNC_EDGE*)canFdNodeInXcpOnCan->getItem("SYNC_EDGE");
    MAX_DLC_REQUIRED *maxdlcrequiredItem = (MAX_DLC_REQUIRED*)canFdNodeInXcpOnCan->getItem("MAX_DLC_REQUIRED");
    SECONDARY_SAMPLE_POINT *secondarysamplepointItem = (SECONDARY_SAMPLE_POINT*)canFdNodeInXcpOnCan->getItem("SECONDARY_SAMPLE_POINT");
    TRANSCEIVER_DELAY_COMPENSATION *transceiverdelaycompensationItem = (TRANSCEIVER_DELAY_COMPENSATION*)canFdNodeInXcpOnCan->getItem("TRANSCEIVER_DELAY_COMPENSATION");

    canFdParMap->insert("Name", canFdNodeInXcpOnCan->name);
    if (maxdlcItem)
        canFdParMap->insert("MAX_DLC", maxdlcItem->getPar("MaxDlc"));
    if (canfdbaudrateItem)
        canFdParMap->insert("CAN_FD_DATA_TRANSFER_BAUDRATE", canfdbaudrateItem->getPar("CANFD_Baudrate"));
    if (samplepointItem)
        canFdParMap->insert("SAMPLE_POINT", samplepointItem->getPar("SAMPLE_POINT"));
    if (btlcyclesItem)
        canFdParMap->insert("BTL_CYCLES", btlcyclesItem->getPar("BTL_CYCLES"));
    if (sjwItem)
        canFdParMap->insert("SJW", sjwItem->getPar("SJW"));
    if (syncedgeItem)
        canFdParMap->insert("SYNC_EDGE", syncedgeItem->getPar("SYNC_EDGE"));
    if (maxdlcrequiredItem)
        canFdParMap->insert("MAX_DLC_REQUIRED", "true");
    if (secondarysamplepointItem)
        canFdParMap->insert("SECONDARY_SAMPLE_POINT", secondarysamplepointItem->getPar("secondarySamplePoint"));
    if (transceiverdelaycompensationItem)
        canFdParMap->insert("TRANSCEIVER_DELAY_COMPENSATION", transceiverdelaycompensationItem->getPar("TransceiverDelayCompensation"));

    return canFdParMap;
}

QList<EVENT *> MainWindow::getEventNodeListByName(QString fullFileName, QString reqEventName)
{
    QList<EVENT*> eventNodeList;
    eventNodeList.clear();

    if (projectList->isEmpty())
        return eventNodeList;
    if (projectList->keys().indexOf(fullFileName) == -1)
        return eventNodeList;
    WorkProject *wp = projectList->value(fullFileName);

    Node *root = wp->a2lFile;
    Node *node = model_2->findChildNodeByName(root, "EVENT");

    Node *parentNode = node->getParentNode();
    int childCount = parentNode->childCount();
    //qDebug()<<"EventNode Parent childCount="<<childCount;
    for (int i = 0; i < childCount; i++)
    {
        Node *child = parentNode->childNodes.at(i);
        if (QString(child->name) == "EVENT")
        {
            eventNodeList.append((EVENT*)child);
        }
    }
    //qDebug()<<"EVENT Node Count="<<eventNodeList.count();

    return eventNodeList;
}

QMap<QString, QString> *MainWindow::getEventNodeInfoByNameAndIndex(QString fullFileName, QString reqEventName, int index)
{
    QList<EVENT*> reqEventNodeList = getEventNodeListByName(fullFileName, reqEventName);

    if (reqEventNodeList.isEmpty() || reqEventNodeList.count() <= index)
        return NULL;

    EVENT *reqEventNode = reqEventNodeList.at(index);
    if (reqEventNode == NULL)
        return NULL;
    QMap<QString, QString>* eventParMap = new QMap<QString, QString>;


    eventParMap->insert("Name", reqEventNode->name);
    eventParMap->insert("EventChannelName", reqEventNode->fixPar("EventChannelName").c_str());
    eventParMap->insert("ShortName", reqEventNode->fixPar("ShortName").c_str());
    eventParMap->insert("ChannelNumber", reqEventNode->fixPar("ChannelNumber").c_str());
    eventParMap->insert("EventType", reqEventNode->fixPar("EventType").c_str());
    eventParMap->insert("MAX_DAQ_LIST", reqEventNode->fixPar("MAX_DAQ_LIST").c_str());
    eventParMap->insert("EventChannelTimeCycle", reqEventNode->fixPar("EventChannelTimeCycle").c_str());
    eventParMap->insert("EventChannelTimeUnit", reqEventNode->fixPar("EventChannelTimeUnit").c_str());
    eventParMap->insert("EventChannelPriority", reqEventNode->fixPar("EventChannelPriority").c_str());

    return eventParMap;
}

QList<EVENT *> MainWindow::getEventNodeListInXcpOnCanByName(QString fullFileName, QString reqEventName, XCP_ON_CAN *xcpOnCanNode)
{
    QList<EVENT*> eventNodeList;
    eventNodeList.clear();

    if (projectList->isEmpty())
        return eventNodeList;
    if (projectList->keys().indexOf(fullFileName) == -1)
        return eventNodeList;

    Node *node = model_2->findChildNodeByName(xcpOnCanNode, "EVENT");
    if (node == NULL)
        return eventNodeList;

    Node *parentNode = node->getParentNode();
    int childCount = parentNode->childCount();
    //qDebug()<<"EventNode In XCP_ON_CAN Parent childCount="<<childCount;
    for (int i = 0; i < childCount; i++)
    {
        Node *child = parentNode->childNodes.at(i);
        if (QString(child->name) == "EVENT")
        {
            eventNodeList.append((EVENT*)child);
        }
    }
    //qDebug()<<"EVENT Node In XCP_ON_CAN Count="<<eventNodeList.count();

    return eventNodeList;
}

QMap<QString, QString> *MainWindow::getEventNodeInfoInXcpOnCanByNameAndIndex(QString fullFileName, QString reqEventName, int index, int indexOfXcpOnCan)
{
    QList<XCP_ON_CAN*> reqXcpOnCANNodeList = getXcpOnCANNodeListByName(fullFileName, "XCP_ON_CAN");

    if (reqXcpOnCANNodeList.isEmpty() || reqXcpOnCANNodeList.count() <= indexOfXcpOnCan)
        return NULL;
    XCP_ON_CAN *reqXcpOnCANNode = reqXcpOnCANNodeList.at(indexOfXcpOnCan);
    if (reqXcpOnCANNode == NULL)
        return NULL;

    QList<EVENT*> reqEventNodeList = getEventNodeListInXcpOnCanByName(fullFileName, reqEventName, reqXcpOnCANNode);

    if (reqEventNodeList.isEmpty() || reqEventNodeList.count() <= index)
        return NULL;

    EVENT *reqEventNode = reqEventNodeList.at(index);
    if (reqEventNode == NULL)
        return NULL;
    QMap<QString, QString>* eventParMap = new QMap<QString, QString>;


    eventParMap->insert("Name", reqEventNode->name);
    eventParMap->insert("EventChannelName", reqEventNode->fixPar("EventChannelName").c_str());
    eventParMap->insert("ShortName", reqEventNode->fixPar("ShortName").c_str());
    eventParMap->insert("ChannelNumber", reqEventNode->fixPar("ChannelNumber").c_str());
    eventParMap->insert("EventType", reqEventNode->fixPar("EventType").c_str());
    eventParMap->insert("MAX_DAQ_LIST", reqEventNode->fixPar("MAX_DAQ_LIST").c_str());
    eventParMap->insert("EventChannelTimeCycle", reqEventNode->fixPar("EventChannelTimeCycle").c_str());
    eventParMap->insert("EventChannelTimeUnit", reqEventNode->fixPar("EventChannelTimeUnit").c_str());
    eventParMap->insert("EventChannelPriority", reqEventNode->fixPar("EventChannelPriority").c_str());

    return eventParMap;
}

QMap<QString, QString> *MainWindow::getDaqMemConsumpNodeInfoInXcpOnCanByName(QString fullFileName, QString reqDaqMemConsumpName, int indexOfXcpOnCan)
{
    QList<XCP_ON_CAN*> reqXcpOnCANNodeList = getXcpOnCANNodeListByName(fullFileName, "XCP_ON_CAN");

    if (reqXcpOnCANNodeList.isEmpty() || reqXcpOnCANNodeList.count() <= indexOfXcpOnCan)
        return NULL;
    XCP_ON_CAN *reqXcpOnCANNode = reqXcpOnCANNodeList.at(indexOfXcpOnCan);
    if (reqXcpOnCANNode == NULL)
        return NULL;

    Node *node = model_2->findChildNodeByName(reqXcpOnCANNode, "DAQ_MEMORY_CONSUMPTION");
    DAQ_MEMORY_CONSUMPTION *reqDaqMemConsumpNode = (DAQ_MEMORY_CONSUMPTION*)node;
    qDebug()<<"get DaqMemConsump in XCP_ON_CAN Node";

    if (reqDaqMemConsumpNode == NULL)
        return NULL;
    QMap<QString, QString>* daqMemConsumpParMap = new QMap<QString, QString>;
    qDebug()<<"DaqMemConsump Node is valid";

    daqMemConsumpParMap->insert("Name", reqDaqMemConsumpNode->name);
    daqMemConsumpParMap->insert("DAQ_MEMORY_LIMIT", reqDaqMemConsumpNode->fixPar("DAQ_MEMORY_LIMIT").c_str());
    daqMemConsumpParMap->insert("DAQ_SIZE", reqDaqMemConsumpNode->fixPar("DAQ_SIZE").c_str());
    daqMemConsumpParMap->insert("ODT_SIZE", reqDaqMemConsumpNode->fixPar("ODT_SIZE").c_str());
    daqMemConsumpParMap->insert("ODT_ENTRY_SIZE", reqDaqMemConsumpNode->fixPar("ODT_ENTRY_SIZE").c_str());
    daqMemConsumpParMap->insert("ODT_DAQ_BUFFER_ELEMENT_SIZE", reqDaqMemConsumpNode->fixPar("ODT_DAQ_BUFFER_ELEMENT_SIZE").c_str());
    daqMemConsumpParMap->insert("ODT_STIM_BUFFER_ELEMENT_SIZE", reqDaqMemConsumpNode->fixPar("ODT_STIM_BUFFER_ELEMENT_SIZE").c_str());

    return daqMemConsumpParMap;


}

QList<EVENT_CAN_ID_LIST *> MainWindow::getEventCanIdListNodeListByName(QString fullFileName, QString reqEventCanIdListName)
{
    QList<EVENT_CAN_ID_LIST*> eventCanIdListNodeList;
    eventCanIdListNodeList.clear();

    if (projectList->isEmpty())
        return eventCanIdListNodeList;
    if (projectList->keys().indexOf(fullFileName) == -1)
        return eventCanIdListNodeList;
    WorkProject *wp = projectList->value(fullFileName);

    Node *root = wp->a2lFile;
    Node *node = model_2->findChildNodeByName(root, "EVENT_CAN_ID_LIST");
    if (node == NULL)
        return eventCanIdListNodeList;
    Node *parentNode = node->getParentNode();
    int childCount = parentNode->childCount();
    qDebug()<<"EventCanIdListNode Parent childCount="<<childCount;
    for (int i = 0; i < childCount; i++)
    {
        Node *child = parentNode->childNodes.at(i);
        if (QString(child->name) == "EVENT_CAN_ID_LIST")
        {
            eventCanIdListNodeList.append((EVENT_CAN_ID_LIST*)child);
        }
    }
    qDebug()<<"EVENT_CAN_ID_LIST Node Count="<<eventCanIdListNodeList.count();

    return eventCanIdListNodeList;
}

QMap<QString, QString> *MainWindow::getEventCanIdListNodeInfoByNameAndIndex(QString fullFileName, QString reqEventCanIdListName, int index)
{
    QList<EVENT_CAN_ID_LIST*> reqEventCanIdListNodeList = getEventCanIdListNodeListByName(fullFileName, reqEventCanIdListName);

    if (reqEventCanIdListNodeList.isEmpty() || reqEventCanIdListNodeList.count() <= index)
        return NULL;

    EVENT_CAN_ID_LIST *reqEventCanIdListNode = reqEventCanIdListNodeList.at(index);
    if (reqEventCanIdListNode == NULL)
        return NULL;

    QMap<QString, QString>* eventCanIdListParMap = new QMap<QString, QString>;

    FIXED *fixedItem = (FIXED*)reqEventCanIdListNode->getItem("FIXED");


    eventCanIdListParMap->insert("Name", reqEventCanIdListNode->name);
    eventCanIdListParMap->insert("EventNumber", reqEventCanIdListNode->fixPar("EventNumber").c_str());
    //qDebug()<<"~~~~~TEST1~~~~~";
    if (fixedItem)
    {
        eventCanIdListParMap->insert("Event_CanId", QString(fixedItem->getPar("CAN_ID")));
    }
    else
    {
        eventCanIdListParMap->insert("Event_CanId", "0x0");
    }
    //qDebug()<<"~~~~~TEST2~~~~~";

    return eventCanIdListParMap;
}

QList<DAQ_LIST_CAN_ID *> MainWindow::getDaqListCanIdNodeListByName(QString fullFileName, QString reqDaqListCanIdName)
{
    QList<DAQ_LIST_CAN_ID*> daqListCanIdNodeList;
    daqListCanIdNodeList.clear();

    if (projectList->isEmpty())
        return daqListCanIdNodeList;
    if (projectList->keys().indexOf(fullFileName) == -1)
        return daqListCanIdNodeList;
    WorkProject *wp = projectList->value(fullFileName);

    Node *root = wp->a2lFile;
    Node *node = model_2->findChildNodeByName(root, "DAQ_LIST_CAN_ID");
    if (node == NULL)
        return daqListCanIdNodeList;
    Node *parentNode = node->getParentNode();
    int childCount = parentNode->childCount();
    qDebug()<<"DaqListCanId Node Parent childCount="<<childCount;
    for (int i = 0; i < childCount; i++)
    {
        Node *child = parentNode->childNodes.at(i);
        if (QString(child->name) == "DAQ_LIST_CAN_ID")
        {
            daqListCanIdNodeList.append((DAQ_LIST_CAN_ID*)child);
        }
    }
    qDebug()<<"DAQ_LIST_CAN_ID Node Count="<<daqListCanIdNodeList.count();

    return daqListCanIdNodeList;
}

QMap<QString, QString> *MainWindow::getDaqListCanIdNodeInfoByNameAndIndex(QString fullFileName, QString reqDaqListCanIdName, int index)
{
    QList<DAQ_LIST_CAN_ID*> reqDaqListCanIdNodeList = getDaqListCanIdNodeListByName(fullFileName, reqDaqListCanIdName);

    if (reqDaqListCanIdNodeList.isEmpty() || reqDaqListCanIdNodeList.count() <= index)
        return NULL;

    DAQ_LIST_CAN_ID *reqDaqListCanIdNode = reqDaqListCanIdNodeList.at(index);
    if (reqDaqListCanIdNode == NULL)
        return NULL;
    QMap<QString, QString>* daqListCanIdParMap = new QMap<QString, QString>;

    FIXED *fixedItem = (FIXED*)reqDaqListCanIdNode->getItem("FIXED");

    daqListCanIdParMap->insert("Name", reqDaqListCanIdNode->name);
    daqListCanIdParMap->insert("DaqListNumber", reqDaqListCanIdNode->fixPar("DaqList").c_str());
    if (fixedItem)
    {
        daqListCanIdParMap->insert("DaqList_CanId", QString(fixedItem->getPar("CAN_ID")));
    }
    else
    {
        daqListCanIdParMap->insert("DaqList_CanId", "0x0");
    }

    return daqListCanIdParMap;
}

COMPU_VTAB *MainWindow::getCompVtabNodeByName(QString fullFileName, QString reqCompVtabName)
{
    if (projectList->isEmpty())
        return NULL;
    if (projectList->keys().indexOf(fullFileName) == -1)
        return NULL;
    WorkProject *wp = projectList->value(fullFileName);

    Node *root = wp->a2lFile;
    Node *compVtabNode = model_2->findChildNodeByName(root, "COMPU_VTAB");
    int compVtabCount = compVtabNode->childCount();

    for(int i = 0; i < compVtabCount; i++)
    {
        COMPU_VTAB *reqCompVtabNode = (COMPU_VTAB*)compVtabNode->childNodes.at(i);
        if (reqCompVtabName == reqCompVtabNode->name)
        {
            return reqCompVtabNode;
        }
    }
    return NULL;
}

QMap<QString, QString> *MainWindow::getCompVtabNodeInfoByName(QString fullFileName, QString reqCompVtabName)
{
    COMPU_VTAB *reqCompVtabNode = getCompVtabNodeByName(fullFileName, reqCompVtabName);
    if (reqCompVtabNode == NULL)
        return NULL;
    QMap<QString, QString>* compVtabParMap = new QMap<QString, QString>;

    compVtabParMap->insert("Name", reqCompVtabNode->name);
    compVtabParMap->insert("LongIdentifier", reqCompVtabNode->fixPar("LongIdentifier").c_str());
    compVtabParMap->insert("ConversionType", reqCompVtabNode->fixPar("ConversionType").c_str());
    compVtabParMap->insert("NumberValuePairs", reqCompVtabNode->fixPar("NumberValuePairs").c_str());

    QString valuePairStr = "";
    QStringList valueList = reqCompVtabNode->getValueList();
    foreach (QString value, valueList) {
        int key = reqCompVtabNode->getPos(value);
        valuePairStr += (QString::number(key) + "-" + value);
        if (value != valueList.last())
        {
            valuePairStr += ",";
        }
    }
    compVtabParMap->insert("ValuePairs", valuePairStr);

    return compVtabParMap;
}

QList<MEMORY_SEGMENT *> MainWindow::getMemorySegmentNodeListByName(QString fullFileName, QString reqMemorySegmentName)
{
    QList<MEMORY_SEGMENT*> memorySegmentNodeList;
    memorySegmentNodeList.clear();

    if (projectList->isEmpty())
        return memorySegmentNodeList;
    if (projectList->keys().indexOf(fullFileName) == -1)
        return memorySegmentNodeList;
    WorkProject *wp = projectList->value(fullFileName);
    Node *root = wp->a2lFile;

    Node *modPar = model_2->findChildNodeByName(root, "MOD_PAR");
    if(!modPar) return memorySegmentNodeList;

    Node *node = model_2->findChildNodeByName(modPar, "MEMORY_SEGMENT");
    if (node == NULL)
    {
        qDebug()<<"Can't find MEMORY_SEGMENT node";
        return memorySegmentNodeList;
    }
    Node *parentNode = node->getParentNode();
    int childCount = parentNode->childCount();
    for (int i = 0; i < childCount; i++)
    {
        Node *child = parentNode->childNodes.at(i);
        if (QString(child->name) == "MEMORY_SEGMENT")
        {
            memorySegmentNodeList.append((MEMORY_SEGMENT*)child);
        }
    }
    //qDebug()<<"MEMORY_SEGMENT Node Count="<<memorySegmentNodeList.count();

    return memorySegmentNodeList;
}

QMap<QString, QString> *MainWindow::getMemorySegmentNodeInfoInA2LFileByIndex(QString fullFileName, QString reqMemorySegmentName, int index)
{
    QList<MEMORY_SEGMENT*> reqMemorySegmentNodeList = getMemorySegmentNodeListByName(fullFileName, reqMemorySegmentName);
    if (reqMemorySegmentNodeList.isEmpty() || reqMemorySegmentNodeList.count() <= index)
        return NULL;
    MEMORY_SEGMENT *reqMemorySegmentNode = reqMemorySegmentNodeList.at(index);
    if (reqMemorySegmentNode == NULL)
        return NULL;
    QMap<QString, QString>* memorySegmentParMap = new QMap<QString, QString>;

    memorySegmentParMap->insert("Name", reqMemorySegmentNode->fixPar("Name").c_str());
    memorySegmentParMap->insert("LongIdentifier", reqMemorySegmentNode->fixPar("LongIdentifier").c_str());
    memorySegmentParMap->insert("PrgType", reqMemorySegmentNode->fixPar("PrgType").c_str());
    memorySegmentParMap->insert("Memorytype", reqMemorySegmentNode->fixPar("Memorytype").c_str());
    memorySegmentParMap->insert("Attribute", reqMemorySegmentNode->fixPar("Attribute").c_str());
    memorySegmentParMap->insert("Address", reqMemorySegmentNode->fixPar("Address").c_str());
    memorySegmentParMap->insert("Size", reqMemorySegmentNode->fixPar("Size").c_str());
    memorySegmentParMap->insert("Offset1", reqMemorySegmentNode->fixPar("Offset1").c_str());
    memorySegmentParMap->insert("Offset2", reqMemorySegmentNode->fixPar("Offset2").c_str());
    memorySegmentParMap->insert("Offset3", reqMemorySegmentNode->fixPar("Offset3").c_str());
    memorySegmentParMap->insert("Offset4", reqMemorySegmentNode->fixPar("Offset4").c_str());
    memorySegmentParMap->insert("Offset5", reqMemorySegmentNode->fixPar("Offset5").c_str());

    SEGMENT *segMentNode = (SEGMENT*)model_2->findChildNodeByName(reqMemorySegmentNode, "SEGMENT");
    if(segMentNode)
    {
        memorySegmentParMap->insert("SEGMENT_INDEX", segMentNode->fixPar("SEGMENT_INDEX").c_str());
        memorySegmentParMap->insert("PAGE_NUMBER", segMentNode->fixPar("PAGE_NUMBER").c_str());
        memorySegmentParMap->insert("ADDRESS_EXTENSION", segMentNode->fixPar("ADDRESS_EXTENSION").c_str());
        memorySegmentParMap->insert("COMPRESSION_METHOD", segMentNode->fixPar("COMPRESSION_METHOD").c_str());
        memorySegmentParMap->insert("ENCRYPTION_METHOD", segMentNode->fixPar("ENCRYPTION_METHOD").c_str());

        CHECKSUM *checksumNode = (CHECKSUM*)model_2->findChildNodeByName(segMentNode, "CHECKSUM");
        if(checksumNode)
        {
            memorySegmentParMap->insert("CHECKSUM_TYPE", checksumNode->fixPar("CHECKSUM_TYPE").c_str());
            MAX_BLOCK_SIZE *maxBlockSizeItem = (MAX_BLOCK_SIZE*)checksumNode->getItem("MAX_BLOCK_SIZE");
            if(maxBlockSizeItem)
                memorySegmentParMap->insert("MAX_BLOCK_SIZE", maxBlockSizeItem->getPar("MAX_BLOCK_SIZE"));
        }
    }

    return memorySegmentParMap;
}

QList<PAGE *> MainWindow::getPageNodeListInMemorySegmentByName(QString fullFileName, QString reqPageName, MEMORY_SEGMENT *memorySegmentNode)
{
    QList<PAGE*> pageNodeList;
    pageNodeList.clear();

    if(!memorySegmentNode)
        return pageNodeList;

    if (projectList->isEmpty())
        return pageNodeList;
    if (projectList->keys().indexOf(fullFileName) == -1)
        return pageNodeList;

    SEGMENT *segmentNode = (SEGMENT*)model_2->findChildNodeByName(memorySegmentNode, "SEGMENT");
    if(!segmentNode)
    {
        qDebug()<<"Can't find SEGMENT node";
        return pageNodeList;
    }

    Node *node = model_2->findChildNodeByName(segmentNode, "PAGE");
    if (!node)
        return pageNodeList;

    Node *parentNode = node->getParentNode();
    int childCount = parentNode->childCount();
    //qDebug()<<"PageNode In SEGMENT Parent childCount="<<childCount;
    for (int i = 0; i < childCount; i++)
    {
        Node *child = parentNode->childNodes.at(i);
        if (QString(child->name) == "PAGE")
        {
            pageNodeList.append((PAGE*)child);
        }
    }
    //qDebug()<<"PAGE Node In MEMORY_SEGMENT Count="<<pageNodeList.count();

    return pageNodeList;
}

QMap<QString, QString> *MainWindow::getPageNodeInfoInMemorySegmentByNameAndIndex(QString fullFileName, QString reqPageName, int index, int indexOfMemorySegment)
{
    QList<MEMORY_SEGMENT*> reqMemorySegmentNodeList = getMemorySegmentNodeListByName(fullFileName, "MEMORY_SEGMENT");

    if (reqMemorySegmentNodeList.isEmpty() || reqMemorySegmentNodeList.count() <= indexOfMemorySegment)
        return NULL;
    MEMORY_SEGMENT *reqMemorySegmentNode = reqMemorySegmentNodeList.at(indexOfMemorySegment);
    if (reqMemorySegmentNode == NULL)
        return NULL;

    QList<PAGE*> reqPageNodeList = getPageNodeListInMemorySegmentByName(fullFileName, reqPageName, reqMemorySegmentNode);

    if (reqPageNodeList.isEmpty() || reqPageNodeList.count() <= index)
        return NULL;

    PAGE *reqPageNode = reqPageNodeList.at(index);
    if (reqPageNode == NULL)
        return NULL;
    QMap<QString, QString>* pageParMap = new QMap<QString, QString>;


    pageParMap->insert("Name", reqPageNode->name);
    pageParMap->insert("PAGE_INDEX", reqPageNode->fixPar("PAGE_INDEX").c_str());
    pageParMap->insert("ECU_ACCESS_TYPE", reqPageNode->fixPar("ECU_ACCESS_TYPE").c_str());
    pageParMap->insert("XCP_READ_ACCESS_TYPE", reqPageNode->fixPar("XCP_READ_ACCESS_TYPE").c_str());
    pageParMap->insert("XCP_WRITE_ACCESS_TYPE", reqPageNode->fixPar("XCP_WRITE_ACCESS_TYPE").c_str());

    return pageParMap;
}

bool MainWindow::addHexFile(QString fullFileName, QString fullHexName, QStringList &infoList)
{
    if (projectList->isEmpty())
        return false;
    if (projectList->keys().indexOf(fullFileName) == -1)
        return false;

    if (fullHexName.isEmpty())
        return false;

    WorkProject *wp = projectList->value(fullFileName);

    if (!wp->a2lFile->isParsed())
    {
        readA2l(wp);
    }

    //check if A2l parsing was successfull
    if (!wp->isOk())
    {
        QMessageBox::information(this,"HEXplorer","action open new dataset failed. A2Lfile is not parsed correctly.");
        writeOutput("action open new dataset failed : A2Lfile could not be parsed correctly.");
        return false;
    }

    //if no MOD_COMMON in ASAP file
    if (wp->a2lFile->getProject()->getNode("MODULE") == NULL)
    {
        QMessageBox::information(this, "HEXplorer", tr("no MOD_COMMON in ASAP file"));
        writeOutput("action open new dataset : no MOD_COMMON in ASAP file");
        return false;
    }

    //if the selected Hex file is already into the project => exit
    if (wp->hexFiles().contains(fullHexName))
    {
        //QMessageBox::information(this, "HEXplorer", "HEX file : " + fullHexName
        //                         + "\nalready included into the selected project");
        writeOutput("action open new dataset : HEX file already in project");
        return false;
    }

    //update currentHexPath
    QSettings settings(qApp->organizationName(), qApp->applicationName());
    QString currentHexPath = QFileInfo(fullHexName).absolutePath();
    settings.setValue("currentHexPath", currentHexPath);

    //start a timer
    double ti = omp_get_wtime();

    //add the file to the project (module)
    HexFile *hex = NULL;
    QList<MODULE*> list = wp->a2lFile->getProject()->listModule();
    if (list.count() == 0)
    {
        writeOutput("action open new dataset : no Module into A2l file !");
        return false;
    }
    else if (list.count() == 1)
    {
        hex = new HexFile(fullHexName, wp, QString(list.at(0)->name));
    }
    else
    {
        // select a module

        QString module;
        DialogChooseModule *diag = new DialogChooseModule(&module);
        QStringList listModuleName;
        foreach (MODULE* module, list)
        {
            listModuleName.append(module->name);
        }
        diag->setList(listModuleName);
        int ret = diag->exec();

        if (ret == QDialog::Accepted)
        {
            hex = new HexFile(fullHexName, wp, module);
        }
        else
        {
            writeOutput("action open new dataset : no module chosen !");
            return false;
        }

    }

    // display status bar
    statusBar()->show();
    progBar->reset();
    connect(hex, SIGNAL(progress(int,int)), this, SLOT(setValueProgressBar(int,int)), Qt::DirectConnection);

    if (hex->read())
    {
        wp->addHex(hex, wp);
    }
    else
        delete hex;

    // hide the statusbar
    statusBar()->hide();
    progBar->reset();

    //stop timer
    double tf = omp_get_wtime();

    //update the treeView model
    //ui->treeView->expand(index);
    ui->treeView->resizeColumnToContents(0);
    editCharac->setEnabled(true);

    writeOutput("action open new dataset : HEX file "  + fullHexName + " add to project in " + QString::number(tf-ti) + " sec");

    return true;

}

QMap<QString, QStringList> *MainWindow::getCharHexValueByName(QString fullFileName, QString fullHexName, QString reqCharName, bool *_ok)
{
    if (projectList->isEmpty())
    {
        *_ok = false;
        return NULL;
    }
    if (projectList->keys().indexOf(fullFileName) == -1)
    {
        *_ok = false;
        return NULL;
    }
    WorkProject *wp = projectList->value(fullFileName);
    HexFile *hexFile = wp->getHex(fullHexName);

    if (hexFile == NULL)
    {
        *_ok = false;
        return NULL;
    }

    Data* charData = hexFile->getData(reqCharName);
    if (charData == NULL)
    {
        *_ok = false;
        return NULL;
    }

    *_ok = true;
    QMap<QString, QStringList> *charHexInfoMap = new QMap<QString, QStringList>;

    QStringList xList = charData->getX();
    QStringList yList = charData->getY();
    QStringList zList = charData->getZ();
    int xCount = charData->xCount();
    int yCount = charData->yCount();
    int zCount = charData->zCount();
    //qDebug()<<"xList Count="<<xCount;
    //qDebug()<<"yList Count="<<yCount;
    //qDebug()<<"zList Count="<<zCount;

    QStringList xyzCountList;
    xyzCountList << QString::number(xCount) << QString::number(yCount) << QString::number(zCount);

    charHexInfoMap->insert("xyzCountList", xyzCountList);
    charHexInfoMap->insert("charXList", xList);
    charHexInfoMap->insert("charYList", yList);
    charHexInfoMap->insert("charZList", zList);

    /*
    QString valueStr;
    for (int i = 0; i < xCount; i++)
    {
        valueStr += xList.at(i);
        valueStr += "-";
    }
    qDebug()<<"xList:"<<valueStr;
    valueStr.clear();

    for (int i = 0; i < yCount; i++)
    {
        valueStr += yList.at(i);
        valueStr += "-";
    }
    qDebug()<<"yList:"<<valueStr;
    valueStr.clear();

    for (int i = 0; i < zCount; i++)
    {
        valueStr += zList.at(i);
        valueStr += "-";
    }
    qDebug()<<"zList:"<<valueStr;
    valueStr.clear();
    */

    return charHexInfoMap;

}





QStringList MainWindow::getAllMeasParamsNames(A2lTreeModel *a2lModel)
{
    QStringList measNameList;
    measNameList.clear();

    if (a2lModel == NULL)
        return measNameList;

    qDebug()<<"test";
    qDebug()<<"a2lModel root child Count="<<a2lModel->getRootNode();
    if (a2lModel->getRootNode() == 0)
        return measNameList;

    Node *root = a2lModel->getRootNode();

    Node *measNode = a2lModel->findNodeByName("MEASUREMENT");
    //Node *characNode = a2lModel->findNodeByName("CHARACTERISTIC");


    if (measNode == NULL)
        return measNameList;

    int measCount = measNode->childNodes.count();
    qDebug()<<"Measurement Params Count="<<measCount;
    for (int i = 0; i < measCount; i++)
    {
        measNameList.append(measNode->childNodes.at(i)->name);
    }

    return measNameList;
}

QStringList MainWindow::getAllCharacParamsNames(A2lTreeModel *a2lModel)
{
    QStringList characNameList;
    characNameList.clear();

    if (a2lModel == NULL)
        return characNameList;
    if (a2lModel->getRootNode() == 0)
        return characNameList;

    Node *root = a2lModel->getRootNode();

    Node *characNode = a2lModel->findNodeByName("CHARACTERISTIC");


    if (characNode == NULL)
        return characNameList;

    int characCount = characNode->childNodes.count();
    qDebug()<<"Characteristic Params Count="<<characCount;
    for (int i = 0; i < characCount; i++)
    {
        characNameList.append(characNode->childNodes.at(i)->name);
    }

    return characNameList;
}


void MainWindow::loadAndParserA2lFile(QString fullFileName, A2lTreeModel *a2lModel, QStringList &infoList)
{

    // create a new Wp
    WorkProject *wp = new WorkProject(fullFileName, a2lModel);

    // parse the a2l file
    wp->parse();

    a2lModel->addNode2RootNode(wp->a2lFile);

    //messagebox in case of a2l parser error
    if (!wp->isOk())
    {
        QMessageBox::warning(this, "A2L_Parser" ,"ASAP file parser error");
    }

    infoList.clear();
    infoList.append(wp->_outputList());

}


void MainWindow::on_actionA2LOpen_triggered()
{
    QSettings settings(qApp->organizationName(), qApp->applicationName());
    QString path = settings.value("currentA2lPath").toString();
    //QString path = "D:\\";

    const QStringList files =
        QFileDialog::getOpenFileNames(this,
                                      tr("select an Asap file (A2L)"), path,
                                      tr("ASAP files (*.a2l);;all files (*.*)"));

    if (files.isEmpty())
    {
       statusBar()->showMessage(tr("Loading canceled"), 2000);
       writeOutput("action open new project : no file selected");
       return;
    }
    else
    {
        //intialize projectList if necessary
       if (projectList == NULL)
            projectList = new QMultiMap<QString, WorkProject*>;

       //check if the project is already open
       foreach (QString fileName, files)
       {
           openProject(fileName);
           ui->treeView->resizeColumnToContents(0);
           qDebug()<<"Open a2l file:"<<fileName;


           /*
           A2lTreeModel *a2lTreeModel = new A2lTreeModel();
           QStringList infoList;
           loadAndParserA2lFile(fileName, a2lTreeModel, infoList);
           if (a2lTreeModel->getRootNode() != 0)
                qDebug()<<"a2lTreeModel root Node:"<<a2lTreeModel->getRootNode()->childNodes.at(0)->name;
           qDebug()<<"Parser Info: ==============";
           for (int i = 0; i < infoList.count(); i++)
               qDebug()<<infoList.at(i);
            */


       }
    }
}

void MainWindow::setValueProgressBar(int n, int max)
{
    qApp->processEvents(QEventLoop::ExcludeUserInputEvents);
    progBar->setMaximum(max);
    progBar->setValue(n);
}

void MainWindow::initToolBars()
{

}

void MainWindow::createMenus()
{

}

void MainWindow::createActions()
{
    editCharac = new QAction(tr("Edit..."), this);
    connect(editCharac, SIGNAL(triggered()), this, SLOT(editChar()));
    editCharac->setDisabled(true);

    delA2lProject = new QAction(tr("Delete"), this);
    connect(delA2lProject, SIGNAL(triggered()), this, SLOT(delA2lProjectSlot()));
    delA2lProject->setDisabled(false);
}

void MainWindow::showContextMenu(QPoint)
{
    QMenu menu;

    if (model_2->getRootNode() != 0)
    {
        QModelIndex index = ui->treeView_2->selectionModel()->currentIndex();
        Node *node = model_2->getNode(index);
        QString name = typeid(*node).name();

        if (name.toLower().endsWith("characteristic") || name.toLower().endsWith("axis_pts"))
        {
            menu.addAction(editCharac);
        }
    }

    menu.exec(QCursor::pos());
}

void MainWindow::showContextMenuInWpTree(QPoint)
{
    QMenu menu;

    if (model->getRootNode() != 0)
    {
        QModelIndex index = ui->treeView->selectionModel()->currentIndex();
        Node *node = model->getNode(index);
        QString name = typeid(*node).name();

        if (name.toLower().endsWith("workproject"))
        {
            menu.addAction(delA2lProject);
        }

    }

    menu.exec(QCursor::pos());
}

void MainWindow::resizeColumn0()
{
    ui->treeView->resizeColumnToContents(0);
}

void MainWindow::doubleClicked(QModelIndex)
{
    QModelIndex index  = ui->treeView->selectionModel()->currentIndex();
    Node *node = model->getNode(index);

    QString name = typeid(*node).name();
    qDebug()<<"name ="<<name;
    if (name.toLower().endsWith("hexfile") || name.toLower().endsWith("srecfile") ||
        name.toLower().endsWith("csv") || name.toLower().endsWith("cdfxfile") || name.toLower().endsWith("dcm"))
    {
        //quicklookFile();
    }

    if (name.toLower().endsWith("workproject"))
    {

    }

}

void MainWindow::doubleClicked_treeView2(QModelIndex)
{
    QModelIndex index  = ui->treeView_2->selectionModel()->currentIndex();
    Node *node = model_2->getNode(index);

    QString name = typeid(*node).name();
    qDebug()<<"name ="<<name;

    QString childName = "";
    Node *child = node->getNode(1);
    if (child)
        childName = typeid(*child).name();
    qDebug()<<"child name ="<<childName;

    if (name.toLower().endsWith("characteristic"))
    {
        QList<Node*> list;
        list.append(node);

        CharModel *charModel = new CharModel();
        charModel->setList(list);

        ui->tableView->setModel(charModel);
    }

    if (name.toLower().endsWith("node") && childName.toLower().endsWith("characteristic"))
    {
        QList<Node*> list;
        list = node->childNodes;

        CharModel *charModel = new CharModel();
        charModel->setList(list);

        ui->tableView->setModel(charModel);
        ui->tableView->setAlternatingRowColors(true);
    }

    if (name.toLower().endsWith("measurement"))
    {
        QList<Node*> list;
        list.append(node);

        MeasModel *measModel = new MeasModel();
        measModel->setList(list);

        ui->tableView->setModel(measModel);
    }

    if (name.toLower().endsWith("node") && childName.toLower().endsWith("measurement"))
    {
        QList<Node*> list;
        list = node->childNodes;

        MeasModel *measModel = new MeasModel();
        measModel->setList(list);

        ui->tableView->setModel(measModel);
        ui->tableView->setAlternatingRowColors(true);
    }

    if (name.toLower().endsWith("compu_method"))
    {
        QList<Node*> list;
        list.append(node);

        CompuMethodModel *compuMethodModel = new CompuMethodModel();
        compuMethodModel->setList(list);

        ui->tableView->setModel(compuMethodModel);
    }

    if (name.toLower().endsWith("node") && childName.toLower().endsWith("compu_method"))
    {
        QList<Node*> list;
        list = node->childNodes;

        CompuMethodModel *compuMethodModel = new CompuMethodModel();
        compuMethodModel->setList(list);

        ui->tableView->setModel(compuMethodModel);
        ui->tableView->setAlternatingRowColors(true);
    }


    if (name.toLower().endsWith("compu_vtab"))
    {
        QList<Node*> list;
        list.append(node);

        CompuVtabModel *compuVtabModel = new CompuVtabModel();
        compuVtabModel->setList(list);

        ui->tableView->setModel(compuVtabModel);
    }

    if (name.toLower().endsWith("node") && childName.toLower().endsWith("compu_vtab"))
    {
        QList<Node*> list;
        list = node->childNodes;

        CompuVtabModel *compuVtabModel = new CompuVtabModel();
        compuVtabModel->setList(list);

        ui->tableView->setModel(compuVtabModel);
        ui->tableView->setAlternatingRowColors(true);
    }


    if (name.toLower().endsWith("axis_pts"))
    {
        QList<Node*> list;
        list.append(node);

        AxisPtsModel *axisPtsModel = new AxisPtsModel();
        axisPtsModel->setList(list);

        ui->tableView->setModel(axisPtsModel);
    }

    if (name.toLower().endsWith("node") && childName.toLower().endsWith("axis_pts"))
    {
        QList<Node*> list;
        list = node->childNodes;

        AxisPtsModel *axisPtsModel = new AxisPtsModel();
        axisPtsModel->setList(list);

        ui->tableView->setModel(axisPtsModel);
        ui->tableView->setAlternatingRowColors(true);
    }

}


bool MainWindow::openProject(QString &fullFileName)
{
    if (projectList->contains(fullFileName))
    {
        //QMessageBox::information(this, "HEXplorer", fullFileName + " already open !");
        return false;
    }

    //update currentA2lPath
    QSettings settings(qApp->organizationName(), qApp->applicationName());
    QString currentA2lPath = QFileInfo(fullFileName).absolutePath();
    settings.setValue("currentA2lPath", currentA2lPath);

    // create a new Wp
    WorkProject *wp = new WorkProject(fullFileName, this->model, this);

    // display status bar
    statusBar()->show();
    progBar->reset();
    connect(wp, SIGNAL(incProgressBar(int,int)), this, SLOT(setValueProgressBar(int,int)), Qt::DirectConnection);

    // parse the a2l file
    wp->parse();
    wp->attach(this);

    // hide the statusbar
    statusBar()->hide();
    progBar->reset();

    //copy the model


    //update the ui->treeView
    model->addNode2RootNode(wp);
    if (ui->treeView->model() != model)
        ui->treeView->setModel(model);
    ui->treeView->setColumnHidden(1, true);

    //update ui->treeView_2
    model_2->addNode2RootNode(wp->a2lFile);
    if (ui->treeView_2->model() != model_2)
        ui->treeView_2->setModel(model_2);
    ui->treeView_2->setColumnHidden(1, true);
    //Node *node = wp->a2lFile->getNode("MODULE");
    //QModelIndex index = model_2->getIndex(node);
    //ui->treeView_2->collapse(index);

    //set completer
    completer->setModel(model);

    //insert the new created project into the projectList
    projectList->insert(fullFileName, wp);

    //messagebox in case of a2l parser error
    if (!wp->isOk())
    {
        QMessageBox::warning(this, "HEXplorer" ,"ASAP file parser error");
        return false;
    }

    //update output console
    ui->listWidget->addItems(wp->_outputList());
    ui->listWidget->scrollToBottom();

    //update current file
    setCurrentFile(fullFileName);

    return true;
}

void MainWindow::setCurrentFile(const QString &fileName)
{
    curFile = fileName;
    setWindowFilePath(curFile);

    QSettings settings(qApp->organizationName(), qApp->applicationName());
    QStringList files = settings.value("recentFileList").toStringList();
    files.removeAll(fileName);
    files.prepend(fileName);
    while (files.size() > MaxRecentFiles)
        files.removeLast();

    settings.setValue("recentFileList", files);
/*
    foreach (QWidget *widget, QApplication::topLevelWidgets()) {
        MDImain *mainWin = qobject_cast<MDImain *>(widget);
        if (mainWin)
            mainWin->updateRecentFileActions();
    }
*/
}


void MainWindow::on_actionAddHex_triggered()
{
    // check if a project is selected in treeView
    QModelIndex index  = ui->treeView->selectionModel()->currentIndex();

    if (index.isValid())
    {
        //get a pointer on the selected item
        Node *node =  model->getNode(index);
        QString name = typeid(*node).name();
        //qDebug()<<"select Node Name = "<<name;

        if (!name.endsWith("WorkProject") && !name.endsWith("DBFILE"))
        {
            QMessageBox::warning(this, "HEXplorer::add hex file to project", "Please select first a project.",
                                             QMessageBox::Ok);
            return;
        }

        int row = index.row();
        if (row < 0)
        {
            QMessageBox::information(this,"HEXplorer","please first select a project");
            writeOutput("action open new dataset cancelled: no project first selected");
            return;
        }
        else
        {
            QSettings settings(qApp->organizationName(), qApp->applicationName());
            QString path = settings.value("currentHexPath").toString();

            QStringList files = QFileDialog::getOpenFileNames(this,
                                              tr("select a dataset (HEX)"), path,
                                              tr("HEX files (*.hex | *.hex_trimmed);;all files (*.*)"));

            if (files.isEmpty())
            {
               statusBar()->showMessage(tr("Loading canceled"), 2000);
               writeOutput("action open new dataset : no HEX file selected");
               return;
            }
            else
            {
                //Get the project (A2l) name
                //QString fullA2lName = model->name(index);

                //create a pointer on the WorkProject
                //WorkProject *wp = projectList->value(fullA2lName);
                WorkProject *wp = (WorkProject*)node;

                if (wp && name.endsWith("WorkProject"))
                {
                    //if the a2lFile is not yet parsed, parse.
                    if (!wp->a2lFile->isParsed())
                    {
                        readA2l(wp);
                    }

                    //check if A2l parsing was successfull
                    if (!wp->isOk())
                    {
                        QMessageBox::information(this,"HEXplorer","action open new dataset failed. A2Lfile is not parsed correctly.");
                        writeOutput("action open new dataset failed : A2Lfile could not be parsed correctly.");
                        return;
                    }

                    //if no MOD_COMMON in ASAP file
                    if (wp->a2lFile->getProject()->getNode("MODULE") == NULL)
                    {
                        QMessageBox::information(this, "HEXplorer", tr("no MOD_COMMON in ASAP file"));
                        writeOutput("action open new dataset : no MOD_COMMON in ASAP file");
                        return;
                    }

                    //check if Hexfile already in project
                    foreach (QString fullHexName, files)
                    {
                        //qDebug()<<"fullHexName = "<<fullHexName;
                        //if the selected Hex file is already into the project => exit
                        if (wp->hexFiles().contains(fullHexName))
                        {
                            QMessageBox::information(this, "HEXplorer", "HEX file : " + fullHexName
                                                     + "\nalready included into the selected project");
                            writeOutput("action open new dataset : HEX file already in project");
                            files.removeOne(fullHexName);
                        }
                    }

                    //Open hex files
                    foreach (QString fullHexName, files)
                    {
                        //update currentHexPath
                        QSettings settings(qApp->organizationName(), qApp->applicationName());
                        QString currentHexPath = QFileInfo(fullHexName).absolutePath();
                        settings.setValue("currentHexPath", currentHexPath);

                        //start a timer
                        double ti = omp_get_wtime();

                        //add the file to the project (module)
                        HexFile *hex = NULL;
                        QList<MODULE*> list = wp->a2lFile->getProject()->listModule();
                        if (list.count() == 0)
                        {
                            writeOutput("action open new dataset : no Module into A2l file !");
                            return;
                        }
                        else if (list.count() == 1)
                        {
                            hex = new HexFile(fullHexName, wp, QString(list.at(0)->name));
                        }
                        else
                        {
                            // select a module

                            QString module;
                            DialogChooseModule *diag = new DialogChooseModule(&module);
                            QStringList listModuleName;
                            foreach (MODULE* module, list)
                            {
                                listModuleName.append(module->name);
                            }
                            diag->setList(listModuleName);
                            int ret = diag->exec();

                            if (ret == QDialog::Accepted)
                            {
                                hex = new HexFile(fullHexName, wp, module);
                            }
                            else
                            {
                                writeOutput("action open new dataset : no module chosen !");
                                return;
                            }

                        }

                        // display status bar
                        statusBar()->show();
                        progBar->reset();
                        //connect(hex, SIGNAL(lineParsed(int,int)), this, SLOT(setValueProgressBar(int,int)), Qt::DirectConnection);
                        connect(hex, SIGNAL(progress(int,int)), this, SLOT(setValueProgressBar(int,int)), Qt::DirectConnection);

                        if (hex->read())
                        {
                            wp->addHex(hex, wp);
                        }
                        else
                            delete hex;

                        // hide the statusbar
                        statusBar()->hide();
                        progBar->reset();

                        //stop timer
                        double tf = omp_get_wtime();


                        //update the treeView model
                        ui->treeView->expand(index);
                        ui->treeView->resizeColumnToContents(0);
                        editCharac->setEnabled(true);

                        writeOutput("action open new dataset : HEX file "  + fullHexName + " add to project in " + QString::number(tf-ti) + " sec");

                    }
                }
                else if (wp && name.endsWith("DBFILE"))
                {
                    //TBD : perform a check of the DB to ensure contains everything to open Hexfile

                    // check if Hexfile already in project
                    foreach (QString fullHexName, files)
                    {
                        //if the selected Hex file is already into the project => exit
                        if (wp->hexFiles().contains(fullHexName))
                        {
                            QMessageBox::information(this, "HEXplorer", "HEX file : " + fullHexName
                                                     + "\nalready included into the selected project");
                            writeOutput("action open new dataset : HEX file already in project");
                            files.removeOne(fullHexName);
                        }
                    }

                    //Open hex files
                    foreach (QString fullHexName, files)
                    {
                        //update currentHexPath
                        QSettings settings(qApp->organizationName(), qApp->applicationName());
                        QString currentHexPath = QFileInfo(fullHexName).absolutePath();
                        settings.setValue("currentHexPath", currentHexPath);

                        //start a timer
                        double ti = omp_get_wtime();

                        //add the file to the project (module)
                        HexFile *hex = new HexFile(fullHexName, wp);

                        // display status bar
                        statusBar()->show();
                        progBar->reset();
                        connect(hex, SIGNAL(progress(int,int)), this, SLOT(setValueProgressBar(int,int)), Qt::DirectConnection);

                        //read hexFile and add node to parent Node
                        if (hex->read_db())
                            wp->addHex(hex, wp);
                        else
                            delete hex;

                        // hide the statusbar
                        statusBar()->hide();
                        progBar->reset();

                        //stop timer
                        double tf = omp_get_wtime();

                        //update the treeView model
                        ui->treeView->expand(index);
                        ui->treeView->resizeColumnToContents(0);

                        writeOutput("action open new dataset into database : HEX file " + fullHexName + " add to project in " + QString::number(tf-ti) + " sec");

                    }
                }
            }
        }
    }
}

void MainWindow::editChar()
{
    QModelIndex index = ui->treeView_2->selectionModel()->currentIndex();
    QModelIndex index_hex = ui->treeView->selectionModel()->currentIndex();

    if (index.isValid() && index_hex.isValid())
    {
        Node *node = model_2->getNode(index);
        QString name = typeid(*node).name();

        Node *nodeHex = model->getNode(index_hex);
        QString nameHex = typeid(*nodeHex).name();


        if (name.toLower().endsWith("characteristic") && nameHex.toLower().endsWith("workproject"))
        {
            //qDebug()<<"edit characteristic triggered.";

            QString characName = ((CHARACTERISTIC*)(node))->getPar("Name");
            qDebug()<<"characName="<<characName;

            QList<Data*> *list = new QList<Data*>;
            WorkProject *wp = (WorkProject*)nodeHex;
            QString hexFileName = wp->hexFiles().keys().at(0);
            HexFile *hex = wp->hexFiles().value(hexFileName);
            Data* data = hex->getData(characName);

            list->append(data);
            //qDebug()<<"list count"<<list.count();
            SpTableModel *spTableModel = new SpTableModel(this);
            spTableModel->setList(list);
            ui->tableView->horizontalHeader()->setDefaultSectionSize(50);
            ui->tableView->verticalHeader()->setDefaultSectionSize(18);
            ui->tableView->setModel(spTableModel);
            ui->tableView->setColumnWidth(0, 200);
        }

        if (name.toLower().endsWith("axis_pts") && nameHex.toLower().endsWith("workproject"))
        {


            QString axisptsName = ((AXIS_PTS*)(node))->getPar("Name");
            qDebug()<<"axisptsName="<<axisptsName;

            QList<Data*> *list = new QList<Data*>;
            WorkProject *wp = (WorkProject*)nodeHex;
            QString hexFileName = wp->hexFiles().keys().at(0);
            HexFile *hex = wp->hexFiles().value(hexFileName);
            Data* data = hex->getData(axisptsName);

            list->append(data);
            //qDebug()<<"list count"<<list.count();
            SpTableModel *spTableModel = new SpTableModel(this);
            spTableModel->setList(list);
            ui->tableView->horizontalHeader()->setDefaultSectionSize(50);
            ui->tableView->verticalHeader()->setDefaultSectionSize(18);
            ui->tableView->setModel(spTableModel);
            ui->tableView->setColumnWidth(0, 200);
        }

    }

}

void MainWindow::delA2lProjectSlot()
{
    QModelIndex index_hex = ui->treeView->selectionModel()->currentIndex();

    if (index_hex.isValid())
    {
        Node *nodeHex = model->getNode(index_hex);
        QString nameHex = typeid(*nodeHex).name();

        qDebug()<<"nodeHex Name:"<<nameHex;

        if (nameHex.toLower().endsWith("workproject"))
        {
            WorkProject *wp = (WorkProject*)nodeHex;
            QString fullFileName = wp->getFullA2lFileName().c_str();
            qDebug()<<"delete full FileName:"<<fullFileName;

            Node *nodeParent = wp->getParentNode();
            model->dataRemoved(nodeParent, nodeParent->childNodes.indexOf(wp));

            A2LFILE *nodeA2LFile = wp->a2lFile;
            nodeParent = nodeA2LFile->getParentNode();
            model_2->dataRemoved(nodeParent, nodeParent->childNodes.indexOf(nodeA2LFile));

            projectList->remove(fullFileName);

            delete(wp);

        }

    }
}

void MainWindow::on_actionMeas_triggered()
{
    QString module = "Measurement";
    DialogChooseModule *dlg = new DialogChooseModule(&module);
    dlg->setWindowTitle(module);
    dlg->setList(getAllMeasParamsNames(model_2));
    dlg->show();
}

void MainWindow::on_actionCharacs_triggered()
{
    QString module = "Characteristic";
    DialogChooseModule *dlg = new DialogChooseModule(&module);
    dlg->setWindowTitle(module);
    dlg->setList(getAllCharacParamsNames(model_2));
    dlg->show();
}
