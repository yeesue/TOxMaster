#include "plotwin.h"

PlotWin::PlotWin(QWidget *parent) : QMainWindow(parent)
{
    setWindowTitle("Chart");
    setGeometry(400, 250, 542, 390);
    //setAttribute(Qt::WA_DeleteOnClose);

    mPlot = new QCustomPlot(this);
    setCentralWidget(mPlot);
    mPlot->legend->setVisible(true);
    QFont legendFont = font();  // start out with MainWindow's font..
    legendFont.setPointSize(9); // and make a bit smaller for legend
    mPlot->legend->setFont(legendFont);
    mPlot->legend->setBrush(QBrush(QColor(255,255,255,230)));
    // by default, the legend is in the inset layout of the main axis rect. So this is how we access it to change legend placement:
    mPlot->axisRect()->insetLayout()->setInsetAlignment(0, Qt::AlignBottom|Qt::AlignRight);
    mPlot->axisRect()->axis(QCPAxis::atRight, 0)->setPadding(60);

    QSharedPointer<QCPAxisTickerTime> timeTicker(new QCPAxisTickerTime);
    timeTicker->setTimeFormat("%h:%m:%s");
    mPlot->xAxis->setTicker(timeTicker);
    mPlot->axisRect()->setupFullAxesBox();
    mPlot->yAxis->setRange(-10, 10);

    // make left and bottom axes transfer their ranges to right and top axes:
    connect(mPlot->xAxis, SIGNAL(rangeChanged(QCPRange)), mPlot->xAxis2, SLOT(setRange(QCPRange)));
    connect(mPlot->yAxis, SIGNAL(rangeChanged(QCPRange)), mPlot->yAxis2, SLOT(setRange(QCPRange)));

    // setup a timer that repeatedly calls MainWindow::realtimeDataSlot:
    connect(&dataTimer, SIGNAL(timeout()), this, SLOT(realtimeDataSlot()));
    dataTimer.start(100);    

    mPlot->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(mPlot, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(contextMenuRequest(QPoint)));
}

PlotWin::~PlotWin()
{
    delete mPlot;

}


void PlotWin::selectPam()
{
    pamListDlg *pamDlg = new pamListDlg;
    pamDlg->setAttribute(Qt::WA_DeleteOnClose);
    pamDlg->show();

    connect(pamDlg, SIGNAL(pamFirstSelected(PARAM*)), this, SLOT(setPam(PARAM*)));
}

void PlotWin::addPamSlot()
{
    FunPamDlg *pamDlg = new FunPamDlg();
    pamDlg->setAttribute(Qt::WA_DeleteOnClose);
    pamDlg->show();

    connect(pamDlg, SIGNAL(pamListSelected(QList<PARAM*>)), this, SLOT(addPams(QList<PARAM*>)));
}

void PlotWin::propPamSlot()
{
    PamPropDlg *pamDlg = new PamPropDlg();
    pamDlg->setPamAttrList(pamAttrList);
    pamDlg->setPams(refPamList);    
    pamDlg->setChartProp(chartProp);
    pamDlg->show();

    //connect(pamDlg, SIGNAL(pamsRemoved(QList<PARAM*>)), this, SLOT(removePamsHandle(QList<PARAM*>)));

    connect(pamDlg, SIGNAL(pamEditFinished(QList<PARAM*>)), this, SLOT(setPams(QList<PARAM*>)));
    connect(pamDlg, SIGNAL(chartPropUpdated(ChartProp)), this, SLOT(chartPropUpdatedSlot(ChartProp)));
    connect(pamDlg, SIGNAL(pamsAttrUpdated(QList<PAMChart>)), this, SLOT(pamsAttrUpdatedSlot(QList<PAMChart>)));
}

void PlotWin::chartFreezeSlot()
{
    if(dataTimer.isActive())
    {
        dataTimer.stop();
    }
    else
    {
        dataTimer.start(100);
    }
}

void PlotWin::Slt_WinDelAndExit()
{
    emit winDeleted(this);
}

void PlotWin::setPams(QList<PARAM *> pams)
{ 
    mPlot->clearGraphs();

    int axisCount = mPlot->axisRect()->axisCount(QCPAxis::atLeft);
    qDebug()<<"axisCount:"<<axisCount;

    for(int i = refPamList.count()-1; i >= 0; i--)
    {
        if(i <= 0)
            break;

        qDebug()<<"delete axis:"<<i;

        QCPAxis *axis = mPlot->axisRect()->axis(QCPAxis::atLeft, i);
        if(axis)
            mPlot->axisRect()->removeAxis(axis);

    }

    qDebug()<<"remove axis done.";
    //qDeleteAll(tagList);
    tagList.clear();

    qDebug()<<"remove tags done.";
    refPamList.clear();

    measPams.clear();

    addPams(pams);

}

