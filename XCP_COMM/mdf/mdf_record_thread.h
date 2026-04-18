#ifndef MDF_RECORD_THREAD_H
#define MDF_RECORD_THREAD_H

#include <QObject>
#include "mdf/mdf3.h"
#include <QTime>
#include <QTimer>
#include <QFile>
#include <QDataStream>
#include "common/smart_ptr.h"

struct RecordStruct {
    QString recordFileName = "mdf_data";
    bool cyclicRecord = false;
    quint32 cycleNum = 0;
    quint32 singleCycleTime_s = 60;
};

class MDF_Record_Thread : public QObject
{
    Q_OBJECT
public:
    MDF_Record_Thread(QObject *parent = 0);
    ~MDF_Record_Thread();

    void addDgPams(QString dgName, QList<PARAM*> pams, quint32 dgBlockSize);

    void setPamsHash(const QHash<quint16, QList<PARAM *> > &value);

    void setDaqSmHash(const QHash<quint16, QSharedMemory *> &value);

    void setPamsBlockSizeHash(const QHash<quint16, quint32> &value);

    void setPollPams(const QList<PARAM *> &value);

    bool getRecord_on() const;

    bool getRecordFinished() const;

    void setWorkName(const QString &value);

    void setRcdSet(const RecordStruct &value);

public slots:
    bool isRecording();
    void setRecordFileName(QString name);
    QString getRecordFileFullPath();

    void setRecordStatus_v2(bool recordFlag);
    void setPollDataBlockSize(int size);

    void mdf_record_slot_v2(ByteArrayPtr buf, quint32 size, QString dgName);
    void mdf_record_slot_poll(ByteArrayPtr buf, quint32 size, int pollIndex);
    void mdf_record_slot_raw(ByteArrayPtr buf, quint32 size, QString dgName);

private slots:
    bool mdf_record_start_v2();
    bool mdf_record_stop_v2();

    void recordTimeUpdated();
    void Slt_cycleRcdMonitor();

protected:
    //void run();

signals:
    void recordTime(QString timeStr);
    void cycleNumUpdated(quint32 cycleNum);

private:
    void loadRecordSetting(QString workName);

private:

    QString workName = "";
    RecordStruct rcdSet;

    QString mdf3FileStorePath;
    QString mdf3FileTempPath;
    QString mdf3FileName;
    QString mdf3FileFullName;
    QString mdf3TempFileName;
    MDF3 *mdf3_ins = NULL;
    bool record_on = false;
    bool recordFinished = false;

    QTime time;
    QTimer *timer = NULL;

    QTimer *cycleRcdTimer = NULL;
    quint32 curCycle = 1;
    bool cycleTransition = false;

    QList<PARAM*> pollPams;
    QFile *mdf3TempFile_poll = NULL;
    QDataStream *out_poll = NULL;
    quint32 numOfRecord_poll = 0;
    QHash<int, quint64> pamPosHash;

    qreal startRecordTime = 0;


    quint64 timeStamp_on = 0;  // 100ns  gap
    quint64 timeStamp_start = 0;
    qreal time_on = 0;            // 1s gap

    QSharedMemory *smRead = NULL;
    int sizeOfRecordBlock = 0;

    QList<PARAM*> allPams;

    QHash<QString, QList<PARAM*>> dgPamsHash;
    QHash<QString, quint32> dgBlockSizeHash;
    QHash<QString, QString> dgTempFilePathHash;
    QHash<QString, QFile*> dgTempFileHash;
    QHash<QString, QDataStream*> dgTempStreamHash;
    QHash<QString, quint32> dgNumRecordHash;
    QStringList dgNameList;

    QHash<quint16, QList<PARAM*>> pamsHash;  //Hash<daqList, pams>
    QHash<quint16, quint32> recordBlockSizeHash;
    QHash<quint16, QSharedMemory*> daqSmHash;
    QHash<quint16, QString> tempFilePathHash;
    QHash<quint16, QFile*> tempFileHash;
    QHash<quint16, QDataStream*> tempStreamHash;
    QHash<quint16, quint32> numRecordHash;
    QList<quint16> dgList;

    quint8 dgCount = 0;
};

#endif // MDF_RECORD_THREAD_H
