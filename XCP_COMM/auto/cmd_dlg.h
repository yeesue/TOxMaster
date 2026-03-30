#ifndef CMD_DLG_H
#define CMD_DLG_H

#include <QDialog>
#include <QStandardItemModel>
#include <QDir>
#include <QFile>
#include "pamlistdlg.h"

namespace Ui {
class CMD_DLG;
}

class CMD_DLG : public QDialog
{
    Q_OBJECT

public:
    explicit CMD_DLG(QWidget *parent = nullptr);
    ~CMD_DLG();

protected:
    void accept();

signals:
    void cmdInfo(QStringList);

private slots:
    void listDoubleClickedSlot(QModelIndex index);
    void showMenuInCmdTable(QPoint pos);

    void addRowSlot();
    void removeRowSlot();
    void pamChooseSlot();
    void pamChooseHandle(QList<PARAM*> pams);

private:
    void init();
    void getCmdFiles();

private:
    Ui::CMD_DLG *ui;

    QStringList cmdList{
                        "PAM_SET", "PAM_SET_CURVE","PAM_SET_MAP",
                        "PAM_GET",
                        "DYNO", "WAIT", "CALL", "CALL_JS",
                        "CONDITION_T_GOTO", "CONDITION_F_GOTO", "CONDITION_F_HOLD",
                        "CSV", "MULTI_PAM_SET_01"
                       };


    QStandardItemModel *cmdModel = NULL;
    QHash<QString, QString> sFilePathHash;
};

#endif // CMD_DLG_H
