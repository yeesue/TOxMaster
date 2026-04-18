#ifndef DATA_REPLAY_MANAGER_H
#define DATA_REPLAY_MANAGER_H

#include <QObject>
#include <QString>
#include <QStringList>
#include <QList>
#include <QDateTime>
#include <QTimer>
#include <QFile>
#include <QTextStream>
#include <memory>

// 回放状态枚举
enum class ReplayStatus {
    Stopped,
    Playing,
    Paused,
    Finished
};

// 回放速度枚举
enum class ReplaySpeed {
    Speed_0_5x,  // 0.5倍速
    Speed_1x,    // 1倍速（正常）
    Speed_2x,    // 2倍速
    Speed_5x,    // 5倍速
    Speed_10x    // 10倍速
};

// 回放数据点结构
struct ReplayDataPoint {
    QDateTime timestamp;
    QStringList paramNames;
    QList<double> paramValues;
    
    ReplayDataPoint() = default;
    ReplayDataPoint(const QDateTime& time, const QStringList& names, const QList<double>& values)
        : timestamp(time), paramNames(names), paramValues(values) {}
};

// 回放配置结构
struct ReplayConfig {
    QString filePath;
    ReplaySpeed speed = ReplaySpeed::Speed_1x;
    bool loopPlayback = false;
    qint64 startTime_ms = 0;
    qint64 endTime_ms = -1;  // -1表示播放到结束
};

class DataReplayManager : public QObject
{
    Q_OBJECT
public:
    explicit DataReplayManager(QObject *parent = nullptr);
    ~DataReplayManager();

    // 配置设置
    void setReplayConfig(const ReplayConfig& config);
    ReplayConfig getReplayConfig() const;

    // 文件操作
    bool loadDataFile(const QString& filePath);
    bool loadDataFromCSV(const QString& filePath);
    bool loadDataFromJSON(const QString& filePath);
    bool loadDataFromXML(const QString& filePath);
    void clearData();

    // 回放控制
    bool startReplay();
    void pauseReplay();
    void resumeReplay();
    void stopReplay();
    void seekTo(qint64 time_ms);
    void seekToPercent(int percent);

    // 状态查询
    ReplayStatus getStatus() const;
    bool isPlaying() const;
    bool isPaused() const;
    bool isStopped() const;

    // 进度查询
    qint64 getCurrentTime() const;
    qint64 getTotalDuration() const;
    int getProgressPercent() const;
    int getCurrentDataIndex() const;
    int getTotalDataCount() const;

    // 数据查询
    QStringList getParamNames() const;
    ReplayDataPoint getCurrentDataPoint() const;
    ReplayDataPoint getDataPointAt(int index) const;
    QList<ReplayDataPoint> getAllDataPoints() const;

    // 速度控制
    void setReplaySpeed(ReplaySpeed speed);
    ReplaySpeed getReplaySpeed() const;
    static QString speedToString(ReplaySpeed speed);
    static double speedToFactor(ReplaySpeed speed);

    // 支持的文件格式
    static QStringList getSupportedFormats();
    static QString getFormatFilter();

signals:
    void replayStarted();
    void replayPaused();
    void replayResumed();
    void replayStopped();
    void replayFinished();
    void replayError(const QString& errorMessage);
    
    void dataPointReady(const ReplayDataPoint& dataPoint);
    void progressChanged(int currentIndex, int totalCount);
    void timeChanged(qint64 currentTime_ms, qint64 totalTime_ms);
    void percentChanged(int percent);

private slots:
    void onTimerTimeout();

private:
    // 数据解析方法
    bool parseCSVLine(const QString& line, ReplayDataPoint& dataPoint);
    bool parseJSONData(const QByteArray& jsonData);
    bool parseXMLData(const QByteArray& xmlData);

    // 辅助方法
    qint64 calculateTimeInterval(int currentIndex);
    void updateCurrentDataPoint();
    QString detectFileFormat(const QString& filePath);
    QDateTime parseTimestamp(const QString& timestampStr);

private:
    ReplayConfig m_config;
    ReplayStatus m_status = ReplayStatus::Stopped;
    
    QList<ReplayDataPoint> m_dataPoints;
    QStringList m_paramNames;
    int m_currentIndex = 0;
    
    QTimer* m_replayTimer = nullptr;
    QDateTime m_startTime;
    qint64 m_elapsedTime_ms = 0;
    
    bool m_loopPlayback = false;
};

#endif // DATA_REPLAY_MANAGER_H
