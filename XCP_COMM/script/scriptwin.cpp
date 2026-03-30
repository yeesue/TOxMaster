#include "scriptwin.h"
#include "ui_scriptwin.h"
#include <QDebug>
#include <QSplitter>

ScriptThread::ScriptThread(QObject *parent) :
    QThread(parent)
{
    engine = new QScriptEngine();
}

ScriptThread::~ScriptThread()
{
    delete engine;
}

void ScriptThread::abortEvaluating()
{
    if(engine)
    {
        engine->abortEvaluation();
    }
}

bool ScriptThread::isEvaluating()
{
    if(!engine) return false;

    return engine->isEvaluating();
}

bool ScriptThread::getScriptRunning() const
{
    return scriptRunning;
}

void ScriptThread::setScriptFile(QString file, QString filePath)
{
    this->scriptFile = file;
    this->scriptFilePath = filePath;
}

void ScriptThread::setLog(LogWin *logWin)
{
    Log = logWin;
}

void ScriptThread::run()
{
    //qInfo()<<"create script object.";
    ScriptObj *scriptObj = new ScriptObj(0, engine);
    QScriptValue scriptObject = engine->newQObject(scriptObj);
    engine->globalObject().setProperty("SCRIPT", scriptObject);

    if(Log)
    {
        connect(scriptObj, SIGNAL(logMsg(QString)), Log, SLOT(log(QString)));
        connect(scriptObj, SIGNAL(logClear()), Log, SLOT(logSaveAndClear()));
    }

    packScriptEngineProperty(engine);

    scriptRunning = false;

    if(scriptFile.isEmpty() || scriptFilePath.isEmpty())
    {
        qDebug()<<"Script file is invalid.";
        emit stateChanged(scriptFile, "脚本非法");
        return;
    }

    QFile file(scriptFilePath);
    if (!file.open(QFile::ReadOnly | QFile::Text)) {
        qInfo()<<tr("Cannot open script file %1:\n%2.")
                 .arg(scriptFilePath)
                 .arg(file.errorString());
        emit stateChanged(scriptFile, "脚本打开错误");
        return;
    }

    QString scriptText = QString::fromStdString(file.readAll().toStdString());

    qInfo()<<"Start to execute script in independent thread :"<<scriptFile;
    emit stateChanged(scriptFile, "脚本执行中");
    scriptRunning = true;

    QScriptValue value = engine->evaluate(scriptText);

    scriptRunning = false;

    if (engine->hasUncaughtException()) {
        int line = engine->uncaughtExceptionLineNumber();
        //qWarning() << "uncaught exception at line " << line << ":" << value.toString();
        QString errMsg = "uncaught exception at line " + QString::number(line) + ":" + value.toString();
        qWarning() << errMsg;
        Log->log(errMsg);
        emit stateChanged(scriptFile, "脚本执行错误");
    }

    QString returnStr = scriptFile + " | execute finished! Return value: "+ value.toString();
    qInfo()<<returnStr;
    emit stateChanged(scriptFile, "脚本执行结束");

    this->exec();

}

void ScriptThread::packScriptEngineProperty(QScriptEngine *engine)
{
    if(!engine)
        return;

    //pack Log

    /*pack pams //改为通过ScriptObj中间接进行参数取值和设定值
    foreach (PARAM *pam, Globals::pamList) {
        if(pam)
        {
            setPamToScriptObject(engine, pam);
        }
    }
    */

}

void ScriptThread::setPamToScriptObject(QScriptEngine *engine, PARAM *pam)
{
    if(!pam || !engine)
        return;
    QScriptValue pamObject = engine->newQObject(pam);
    engine->globalObject().setProperty(pam->Name, pamObject);
}


