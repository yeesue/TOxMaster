#ifndef WIN_WT5000_H
#define WIN_WT5000_H

#include <QMainWindow>
#include <QXmlStreamWriter>
#include <QSharedMemory>
#include <QStandardItemModel>

namespace Ui {
class WIN_WT5000;
}

class WT5000Thread;
class WIN_WT5000 : public QMainWindow
{
    Q_OBJECT

public:
    explicit WIN_WT5000(QWidget *parent = 0);
    ~WIN_WT5000();

private slots:
    void dataUpdatedSlot();

private:
    Ui::WIN_WT5000 *ui;
    WT5000Thread *thread;
    //QXmlStreamWriter *stream;
    QSharedMemory *sm_wt5000 = NULL;
    QTimer *timer = NULL;

    QStandardItemModel *pamModel = NULL;
    int pamCount = 48;
    QStringList pamNameList;

    void init();
    void createConnections();
    void connectDevice();
    void disconnectDevice();
    void reconnectDevice();

    void writeSetting();
    void readSetting();

    void createSM();
    void deleteSM();

    void initTable();
    void fromRawDataToPamValue(char *data);
    void reverseData_4bytes(char *src, char *dst);

};

#endif // WIN_WT5000_H
