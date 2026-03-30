#include <QtWidgets>
#include "mytreeview.h"

MsgTreeView::MsgTreeView(QWidget *parent)
    : QTreeView(parent)
{
    setAcceptDrops(true);
}

QString MsgTreeView::getMsgsString()
{
    QStringList msgsList;
    QString msgsString = "";

    if(!model()) return msgsString;

    int count = model()->rowCount();
    QStandardItemModel *pModel = (QStandardItemModel*)model();

    for (int i = 0; i < count; i++)
    {
        msgsList << pModel->item(i, 0)->text();
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

QStringList MsgTreeView::getMsgsStringList()
{
    QStringList msgsList;

    if(!model()) return msgsList;
    int count = model()->rowCount();
    QStandardItemModel *pModel = (QStandardItemModel*)model();

    for (int i = 0; i < count; i++)
    {
        msgsList<<(pModel->item(i, 0)->text());
    }
    return msgsList;
}

QString MsgTreeView::getSigsString()
{
    QStringList sigsList;
    QString sigsString;

    if(!model()) return sigsString;
    int count = model()->rowCount();
    QStandardItemModel *pModel = (QStandardItemModel*)model();

    for(int i = 0; i < count; i++)
    {

        QStandardItem *frameItem = pModel->item(i, 0);
        int sigCount = frameItem->rowCount();
        for (int j = 0; j < sigCount; j++)
        {
            sigsList<<frameItem->child(j, 0)->text();
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

void MsgTreeView::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton)
        startPos = event->pos();

    QTreeView::mousePressEvent(event);
}

void MsgTreeView::mouseMoveEvent(QMouseEvent *event)
{
    if (event->buttons() && Qt::LeftButton)
    {
        int distance = (event->pos() - startPos).manhattanLength();
        if (distance >= QApplication::startDragDistance())
            performDrag();
    }
    QTreeView::mouseMoveEvent(event);
}

void MsgTreeView::dragEnterEvent(QDragEnterEvent *event)
{
    MsgTreeView *source =
            qobject_cast<MsgTreeView *>(event->source());

    if(!source)
        return;

    if (source != this)
    {
        event->setDropAction(Qt::MoveAction);
        event->accept();
    }
}

void MsgTreeView::dragMoveEvent(QDragMoveEvent *event)
{
    MsgTreeView *source =
            qobject_cast<MsgTreeView *>(event->source());

    if(!source)
        return;

    if (source != this)
    {
        event->setDropAction(Qt::MoveAction);
        event->accept();
    }
}

void MsgTreeView::dropEvent(QDropEvent *event)
{
    MsgTreeView *source =
            qobject_cast<MsgTreeView *>(event->source());

    if(!source)
        return;

    QStandardItemModel *pModel = (QStandardItemModel*)model();

    if (source != this)
    {
        QString MsgAndSigs = event->mimeData()->text();
        QStringList textList = MsgAndSigs.split(',');
        int sigsCount = textList.count() - 2;
        //qDebug()<<"sig count:"<<sigsCount;

        QString msgName = textList[0];
        QString idText = textList[1];
        QStandardItem *msgItem0 = new QStandardItem(msgName);
        QStandardItem *msgItem1 = new QStandardItem("-");
        QStandardItem *msgItem2 = new QStandardItem(idText);
        msgItem2->setTextAlignment(Qt::AlignCenter);
        QStandardItem *msgItem3 = new QStandardItem("-");

        QList<QStandardItem*> items;
        items << msgItem0 << msgItem1 << msgItem2 << msgItem3;

        for(int k = 0; k < items.count(); k++)
        {
            QStandardItem *item = items.at(k);
            QBrush brush(QColor(0xFFEC8B));
            item->setBackground(brush);
        }


        for (int i = 0; i < sigsCount; i++)
        {
            QString sigName = textList[i + 2];
            QStandardItem *sigItem0 = new QStandardItem(sigName);
            QStandardItem *sigItem1 = new QStandardItem("0");
            sigItem1->setTextAlignment(Qt::AlignCenter);

            QList<QStandardItem*> sigList;
            sigList << sigItem0 << sigItem1;

            msgItem0->appendRow(sigList);
        }


        if (pModel->findItems(msgName, Qt::MatchExactly, 0).isEmpty())
        {
            pModel->appendRow(items);
        }
        event->setDropAction(Qt::MoveAction);
        event->accept();
    }
}

void MsgTreeView::performDrag()
{

    QString MsgAndSigs = "";
    QStandardItemModel *pModel = (QStandardItemModel*)model();
    QStandardItem *item = pModel->itemFromIndex(currentIndex());
    if(!item) return;
    QStandardItem *parentItem = item->parent();
    if(parentItem) return;

    MsgAndSigs += item->text();
    MsgAndSigs += ",";

    QList<QStandardItem *> childrenItem;
    for (int i = 0; i < item->rowCount(); i++)
    {
        childrenItem.append(item->child(i));
        MsgAndSigs += item->child(i)->text();
        if (i != (item->rowCount() - 1))
            MsgAndSigs +=",";
    }  

    QMimeData *mimeData = new QMimeData();
    mimeData->setText(MsgAndSigs);

    QDrag *drag = new QDrag(this);
    drag->setMimeData(mimeData);

    if (drag->exec(Qt::MoveAction) == Qt::MoveAction)
    {
        QString msgName = item->text();
        pModel->removeRow(item->row());
        //delete item;
    }
}

/*
void MsgTreeView::openEditor(QTreeWidgetItem* item, int column)
{
    if (column == 1)
        item->setFlags(item->flags() | Qt::ItemIsEditable);
    else
        item->setFlags(item->flags() & ~Qt::ItemIsEditable);
}

void MsgTreeView::closeEditor()
{

}

QTreeWidgetItem *MsgTreeView::getClickedItem(QTreeWidgetItem *item, int column)
{
    return item;
}
*/

MsgTreeView_W::MsgTreeView_W(QWidget *parent)
{
    setAcceptDrops(true);
}

void MsgTreeView_W::dropEvent(QDropEvent *event)
{
    MsgTreeView *source =
            qobject_cast<MsgTreeView *>(event->source());


    QStandardItemModel *pModel = (QStandardItemModel*)model();

    if (source != this)
    {
        QString MsgAndSigs = event->mimeData()->text();
        QStringList textList = MsgAndSigs.split(',');
        int sigsCount = textList.count() - 2;

        QString msgName = textList[0];
        QString idText = textList[1];
        QStandardItem *msgItem0 = new QStandardItem(msgName);
        QStandardItem *msgItem1 = new QStandardItem("-");
        QStandardItem *msgItem2 = new QStandardItem(idText);
        msgItem2->setTextAlignment(Qt::AlignCenter);
        QStandardItem *msgItem3 = new QStandardItem("-");

        QList<QStandardItem*> items;
        items << msgItem0 << msgItem1 << msgItem2 << msgItem3;

        for(int k = 0; k < items.count(); k++)
        {
            QStandardItem *item = items.at(k);
            QBrush brush(QColor(0xC1FFC1));
            item->setBackground(brush);
        }

        for (int i = 0; i < sigsCount; i++)
        {
            QString sigName = textList[i + 2];
            QStandardItem *sigItem0 = new QStandardItem(sigName);
            QStandardItem *sigItem1 = new QStandardItem("0");
            sigItem1->setTextAlignment(Qt::AlignCenter);

            QList<QStandardItem*> sigList;
            sigList << sigItem0 << sigItem1;

            msgItem0->appendRow(sigList);
        }

        if (pModel->findItems(msgName, Qt::MatchExactly, 0).isEmpty())
        {
            pModel->appendRow(items);
        }
        event->setDropAction(Qt::MoveAction);
        event->accept();
    }
}



DBCTreeView::DBCTreeView(QWidget *parent)
{
    setAcceptDrops(true);
}

void DBCTreeView::mouseMoveEvent(QMouseEvent *event)
{
    //qDebug()<<"mouse move 1";
    if (event->buttons() & Qt::LeftButton)
    {
        int distance = (event->pos() - startPos).manhattanLength();
        //qDebug()<<"mouse move 2";
        if (distance >= QApplication::startDragDistance())
        {
            //qDebug()<<"mouse move 3";
            performDrag();
        }
    }
    QTreeView::mouseMoveEvent(event);
}

void DBCTreeView::dropEvent(QDropEvent *event)
{
    event->accept();
}

void DBCTreeView::performDrag()
{

    QString MsgAndSigs = "";

    QStandardItemModel *pModel = (QStandardItemModel*)model();
    QStandardItem *item = pModel->itemFromIndex(currentIndex());
    if(!item) return;
    QStandardItem *parentItem = item->parent();
    if(!parentItem) return;

    int row = currentIndex().row();
    QStandardItem *idItem = parentItem->child(row, 1);

    MsgAndSigs += item->text();
    MsgAndSigs += ",";

    MsgAndSigs += idItem->text();
    MsgAndSigs += ",";

    QList<QStandardItem *> childrenItem;
    for (int i = 0; i < item->rowCount(); i++)
    {
        childrenItem.append(item->child(i));
        MsgAndSigs += item->child(i, 0)->text();
        if (i != (item->rowCount() - 1))
            MsgAndSigs +=",";
    }

    QMimeData *mimeData = new QMimeData();
    mimeData->setText(MsgAndSigs);

    QDrag *drag = new QDrag(this);
    drag->setMimeData(mimeData);
    drag->exec(Qt::CopyAction);

}

