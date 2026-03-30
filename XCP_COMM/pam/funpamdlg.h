#ifndef FUNPAMDLG_H
#define FUNPAMDLG_H

#include <QDialog>
#include "common.h"
#include "globals.h"
#include "a2l_varchar.h"
#include "a2l_varmeas.h"
#include <QListWidgetItem>

namespace Ui {
class FunPamDlg;
}

class FunPamDlg : public QDialog
{
    Q_OBJECT

public:
    explicit FunPamDlg(QWidget *parent = nullptr, int mode = 0);
    ~FunPamDlg();

    void clearSelection();

public slots:
    void Slt_ModuleChanged(QString text);

    void pamItemClickedSlot(QListWidgetItem* item);
    void pamSelectedItemDoubleClicked(QListWidgetItem* item);

protected:
    void accept();

signals:
    void listItemsSelected(QStringList);

    void pamListSelected(QList<A2L_VarMeas*>);
    void pamFirstSelected(A2L_VarMeas*);

    void pamListSelected(QList<PARAM*>);
    void pamFirstSelected(PARAM*);

    void charPamListSelected(QList<A2L_VarChar*>);
    void charPamFirstSelected(A2L_VarChar*);

    void writePamListSelected(QList<PARAM*>);
    void writePamFirstSelected(PARAM*);

private:
    QList<PARAM*> getPamsByNames(QStringList names);
    PARAM *findPambyName(QString module, QString name);

    QList<PARAM*> getWritePamsByNames(QStringList names);
    PARAM *findWritePambyName(QString module, QString name);

    QList<A2L_VarChar*> getCharPamsByNames(QStringList names);
    A2L_VarChar *findCharPambyName(QString module, QString name);


private:
    Ui::FunPamDlg *ui;

    int pamMode = 0; // 0 = meas, 1 = char

    QList<PARAM*> pamSelected;


    QList<A2L_VarChar*> charPamSelected;
    QList<PARAM*> writePamSelected;

};

#endif // FUNPAMDLG_H
