#include "autoexewin.h"
#include "ui_autoexewin.h"
#include <QDebug>
#include "globals.h"

AutoExeWin::AutoExeWin(QWidget *parent, QString name) :
    QMainWindow(parent),
    workName(name),
    ui(new Ui::AutoExeWin)
{
    ui->setupUi(this);
    setWindowTitle("AutoExe");

    Log = new LogWin(this, false);
    Log->setWindowTitle("ScriptLog");
    Log->setWindowFlags(Qt::CustomizeWindowHint | Qt::FramelessWindowHint);

    ui->mdiArea_Log->addSubWindow(Log);
    Log->showMaximized();

    if(!autoPam)
    {
        //autoPam = new AUTO_PAM(this);
    }

    if(!sysPam)
    {
        //sysPam = new SYS_PAM(this);
    }

    if(!csvObj)
    {
        csvObj = new CSVObject(this);
        csvObj->setWorkName(this->workName);
    }

    cmdModel = new QStandardItemModel();

    jsModel = new QStandardItemModel();

    autoModel = new QStandardItemModel();
    ui->cb_fileList->setModel(autoModel);

    getCmdScriptFiles();

    getJsScriptFiles();

    getAutoFiles();

    initTable();

    ui->fileTable->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(ui->fileTable, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(showMenuInFTable(QPoint)));

    //connect(autoPam, SIGNAL(curStepChanged()), this, SLOT(curStepChangedSlot()));
    //connect(autoPam, SIGNAL(autoRunChanged()), this, SLOT(autoRunChangedSlot()));

    //ui->lightButton->setGray();

    runTime.start();

    connect(&runTimer, SIGNAL(timeout()), this, SLOT(timeoutHandle()));


    if(csvSetDlg == NULL)
    {
        csvSetDlg = new CsvSettingDlg(nullptr, workName);
    }
}

AutoExeWin::~AutoExeWin()
{
    delete Log;
    delete cmdModel;
    delete ui;
}

void AutoExeWin::getCmdScriptFiles()
{
    QString curPath = QApplication::applicationDirPath();
    QString cmdFilePath = curPath + "/Auto";
    QDir *dir = new QDir(cmdFilePath);

    QStringList nameFilter;
    nameFilter << "*.js";
    QFileInfoList fileInfo = dir->entryInfoList(nameFilter);

    for (int i = 0; i < fileInfo.count(); i++)
    {
        QString filePath = fileInfo.at(i).filePath();
        QString shortFile = filePath.split('/').last();
        QString cmdName = shortFile;
        cmdName.replace(".js", "");
        cmdFilePathHash.insert(cmdName, filePath);

        if(cmdName.isEmpty())
            continue;

        QStandardItem *cmdItem = new QStandardItem(cmdName);
        cmdItem->setIcon(QIcon(":/icon/icon/fun.png"));
        cmdItem->setCheckable(true);

        QList<QStandardItem*> itemList;
        itemList<<cmdItem;

        cmdModel->appendRow(itemList);
    }

    ui->statusbar->setToolTip(tr("Load cmd scripts completed!"));
}

void AutoExeWin::getAutoFiles()
{
    QString appPath = QApplication::applicationDirPath();
    QString autoFilePath = appPath + "//work//" + workName + "/AutoFile";
    QDir *dir = new QDir(autoFilePath);

    QStringList nameFilter;
    nameFilter << "*.csv";
    QFileInfoList fileInfo = dir->entryInfoList(nameFilter);
    //QStringList autoFileList;
    for (int i = 0; i < fileInfo.count(); i++)
    {
        QString filePath = fileInfo.at(i).filePath();
        QString shortFile = filePath.split('/').last();
        QString autoName = shortFile;
        autoName.replace(".csv", "");
        autoFilePathHash.insert(autoName, filePath);

        if(autoName.isEmpty())
            continue;

        //autoFileList << filePath;


        QStandardItem *autoItem = new QStandardItem(autoName);
        autoItem->setIcon(QIcon(":/icon/icon/auto.png"));
        autoItem->setCheckable(false);

        QList<QStandardItem*> itemList;
        itemList<<autoItem;

        autoModel->appendRow(itemList);

    }
}

