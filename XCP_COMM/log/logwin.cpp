#include "logwin.h"
#include "ui_logwin.h"
#include <QDebug>

LogWin::LogWin(QWidget *parent, bool flag) :
    QMainWindow(parent), sysLog(flag),
    ui(new Ui::LogWin)
{
    ui->setupUi(this);
    setWindowTitle("Log");

    log("***************************Log start**************************");

    if(sysLog)
    {
        connect(MsgHandlerWapper::instance(),SIGNAL(message(QtMsgType,QString)), SLOT(outputDebugMsg(QtMsgType,QString)));
    }


    qDebug()<<"Debug color test";
    qInfo()<<"Info color test";
    qWarning()<<"Warning color test";
    qCritical()<<"Critical color test";

}

LogWin::~LogWin()
{
    saveLog();
    delete ui;
}

void LogWin::log(QString str, QColor strColor)
{
    QString curTimeStr = QTime::currentTime().toString("hh:mm:ss");

    ui->textEdit_Log->append(curTimeStr + "  :  " + str);
}

void LogWin::logDoubleValue(double value)
{
    QString valueTxt = QString::number(value, 'g', 6);
    log(valueTxt);
}

void LogWin::logSaveAndClear()
{
    ui->textEdit_Log->clear();
    log("******************** log start *****************");
}

void LogWin::sleep(quint32 msec)
{
    QTime dieTime = QTime::currentTime().addMSecs(msec);
        while( QTime::currentTime() < dieTime )
            QCoreApplication::processEvents(QEventLoop::AllEvents, 100);
}

void LogWin::ReadLogInit()
{
    QString curPath = QApplication::applicationDirPath();
    QFile file (curPath + "/log/log_conf.ini");
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)){//判断文件是否可执行
        return;
    }
    while (!file.atEnd()) {
        QByteArray  strBuf = file.readLine();
        if(strBuf == "[LOG_CONFIG]\n")
        {
            strBuf = file.readLine();
            LogType = strBuf.mid(strBuf.size()-1,1).toInt();
        }
    }
}


void LogWin::MsgLog(QtMsgType type, const QMessageLogContext &context, const QString &msg)
{
    //加锁：避免对文件的同时读写
    static QMutex mutex;
    mutex.lock();
    //读写消息
    QByteArray localMsg = msg.toLocal8Bit();
    //输出的字符串
    QString strOutStream = "";
    //case 生成要求格式日志文件，加日志等级过滤
    switch (type) {
          case QtDebugMsg:
              if(LogType == Debug)
              {
                  strOutStream = QString("%1 %2 %3 %4 [Debug] %5 \n").arg(QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss")).arg(QString(context.file)).arg(context.line).arg(QString(context.function)).arg(QString(localMsg));
              }
              break;
          case QtInfoMsg:
              if(LogType >= Info)
              {
                  strOutStream = QString("%1 %2 %3 %4 [Info]: %5 \n").arg(QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss")).arg(QString(context.file)).arg(context.line).arg(QString(context.function)).arg(QString(localMsg));
              }
              break;
          case QtWarningMsg:
              if(LogType >= Warning)
              {
                  strOutStream = QString("%1 %2 %3 %4 [Warning]: %5 \n").arg(QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss")).arg(QString(context.file)).arg(context.line).arg(QString(context.function)).arg(QString(localMsg));
              }
              break;
          case QtCriticalMsg:
              if(LogType >= Critical)
              {
                  strOutStream = QString("%1 %2 %3 %4 [Critical]: %5 \n").arg(QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss")).arg(QString(context.file)).arg(context.line).arg(QString(context.function)).arg(QString(localMsg));
              }
              break;
          case QtFatalMsg:
              if(LogType >= Fatal)
              {
                  strOutStream = QString("%1 %2 %3 %4 [Fatal]: %5 \n").arg(QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss")).arg(QString(context.file)).arg(context.line).arg(QString(context.function)).arg(QString(localMsg));
              }
              abort();
          }

    //每天生成一个新的log日志文件，文件名 yyyyMMdd.txt
     QString strFileName = QString("%1.txt").arg(QDateTime::currentDateTime().date().toString("yyyyMMdd"));
     QFile logfile("./log/" + strFileName);
     logfile.open(QIODevice::WriteOnly | QIODevice::Append);
     if(strOutStream != "")
     {
         QTextStream logStream(&logfile);
         logStream<<strOutStream<<"\r\n";
     }
    //清楚缓存文件，解锁
     logfile.flush();
     logfile.close();
     mutex.unlock();

     log(strOutStream);
}


void LogWin::msgUpdateSlot(QString msg)
{
    log(msg);
}

void LogWin::outputDebugMsg(QtMsgType type, QString msg)
{
    QColor color(Qt::black);
    switch (type) {
    case QtDebugMsg:

        break;
    case QtInfoMsg:
        color = Qt::blue;
        break;
    case QtWarningMsg:
        color = QColor(255, 140, 0);
        break;
    case QtCriticalMsg:
        color = Qt::red;
    case QtFatalMsg:
        color = Qt::red;
    default:
        break;
    }
    ui->textEdit_Log->setTextColor(color);
    log(msg);
}

void LogWin::saveLog()
{
    QString appPath = QApplication::applicationDirPath();

    log("***************************Log end**************************\r\n", Qt::blue);
    //每天生成一个新的log日志文件，文件名 yyyyMMdd.txt
     QString strFileName = QString("%1.txt").arg(QDateTime::currentDateTime().date().toString("yyyyMMdd"));
     QFile logfile(appPath + "/log/" + strFileName);
     logfile.open(QIODevice::WriteOnly | QIODevice::Append);

     QString strOutStream = ui->textEdit_Log->toPlainText();
     if(strOutStream != "")
     {
         QTextStream logStream(&logfile);
         logStream<<strOutStream<<"\r\n";
     }
    //清除缓存文件
     logfile.flush();
     logfile.close();
}

void LogWin::on_actionClear_triggered()
{
    saveLog();
    ui->textEdit_Log->clear();
    log("***************************Log save and clear**************************\r\n", Qt::blue);
}
