#include "cmd_dlg.h"
#include "ui_cmd_dlg.h"
#include <QMenu>

CMD_DLG::CMD_DLG(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::CMD_DLG)
{
    ui->setupUi(this);
    setWindowTitle(tr("命令编辑"));

    init();
}

CMD_DLG::~CMD_DLG()
{
    delete ui;
}

void CMD_DLG::accept()
{
    QStringList infoList;

    QModelIndex cmdIndex = ui->list_cmd->currentIndex();
    QString cmd = cmdModel->itemFromIndex(cmdIndex)->text();
    infoList.append(cmd);

    int rowCount = ui->table_cmdAttr->rowCount();
    int colCount = ui->table_cmdAttr->columnCount();

    for(int i = 0; i < rowCount; i++)
    {
        for(int j = 0; j < colCount; j++)
        {
            QTableWidgetItem *tableItem = ui->table_cmdAttr->item(i, j);
            if(!tableItem)
                continue;
            QString cmdStr = tableItem->text();

            infoList.append(cmdStr);
        }
    }

    emit cmdInfo(infoList);

    QDialog::accept();
}

void CMD_DLG::listDoubleClickedSlot(QModelIndex index)
{
    QString cmdName = cmdModel->itemFromIndex(index)->text();
    ui->table_cmdAttr->clear();
    ui->table_cmdAttr->setRowCount(0);

    ui->table_cmdAttr->insertRow(ui->table_cmdAttr->rowCount());
    int curRow = ui->table_cmdAttr->rowCount()-1;

    QStringList header;
    QString cmdComment;
    if(cmdName.contains("DYNO"))
    {
        header<<tr("BenchMode")<<tr("D_Req")<<tr("D_Trans(s)")
                <<tr("L_Req")<<tr("L_Trans(s)")<<tr("n_Diff_Req")
                <<tr("T_Diff_Req")<<tr("T_Limit_n_ctrl");

        cmdComment = tr("【DYNO】命令：\n"
                        "输入： 台架模式 - 驱动需求 - 驱动过渡时间 - 负载需求 - 负载过渡时间 - 差速需求 - 差扭限值 - 转速模式扭矩限值\n"
                        "输出： 执行成功状态\n");

        ui->table_cmdAttr->setItem(curRow, 0, new QTableWidgetItem("0"));
        ui->table_cmdAttr->setItem(curRow, 1, new QTableWidgetItem("0"));
        ui->table_cmdAttr->setItem(curRow, 2, new QTableWidgetItem("5"));
        ui->table_cmdAttr->setItem(curRow, 3, new QTableWidgetItem("0"));
        ui->table_cmdAttr->setItem(curRow, 4, new QTableWidgetItem("5"));
        ui->table_cmdAttr->setItem(curRow, 5, new QTableWidgetItem("0"));
        ui->table_cmdAttr->setItem(curRow, 6, new QTableWidgetItem("300"));
        ui->table_cmdAttr->setItem(curRow, 7, new QTableWidgetItem("1500"));
    }
    else if(cmdName == "PAM_SET")
    {
        header<<tr("模块名")<<tr("参数名")<<tr("计算类型")<<tr("计算值")<<tr("等待时间(ms)");

        cmdComment = tr("【PAM_SET】命令：\n"
                        "输入： 模块名称 - 参数名称 - 计算类型 - 计算值 - 动作后等待时间(ms)\n"
                        "输出： 写参数成功状态\n"
                        "计算类型集合：+, -, x, /, %, ++, --, =, *, X, ÷\n"
                        "计算逻辑：\n"
                        "[+=],[-=],[*=],[/=],[%=],[++],[--],[=],[*=],[*=],[/=],[/=]"
                        );
    }
    else if(cmdName == "PAM_SET_CURVE")
    {
        header<<tr("模块名")<<tr("参数名")<<tr("索引_X")<<tr("索引_Y")<<tr("设定值")<<tr("等待时间(ms)");

        cmdComment = tr("【PAM_SET_CURVE】命令：\n"
                        "输入： 模块名称 - 参数名称 - CURVE位置索引_X - [默认0]CURVE位置索引_Y - 参数设定值 - 动作后等待时间(ms)\n"
                        "输出： 写参数成功状态\n");
    }
    else if(cmdName == "PAM_SET_MAP")
    {
        header<<tr("模块名")<<tr("参数名")<<tr("索引_X")<<tr("索引_Y")<<tr("设定值")<<tr("等待时间(ms)");

        cmdComment = tr("【PAM_SET_MAP】命令：\n"
                        "输入： 模块名称 - 参数名称 - MAP位置索引_X - MAP位置索引_Y - 参数设定值 - 动作后等待时间(ms)\n"
                        "输出： 写参数成功状态\n");
    }
    else if(cmdName == "PAM_GET")
    {
        header<<tr("模块名")<<tr("参数名")<<tr("等待时间(ms)");

        cmdComment = tr("【PAM_GET】命令：\n"
                        "输入: 模块名称 - 参数名称 - 动作后等待时间(ms)\n"
                        "输出: 返回的参数值\n");
    }
    else if(cmdName.contains("WAIT"))
    {
        header<<tr("等待时间(ms)");

        cmdComment = tr("【WAIT】命令：\n"
                        "输入: 等待时间(ms)\n"
                        "输出: 无返回值\n");

        ui->table_cmdAttr->setItem(curRow, 0, new QTableWidgetItem("1000"));
    }
    else if(cmdName.contains("CALL"))
    {
        header<<tr("子程序");

        cmdComment = tr("【CALL】命令：\n"
                        "输入: 子程序名称\n"
                        "输出: 返回true\n"
                        "逻辑说明：\n"
                        "调用另一个脚本程序\n");
    }
    else if(cmdName.contains("CALL_JS"))
    {
        header<<tr("JS脚本");

        cmdComment = tr("【CALL_JS】命令：\n"
                        "输入: JS脚本名称\n"
                        "输出: 返回true\n"
                        "逻辑说明：\n"
                        "调用一个JS脚本\n");
    }
    else if(cmdName.contains("CSV"))
    {
        header<<tr("平均总时间(s)")<<tr("单次记录时间(ms)")<<tr("等待时间(ms)");

        cmdComment = tr("【CSV】命令：\n"
                        "输入: 平均总时间(s) - 单次记录时间(ms) - 等待时间(ms)\n"
                        "输出: 返回true\n"
                        "逻辑说明：\n"
                        "在建立的CSV记录文件中产生一条记录，记录的结果是在‘平均总时间’内多次采集结果的平均值，采集的间隔时间为‘单次记录时间’\n");
    }
    else if(cmdName.contains("COUNTER"))
    {
        header<<tr("Counter索引(1-5)")<<tr("计算类型")<<tr("计算值")<<tr("等待时间(ms)");

        cmdComment = tr("【COUNTER】命令：\n"
                        "输入: Counter索引值(1-5) - 计算类型 - 计算对象值 - 等待时间(ms)\n"
                        "输出: 返回true\n"
                        "计算类型集合：+, -, x, /, %, ++, --, =, *, X, ÷\n"
                        "逻辑说明：\n"
                        "自动程序内置的Counter1-5参数，可进行加、减、乘、除、自加、自减等运算，并作为选择、循环等逻辑脚本的判断依据\n");
    }
    else if(cmdName.contains("CONDITION_F_GOTO"))
    {
        header<<tr("模块名")<<tr("参数名")<<tr("比较类型")
                <<tr("比较值1")<<tr("比较值2")<<tr("Goto Step(If-False)")<<tr("等待时间(ms)");

        cmdComment = tr("【CONDITION_F_GOTO】命令：\n"
                        "输入: 模块名称 - 参数名称 - 比较类型  - 比较值1 - 比较值2 - Goto Step(If-False) - 等待时间(ms)\n"
                        "输出: 返回比较结果状态 True or False\n"
                        "比较类型集合：==, >, >=, <, <=, !=, <>, (), [), (], []\n"
                        "逻辑说明：\n"
                        "第1种 '=='：IF(!(获取的参数值 == 比较值1)) Goto 目标步骤 ELSE Goto 下一步\n"
                        "第2种 '[]'：IF(!(获取的参数值 >= 比较值1 && 获取的参数值 <= 比较值2)) Goto 目标步骤 ELSE Goto 下一步\n");
    }
    else if(cmdName.contains("CONDITION_F_HOLD"))
    {
        header<<tr("模块名")<<tr("参数名")<<tr("比较类型")
                <<tr("比较值1")<<tr("比较值2")<<tr("等待时间(ms)");

        cmdComment = tr("【CONDITION_F_HOLD】命令：\n"
                        "输入: 模块名称 - 参数名称 - 比较类型  - 比较值1 - 比较值2 - 等待时间(ms)\n"
                        "输出: 返回比较结果状态 True or False\n"
                        "比较类型集合：==, >, >=, <, <=, !=, <>, (), [), (], []\n"
                        "逻辑说明：\n"
                        "第1类 '=='：IF(!(获取的参数值 == 比较值1)) Goto 当前步骤 ELSE Goto 下一步\n"
                        "第2类 '[]'：IF(!(获取的参数值 >= 比较值1 && 获取的参数值 <= 比较值2)) Goto 当前步骤 ELSE Goto 下一步\n");
    }
    else if(cmdName.contains("CONDITION_T_GOTO"))
    {
        header<<tr("模块名")<<tr("参数名")<<tr("比较类型")
                <<tr("比较值1")<<tr("比较值2")<<tr("Goto Step(If-True)")<<tr("等待时间(ms)");

        cmdComment = tr("【CONDITION_T_GOTO】命令：\n"
                        "输入: 模块名称 - 参数名称 - 比较类型  - 比较值1 - 比较值2 - Goto Step(If-False) - 等待时间(ms)\n"
                        "输出: 返回比较结果状态 True or False\n"
                        "比较类型集合：==, >, >=, <, <=, !=, <>, (), [), (], []\n"
                        "逻辑说明：\n"
                        "第1种 '=='：IF(获取的参数值 == 比较值1) Goto 目标步骤 ELSE Goto 下一步\n"
                        "第2种 '[]'：IF(获取的参数值 >= 比较值1 && 获取的参数值 <= 比较值2) Goto 目标步骤 ELSE Goto 下一步\n");
    }
    else if(cmdName.contains("CONDITION_T_HOLD"))
    {
        header<<tr("模块名")<<tr("参数名")<<tr("比较类型")
                <<tr("比较值1")<<tr("比较值2")<<tr("等待时间(ms)");

        cmdComment = tr("【CONDITION_T_HOLD】命令：\n"
                        "输入: 模块名称 - 参数名称 - 比较类型  - 比较值1 - 比较值2 - 等待时间(ms)\n"
                        "输出: 返回比较结果状态 True or False\n"
                        "比较类型集合：==, >, >=, <, <=, !=, <>, (), [), (], []\n"
                        "逻辑说明：\n"
                        "第1类 '=='：IF(获取的参数值 == 比较值1) Goto 当前步骤 ELSE Goto 下一步\n"
                        "第2类 '[]'：IF(获取的参数值 >= 比较值1 && 获取的参数值 <= 比较值2) Goto 当前步骤 ELSE Goto 下一步\n");
    }
    else if(cmdName.contains("MULTI_PAM_SET_01"))
    {
        header<<tr("参数值_0")<<tr("参数值_1")<<tr("参数值_2")<<tr("参数值_3")<<tr("参数值_4")<<tr("参数值_5")<<tr("参数值_6")<<tr("参数值_7")<<tr("参数值_8");
                tr("等待时间(ms)");

        cmdComment = tr("【MULTI_PAM_SET_01】命令：\n"
                        "输入:  参数值0-8 - 等待时间(ms)\n"
                        "输出: 返回比较结果状态 True or False\n"
                        "逻辑说明：\n"
                        "单个命令内进行多个参数的值设置，最大参数数量为9，参数名称在JS命令文件内进行设置\n"
                        "类似的命令可以自定义，方便使用\n");
    }

    ui->table_cmdAttr->setHorizontalHeaderLabels(header);
    ui->text_cmdDescription->setText(cmdComment);

    //addRowSlot();

}

