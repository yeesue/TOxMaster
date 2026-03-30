#ifndef PAMLISTDLG_H
#define PAMLISTDLG_H

#include <QDialog>
#include "common.h"
#include <QListWidgetItem>
#include "globals.h"

namespace Ui {
class pamListDlg;
}

class pamListDlg : public QDialog
{
    Q_OBJECT

public:
    explicit pamListDlg(QWidget *parent = 0);
    ~pamListDlg();

    void clearSelection();

public slots:
    void SltModuleChanged(int index);

    void pamItemClickedSlot(QListWidgetItem* item);
    void pamSelectedItemDoubleClicked(QListWidgetItem* item);

protected:
    void accept();

signals:
    void listItemsSelected(QStringList);

    void pamListSelected(QList<PARAM*>);
    void pamFirstSelected(PARAM*);

private:
    Ui::pamListDlg *ui;

    QList<PARAM*> pamSelected;
    QList<PARAM*> getPamsByNames(QStringList names);
    PARAM *findPambyName(QString key, QString name);

};

#endif // PAMLISTDLG_H
