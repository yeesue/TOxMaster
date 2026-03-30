#ifndef COMPUMETHODMODEL_H
#define COMPUMETHODMODEL_H

#include <QAbstractTableModel>
#include <Nodes/compu_method.h>

class CompuMethodModel : public QAbstractTableModel
{
public:
    CompuMethodModel(QObject *parent = 0);
    ~CompuMethodModel();

    void setList(QList<Node*> labelList);

    int rowCount(const QModelIndex &parent) const;
    int columnCount(const QModelIndex &parent) const;
    QVariant data(const QModelIndex &index, int role) const;
    QVariant headerData(int section, Qt::Orientation orientation, int role) const;
    Qt::ItemFlags flags(const QModelIndex &index) const;

private:
    QList<Node*> listCompuMethods;
    int nColumn;
    int nRow;
};

#endif // COMPUMETHODMODEL_H
