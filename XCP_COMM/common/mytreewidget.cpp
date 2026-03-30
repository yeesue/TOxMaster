#include <QtWidgets>
#include "mytreewidget.h"

MsgTreeWidget::MsgTreeWidget(QWidget *parent)
    : QTreeWidget(parent)
{
    setAcceptDrops(true);
}

QString MsgTreeWidget::getMsgsString()
{
    QStringList msgsList;
    QString msgsString = "";
    //if (topLevelItemCount() == 0)
        //return msgsString;
    for (int i = 0; i < topLevelItemCount(); i++)
    {
        msgsList<<topLevelItem(i)->text(0);
    }
    for (int i = 0; i < msgsList.count(); i++)
    {
        if (i != (msgsList.count()-1))
            msgsString += (msgsList[i] + ",");
        else
            msgsString +=msgsList[i];
    }
    return msgsString;
}

QStringList MsgTreeWidget::getMsgsStringList()
{
    QStringList msgsList;
    for (int i = 0; i < topLevelItemCount(); i++)
    {
        msgsList<<(topLevelItem(i)->text(0));
    }
    return msgsList;
}

QString MsgTreeWidget::getSigsString()
{
    QStringList sigsList;
    QString sigsString;
    for(int i = 0; i < topLevelItemCount(); i++)
    {
        for (int j = 0; j < topLevelItem(i)->childCount(); j++)
        {
            sigsList<<topLevelItem(i)->child(j)->text(0);
        }
    }

    for (int i = 0; i < sigsList.count(); i++)
    {
        if(i != (sigsList.count()-1))
            sigsString += sigsList[i] + ",";
        else
            sigsString += sigsList[i];
    }
    return sigsString;
}

QHash<QString, QStringList> MsgTreeWidget::getFrameSigPamString()
{
    QHash<QString, QStringList> frameSigPamHash;
    for(int i = 0; i < topLevelItemCount(); i++)
    {
        QString frameStr = topLevelItem(i)->text(0);
        //QStringList sigsList;
        //QHash<QString, QStringList> frameSigPamHash;
        for (int j = 0; j < topLevelItem(i)->childCount(); j++)
        {
            QString sigName = topLevelItem(i)->child(j)->text(0);
            QStringList sigPams;
            for(int z = 0; z < 4; z++)
            {
                sigPams<<topLevelItem(i)->child(j)->text(z+2);
            }
            frameSigPamHash.insert(sigName, sigPams);
        }
    }
    return frameSigPamHash;
}

QStringList MsgTreeWidget::getSigSetValueStringList()
{
    QStringList sigValueList;
    for(int i = 0; i < topLevelItemCount(); i++)
    {
        for (int j = 0; j < topLevelItem(i)->childCount(); j++)
        {
            sigValueList<<topLevelItem(i)->child(j)->text(1);
        }
    }

    return sigValueList;
}

void MsgTreeWidget::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton)
        startPos = event->pos();
    QTreeWidget::mousePressEvent(event);
}

void MsgTreeWidget::mouseMoveEvent(QMouseEvent *event)
{
    if (event->buttons() & Qt::LeftButton)
    {
        int distance = (event->pos() - startPos).manhattanLength();
        if (distance >= QApplication::startDragDistance())
            performDrag();
    }
    QTreeWidget::mouseMoveEvent(event);
}

void MsgTreeWidget::dragEnterEvent(QDragEnterEvent *event)
{
    MsgTreeWidget *source =
            qobject_cast<MsgTreeWidget *>(event->source());
    if(!source)
        return;
    if (source != this)
    {
        event->setDropAction(Qt::MoveAction);
        event->accept();
    }
}

void MsgTreeWidget::dragMoveEvent(QDragMoveEvent *event)
{
    MsgTreeWidget *source =
            qobject_cast<MsgTreeWidget *>(event->source());
    if(!source)
        return;
    if (source != this)
    {
        event->setDropAction(Qt::MoveAction);
        event->accept();
    }
}

void MsgTreeWidget::dropEvent(QDropEvent *event)
{
    MsgTreeWidget *source =
            qobject_cast<MsgTreeWidget *>(event->source());

    if(!source)
        return;

    if (source != this)
    {

        QString MsgAndSigs = event->mimeData()->text();
        QStringList textList = MsgAndSigs.split(',');
        int sigsCount = textList.count()-1;
        QString msgName = textList[0];
        QTreeWidgetItem *msgItem = new QTreeWidgetItem;
        msgItem->setText(0, msgName);
        for(int i = 0; i < 4; i++)
            msgItem->setBackgroundColor(i, QColor(0xFFEC8B));
        QList<QTreeWidgetItem *> sigList;
        for (int i = 1; i <= sigsCount; i++)
        {
            QTreeWidgetItem *sigItem = new QTreeWidgetItem;
            sigItem->setText(0, textList[i]);
            sigList.append(sigItem);
        }
        if (this->findItems(msgName, Qt::MatchExactly, 0).isEmpty())
        {
            addTopLevelItem(msgItem);
            msgItem->addChildren(sigList);            
        }
        event->setDropAction(Qt::MoveAction);
        event->accept();
    }
}

