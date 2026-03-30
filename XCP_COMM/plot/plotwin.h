#ifndef PLOTWIN_H
#define PLOTWIN_H

#include <QMainWindow>
#include "qcustomplot.h"
#include "pamlistdlg.h"
#include "axistag.h"
#include "pampropdlg.h"
#include "common.h"
#include "funpamdlg.h"

enum BGStyle{
    Light,
    Dark
};

class PlotWin : public QMainWindow
{
    Q_OBJECT
public:
    explicit PlotWin(QWidget *parent = 0);
    ~PlotWin();

    void setChartProp(const ChartProp &value);

    QString getWinName() const;
    void setWinName(const QString &value);

    QList<PamInfo> getMeasPams() const;
    void setMeasPams(const QList<PamInfo> &value);

protected:


signals:
    void winUpdated();
    void winDeleted(QWidget*);

public slots:
    void Slt_Triggered(bool active);
    void Slt_RunActive(bool runActive);

private slots:
    void selectPam();
    void addPamSlot();
    void propPamSlot();
    void chartFreezeSlot();

    void Slt_WinDelAndExit();

    void setPams(QList<PARAM*> pams);
    void addPams(QList<PARAM *> pams);
    void addGraphForPam(PARAM *pam, int index);

    void removePamsHandle(QList<PARAM *> pams);
    void removeGraphForPam(PARAM *pam, int index);

    void pamValueUpdatedSlot(qreal value);
    void realtimeDataSlot();

    void chartPropUpdatedSlot(ChartProp prop);
    void pamsAttrUpdatedSlot(QList<PAMChart> pamAttrs);


    void setPlotBg(int style);

    void contextMenuRequest(QPoint pos);
    void moveLegend();

    void reLoadPams();
    void addMeasByPamInfo();

private:
    QString winName = "";

    QCustomPlot *mPlot = NULL;
    QTimer dataTimer;

    QList<PamInfo> measPams;
    QList<PARAM*> refPamList;
    QList<AxisTag*> tagList;

    int maxLine = 8;
    QList<QColor> LineColorList = {QColor(40, 110, 255), QColor(255, 40, 40),
                                   QColor(225, 200, 40), QColor(40, 255, 40),
                                   QColor(220, 110, 220)};

    ChartProp chartProp;
    QList<PAMChart> pamAttrList;
};

#endif // PLOTWIN_H
