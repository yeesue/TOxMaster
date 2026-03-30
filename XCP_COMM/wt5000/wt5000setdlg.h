#ifndef WT5000SETDLG_H
#define WT5000SETDLG_H

#include <QDialog>
#include <QSettings>

namespace Ui {
class WT5000SetDlg;
}

class WT5000SetDlg : public QDialog
{
    Q_OBJECT

public:
    explicit WT5000SetDlg(QWidget *parent = nullptr, QString work_name = "", QString fun_name = "",
                          QString intf_name = "", QString set_file_path = "");
    ~WT5000SetDlg();

    QString getSetFilePath() const;

protected:
    void accept();

signals:
    void setUpdated();

private:
    void writeSetting();
    void readSetting();

private:
    Ui::WT5000SetDlg *ui;

    QString workName = "";
    QString funName = "";
    QString intfName = "";
    QString setFileName = "";
    QString setFilePath = "";

};

#endif // WT5000SETDLG_H
