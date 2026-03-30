#ifndef METER_1_H
#define METER_1_H

#include <QMainWindow>
#include <QContextMenuEvent>
#include "param.h"
#include "pamlistdlg.h"
#include "a2l_varmeas.h"

namespace Ui {
class Meter_1;
}

class Meter_1 : public QMainWindow
{
    Q_OBJECT

public:
    explicit Meter_1(QWidget *parent = 0);
    ~Meter_1();

public slots:
    void setPam(A2L_VarMeas *pam);

    void propAction_triggered();
    void selectPam();

    void setValue(qreal value);
    void setRange(qreal min, qreal max);

protected:
    void contextMenuEvent(QContextMenuEvent *event);
    void closeEvent(QCloseEvent *event);

private:
    Ui::Meter_1 *ui;

    A2L_VarMeas *refPam = NULL;
    QMenu *m_contextMenu = NULL;
};

#endif // METER_1_H
