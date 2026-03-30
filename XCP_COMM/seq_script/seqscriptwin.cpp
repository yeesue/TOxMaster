#include "seqscriptwin.h"
#include "ui_seqscriptwin.h"

SeqScriptWin::SeqScriptWin(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::SeqScriptWin)
{
    ui->setupUi(this);
    setWindowTitle("自动程序");

    cmdModel1 = new QStandardItemModel();
    ui->cmdTable_1->setModel(cmdModel1);

    QStringList header;
    header << "序号" << "步骤名" << "指令" << "指令值" << "延时(ms)";
    cmdModel1->setHorizontalHeaderLabels(header);

    cmdAttrModel = new QStandardItemModel();
    ui->cmdAttrTable->setModel(cmdAttrModel);

    QStringList header1;
    header << "序号" << "名称" << "值";
    cmdAttrModel->setHorizontalHeaderLabels(header1);
}

SeqScriptWin::~SeqScriptWin()
{
    delete ui;
}
