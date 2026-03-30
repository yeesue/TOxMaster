#ifndef COMPUVTABMODEL_H
#define COMPUVTABMODEL_H

#include <QAbstractTableModel>
#include <Nodes/compu_vtab.h>

class CompuVtabModel : public QAbstractTableModel
{
public:
    CompuVtabModel(QObject *parent = 0);
    ~CompuVtabModel();

    void setList(QList<Node*> labelList);

    int rowCount(const QModelIndex &parent) const;
    int columnCount(const QModelIndex &parent) const;
    QVariant data(const QModelIndex &index, int role) const;
    QVariant headerData(int section, Qt::Orientation orientation, int role) const;
    Qt::ItemFlags flags(const QModelIndex &index) const;

private:
    QList<Node*> listCompuVtabs;
    int nColumn;
    int nRow;
};

#endif // COMPUVTABMODEL_H
