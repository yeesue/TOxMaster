
#ifndef DELEGATE_H
#define DELEGATE_H

#include <QStyledItemDelegate>

/**
 * @brief DoubleSpinBox委托 - 支持参数编辑
 * 
 * 优化点：
 * 1. 支持从Model获取小数位数配置
 * 2. 支持从Model获取范围限制
 * 3. 支持参数指针配置
 */
class DoubleSpinBoxDelegate : public QStyledItemDelegate
{
    Q_OBJECT

public:
    explicit DoubleSpinBoxDelegate(QObject *parent = nullptr);

    QWidget *createEditor(QWidget *parent, const QStyleOptionViewItem &option,
                          const QModelIndex &index) const override;

    void setEditorData(QWidget *editor, const QModelIndex &index) const override;
    void setModelData(QWidget *editor, QAbstractItemModel *model,
                      const QModelIndex &index) const override;

    void updateEditorGeometry(QWidget *editor,
        const QStyleOptionViewItem &option, const QModelIndex &index) const override;

signals:
    void modelDataUpdated(int rowIndex, double value) const;

};
//! [0]


#endif
