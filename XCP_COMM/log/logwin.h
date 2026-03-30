#ifndef LOGWIN_H
#define LOGWIN_H

#include <QMainWindow>
#include <QTime>
#include <QMutex>
#include <QFile>
#include <QTextStream>
#include "msghandlerwapper.h"

static enum{
    Fatal = 0,
    Critical,
    Warning,
    Info,
    Debug
}LogLeaver;

namespace Ui {
class LogWin;
}

class LogWin : public QMainWindow
{
    Q_OBJECT

public:
    explicit LogWin(QWidget *parent = 0, bool flag = false);
    ~LogWin();

public slots:

    void log(QString str = "", QColor strColor = Qt::black);
    void logDoubleValue(double value);

    void logSaveAndClear();

    void sleep(quint32 msec);

    void ReadLogInit();
    void MsgLog(QtMsgType type,const QMessageLogContext &context,const QString &msg);
    void msgUpdateSlot(QString msg);

    void outputDebugMsg(QtMsgType type, QString msg);

signals:
    void msgUpdated(QString msg);

private slots:
    void on_actionClear_triggered();

private:
    Ui::LogWin *ui;

    int LogType = 4; //日志等级初始化设置
    void saveLog();

    bool sysLog = true;  //是否在Log窗口中显示系统信息

};

#endif // LOGWIN_H
