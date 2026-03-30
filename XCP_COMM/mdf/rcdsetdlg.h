#ifndef RCDSETDLG_H
#define RCDSETDLG_H

#include <QDialog>

namespace Ui {
class RcdSetDlg;
}

class RcdSetDlg : public QDialog
{
    Q_OBJECT

public:
    explicit RcdSetDlg(QWidget *parent = nullptr, QString name = "");
    ~RcdSetDlg();

    void setWorkName(const QString &value);

protected:
    void accept();

private:
    void readSetting();
    void writeSetting();

private:
    Ui::RcdSetDlg *ui;

    QString workName = "";
    QString workRecordDir = "";
};

#endif // RCDSETDLG_H
