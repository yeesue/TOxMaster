#ifndef PLOTWIN_H
#define PLOTWIN_H

#include <QMainWindow>
#include "qcustomplot.h"
#include "pamlistdlg.h"
#include "axistag.h"
#include "pampropdlg.h"
#include "common.h"
#include "funpamdlg.h"
#include "chartthread.h"

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

    QString getWinName() const;
    void setWinName(const QString &value);

    QList<PamInfo> getPamInfoList() const;
    void setPamInfoList(const QList<PamInfo> &value);


    ChartProp getChartProp();
    void setChartProp(ChartProp value);

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
    void Slt_chartDataUpdated(double key, QList<double> valueList);

    void chartPropUpdatedSlot(ChartProp prop);
    void pamsAttrUpdatedSlot(QList<PAMChart> pamAttrs);

    void setPlotBg(int style);

    void contextMenuRequest(QPoint pos);
    void moveLegend();

    void reLoadPams();
    void addPamsByPamInfo();

    void selectionChanged();

private:
    QString winName = "";

    QCustomPlot *mPlot = NULL;
    QTimer dataTimer;
    ChartThread *chartThread = NULL;

    QList<PamInfo> pamInfoList;  //用于保存显示参数信息
    QList<PARAM*> refPamList; //关联的变量
    //QList<AxisTag*> tagList;

    int maxLine = 8;
    QList<QColor> LineColorList = {QColor(0, 0, 0), QColor(40, 110, 255), QColor(255, 40, 40),
                                   QColor(225, 200, 40), QColor(40, 255, 40), QColor(220, 110, 220),
                                   QColor(110, 255, 220), QColor(160, 160, 0),};

    ChartProp chartProp;
    QList<PAMChart> pamAttrList;

};

#endif // PLOTWIN_H
