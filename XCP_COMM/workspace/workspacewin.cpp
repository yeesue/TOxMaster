#include "workspacewin.h"
#include "ui_workspacewin.h"
#include "linedelegate.h"
#include <QDebug>

WorkSpaceWin::WorkSpaceWin(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::WorkSpaceWin)
{
    ui->setupUi(this);
    setWindowTitle("Work Manage");

    logWin = new LogWin(this, false);

    //QDesktopWidget *deskTop =  QApplication::desktop();
    //resize(deskTop->width()/2, deskTop->height()/2);
    showMaximized();

    QStringList headers;
    headers << "Work" << "Status";

    ui->splitter->setStretchFactor(3,8);

    QTreeView *tree = ui->treeView_workspace;

    initWorkTreeStyle();

    model_workspace = new QStandardItemModel();
    tree->setModel(model_workspace);
    tree->setContextMenuPolicy(Qt::CustomContextMenu);
    tree->setEditTriggers(QTreeView::SelectedClicked);
    tree->setSelectionBehavior(QTreeView::SelectRows);
    tree->setSelectionMode(QTreeView::SingleSelection);
    //tree->setAlternatingRowColors(true);
    tree->setFocusPolicy(Qt::NoFocus);
    tree->header()->setStretchLastSection(true);
    tree->header()->setDefaultAlignment(Qt::AlignCenter);

    model_workspace->setHorizontalHeaderLabels(headers);
    tree->header()->resizeSection(0, 360);

    LineDelegate *lineDelegate = new LineDelegate();
    tree->setItemDelegate(lineDelegate);

    connect(tree, &QTreeView::customContextMenuRequested, this, &WorkSpaceWin::Slt_ContextMenuRequest);
    connect(lineDelegate, &LineDelegate::modelDataUpdated, this, &WorkSpaceWin::Slt_ModelDataUpdated);
    connect(tree, &QTreeView::clicked, this, &WorkSpaceWin::Slt_Clicked);
    connect(tree, &QTreeView::doubleClicked, this, &WorkSpaceWin::Slt_DoubleClicked);

    connect(this, &WorkSpaceWin::workNameUpdated, this, &WorkSpaceWin::Slt_WorkNameUpdated);

    readWorkspaces();
    showWorkspaces();

    if(a2lProjectWin == NULL)
    {
        a2lProjectWin = new A2LProjectWin(this);
    }

}

WorkSpaceWin::~WorkSpaceWin()
{
    qDeleteAll(workList);

    delete ui;
}

void WorkSpaceWin::Slt_ContextMenuRequest(const QPoint &pos)
{
    QModelIndex curIndex = ui->treeView_workspace->indexAt(pos);      //当前点击的元素的index

    QMenu menu;
    menu.addAction(QStringLiteral("New"), this, SLOT(Slt_New(bool)));
    menu.addAction(QStringLiteral("Open"), this, SLOT(Slt_Open(bool)));
    menu.addAction(QStringLiteral("Copy"), this, SLOT(Slt_Copy(bool)));
    menu.addAction(QStringLiteral("Del"), this, SLOT(Slt_Del(bool)));
    menu.addAction(QStringLiteral("Hide"), this, SLOT(Slt_Hide(bool)));
    menu.exec(QCursor::pos());
}

void WorkSpaceWin::Slt_ModelDataUpdated(int row, int col, QString text)
{
    if(workList.count() <= row)
        return;
    //qDebug()<<"row:"<< row <<", col:"<<col<<", text:"<<text;

    WorkStruct *work = workList.at(row);

    if(col == 0)
    {
        int exitIndex = findWorkIndexByName(text);

        if(exitIndex >= 0)
        {
            model_workspace->item(row, col)->setText(oldText);
        }
        else
        {
            QString newWorkName = text;

            QString oldWorkDir = QApplication::applicationDirPath() + "/work/" + work->workName;
            QString newWorkDir = QApplication::applicationDirPath() + "/work/" + newWorkName;
            QDir dir;
            if(dir.rename(oldWorkDir, newWorkDir))
            {
                qDebug()<<"=== Rename workDir as:"<<newWorkDir;
            }

            work->workName = newWorkName;

            //qDebug()<<"~~~workName update emit";
            emit workNameUpdated(row);
        }
    }
}

void WorkSpaceWin::Slt_Clicked(QModelIndex index)
{
    oldText = index.data().toString();
}

