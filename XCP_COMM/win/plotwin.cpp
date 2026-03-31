#include "plotwin.h"

PlotWin::PlotWin(QWidget *parent) : QMainWindow(parent)
{
    setWindowTitle("Chart");
    setGeometry(400, 250, 542, 390);

    mPlot = new QCustomPlot(this);
    setCentralWidget(mPlot);

    //mPlot->setOpenGl(true);
    //qDebug()<<"opengl open "<<mPlot->openGl();

    mPlot->legend->setVisible(true);
    QFont legendFont = QFont("Arial");  // start out with MainWindow's font..
    legendFont.setPointSize(7); // and make a bit smaller for legend
    mPlot->legend->setFont(legendFont);
    mPlot->legend->setBrush(QBrush(QColor(255,255,255,160)));
    mPlot->legend->setRowSpacing(-3);
    mPlot->legend->setMargins(QMargins(2, 2, 2, 2));

    mPlot->legend->setFillOrder(QCPLayoutGrid::foColumnsFirst);
    mPlot->plotLayout()->addElement(1, 0, mPlot->legend);
    mPlot->plotLayout()->setRowStretchFactor(1, 0.001);
    mPlot->legend->setBorderPen(Qt::NoPen);

    mPlot->showMaximized();

    //设置基本坐标轴（左侧Y轴和下方X轴）可拖动、可缩放、曲线可选、legend可选、设置伸缩比例，使所有图例可见
    //mPlot->setInteractions(QCP::iRangeDrag|QCP::iRangeZoom| QCP::iSelectAxes | QCP::iSelectLegend | QCP::iSelectPlottables);
    //设置legend只能选择图例
    //mPlot->legend->setSelectableParts(QCPLegend::spItems);
    //connect(mPlot, SIGNAL(selectionChangedByUser()), this, SLOT(selectionChanged()));

    // by default, the legend is in the inset layout of the main axis rect. So this is how we access it to change legend placement:
    //mPlot->axisRect()->insetLayout()->setInsetAlignment(0, Qt::AlignTop|Qt::AlignLeft);
    //mPlot->axisRect()->axis(QCPAxis::atRight, 0)->setPadding(0);

    QSharedPointer<QCPAxisTickerTime> timeTicker(new QCPAxisTickerTime);
    timeTicker->setTimeFormat("%h:%m:%s");
    mPlot->xAxis->setTicker(timeTicker);
    mPlot->axisRect()->setupFullAxesBox();
    mPlot->yAxis->setRange(-10, 10);

    QFont labelFont = legendFont;
    labelFont.setPointSize(8);
    mPlot->setFont(labelFont);
    mPlot->xAxis->setTickLabelFont(labelFont);
    mPlot->yAxis->setTickLabelFont(labelFont);

    // make left and bottom axes transfer their ranges to right and top axes:
    connect(mPlot->xAxis, &QCPAxis::rangeChanged, mPlot->xAxis2, &QCPAxis::setRange);
    connect(mPlot->yAxis, &QCPAxis::rangeChanged, mPlot->yAxis2, &QCPAxis::setRange);

    // setup a timer that repeatedly calls MainWindow::realtimeDataSlot:
    //connect(&dataTimer, SIGNAL(timeout()), this, SLOT(realtimeDataSlot()));
    //dataTimer.start(100);

    chartThread = new ChartThread(this);
    connect(chartThread, &ChartThread::dataUpdated, this, &PlotWin::Slt_chartDataUpdated);
    chartThread->start();

    mPlot->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(mPlot, &QCustomPlot::customContextMenuRequested, this, &PlotWin::contextMenuRequest);
}

PlotWin::~PlotWin()
{
    if(chartThread)
    {
        chartThread->setIsStop(true);
        chartThread->quit();
        chartThread->wait();

        delete chartThread;
        chartThread = NULL;
    }
    delete mPlot;
}


void PlotWin::selectPam()
{
    pamListDlg *pamDlg = new pamListDlg;
    pamDlg->setAttribute(Qt::WA_DeleteOnClose);
    pamDlg->show();

    connect(pamDlg, &pamListDlg::pamFirstSelected, this, &PlotWin::setPam);
}

void PlotWin::addPamSlot()
{
    FunPamDlg *pamDlg = new FunPamDlg();
    pamDlg->setAttribute(Qt::WA_DeleteOnClose);
    pamDlg->show();

    connect(pamDlg, &pamListDlg::pamListSelected, this, &PlotWin::addPams);
}