void AutoExeWin::getJsScriptFiles()
{
    QString appPath = QApplication::applicationDirPath();
    QString jsFilePath = appPath + "//work//" + workName + "//AutoFile//jsFile";
    QDir *dir = new QDir(jsFilePath);

    QStringList nameFilter;
    nameFilter << "*.js";
    QFileInfoList fileInfo = dir->entryInfoList(nameFilter);

    for (int i = 0; i < fileInfo.count(); i++)
    {
        QString filePath = fileInfo.at(i).filePath();
        QString shortFile = filePath.split('/').last();
        QString jsName = shortFile;
        jsName.replace(".js", "");
        jsFilePathHash.insert(jsName, filePath);

        if(jsName.isEmpty())
            continue;

        QStandardItem *jsItem = new QStandardItem(jsName);
        jsItem->setIcon(QIcon(":/icon/icon/fun.png"));
        jsItem->setCheckable(true);

        QList<QStandardItem*> itemList;
        itemList<<jsItem;

        jsModel->appendRow(itemList);
    }

    ui->statusbar->setToolTip(tr("Load jsScript completed!"));
}

void AutoExeWin::openAutoFilePath(QString autoFilePath)
{
    if(autoFilePath.isEmpty())
        return;
    QString openFileName = autoFilePath.split("/").last();
    qDebug()<<"open file name:"<<openFileName;

    initTable();

    curFilePath = autoFilePath;

    QFile *csvAutoFile = new QFile(autoFilePath);
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
        curFileModel->appendRow(itemList);
    }

    ui->le_curFile->setText(openFileName);

    csvAutoFile->close();

    delete csvAutoFile;

    //ui->fileTable->resizeRowsToContents();
    ui->fileTable->resizeColumnToContents(0);
    //ui->fileTable->resizeColumnToContents(1);
    //ui->fileTable->resizeColumnToContents(2);
    ui->fileTable->resizeColumnToContents(11);
}

void AutoExeWin::openSubAutoFilePath(QString autoFilePath)
{
    if(autoFilePath.isEmpty())
        return;
    QString openFileName = autoFilePath.split("/").last();
    qDebug()<<"open sub file name:"<<openFileName;

    initSubTable();

    curFilePath = autoFilePath;

    QFile *csvAutoFile = new QFile(autoFilePath);
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
        curFileModel->appendRow(itemList);
    }

    ui->le_curFile->setText(openFileName);

    csvAutoFile->close();

    delete csvAutoFile;

    ui->fileTable->resizeColumnToContents(0);
    ui->fileTable->resizeColumnToContents(11);
}

void AutoExeWin::initTable()
{
    if(curFileModel)
    {
        delete curFileModel;
        curFileModel = NULL;
    }
    QStandardItemModel *autoFileModel = new QStandardItemModel();
    autoFileModel->setColumnCount(12);
    ui->fileTable->setModel(autoFileModel);
    ui->fileTable->setShowGrid(true);

    QStringList headers;
    headers<<tr("命令")<<tr("参数0")<<tr("参数1")<<tr("参数2")<<tr("参数3")<<tr("参数4")<<tr("参数5")<<tr("参数6")<<tr("参数7")<<tr("参数8")<<tr("参数9")<<tr("说明");
    autoFileModel->setHorizontalHeaderLabels(headers);

    //QStringList headers;
    //headers<<tr("命令")<<tr("模块名")<<tr("参数名")<<tr("设定值")<<tr("比较/计算类型")<<tr("比较值1")<<tr("比较值2")<<tr("目标步骤")<<tr("索引_X")<<tr("索引_Y")<<tr("参数9");
    //autoFileModel->setHorizontalHeaderLabels(headers);

    curFileModel = autoFileModel;
}

void AutoExeWin::initSubTable()
{

    QStandardItemModel *autoFileModel = new QStandardItemModel();
    autoFileModel->setColumnCount(12);
    ui->fileTable->setModel(autoFileModel);
    ui->fileTable->setShowGrid(true);

    QStringList headers;
    headers<<tr("命令")<<tr("参数0")<<tr("参数1")<<tr("参数2")<<tr("参数3")<<tr("参数4")<<tr("参数5")<<tr("参数6")<<tr("参数7")<<tr("参数8")<<tr("参数9")<<tr("说明");
    autoFileModel->setHorizontalHeaderLabels(headers);

    curFileModel = autoFileModel;
}