void CMD_DLG::showMenuInCmdTable(QPoint pos)
{
    QMenu *menu = new QMenu(ui->table_cmdAttr);

    QAction *actionAdd = new QAction("Add", ui->table_cmdAttr);
    QAction *actionRemove = new QAction("Remove", ui->table_cmdAttr);
    QAction *actionPamSelect = new QAction("Pam Choose", ui->table_cmdAttr);

    connect(actionAdd, SIGNAL(triggered()), this, SLOT(addRowSlot()));
    connect(actionRemove, SIGNAL(triggered()), this, SLOT(removeRowSlot()));
    connect(actionPamSelect, SIGNAL(triggered()), this, SLOT(pamChooseSlot()));

    menu->addAction(actionAdd);
    menu->addAction(actionRemove);
    menu->addAction(actionPamSelect);

    menu->move(cursor().pos());
    menu->show();
}

void CMD_DLG::addRowSlot()
{
    ui->table_cmdAttr->insertRow(ui->table_cmdAttr->rowCount());
    QString cmdName = cmdModel->itemFromIndex(ui->list_cmd->currentIndex())->text();
    int curRow = ui->table_cmdAttr->rowCount()-1;

    int cmdIndex = cmdList.indexOf(cmdName);

}

void CMD_DLG::removeRowSlot()
{
    ui->table_cmdAttr->removeRow(ui->table_cmdAttr->rowCount()-1);
}

