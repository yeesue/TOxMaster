#ifndef SEQSCRIPTWIN_H
#define SEQSCRIPTWIN_H

#include <QMainWindow>
#include "QStandardItemModel"

namespace Ui {
class SeqScriptWin;
}

class SeqScriptWin : public QMainWindow
{
    Q_OBJECT

public:
    explicit SeqScriptWin(QWidget *parent = nullptr);
    ~SeqScriptWin();

private:
    Ui::SeqScriptWin *ui;

    QStandardItemModel *cmdModel1 = NULL;
    QStandardItemModel *cmdAttrModel = NULL;
};

#endif // SEQSCRIPTWIN_H
