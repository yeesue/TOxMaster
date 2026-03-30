// ComboBoxDelegate.cpp
#include "comboboxdelegate.h"

ComboBoxDelegate::ComboBoxDelegate(QObject* parent)
    : QStyledItemDelegate(parent) {}

void ComboBoxDelegate::setItems(const QStringList& items) {
    m_items = items;
}

void ComboBoxDelegate::setItems(const QMap<int, QString>& valueTextMap) {
    m_valueTextMap = valueTextMap;
}

QWidget* ComboBoxDelegate::createEditor(QWidget* parent,
                                       const QStyleOptionViewItem& /*option*/,
                                       const QModelIndex& /*index*/) const {
    QComboBox* editor = new QComboBox(parent);
    
    if (!m_items.isEmpty()) {
        editor->addItems(m_items);
    } else if (!m_valueTextMap.isEmpty()) {
        QMapIterator<int, QString> it(m_valueTextMap);
        while (it.hasNext()) {
            it.next();
            editor->addItem(it.value(), it.key());
        }
    }
    
    editor->setFrame(false);  // 无边框
    return editor;
}

void ComboBoxDelegate::setEditorData(QWidget* editor,
                                    const QModelIndex& index) const {
    QComboBox* comboBox = qobject_cast<QComboBox*>(editor);
    if (!comboBox) return;
    
    QVariant value = index.model()->data(index, Qt::EditRole);
    if (!m_valueTextMap.isEmpty()) {
        // 使用值查找
        comboBox->setCurrentIndex(comboBox->findData(value));
    } else {
        // 使用文本查找
        comboBox->setCurrentText(value.toString());
    }
}

void ComboBoxDelegate::setModelData(QWidget* editor,
                                   QAbstractItemModel* model,
                                   const QModelIndex& index) const {
    QComboBox* comboBox = qobject_cast<QComboBox*>(editor);
    if (!comboBox) return;
    
    QVariant value;
    if (!m_valueTextMap.isEmpty()) {
        // 存储实际值
        value = comboBox->currentData();
    } else {
        // 存储文本
        value = comboBox->currentText();
    }
    
    model->setData(index, value, Qt::EditRole);

    emit modelDataUpdated(index.row(), index.column(), value.toString());
}

void ComboBoxDelegate::updateEditorGeometry(QWidget* editor,
                                          const QStyleOptionViewItem& option,
                                          const QModelIndex& /*index*/) const {
    editor->setGeometry(option.rect);
}
