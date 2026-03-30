#ifndef A2LPROJECTWIN_H
#define A2LPROJECTWIN_H

#include <QMainWindow>
#include "a2l_parser_interface.h"
#include "xcp_common.h"
#include "a2l_project.h"
#include <QListWidgetItem>
//#include "xcpwin.h"
#include <QTreeWidgetItem>

namespace Ui {
class A2LProjectWin;
}

class A2LProjectWin : public QMainWindow
{
    Q_OBJECT

public:
    explicit A2LProjectWin(QWidget *parent = 0);
    ~A2LProjectWin();

    QStringList getProjectNameList();
    A2L_Project *getProjectByName(QString projectName);

    void addA2LProject(QString fullA2LName, QString fullHexName);

private slots:

    void listItemDoubleClickedSlot(QListWidgetItem* item);

    void on_actionAddA2LFile_triggered();

    void on_actionAddHEXFile_triggered();

    void on_actionDelA2LFile_triggered();

    void on_actionShowA2LWin_triggered();

protected:
    void closeEvent(QCloseEvent *event);

signals:
    void delA2LFile(QString);

    void winClosed();

private:
    Ui::A2LProjectWin *ui;

    A2L_PARSER_INTERFACE *a2lParser = NULL;
    QStringList a2lFileNameList;
    QMap<QString, QString> a2lFileMap;
    QString currentA2LFileName;

    QMap<QString, A2L_Project*> a2lProjectMap;
    A2L_Project *curA2LProject = NULL;

    QTreeWidgetItem *root = NULL;
};

#endif // A2LPROJECTWIN_H
