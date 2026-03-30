#ifndef VERIFYDIALOG_H
#define VERIFYDIALOG_H

#include <QDialog>

namespace Ui {
class VerifyDialog;
}

class VerifyDialog : public QDialog
{
    Q_OBJECT

public:
    explicit VerifyDialog(QWidget *parent = 0);
    ~VerifyDialog();


private:
    Ui::VerifyDialog *ui;

    QString password = "LyxCspLqt_@_#_&_122505";
};

#endif // VERIFYDIALOG_H
