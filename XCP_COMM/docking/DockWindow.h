#ifndef DOCKWINDOW_H
#define DOCKWINDOW_H
/*******************************************************************************
** Qt Advanced Docking System
** Copyright (C) 2017 Uwe Kindler
**
** This library is free software; you can redistribute it and/or
** modify it under the terms of the GNU Lesser General Public
** License as published by the Free Software Foundation; either
** version 2.1 of the License, or (at your option) any later version.
**
** This library is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
** Lesser General Public License for more details.
**
** You should have received a copy of the GNU Lesser General Public
** License along with this library; If not, see <http://www.gnu.org/licenses/>.
******************************************************************************/


//============================================================================
//                                   INCLUDES
//============================================================================
#include <QMainWindow>

#include "tablewin.h"
#include "plotwin.h"

struct MainWindowPrivate;


/**
 * Simple main window for demo
 */
class DockWindow : public QMainWindow
{
	Q_OBJECT
private:
	MainWindowPrivate* d;///< private data - pimpl
	friend struct MainWindowPrivate;

protected:
	virtual void closeEvent(QCloseEvent* event) override;

public:
    explicit DockWindow(QWidget *parent = 0, QString name = "");
    virtual ~DockWindow();

    void setWorkName(QString wName);
    void restoreAll();

signals:


private slots:
	void on_actionSaveState_triggered(bool);
	void on_actionRestoreState_triggered(bool);
    void on_actionActive_triggered(bool);

	void savePerspective();
	void onViewToggled(bool Open);
	void onViewVisibilityChanged(bool Visible);

    void createPamTable();
    void createWritePamTable();
    void createChart();

    void onPamReadTableCloseRequested();
    void onPamWriteTableCloseRequested();
    void onChartCloseRequested();

	void showStatusDialog();
	void toggleDockWidgetWindowTitle();
	void applyVsStyle();
	void lockWorkspace(bool Value);

private:
    QString workName = "";
    QHash<QString, QWidget*> winHash;

    QList<TableWin*> readTableList;
    QList<TableWin*> writeTableList;
    QList<PlotWin*> chartList;

private:
    bool saveWins();
    bool restoreWins();

    TableWin *addReadTable(QString winName);
    TableWin *addWriteTable(QString winName);
    PlotWin *addChartWin(QString winName);

    QString genDefaultReadTableWinName();
    QString genDefaultWriteTableWinName();
    QString genDefaultChartWinName();
};

#endif // DOCKWINDOW_H
