#include "plot3dwin.h"
#include "chartthread.h"
#include "pamlistdlg.h"
#include "common.h"

Plot3DWin::Plot3DWin(QWidget *parent) : QMainWindow(parent)
{
    setWindowTitle("3D Data Visualization");
    setGeometry(100, 100, 800, 600);

    // 初始化3D表面图
    init3DSurface();

    // 创建图表线程
    chartThread = new ChartThread(this);
    connect(chartThread, &ChartThread::dataUpdated, this, &Plot3DWin::Slt_chartDataUpdated);

    // 初始化数据定时器
    connect(&dataTimer, &QTimer::timeout, this, &Plot3DWin::realtimeDataSlot);
    dataTimer.start(100); // 100ms更新一次

    // 创建菜单
    QMenuBar *menuBar = new QMenuBar(this);
    setMenuBar(menuBar);

    QMenu *fileMenu = menuBar->addMenu("File");
    QAction *exitAction = fileMenu->addAction("Exit");
    connect(exitAction, &QAction::triggered, this, &Plot3DWin::Slt_WinDelAndExit);

    QMenu *pamMenu = menuBar->addMenu("Parameters");
    QAction *selectAction = pamMenu->addAction("Select Parameters");
    connect(selectAction, &QAction::triggered, this, &Plot3DWin::selectPam);

    QAction *addAction = pamMenu->addAction("Add Parameters");
    connect(addAction, &QAction::triggered, this, &Plot3DWin::addPamSlot);

    QMenu *viewMenu = menuBar->addMenu("View");
    QAction *freezeAction = viewMenu->addAction("Freeze");
    connect(freezeAction, &QAction::triggered, this, &Plot3DWin::chartFreezeSlot);
}

Plot3DWin::~Plot3DWin()
{
    if (chartThread) {
        chartThread->setIsStop(true);
        chartThread->wait();
        delete chartThread;
    }

    if (mSurface) {
        delete mSurface;
    }
}

QString Plot3DWin::getWinName() const
{
    return winName;
}

void Plot3DWin::setWinName(const QString &value)
{
    winName = value;
    setWindowTitle(winName);
}

QList<PamInfo> Plot3DWin::getPamInfoList() const
{
    return pamInfoList;
}

void Plot3DWin::setPamInfoList(const QList<PamInfo> &value)
{
    pamInfoList = value;
    addPamsByPamInfo();
}

void Plot3DWin::init3DSurface()
{
    // 创建3D表面图
    mSurface = new QtDataVisualization::Q3DSurface();
    QWidget *container = QWidget::createWindowContainer(mSurface);

    // 设置容器属性
    container->setMinimumSize(800, 600);
    container->setMaximumSize(1600, 1200);
    container->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    container->setFocusPolicy(Qt::StrongFocus);

    // 创建数据代理和系列
    mProxy = new QtDataVisualization::QSurfaceDataProxy();
    mSeries = new QtDataVisualization::QSurface3DSeries(mProxy);
    mSeries->setItemLabelFormat("(@xLabel, @zLabel): @yLabel");
    mSeries->setMeshSmooth(true);
    mSurface->addSeries(mSeries);

    // 设置轴标签
    mSurface->axisX()->setTitle("X Axis");
    mSurface->axisY()->setTitle("Y Axis");
    mSurface->axisZ()->setTitle("Z Axis");

    // 设置窗口中心部件
    setCentralWidget(container);

    // 初始数据
    update3DData();
}

void Plot3DWin::update3DData()
{
    // 创建测试数据
    const int sampleCountX = 50;
    const int sampleCountZ = 50;
    float stepX = 8.0f / float(sampleCountX - 1);
    float stepZ = 8.0f / float(sampleCountZ - 1);

    QtDataVisualization::QSurfaceDataArray *dataArray = new QtDataVisualization::QSurfaceDataArray();
    dataArray->reserve(sampleCountZ);

    for (int i = 0; i < sampleCountZ; i++) {
        QtDataVisualization::QSurfaceDataRow *newRow = new QtDataVisualization::QSurfaceDataRow(sampleCountX);
        float z = float(i) * stepZ - 4.0f;
        for (int j = 0; j < sampleCountX; j++) {
            float x = float(j) * stepX - 4.0f;
            float y = qSin(qSqrt(x * x + z * z));
            (*newRow)[j].setPosition(QVector3D(x, y, z));
        }
        *dataArray << newRow;
    }

    mProxy->resetArray(dataArray);
}

void Plot3DWin::Slt_Triggered(bool active)
{
    if (active) {
        chartThread->start();
    } else {
        chartThread->setIsStop(true);
        chartThread->wait();
    }
}

void Plot3DWin::Slt_RunActive(bool runActive)
{
    // 处理运行状态变化
}

void Plot3DWin::selectPam()
{
    pamListDlg dlg(this);
    if (dlg.exec() == QDialog::Accepted) {
        // 获取选中的参数
        // 注意：pamListDlg没有getSelectedPams()方法，需要通过信号获取
        // 这里暂时留空，需要根据实际情况实现
    }
}

void Plot3DWin::addPamSlot()
{
    pamListDlg dlg(this);
    if (dlg.exec() == QDialog::Accepted) {
        // 获取选中的参数
        // 注意：pamListDlg没有getSelectedPams()方法，需要通过信号获取
        // 这里暂时留空，需要根据实际情况实现
    }
}

void Plot3DWin::propPamSlot()
{
    // 参数属性设置
}

void Plot3DWin::chartFreezeSlot()
{
    // 冻结图表
}

void Plot3DWin::Slt_WinDelAndExit()
{
    close();
    emit winDeleted(this);
}

void Plot3DWin::setPams(QList<PARAM*> pams)
{
    refPamList.clear();
    refPamList = pams;
    chartThread->setPamList(pams);
}

void Plot3DWin::addPams(QList<PARAM *> pams)
{
    for (PARAM *pam : pams) {
        if (!refPamList.contains(pam)) {
            refPamList.append(pam);
        }
    }
    chartThread->setPamList(refPamList);
}

void Plot3DWin::removePamsHandle(QList<PARAM *> pams)
{
    for (PARAM *pam : pams) {
        refPamList.removeAll(pam);
    }
    chartThread->setPamList(refPamList);
}

void Plot3DWin::pamValueUpdatedSlot(qreal value)
{
    // 参数值更新
}

void Plot3DWin::realtimeDataSlot()
{
    // 实时数据更新
}

void Plot3DWin::Slt_chartDataUpdated(double key, QList<double> valueList)
{
    // 处理图表数据更新
    // 这里可以根据valueList更新3D数据
}

void Plot3DWin::contextMenuRequest(QPoint pos)
{
    // 上下文菜单
}

void Plot3DWin::addPamsByPamInfo()
{
    // 根据pamInfoList添加参数
    QList<PARAM*> pams;
    for (const PamInfo &info : pamInfoList) {
        // 这里需要根据info获取对应的PARAM对象
        // 暂时留空，需要根据实际情况实现
    }
    setPams(pams);
}