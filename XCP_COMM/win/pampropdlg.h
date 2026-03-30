#ifndef PAMPROPDLG_H
#define PAMPROPDLG_H

#include <QDialog>
#include <QStandardItemModel>
#include "param.h"

namespace Ui {
class PamPropDlg;
}

struct ChartProp {
    quint32 timeGap = 10;
    int style = 0;
};

struct PAMChart {
    PARAM *refPam;
    bool visible = true;
    bool autoScale = true;
    qreal minYAxis = -1000;
    qreal maxYAxis = 1000;

};

class PamPropDlg : public QDialog
{
    Q_OBJECT

public:
    explicit PamPropDlg(QWidget *parent = 0);
    ~PamPropDlg();

    void setPams(QList<PARAM*> pams);

    QList<PAMChart> getPamAttrList() const;
    void setPamAttrList(const QList<PAMChart> &value);

    ChartProp getChartProp() const;
    void setChartProp(ChartProp value);

protected:
    void accept();

signals:
    void pamEditFinished(QList<PARAM*>);
    void pamsRemoved(QList<PARAM*>);

    void chartPropUpdated(ChartProp);
    void pamsAttrUpdated(QList<PAMChart>);

private slots:
    void on_pb_Remove_clicked();

    void saveChartProp();
    void savePamAttr();

private:
    Ui::PamPropDlg *ui;
    QList<PARAM*> pamList;
    QStandardItemModel *model = NULL;

    QList<PARAM*> removePams;

    ChartProp chartProp;

    QList<PAMChart> pamAttrList;
};

#endif // PAMPROPDLG_H