////////////////////////////////////////////////////////////////////////////////////////////////
ScriptWin::ScriptWin(QWidget *parent, QString name) :
    QMainWindow(parent),
    workName(name),
    ui(new Ui::ScriptWin)
{
    ui->setupUi(this);
    setWindowTitle("脚本");

    treeView = new QTreeView(this);
    treeView->setAlternatingRowColors(true);
    treeView->setEditTriggers(QAbstractItemView::NoEditTriggers);

    treeView_UI = new QTreeView(this);
    treeView_UI->setAlternatingRowColors(true);
    treeView_UI->setEditTriggers(QAbstractItemView::NoEditTriggers);

    Log = new LogWin(this);
    Log->setWindowTitle("ScriptLog");
    Log->setWindowFlags(Qt::CustomizeWindowHint | Qt::FramelessWindowHint);

    QPushButton *pb_runScript = new QPushButton(QIcon(":/Icon/Icon/run-solid.png"), "运行", this);
    QPushButton *pb_stopScript = new QPushButton(QIcon(":/Icon/Icon/stop.png"), "停止", this);
    QPushButton *pb_reloadScript = new QPushButton(QIcon(":/Icon/Icon/icons_reload.png"), "重载", this);

    connect(pb_runScript, SIGNAL(released()), this, SLOT(executeSlot()));
    connect(pb_stopScript, SIGNAL(released()), this, SLOT(abortSlot()));
    connect(pb_reloadScript, SIGNAL(released()), this, SLOT(reloadScriptSlot()));

    QPushButton *pb_runUIScript = new QPushButton(QIcon(":/Icon/Icon/run-solid.png"), "运行", this);
    QPushButton *pb_stopUIScript = new QPushButton(QIcon(":/Icon/Icon/stop.png"), "停止", this);
    QPushButton *pb_reloadUIScript = new QPushButton(QIcon(":/Icon/Icon/refresh.png"), "重载", this);

    connect(pb_runUIScript, SIGNAL(released()), this, SLOT(executeSlotInMainThread()));
    connect(pb_stopUIScript, SIGNAL(released()), this, SLOT(abortSlot_UI()));
    connect(pb_reloadUIScript, SIGNAL(released()), this, SLOT(reloadUIScriptSlot()));

    //界面布局
    QSplitter *vSplitter = new QSplitter(Qt::Vertical, this);
    QSplitter *hSplitter = new QSplitter(Qt::Horizontal, vSplitter);
    hSplitter->addWidget(treeView);
    hSplitter->addWidget(treeView_UI);
    hSplitter->setStretchFactor(0, 1);
    hSplitter->setStretchFactor(1, 1);

    QSplitter *hSplitter2 = new QSplitter(Qt::Horizontal, vSplitter);
    hSplitter2->addWidget(pb_runScript);
    hSplitter2->addWidget(pb_stopScript);
    hSplitter2->addWidget(pb_reloadScript);
    hSplitter2->addWidget(pb_runUIScript);
    hSplitter2->addWidget(pb_stopUIScript);
    hSplitter2->addWidget(pb_reloadUIScript);
    hSplitter2->setStretchFactor(0, 1);
    hSplitter2->setStretchFactor(1, 1);
    hSplitter2->setStretchFactor(2, 1);
    hSplitter2->setStretchFactor(3, 1);
    hSplitter2->setStretchFactor(4, 1);
    hSplitter2->setStretchFactor(5, 1);

    vSplitter->addWidget(Log);
    vSplitter->setStretchFactor(0, 5);
    vSplitter->setStretchFactor(1, 1);
    vSplitter->setStretchFactor(2, 6);

    QGridLayout *gLayout = new QGridLayout();
    gLayout->addWidget(vSplitter);
    QWidget *w = new QWidget(this);
    w->setLayout(gLayout);
    this->setCentralWidget(w);

    gLayout->setHorizontalSpacing(0);
    gLayout->setVerticalSpacing(0);
    gLayout->setMargin(0);

    scriptModel = new QStandardItemModel();
    treeView->setModel(scriptModel);

    QStandardItem *header0 = new QStandardItem("脚本名");
    QStandardItem *header1 = new QStandardItem("状态");
    header0->setBackground(Qt::gray);
    header1->setBackground(Qt::gray);
    header0->setTextAlignment(Qt::AlignCenter);
    header1->setTextAlignment(Qt::AlignCenter);
    scriptModel->setHorizontalHeaderItem(0, header0);
    scriptModel->setHorizontalHeaderItem(1, header1);
    treeView->setColumnWidth(0, 300);
    treeView->setColumnWidth(1, 300);

    scriptModel_UI = new QStandardItemModel();
    treeView_UI->setModel(scriptModel_UI);

    QStandardItem *header10 = new QStandardItem("UI 脚本名");
    QStandardItem *header11 = new QStandardItem("状态");
    header10->setBackground(Qt::gray);
    header11->setBackground(Qt::gray);
    header10->setTextAlignment(Qt::AlignCenter);
    header11->setTextAlignment(Qt::AlignCenter);
    scriptModel_UI->setHorizontalHeaderItem(0, header10);
    scriptModel_UI->setHorizontalHeaderItem(1, header11);
    treeView_UI->setColumnWidth(0, 300);
    treeView_UI->setColumnWidth(1, 300);


    getScriptFiles();
    readSetting();

    connect(treeView, SIGNAL(clicked(QModelIndex)), this, SLOT(clickedSlot(QModelIndex)));
    connect(treeView, SIGNAL(doubleClicked(QModelIndex)), this, SLOT(doubleClickedScriptSlot(QModelIndex)));
    treeView->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(treeView, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(showMenu(QPoint)));

    executeAllScript();


    //Main Thread script
    getScriptUIFiles();
    readSettingUI();

    connect(treeView_UI, SIGNAL(clicked(QModelIndex)), this, SLOT(clickedUISlot(QModelIndex)));
    connect(treeView_UI, SIGNAL(doubleClicked(QModelIndex)), this, SLOT(doubleClickedUIScriptSlot(QModelIndex)));
    treeView_UI->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(treeView_UI, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(showMenu_UI(QPoint)));


    QString appPath = QApplication::applicationDirPath();
    notePadPath = appPath + "/notepad++/notepad++.exe";
    notePadProcess = new QProcess(this);
}

ScriptWin::~ScriptWin()
{
    abortAllScript();
    abortAllScriptUI();

    delete scriptModel;
    delete scriptModel_UI;
    delete notePadProcess;

    delete ui;
}

void ScriptWin::runAll()
{
    executeAllScript();

    ui->actionRun->setEnabled(true);
    ui->actionStop->setEnabled(true);
}

void ScriptWin::stopAll()
{
    abortAllScript();

    ui->actionRun->setEnabled(true);
    ui->actionStop->setEnabled(false);
}

