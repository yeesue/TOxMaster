#ifndef MAPWIN_H
#define MAPWIN_H

#include <QMainWindow>
#include <QLineEdit>
#include <QTableWidgetItem>
#include <QStandardItemModel>
#include <QTableView>
#include <QHeaderView>
#include "a2l_varchar.h"
#include "dsbdelegate.h"


namespace Ui {
class MapWin;
}

class MapWin : public QMainWindow
{
    Q_OBJECT

public:
    explicit MapWin(QWidget *parent = 0);
    ~MapWin();


    void setMapList(QList<A2L_VarChar*> list);
    void setXcpConnected(bool connected);

    void setXcpFunMapListHash(const QHash<QString, QList<A2L_VarChar *> > &value);

    void setXcpFunMapSmHash(const QHash<QString, QHash<A2L_VarChar *, QSharedMemory *> > &value);

public slots:
    void currentFunTextChangedSlot(QString text);
    void currentMapIndexChangedSlot(int index);

    void updateMapList(QList<A2L_VarChar*> list);
    void setMapSmHash(QHash<A2L_VarChar*, QSharedMemory*> mapSMHash);

    void updateMapValueInSM(A2L_VarChar *map, quint16 index_x, quint16 index_y, char *data);

    void modelDataUpdatedSlot(int row,int col, double value);

signals:

private slots:
    void curMapDataUpdatedSlot(int row, int col, double value);

    void transferPhyValueToRawData(A2L_VarChar *charVar, double value, char *data);

private:
    Ui::MapWin *ui;

    bool xcpConnected = false;

    QStringList xcpFunNameList;

    QHash<QString, QList<A2L_VarChar*>> xcpFunMapListHash;
    QHash<QString, QHash<A2L_VarChar*, QSharedMemory*>> xcpFunMapSmHash;

    QList<A2L_VarChar*> mapList;

    A2L_VarChar *curMap = NULL;

    QHash<A2L_VarChar*, QSharedMemory*> mapSmHash;

    QLineEdit *input = NULL;
    quint16 curRow = 0;
    quint16 curCol = 0;
    float updateValue = 0;

    QStandardItemModel *model = NULL;
    DSBDelegate *delegate = NULL;

    QStringList dataTypeEnum = {"UBYTE", "SBYTE", "UWORD", "SWORD",
                               "ULONG", "SLONG", "A_UINT64", "A_INT64",
                               "FLOAT32_IEEE", "FLOAT64_IEEE"};

};

#endif // MAPWIN_H
