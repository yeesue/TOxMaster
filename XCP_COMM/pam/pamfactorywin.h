#ifndef PAMFACTORYWIN_H
#define PAMFACTORYWIN_H

#include <QMainWindow>
#include <QStandardItemModel>
#include <QStandardItem>
#include "globals.h"

namespace Ui {
class PamFactoryWin;
}

class PamFactoryWin : public QMainWindow
{
    Q_OBJECT

public:
    explicit PamFactoryWin(QWidget *parent = nullptr);
    ~PamFactoryWin();

public slots:
    void updatePamView(bool runFlag);

    void stopPamUpdate();
    void startPamUpdate();


private slots:
    void updatePamValueView();
    void updateWritePamValueView();
    void updateMapPamValueView();

    void on_actionExpand_triggered();

private:
    Ui::PamFactoryWin *ui;

    bool pamActiveFlag = false;

    QStandardItemModel *pamModel = NULL;
    bool isAllExpanded = false;

    QTimer *updateTimer = NULL;
};

#endif // PAMFACTORYWIN_H
