#include "verifydialog.h"
#include "ui_verifydialog.h"

VerifyDialog::VerifyDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::VerifyDialog)
{
    ui->setupUi(this);

    ui->lineEdit_Password->setEchoMode(QLineEdit::Password);

}

VerifyDialog::~VerifyDialog()
{
    delete ui;
}
