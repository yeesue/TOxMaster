#include "record_service.h"

#include <QMetaObject>
#include <QThread>

#include "mdf_record_thread.h"
#include "xcp_main_thread.h"
#include "xcpmaster.h"
#include "xcp_polling_thread.h"

RecordService::RecordService(QObject *parent)
    : QObject(parent)
{
}

RecordService::~RecordService()
{
    dispose();
}

void RecordService::setContext(MainWindowContext *context)
{
    m_context = context;
}

void RecordService::setRecordFileName(const QString &fileName)
{
    m_recordFileName = fileName;
}

void RecordService::setMeasToPamConverter(const MeasToPamConverter &converter)
{
    m_measToPamConverter = converter;
}

bool RecordService::startRecording(const QList<XCP_Main_Thread *> &threads)
{
    if (!initializeRecorder(threads))
        return false;

    if (m_recorder) {
        QMetaObject::invokeMethod(
            m_recorder,
            "setRecordStatus_v2",
            Qt::BlockingQueuedConnection,
            Q_ARG(bool, true));
    }

    if (m_context && m_context->recordOn) {
        *m_context->recordOn = true;
    }
    emit recordingStateChanged(true);
    return true;
}

bool RecordService::stopRecording()
{
    if (!m_recorder && !m_recordThread)
        return false;

    if (m_recorder) {
        QMetaObject::invokeMethod(
            m_recorder,
            "setRecordStatus_v2",
            Qt::BlockingQueuedConnection,
            Q_ARG(bool, false));
    }

    disconnectTrackedThreads();
    shutdownThread();

    if (m_context && m_context->recordOn) {
        *m_context->recordOn = false;
    }
    emit recordingStateChanged(false);
    return true;
}

void RecordService::dispose()
{
    stopRecording();
}

bool RecordService::isInitialized() const
{
    return m_recorder != nullptr || m_recordThread != nullptr;
}

bool RecordService::initializeRecorder(const QList<XCP_Main_Thread *> &threads)
{
    QList<XCP_Main_Thread*> validThreads;
    for (XCP_Main_Thread *thread : threads) {
        if (thread) {
            validThreads.append(thread);
        }
    }

    if (validThreads.isEmpty() || !m_measToPamConverter) {
        return false;
    }

    if (!m_recorder) {
        m_recorder = new MDF_Record_Thread();
    }

    const QString fileName =
        !m_recordFileName.isEmpty() ? m_recordFileName :
        (m_context && m_context->mdfFileName && !m_context->mdfFileName->isEmpty()
             ? *m_context->mdfFileName
             : QStringLiteral("Default_mdf"));
    m_recorder->setRecordFileName(fileName);

    bool timeSignalConnected = false;
    m_trackedThreads.clear();

    for (XCP_Main_Thread *thread : validThreads) {
        m_trackedThreads.append(thread);

        XCPMaster *xcpMaster = thread->getXcpMaster();
        if (xcpMaster) {
            const QHash<QString, QList<A2L_VarMeas*>> dgNameVarsHash = xcpMaster->getDgNameVarHash();
            const QStringList keys = dgNameVarsHash.uniqueKeys();
            for (const QString &dgName : keys) {
                quint32 blockSize = 0;
                QList<PARAM*> pams = m_measToPamConverter(dgNameVarsHash.value(dgName), blockSize);
                m_recorder->addDgPams(dgName, pams, blockSize);
            }

            connect(xcpMaster,
                    QOverload<ByteArrayPtr, quint32, QString>::of(&XCPMaster::ODTDataForRecord),
                    m_recorder,
                    &MDF_Record_Thread::mdf_record_slot_v2,
                    Qt::UniqueConnection);
        }

        XCP_Polling_Thread *xcpPollingThread = thread->getXcpPollThread();
        if (xcpPollingThread) {
            connect(xcpPollingThread,
                    QOverload<ByteArrayPtr, quint32, QString>::of(&XCP_Polling_Thread::pollDataForRecord),
                    m_recorder,
                    &MDF_Record_Thread::mdf_record_slot_raw,
                    Qt::UniqueConnection);
        }

        if (!timeSignalConnected) {
            connect(m_recorder,
                    &MDF_Record_Thread::recordTime,
                    this,
                    &RecordService::recordTimeUpdated,
                    Qt::UniqueConnection);
            timeSignalConnected = true;
        }
    }

    if (!m_recordThread) {
        m_recordThread = new QThread();
        m_recorder->moveToThread(m_recordThread);
        connect(m_recordThread, &QThread::finished, m_recorder, &QObject::deleteLater);
    }

    if (!m_recordThread->isRunning()) {
        m_recordThread->start();
    }

    return true;
}

void RecordService::disconnectTrackedThreads()
{
    if (!m_recorder)
        return;

    for (const QPointer<XCP_Main_Thread> &thread : m_trackedThreads) {
        if (!thread)
            continue;

        XCPMaster *xcpMaster = thread->getXcpMaster();
        if (xcpMaster) {
            disconnect(xcpMaster,
                       QOverload<ByteArrayPtr, quint32, QString>::of(&XCPMaster::ODTDataForRecord),
                       m_recorder,
                       &MDF_Record_Thread::mdf_record_slot_v2);
        }

        XCP_Polling_Thread *xcpPollingThread = thread->getXcpPollThread();
        if (xcpPollingThread) {
            disconnect(xcpPollingThread,
                       QOverload<ByteArrayPtr, quint32, QString>::of(&XCP_Polling_Thread::pollDataForRecord),
                       m_recorder,
                       &MDF_Record_Thread::mdf_record_slot_raw);
        }
    }

    disconnect(m_recorder, &MDF_Record_Thread::recordTime, this, &RecordService::recordTimeUpdated);
    m_trackedThreads.clear();
}

void RecordService::shutdownThread()
{
    if (!m_recordThread)
        return;

    m_recordThread->quit();
    m_recordThread->wait();

    delete m_recordThread;
    m_recordThread = nullptr;
    m_recorder = nullptr;
}
