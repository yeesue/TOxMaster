#ifndef CSVSETTINGDLG_H
#define CSVSETTINGDLG_H

#include <QDialog>
#include <QFileDialog>
#include <QDir>
#include <QTextStream>
#include <QTableWidgetItem>
#include <QStandardItemModel>
#include "param.h"
#include "globals.h"
#include "csvobject.h"

namespace Ui {
class CsvSettingDlg;
}

class CsvSettingDlg : public QDialog
{
    Q_OBJECT

public:
    explicit CsvSettingDlg(QWidget *parent = nullptr, QString name = "");
    ~CsvSettingDlg();

protected:
    void accept();

private slots:
    void on_pb_config_released();

    void on_pb_filter_released();

    void on_pb_refresh_released();

    void on_pb_selected_released();

    void on_pb_unselected_released();

    void on_pb_up_released();

    void on_pb_down_released();

private:
    void readSetting();
    void writeSetting();

    void showAllPamsInTable();
    void getSelectedPamsInTable();
private:
    Ui::CsvSettingDlg *ui;

    QList<PARAM*> recordPamList;
    QHash<QString, PARAM*> recordPamHash;

    QString csvFileName = "";
    QString csvFilePath = "";
    QFile *csvFile = NULL;
    QTextStream out;

    QString workName = "";
    QString workRecordDir = "";

    QStandardItemModel *pamModel = NULL;

    QStandardItemModel *selPamModel = NULL;
};

#endif // CSVSETTINGDLG_H