void ScriptWin::showMenu(QPoint pos)
{
    QMenu *menu = new QMenu(treeView);

    QAction *actionDelete = new QAction("删除", treeView);
    QAction *actionExecute = new QAction("执行", treeView);
    QAction *actionAbort = new QAction("中止", treeView);
    //QAction *actionExecuteInMain = new QAction("ExecuteInMain", treeView);

    connect(actionDelete, SIGNAL(triggered()), this, SLOT(deleteSlot()));
    connect(actionExecute, SIGNAL(triggered()), this, SLOT(executeSlot()));
    connect(actionAbort, SIGNAL(triggered()), this, SLOT(abortSlot()));
    //connect(actionExecuteInMain, SIGNAL(triggered()), this, SLOT(executeSlotInMainThread()));

    menu->addAction(actionDelete);
    menu->addAction(actionExecute);
    menu->addAction(actionAbort);
    //menu->addAction(actionExecuteInMain);

    menu->move(cursor().pos());
    menu->show();

    int x = pos.x();
    int y = pos.y();
    QModelIndex index = treeView->indexAt(QPoint(x, y));
    selScript = scriptModel->data(index).toString();
    int row = index.row();
    int col = index.column();
    //qDebug()<<"row="<<row<<",col="<<col<<", name="<<selScript;

    selScriptRow = row;
    selScriptCol = col;

}

void ScriptWin::showMenu_UI(QPoint pos)
{
    QMenu *menu = new QMenu(treeView_UI);

    QAction *actionDelete = new QAction("删除", treeView_UI);
    QAction *actionExecute = new QAction("执行", treeView_UI);
    QAction *actionAbort = new QAction("停止", treeView_UI);

    connect(actionDelete, SIGNAL(triggered()), this, SLOT(deleteSlot_UI()));
    connect(actionExecute, SIGNAL(triggered()), this, SLOT(executeSlotInMainThread()));
    connect(actionAbort, SIGNAL(triggered()), this, SLOT(abortSlot_UI()));

    menu->addAction(actionDelete);
    menu->addAction(actionExecute);
    menu->addAction(actionAbort);

    menu->move(cursor().pos());
    menu->show();

    int x = pos.x();
    int y = pos.y();
    QModelIndex index = treeView_UI->indexAt(QPoint(x, y));
    selScript_UI = scriptModel_UI->data(index).toString();
    int row = index.row();
    int col = index.column();

    selScriptRow_UI = row;
    selScriptCol_UI = col;
}

void ScriptWin::deleteSlot()
{
    if(scriptFileHash.keys().indexOf(selScript) == -1)
        return;

    int result = QMessageBox::warning(this,
                         tr("Script delete warning"),
                         tr("Confirm to delete this script? \n "
                            ),
                         QMessageBox::Ok | QMessageBox::Cancel, QMessageBox::NoButton
                         );
    if (result == QMessageBox::Cancel)
    {
        return;
    }
    else
    {
        QString scriptFile = selScript;
        ScriptThread *thread = scriptThreadHash.value(scriptFile, NULL);
        if(thread)
        {
            thread->abortEvaluating();

            if(thread->isRunning())
            {
                thread->quit();
                thread->wait();

                //qInfo()<<"Stop script engine:"<<scriptFile;
            }

            scriptThreadHash.remove(scriptFile);

            delete thread;
            thread = NULL;

            //qInfo()<<"Remove script engine:"<<scriptFile;
        }

        QString filePath = scriptFileHash.value(scriptFile);
        scriptModel->removeRow(selScriptRow);

        scriptFileHash.remove(scriptFile);
        scriptStruct *sStruct = scriptHash.value(scriptFile);
        scriptList.removeAt(scriptList.indexOf(sStruct));
        scriptHash.remove(scriptFile);

        QDir dir(filePath);

        QFileInfo file(filePath);
        if(file.isFile())
        {
            file.dir().remove(filePath);
            //qInfo()<<"delete script file :"<<filePath;
        }
    }

}

void ScriptWin::deleteSlot_UI()
{
    if(scriptFileHash_UI.keys().indexOf(selScript_UI) == -1)
        return;

    int result = QMessageBox::warning(this,
                         tr("Script delete warning"),
                         tr("Confirm to delete this script? \n "
                            ),
                         QMessageBox::Ok | QMessageBox::Cancel, QMessageBox::NoButton
                         );
    if (result == QMessageBox::Cancel)
    {
        return;
    }
    else
    {
        QString scriptFile = selScript_UI;
        QScriptEngine *engine = scriptEngineHash.value(scriptFile, NULL);
        if(engine)
        {
            if(engine->isEvaluating())
            {
                engine->abortEvaluation();

                qInfo()<<"Engine existed, abort evaluation script: "<<scriptFile;
                Log->log("Engine existed, abort evaluation script: " + scriptFile);
            }
            scriptEngineHash.remove(scriptFile);
        }

        QString filePath = scriptFileHash_UI.value(scriptFile);
        scriptModel_UI->removeRow(selScriptRow_UI);

        scriptFileHash_UI.remove(scriptFile);

        scriptStruct *sStruct = scriptHash_UI.value(scriptFile);
        scriptList_UI.removeAt(scriptList_UI.indexOf(sStruct));

        scriptHash_UI.remove(scriptFile);

        QDir dir(filePath);

        QFileInfo file(filePath);
        if(file.isFile())
        {
            file.dir().remove(filePath);
            qInfo()<<"delete ui script file :"<<filePath;
        }

    }
}