void CMD_DLG::pamChooseSlot()
{
    QString cmdName = cmdModel->itemFromIndex(ui->list_cmd->currentIndex())->text();

    if(!cmdName.contains("PAM_") && !(cmdName.contains("CONDITION")))
        return;

    pamListDlg *pamDlg = new pamListDlg();
    pamDlg->setAttribute(Qt::WA_DeleteOnClose);
    connect(pamDlg, SIGNAL(pamListSelected(QList<PARAM*>)), this, SLOT(pamChooseHandle(QList<PARAM*>)));

    pamDlg->show();

}

void CMD_DLG::pamChooseHandle(QList<PARAM *> pams)
{
    int pamIndex = 0;

    int pamCount = pams.count();
    if(pamCount == 0)
        return;

    int curRow = ui->table_cmdAttr->currentIndex().row();

    for(int i = curRow; i < ui->table_cmdAttr->rowCount(); i++)
    {
        if(pamCount <= pamIndex)
            return;

        PARAM *pam = pams.at(pamIndex);
        ui->table_cmdAttr->setItem(i, 0, new QTableWidgetItem(pam->smKey));
        ui->table_cmdAttr->setItem(i, 1, new QTableWidgetItem(pam->Name));
        ui->table_cmdAttr->setItem(i, 2, new QTableWidgetItem(0));
        pamIndex++;
    }
}

