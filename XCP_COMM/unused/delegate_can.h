
#ifndef DELEGATE_CAN_H
#define DELEGATE_CAN_H

#include <QStyledItemDelegate>

//! [0]
class DSBDelegateCAN : public QStyledItemDelegate
{
    Q_OBJECT

public:
    DSBDelegateCAN(QObject *parent = 0);

    QWidget *createEditor(QWidget *parent, const QStyleOptionViewItem &option,
                          const QModelIndex &index) const override;

    void setEditorData(QWidget *editor, const QModelIndex &index) const override;
    void setModelData(QWidget *editor, QAbstractItemModel *model,
                      const QModelIndex &index) const override;

    void updateEditorGeometry(QWidget *editor,
        const QStyleOptionViewItem &option, const QModelIndex &index) const override;

signals:
    void modelDataUpdated(QModelIndex rowIndex, double value) const;

};
//! [0]


#endif