void WorkSpaceWin::Slt_DoubleClicked(QModelIndex index)
{
    int workIndex = index.row();
    if(workList.count() <= workIndex)
        return;

    WorkStruct *work = workList.at(workIndex);
    if(work->funWin == NULL)
    {
        FunctionWin *funWin = new FunctionWin(nullptr, work->workName);
        funWin->setA2lProjectWin(this->a2lProjectWin);
        work->funWin = funWin;
    }
    else
    {
        work->funWin->setA2lProjectWin(this->a2lProjectWin);
    }

    connect(work->funWin, &FunctionWin::funUpdated, this, &WorkSpaceWin::Slt_funUpdated);
    connect(work->funWin, &FunctionWin::workStateUpdated, this, &WorkSpaceWin::Slt_workStateUpdated);

    //work->funWin->show();

    if(curFunWin)
    {
        curFunWin->hide();
    }
    curFunWin = work->funWin;

    putFunWidget(curFunWin);

}

void WorkSpaceWin::Slt_WorkNameUpdated(int index)
{
    if(workList.count() <= index)
        return;


    //qDebug()<<"work name updated.";
    WorkStruct *work = workList.at(index);

    FunctionWin *funWin = work->funWin;
    if(funWin)
    {
        funWin->setWorkName(work->workName);
    }

    QString winFileName = "win_config-" + work->workName.toLower() +".xml";
    QString winFilePath_ref = "/work/" + work->workName + "/" + winFileName;
    QString winFilePath = QApplication::applicationDirPath() + winFilePath_ref;

    QString winFilePath_last = QApplication::applicationDirPath() + "/work/" + work->workName + "/" +
             work->winConfigXmlPath.split("/").last();

    //qDebug()<<"~~old win file:"<<winFilePath_last;
    //qDebug()<<"~~new win file:"<<winFilePath;

    QFileInfo fileInfo(winFilePath_last);
    if(fileInfo.isFile())
    {
        QFile file(winFilePath_last);
        bool ok = file.rename(winFilePath);
        if(ok)
        {
            //work->winConfigXmlPath = winFilePath;
            work->winConfigXmlPath = winFilePath_ref;
            qDebug()<<"=== Rename work win file as:"<<work->winConfigXmlPath;
        }
    }
}

void WorkSpaceWin::Slt_funUpdated()
{
    saveWorkspaces();
}

void WorkSpaceWin::Slt_workStateUpdated(QString workName, int state)
{
    QList<QStandardItem*> items = model_workspace->findItems(workName);
    if(items.isEmpty()) return;

    int workIndex = items.at(0)->row();
    QColor bgColor = Qt::white;
    QString stateStr = "";

    switch (state) {
    case -1:
        bgColor = Qt::red;
        stateStr = "Error";
        break;
    case 0:
        bgColor = Qt::white;
        stateStr = "";
        break;
    case 1:
        bgColor = Qt::green;
        stateStr = "Active";
        break;
    default:
        break;
    }

    model_workspace->item(workIndex, 1)->setText(stateStr);
    model_workspace->item(workIndex, 1)->setBackground(bgColor);
}

void WorkSpaceWin::Slt_New(bool flag)
{
    int count = workList.count();

    int index = 0;
    QString name = "Work_" + QString::number(index);
    while(findWorkIndexByName(name) >= 0 && index < 1024)
    {
        index++;
        name = "Work_" + QString::number(index);
    }

    WorkStruct *work = createWork(name);

    workList.append(work);

    appendWorkRow(work->workName);

    saveWorkspaces();
}

void WorkSpaceWin::Slt_Copy(bool flag)
{
    QModelIndex index = ui->treeView_workspace->currentIndex();
    int row = index.row();
    WorkStruct *work = workList.at(row);

    WorkStruct *work_copy = copyWork(work);
}

void WorkSpaceWin::Slt_Open(bool flag)
{
    QModelIndex index = ui->treeView_workspace->currentIndex();
    Slt_DoubleClicked(index);
}

void WorkSpaceWin::Slt_Del(bool flag)
{
    QModelIndex curIndex = ui->treeView_workspace->currentIndex();
    int rowIndex = curIndex.row();

    WorkStruct *work = workList.takeAt(rowIndex);

    int ret = QMessageBox::warning(this, tr("Delete Warning"),
                                     tr("Confirm to delete work [") + work->workName + "]?",
                                     QMessageBox::Ok | QMessageBox::Cancel,
                                     QMessageBox::Cancel);
    if(ret == QMessageBox::Cancel)
        return;

    model_workspace->removeRow(rowIndex);
    delWork(work);

    saveWorkspaces();
}