void MsgTreeWidget::performDrag()
{
    QString MsgAndSigs = "";
    QTreeWidgetItem *item = currentItem();

    if(!item)
        return;

    //QTreeWidgetItem *item = itemAt(startPos);
    MsgAndSigs += item->text(0);
    MsgAndSigs += ",";
    QList<QTreeWidgetItem *> childrenItem;
    for (int i = 0; i < item->childCount(); i++)
    {
        childrenItem.append(item->child(i));
        MsgAndSigs += item->child(i)->text(0);
        if (i != (item->childCount()-1))
            MsgAndSigs +=",";
    }
    if (!item->parent() && item)
    {
        QMimeData *mimeData = new QMimeData;
        mimeData->setText(MsgAndSigs);

        QDrag *drag = new QDrag(this);
        drag->setMimeData(mimeData);

        if (drag->exec(Qt::MoveAction) == Qt::MoveAction)
        {
            QString msgName = item->text(0);
            //if (allLineEdit.contains(msgName))
           //       allLineEdit.remove(msgName);
            delete item;
        }
    }
}

void MsgTreeWidget::openEditor(QTreeWidgetItem* item, int column)
{
    if (column == 1)
        item->setFlags(item->flags() | Qt::ItemIsEditable);
    else
        item->setFlags(item->flags() & ~Qt::ItemIsEditable);
}

void MsgTreeWidget::closeEditor()
{

}

QTreeWidgetItem *MsgTreeWidget::getClickedItem(QTreeWidgetItem *item, int column)
{
    return item;
}




DBCTreeWidget::DBCTreeWidget(QWidget *parent)
{
    setAcceptDrops(true);
}

void DBCTreeWidget::mouseMoveEvent(QMouseEvent *event)
{
    if (event->buttons() & Qt::LeftButton)
    {
        int distance = (event->pos() - startPos).manhattanLength();
        if (distance >= QApplication::startDragDistance())
            performDrag();
    }
    QTreeWidget::mouseMoveEvent(event);
}

void DBCTreeWidget::dropEvent(QDropEvent *event)
{
    event->accept();
}

void DBCTreeWidget::performDrag()
{
    QString MsgAndSigs = "";
    QTreeWidgetItem *item = currentItem();
    if(!item)
        return;

    MsgAndSigs += item->text(0);
    MsgAndSigs += ",";
    QList<QTreeWidgetItem *> childrenItem;
    for (int i = 0; i < item->childCount(); i++)
    {
        childrenItem.append(item->child(i));
        MsgAndSigs += item->child(i)->text(0);
        if (i != (item->childCount()-1))
            MsgAndSigs +=",";
    }
    QTreeWidgetItem *parentItem = item->parent();
    if (parentItem && indexOfTopLevelItem(parentItem) != -1)
    {
        QMimeData *mimeData = new QMimeData;
        mimeData->setText(MsgAndSigs);

        QDrag *drag = new QDrag(this);
        drag->setMimeData(mimeData);
        drag->exec(Qt::CopyAction);
    }
}

MsgTreeWidget_W::MsgTreeWidget_W(QWidget *parent)
{
    setAcceptDrops(true);
}

void MsgTreeWidget_W::dropEvent(QDropEvent *event)
{
    MsgTreeWidget *source =
            qobject_cast<MsgTreeWidget *>(event->source());
    if(!source)
        return;

    if (source != this)
    {

        QString MsgAndSigs = event->mimeData()->text();
        QStringList textList = MsgAndSigs.split(',');
        int sigsCount = textList.count()-1;
        QString msgName = textList[0];
        QTreeWidgetItem *msgItem = new QTreeWidgetItem;
        msgItem->setText(0, msgName);
        for(int i = 0; i < 4; i++)
            msgItem->setBackgroundColor(i, QColor(0xC1FFC1));
        QList<QTreeWidgetItem *> sigList;
        for (int i = 1; i <= sigsCount; i++)
        {
            QTreeWidgetItem *sigItem = new QTreeWidgetItem;
            sigItem->setText(0, textList[i]);
            sigItem->setText(1, "0");
            sigList.append(sigItem);
        }
        if (this->findItems(msgName, Qt::MatchExactly, 0).isEmpty())
        {
            addTopLevelItem(msgItem);
            msgItem->addChildren(sigList);
 /*
            QList<QLineEdit *> msgLineEdit;

            for(int i = 0; i < sigList.count(); i++)
            {
                QLineEdit *lineEditItem = new QLineEdit(this);
                QDoubleValidator *doubleValidator = new QDoubleValidator;
                lineEditItem->setValidator(doubleValidator);
                lineEditItem->setText("0");
                lineEditItem->setFrame(false);
                this->setItemWidget(sigList[i], 1, lineEditItem);
                msgLineEdit.append(lineEditItem);
            }
            allLineEdit.insert(msgName, msgLineEdit);
*/
        }
        event->setDropAction(Qt::MoveAction);
        event->accept();
    }
}