void PlotWin::propPamSlot()
{
    PamPropDlg *pamDlg = new PamPropDlg();
    pamDlg->setPamAttrList(pamAttrList);
    pamDlg->setPams(refPamList);
    pamDlg->setChartProp(chartProp);

    //connect(pamDlg, SIGNAL(pamsRemoved(QList<PARAM*>)), this, SLOT(removePamsHandle(QList<PARAM*>)));
    connect(pamDlg, &pamListDlg::pamEditFinished, this, &PlotWin::setPams);
    connect(pamDlg, &pamListDlg::chartPropUpdated, this, &PlotWin::chartPropUpdatedSlot);
    connect(pamDlg, &pamListDlg::pamsAttrUpdated, this, &PlotWin::pamsAttrUpdatedSlot);

    pamDlg->show();
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

    //int axisCount = mPlot->axisRect()->axisCount(QCPAxis::atLeft);
    //qDebug()<<"axisCount:"<<axisCount;

    if(refPamList.count() > 0)
    {
        for(int i = refPamList.count()-1; i >= 0; i--)
        {
            if(i <= 0)
                break;

            //qDebug()<<"delete axis:"<<i;

            QCPAxis *axis = mPlot->axisRect()->axis(QCPAxis::atLeft, i);
            if(axis)
                mPlot->axisRect()->removeAxis(axis);
        }
    }

    //qDebug()<<"remove axis done.";
    //qDeleteAll(tagList);
    //tagList.clear();

    //qDebug()<<"remove tags done.";
    refPamList.clear();
    pamInfoList.clear();

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
    if(chartThread)
    {
        chartThread->setPamList(refPamList);
    }

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
        pamInfo.autoScale = pamChart.autoScale;
        pamInfo.showMin = pamChart.minYAxis;
        pamInfo.showMax = pamChart.maxYAxis;

        pamInfoList.append(pamInfo);
    }
}