void WorkSpaceWin::Slt_Hide(bool flag)
{
    if(curFunWin) curFunWin->hide();

}

int WorkSpaceWin::findWorkIndexByName(QString name)
{
    if(workList.isEmpty())
        return -1;

    for(int i = 0; i < workList.count(); i++)
    {
        if(workList.at(i)->workName == name)
            return i;
    }
    return -1;

}

void WorkSpaceWin::showWorkspaces()
{
    for(int i = 0; i < workList.count(); i++)
    {
        WorkStruct *work = workList.at(i);
        appendWorkRow(work->workName);
    }

}

void WorkSpaceWin::saveWorkspaces()
{
    QString appPath = QApplication::applicationDirPath();
    //QString setFilePath = appPath + "/workspace/setting.ini";

    QString workSetFilePath = appPath + "/work/setting.ini";

    QSettings setting(workSetFilePath, QSettings::IniFormat);
    setting.clear();

    setting.beginWriteArray("Worklist");
    int workCount = workList.count();
    for (int i = 0; i < workCount; i++)
    {
        setting.setArrayIndex(i);

        WorkStruct *work = workList.at(i);

        setting.setValue("WorkName", work->workName);
        setting.setValue("Active", QString::number(work->activeStatus ? 1 : 0));
        setting.setValue("WinConfigFilePath", work->winConfigXmlPath);

        FunctionWin *funWin = work->funWin;
        if(funWin)
        {
            QList<FunStruct*> funList = funWin->getFunList();

            int funCount = funList.count();
            setting.setValue("FunSize", QString::number(funCount));
            for(int j = 0; j < funCount; j++)
            {
                FunStruct *fun = funList.at(j);
                QString indexStr = QString::number(j);
                setting.setValue("FunName_"+indexStr, fun->funName);
                setting.setValue("IntfName_"+indexStr, fun->intfName);
                setting.setValue("DevType_"+indexStr, fun->devType);
                setting.setValue("FunType_"+indexStr, fun->funType);
                setting.setValue("FunSetPath_"+indexStr, fun->setFilePath);

                //qDebug()<<"=== Write work:"<<work->workName<<", fun:"<<fun->funName<<", set file:"
                       //<<fun->setFilePath;
            }
        }

    }
    setting.endArray();

    qDebug()<<"Write work setting done.";
}

void WorkSpaceWin::readWorkspaces()
{
    QString appPath = QApplication::applicationDirPath();
    QString workSetFilePath = appPath + "/work/setting.ini";

    QSettings setting(workSetFilePath, QSettings::IniFormat);

    qDeleteAll(workList);
    workList.clear();
    int count = setting.beginReadArray("Worklist");
    for (int i = 0; i < count; i++)
    {
        setting.setArrayIndex(i);

        QString name = setting.value("WorkName").toString();
        bool active = (setting.value("Active").toInt() > 0);

        WorkStruct *work = createWork(name);
        work->activeStatus = active;


        QList<FunStruct*> funList;
        int funCount = setting.value("FunSize").toInt();
        for(int j = 0; j < funCount; j++)
        {

            QString indexStr = QString::number(j);
            FunStruct *fun = new FunStruct;
            fun->funName = setting.value("FunName_"+indexStr).toString();
            fun->intfName = setting.value("IntfName_"+indexStr).toString();
            fun->devType = setting.value("DevType_"+indexStr).toString();
            fun->funType= setting.value("FunType_"+indexStr).toString();
            fun->setFilePath = setting.value("FunSetPath_"+indexStr).toString();

            qDebug()<<"=== Read work:"<<work->workName<<"-fun:"<<fun->funName<<"-set file:"
                   <<fun->setFilePath;

            funList.append(fun);
        }

        work->funWin->setFunList(funList);

        workList.append(work);

    }
    setting.endArray();

    qDebug()<<"Read work setting done.";
}

