#ifndef PLOT3DWIN_H
#define PLOT3DWIN_H

#include <QMainWindow>
#include <QTimer>
#include <QMenuBar>
#include <QMenu>
#include <QAction>
#include <QtDataVisualization/Q3DSurface>
#include <QtDataVisualization/QSurfaceDataProxy>
#include <QtDataVisualization/QSurface3DSeries>
#include "param.h"
#include "pamlistdlg.h"
#include "common.h"

class ChartThread;

class Plot3DWin : public QMainWindow
{
    Q_OBJECT
public:
    explicit Plot3DWin(QWidget *parent = 0);
    ~Plot3DWin();

    QString getWinName() const;
    void setWinName(const QString &value);

    QList<PamInfo> getPamInfoList() const;
    void setPamInfoList(const QList<PamInfo> &value);

protected:


signals:
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
    void addPamsByPamInfo();

    void removePamsHandle(QList<PARAM *> pams);

    void pamValueUpdatedSlot(qreal value);
    void realtimeDataSlot();
    void Slt_chartDataUpdated(double key, QList<double> valueList);

    void contextMenuRequest(QPoint pos);

private:
    QString winName = "";

    QtDataVisualization::Q3DSurface *mSurface = NULL;
    QtDataVisualization::QSurfaceDataProxy *mProxy = NULL;
    QtDataVisualization::QSurface3DSeries *mSeries = NULL;
    QTimer dataTimer;
    ChartThread *chartThread = NULL;

    QList<PamInfo> pamInfoList;  //用于保存显示参数信息
    QList<PARAM*> refPamList; //关联的变量

    int maxLine = 8;
    QList<QColor> LineColorList = {QColor(0, 0, 0), QColor(40, 110, 255), QColor(255, 40, 40),
                                   QColor(225, 200, 40), QColor(40, 255, 40), QColor(220, 110, 220),
                                   QColor(110, 255, 220), QColor(160, 160, 0),};

    void init3DSurface();
    void update3DData();
};

#endif // PLOT3DWIN_H