void AutoExeWin::autoFileRun()
{
    if(!curFileModel || !autoPam)
    {
        return;
    }

    int cmdRowCount = curFileModel->rowCount();
    qDebug()<<"cmd row count:"<<cmdRowCount;
    if(cmdRowCount == 0)
        return;

    int setStart = ui->sb_setStart->value();
    setStart = (setStart < 1 ? 1 : setStart);
    setStart = (setStart > cmdRowCount ? cmdRowCount : setStart);

    int setEnd = ui->sb_setEnd->value();
    setEnd = (setEnd <= 1 ? cmdRowCount : setEnd);
    setEnd = (setEnd > cmdRowCount ? cmdRowCount : setEnd);

    if(setEnd < setStart)
        return;

    autoPam->setStartStep(setStart);
    autoPam->setCurStep(autoPam->startStep());
    autoPam->setEndStep(setEnd);
    autoPam->setAutoRun(1);
    runTime.restart();
    runTimer.start(100);

    autoTestRun();

    /*
    ui->xProgressBar->setValue(autoPam->startStep());
    ui->xProgressBar->setMaximum(autoPam->endStep());

    while(autoPam->curStep() <= autoPam->endStep() && autoPam->autoRun() > 0)
    {

        if(autoPam->curStep() < 1 || autoPam->curStep() > autoPam->endStep())
            return;

        quint32 curStep = autoPam->curStep();

        autoPam->setNextStep(autoPam->curStep() + 1);

        QModelIndex funIndex = curFileModel->index(curStep-1, 0);

        QStandardItem *funItem = curFileModel->itemFromIndex(funIndex);
        QString funName = funItem->text();

        qDebug()<<"current step:"<<curStep<<", fun:"<<funName;
        funItem->setBackground(QBrush(QColor("#55FF55")));

        if(funName == "CALL")
        {
            QString progPath = curFileModel->itemFromIndex(curFileModel->index(curStep - 1, 1))->text();
            progCall(progPath);
        }

        if(sFilePathHash.keys().indexOf(funName) == -1)
        {
            qDebug()<<"function is not exist.";
            continue;
        }
        QString funFilePath = sFilePathHash.value(funName);

        int argCount = curFileModel->columnCount();
        //qDebug()<<"arg count:"<<argCount;

        QScriptValueList args;
        for(int j = 1; j < argCount; j++)
        {
            QStandardItem *stepItem = curFileModel->itemFromIndex(curFileModel->index(curStep-1, j));

            QString argStr =stepItem->text();
            if(argStr.isEmpty())
                continue;

            bool ok;
            double argNum = argStr.toDouble(&ok);
            if(ok)
            {
                args.append(argNum);
            }
            else
            {
                args.append(argStr);
            }

        }


        if(!scriptFunExecuteWithArgs(funName, funFilePath, args))
        {
            qDebug()<<"Script execute false";
        }

        funItem->setBackground(QBrush(QColor("#FFFFFF")));

        autoPam->setCurStep(autoPam->nextStep());
    }
    */

    autoPam->setAutoRun(0);
    runTimer.stop();
}

void AutoExeWin::timeoutHandle()
{
    int elapsedTime_ms = runTime.elapsed();

    int hour = elapsedTime_ms/1000/3600;
    int min = elapsedTime_ms/1000%3600/60;
    int sec = elapsedTime_ms/1000%3600%60;
    int msec = elapsedTime_ms%1000;
    ui->timeEdit->setTime(QTime(hour, min, sec, msec));
}

