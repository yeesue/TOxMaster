#ifndef TSCANSETDLG_H
#define TSCANSETDLG_H

#include <QDialog>
#include <QTreeWidgetItem>
#include <QDebug>
#include <QFileDialog>
#include <QDrag>
#include <QMimeData>
#include <QMouseEvent>
#include <QStandardItemModel>
#include <QStandardItem>
#include <QSettings>
#include <QMessageBox>
#include <QTableView>
#include "dbcparser.h"
#include "xml_gen.h"
#include "common.h"
#include "arxmlpraser.h"

namespace Ui {
class TsCanSetDlg;
}

class TsCanSetDlg : public QDialog
{
    Q_OBJECT

public:
    explicit TsCanSetDlg(QWidget *parent = nullptr,
                       QString work_name = "", QString fun_name = "",
                       QString intf_name = "", QString set_file_path = "");
    ~TsCanSetDlg();


    QString getCanSetFileName() const;

    QString getCanSetFilePath() const;

protected:
    void accept();

signals:
    void canSetUpdated();

private slots:

    void showClusterInTreeView(CLUSTER_ *cluster);

    void initDeviceTypeNumHash();
    void setHeader(QString filePath);
    void loadDatabase(QString dbFilePath);
    void loadDbc(QString dbcFile);

    void on_pb_loadDatabase_clicked();

    void on_pb_reset_clicked();

private:

    void setDatabaseHeader(QString filePath);

    QList<dbc_frame*> getFramesByNames(QStringList frameNameList);

    void loadMsgToModel();

    QStringList getMsgNameList(QTreeView *treeView);

    void loadFramesToReadTreeView(QList<BO_*> boList);
    void loadFramesToWriteTreeView(QList<BO_*> boList);

    BO_* getBoById(quint32 id);
    BO_* getBoByName(QString boName);
    QList<BO_*> getBoListByNames(QStringList boNameList);
    SG_* getSgByName(BO_* bo, QString sgName);

    bool saveXml();
    bool loadXml();

private:
    Ui::TsCanSetDlg *ui;

    FunStruct *fun = NULL;
    int funIndex = -1;

    QString dbcFilePath = "";
    QString dbcFilePath_ref = "";
    CLUSTER_ *m_cluster = NULL;

    QString canSetFileName = "";
    QString canSetFilePath = "";
    QString workName = "";
    QString funName = "";
    QString intfName = "";

    bool intfSettingLoad = false;
    bool praserOnArxml = false;

    TsCanConfig canConfig;

    QStringList genDbcFileList;
    QHash<QString, QString> clusterDbcHash;

    QHash<QString, CLUSTER_*> clusterHash;

    QHash<QString, quint32> deviceTypeNumHash;

    QStandardItemModel *dbcModel = NULL;
    QStandardItemModel *readModel = NULL;
    QStandardItemModel *writeModel = NULL;


    XML_GEN xml_ins;
};

#endif // TSCANSETDLG_H
