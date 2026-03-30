#include "compuvtabmodel.h"
#include <QFont>
#include <QDebug>

CompuVtabModel::CompuVtabModel(QObject *parent)
{
    nRow = 0;
    nColumn = 0;
}

CompuVtabModel::~CompuVtabModel()
{

}

void CompuVtabModel::setList(QList<Node *> labelList)
{
    listCompuVtabs = labelList;

    nRow = labelList.count();
    nColumn = 5;

    beginResetModel();
    endResetModel();
}

int CompuVtabModel::rowCount(const QModelIndex &parent) const
{
    return nRow;
}

int CompuVtabModel::columnCount(const QModelIndex &parent) const
{
    return nColumn;
}

QVariant CompuVtabModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();

    int row = index.row();
    int column = index.column();


   if (0 <= row && row < nRow)
    {
        COMPU_VTAB *label = (COMPU_VTAB*)listCompuVtabs.at(row);

        switch (role)
        {
        case Qt::DisplayRole :
            {

                Node * node = label->getParentNode()->getParentNode();

                if (column == 0)
                {
                    return label->name;
                }
                else if (column == 1)
                {
                    QString str = label->fixPar("LongIdentifier").c_str();
                    return str;
                }
                else if (column == 2)
                {
                    QString str = label->fixPar("ConversionType").c_str();
                    return str;
                }
                else if (column == 3)
                {

                    QString str = label->fixPar("NumberValuePairs").c_str();
                    return str;
                }
                else if (column == 4)
                {
                    QString valuePairStr = "";
                    QStringList valueList = label->getValueList();
                    foreach (QString value, valueList) {
                        int key = label->getPos(value);
                        valuePairStr += (QString::number(key) + "-" + value);
                        if (value != valueList.last())
                        {
                            valuePairStr += ",";
                        }
                    }
                    return valuePairStr.toLocal8Bit().data();
                }

                else
                    return QVariant();
            }
            break;

        case Qt::DecorationRole: // The data to be rendered as a decoration in the form of an icon.
            break;

        case Qt::EditRole:
            {
            }
            break;

        case Qt::ToolTipRole:
            {
            }
            break;

        case Qt::StatusTipRole: // The data displayed in the status bar.
            break;

        case Qt::WhatsThisRole: // The data displayed for the item in "What's This?" mode.
            break;

        case Qt::SizeHintRole: // The size hint for the item that will be supplied to views.
            break;

        case Qt::FontRole : // The font used for items rendered with the default delegate.
            {
                if (column == 0)
                {
                    QFont font;
                    font.setBold(true);
                    font.setFamily("Microsoft YaHei");
                    font.setPixelSize(12);
                    return font;
                }
            }
            break;

        case Qt::TextAlignmentRole:
            break;

        case Qt::BackgroundRole:
            {

            }
            break;

        case Qt::ForegroundRole: // the foreground brush (text color, typically) used for items rendered with the default delegate.
            {
                if (column == 0)
                {
                    QColor color = Qt::blue;
                    return color;
                }
            }
            break;
        }
    }

    return QVariant();
}

QVariant CompuVtabModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    switch (role)
    {
        case Qt::DisplayRole :
            {
            if (orientation == Qt::Horizontal)
            {
                if (section == 0)
                {
                    return "Name";
                }
                else if (section == 1)
                {
                    return "LongIdentifier";
                }
                else if (section == 2)
                {
                    return "ConversionType";
                }
                else if (section == 3)
                {
                    return "NumberValuePairs";
                }
                else if (section == 4)
                {
                    return "ValuePairs";
                }
                else
                    return QVariant();
            }
            break;

        case Qt::DecorationRole: // The data to be rendered as a decoration in the form of an icon.
            break;

        case Qt::EditRole:
            {
            }
            break;

        case Qt::ToolTipRole:
            {
            }
            break;

        case Qt::StatusTipRole: // The data displayed in the status bar.
            break;

        case Qt::WhatsThisRole: // The data displayed for the item in "What's This?" mode.
            break;

        case Qt::SizeHintRole: // The size hint for the item that will be supplied to views.
            break;

        case Qt::FontRole : // The font used for items rendered with the default delegate.
            {
               QFont font;
               font.setBold(true);
               font.setFamily("Microsoft YaHei");
               font.setPixelSize(12);
               return font;
            }
            break;

        case Qt::TextAlignmentRole:
            break;

        case Qt::BackgroundRole:
            {

            }
            break;

        case Qt::ForegroundRole: // the foreground brush (text color, typically) used for items rendered with the default delegate.
            {
                QColor color = Qt::red;
                return color;
            }
            break;
        }
    }

    return QVariant();
}

Qt::ItemFlags CompuVtabModel::flags(const QModelIndex &index) const
{
    Qt::ItemFlags flags = QAbstractItemModel::flags(index);
    flags |= Qt::ItemIsEditable;
    return flags;
}
