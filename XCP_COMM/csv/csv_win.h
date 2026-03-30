#ifndef CSV_WIN_H
#define CSV_WIN_H

#include <QMainWindow>
#include <QTime>
#include <QTimer>
#include <QFileDialog>
#include <QDir>
#include <QTextStream>
#include <QTableWidgetItem>
#include <QStandardItemModel>
#include "param.h"
#include "globals.h"
#include "csvobject.h"
#include <QThread>

namespace Ui {
class CSV_Win;
}

class CSV_Win : public QMainWindow
{
    Q_OBJECT

public:
    explicit CSV_Win(QWidget *parent = 0);
    ~CSV_Win();

    QList<PARAM *> getRecordPamList() const;
    void setRecordPamList(const QList<PARAM *> &value);

private slots:

    void showAllPamsInTableWidget();
    void getSelectedPamsInTable();

    void on_pb_Save_clicked();

    void on_pb_rcd_path_config_clicked();

    void on_pb_Filter_clicked();

signals:

private:
    Ui::CSV_Win *ui;

    QList<PARAM*> recordPamList;

    QString csv_FileName;
    QString csv_FilePath;
    QFile *csv_File = NULL;
    QTextStream out;

    QStandardItemModel *pamModel = NULL;

    CSVObject *csvObj = NULL;

};

#endif // CSV_WIN_H