void ScriptWin::executeSlot()
{
    //qDebug()<<"selScript:"<<selScript;

    if(scriptFileHash.keys().indexOf(selScript) == -1)
        return;

    QString scriptFilePath = scriptFileHash.value(selScript);

    qScriptExecute(selScript, scriptFilePath);

}

void ScriptWin::executeSlotInMainThread()
{
    if(scriptFileHash_UI.keys().indexOf(selScript_UI) == -1)
        return;

    QString scriptFilePath = scriptFileHash_UI.value(selScript_UI);

    qScriptExecuteInMainThread(selScript_UI, scriptFilePath);

}

void ScriptWin::abortSlot()
{
    if(scriptFileHash.keys().indexOf(selScript) == -1)
        return;

    ScriptThread *thread = scriptThreadHash.value(selScript, NULL);
    if(thread)
    {
        thread->abortEvaluating();
        if(thread->isRunning())
        {
            thread->quit();
            thread->wait();
        }


        delete thread;
        thread = NULL;

        scriptThreadHash.remove(selScript);
        qDebug()<<"Script Thread remove.";
        stateChangedSlot(selScript, "脚本退出");
    }
    else
    {
        stateChangedSlot(selScript, "脚本退出");
    }

}

void ScriptWin::abortSlot_UI()
{
    if(scriptFileHash_UI.keys().indexOf(selScript_UI) == -1)
    {
        qDebug()<<"can't find the selected script_ui file.";
        return;
    }

    QScriptEngine *engine = scriptEngineHash.value(selScript_UI, NULL);
    if(engine)
    {
        qDebug()<<"Engine evaluating state:"<<engine->isEvaluating();
        if(engine->isEvaluating())
        {
            engine->abortEvaluation();

            qInfo()<<"Engine existed, abort evaluation script: "<<selScript_UI;
            Log->log("Engine existed, abort evaluation script: " + selScript_UI);
        }


        scriptEngineHash.remove(selScript_UI);
        //qDebug()<<"script engine removed.";

        delete engine;
        engine = NULL;
        //qDebug()<<"script engine deleted.";

    }

}

void ScriptWin::reloadScriptSlot()
{
    int result = QMessageBox::warning(this,
                         QString::fromLocal8Bit("重载脚本"),
                         QString::fromLocal8Bit("确认重载脚本?\n"
                            "退出前请确认无运行脚本。"),
                         QMessageBox::Ok | QMessageBox::Cancel, QMessageBox::NoButton
                         );
    if (result == QMessageBox::Cancel)
    {

    }
    else
    {
        //停止当前运行脚本
        abortAllScript();

        scriptModel->clear();

        QStandardItem *header0 = new QStandardItem("脚本名");
        QStandardItem *header1 = new QStandardItem("状态");
        header0->setBackground(Qt::gray);
        header1->setBackground(Qt::gray);
        header0->setTextAlignment(Qt::AlignCenter);
        header1->setTextAlignment(Qt::AlignCenter);
        scriptModel->setHorizontalHeaderItem(0, header0);
        scriptModel->setHorizontalHeaderItem(1, header1);
        treeView->setColumnWidth(0, 300);
        treeView->setColumnWidth(1, 300);

        getScriptFiles();
        readSetting();
    }
}

void ScriptWin::reloadUIScriptSlot()
{
    int result = QMessageBox::warning(this,
                         QString::fromLocal8Bit("重载UI脚本"),
                         QString::fromLocal8Bit("确认重载UI脚本?\n"
                            "退出前请确认无UI运行脚本。"),
                         QMessageBox::Ok | QMessageBox::Cancel, QMessageBox::NoButton
                         );
    if (result == QMessageBox::Cancel)
    {

    }
    else
    {
        //停止当前运行UI脚本
        abortAllScriptUI();

        scriptModel_UI->clear();

        QStandardItem *header10 = new QStandardItem("UI 脚本名");
        QStandardItem *header11 = new QStandardItem("状态");
        header10->setBackground(Qt::gray);
        header11->setBackground(Qt::gray);
        header10->setTextAlignment(Qt::AlignCenter);
        header11->setTextAlignment(Qt::AlignCenter);
        scriptModel_UI->setHorizontalHeaderItem(0, header10);
        scriptModel_UI->setHorizontalHeaderItem(1, header11);
        treeView_UI->setColumnWidth(0, 300);
        treeView_UI->setColumnWidth(1, 300);

        getScriptUIFiles();
        readSettingUI();
    }
}

