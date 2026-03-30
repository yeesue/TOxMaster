// ComboBoxDelegate.h
#pragma once

#include <QStyledItemDelegate>
#include <QComboBox>

class ComboBoxDelegate : public QStyledItemDelegate {
    Q_OBJECT

public:
    ComboBoxDelegate(QObject* parent = nullptr);
    
    // 设置枚举项
    void setItems(const QStringList& items);
    void setItems(const QMap<int, QString>& valueTextMap);
    
    // 必须重写的函数
    QWidget* createEditor(QWidget* parent, 
                         const QStyleOptionViewItem& option,
                         const QModelIndex& index) const override;
    
    void setEditorData(QWidget* editor, 
                      const QModelIndex& index) const override;
    
    void setModelData(QWidget* editor,
                     QAbstractItemModel* model,
                     const QModelIndex& index) const override;
    
    void updateEditorGeometry(QWidget* editor,
                             const QStyleOptionViewItem& option,
                             const QModelIndex& index) const override;

signals:
    void modelDataUpdated(int rowIndex, int colIndex, QString text) const;

private:
    QStringList m_items;  // 文本列表
    QMap<int, QString> m_valueTextMap;  // 值-文本映射
};
