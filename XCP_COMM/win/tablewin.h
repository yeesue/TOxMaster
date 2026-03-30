#ifndef TABLEWIN_H
#define TABLEWIN_H

#include <QMainWindow>
#include <QStandardItemModel>
#include <QStandardItem>
#include "common.h"
#include "a2l_project.h"
#include "param.h"
#include "a2l_varmeas.h"
#include "a2l_varchar.h"
#include "funpamdlg.h"


namespace Ui {
class TableWin;
}

class TableWin : public QMainWindow
{
    Q_OBJECT

public:
    explicit TableWin(QWidget *parent = nullptr, QString name = "", int mode = 0);
    ~TableWin();

    void setWinName(const QString &value);

    QString getWinName() const;

    void setFunMeasListHash(const QHash<QString, QList<A2L_VarMeas *> > &value);

    void setFunCharListHash(const QHash<QString, QList<A2L_VarChar *> > &value);

    void hideMenu();

    QList<PamInfo> getMeasPams() const;

    QList<PamInfo> getCharPams() const;

    void setMeasPams(const QList<PamInfo> &value);

    void setCharPams(const QList<PamInfo> &value);

    void setReadPams(const QList<PamInfo> &value);

    QList<PamInfo> getReadPams() const;

    QList<PamInfo> getWritePams() const;
    void setWritePams(const QList<PamInfo> &value);

public slots:
    void Slt_Trigger(bool active);
    void Slt_RunActive(bool runActive);

signals:
    void winDeleted(QWidget *win);
    void winUpdated();

    void charValueUpdated(A2L_VarChar*, qreal);
    void pamValueUpdated(PARAM*, qreal);

private slots:
    void Slt_ContextMenuRequest(QPoint pos);
    void Slt_ModelDataUpdated(int index, double value);
    //void Slt_DoubleClicked(QModelIndex index);

    void Slt_Updated();

    void Slt_AddMeas(QList<A2L_VarMeas *> pams);
    void Slt_AddChar(QList<A2L_VarChar *> pams);

    void Slt_AddParams(QList<PARAM*> pams);
    void Slt_AddWriteParams(QList<PARAM*> pams);

    void Slt_AddPams();
    void Slt_DelPams();
    void Slt_Rename();

    void Slt_UpPam();
    void Slt_DownPam();

    void reLoadPams();
    void addMeasByPamInfo();
    void addCharsByPamInfo();

    void addPamsByPamInfo();
    void addWritePamsByPamInfo();

    void on_actionExitAndDel_triggered();

    void on_actionRename_triggered();

private:
    Ui::TableWin *ui;

    QString winName = "Table_";

    int winMode = 0;  // 0 = read, 1 = write

    QTimer *updateTimer = NULL;

    QStandardItemModel *model = NULL;

    QList<PamInfo> measPams;
    QList<A2L_VarMeas*> measPamList;

    QList<PamInfo> readPams;
    QList<PARAM*> pamList;

    QList<PamInfo> writePams;
    QList<PARAM*> writePamList;

    QList<PamInfo> charPams;
    QList<A2L_VarChar*> charPamList;

    QHash<QString, QList<A2L_VarMeas*>> funMeasListHash;
    QHash<QString, QList<A2L_VarChar*>> funCharListHash;

};

#endif // TABLEWIN_H