void ScriptWin::qScriptExecuteInMainThread(QString scriptFile, QString scriptFilePath)
{
    if(scriptFile.isEmpty() || scriptFilePath.isEmpty())
    {
        qDebug()<<"Script file is invalid.";
        stateChangedUISlot(scriptFile, "脚本非法");
        return;
    }

    QFile file(scriptFilePath);
    if (!file.open(QFile::ReadOnly | QFile::Text)) {
        qInfo()<<tr("Cannot open script file %1:\n%2.")
                 .arg(scriptFilePath)
                 .arg(file.errorString());
        stateChangedUISlot(scriptFile, "脚本打开错误");
        return;
    }

    QString scriptText = QString::fromStdString(file.readAll().toStdString());

    QScriptEngine *engine = scriptEngineHash.value(scriptFile, NULL);

    if(!engine)
    {
        QScriptEngine *engineUI = new QScriptEngine(this);
        scriptEngineHash.insert(scriptFile, engineUI);

        packScriptEnginePropertyforMainThread(engineUI);


        qInfo()<<"Start to execute script in main thread: "<<scriptFile;
        Log->log("Start to execute script in main thread: " + scriptFile);
        stateChangedUISlot(scriptFile, "脚本执行中");
        QScriptValue value = engineUI->evaluate(scriptText);

        if (engineUI->hasUncaughtException()) {
            int line = engineUI->uncaughtExceptionLineNumber();
            QString errMsg = "uncaught exception at line" + QString::number(line) + ":" + value.toString();
            qWarning() << errMsg;
            Log->log(errMsg);
            stateChangedUISlot(scriptFile, "脚本执行错误");
        }
        else
        {
            QString returnStr = scriptFile + " | execute finished! return value:"+ value.toString();
            Log->log(returnStr);
            stateChangedUISlot(scriptFile, "脚本执行结束");
        }

    }
    else
    {
        if(engine->isEvaluating())
        {
            engine->abortEvaluation();

            qInfo()<<"Engine existed, abort evaluation script: "<<scriptFile;
            Log->log("Engine existed, abort evaluation script: " + scriptFile);
        }
        scriptEngineHash.remove(scriptFile);
        //qDebug()<<"script engine removed";

        delete engine;
        engine = NULL;
        //qDebug()<<"script engine deleted.";

        stateChangedUISlot(scriptFile, "脚本退出[主线程]");

        qScriptExecuteInMainThread(scriptFile, scriptFilePath);

    }
}


void ScriptWin::packScriptEngineProperty(QScriptEngine *engine)
{
    //pack scriptObj
    ScriptObj *scriptObj = new ScriptObj(0, engine);
    QScriptValue scriptObject = engine->newQObject(scriptObj);
    engine->globalObject().setProperty("SCRIPT", scriptObject);
    connect(scriptObj, SIGNAL(logMsg(QString)), Log, SLOT(log(QString)));


    //pack Log
    if(Log)
    {
        QScriptValue logObject = engine->newQObject(Log);
        engine->globalObject().setProperty("LOG", logObject);
    }

}

void ScriptWin::setPamToScriptObject(QScriptEngine *engine, PARAM *pam)
{
    if(!pam || !engine)
        return;

    QScriptValue pamObject = engine->newQObject(pam);

    engine->globalObject().setProperty(pam->Name, pamObject);
}

void ScriptWin::packScriptEnginePropertyforMainThread(QScriptEngine *engine)
{
    //pack scriptObj
    ScriptObj *scriptObj = new ScriptObj(0, engine);
    QScriptValue scriptObject = engine->newQObject(scriptObj);
    engine->globalObject().setProperty("SCRIPT", scriptObject);
    connect(scriptObj, SIGNAL(logMsg(QString)), Log, SLOT(log(QString)));


    //pack Log
    if(Log)
    {
        QScriptValue logObject = engine->newQObject(Log);
        engine->globalObject().setProperty("LOG", logObject);
    }

    //pack INCA_COM
    if(incaObj)
    {
        QScriptValue incaObject = engine->newQObject(incaObj);
        engine->globalObject().setProperty("INCA", incaObject);
    }

}

void ScriptWin::qScriptExecute(QString scriptFile, QString scriptFilePath)
{
    ScriptThread *thread = scriptThreadHash.value(scriptFile, NULL);
    if(thread == NULL)
    {
        thread = new ScriptThread(this);

        connect(thread, SIGNAL(stateChanged(QString, QString)), this, SLOT(stateChangedSlot(QString, QString)));
        stateChangedSlot(scriptFile, "脚本创建");

        thread->setScriptFile(scriptFile, scriptFilePath);
        thread->setLog(Log);
        thread->start();

        scriptThreadHash.insert(scriptFile, thread);
        qDebug()<<"Script thread is created and started." << scriptFile;
    }
    else
    {
        thread->abortEvaluating();
        if(thread->isRunning())
        {
            thread->quit();
            thread->wait();
        }
        //qDebug()<<"thread is running ? "<<thread->isRunning();

        delete thread;
        thread = NULL;

        scriptThreadHash.remove(scriptFile);
        qDebug()<<"Script thread quit and remove." << scriptFile;
        stateChangedSlot(scriptFile, "脚本退出");

        qScriptExecute(scriptFile, scriptFilePath);
    }
}

void ScriptWin::executeAllScript()
{
    foreach (QString scriptFile, scriptFileHash.keys()) {
        selScript = scriptFile;
        scriptStruct *script = scriptHash.value(selScript);
        selScriptRow = script->index;
        if(!script->autoRun)
            continue;

        executeSlot();
    }
}

void ScriptWin::abortAllScript()
{
    foreach (QString scriptFile, scriptThreadHash.keys()) {
        selScript = scriptFile;
        abortSlot();
    }
}

void ScriptWin::executeAllScriptUI()
{
    readSettingUI();

    foreach (QString scriptFile, scriptHash_UI.keys()) {
        selScript_UI = scriptFile;
        if(scriptHash_UI.value(selScript_UI)->autoRun)
        {
            executeSlotInMainThread();
        }
    }
}

