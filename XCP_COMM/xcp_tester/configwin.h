#ifndef CONFIGWIN_H
#define CONFIGWIN_H

#include <QMainWindow>
#include <QSettings>
#include <QFileInfo>

namespace Ui {
class ConfigWin;
}

class ConfigWin : public QMainWindow
{
    Q_OBJECT

public:
    explicit ConfigWin(QWidget *parent = nullptr);
    ~ConfigWin();

private slots:
    void on_pb_select_MeasPamFile_clicked();

    void on_pb_select_CharPamFile_clicked();

    void on_pb_load_clicked();

    void on_pb_save_clicked();

    void on_pb_select_CalPamFile_clicked();

    void on_pb_select_CanPamFile_clicked();

    void on_pb_select_ScriptFile_clicked();

    void on_actionNew_Configuration_triggered();

    void on_actionOpen_Configuration_triggered();

    void on_actionSave_triggered();

    void on_actionSave_As_triggered();

private:
    bool readSetting(QString configFilePath);
    bool writeSetting(QString configFilePath);



private:
    Ui::ConfigWin *ui;

    int flag_isOpen = 0;
    int flag_isNew = 0;
    QString Last_FileName;
    QString Last_FileContent;

};

#endif // CONFIGWIN_H