/*
void AutoExeWin::autoTestRun_V2()
{
    //ui->xProgressBar->setValue(autoPam->startStep());
    //ui->xProgressBar->setMaximum(autoPam->endStep());

    ui->sb_Level->setValue(autoStack.count() + 1);

    while(autoPam->curStep() <= autoPam->endStep() && autoPam->autoRun() > 0)
    {

        if(autoPam->curStep() < 1 || autoPam->curStep() > autoPam->endStep())
            return;

        quint32 curStep = autoPam->curStep();

        autoPam->setNextStep(autoPam->curStep() + 1);

        QModelIndex funIndex = curFileModel->index(curStep-1, 0);

        QStandardItem *funItem = curFileModel->itemFromIndex(funIndex);
        QString funName = funItem->text();

        qDebug()<<"current step:"<<curStep<<", fun:"<<funName;
        funItem->setBackground(QBrush(QColor("#55FF55")));

        //调用子程序方法:递归 及状态入栈和现场恢复
        if(funName == "CALL")
        {
            QString progPath = curFileModel->itemFromIndex(curFileModel->index(curStep - 1, 1))->text();
            progCall(progPath);

            autoTestRun();

            //下一级递归返回后现场恢复
            if(!autoStack.isEmpty())
            {
                if(curFileModel)
                {
                    delete curFileModel;
                    curFileModel = NULL;
                }

                AutoINFO autoInfo = autoStack.last();

                curFilePath = autoInfo.autoFilePath;
                curFileModel = autoInfo.fileModel;
                autoPam->setStartStep(autoInfo.startStep);
                autoPam->setEndStep(autoInfo.endStep);
                autoPam->setCurStep(autoInfo.curStep);
                autoPam->setNextStep(autoInfo.nextStep);

                ui->fileTable->setModel(curFileModel);

                autoStack.removeLast();
            }

            ui->sb_Level->setValue(autoStack.count() + 1);

        }

        if(sFilePathHash.keys().indexOf(funName) == -1)
        {
            qDebug()<<"Function is not exist, Force stop!";
            autoPam->setAutoRun(0);

            continue;
        }
        QString funFilePath = sFilePathHash.value(funName);

        int argCount = curFileModel->columnCount();
        //qDebug()<<"arg count:"<<argCount;

        QStringList args;
        for(int j = 1; j < argCount; j++)
        {
            QStandardItem *stepItem = curFileModel->itemFromIndex(curFileModel->index(curStep-1, j));

            QString argStr =stepItem->text();

            args.append(argStr);
        }

        if(!cmdExecuteWithArgs(funName, args))
        {
            qDebug()<<"cmd execute false";
        }

        funItem->setBackground(QBrush(QColor("#FFFFFF")));

        autoPam->setCurStep(autoPam->nextStep());
    }

}

bool AutoExeWin::cmdExecuteWithArgs(QString cmd, QStringList args)
{
    if(cmd == "CALL")
    {

    }
    else if(cmd == "PAM_SET")
    {
        if(args.count() != 3)
            return false;

        bool ok = false;
        double value = args[2].toDouble(&ok);
        if(!ok)
        {
            return false;
        }

        return setPamValue(args[0], args[1], value);

    }
    else if(cmd == "PAM_SET_CURVE")
    {
        if(args.count() != 5)
            return false;

        bool ok = false;
        int index_x = args[2].toInt(&ok);
        if(!ok) return false;

        int index_y = args[3].toInt(&ok);
        if(!ok) return false;

        double value = args[4].toDouble(&ok);
        if(!ok) return false;

        return setMapPamValue(args[0], args[1], index_x, index_y, value);
    }
    else if(cmd == "PAM_SET_MAP")
    {
        if(args.count() != 5)
            return false;

        bool ok = false;
        int index_x = args[2].toInt(&ok);
        if(!ok) return false;

        int index_y = args[3].toInt(&ok);
        if(!ok) return false;

        double value = args[4].toDouble(&ok);
        if(!ok) return false;

        return setMapPamValue(args[0], args[1], index_x, index_y, value);

    }
    else if(cmd == "PAM_GET")
    {
        if(args.count() != 2)
            return false;

        double value = getPamValue(args[0], args[1]);

        log("PAM_GET:" + args[1] + "." + args[0] + "=" + value);

        return true;
    }
    else if(cmd == "CONDITION_T_GOTO")
    {
        return cmdCondition(cmd, args, 0);
    }
    else if(cmd == "CONDITION_F_GOTO")
    {
        return cmdCondition(cmd, args, 1);
    }
    else if(cmd == "CONDITION_T_HOLD")
    {
        return cmdCondition(cmd, args, 2);
    }
    else if(cmd == "CONDITION_F_HOLD")
    {
        return cmdCondition(cmd, args, 3);
    }
    else if(cmd == "WAIT")
    {
        if(args.count() != 1)
            return false;

        bool ok = false;
        int waitTime_ms = args[0].toInt(&ok);
        if(!ok) return false;

        wait(waitTime_ms);

    }
    else if(cmd == "DYNO")
    {

    }
    else
    {
        qDebug()<<"unknown cmd";
        return false;
    }

    return true;
}
*/

