#ifndef HEXSPINBOXDELEGATE_H
#define HEXSPINBOXDELEGATE_H

#include <QStyledItemDelegate>


class HexSpinBoxDelegate : public QStyledItemDelegate
{
    Q_OBJECT
public:
    explicit HexSpinBoxDelegate(QObject *parent = nullptr);
    ~HexSpinBoxDelegate();

    QWidget *createEditor(QWidget *parent, const QStyleOptionViewItem &option,
                          const QModelIndex &index) const override;

    void setEditorData(QWidget *editor, const QModelIndex &index) const override;
    void setModelData(QWidget *editor, QAbstractItemModel *model,
                      const QModelIndex &index) const override;

    void updateEditorGeometry(QWidget *editor,
        const QStyleOptionViewItem &option, const QModelIndex &index) const override;

signals:
    void modelDataUpdated(QModelIndex rowIndex, int value) const;

};

#endif // HEXSPINBOXDELEGATE_H
