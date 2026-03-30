#ifndef CANSETDLG_H
#define CANSETDLG_H

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
#include "nixnet.h"
#include "xml_gen.h"
#include "common.h"
#include "arxmlparser.h"

namespace Ui {
class CanSetDlg;
}

class CanSetDlg : public QDialog
{
    Q_OBJECT

public:
    explicit CanSetDlg(QWidget *parent = nullptr,
                       QString work_name = "", QString fun_name = "",
                       QString intf_name = "", QString set_file_path = "");
    ~CanSetDlg();


    QString getCanSetFileName() const;

    QString getCanSetFilePath() const;

protected:
    void accept();

signals:
    void canSetUpdated();

private slots:
    void clusterChangedSlot(QString clusterName);
    void showClusterInfo(dbc_Cluster *cluster);

    void loadDatabase(QString dbFilePath);
    void loadDbc(QString filePath);
    void loadArxml(QString filePath);
    dbc_Cluster* loadDbcFromArxml(QString filePath);

    void on_pb_loadDatabase_clicked();

    void on_pb_reset_clicked();

private:

    void setDatabaseHeader(QString filePath);

    QList<dbc_frame*> getFramesByNames(QStringList frameNameList);

    void loadMsgToModel();

    bool readSettingXml();
    bool writeSettingXml();

    //bool saveXml();
    //bool loadXml();

private:
    Ui::CanSetDlg *ui;

    FunStruct *fun = NULL;
    int funIndex = -1;

    QByteArray temp_path;
    QByteArray temp_name;

    char* dbcPath;
    char* dbcName;

    QString canSetFileName = "";
    QString canSetFilePath = "";
    QString workName = "";
    QString funName = "";
    QString intfName = "";

    bool intfSettingLoad = false;
    bool parserOnArxml = false;

    CanConfig canConfig;

    QStringList genDbcFileList;
    QHash<QString, QString> clusterDbcHash;
    QList<dbc_Cluster*> clusters;
    QHash<QString, dbc_Cluster*> clusterHash;

    QStandardItemModel readTreeModel;
    QStandardItemModel writeTreeModel;

    AR_XML_PACKAGES *arXmlPackages = NULL;
    QList<dbc_Cluster*> canClusters;

    XML_GEN xml_ins;
};

#endif // CANSETDLG_H
