#include <QApplication>
#include "workspacewin.h"

int main(int argc, char *argv[])
{
    //qDebug()<<argc<<"/n";

    for(int i = 0; i < argc; i++)
    {
        //qDebug()<<argv[i]<<"/n";
    }

    QApplication a(argc, argv);

    //设置中文字体
    a.setFont(QFont("Microsoft Yahei UI", 9));

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