void CMD_DLG::init()
{
    cmdModel = new QStandardItemModel();
    ui->list_cmd->setModel(cmdModel);
    ui->list_cmd->setAlternatingRowColors(true);
    cmdModel->setHeaderData(0, Qt::Horizontal, tr("Function"));

    connect(ui->list_cmd, SIGNAL(doubleClicked(QModelIndex)), this, SLOT(listDoubleClickedSlot(QModelIndex)));

    getCmdFiles();

    ui->table_cmdAttr->setColumnCount(10);
    ui->table_cmdAttr->setRowCount(1);
    QStringList header_table;
    header_table<<tr("参数0")<<tr("参数1")<<tr("参数2")<<tr("参数3")<<tr("参数4")
                <<tr("参数5")<<tr("参数6")<<tr("参数7")<<tr("参数8")<<tr("参数9");
    ui->table_cmdAttr->setHorizontalHeaderLabels(header_table);
    ui->table_cmdAttr->setShowGrid(true);
    ui->table_cmdAttr->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(ui->table_cmdAttr, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(showMenuInCmdTable(QPoint)));

}

void CMD_DLG::getCmdFiles()
{
    QString curPath = QApplication::applicationDirPath();
    QString scriptFilePath = curPath + "/Auto";
    QDir *dir = new QDir(scriptFilePath);

    QStringList nameFilter;
    nameFilter << "*.js";
    QFileInfoList fileInfo = dir->entryInfoList(nameFilter);
    QStringList scriptFileList;
    for (int i = 0; i < fileInfo.count(); i++)
    {
        QString filePath = fileInfo.at(i).filePath();
        QString shortFile = filePath.split('/').last();
        QString funName = shortFile;
        funName.replace(".js", "");
        sFilePathHash.insert(funName, filePath);

        if(funName.isEmpty())
            continue;


        scriptFileList << filePath;

        QStandardItem *scriptItem = new QStandardItem(funName);
        scriptItem->setIcon(QIcon(":/icon/icon/fun.png"));
        //scriptItem->setCheckable(true);

        QList<QStandardItem*> itemList;
        itemList<<scriptItem;

        cmdModel->appendRow(itemList);

    }
}
