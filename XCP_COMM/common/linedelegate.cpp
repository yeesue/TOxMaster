#include "linedelegate.h"
#include <QLineEdit>

LineDelegate::LineDelegate(QObject *parent)
    : QStyledItemDelegate(parent)
{

}

QWidget *LineDelegate::createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    //if(index.column() == 0 && index.row() == 0)
        //return NULL;

    QVariant variant = index.data(Qt::UserRole);

    QLineEdit *editor = new QLineEdit(parent);
    editor->setFrame(false);

    return editor;
}

void LineDelegate::setEditorData(QWidget *editor, const QModelIndex &index) const
{
    QString text = index.model()->data(index, Qt::EditRole).toString();

    QLineEdit *line = static_cast<QLineEdit*>(editor);
    line->setText(text);
}

void LineDelegate::setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const
{
    QLineEdit *line = static_cast<QLineEdit*>(editor);
    if(!line) return;

    QString text = line->text();

    model->setData(index, text, Qt::EditRole);

    emit modelDataUpdated(index.row(), index.column(), text);
}

void LineDelegate::updateEditorGeometry(QWidget *editor, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    editor->setGeometry(option.rect);
}