void AutoExeWin::autoTestRun()
{
    //ui->xProgressBar->setValue(autoPam->startStep());
    //ui->xProgressBar->setMaximum(autoPam->endStep());

    ui->sb_Level->setValue(autoStack.count() + 1);
    autoPam->setAutoProgLevel(autoStack.count() + 1);

    while(autoPam->curStep() <= autoPam->endStep() && autoPam->autoRun() > 0)
    {

        if(autoPam->curStep() < 1 || autoPam->curStep() > autoPam->endStep())
            return;

        quint32 curStep = autoPam->curStep();

        autoPam->setNextStep(autoPam->curStep() + 1);

        QModelIndex cmdIndex = curFileModel->index(curStep-1, 0);

        QStandardItem *cmdItem = curFileModel->itemFromIndex(cmdIndex);
        QString cmdName = cmdItem->text();

        qDebug()<<"current step:"<<curStep<<", cmd:"<<cmdName;
        cmdItem->setBackground(QBrush(QColor("#55FF55")));

        //调用子程序方法:递归 及状态入栈和现场恢复
        if(cmdName == "CALL")
        {
            QString progName = curFileModel->itemFromIndex(curFileModel->index(curStep - 1, 1))->text();
            progCall(progName);

            autoTestRun();

            //下一级递归返回后现场恢复
            if(!autoStack.isEmpty())
            {
                if(curFileModel)
                {
                    delete curFileModel;
                    curFileModel = NULL;
                }

                AutoINFO autoInfo = autoStack.last();

                curFilePath = autoInfo.autoFilePath;
                curFileModel = autoInfo.fileModel;
                autoPam->setStartStep(autoInfo.startStep);
                autoPam->setEndStep(autoInfo.endStep);
                autoPam->setCurStep(autoInfo.curStep);
                autoPam->setNextStep(autoInfo.nextStep);

                ui->fileTable->setModel(curFileModel);

                autoStack.removeLast();
            }

            ui->sb_Level->setValue(autoStack.count() + 1);
            autoPam->setAutoProgLevel(autoStack.count() + 1);

        }

        //调用Javascript脚本方法
        if(cmdName == "CALL_JS")
        {
            QString jsFileName = curFileModel->itemFromIndex(curFileModel->index(curStep - 1, 1))->text();
            QString jsFilePath = jsFilePathHash.value(jsFileName);

            scriptExecute(jsFileName, jsFilePath);
        }

        if(cmdFilePathHash.keys().indexOf(cmdName) == -1)
        {
            qDebug()<<"Cmd is not exist, Force stop!";
            autoPam->setAutoRun(0);

            continue;
        }
        QString funFilePath = cmdFilePathHash.value(cmdName);

        int argCount = curFileModel->columnCount();
        qDebug()<<"cmd:"<<cmdName<<",argCount:"<<argCount;

        QScriptValueList args;
        for(int j = 1; j < argCount; j++)
        {
            QStandardItem *stepItem = curFileModel->itemFromIndex(curFileModel->index(curStep-1, j));

            QString argStr = stepItem->text();

            //碰到空字符串的参数是否跳过
            //if(argStr.isEmpty())
            //    continue;

            //if(cmdName == "MULTI_PAM_SET_01")
            //{
            //    qDebug()<<"j:"<<j<<", arg:"<<argStr;
            //}

            bool ok;
            double argNum = argStr.toDouble(&ok);
            if(ok)
            {
                args.append(argNum);
            }
            else
            {
                args.append(argStr);
            }

        }

        if(!scriptFunExecuteWithArgs(cmdName, funFilePath, args))
        {
            qDebug()<<"Script execute false";
        }

        cmdItem->setBackground(QBrush(QColor("#FFFFFF")));

        autoPam->setCurStep(autoPam->nextStep());
    }

    if(autoPam->autoProgLevel() == 1)
    {
        quint32 curStep = autoPam->curStep();
        if(curStep == autoPam->endStep())
        {
            QModelIndex cmdIndex = curFileModel->index(curStep-1, 0);
            QStandardItem *cmdItem = curFileModel->itemFromIndex(cmdIndex);
            cmdItem->setBackground(QBrush(QColor("#FF5555")));
        }
    }

}

void AutoExeWin::progCall(QString progName)
{
    if(!curFileModel || progName.isEmpty())
        return;
    QString progPath = autoFilePathHash.value(progName);
    if(progPath.isEmpty())
        return;

    //上一级状态入栈
    AutoINFO autoInfo;
    autoInfo.autoFilePath = curFilePath;
    autoInfo.fileModel = curFileModel;
    autoInfo.curStep = autoPam->curStep();
    autoInfo.nextStep = autoPam->nextStep();
    autoInfo.startStep = autoPam->startStep();
    autoInfo.endStep = autoPam->endStep();

    autoStack.append(autoInfo);

    openSubAutoFilePath(progPath);


    if(!curFileModel || curFilePath.isEmpty())
        return;
    int cmdRowCount = curFileModel->rowCount();
    if(cmdRowCount == 0)
        return;

    autoPam->setStartStep(1);
    autoPam->setCurStep(autoPam->startStep());
    autoPam->setEndStep(cmdRowCount);

}

void AutoExeWin::packObjects(QScriptEngine *engine)
{
    if(!engine)
        return;

    //pack scriptObj
    ScriptObj *scriptObj = new ScriptObj(0, engine);
    QScriptValue scriptObject = engine->newQObject(scriptObj);
    engine->globalObject().setProperty("SCRIPT", scriptObject);

    if(Log)
        connect(scriptObj, SIGNAL(logMsg(QString)), Log, SLOT(log(QString)));

    if(autoPam)
    {
        //pack autoObj
        QScriptValue autoObject = engine->newQObject(autoPam);
        engine->globalObject().setProperty("AUTO", autoObject);
    }

    if(sysPam)
    {
        //pack sysObj
        QScriptValue sysObject = engine->newQObject(sysPam);
        engine->globalObject().setProperty("SYS", sysObject);
    }

    if(csvObj)
    {
        //pack csvObj
        QScriptValue csvObject = engine->newQObject(csvObj);
        engine->globalObject().setProperty("CSV", csvObject);
    }
}

