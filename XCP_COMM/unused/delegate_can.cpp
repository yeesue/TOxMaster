/*
    delegate.cpp

    A delegate that allows the user to change integer values from the model
    using a double spin box widget.
*/

#include "delegate_can.h"

#include <QDoubleSpinBox>
#include "config.h"

//! [0]
DSBDelegateCAN::DSBDelegateCAN(QObject *parent)
    : QStyledItemDelegate(parent)
{
}
//! [0]

//! [1]
QWidget *DSBDelegateCAN::createEditor(QWidget *parent,
    const QStyleOptionViewItem &/* option */,
    const QModelIndex &index) const
{
    if(index.column() != 1)
        return NULL;

    QVariant variant = index.data(Qt::UserRole);
    dbc_signal *signal = variant.value<dbc_signal*>();

    QDoubleSpinBox *editor = new QDoubleSpinBox(parent);
    editor->setFrame(false);
    editor->setMinimum(signal->Minimum);
    editor->setMaximum(signal->Maximum);

    return editor;
}
//! [1]

//! [2]
void DSBDelegateCAN::setEditorData(QWidget *editor,
                                    const QModelIndex &index) const
{

    double value = index.model()->data(index, Qt::EditRole).toDouble();

    QDoubleSpinBox *spinBox = static_cast<QDoubleSpinBox*>(editor);
    spinBox->setValue(value);
}
//! [2]

//! [3]
void DSBDelegateCAN::setModelData(QWidget *editor, QAbstractItemModel *model,
                                   const QModelIndex &index) const
{
    QDoubleSpinBox *spinBox = static_cast<QDoubleSpinBox*>(editor);
    spinBox->interpretText();
    double value = spinBox->value();

    model->setData(index, value, Qt::EditRole);

    emit modelDataUpdated(index, value);
}
//! [3]

//! [4]
void DSBDelegateCAN::updateEditorGeometry(QWidget *editor,
    const QStyleOptionViewItem &option, const QModelIndex &/* index */) const
{
    editor->setGeometry(option.rect);
}
//! [4]