void PlotWin::addPams(QList<PARAM*> pams)
{
    if(refPamList.count() == maxLine || pams.isEmpty())
        return;

    int leftSize = maxLine - refPamList.count();
    while(pams.count() > leftSize)
    {
        pams.removeLast();
    }

    refPamList.append(pams);

    foreach (PARAM *pam, pams) {
        addGraphForPam(pam, refPamList.indexOf(pam));

        PAMChart pamChart;
        pamChart.refPam = pam;
        pamChart.visible = true;
        pamChart.autoScale = true;

        pamAttrList.append(pamChart);

        PamInfo pamInfo;
        pamInfo.modName = pam->smKey;
        pamInfo.pamName = pam->Name;

        measPams.append(pamInfo);
    }


}

void PlotWin::addGraphForPam(PARAM *pam, int index)
{
    if(index > 0)
    {
        mPlot->axisRect()->addAxis(QCPAxis::atLeft);
    }

    mPlot->addGraph(mPlot->xAxis, mPlot->axisRect()->axis(QCPAxis::atLeft, index));
    mPlot->graph(index)->setPen(QPen(LineColorList.at(index)));
    mPlot->graph(index)->setName(pam->Name);

    mPlot->axisRect()->axis(QCPAxis::atLeft,index)->setBasePen(mPlot->graph(index)->pen());

    AxisTag *mTag = new AxisTag(mPlot->graph(index)->valueAxis());

    mTag->setPen(mPlot->graph(index)->pen());
    tagList.append(mTag);
}

void PlotWin::removePamsHandle(QList<PARAM *> pams)
{
    if(pams.isEmpty())
        return;
    foreach (PARAM *pam, pams) {
        int index = this->refPamList.indexOf(pam);
        removeGraphForPam(pam, index);
    }
}

void PlotWin::removeGraphForPam(PARAM *pam, int index)
{
    mPlot->removeGraph(index);
    QCPAxis *axis = mPlot->axisRect()->axis(QCPAxis::atLeft, index);
    mPlot->axisRect()->removeAxis(axis);

    AxisTag *tag = tagList.at(index);
    this->tagList.removeAt(index);
    delete tag;

    this->pamAttrList.removeAt(index);
    this->refPamList.removeAt(index);
    this->measPams.removeAt(index);
}

void PlotWin::pamValueUpdatedSlot(qreal value)
{
    static QTime timeStart = QTime::currentTime();
    // calculate two new data points:
    double key = timeStart.msecsTo(QTime::currentTime())/1000.0; // time elapsed since start of demo, in seconds
    static double lastPointKey = 0;
    if (key-lastPointKey > 0.002) // at most add point every 2 ms
    {
      // add data to lines:
      mPlot->graph(0)->addData(key, value);

      // rescale value (vertical) axis to fit the current data:
      mPlot->graph(0)->rescaleValueAxis();

      lastPointKey = key;
    }
    // make key axis range scroll with the data (at a constant range size of 8):
    mPlot->xAxis->setRange(key, 8, Qt::AlignRight);
    mPlot->replot();
}

void PlotWin::realtimeDataSlot()
{
    static QTime timeStart = QTime::currentTime();
    // calculate two new data points:
    double key = timeStart.msecsTo(QTime::currentTime())/1000.0; // time elapsed since start of demo, in seconds
    static double lastPointKey = 0;
    if (key-lastPointKey > 0.002) // at most add point every 2 ms
    {
      for(int i = 0; i < refPamList.count(); i++)
      {
          PARAM *measPam = refPamList.at(i);
          if(!measPam)
              continue;
          PAMChart pamAtrr = pamAttrList.at(i);

          double value = measPam->getValue();

          tagList.at(i)->setText(QString::number(value, 'f', 1));
          tagList.at(i)->updatePosition(value);

          // add data to lines:
          mPlot->graph(i)->addData(key, value);

          if(pamAtrr.autoScale)
          {
              // rescale value (vertical) axis to fit the current data:
              mPlot->graph(i)->rescaleValueAxis();
          }
          else
          {

          }

      }
      lastPointKey = key;
    }
    // make key axis range scroll with the data (at a constant range size of 8):
    mPlot->xAxis->setRange(key, chartProp.timeGap, Qt::AlignRight);
    mPlot->replot();
}

void PlotWin::chartPropUpdatedSlot(ChartProp prop)
{
    setChartProp(prop);
}

void PlotWin::pamsAttrUpdatedSlot(QList<PAMChart> pamAttrs)
{
    this->pamAttrList = pamAttrs;

    for(int i = 0; i < pamAttrList.count(); i++)
    {
        PAMChart pamAttr = pamAttrList.at(i);
        if(mPlot->graphCount() <= i)
            break;

        mPlot->graph(i)->setVisible(pamAttr.visible);
        if(!pamAttr.autoScale)
        {
            QCPAxis *axis = mPlot->axisRect()->axis(QCPAxis::atLeft, i);
            axis->setRange(pamAttr.minYAxis, pamAttr.maxYAxis);
        }
    }
}

