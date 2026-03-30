#ifndef XML_GEN_H
#define XML_GEN_H

#include <QObject>
#include <QWidget>
#include <QDir>
#include <QMessageBox>
#include <QDomDocument>
#include <QTextStream>
#include <QFile>
#include "config.h"
#include "nixnet_thread.h"

class XML_GEN : public QObject
{
    Q_OBJECT
public:
    explicit XML_GEN(QObject *parent = 0);

    bool genXML();
    bool readXML();


    QString getSel_frames_r() const;
    void setSel_frames_r(const QString &value);

    QString getSel_frames_w() const;
    void setSel_frames_w(const QString &value);

    QString getSel_sigs_r() const;
    void setSel_sigs_r(const QString &value);

    QString getSel_sigs_w() const;
    void setSel_sigs_w(const QString &value);

    void setXmlFilePath(const QString &value);

private:
    QString xmlFilePath = "";
    QString xmlFileName = "";

    bool isArxml = false;

    QString sel_arxmlPath = "";
    QString sel_arxmlPath_ref = "";
    QString sel_arCluster = "";
    QString sel_dbcPath = "";
    QString sel_dbcPath_ref = "";
    QString sel_appDir = "";
    QString sel_cluster = "";
    QString sel_interface = "";
    u64 sel_baudrate = 0;
    u32 sel_canio_mode = 0;
    u64 sel_canfd_baudrate = 0;
    u32 canTermination = 0;
    u32 customBD = 0;
    u32 customFDBD = 0;
    qreal samplePoint_BD = 87.5;
    qreal samplePoint_FDBD = 87.5;

    QString sel_frames_r = "";
    QString sel_frames_w = "";
    QString sel_sigs_r = "";
    QString sel_sigs_w = "";

    QList<dbc_frame*> dbc_frames_r;
    QList<dbc_frame*> dbc_frames_w;

signals:
    void readyForLoadDBC(QString);
    void readyForLoadArxml(QString);

    void readyForLoadStoredMsg();

public slots:

    void setIsArxml(bool value);
    void setArxmlPath(QString path);
    void setArxmlRefPath(QString refPath);
    void setArCluster(QString cluster);
    void setDbcPath(QString path);
    void setDbcRefPath(QString refPath);
    void setCluster(QString cluster);
    void setInterface(QString intf);
    void setBaudrate(u64 baudrate);
    void setCANIOMode(u32 canioMode);
    void setCANFDBaudrate(u64 canfdBaudrate);
    void setCANTermination(u32 Termination);
    void setCustomBD(u32 customBD);
    void setCustomFDBD(u32 customFDBD);
    void setSamplePoint_BD(qreal samplePoint);
    void setSamplePoint_FDBD(qreal samplePoint);
    void setFramesForRead(QList<dbc_frame*> frames);
    void setFramesForWrite(QList<dbc_frame*> frames);


    bool getIsArxml();
    QString getArxmlPath();
    QString getArxmlRefPath();
    QString getArCluster();
    QString getDbcPath();
    QString getDbcRefPath();
    QString getCluster();
    QString getInterface();
    u64 getBaudrate();
    u32 getCANIOMode();
    u64 getCANFDBaudrate();
    u32 getCANTermination();
    u32 getCustomBD();
    u32 getCustomFDBD();
    qreal getSamplePoint_BD();
    qreal getSamplePoint_FDBD();
    QList<dbc_frame *> getFramesForRead();
    QList<dbc_frame*> getFramesForWrite();
};

#endif // XML_GEN_H
