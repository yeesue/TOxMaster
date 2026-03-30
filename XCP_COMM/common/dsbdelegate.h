#ifndef DSBDELEGATE_H
#define DSBDELEGATE_H

#include <QStyledItemDelegate>

class DSBDelegate : public QStyledItemDelegate
{
    Q_OBJECT
public:
    DSBDelegate(QObject *parent = 0);
    QWidget *createEditor(QWidget *parent, const QStyleOptionViewItem &option,
                          const QModelIndex &index) const override;

    void setEditorData(QWidget *editor, const QModelIndex &index) const override;
    void setModelData(QWidget *editor, QAbstractItemModel *model,
                      const QModelIndex &index) const override;

    void updateEditorGeometry(QWidget *editor,
        const QStyleOptionViewItem &option, const QModelIndex &index) const override;

signals:
    void modelDataUpdated(int rowIndex, int colIndex, double value) const;
};

#endif // DSBDELEGATE_H
