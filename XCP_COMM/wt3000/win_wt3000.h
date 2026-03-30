#ifndef WIN_WT3000_H
#define WIN_WT3000_H

#include <QMainWindow>
#include <QXmlStreamWriter>
#include <QSharedMemory>
#include <QStandardItemModel>

namespace Ui {
class WIN_WT3000;
}

class WT3000Thread;
class WIN_WT3000 : public QMainWindow
{
    Q_OBJECT

public:
    explicit WIN_WT3000(QWidget *parent = 0);
    ~WIN_WT3000();

private slots:
    void dataUpdatedSlot();

private:
    Ui::WIN_WT3000 *ui;
    WT3000Thread *thread;

    QSharedMemory *sm_wt3000 = NULL;
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

    /*
    bool openDataBase();
    bool openConfigTable();
    void saveDefaultConfig();
    bool loadDefaultConfig();
    */

};

#endif // WIN_WT3000_H
