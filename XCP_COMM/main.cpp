#include <QApplication>
#include <QIcon>
#include <QPixmap>
#include <QDebug>
#include <QTextCodec>
#include <QFile>
#include "workspacewin.h"

#ifdef Q_OS_WIN
#include <windows.h>
#endif

int main(int argc, char *argv[])
{
    // 设置控制台输出为 UTF-8（Windows）
#ifdef Q_OS_WIN
    SetConsoleOutputCP(CP_UTF8);
    SetConsoleCP(CP_UTF8);
#endif

    //qDebug()<<argc<<"/n";

    for(int i = 0; i < argc; i++)
    {
        //qDebug()<<argv[i]<<"/n";
    }

    QApplication a(argc, argv);
    
    // 初始化资源 - 必须在 QApplication 之后调用
    Q_INIT_RESOURCE(icon);
    Q_INIT_RESOURCE(icons);
    
    // 调试：测试图标资源加载
    qDebug() << "=== Icon Resource Test ===";
    QPixmap testPix(":/icon/icon/connect.png");
    qDebug() << "connect.png:" << (testPix.isNull() ? "NULL" : QString("%1x%2").arg(testPix.width()).arg(testPix.height()));
    
    QIcon testIcon(":/icon/icon/run.png");
    qDebug() << "run.png QIcon:" << (testIcon.isNull() ? "NULL" : "OK");
    
    QPixmap testPix2(":/icons/A2L.png");
    qDebug() << "A2L.png:" << (testPix2.isNull() ? "NULL" : QString("%1x%2").arg(testPix2.width()).arg(testPix2.height()));

    //设置中文字体
    a.setFont(QFont("Microsoft Yahei UI", 9));

    // 加载全局样式表
    QFile styleFile(":/icon/res/style.qss");
    if (styleFile.open(QFile::ReadOnly)) {
        QString styleSheet = QLatin1String(styleFile.readAll());
        a.setStyleSheet(styleSheet);
        styleFile.close();
        qDebug() << "Global stylesheet loaded successfully";
    } else {
        qDebug() << "Failed to load global stylesheet";
    }

    //设置中文编码
    #if (QT_VERSION <= QT_VERSION_CHECK(5,0,0))
    #if _MSC_VER
        QTextCodec *codec = QTextCodec::codecForName("gbk");
    #else
        QTextCodec *codec = QTextCodec::codecForName("utf-8");
    #endif
        QTextCodec::setCodecForLocale(codec);
        QTextCodec::setCodecForCStrings(codec);
        QTextCodec::setCodecForTr(codec);
    #else
        QTextCodec *codec = QTextCodec::codecForName("utf-8");
        QTextCodec::setCodecForLocale(codec);
    #endif

    WorkSpaceWin w;

    if(argc >= 2 && *argv[2] == '1')
    {

    }
    else
    {

    }

    w.show();

    return a.exec();
}
