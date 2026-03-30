#include "axisptsmodel.h"
#include <QFont>

AxisPtsModel::AxisPtsModel(QObject *parent)
{
    nRow = 0;
    nColumn = 0;
}

AxisPtsModel::~AxisPtsModel()
{

}

void AxisPtsModel::setList(QList<Node *> labelList)
{
    listAxisPts = labelList;

    nRow = labelList.count();
    nColumn = 10;

    beginResetModel();
    endResetModel();
}

int AxisPtsModel::rowCount(const QModelIndex &parent) const
{
    return nRow;
}

int AxisPtsModel::columnCount(const QModelIndex &parent) const
{
    return nColumn;
}

QVariant AxisPtsModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();

    int row = index.row();
    int column = index.column();


   if (0 <= row && row < nRow)
    {
        AXIS_PTS *label = (AXIS_PTS*)listAxisPts.at(row);

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
                    QString str = label->fixPar("Address").c_str();
                    return str;
                }
                else if (column == 3)
                {
                    QString str = label->fixPar("InputQuantity").c_str();
                    return str;
                }
                else if (column == 4)
                {
                    QString str = label->fixPar("Deposit").c_str();
                    return str;
                }
                else if (column == 5)
                {
                    QString str = label->fixPar("MaxDiff").c_str();
                    return str;
                }
                else if (column == 6)
                {
                    QString str = label->fixPar("Conversion").c_str();
                    return str;
                }
                else if (column == 7)
                {
                    QString str = label->fixPar("MaxAxisPoints").c_str();
                    return str;
                }
                else if (column == 8)
                {
                    QString str = label->fixPar("LowerLimit").c_str();
                    return str;
                }
                else if (column == 9)
                {
                    QString str = label->fixPar("UpperLimit").c_str();
                    return str;
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

QVariant AxisPtsModel::headerData(int section, Qt::Orientation orientation, int role) const
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
                    return "Address";
                }
                else if (section == 3)
                {
                    return "InputQuantity";
                }
                else if (section == 4)
                {
                    return "Deposit";
                }
                else if (section == 5)
                {
                    return "MaxDiff";
                }
                else if (section == 6)
                {
                    return "Conversion";
                }
                else if (section == 7)
                {
                    return "MaxAxisPoints";
                }
                else if (section == 8)
                {
                    return "LowerLimit";
                }
                else if (section == 9)
                {
                    return "UpperLimit";
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

Qt::ItemFlags AxisPtsModel::flags(const QModelIndex &index) const
{
    Qt::ItemFlags flags = QAbstractItemModel::flags(index);
    flags |= Qt::ItemIsEditable;
    return flags;
}