void PlotWin::setPlotBg(int style)
{
    if(style == 1)
    {
        // set dark background gradient:
        QLinearGradient gradient(0, 0, 0, 400);
        gradient.setColorAt(0, QColor(90, 90, 90));
        gradient.setColorAt(0.38, QColor(105, 105, 105));
        gradient.setColorAt(1, QColor(70, 70, 70));
        mPlot->setBackground(QBrush(gradient));

        mPlot->xAxis->setBasePen(QPen(Qt::white));
        mPlot->xAxis->setTickPen(QPen(Qt::white));
        mPlot->xAxis->grid()->setVisible(true);
        mPlot->xAxis->grid()->setPen(QPen(QColor(130, 130, 130), 0, Qt::DotLine));
        mPlot->xAxis->setTickLabelColor(Qt::white);
        mPlot->xAxis->setLabelColor(Qt::white);
    }
    else if(style == 0)
    {
        // set dark background gradient:
        QLinearGradient gradient(0, 0, 0, 400);
        gradient.setColorAt(0, QColor(255, 255, 255));
        gradient.setColorAt(0.38, QColor(255, 255, 255));
        gradient.setColorAt(1, QColor(255, 255, 255));
        mPlot->setBackground(QBrush(gradient));

        mPlot->xAxis->setBasePen(QPen(Qt::black));
        mPlot->xAxis->setTickPen(QPen(Qt::black));
        mPlot->xAxis->grid()->setVisible(true);
        mPlot->xAxis->grid()->setPen(QPen(QColor(130, 130, 130), 0, Qt::DotLine));
        mPlot->xAxis->setTickLabelColor(Qt::black);
        mPlot->xAxis->setLabelColor(Qt::black);
    }
}


void PlotWin::setChartProp(const ChartProp &value)
{
    if(value.style != chartProp.style)
    {

    }

    chartProp = value;
}

void PlotWin::contextMenuRequest(QPoint pos)
{
    QMenu *menu = new QMenu(this);
    menu->setAttribute(Qt::WA_DeleteOnClose);

    if (mPlot->legend->selectTest(pos, false) >= 0) // context menu on legend requested
    {
      menu->addAction("Move to top left", this, SLOT(moveLegend()))->setData((int)(Qt::AlignTop|Qt::AlignLeft));
      menu->addAction("Move to top center", this, SLOT(moveLegend()))->setData((int)(Qt::AlignTop|Qt::AlignHCenter));
      menu->addAction("Move to top right", this, SLOT(moveLegend()))->setData((int)(Qt::AlignTop|Qt::AlignRight));
      menu->addAction("Move to bottom right", this, SLOT(moveLegend()))->setData((int)(Qt::AlignBottom|Qt::AlignRight));
      menu->addAction("Move to bottom left", this, SLOT(moveLegend()))->setData((int)(Qt::AlignBottom|Qt::AlignLeft));
    } else  // general context menu on graphs requested
    {
      menu->addAction("Add Pams", this, SLOT(addPamSlot()));
      menu->addAction("Property", this, SLOT(propPamSlot()));
      menu->addAction("FreezeOrRun", this, SLOT(chartFreezeSlot()));

      menu->addAction("WinDelAndExit", this, SLOT(Slt_WinDelAndExit()));
    }

    menu->popup(mPlot->mapToGlobal(pos));
}

void PlotWin::moveLegend()
{
  if (QAction* contextAction = qobject_cast<QAction*>(sender())) // make sure this slot is really called by a context menu action, so it carries the data we need
  {
    bool ok;
    int dataInt = contextAction->data().toInt(&ok);
    if (ok)
    {
      mPlot->axisRect()->insetLayout()->setInsetAlignment(0, (Qt::Alignment)dataInt);
      mPlot->replot();
    }
  }
}

void PlotWin::reLoadPams()
{
    if(measPams.isEmpty())
        return;
    addMeasByPamInfo();
}

void PlotWin::addMeasByPamInfo()
{
    QList<PARAM*> pamList;
    for(int i = 0; i < measPams.count(); i++)
    {
        PamInfo pamInfo = measPams.at(i);

        QList<PARAM*> pams = Globals::funPamListHash.value(pamInfo.modName);

        foreach (PARAM *pam, pams) {
            if(pam->Name == pamInfo.pamName)
            {
                pamList.append(pam);
                continue;
            }
        }
    }

    setPams(pamList);
}

QList<PamInfo> PlotWin::getMeasPams() const
{
    return measPams;
}

void PlotWin::setMeasPams(const QList<PamInfo> &value)
{
    measPams = value;
}

QString PlotWin::getWinName() const
{
    return winName;
}

void PlotWin::setWinName(const QString &value)
{
    winName = value;
    setWindowTitle(winName);
}

void PlotWin::Slt_Triggered(bool active)
{
    this->show();
}

void PlotWin::Slt_RunActive(bool runActive)
{
    if(runActive)
    {
        reLoadPams();
    }
}
