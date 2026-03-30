#include "hexspinboxdelegate.h"
#include "hexspinbox.h"
#include <QDebug>

HexSpinBoxDelegate::HexSpinBoxDelegate(QObject *parent)
    : QStyledItemDelegate(parent)
{

}

HexSpinBoxDelegate::~HexSpinBoxDelegate()
{

}

QWidget *HexSpinBoxDelegate::createEditor(QWidget *parent,
        const QStyleOptionViewItem &option,
        const QModelIndex &index) const
{
    if(index.column() != 1)
        return NULL;

    HexSpinBox *editor = new HexSpinBox(parent);
    editor->setPrefix("0x");
    editor->setFrame(false);
    editor->setMaximum(0xFFFF);

    return editor;
}

void HexSpinBoxDelegate::setEditorData(QWidget *editor,
                                       const QModelIndex &index) const
{
    bool ok = false;
    int value = index.model()->data(index, Qt::EditRole).toString().toUInt(&ok, 16);

    HexSpinBox *spinBox = static_cast<HexSpinBox*>(editor);
    spinBox->setValue(value);
}

void HexSpinBoxDelegate::setModelData(QWidget *editor, QAbstractItemModel *model,
                                      const QModelIndex &index) const
{
    HexSpinBox *spinBox = static_cast<HexSpinBox*>(editor);
    spinBox->interpretText();
    int value = spinBox->value();
    //qDebug()<<"value:"<<value;

    QString valueStr = QString("%1").arg(value, 2, 16, QLatin1Char('0')).toUpper();

    valueStr = "0x" + valueStr;

    model->setData(index, valueStr, Qt::EditRole);

    emit modelDataUpdated(index, value);
}

void HexSpinBoxDelegate::updateEditorGeometry(QWidget *editor,
                                              const QStyleOptionViewItem &option,
                                              const QModelIndex &index) const
{
    editor->setGeometry(option.rect);
}
