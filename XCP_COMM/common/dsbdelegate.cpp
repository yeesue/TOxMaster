#include "dsbdelegate.h"
#include <QDoubleSpinBox>
#include "a2l_varchar.h"

DSBDelegate::DSBDelegate(QObject *parent)
    : QStyledItemDelegate(parent)
{

}

QWidget *DSBDelegate::createEditor(QWidget *parent,
    const QStyleOptionViewItem &/* option */,
    const QModelIndex &index) const
{
    if(index.column() == 0 && index.row() == 0)
        return NULL;

    QVariant variant = index.data(Qt::UserRole);
    A2L_VarChar *mapVar = variant.value<A2L_VarChar*>();

    QDoubleSpinBox *editor = new QDoubleSpinBox(parent);
    editor->setFrame(false);
    editor->setMinimum(-10000);
    editor->setMaximum(10000);

    if(index.row() == 0)
    {
        editor->setMinimum(mapVar->Axis_X_LowerLimit);
        editor->setMaximum(mapVar->Axis_X_UpperLimit == 0 ? 255 : mapVar->Axis_X_UpperLimit);
    }
    else if(index.column() == 0)
    {
        editor->setMinimum(mapVar->Axis_Y_LowerLimit);
        editor->setMaximum(mapVar->Axis_Y_UpperLimit == 0 ? 255 : mapVar->Axis_Y_UpperLimit);
    }
    else
    {
        editor->setMinimum(mapVar->LowerLimit);
        editor->setMaximum(mapVar->UpperLimit);
    }


    return editor;
}

void DSBDelegate::setEditorData(QWidget *editor,
                                     const QModelIndex &index) const
{
    double value = index.model()->data(index, Qt::EditRole).toDouble();

    QDoubleSpinBox *spinBox = static_cast<QDoubleSpinBox*>(editor);
    spinBox->setValue(value);
}

void DSBDelegate::setModelData(QWidget *editor, QAbstractItemModel *model,
                                    const QModelIndex &index) const
{
    QDoubleSpinBox *spinBox = static_cast<QDoubleSpinBox*>(editor);
    spinBox->interpretText();
    double value = spinBox->value();

    model->setData(index, value, Qt::EditRole);

    emit modelDataUpdated(index.row(), index.column(), value);
}

void DSBDelegate::updateEditorGeometry(QWidget *editor,
   const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    editor->setGeometry(option.rect);
}