void PlotWin::addGraphForPam(PARAM *pam, int index)
{
    if(!pam) return;

    if(index > 0)
    {
        mPlot->axisRect()->addAxis(QCPAxis::atLeft);
    }

    mPlot->addGraph(mPlot->xAxis, mPlot->axisRect()->axis(QCPAxis::atLeft, index));
    mPlot->graph(index)->setPen(QPen(LineColorList.at(index)));
    mPlot->graph(index)->setName(pam->Name);

    mPlot->axisRect()->axis(QCPAxis::atLeft,index)->setBasePen(mPlot->graph(index)->pen());

    //AxisTag *mTag = new AxisTag(mPlot->graph(index)->valueAxis());
    //mTag->setPen(mPlot->graph(index)->pen());
    //tagList.append(mTag);

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

    //AxisTag *tag = tagList.at(index);
    //this->tagList.removeAt(index);
    //delete tag;

    this->pamAttrList.removeAt(index);
    this->refPamList.removeAt(index);
    this->pamInfoList.removeAt(index);
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
    if (key - lastPointKey > 0.002) // at most add point every 2 ms
    {
      for(int i = 0; i < refPamList.count(); i++)
      {
          PARAM *measPam = refPamList.at(i);
          if(!measPam) continue;
          PAMChart pamAtrr = pamAttrList.at(i);
          PamInfo pamInfo = pamInfoList.at(i);

          double value = measPam->getValue();

          //tagList.at(i)->setText(QString::number(value, 'f', 1));
          //tagList.at(i)->updatePosition(value);

          // add data to lines:
          mPlot->graph(i)->addData(key, value);

          mPlot->graph(i)->setName(pamInfo.pamName + " | " + QString::number(value, 'f', 1));

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

void PlotWin::Slt_chartDataUpdated(double key, QList<double> valueList)
{
    for(int i = 0; i < valueList.count(); i++)
    {
        double value = valueList.at(i);

        PAMChart pamAtrr = pamAttrList.at(i);
        PamInfo pamInfo = pamInfoList.at(i);

        // add data to lines:
        mPlot->graph(i)->addData(key, value);

        mPlot->graph(i)->setName(pamInfo.pamName + " | " + QString::number(value, 'f', 1));

        if(pamAtrr.autoScale)
        {
            // rescale value (vertical) axis to fit the current data:
            mPlot->graph(i)->rescaleValueAxis();
        }
    }
    // make key axis range scroll with the data (at a constant range size of 8):
    mPlot->xAxis->setRange(key, chartProp.timeGap, Qt::AlignRight);
    mPlot->replot();
}

void PlotWin::chartPropUpdatedSlot(ChartProp prop)
{
    chartProp = prop;
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

        PamInfo pamInfo = pamInfoList.at(i);
        pamInfo.autoScale = pamAttr.autoScale;
        pamInfo.showMin = pamAttr.minYAxis;
        pamInfo.showMax = pamAttr.maxYAxis;
        pamInfoList.replace(i, pamInfo);
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
    if(pamInfoList.isEmpty())
        return;

    addPamsByPamInfo();

    chartPropUpdatedSlot(chartProp);
    pamsAttrUpdatedSlot(pamAttrList);

}

void PlotWin::addPamsByPamInfo()
{
    mPlot->clearGraphs();

    if(refPamList.count() > 0)
    {
        for(int i = refPamList.count()-1; i >= 0; i--)
        {
            if(i <= 0)
                break;

            QCPAxis *axis = mPlot->axisRect()->axis(QCPAxis::atLeft, i);
            if(axis)
                mPlot->axisRect()->removeAxis(axis);
        }
    }

    //tagList.clear();
    refPamList.clear();

    pamAttrList.clear();

    for(int i = 0; i < pamInfoList.count(); i++)
    {
        PamInfo pamInfo = pamInfoList.at(i);

        PAMChart pamAttr;

        QList<PARAM*> pams = Globals::funPamListHash.value(pamInfo.modName);

        foreach (PARAM *pam, pams) {
            if(pam->Name == pamInfo.pamName)
            {
                refPamList.append(pam);

                pamAttr.refPam = pam;
                pamAttr.minYAxis = pamInfo.showMin;
                pamAttr.maxYAxis = pamInfo.showMax;
                pamAttr.autoScale = pamInfo.autoScale;

                pamAttrList.append(pamAttr);

                addGraphForPam(pam, i);

                continue;
            }
        }
    }

    if(chartThread)
    {
        chartThread->setPamList(refPamList);
    }

}

void PlotWin::selectionChanged()
{
    // make top and bottom axes be selected synchronously, and handle axis and tick labels as one selectable object:
    if (mPlot->xAxis->selectedParts().testFlag(QCPAxis::spAxis) || mPlot->xAxis->selectedParts().testFlag(QCPAxis::spTickLabels) ||
        mPlot->xAxis2->selectedParts().testFlag(QCPAxis::spAxis) || mPlot->xAxis2->selectedParts().testFlag(QCPAxis::spTickLabels))
    {
      mPlot->xAxis2->setSelectedParts(QCPAxis::spAxis|QCPAxis::spTickLabels);
      mPlot->xAxis->setSelectedParts(QCPAxis::spAxis|QCPAxis::spTickLabels);
    }
    // make left and right axes be selected synchronously, and handle axis and tick labels as one selectable object:
    if (mPlot->yAxis->selectedParts().testFlag(QCPAxis::spAxis) || mPlot->yAxis->selectedParts().testFlag(QCPAxis::spTickLabels) ||
        mPlot->yAxis2->selectedParts().testFlag(QCPAxis::spAxis) || mPlot->yAxis2->selectedParts().testFlag(QCPAxis::spTickLabels))
    {
      mPlot->yAxis2->setSelectedParts(QCPAxis::spAxis|QCPAxis::spTickLabels);
      mPlot->yAxis->setSelectedParts(QCPAxis::spAxis|QCPAxis::spTickLabels);
    }

    // 将图形的选择与相应图例项的选择同步
    for (int i = 0; i < mPlot->graphCount(); ++i)
    {
      QCPGraph *graph = mPlot->graph(i);
      QCPPlottableLegendItem *item = mPlot->legend->itemWithPlottable(graph);
      if (item->selected() || graph->selected())
      {
        item->setSelected(true);
        //注意：这句需要Qcustomplot2.0系列版本
        graph->setSelection(QCPDataSelection(graph->data()->dataRange()));
        //这句1.0系列版本即可
        //graph->setSelected(true);
      }
    }
}

ChartProp PlotWin::getChartProp()
{
    return chartProp;
}

void PlotWin::setChartProp(ChartProp value)
{
    chartProp = value;
}

QList<PamInfo> PlotWin::getPamInfoList() const
{
    return pamInfoList;
}

void PlotWin::setPamInfoList(const QList<PamInfo> &value)
{
    pamInfoList = value;
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