WorkStruct *WorkSpaceWin::createWork(QString workName)
{
    WorkStruct *work = new WorkStruct;
    work->workName = workName;
    work->activeStatus = true;

    QString workDir = QApplication::applicationDirPath() + "/work/" + work->workName;
    QString scriptDir = workDir + "/script";
    QString scriptSetFile = scriptDir + "/script_setting.ini";
    QString recordDir = workDir + "/record";
    QString recordSetFile = recordDir + "/record_setting.ini";
    QString csvSetFile = recordDir + "/csv_setting.ini";

    QString excelPamDir = workDir + "/xcp_pams";

    QString autoFileDir = workDir + "/autoFile";
    QString jsAutoFileDir = workDir + "/autoFile/jsFile";

    QFileInfo fileInfo(workDir);
    if(!fileInfo.isDir())
    {
        QDir dir;
        dir.mkpath(workDir);
        dir.mkpath(scriptDir);
        dir.mkpath(recordDir);
        dir.mkpath(excelPamDir);
        dir.mkpath(autoFileDir);
        dir.mkpath(jsAutoFileDir);

        qDebug()<<"***Create work dir:"<<workDir;


        QFileInfo scriptfileInfo(scriptSetFile);
        if(!scriptfileInfo.isFile())
        {
            QFile file(scriptSetFile);
            file.open(QIODevice::WriteOnly);
            file.close();
        }

        QFileInfo recordfileInfo(recordSetFile);
        if(!recordfileInfo.isFile())
        {
            QFile file(recordSetFile);
            file.open(QIODevice::WriteOnly);
            file.close();
        }

        QFileInfo csvfileInfo(csvSetFile);
        if(!csvfileInfo.isFile())
        {
            QFile file(csvSetFile);
            file.open(QIODevice::WriteOnly);
            file.close();
        }
    }

    QString winFilePath_ref = "/work/" + work->workName + "/win_config-" + work->workName.toLower() + ".xml";
    //work->winConfigXmlPath = workDir + "/win_config-" + work->workName.toLower() + ".xml";
    work->winConfigXmlPath = winFilePath_ref;

    FunctionWin *fWin = new FunctionWin(this, work->workName);
    work->funWin = fWin;

    connect(work->funWin, &FunctionWin::funUpdated, this, &WorkSpaceWin::Slt_funUpdated);
    connect(work->funWin, &FunctionWin::workStateUpdated, this, &WorkSpaceWin::Slt_workStateUpdated);

    return work;
}

WorkStruct *WorkSpaceWin::copyWork(WorkStruct *work)
{
    if(!work)
        return NULL;

    WorkStruct *workCopy = new WorkStruct();

    workCopy->workName = work->workName + "_copy";
    workCopy->activeStatus = work->activeStatus;

    //copy work dir
    QString workCopyDir = QApplication::applicationDirPath() + "/work/" + workCopy->workName;
    QDir dir;
    dir.mkpath(workCopyDir);
    qDebug()<<"=== Copy work dir:"<<workCopyDir;

    QString workDir = QApplication::applicationDirPath() + "/work/" + work->workName;
    copyPath(workDir, workCopyDir);

    // copy win config file
    QString fileName = "win_config-" + workCopy->workName.toLower() + ".xml";
    QString filePath_ref = "/work/" + work->workName + "/win_config-" + workCopy->workName.toLower() + ".xml";

    //workCopy->winConfigXmlPath = workCopyDir + "/win_config-" + workCopy->workName.toLower() + ".xml";
    workCopy->winConfigXmlPath = filePath_ref;
    QString filePath = workCopyDir + "/win_config-" + workCopy->workName.toLower() + ".xml";

    QString winFileName = workCopyDir + "/win_config-" + work->workName.toLower() + ".xml";
    QFile winFile(winFileName);
    winFile.rename(filePath);
    qDebug()<<"=== Rename work win file:"<<workCopy->winConfigXmlPath;

    if(work->funWin)
    {
        FunctionWin *funWinCopy = new FunctionWin(this, workCopy->workName);
        workCopy->funWin = funWinCopy;

        QList<FunStruct*> funListCopy;
        QList<FunStruct*> funList = work->funWin->getFunList();
        int funCount = funList.count();
        foreach (FunStruct *fun, funList) {
            FunStruct *funCopy = new FunStruct;
            funCopy->funName = fun->funName;
            funCopy->intfName = fun->intfName;
            funCopy->devType = fun->devType;
            funCopy->funType = fun->funType;

            // copy fun setting file            
            QString setFile = work->workName.toLower() + "-" + funCopy->funName.toLower() + "-" + "setting.xml";
            QString setFilePath = workCopyDir + "/" + setFile;
            QString setFileCopy = workCopy->workName.toLower() + "-" + funCopy->funName.toLower() + "-" + "setting.xml";
            QString setFilePathCopy = workCopyDir + "/" + setFileCopy;
            if(funCopy->funType == "XCP")
            {
                setFilePath.replace(".xml", ".ini");
                setFilePathCopy.replace(".xml", ".ini");
                setFileCopy.replace(".xml", ".ini");
            }
            QFile file(setFilePath);
            file.rename(setFilePathCopy);

            funCopy->setFilePath = "/work/" +workCopy->workName + "/" + setFileCopy;
            qDebug()<<"=== Rename fun set file:"<<funCopy->setFilePath;

            funListCopy.append(funCopy);
        }

        funWinCopy->setFunList(funListCopy);

    }

    workList.append(workCopy);

    appendWorkRow(workCopy->workName);

    saveWorkspaces();

    return workCopy;

}

