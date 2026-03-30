#include "meter_1.h"
#include "ui_meter_1.h"
#include <QMessageBox>

Meter_1::Meter_1(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::Meter_1)
{
    ui->setupUi(this);

    setWindowTitle("MeterWin");
    setAttribute(Qt::WA_DeleteOnClose);

    m_contextMenu = new QMenu();
    QAction *propAction = new QAction("Param property", this);
    QAction *pamSelAction = new QAction("Param Selection", this);
    m_contextMenu->addAction(propAction);
    m_contextMenu->addAction(pamSelAction);

    connect(propAction, SIGNAL(triggered()), this, SLOT(propAction_triggered()));
    connect(pamSelAction, SIGNAL(triggered()), this, SLOT(selectPam()));

}

Meter_1::~Meter_1()
{
    delete m_contextMenu;
    delete ui;
}

void Meter_1::setPam(A2L_VarMeas *pam)
{
    if(pam == NULL)
        return;

    refPam = pam;
    connect(refPam, SIGNAL(valueUpdated(qreal)), this, SLOT(setValue(qreal)));

    setWindowTitle(refPam->Name);

}

void Meter_1::propAction_triggered()
{

}

void Meter_1::selectPam()
{
    pamListDlg *pamDlg = new pamListDlg;
    pamDlg->setAttribute(Qt::WA_DeleteOnClose);
    pamDlg->show();

    connect(pamDlg, SIGNAL(pamFirstSelected(A2L_VarMeas*)), this, SLOT(setPam(A2L_VarMeas*)));
}

void Meter_1::setValue(qreal value)
{
    ui->gaugeCar->setValue(value);
}

void Meter_1::setRange(qreal min, qreal max)
{
    if(max <= min)
        return;
    ui->gaugeCar->setRange(min, max);
}

void Meter_1::contextMenuEvent(QContextMenuEvent *event)
{
    m_contextMenu->exec(event->globalPos());
}

void Meter_1::closeEvent(QCloseEvent *event)
{
    QMessageBox msgBox;
    msgBox.setIcon(QMessageBox::Warning);
    msgBox.setWindowTitle("Window close warning");
    msgBox.setText("Confirm to close this window?");
    msgBox.setStandardButtons(QMessageBox::Ok | QMessageBox::Cancel);
    msgBox.setDefaultButton(QMessageBox::Ok);
    int ret = msgBox.exec();
    if (ret == QMessageBox::Cancel)
    {
        event->ignore();
        return;
    }
    else
    {
        event->accept();
    }
}
