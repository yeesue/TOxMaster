#ifndef UDSSETDLG_H
#define UDSSETDLG_H

#include <QDialog>
#include <QSettings>
#include "uds_config.h"

namespace Ui {
class UdsSetDlg;
}

class UdsSetDlg : public QDialog
{
    Q_OBJECT

public:
    explicit UdsSetDlg(QWidget *parent = nullptr, QString work_name = "", QString fun_name = "",
                       QString intf_name = "", QString set_file_path = "");
    ~UdsSetDlg();

    QString getUdsSetFileName() const;

    QString getUdsSetFilePath() const;

protected:
    void accept();

signals:
    void udsSetUpdated();

private:
    void readSetting();
    void writeSetting();

private:
    Ui::UdsSetDlg *ui;

    QString udsSetFileName = "";
    QString udsSetFilePath = "";
    QString workName = "";
    QString funName = "";
    QString intfName = "";

    UDSInfo udsInfo;

};

#endif // UDSSETDLG_H