void WorkSpaceWin::delWork(WorkStruct *work)
{
    if(!work)
        return;

    QString workDir = QApplication::applicationDirPath() + "/work/" + work->workName;
    QDir dir(workDir);
    dir.removeRecursively();
    qDebug()<<"=== Remove work dir:"<<workDir;

    if(work->funWin)
    {
        QList<FunStruct*> funList = work->funWin->getFunList();
        foreach (FunStruct *fun, funList) {
            if(fun)
            {
                //QFileInfo fileInfo(fun->setFilePath);
                //if(fileInfo.isFile())
                    //QFile::remove(fun->setFilePath);

                delete fun;
            }
        }

        //QString funWinFilePath = work->winConfigXmlPath;
        //QFileInfo fileInfo(funWinFilePath);
        //if(fileInfo.isFile())
            //QFile::remove(funWinFilePath);

        delete work->funWin;
        curFunWin = NULL;
    }

    return;
}

void WorkSpaceWin::copyPath(QString src, QString dst)
{
    QDir dir(src);
    if (!dir.exists())
        return;

    foreach (QString d, dir.entryList(QDir::Dirs | QDir::NoDotAndDotDot)) {
        QString dst_path = dst + QDir::separator() + d;
        dir.mkpath(dst_path);
        copyPath(src+ QDir::separator() + d, dst_path);//use recursion
    }

    foreach (QString f, dir.entryList(QDir::Files)) {
        QFile::copy(src + QDir::separator() + f, dst + QDir::separator() + f);
    }

}

void WorkSpaceWin::appendWorkRow(QString workName)
{
    QStandardItem *workItem = new QStandardItem(QIcon(":/icon/icon/work.png"), workName);
    QStandardItem *stateItem = new QStandardItem("");
    stateItem->setTextAlignment(Qt::AlignCenter);
    QList<QStandardItem*> itemList;
    itemList<<workItem<<stateItem;
    model_workspace->appendRow(itemList);

    ui->treeView_workspace->resizeColumnToContents(0);
}

void WorkSpaceWin::putFunWidget(FunctionWin *funWin)
{
    if(!funWin) return;
    ui->hb_layout->addWidget(funWin);
    funWin->show();
}

void WorkSpaceWin::setA2lProjectWin(A2LProjectWin *value)
{
    a2lProjectWin = value;
}

void WorkSpaceWin::closeEvent(QCloseEvent *event)
{
    QMessageBox::StandardButton result = QMessageBox::question(this, "确认", "确定完全退出[TCM]吗？",
                          QMessageBox::Yes | QMessageBox::No | QMessageBox::Cancel,
                          QMessageBox::No);

    if (result==QMessageBox::Yes)
        event->accept();
    else
        event->ignore();
}

void WorkSpaceWin::on_actionSave_triggered()
{
    saveWorkspaces();
}

void WorkSpaceWin::on_actionA2L_triggered()
{
    if(a2lProjectWin)
    {
        a2lProjectWin->show();
    }
}

void WorkSpaceWin::on_actionLog_triggered()
{
    if(logWin) logWin->show();
}

void WorkSpaceWin::on_actionXcpTester_triggered()
{
    MainWindow *xcpTester = new MainWindow(this);
    xcpTester->show();
}

void WorkSpaceWin::on_actionWT5000_triggered()
{
    if(!win_wt5000)
    {
        win_wt5000 = new WIN_WT5000();
    }

    win_wt5000->show();
}

void WorkSpaceWin::on_actionWT3000_triggered()
{
    if(!win_wt3000)
    {
        win_wt3000 = new WIN_WT3000();
    }

    win_wt3000->show();
}

void WorkSpaceWin::on_actionAutoExe_triggered()
{
    if(!autoExeWin)
    {
        autoExeWin = new AutoExeWin(this);
    }

    autoExeWin->show();
}

void WorkSpaceWin::initWorkTreeStyle()
{
    // 树视图样式已移至全局样式表 res/style.qss
}