bool AutoExeWin::scriptFunExecuteWithArgs(QString scriptFile, QString scriptFilePath, QScriptValueList args)
{
    if(scriptFile.isEmpty() || scriptFilePath.isEmpty())
    {
        qDebug()<<"Script file is invalid.";
        return false;
    }

    QFile file(scriptFilePath);
    if (!file.open(QFile::ReadOnly | QFile::Text)) {
        qInfo()<<tr("Cannot open script file %1:\n%2.")
                 .arg(scriptFilePath)
                 .arg(file.errorString());
        return false;
    }

    QString scriptText = QString::fromStdString(file.readAll().toStdString());

    QScriptEngine *engine = new QScriptEngine();
    packObjects(engine);


    qInfo()<<"Start execute script in auto : "<<scriptFile;

    QScriptValue fun = engine->evaluate(scriptText);

    QScriptValue returnValue = fun.call(QScriptValue(), args);

    QString returnStr = scriptFile + "| after execute return :"+ returnValue.toString();
    qInfo()<<returnStr;

    engine->abortEvaluation();
    delete engine;

    return true;
}

bool AutoExeWin::scriptExecute(QString scriptFile, QString scriptFilePath)
{
    if(scriptFile.isEmpty() || scriptFilePath.isEmpty())
    {
        qDebug()<<"Script file is invalid.";
        return false;
    }

    QFile file(scriptFilePath);
    if (!file.open(QFile::ReadOnly | QFile::Text)) {
        qInfo()<<tr("Cannot open js script file %1:\n%2.")
                 .arg(scriptFilePath)
                 .arg(file.errorString());
        return false;
    }

    QString jsScriptText = QString::fromStdString(file.readAll().toStdString());

    QScriptEngine *engine = new QScriptEngine();
    packObjects(engine);


    qInfo()<<"执行 js script in jsFile : "<<scriptFile;
    curScriptEngine = engine;

    QScriptValue value = engine->evaluate(jsScriptText);

    if(engine->hasUncaughtException()) {
        int line = engine->uncaughtExceptionLineNumber();
        QString errMsg = "脚本执行错误—uncaught exception at line " + QString::number(line) + ":" + value.toString();
        qWarning() << errMsg;
        Log->log(errMsg);
        return false;
    }

    QString returnStr = scriptFile + " | 脚本执行结束! Return value: "+ value.toString();
    qInfo()<<returnStr;

    engine->abortEvaluation();
    delete engine;

    return true;
}