void ScriptWin::abortAllScriptUI()
{
    foreach (QString scriptFile, scriptEngineHash.keys()) {
        selScript_UI = scriptFile;
        abortSlot_UI();
    }
}

void ScriptWin::initINCAObj()
{
    if(incaObj == NULL)
    {
        incaObj = new INCACOMM();
    }
}

void ScriptWin::on_actionLog_triggered()
{
    if(!Log)
        return;
    Log->show();
}

void ScriptWin::on_actionLoad_triggered()
{
    QString fileName =
            QFileDialog::getOpenFileName(this, tr("Open JS File"),
                                         QDir::currentPath() + "\\Script",
                                         tr("JS Files (*.js)"));
    if (fileName.isEmpty())
        return;
    qDebug()<<"Script File Name:"<<fileName;

    QFile file(fileName);
    if (!file.open(QFile::ReadOnly | QFile::Text)) {
        QMessageBox::warning(this, tr("js file"),
                             tr("Cannot read file %1:\n%2.")
                             .arg(fileName)
                             .arg(file.errorString()));
        return;
    }

}

void ScriptWin::on_actionRun_triggered()
{
    executeAllScript();

    ui->actionRun->setEnabled(false);
    ui->actionStop->setEnabled(true);

}

void ScriptWin::on_actionStop_triggered()
{
    abortAllScript();

    ui->actionRun->setEnabled(true);
    ui->actionStop->setEnabled(false);
}

