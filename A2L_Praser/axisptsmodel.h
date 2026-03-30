#ifndef AXISPTSMODEL_H
#define AXISPTSMODEL_H

#include <QAbstractTableModel>
#include <Nodes/axis_pts.h>

class AxisPtsModel : public QAbstractTableModel
{
public:
    AxisPtsModel(QObject *parent = 0);
    ~AxisPtsModel();

    void setList(QList<Node*> labelList);

    int rowCount(const QModelIndex &parent) const;
    int columnCount(const QModelIndex &parent) const;
    QVariant data(const QModelIndex &index, int role) const;
    QVariant headerData(int section, Qt::Orientation orientation, int role) const;
    Qt::ItemFlags flags(const QModelIndex &index) const;

private:
    QList<Node*> listAxisPts;
    int nColumn;
    int nRow;
};

#endif // AXISPTSMODEL_H
