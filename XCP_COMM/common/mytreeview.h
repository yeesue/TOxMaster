#ifndef MYTREEWIDGET_H
#define MYTREEWIDGET_H

#include <QTreeView>

class MsgTreeView : public QTreeView
{
    Q_OBJECT
public:
    MsgTreeView(QWidget *parent = 0);

    QString getMsgsString();
    QStringList getMsgsStringList();
    QString getSigsString();

    //QMap<QString, QList<QLineEdit *>> allLineEdit;

protected:
    void mousePressEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void dragEnterEvent(QDragEnterEvent *event);
    void dragMoveEvent(QDragMoveEvent *event);
    void dropEvent(QDropEvent *event);

private:
    void performDrag();

    QPoint startPos;

public slots:
    //void openEditor(QTreeWidgetItem*, int);
    //void closeEditor();

    //QTreeWidgetItem *getClickedItem(QTreeWidgetItem *item, int column);
};

class MsgTreeView_W : public MsgTreeView
{
    Q_OBJECT
public:
    MsgTreeView_W(QWidget *parent = 0);
    //QMap<QString, QList<QLineEdit *>> allLineEdit;

protected:
    void dropEvent(QDropEvent *event);
private:

};

class DBCTreeView : public MsgTreeView
{
    Q_OBJECT
public:
    DBCTreeView(QWidget *parent = 0);

protected:
    //void mousePressEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    //void dragEnterEvent(QDragEnterEvent *event);
    //void dragMoveEvent(QDragMoveEvent *event);
    void dropEvent(QDropEvent *event);

private:
    void performDrag();

    QPoint startPos;
};

#endif // MYTREEWIDGET_H