void ScriptWin::on_actionNotePad_triggered()
{
    QStringList args;

    QString scriptDir = QApplication::applicationDirPath() + "//work//" + workName + "//script";
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

void ScriptWin::clickedSlot(QModelIndex index)
{
    int row = index.row();
    int col = index.column();

    selScript = scriptModel->item(row, 0)->text();

    selScriptRow = row;
    selScriptCol = col;
}

void ScriptWin::doubleClickedScriptSlot(QModelIndex index)
{

    QString scriptName = scriptModel->itemFromIndex(index)->text();
    QString scriptFilePath = scriptFileHash.value(scriptName);

    if (scriptFilePath.isEmpty())
        return;
    //qDebug()<<"Script File Name:"<<scriptFilePath;

    QStringList args;
    args.append(scriptFilePath);

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

void ScriptWin::stateChangedSlot(QString scriptFile, QString stateStr)
{
    scriptStruct *script = scriptHash.value(scriptFile);
    QStandardItem *statusItem = scriptModel->item(script->index, 1);
    QColor bgColor;
    if(stateStr == "脚本执行中" || stateStr == "脚本执行结束")
    {
        bgColor = Qt::green;
    }
    else if(stateStr == "脚本初始化")
    {
        bgColor = Qt::white;
    }
    else if(stateStr == "脚本执行错误")
    {
        bgColor = Qt::red;
    }
    else
    {
        bgColor = Qt::gray;
    }
    statusItem->setBackground(bgColor);
    statusItem->setText(stateStr);
}


void ScriptWin::clickedUISlot(QModelIndex index)
{
    int row = index.row();
    int col = index.column();

    selScript_UI = scriptModel_UI->item(row, 0)->text();

    selScriptRow_UI = row;
    selScriptCol_UI = col;
}

void ScriptWin::doubleClickedUIScriptSlot(QModelIndex index)
{
    QString scriptName = scriptModel_UI->itemFromIndex(index)->text();
    QString scriptFilePath = scriptFileHash_UI.value(scriptName);

    if (scriptFilePath.isEmpty())
        return;
    //qDebug()<<"UI Script File Name:"<<scriptFilePath;

    QStringList args;
    args.append(scriptFilePath);

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

void ScriptWin::stateChangedUISlot(QString scriptFile, QString stateStr)
{
    if(selScript_UI == scriptFile)
    {
        selScriptRow_UI = scriptHash_UI.value(scriptFile)->index;
        QStandardItem *statusItem = scriptModel_UI->item(selScriptRow_UI, 1);
        QColor bgColor;
        if((stateStr == "脚本执行中" || stateStr == "脚本执行结束" ))
        {
            bgColor = Qt::green;
        }
        else if(stateStr == "脚本初始化")
        {
            bgColor = Qt::white;
        }
        else if(stateStr == "脚本执行错误")
        {
            bgColor = Qt::red;
        }
        else
        {
            bgColor = Qt::gray;
        }
        statusItem->setBackground(bgColor);
        statusItem->setText(stateStr);
    }
}


void ScriptWin::on_actionRefresh_triggered()
{
    scriptModel->clear();
    QStandardItem *header0 = new QStandardItem("Script");
    QStandardItem *header1 = new QStandardItem("Status");
    header0->setBackground(Qt::gray);
    header1->setBackground(Qt::gray);
    header0->setTextAlignment(Qt::AlignCenter);
    header1->setTextAlignment(Qt::AlignCenter);
    scriptModel->setHorizontalHeaderItem(0, header0);
    scriptModel->setHorizontalHeaderItem(1, header1);
    treeView->setColumnWidth(0, 300);
    treeView->setColumnWidth(1, 300);

    getScriptFiles();
    readSetting();

}

void ScriptWin::readSetting()
{
    QString appPath = QApplication::applicationDirPath();
    QString scriptSetFilePath = appPath + "\\work\\" + workName + "\\script\\" + SET_FILENAME;
    //QSettings setting(appPath + "\\setting\\" + SET_FILENAME, QSettings::IniFormat);
    QSettings setting(scriptSetFilePath, QSettings::IniFormat);

    qInfo()<<"read script setting:" + setting.fileName();

    int size = setting.beginReadArray("scriptList");

    for (int i = 0; i < size; i++)
    {
        setting.setArrayIndex(i);
        QString scriptName = setting.value("scriptName").toString();

        if(scriptHash.uniqueKeys().indexOf(scriptName) == -1)
            continue;

        //qDebug()<<scriptName;
        scriptStruct *script = scriptHash.value(scriptName);
        //qDebug()<<scriptName<<",index="<<script->index;

        script->autoRun = (setting.value("autoRun").toString() == "True") ? true : false;
        Qt::CheckState checkState = (script->autoRun == true) ? Qt::Checked : Qt::Unchecked;
        QStandardItem *item = scriptModel->item(script->index, 0);
        item->setCheckState(checkState);
    }
    setting.endArray();
}

void ScriptWin::writeSetting()
{
    saveScriptConfiguration();

    QString appPath = QApplication::applicationDirPath();
    QString scriptSetFilePath = appPath + "\\work\\" + workName + "\\script\\" + SET_FILENAME;
    //QSettings setting(appPath + "\\setting\\" + SET_FILENAME, QSettings::IniFormat);
    QSettings setting(scriptSetFilePath, QSettings::IniFormat);

    setting.clear();

    qInfo()<<"Write script setting:" + setting.fileName();

    setting.beginWriteArray("scriptList");
    QStringList keys = scriptHash.uniqueKeys();
    int scriptCount = keys.count();
    //qDebug()<<"script count:"<<scriptCount;

    for (int i = 0; i < scriptCount; i++)
    {
        setting.setArrayIndex(i);
        QString scriptName = keys.at(i);
        scriptStruct *script = scriptHash.value(scriptName);

        QString scriptFilePath = script->filePath;
        bool autoRun = script->autoRun;

        //qDebug()<<"scriptName:"<<scriptName<<", autorun:"<<autoRun;

        setting.setValue("scriptName", scriptName);
        setting.setValue("scriptFilePath", scriptFilePath);
        setting.setValue("autoRun", autoRun ? "True" : "False");

    }
    setting.endArray();
}

QStringList ScriptWin::getScriptFiles()
{
    scriptFileHash.clear();
    scriptNameList.clear();
    scriptList.clear();
    scriptHash.clear();

    QString appPath = QApplication::applicationDirPath();
    QString scriptFileDir = appPath  + "//work//"+ workName +"//script";
    QDir *dir = new QDir(scriptFileDir);

    statusBar()->showMessage("scritp dir:" + scriptFileDir);

    QStringList nameFilter;
    nameFilter << "*.js";
    QFileInfoList fileInfo = dir->entryInfoList(nameFilter);
    QStringList scriptFileList;
    for (int i = 0; i < fileInfo.count(); i++)
    {
        QString filePath = fileInfo.at(i).filePath();
        QString shortFile = filePath.split('/').last();
        scriptFileHash.insert(shortFile, filePath);

        if(shortFile.isEmpty())
            continue;

        scriptStruct *script = new scriptStruct;
        script->shortName = shortFile;
        script->filePath = filePath;
        script->autoRun = false;
        script->index = scriptModel->rowCount();

        scriptNameList.append(shortFile);
        scriptList.append(script);
        scriptHash.insert(shortFile, script);

        scriptFileList << filePath;
        //qInfo()<<"js file Path:"<<filePath;

        QStandardItem *scriptItem = new QStandardItem(shortFile);
        scriptItem->setIcon(QIcon(":/Icon/Icon/js.png"));
        scriptItem->setCheckable(true);
        QStandardItem *statusItem = new QStandardItem("Not running");
        statusItem->setBackground(Qt::gray);
        statusItem->setTextAlignment(Qt::AlignCenter);

        QList<QStandardItem*> itemList;
        itemList<<scriptItem << statusItem;

        scriptModel->appendRow(itemList);

    }
    return scriptFileList;
}

void ScriptWin::saveScriptConfiguration()
{
    int rowCount = scriptModel->rowCount();
    //qDebug()<<"script count:"<<rowCount;

    for(int i = 0; i < rowCount; i++)
    {
        QStandardItem *nameItem = scriptModel->item(i, 0);

        QString shortName = nameItem->text();
        int checkState = nameItem->checkState();

        bool autoRunSet = (checkState == Qt::Checked) ? true : false;
        scriptStruct *script = scriptHash.value(shortName);
        if(script)
        {
            script->autoRun = autoRunSet;
        }
    }
}

void ScriptWin::readSettingUI()
{
    QString appPath = QApplication::applicationDirPath();
    QString scriptSetFilePath = appPath + "\\work\\" + workName + "\\script_ui\\" + SET_UI_FILENAME;
    QSettings setting(scriptSetFilePath, QSettings::IniFormat);

    //qInfo()<<"read script ui setting:" + setting.fileName();

    int size = setting.beginReadArray("scriptList");

    for (int i = 0; i < size; i++)
    {
        setting.setArrayIndex(i);
        QString scriptName = setting.value("scriptName").toString();

        if(scriptHash_UI.uniqueKeys().indexOf(scriptName) == -1)
            continue;

        //qDebug()<<scriptName;
        scriptStruct *script = scriptHash_UI.value(scriptName);
        //qDebug()<<scriptName<<",index="<<script->index;

        script->autoRun = (setting.value("autoRun").toString() == "True") ? true : false;
        Qt::CheckState checkState = (script->autoRun == true) ? Qt::Checked : Qt::Unchecked;
        QStandardItem *item = scriptModel_UI->item(script->index, 0);
        item->setCheckState(checkState);

        script->winPos_x = setting.value("winPosX").toInt();
        script->winPos_y = setting.value("winPosY").toInt();
        script->winWidth = setting.value("winWidth").toInt();
        script->winHeight = setting.value("winHeight").toInt();
    }
    setting.endArray();

    //qInfo()<<"read script ui setting done.";
}

void ScriptWin::writeSettingUI()
{
    saveScriptUIConfiguration();

    QString appPath = QApplication::applicationDirPath();
    QString scriptSetFilePath = appPath + "\\work\\" + workName + "\\script_ui\\" + SET_UI_FILENAME;
    QSettings setting(scriptSetFilePath, QSettings::IniFormat);

    setting.clear();

    //qInfo()<<"Write script ui setting:" + setting.fileName();

    setting.beginWriteArray("scriptList");
    QStringList keys = scriptHash_UI.uniqueKeys();
    int scriptCount = keys.count();
    //qDebug()<<"script count:"<<scriptCount;

    for (int i = 0; i < scriptCount; i++)
    {
        setting.setArrayIndex(i);
        QString scriptName = keys.at(i);
        scriptStruct *script = scriptHash_UI.value(scriptName);

        QString scriptFilePath = script->filePath;
        bool autoRun = script->autoRun;

        //qDebug()<<"scriptName:"<<scriptName<<", autorun:"<<autoRun;

        setting.setValue("scriptName", scriptName);
        setting.setValue("scriptFilePath", scriptFilePath);
        setting.setValue("autoRun", autoRun ? "True" : "False");
        setting.setValue("winPosX", script->winPos_x);
        setting.setValue("winPosY", script->winPos_y);
        setting.setValue("winWidth", script->winWidth);
        setting.setValue("winHeight", script->winHeight);

    }
    setting.endArray();
}

QStringList ScriptWin::getScriptUIFiles()
{
    QString appPath = QApplication::applicationDirPath();
    QString scriptFileDir = appPath  + "//work//"+ workName +"//script_ui";
    QDir *dir = new QDir(scriptFileDir);

    scriptList_UI.clear();
    scriptHash_UI.clear();
    scriptFileHash_UI.clear();

    QStringList nameFilter;
    nameFilter << "*.js";
    QFileInfoList fileInfo = dir->entryInfoList(nameFilter);
    QStringList scriptFileList;
    for (int i = 0; i < fileInfo.count(); i++)
    {
        QString filePath = fileInfo.at(i).filePath();
        QString shortFile = filePath.split('/').last();
        scriptFileHash_UI.insert(shortFile, filePath);

        if(shortFile.isEmpty())
            continue;

        scriptStruct *script = new scriptStruct;
        script->shortName = shortFile;
        script->filePath = filePath;
        script->autoRun = false;
        script->index = scriptModel_UI->rowCount();

        scriptList_UI.append(script);
        scriptHash_UI.insert(shortFile, script);

        scriptFileList << filePath;
        //qInfo()<<"js file Path:"<<filePath;

        QStandardItem *scriptItem = new QStandardItem(shortFile);
        scriptItem->setIcon(QIcon(":/Icon/Icon/js.png"));
        scriptItem->setCheckable(true);
        QStandardItem *statusItem = new QStandardItem("脚本初始化");
        statusItem->setBackground(Qt::white);
        statusItem->setTextAlignment(Qt::AlignCenter);

        QList<QStandardItem*> itemList;
        itemList<<scriptItem << statusItem;

        scriptModel_UI->appendRow(itemList);

    }
    return scriptFileList;
}

void ScriptWin::saveScriptUIConfiguration()
{
    int rowCount = scriptModel_UI->rowCount();
    //qDebug()<<"script count:"<<rowCount;

    for(int i = 0; i < rowCount; i++)
    {
        QStandardItem *nameItem = scriptModel_UI->item(i, 0);

        QString shortName = nameItem->text();
        int checkState = nameItem->checkState();

        bool autoRunSet = (checkState == Qt::Checked) ? true : false;
        scriptStruct *script = scriptHash_UI.value(shortName);
        if(script)
        {
            script->autoRun = autoRunSet;
        }
    }
}


void ScriptWin::on_actionSave_triggered()
{
    writeSetting();

}

void ScriptWin::setWorkName(const QString &value)
{
    workName = value;
}

void ScriptWin::on_actionINCA_Connect_triggered()
{
    initINCAObj();

    if(!incaObj->connectToINCA())
    {
        qWarning()<<"INCA connect fail";
    }

    qInfo()<<"INCA connected!";
}

void ScriptWin::on_actionINCA_Disconnect_triggered()
{
    if(incaObj)
    {
        incaObj->disconnectFromINCA();

        delete incaObj;
        incaObj = NULL;

        qInfo()<<"INCA disconnected and deleted.";
    }
}
