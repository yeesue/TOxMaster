#ifndef WORKER_THREAD_H
#define WORKER_THREAD_H

#include <QThread>
#include <QObject>
#include "xcpmaster.h"
#include "a2l_varmeas.h"
#include "a2l_varchar.h"

class WorkerThread : public QThread
{
    Q_OBJECT

public:
    explicit WorkerThread(QObject *parent = nullptr);
    ~WorkerThread();

    void setXcpMaster(XCPMaster *master);
    void setMeasVars(const QList<A2L_VarMeas*> &measVars);
    void setCharVars(const QList<A2L_VarChar*> &charVars);
    void setProjectName(const QString &name);

    enum TaskType {
        INIT_XCP,
        STOP_XCP,
        DOWNLOAD_CHARS,
        UPLOAD_CHARS
    };

    void setTask(TaskType task);

signals:
    void taskStarted(const QString &message);
    void taskFinished(bool success, const QString &message);
    void progressUpdated(int progress);

protected:
    void run() override;

private:
    XCPMaster *m_xcpMaster;
    QList<A2L_VarMeas*> m_measVars;
    QList<A2L_VarChar*> m_charVars;
    QString m_projectName;
    TaskType m_task;
    
    void initXCP();
    void stopXCP();
    void downloadChars();
    void uploadChars();
};

#endif // WORKER_THREAD_H