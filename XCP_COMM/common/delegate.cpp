/*
    delegate.cpp

    A delegate that allows the user to change integer values from the model
    using a double spin box widget.
*/

#include "delegate.h"

#include <QDoubleSpinBox>
#include "a2l_varchar.h"

//! [0]
DoubleSpinBoxDelegate::DoubleSpinBoxDelegate(QObject *parent)
    : QStyledItemDelegate(parent)
{
}
//! [0]

//! [1]
QWidget *DoubleSpinBoxDelegate::createEditor(QWidget *parent,
    const QStyleOptionViewItem &/* option */,
    const QModelIndex &index) const
{
    if(index.column() != 1)
        return NULL;
    QVariant variant = index.data(Qt::UserRole);
    //A2L_VarChar *charVar = variant.value<A2L_VarChar*>();
    PARAM *pam = variant.value<PARAM*>();

    QDoubleSpinBox *editor = new QDoubleSpinBox(parent);
    editor->setFrame(false);
    editor->setMinimum(pam->LowerLimit);
    editor->setMaximum(pam->UpperLimit);

    return editor;
}
//! [1]

//! [2]
void DoubleSpinBoxDelegate::setEditorData(QWidget *editor,
                                    const QModelIndex &index) const
{

    double value = index.model()->data(index, Qt::EditRole).toDouble();

    QDoubleSpinBox *spinBox = static_cast<QDoubleSpinBox*>(editor);
    spinBox->setValue(value);
}
//! [2]

//! [3]
void DoubleSpinBoxDelegate::setModelData(QWidget *editor, QAbstractItemModel *model,
                                   const QModelIndex &index) const
{
    QDoubleSpinBox *spinBox = static_cast<QDoubleSpinBox*>(editor);
    spinBox->interpretText();
    double value = spinBox->value();

    model->setData(index, value, Qt::EditRole);

    emit modelDataUpdated(index.row(), value);
}
//! [3]

//! [4]
void DoubleSpinBoxDelegate::updateEditorGeometry(QWidget *editor,
    const QStyleOptionViewItem &option, const QModelIndex &/* index */) const
{
    editor->setGeometry(option.rect);
}
//! [4]
