#ifndef RECORD_SERVICE_H
#define RECORD_SERVICE_H

#include <QObject>
#include <QList>
#include <QPointer>
#include <QString>
#include <functional>

#include "mainwindow_context.h"

class QThread;
class MDF_Record_Thread;
class XCP_Main_Thread;
class A2L_VarMeas;
class PARAM;

class RecordService : public QObject
{
    Q_OBJECT

public:
    using MeasToPamConverter = std::function<QList<PARAM*>(const QList<A2L_VarMeas*>&, quint32&)>;

    explicit RecordService(QObject *parent = nullptr);
    ~RecordService();

    void setContext(MainWindowContext *context);
    void setRecordFileName(const QString &fileName);
    void setMeasToPamConverter(const MeasToPamConverter &converter);

    bool startRecording(const QList<XCP_Main_Thread*> &threads);
    bool stopRecording();
    void dispose();

    bool isInitialized() const;

signals:
    void recordTimeUpdated(const QString &time);
    void recordingStateChanged(bool active);

private:
    bool initializeRecorder(const QList<XCP_Main_Thread*> &threads);
    void disconnectTrackedThreads();
    void shutdownThread();

private:
    MainWindowContext *m_context = nullptr;
    MeasToPamConverter m_measToPamConverter;
    QString m_recordFileName;

    MDF_Record_Thread *m_recorder = nullptr;
    QThread *m_recordThread = nullptr;
    QList<QPointer<XCP_Main_Thread>> m_trackedThreads;
};

#endif // RECORD_SERVICE_H
