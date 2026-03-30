#ifndef PAMSELDLG_H
#define PAMSELDLG_H

#include <QDialog>
#include <QRegExp>
#include <QListWidgetItem>
#include <QTableWidgetItem>
#include "a2l_project.h"

namespace Ui {
class PAMSELDlg;
}

class PAMSELDlg : public QDialog
{
    Q_OBJECT

public:
    explicit PAMSELDlg(QWidget *parent = 0, A2L_Project *project = NULL);
    ~PAMSELDlg();

public slots:
    void setMeasAll(QStringList measNameList);
    void setCharAll(QStringList charNameList);

    void setList(QStringList measList, QStringList charList);
    void setMeasList(QStringList measList);
    void setCharList(QStringList charList);

    void setEventNameList(QStringList eventList);
    void setEventCurSizeHash(QHash<QString, quint32> curSizeHash);
    void setEventMaxSizeHash(QHash<QString, quint32> maxSizeHash);
    void setEventRateHash(QHash<QString, qreal> rateHash);

    void checkStateChangedMeasSlot(int state);
    void checkStateChangedCharSlot(int state);

    void measItemClickedSlot(QListWidgetItem *item);
    void charItemClickedSlot(QListWidgetItem *item);
    void measMatrixItemClickedSlot(QListWidgetItem *item);

    void selectedItemClickedSlot(QListWidgetItem *item);
    void doubleClickedItemClickedSlot(QTableWidgetItem *item);

    QListWidgetItem* findItemByName(QListWidget *list, QString name);
    bool takeItemByName(QListWidget *list, QString name);

    QStringList getPamsMatchExp(QStringList pamList, QString searchExp);

    void searchTextChangedSlot(QString searchText);

    void getResultPamNames();

    void getPamRateMeasList();

    void eventComboTextChangedSlot(QString text);

    void setProject(A2L_Project *project);

    void updateAllTempEventCurSize();
    void updateTempEventCurSize(A2L_VarMeas *measVar);

    void removeMeasVarByName(QString measVarName);

    int findSelMeasTableRowByName(QString pamName);

    void selMeasRateChangedSlot(QString text);

signals:
    void pamSelectionAccepted(QStringList measPamList, QStringList charPamList, QStringList measPamRateList);
    void pamRateMeasSelectionAccepted(QStringList measPamRateList);

    void measPamSelAccepted(QStringList measPamList);
    void charPamSelAccepted(QStringList charPamList);
private:
    Ui::PAMSELDlg *ui;

    QStringList pamNameAll;

    QStringList pamNameMeasAll;
    QStringList pamNameCharAll;

    QStringList pamNameMeasFilted;
    QStringList pamNameCharFilted;

    QStringList pamNameMeasSelected;
    QStringList pamNameCharSelected;

    QStringList pamRateMeasSelected;

    QStringList eventNameList;

    QHash<QString, qreal> eventNameRateHash;

    QHash<QString, quint32> eventNameCurSizeHash;
    QHash<QString, quint32> eventNameMaxSizeHash;
    QHash<QString, quint32> eventNameTempCurSizeHash;

    A2L_Project *a2lProject = NULL;

    QHash<A2L_VarMeas*, float> measVarRateHash;
    QList<A2L_VarMeas*> tempMeasVarList;

    //QRegExp exp;
};

#endif // PAMSELDLG_H
