#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QProgressBar>
#include <QModelIndex>
#include "Nodes/measurement.h"
#include "Nodes/characteristic.h"
#include "Nodes/compu_method.h"
#include "Nodes/axis_pts.h"
#include "Nodes/record_layout.h"
#include "Nodes/compu_vtab.h"
#include "Nodes/memory_segment.h"

class WorkProject;
class A2lTreeModel;
class TreeModelCompleter;

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

    void writeOutput(QString str);
    void writeOutput(QStringList list);

    void readA2l(WorkProject *wp);

    bool loadAndParserA2lFile(QString fullFileName, QStringList &infoList);
    bool getParserStatus();
    QStringList getParserInfoList();

    QStringList getAllMeasNamesInA2LFile(QString fullFileName);
    QStringList getAllCharacNamesInA2LFile(QString fullFileName);

    MEASUREMENT* getMeasNodeByName(QString fullFileName, QString reqMeasName);
    QMap<QString, QString> *getMeasNodeInfoByName(QString fullFileName, QString reqMeasName);

    CHARACTERISTIC* getCharacNodeByName(QString fullFileName, QString reqCharacName);
    QMap<QString, QString> *getCharacNodeInfoByName(QString fullFileName, QString reqCharacName);

    COMPU_METHOD* getCompMNodeByName(QString fullFileName, QString reqCompMName);
    QMap<QString, QString> *getCompMNodeInfoByName(QString fullFileName, QString reqCompMName);

    RECORD_LAYOUT* getRecordLayoutNodeByName(QString fullFileName, QString reqRecordLayoutName);
    QMap<QString, QString> *getRecordLayoutNodeInfoByName(QString fullFileName, QString reqRecordLayoutName);

    AXIS_PTS* getAxisPtsNodeByName(QString fullFileName, QString reqAxisPtsName);
    QMap<QString, QString> *getAxisPtsNodeInfoByName(QString fullFileName, QString reqAxisPtsName);

    DAQ* getDaqNodeByName(QString fullFileName, QString reqDaqName);
    QMap<QString, QString> *getDaqNodeInfoByName(QString fullFileName, QString reqDaqName);

    QList<DAQ_LIST*> getDaqListNodeListByName(QString fullFileName, QString reqDaqListName);
    QMap<QString, QString> *getDaqListNodeInfoByNameAndIndex(QString fullFileName, QString reqDaqListName, int index);

    DAQ_MEMORY_CONSUMPTION* getDaqMemConsumpNodeByName(QString fullFileName, QString reqDaqMemConsumpName);
    QMap<QString, QString> *getDaqMemConsumpNodeInfoByName(QString fullFileName, QString reqDaqMemConsumpName);

    XCP_ON_CAN* getXcpOnCANNodeByName(QString fullFileName, QString reqXcpOnCANName);
    QMap<QString, QString> *getXcpOnCANNodeInfoByName(QString fullFileName, QString reqXcpOnCANName);

    QList<XCP_ON_CAN*> getXcpOnCANNodeListByName(QString fullFileName, QString reqXcpOnCANName);
    QMap<QString, QString> *getXcpOnCANNodeInfoByNameAndIndex(QString fullFileName, QString reqXcpOnCANName, int index);

    //DAQ* getDaqNodeInXcpOnCanByName(XCP_ON_CAN* xcpOnCanNode, QString reqDaqName);
    QMap<QString, QString> *getDaqNodeInfoInXcpOnCanByName(QString fullFileName, QString reqDaqName,int indexOfXcpOnCan);

    QMap<QString, QString> *getCanFdNodeInfoInXcpOnCanByName(QString fullFileName, QString reqCanFdName,int indexOfXcpOnCan);

    QList<EVENT*> getEventNodeListByName(QString fullFileName, QString reqEventName);
    QMap<QString, QString> *getEventNodeInfoByNameAndIndex(QString fullFileName, QString reqEventName, int index);

    QList<EVENT*> getEventNodeListInXcpOnCanByName(QString fullFileName, QString reqEventName, XCP_ON_CAN* xcpOnCanNode);
    QMap<QString, QString> *getEventNodeInfoInXcpOnCanByNameAndIndex(QString fullFileName, QString reqEventName, int index, int indexOfXcpOnCan);

    QMap<QString, QString> *getDaqMemConsumpNodeInfoInXcpOnCanByName(QString fullFileName, QString reqDaqMemConsumpName,int indexOfXcpOnCan);

    QList<EVENT_CAN_ID_LIST*> getEventCanIdListNodeListByName(QString fullFileName, QString reqEventCanIdListName);
    QMap<QString, QString> *getEventCanIdListNodeInfoByNameAndIndex(QString fullFileName, QString reqEventCanIdListName, int index);

    QList<DAQ_LIST_CAN_ID*> getDaqListCanIdNodeListByName(QString fullFileName, QString reqDaqListCanIdName);
    QMap<QString, QString> *getDaqListCanIdNodeInfoByNameAndIndex(QString fullFileName, QString reqDaqListCanIdName, int index);

    COMPU_VTAB* getCompVtabNodeByName(QString fullFileName, QString reqCompVtabName);
    QMap<QString, QString> *getCompVtabNodeInfoByName(QString fullFileName, QString reqCompVtabName);

    QList<MEMORY_SEGMENT*> getMemorySegmentNodeListByName(QString fullFileName, QString reqMemorySegmentName);
    QMap<QString, QString> *getMemorySegmentNodeInfoInA2LFileByIndex(QString fullFileName, QString reqMemorySegmentName, int index);

    QList<PAGE*> getPageNodeListInMemorySegmentByName(QString fullFileName, QString reqPageName, MEMORY_SEGMENT* memorySegmentNode);
    QMap<QString, QString> *getPageNodeInfoInMemorySegmentByNameAndIndex(QString fullFileName, QString reqPageName, int index, int indexOfMemorySegment);

    bool addHexFile(QString fullFileName, QString fullHexName, QStringList &infoList);
    QMap<QString, QStringList> *getCharHexValueByName(QString fullFileName, QString fullHexName, QString reqCharName, bool *_ok);


    QStringList getAllMeasParamsNames(A2lTreeModel *a2lModel);
    QStringList getAllCharacParamsNames(A2lTreeModel *a2lModel);

    void loadAndParserA2lFile(QString fullFileName, A2lTreeModel *a2lModel, QStringList &infoList);

private slots:
    void on_actionA2LOpen_triggered();
    void setValueProgressBar(int n, int max);
    void showContextMenu(QPoint);
    void showContextMenuInWpTree(QPoint);
    void resizeColumn0();
    void doubleClicked(QModelIndex);
    void doubleClicked_treeView2(QModelIndex);

    void on_actionAddHex_triggered();
    void editChar();
    void delA2lProjectSlot();


    void on_actionMeas_triggered();

    void on_actionCharacs_triggered();

private:
    Ui::MainWindow *ui;

    QMap<QString, WorkProject*> *projectList;

    QProgressBar *progBar;
    A2lTreeModel *model;
    A2lTreeModel *model_2;
    TreeModelCompleter *completer;
    QString curFile;
    enum { MaxRecentFiles = 5};
    bool parserOK = false;
    QStringList parserInfoList;

    QAction *editCharac;
    QAction *delA2lProject;

    void initToolBars();
    void createMenus();
    void createActions();

    bool openProject(QString &fileName);
    void setCurrentFile(const QString &fileName);


};

#endif // MAINWINDOW_H
