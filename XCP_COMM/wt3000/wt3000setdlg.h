#ifndef WT3000SETDLG_H
#define WT3000SETDLG_H

#include <QDialog>
#include <QSettings>

namespace Ui {
class WT3000SetDlg;
}

class WT3000SetDlg : public QDialog
{
    Q_OBJECT

public:
    explicit WT3000SetDlg(QWidget *parent = nullptr, QString work_name = "", QString fun_name = "",
                          QString intf_name = "", QString set_file_path = "");
    ~WT3000SetDlg();

    QString getSetFilePath() const;

protected:
    void accept();

signals:
    void setUpdated();

private:
    void writeSetting();
    void readSetting();

private:
    Ui::WT3000SetDlg *ui;

    QString workName = "";
    QString funName = "";
    QString intfName = "";
    QString setFileName = "";
    QString setFilePath = "";

};

#endif // WT3000SETDLG_H
