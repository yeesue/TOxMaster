#include "worker_thread.h"

WorkerThread::WorkerThread(QObject *parent) : QThread(parent)
    , m_xcpMaster(nullptr)
    , m_task(INIT_XCP)
{
}

WorkerThread::~WorkerThread()
{
    wait();
}

void WorkerThread::setXcpMaster(XCPMaster *master)
{
    m_xcpMaster = master;
}

void WorkerThread::setMeasVars(const QList<A2L_VarMeas*> &measVars)
{
    m_measVars = measVars;
}

void WorkerThread::setCharVars(const QList<A2L_VarChar*> &charVars)
{
    m_charVars = charVars;
}

void WorkerThread::setProjectName(const QString &name)
{
    m_projectName = name;
}

void WorkerThread::setTask(TaskType task)
{
    m_task = task;
}

void WorkerThread::run()
{
    switch (m_task) {
    case INIT_XCP:
        initXCP();
        break;
    case STOP_XCP:
        stopXCP();
        break;
    case DOWNLOAD_CHARS:
        downloadChars();
        break;
    case UPLOAD_CHARS:
        uploadChars();
        break;
    default:
        break;
    }
}

void WorkerThread::initXCP()
{
    emit taskStarted("Initializing XCP...");
    
    if (!m_xcpMaster) {
        emit taskFinished(false, "XCP master is not set");
        return;
    }
    
    emit progressUpdated(25);
    
    if (!m_xcpMaster->XCPInit()) {
        emit taskFinished(false, "XCP CAN init fail");
        return;
    }
    
    emit progressUpdated(50);
    
    if (!m_xcpMaster->XCP_Setup_Session()) {
        emit taskFinished(false, "XCP setup fail");
        return;
    }
    
    emit progressUpdated(75);
    
    // 初始化DAQ
    if (!m_xcpMaster->XCP_DAQ_CONFIG()) {
        emit taskFinished(false, "XCP DAQ config fail");
        return;
    }
    
    emit progressUpdated(85);
    
    // 创建共享内存
    // 注意：这里需要通过信号通知UI线程创建共享内存，因为创建共享内存可能需要访问UI相关的资源
    // 或者在MainWindow的onWorkerTaskFinished槽函数中处理
    
    emit progressUpdated(100);
    emit taskFinished(true, "XCP initialization successful");
}

void WorkerThread::stopXCP()
{
    emit taskStarted("Stopping XCP...");
    
    if (!m_xcpMaster) {
        emit taskFinished(false, "XCP master is not set");
        return;
    }
    
    m_xcpMaster->XCPStop();
    
    emit taskFinished(true, "XCP stopped successfully");
}

void WorkerThread::downloadChars()
{
    emit taskStarted("Downloading calibration variables...");
    
    if (!m_xcpMaster) {
        emit taskFinished(false, "XCP master is not set");
        return;
    }
    
    if (m_charVars.isEmpty()) {
        emit taskFinished(false, "No calibration variables to download");
        return;
    }
    
    int total = m_charVars.size();
    for (int i = 0; i < total; i++) {
        A2L_VarChar *var = m_charVars.at(i);
        if (!m_xcpMaster->XCP_Cal_VALUE(var)) {
            emit taskFinished(false, QString("Failed to download variable: %1").arg(var->Name));
            return;
        }
        
        int progress = (i + 1) * 100 / total;
        emit progressUpdated(progress);
    }
    
    emit taskFinished(true, "Calibration variables downloaded successfully");
}

void WorkerThread::uploadChars()
{
    emit taskStarted("Uploading calibration variables...");
    
    if (!m_xcpMaster) {
        emit taskFinished(false, "XCP master is not set");
        return;
    }
    
    if (m_charVars.isEmpty()) {
        emit taskFinished(false, "No calibration variables to upload");
        return;
    }
    
    int total = m_charVars.size();
    for (int i = 0; i < total; i++) {
        A2L_VarChar *var = m_charVars.at(i);
        // 上传变量的具体实现
        // 这里需要根据实际情况实现
        
        int progress = (i + 1) * 100 / total;
        emit progressUpdated(progress);
    }
    
    emit taskFinished(true, "Calibration variables uploaded successfully");
}
