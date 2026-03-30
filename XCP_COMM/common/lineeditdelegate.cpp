#include "lineeditdelegate.h"
#include <QLineEdit>

LineEditDelegate::LineEditDelegate()
{

}

LineEditDelegate::~LineEditDelegate()
{

}


//! [1]
QWidget *LineEditDelegate::createEditor(QWidget *parent,
    const QStyleOptionViewItem &/* option */,
    const QModelIndex &index) const
{
    if(index.column() != 1)
        return NULL;

    QVariant variant = index.data(Qt::UserRole);

    QLineEdit *editor = new QLineEdit(parent);
    editor->setFrame(false);

    return editor;
}
//! [1]

//! [2]
void LineEditDelegate::setEditorData(QWidget *editor,
                                    const QModelIndex &index) const
{

    QString str = index.model()->data(index, Qt::EditRole).toString();

    QLineEdit *lineEdit = static_cast<QLineEdit*>(editor);
    lineEdit->setText(str);
}
//! [2]

//! [3]
void LineEditDelegate::setModelData(QWidget *editor, QAbstractItemModel *model,
                                   const QModelIndex &index) const
{
    QLineEdit *lineEdit = static_cast<QLineEdit*>(editor);
    QString str = lineEdit->text();

    model->setData(index, str, Qt::EditRole);

    emit modelDataUpdated(index, str);
}
//! [3]

//! [4]
void LineEditDelegate::updateEditorGeometry(QWidget *editor,
    const QStyleOptionViewItem &option, const QModelIndex &/* index */) const
{
    editor->setGeometry(option.rect);
}
//! [4]