void AutoExeWin::showMenuInFTable(QPoint pos)
{
    QMenu *menu = new QMenu(ui->fileTable);

    QAction *actionSetActive = new QAction("Set Active Step", ui->fileTable);

    connect(actionSetActive, SIGNAL(triggered()), this, SLOT(setActiveSlot()));

    menu->addAction(actionSetActive);

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

void AutoExeWin::setActiveSlot()
{
    if(autoPam->autoRun())
    {
        return;
    }
    else
    {
        int curStep = ui->fileTable->currentIndex().row();
        ui->sb_setStart->setValue(curStep + 1);
    }
}

void AutoExeWin::curStepChangedSlot()
{
    ui->spinBox->setValue(autoPam->curStep());
    //ui->xProgressBar->setValue(autoPam->curStep());
}

void AutoExeWin::autoRunChangedSlot()
{
    if(autoPam->autoRun() > 0)
    {
        //ui->lightButton->setGreen();
    }
    else
    {
        //ui->lightButton->setGray();
    }
}

void AutoExeWin::on_actionLoad_triggered()
{
    QString baseDir = QApplication::applicationDirPath() + "/AutoFile";
    QString openFilePath = QFileDialog::getOpenFileName(this,
                                 tr("Open auto file..."),
                                 baseDir,
                                 tr("CSV files (*.csv)"));

    openAutoFilePath(openFilePath);
}

void AutoExeWin::on_actionLog_triggered()
{
    if(Log == NULL)
    {
        Log = new LogWin(this, false);
        return;
    }
    Log->show();
}

void AutoExeWin::on_actionTestRun_triggered()
{
    ui->actionTestRun->setEnabled(false);
    ui->sb_setStart->setEnabled(false);
    ui->sb_setEnd->setEnabled(false);

    ui->pb_Load->setEnabled(false);

    autoFileRun();

    ui->actionTestRun->setEnabled(true);
    ui->sb_setStart->setEnabled(true);
    ui->sb_setEnd->setEnabled(true);

    ui->pb_Load->setEnabled(true);
}

void AutoExeWin::on_actionTestPause_triggered()
{
    autoPam->setAutoRun(0);
    ui->sb_setStart->setValue(autoPam->curStep());
}

void AutoExeWin::on_actionTestStop_triggered()
{
    autoPam->setAutoRun(0);

    //if CALL_JS is going, how to stop
    quint32 curStep = autoPam->curStep();
    QModelIndex cmdIndex = curFileModel->index(curStep-1, 0);
    QStandardItem *cmdItem = curFileModel->itemFromIndex(cmdIndex);
    QString cmdName = cmdItem->text();

    if(cmdName == "CALL_JS")
    {
        if(curScriptEngine)
        {
            if(curScriptEngine->isEvaluating())
            {
                curScriptEngine->abortEvaluation();
            }
        }
    }

}

void AutoExeWin::on_actionReload_triggered()
{
    cmdModel->clear();
    cmdFilePathHash.clear();
    getCmdScriptFiles();

    jsModel->clear();
    jsFilePathHash.clear();
    getJsScriptFiles();

    autoModel->clear();
    getAutoFiles();
}

void AutoExeWin::on_pb_Load_clicked()
{
    QString fileName = ui->cb_fileList->currentText();

    QString filePath = autoFilePathHash.value(fileName);
    qDebug()<<"current file name:"<<fileName<<"\nCurrent file path:"<<filePath;

    if(filePath.isEmpty())
        return;

    openAutoFilePath(filePath);
}

void AutoExeWin::on_actionAutoEditor_triggered()
{
    if(!autoEditor)
    {
        autoEditor = new Auto_Win(this, workName);
    }

    autoEditor->show();
}

void AutoExeWin::log(QString msg)
{
    if(!Log) return;

    Log->log(msg);
}

void AutoExeWin::wait(quint32 msec)
{
    QEventLoop loop; //
    QTimer::singleShot(msec, &loop, SLOT(quit())); //
    loop.exec(); //
}

void AutoExeWin::msleep(quint32 msec)
{
    QTime dieTime = QTime::currentTime().addMSecs(msec);
    while( QTime::currentTime() < dieTime )
        QCoreApplication::processEvents(QEventLoop::AllEvents, 100);
}

bool AutoExeWin::cmdCondition(QString cmdName, QStringList args, int condType)
{
    //cmd [CONDITION] args: modName - pamName - compareValue1 - comPareValue2 - aimStep(goto)

    if(args.count() != 5) return false;

    double value = getPamValue(args[0], args[1]);

    bool ok = false;
    double compareValue = args[2].toDouble(&ok);
    if(!ok) return false;

    double compareValue2 = args[3].toDouble(&ok);

    quint32 aimStep = args[4].toUInt(&ok);
    if(!ok) return false;

    QStringList compareTypeList = {"==", ">", ">=", "<", "<=",
                                   "!=", "<>", "()", "[)", "(]",
                                   "[]"};

    int typeIndex = compareTypeList.indexOf(args[2]);
    if(typeIndex == -1) return false;

    bool status = false;

    switch (typeIndex) {
    case 0:
        if(value == compareValue)
            status = true;
        break;
    case 1:
        if(value > compareValue)
            status = true;
        break;
    case 2:
        if(value >= compareValue)
            status = true;
        break;
    case 3:
        if(value < compareValue)
            status = true;
        break;
    case 4:
        if(value <= compareValue)
            status = true;
        break;
    case 5:
        if(value != compareValue)
            status = true;
        break;
    case 6:
        if(value != compareValue)
            status = true;
        break;
    case 7:
        if(value > compareValue && value < compareValue2)
            status = true;
        break;
    case 8:
        if(value >= compareValue && value < compareValue2)
            status = true;
        break;
    case 9:
        if(value > compareValue && value <= compareValue2)
            status = true;
        break;
    case 10:
        if(value >= compareValue && value <= compareValue2)
            status = true;
        break;
    default:

        break;
    }


    //different condition type handle
    //condType: 0 = if true goto; 1 = if false goto; 2 = if true hold; 3 = if false hold;
    switch (condType) {
    case 0:
        if(status)
        {
            autoPam->setNextStep(aimStep);
        }
        break;
    case 1:
        if(!status)
        {
            autoPam->setNextStep(aimStep);
        }
        break;
    case 2:
        if(status)
        {
            autoPam->setNextStep(autoPam->curStep());
        }
        break;
    case 3:
        if(!status)
        {
            autoPam->setNextStep(autoPam->curStep());
        }
        break;
    default:

        break;
    }

    return true;
}

void AutoExeWin::transferPhyValueToRawData(A2L_VarChar *charVar, double value, char *data)
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

void AutoExeWin::updateMapValueInSM(A2L_VarChar *map, quint16 index_x, quint16 index_y, char *data)
{
    //qDebug()<<"in update MapValue in SM."<<map->Name<<",index_x="<<index_x<<",index_y="<<index_y;

    QSharedMemory sm;
    sm.setKey(map->smKey);

    if (!sm.isAttached())
    {
        if(!sm.attach())
        {
            qDebug()<<"unable attach to map sharedMemory :"<<sm.key();
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

    //qDebug()<<"index offset="<<index;

    sm.lock();
    memcpy((char*)sm.data()+index, data, map->DataSizeAG);
    sm.unlock();

    //qDebug()<<"map Value updated."<<map->Name<<",index_x="<<index_x<<", index_y="<<index_y;

}

bool AutoExeWin::setSMPamValue(QString smKey, QString pamName, qreal value)
{
    if(smKey.isEmpty() || pamName.isEmpty())
        return false;
    PARAM *pam = Globals::pamHash.value(pamName + "." + smKey);
    if(!pam)
        return false;

    pam->setValue(value);

    return true;
}

qreal AutoExeWin::getSMPamValue(QString smKey, QString pamName)
{
    if(smKey.isEmpty() || pamName.isEmpty())
        return 0;
    PARAM *pam = Globals::pamHash.value(pamName + "." + smKey);
    if(!pam)
        return 0;

    return pam->getValue();
}

QString AutoExeWin::getSMPamValueStr(QString smKey, QString pamName)
{
    if(smKey.isEmpty() || pamName.isEmpty())
        return "None";
    PARAM *pam = Globals::pamHash.value(pamName + "." + smKey);
    if(!pam)
        return "None";

    return QString::number(pam->getValue());
}

bool AutoExeWin::setPamValue(QString modName, QString pamName, qreal value)
{
    if(modName.isEmpty() || pamName.isEmpty())
        return false;

    PARAM *pam = Globals::pamHash.value(pamName + "." + modName);
    if(!pam)
        return false;

    pam->setValue(value);

    log("PAM_SET:" + pamName + "." + modName + "=" + value);

    return true;
}

qreal AutoExeWin::getPamValue(QString modName, QString pamName)
{
    if(modName.isEmpty() || pamName.isEmpty())
        return 0;
    PARAM *pam = Globals::pamHash.value(pamName + "." + modName);
    if(!pam)
        return 0;

    return pam->getValue();
}

QString AutoExeWin::getPamValueStr(QString modName, QString pamName)
{
    if(modName.isEmpty() || pamName.isEmpty())
        return "None";

    PARAM *pam = Globals::pamHash.value(pamName + "." + modName);
    if(!pam)
        return "None";

    return QString::number(pam->getValue(), 'g', 8);
}

bool AutoExeWin::setMapPamValue(QString modName, QString pamName, int index_x, int index_y, qreal value)
{
    //qDebug()<<"in script obj setPamValue";
    if(modName.isEmpty() || pamName.isEmpty())
        return false;

    PARAM *pam = Globals::mapPamHash.value(pamName + "." + modName);
    if(!pam)
        return false;

    //pam->setValue(value);

    A2L_VarChar *mapChar = (A2L_VarChar*)pam;

    if(index_x == 0)
    {
        qDebug()<<"Axis value updated."<<pamName;
    }
    else if (index_y == 0)
    {
        qDebug()<<"Yxis value updated."<<pamName;
    }
    else
    {
        //qDebug()<<"Z map value updated."<<pamName;

        char *data = new char[mapChar->DataSizeAG];
        transferPhyValueToRawData(mapChar, value, data);

        updateMapValueInSM(mapChar, index_y - 1, index_x - 1, data);
    }

    log("PAM_SET_MAP:" + pamName + "." + modName + "[" + index_x + "]" + "[" + index_y + "]" + "=" + value);

    return true;
}

void AutoExeWin::setSysPam(SYS_PAM *value)
{
    sysPam = value;
}

void AutoExeWin::setAutoPam(AUTO_PAM *value)
{
    autoPam = value;

    connect(autoPam, SIGNAL(curStepChanged()), this, SLOT(curStepChangedSlot()));
    connect(autoPam, SIGNAL(autoRunChanged()), this, SLOT(autoRunChangedSlot()));

}

void AutoExeWin::setWorkName(const QString &value)
{
    workName = value;
}


void AutoExeWin::on_actionCsvSetting_triggered()
{
    if(csvSetDlg == NULL)
    {
        csvSetDlg = new CsvSettingDlg(nullptr, workName);
    }

    csvSetDlg->show();
}

void AutoExeWin::on_actionCsv_triggered(bool checked)
{
    if(!csvObj)
    {
        csvObj = new CSVObject(this);
    }
    csvObj->setWorkName(this->workName);

    if(checked)
    {
        csvObj->csv_record_start();
    }
    else
    {
        csvObj->csv_record_stop();
    }
}
