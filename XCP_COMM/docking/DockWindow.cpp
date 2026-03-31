
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
#include "DockWindow.h"
#include "ui_dockwindow.h"

#include <iostream>

#include <QTime>
#include <QLabel>
#include <QTextEdit>
#include <QCalendarWidget>
#include <QFrame>
#include <QTreeView>
#include <QFileSystemModel>
#include <QBoxLayout>
#include <QSettings>
#include <QDockWidget>
#include <QDebug>
#include <QResizeEvent>
#include <QAction>
#include <QWidgetAction>
#include <QComboBox>
#include <QInputDialog>
#include <QRubberBand>
#include <QPlainTextEdit>
#include <QTableWidget>
#include <QScreen>
#include <QStyle>
#include <QMessageBox>
#include <QMenu>
#include <QToolButton>
#include <QToolBar>
#include <QPointer>
#include <QMap>
#include <QElapsedTimer>
#include <QQuickWidget>


#if QT_VERSION >= QT_VERSION_CHECK(5, 10, 0)
#include <QRandomGenerator>
#endif

#ifdef Q_OS_WIN
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
#include <QAxWidget>
#endif
#endif

#include "DockManager.h"
#include "DockWidget.h"
#include "DockAreaWidget.h"
#include "DockAreaTitleBar.h"
#include "DockAreaTabBar.h"
#include "FloatingDockContainer.h"
#include "DockComponentsFactory.h"
#include "StatusDialog.h"
#include "DockSplitter.h"
#include "ImageViewer.h"


/**
 * Returns a random number from 0 to highest - 1
 */
int randomNumberBounded(int highest)
{
#if QT_VERSION >= QT_VERSION_CHECK(5, 10, 0)
	return QRandomGenerator::global()->bounded(highest);
#else
	qsrand(QTime::currentTime().msec());
	return qrand() % highest;
#endif
}


/**
 * Function returns a features string with closable (c), movable (m) and floatable (f)
 * features. i.e. The following string is for a not closable but movable and floatable
 * widget: c- m+ f+
 */
static QString featuresString(ads::CDockWidget* DockWidget)
{
	auto f = DockWidget->features();
	return QString("c%1 m%2 f%3")
		.arg(f.testFlag(ads::CDockWidget::DockWidgetClosable) ? "+" : "-")
		.arg(f.testFlag(ads::CDockWidget::DockWidgetMovable) ? "+" : "-")
		.arg(f.testFlag(ads::CDockWidget::DockWidgetFloatable) ? "+" : "-");
}


/**
 * Appends the string returned by featuresString() to the window title of
 * the given DockWidget
 */
static void appendFeaturStringToWindowTitle(ads::CDockWidget* DockWidget)
{
	DockWidget->setWindowTitle(DockWidget->windowTitle()
		+  QString(" (%1)").arg(featuresString(DockWidget)));
}

/**
 * Helper function to create an SVG icon
 */
static QIcon svgIcon(const QString& File)
{
	// This is a workaround, because in item views SVG icons are not
	// properly scaled and look blurry or pixelate
	QIcon SvgIcon(File);
	SvgIcon.addPixmap(SvgIcon.pixmap(92));
	return SvgIcon;
}


//============================================================================
class CCustomComponentsFactory : public ads::CDockComponentsFactory
{
public:
	using Super = ads::CDockComponentsFactory;
	ads::CDockAreaTitleBar* createDockAreaTitleBar(ads::CDockAreaWidget* DockArea) const override
	{
		auto TitleBar = new ads::CDockAreaTitleBar(DockArea);
		auto CustomButton = new QToolButton(DockArea);
		CustomButton->setToolTip(QObject::tr("Help"));
        CustomButton->setIcon(svgIcon(":/dock/images/help_outline.svg"));
		CustomButton->setAutoRaise(true);
		int Index = TitleBar->indexOf(TitleBar->button(ads::TitleBarButtonTabsMenu));
		TitleBar->insertWidget(Index + 1, CustomButton);
		return TitleBar;
	}
};


//===========================================================================
/**
 * Custom QTableWidget with a minimum size hint to test CDockWidget
 * setMinimumSizeHintMode() function of CDockWidget
 */
class CMinSizeTableWidget : public QTableWidget
{
public:
	using QTableWidget::QTableWidget;
	virtual QSize minimumSizeHint() const override
	{
		return QSize(300, 100);
	}
};



//============================================================================
/**
 * Private data class pimpl
 */
struct MainWindowPrivate
{
    DockWindow* _this;
    Ui::DockWindow ui;
	QAction* SavePerspectiveAction = nullptr;
	QWidgetAction* PerspectiveListAction = nullptr;
	QComboBox* PerspectiveComboBox = nullptr;
	ads::CDockManager* DockManager = nullptr;
	ads::CDockWidget* WindowTitleTestDockWidget = nullptr;
	QPointer<ads::CDockWidget> LastDockedEditor;
	QPointer<ads::CDockWidget> LastCreatedFloatingEditor;

    QHash<QString, quint32> winTypeNumHash;
    QHash<QString, QWidget*> winHash;
    QString workName = "";

    MainWindowPrivate(DockWindow* _public) : _this(_public) {}

	/**
	 * Creates the toolbar actions
	 */
	void createActions();


	/**
	 * Saves the dock manager state and the main window geometry
	 */
	void saveState();

	/**
	 * Save the list of perspectives
	 */
	void savePerspectives();

	/**
	 * Restores the dock manager state
	 */
	void restoreState();

	/**
	 * Restore the perspective listo of the dock manager
	 */
	void restorePerspectives();

    /**
     * Creates PamReadTable widget
     */
    ads::CDockWidget* createPamReadTableWidget()
    {
        static int PamReadTableCount = 0;
        TableWin* w = new TableWin();
        w->setProperty("borderless", "true");
        w->style()->unpolish(w);
        w->style()->polish(w);
        ads::CDockWidget* DockWidget = new ads::CDockWidget(QString("R_Table %1").arg(PamReadTableCount++));
        DockWidget->setWidget(w);
        DockWidget->setIcon(svgIcon(":/dock/images/readPamTable.svg"));
        DockWidget->setFeature(ads::CDockWidget::CustomCloseHandling, true);
        //ui.menuView->addAction(DockWidget->toggleViewAction());

        QMenu* OptionsMenu = new QMenu(DockWidget);
        OptionsMenu->setTitle(QObject::tr("Options"));
        OptionsMenu->setToolTip(OptionsMenu->title());
        OptionsMenu->setIcon(svgIcon(":/adsdemo/images/custom-menu-button.svg"));
        auto MenuAction = OptionsMenu->menuAction();
        // The object name of the action will be set for the QToolButton that
        // is created in the dock area title bar. You can use this name for CSS
        // styling
        MenuAction->setObjectName("optionsMenu");
        DockWidget->setTitleBarActions({OptionsMenu->menuAction()});
        //auto a = OptionsMenu->addAction(QObject::tr("Clear Editor"));
        //w->connect(a, SIGNAL(triggered()), SLOT(clear()));

        return DockWidget;
    }

};


//============================================================================
void MainWindowPrivate::createActions()
{
	ui.toolBar->addAction(ui.actionSaveState);
	ui.toolBar->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
    ui.actionSaveState->setIcon(svgIcon(":/dock/images/save.svg"));
	ui.toolBar->addAction(ui.actionRestoreState);
    ui.actionRestoreState->setIcon(svgIcon(":/dock/images/restore.svg"));

	ui.toolBar->addSeparator();

	QAction* a = ui.toolBar->addAction("Lock Workspace");
    a->setIcon(svgIcon(":/dock/images/lock_outline.svg"));
	a->setCheckable(true);
	a->setChecked(false);
    QObject::connect(a, &QAction::triggered, _this, &DockWindow::lockWorkspace);

	PerspectiveListAction = new QWidgetAction(_this);
	PerspectiveComboBox = new QComboBox(_this);
	PerspectiveComboBox->setSizeAdjustPolicy(QComboBox::AdjustToContents);
	PerspectiveComboBox->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
	PerspectiveListAction->setDefaultWidget(PerspectiveComboBox);
	ui.toolBar->addAction(PerspectiveListAction);

	a = SavePerspectiveAction = ui.toolBar->addAction("Create Perspective");
    a->setIcon(svgIcon(":/dock/images/picture_in_picture.svg"));
    QObject::connect(a, &QAction::triggered, _this, &DockWindow::savePerspective);
	ui.toolBar->addAction(SavePerspectiveAction);

	ui.toolBar->addSeparator();
/*
	a = ui.toolBar->addAction("Create Floating Editor");
	a->setProperty("Floating", true);
	a->setToolTip("Creates floating dynamic dockable editor windows that are deleted on close");
    a->setIcon(svgIcon(":/dock/images/note_add.svg"));
	_this->connect(a, SIGNAL(triggered()), SLOT(createEditor()));
	ui.menuTests->addAction(a);

	a = ui.toolBar->addAction("Create Docked Editor");
	a->setProperty("Floating", false);
	a->setToolTip("Creates a docked editor windows that are deleted on close");
    a->setIcon(svgIcon(":/dock/images/docked_editor.svg"));
	_this->connect(a, SIGNAL(triggered()), SLOT(createEditor()));
	ui.menuTests->addAction(a);

	a = ui.toolBar->addAction("Create Editor Tab");
	a->setProperty("Floating", false);
	a->setToolTip("Creates a editor tab and inserts it as second tab into an area");
    a->setIcon(svgIcon(":/dock/images/tab.svg"));
	a->setProperty("Tabbed", true);
	_this->connect(a, SIGNAL(triggered()), SLOT(createEditor()));
	ui.menuTests->addAction(a);
*/
/*
    a = ui.toolBar->addAction("Create PamTable");
    a->setProperty("Floating", false);
    a->setToolTip("Creates a pam table and inserts it as second tab into an area");
    a->setIcon(svgIcon(":/dock/images/readPamTable.svg"));
    a->setProperty("Tabbed", true);
    _this->connect(a, SIGNAL(triggered()), SLOT(createPamTable()));
    ui.menuTests->addAction(a);
*/

    a = ui.toolBar->addAction("Create PamTable");
    auto ToolButton1 = qobject_cast<QToolButton*>(ui.toolBar->widgetForAction(a));
    ToolButton1->setPopupMode(QToolButton::InstantPopup);
    a->setToolTip("Creates floating, docked or pinned pam table");
    a->setIcon(svgIcon(":/dock/images/readPamTable.svg"));
    ui.menuTests->addAction(a);
    auto Menu1 = new QMenu();
    ToolButton1->setMenu(Menu1);
    a = Menu1->addAction("Floating Read PamTable");
    _this->connect(a, SIGNAL(triggered()), SLOT(createPamTable()));
    a = Menu1->addAction("Docked Read PamTable");
    _this->connect(a, SIGNAL(triggered()), SLOT(createPamTable()));
    a = Menu1->addAction("Pinned Read PamTable");
    _this->connect(a, SIGNAL(triggered()), SLOT(createPamTable()));

    a = ui.toolBar->addAction("Create WritePamTable");
    auto ToolButton2 = qobject_cast<QToolButton*>(ui.toolBar->widgetForAction(a));
    ToolButton2->setPopupMode(QToolButton::InstantPopup);
    a->setToolTip("Creates floating, docked or pinned write pam table");
    a->setIcon(svgIcon(":/dock/images/writePamTable.svg"));
    ui.menuTests->addAction(a);
    auto Menu2 = new QMenu();
    ToolButton2->setMenu(Menu2);
    a = Menu2->addAction("Floating Write PamTable");
    _this->connect(a, SIGNAL(triggered()), SLOT(createWritePamTable()));
    a = Menu2->addAction("Docked Write PamTable");
    _this->connect(a, SIGNAL(triggered()), SLOT(createWritePamTable()));
    a = Menu2->addAction("Pinned Write PamTable");
    _this->connect(a, SIGNAL(triggered()), SLOT(createWritePamTable()));

    a = ui.toolBar->addAction("Create Chart");
    auto ToolButton3 = qobject_cast<QToolButton*>(ui.toolBar->widgetForAction(a));
    ToolButton3->setPopupMode(QToolButton::InstantPopup);
    a->setToolTip("Creates floating, docked or pinned chart");
    a->setIcon(svgIcon(":/dock/images/chart.svg"));
    ui.menuTests->addAction(a);
    auto Menu3 = new QMenu();
    ToolButton3->setMenu(Menu3);
    a = Menu3->addAction("Floating Chart");
    _this->connect(a, SIGNAL(triggered()), SLOT(createChart()));
    a = Menu3->addAction("Docked Chart");
    _this->connect(a, SIGNAL(triggered()), SLOT(createChart()));
    a = Menu3->addAction("Pinned Chart");
    _this->connect(a, SIGNAL(triggered()), SLOT(createChart()));

    /*
	ui.toolBar->addSeparator();
	a = ui.toolBar->addAction("Create Floating Table");
	a->setToolTip("Creates floating dynamic dockable table with millions of entries");
    a->setIcon(svgIcon(":/dock/images/grid_on.svg"));
	_this->connect(a, SIGNAL(triggered()), SLOT(createTable()));
	ui.menuTests->addAction(a);

	a = ui.toolBar->addAction("Create Image Viewer");
	auto ToolButton = qobject_cast<QToolButton*>(ui.toolBar->widgetForAction(a));
	ToolButton->setPopupMode(QToolButton::InstantPopup);
	a->setToolTip("Creates floating, docked or pinned image viewer");
    a->setIcon(svgIcon(":/dock/images/panorama.svg"));
	ui.menuTests->addAction(a);
	auto Menu = new QMenu();
	ToolButton->setMenu(Menu);
	a = Menu->addAction("Floating Image Viewer");
	_this->connect(a, SIGNAL(triggered()), SLOT(createImageViewer()));
	a = Menu->addAction("Docked Image Viewer");
	_this->connect(a, SIGNAL(triggered()), SLOT(createImageViewer()));
	a = Menu->addAction("Pinned Image Viewer");
	_this->connect(a, SIGNAL(triggered()), SLOT(createImageViewer()));


	ui.menuTests->addSeparator();
	a = ui.menuTests->addAction("Show Status Dialog");
	_this->connect(a, SIGNAL(triggered()), SLOT(showStatusDialog()));

	a = ui.menuTests->addAction("Toggle Label 0 Window Title");
	_this->connect(a, SIGNAL(triggered()), SLOT(toggleDockWidgetWindowTitle()));
	ui.menuTests->addSeparator();

	a = ui.toolBar->addAction("Apply VS Style");
	a->setToolTip("Applies a Visual Studio light style (visual_studio_light.css)." );
    a->setIcon(svgIcon(":/dock/images/color_lens.svg"));
    QObject::connect(a, &QAction::triggered, _this, &DockWindow::applyVsStyle);
	ui.menuTests->addAction(a);
    */
}


//============================================================================
void MainWindowPrivate::saveState()
{
    QString appPath = QCoreApplication::applicationDirPath();
    QString fileName = "dock_setting-" + workName.toLower() +".ini";
    QString dockSettingFile = appPath + "/work/" + workName + "/" + fileName;

    QSettings Settings(dockSettingFile, QSettings::IniFormat);
    //QSettings Settings("Settings.ini", QSettings::IniFormat);
    Settings.setValue("DockWindow/Geometry", _this->saveGeometry());
    Settings.setValue("DockWindow/State", _this->saveState());
    Settings.setValue("DockWindow/DockingState", DockManager->saveState());
}


//============================================================================
void MainWindowPrivate::restoreState()
{
    QString appPath = QCoreApplication::applicationDirPath();
    QString fileName = "dock_setting-" + workName.toLower() +".ini";
    QString dockSettingFile = appPath + "/work/" + workName + "/" + fileName;

    QSettings Settings(dockSettingFile, QSettings::IniFormat);
    //QSettings Settings("Settings.ini", QSettings::IniFormat);
    _this->restoreGeometry(Settings.value("DockWindow/Geometry").toByteArray());
    _this->restoreState(Settings.value("DockWindow/State").toByteArray());
    DockManager->restoreState(Settings.value("DockWindow/DockingState").toByteArray());
}


//============================================================================
void MainWindowPrivate::savePerspectives()
{
    QString appPath = QCoreApplication::applicationDirPath();
    QString fileName = "dock_setting-" + workName.toLower() +".ini";
    QString dockSettingFile = appPath + "/work/" + workName + "/" + fileName;

    QSettings Settings(dockSettingFile, QSettings::IniFormat);
    //QSettings Settings("Settings.ini", QSettings::IniFormat);
	DockManager->savePerspectives(Settings);
}


//============================================================================
void MainWindowPrivate::restorePerspectives()
{
    QString appPath = QCoreApplication::applicationDirPath();
    QString fileName = "dock_setting-" + workName.toLower() +".ini";
    QString dockSettingFile = appPath + "/work/" + workName + "/" + fileName;

    QSettings Settings(dockSettingFile, QSettings::IniFormat);
    //QSettings Settings("Settings.ini", QSettings::IniFormat);
	DockManager->loadPerspectives(Settings);
	PerspectiveComboBox->clear();
	PerspectiveComboBox->addItems(DockManager->perspectiveNames());
}


//============================================================================
DockWindow::DockWindow(QWidget *parent, QString name) :
	QMainWindow(parent),
    d(new MainWindowPrivate(this)),
    workName(name)
{
	using namespace ads;
	d->ui.setupUi(this);
    setWindowTitle("DockView");
    setWindowIcon(QIcon(":/dock/images/docker.svg"));
	d->createActions();

	// uncomment the following line if the tab close button should be
	// a QToolButton instead of a QPushButton
	// CDockManager::setConfigFlags(CDockManager::configFlags() | CDockManager::TabCloseButtonIsToolButton);

    // uncomment the following line if you want to use opaque undocking and
	// opaque splitter resizing
    //CDockManager::setConfigFlags(CDockManager::DefaultOpaqueConfig);

    // uncomment the following line if you want a fixed tab width that does
	// not change if the visibility of the close button changes
    //CDockManager::setConfigFlag(CDockManager::RetainTabSizeWhenCloseButtonHidden, true);

	// uncomment the following line if you don't want close button on DockArea's title bar
	//CDockManager::setConfigFlag(CDockManager::DockAreaHasCloseButton, false);

	// uncomment the following line if you don't want undock button on DockArea's title bar
	//CDockManager::setConfigFlag(CDockManager::DockAreaHasUndockButton, false);

	// uncomment the following line if you don't want tabs menu button on DockArea's title bar
	//CDockManager::setConfigFlag(CDockManager::DockAreaHasTabsMenuButton, false);

	// uncomment the following line if you don't want disabled buttons to appear on DockArea's title bar
	//CDockManager::setConfigFlag(CDockManager::DockAreaHideDisabledButtons, true);

	// uncomment the following line if you want to show tabs menu button on DockArea's title bar only when there are more than one tab and at least of them has elided title
	//CDockManager::setConfigFlag(CDockManager::DockAreaDynamicTabsMenuButtonVisibility, true);

	// uncomment the following line if you want floating container to always show application title instead of active dock widget's title
	//CDockManager::setConfigFlag(CDockManager::FloatingContainerHasWidgetTitle, false);

	// uncomment the following line if you want floating container to show active dock widget's icon instead of always showing application icon
	//CDockManager::setConfigFlag(CDockManager::FloatingContainerHasWidgetIcon, true);

	// uncomment the following line if you want a central widget in the main dock container (the dock manager) without a titlebar
	// If you enable this code, you can test it in the demo with the Calendar 0
	// dock widget.
	//CDockManager::setConfigFlag(CDockManager::HideSingleCentralWidgetTitleBar, true);

	// uncomment the following line to enable focus highlighting of the dock
	// widget that has the focus
    CDockManager::setConfigFlag(CDockManager::FocusHighlighting, true);

	// uncomment if you would like to enable dock widget auto hiding
    CDockManager::setAutoHideConfigFlags({CDockManager::DefaultAutoHideConfig});

	// uncomment if you would like to enable an equal distribution of the
	// available size of a splitter to all contained dock widgets
	// CDockManager::setConfigFlag(CDockManager::EqualSplitOnInsertion, true);
	
	// uncomment if you would like to close tabs with the middle mouse button, web browser style
	// CDockManager::setConfigFlag(CDockManager::MiddleMouseButtonClosesTab, true);

	// Now create the dock manager and its content
	d->DockManager = new CDockManager(this);
	d->DockManager->setDockWidgetToolBarStyle(Qt::ToolButtonIconOnly, ads::CDockWidget::StateFloating);

 #if (QT_VERSION < QT_VERSION_CHECK(6, 0, 0))
	connect(d->PerspectiveComboBox, SIGNAL(activated(QString)),
		d->DockManager, SLOT(openPerspective(QString)));
 #else
    connect(d->PerspectiveComboBox, SIGNAL(textActivated(QString)),
        d->DockManager, SLOT(openPerspective(QString)));
 #endif

    d->workName = workName;

	// Default window geometry - center on screen
    resize(1280, 720);
    setGeometry(QStyle::alignedRect(
        Qt::LeftToRight, Qt::AlignCenter, frameSize(),
        QGuiApplication::primaryScreen()->availableGeometry()
    ));

    restoreWins();


    d->restorePerspectives();
    d->restoreState();
}


//============================================================================
DockWindow::~DockWindow()
{
    delete d;
}

void DockWindow::setWorkName(QString wName)
{
    d->workName = wName;
}

void DockWindow::restoreAll()
{
    d->restorePerspectives();
    d->restoreState();
}


//============================================================================
void DockWindow::closeEvent(QCloseEvent* event)
{
    d->savePerspectives();
	d->saveState();

    //saveWins();

    // Delete dock manager here to delete all floating widgets. This ensures
    // that all top level windows of the dock manager are properly closed
    //d->DockManager->deleteLater();

	QMainWindow::closeEvent(event);
}


//============================================================================
void DockWindow::on_actionSaveState_triggered(bool)
{
	qDebug() << "MainWindow::on_actionSaveState_triggered";
    saveWins();
    //d->saveState();
}


//============================================================================
void DockWindow::on_actionRestoreState_triggered(bool)
{
	qDebug() << "MainWindow::on_actionRestoreState_triggered";
    restoreWins();
    //d->restoreState();
}

void DockWindow::on_actionActive_triggered(bool)
{

}


//============================================================================
void DockWindow::savePerspective()
{
	QString PerspectiveName = QInputDialog::getText(this, "Save Perspective", "Enter unique name:");
	if (PerspectiveName.isEmpty())
	{
		return;
	}

	d->DockManager->addPerspective(PerspectiveName);
	QSignalBlocker Blocker(d->PerspectiveComboBox);
	d->PerspectiveComboBox->clear();
	d->PerspectiveComboBox->addItems(d->DockManager->perspectiveNames());
	d->PerspectiveComboBox->setCurrentText(PerspectiveName);

	d->savePerspectives();
}


//============================================================================
void DockWindow::onViewToggled(bool Open)
{
	Q_UNUSED(Open);
	auto DockWidget = qobject_cast<ads::CDockWidget*>(sender());
	if (!DockWidget)
	{
		return;
	}

	//qDebug() << DockWidget->objectName() << " viewToggled(" << Open << ")";
}


//============================================================================
void DockWindow::onViewVisibilityChanged(bool Visible)
{
	Q_UNUSED(Visible);
	auto DockWidget = qobject_cast<ads::CDockWidget*>(sender());
    if (!DockWidget)
    {
        return;
    }

    //qDebug() << DockWidget->objectName() << " visibilityChanged(" << Visible << ")";
}


void DockWindow::onPamReadTableCloseRequested()
{
    auto DockWidget = qobject_cast<ads::CDockWidget*>(sender());
    int Result = QMessageBox::question(this, "Close Read PamTable", QString("%1 "
        "contains unsaved changes? Would you like to close it?")
        .arg(DockWidget->windowTitle()));
    if (QMessageBox::Yes == Result)
    {
        TableWin *table = static_cast<TableWin*>(DockWidget->widget());
        QString winName = table->getWinName();
        qDebug()<<"remove a pam table:"<<winName;
        winHash.remove(winName);
        readTableList.removeOne(table);

        DockWidget->closeDockWidget(); 
    }
}

void DockWindow::onPamWriteTableCloseRequested()
{
    auto DockWidget = qobject_cast<ads::CDockWidget*>(sender());
    int Result = QMessageBox::question(this, "Close Write PamTable", QString("%1 "
        "contains unsaved changes? Would you like to close it?")
        .arg(DockWidget->windowTitle()));
    if (QMessageBox::Yes == Result)
    {
        TableWin *table = static_cast<TableWin*>(DockWidget->widget());
        QString winName = table->getWinName();
        qDebug()<<"remove a write pam table:"<<winName;
        winHash.remove(winName);
        writeTableList.removeOne(table);

        DockWidget->closeDockWidget();
    }
}

void DockWindow::onChartCloseRequested()
{
    auto DockWidget = qobject_cast<ads::CDockWidget*>(sender());
    int Result = QMessageBox::question(this, "Close Chart", QString("%1 "
        "contains unsaved changes? Would you like to close it?")
        .arg(DockWidget->windowTitle()));
    if (QMessageBox::Yes == Result)
    {
        PlotWin *chart = static_cast<PlotWin*>(DockWidget->widget());
        QString winName = chart->getWinName();
        qDebug()<<"remove a pam table:"<<winName;
        winHash.remove(winName);
        chartList.removeOne(chart);

        DockWidget->closeDockWidget();
    }
}


//============================================================================
void DockWindow::createPamTable()
{

    QAction* a = qobject_cast<QAction*>(sender());
    qDebug() << "createReadTable " << a->text();

    /*
    auto DockWidget = d->createPamReadTableWidget();
    DockWidget->setFeature(ads::CDockWidget::DockWidgetDeleteOnClose, true);
    DockWidget->setFeature(ads::CDockWidget::DockWidgetForceCloseWithArea, true);
    DockWidget->setFeature(ads::CDockWidget::CustomCloseHandling, true);
    DockWidget->resize(QSize(420, 480));
    connect(DockWidget, &ads::CDockWidget::closeRequested, this,
        &DockWindow::onPamReadTableCloseRequested);

    if (a->text().startsWith("Floating"))
    {
        auto FloatingWidget = d->DockManager->addDockWidgetFloating(DockWidget);
        FloatingWidget->move(QPoint(20, 20));
    }
    else if (a->text().startsWith("Docked"))
    {
        d->DockManager->addDockWidget(ads::LeftDockWidgetArea, DockWidget);
    }
    else if (a->text().startsWith("Pinned"))
    {
        d->DockManager->addAutoHideDockWidget(ads::SideBarLeft, DockWidget);
    }
    else
    {
        d->DockManager->addDockWidget(ads::LeftDockWidgetArea, DockWidget);
    }

    d->LastDockedEditor = DockWidget;
    */

    QString winName = genDefaultReadTableWinName();
    if(winName.isEmpty()) return;

    TableWin *table = addReadTable(winName);

}

void DockWindow::createWritePamTable()
{

    QAction* a = qobject_cast<QAction*>(sender());
    qDebug() << "createPamWriteTable " << a->text();

    /*
    auto DockWidget = d->createPamWriteTableWidget();
    DockWidget->setFeature(ads::CDockWidget::DockWidgetDeleteOnClose, true);
    DockWidget->setFeature(ads::CDockWidget::DockWidgetForceCloseWithArea, true);
    DockWidget->setFeature(ads::CDockWidget::CustomCloseHandling, true);
    DockWidget->resize(QSize(420, 480));
    connect(DockWidget, &ads::CDockWidget::closeRequested, this,
        &DockWindow::onPamWriteTableCloseRequested);

    if (a->text().startsWith("Floating"))
    {
        auto FloatingWidget = d->DockManager->addDockWidgetFloating(DockWidget);
        FloatingWidget->move(QPoint(20, 20));
    }
    else if (a->text().startsWith("Docked"))
    {
        d->DockManager->addDockWidget(ads::RightDockWidgetArea, DockWidget);
    }
    else if (a->text().startsWith("Pinned"))
    {
        d->DockManager->addAutoHideDockWidget(ads::SideBarLeft, DockWidget);
    }
    */

    QString winName = genDefaultWriteTableWinName();
    if(winName.isEmpty()) return;

    TableWin *table = addWriteTable(winName);
}

void DockWindow::createChart()
{

    QAction* a = qobject_cast<QAction*>(sender());
    qDebug() << "createChart " << a->text();

    /*
    auto DockWidget = d->createChartWidget();
    DockWidget->setFeature(ads::CDockWidget::DockWidgetDeleteOnClose, true);
    DockWidget->setFeature(ads::CDockWidget::DockWidgetForceCloseWithArea, true);
    DockWidget->setFeature(ads::CDockWidget::CustomCloseHandling, true);
    DockWidget->resize(QSize(640, 320));
    connect(DockWidget, &ads::CDockWidget::closeRequested, this,
        &DockWindow::onChartCloseRequested);

    if (a->text().startsWith("Floating"))
    {
        auto FloatingWidget = d->DockManager->addDockWidgetFloating(DockWidget);
        FloatingWidget->move(QPoint(20, 20));
    }
    else if (a->text().startsWith("Docked"))
    {
        d->DockManager->addDockWidget(ads::TopDockWidgetArea, DockWidget);
    }
    else if (a->text().startsWith("Pinned"))
    {
        d->DockManager->addAutoHideDockWidget(ads::SideBarLeft, DockWidget);
    }
    */

    QString winName = genDefaultChartWinName();
    if(winName.isEmpty()) return;

    PlotWin *chart = addChartWin(winName);
}

//============================================================================
void DockWindow::showStatusDialog()
{
	CStatusDialog Dialog(d->DockManager);
	Dialog.exec();
}


//============================================================================
void DockWindow::toggleDockWidgetWindowTitle()
{
	QString Title = d->WindowTitleTestDockWidget->windowTitle();
	int i = Title.indexOf(" (Test)");
	if (-1 == i)
	{
		Title += " (Test)";
	}
	else
	{
		Title = Title.left(i);
	}
	d->WindowTitleTestDockWidget->setWindowTitle(Title);
}


//============================================================================
void DockWindow::applyVsStyle()
{
    QFile StyleSheetFile(":dock/res/visual_studio_light.css");
	StyleSheetFile.open(QIODevice::ReadOnly);
	QTextStream StyleSheetStream(&StyleSheetFile);
	auto Stylesheet = StyleSheetStream.readAll();
	StyleSheetFile.close();
	d->DockManager->setStyleSheet(Stylesheet);
}

//============================================================================
void DockWindow::lockWorkspace(bool Value)
{
	if (Value)
	{
		d->DockManager->lockDockWidgetFeaturesGlobally();
	}
	else
	{
		d->DockManager->lockDockWidgetFeaturesGlobally(ads::CDockWidget::NoDockWidgetFeatures);
    }
}

bool DockWindow::saveWins()
{
    QString appPath = QApplication::applicationDirPath();
    QString fileName = "dock_config-" + workName.toLower() +".xml";
    QString winXmlPath = appPath + "/work/" + workName + "/" + fileName;

    if(winXmlPath.isEmpty())
        return false;

    QFile xmlFile(winXmlPath);
    if (!xmlFile.open(QFile::WriteOnly | QFile::Text))
    {
        qDebug()<<tr("Cannot write file %1:\n%2.").arg(winXmlPath).arg(xmlFile.errorString());
        return false;
    }
    qDebug()<<"=== Write dock config xml file:"<<winXmlPath;

    QDomDocument xml("WIN_CONFIG");

    QDomElement root = xml.createElement("Root");
    xml.appendChild(root);

    QDomElement readTableRootElem = xml.createElement("TableReadRoot");

    root.appendChild(readTableRootElem);

    for(int i = 0; i < readTableList.count(); i++)
    {
        TableWin *table = readTableList.at(i);

        QDomElement readTableElem = xml.createElement("ReadTable_" + QString::number(i));
        readTableRootElem.appendChild(readTableElem);

        readTableElem.setAttribute("WinName", table->getWinName());

        QList<PamInfo> pams = table->getReadPams();
        for(int j = 0; j < pams.count(); j++)
        {
            PamInfo pamInfo = pams.at(j);
            QDomElement pamElem = xml.createElement("Pam_" + QString::number(j));
            readTableElem.appendChild(pamElem);

            pamElem.setAttribute("ModName", pamInfo.modName);
            pamElem.setAttribute("PamName", pamInfo.pamName);
        }
    }


    QDomElement writeTableRootElem = xml.createElement("TableWriteRoot");

    root.appendChild(writeTableRootElem);

    for(int i = 0; i < writeTableList.count(); i++)
    {
        TableWin *table = writeTableList.at(i);

        QDomElement writeTableElem = xml.createElement("WriteTable_" + QString::number(i));
        writeTableRootElem.appendChild(writeTableElem);

        writeTableElem.setAttribute("WinName", table->getWinName());

        QList<PamInfo> pams = table->getWritePams();
        for(int j = 0; j < pams.count(); j++)
        {
            PamInfo pamInfo = pams.at(j);
            QDomElement pamElem = xml.createElement("Pam_" + QString::number(j));
            writeTableElem.appendChild(pamElem);

            pamElem.setAttribute("ModName", pamInfo.modName);
            pamElem.setAttribute("PamName", pamInfo.pamName);
            pamElem.setAttribute("PamSetValue", QString::number(pamInfo.pamSetValue, 'g', 8));
            //qDebug()<<"pam setValue:"<<pamInfo.pamSetValue;
        }
    }

    QDomElement plotWinRootElem = xml.createElement("PlotWinRoot");

    root.appendChild(plotWinRootElem);

    for(int i = 0; i < chartList.count(); i++)
    {
        PlotWin *plot = chartList.at(i);

        QDomElement plotWinElem = xml.createElement("PlotWin_" + QString::number(i));
        plotWinRootElem.appendChild(plotWinElem);

        plotWinElem.setAttribute("WinName", plot->getWinName());

        ChartProp chartProp = plot->getChartProp();
        plotWinElem.setAttribute("timeGap", chartProp.timeGap);

        QList<PamInfo> pams = plot->getPamInfoList();
        for(int j = 0; j < pams.count(); j++)
        {
            PamInfo pamInfo = pams.at(j);
            QDomElement pamElem = xml.createElement("Pam_" + QString::number(j));
            plotWinElem.appendChild(pamElem);

            pamElem.setAttribute("ModName", pamInfo.modName);
            pamElem.setAttribute("PamName", pamInfo.pamName);

            pamElem.setAttribute("AutoScale", pamInfo.autoScale ? 1 : 0);
            pamElem.setAttribute("ShowMin", pamInfo.showMin);
            pamElem.setAttribute("ShowMax", pamInfo.showMax);

        }
    }

    QString xmlString = xml.toString();
    QTextStream out(&xmlFile);
    out<<xmlString;

    return true;
}

bool DockWindow::restoreWins()
{
    QString appPath = QApplication::applicationDirPath();
    QString fileName = "dock_config-" + workName.toLower() +".xml";
    QString winXmlPath = appPath + "/work/" + workName + "/" + fileName;


    if(winXmlPath.isEmpty())
        return false;

    QFile xmlFile(winXmlPath);
    if (!xmlFile.open(QFile::ReadOnly | QFile::Text))
    {
        qDebug()<<tr("Cannot read file %1:\n%2.").arg(winXmlPath).arg(xmlFile.errorString());
        return false;
    }
    qDebug()<<"=== Read dock config xml file:"<<winXmlPath;

    QDomDocument xml("WIN_CONFIG");
    if(!xml.setContent(&xmlFile))
    {
        xmlFile.close();
        return false;
    }
    xmlFile.close();

    QDomElement rootElem = xml.documentElement();

    QDomElement elem = rootElem.firstChildElement();

    while(!elem.isNull())
    {
        QString elemStr = elem.tagName();

        if(elemStr == "TableReadRoot")
        {
            QDomElement readTableElem = elem.firstChildElement();

            while(!readTableElem.isNull())
            {
                QString winName = readTableElem.attribute("WinName");


                TableWin *table = addReadTable(winName);
                if(!table) continue;

                QList<PamInfo> pamInfoList;

                QDomElement pamElem = readTableElem.firstChildElement();

                while(!pamElem.isNull())
                {
                    QString modName = pamElem.attribute("ModName");
                    QString pamName = pamElem.attribute("PamName");

                    PamInfo pamInfo;
                    pamInfo.modName = modName;
                    pamInfo.pamName = pamName;
                    pamInfoList.append(pamInfo);

                    pamElem = pamElem.nextSiblingElement();
                }

                table->setReadPams(pamInfoList);
                table->Slt_RunActive(true);

                readTableElem = readTableElem.nextSiblingElement();
            }

        }
        else if(elemStr == "TableWriteRoot")
        {
            QDomElement writeTableElem = elem.firstChildElement();

            while(!writeTableElem.isNull())
            {
                QString winName = writeTableElem.attribute("WinName");

                TableWin *table = addWriteTable(winName);

                QList<PamInfo> pamInfoList;

                QDomElement pamElem = writeTableElem.firstChildElement();

                while(!pamElem.isNull())
                {
                    QString modName = pamElem.attribute("ModName");
                    QString pamName = pamElem.attribute("PamName");
                    double pamSetValue = pamElem.attribute("PamSetValue").toDouble();

                    PamInfo pamInfo;
                    pamInfo.modName = modName;
                    pamInfo.pamName = pamName;
                    pamInfo.pamSetValue = pamSetValue;
                    pamInfoList.append(pamInfo);

                    pamElem = pamElem.nextSiblingElement();
                }

                table->setWritePams(pamInfoList);
                table->Slt_RunActive(true);

                writeTableElem = writeTableElem.nextSiblingElement();
            }
        }
        else if(elemStr == "PlotWinRoot")
        {
            QDomElement plotWinElem = elem.firstChildElement();

            while(!plotWinElem.isNull())
            {
                QString winName = plotWinElem.attribute("WinName");

                PlotWin *plot = addChartWin(winName);

                ChartProp chartProp;
                chartProp.timeGap = plotWinElem.attribute("timeGap").toUInt();

                plot->setChartProp(chartProp);

                QList<PamInfo> pamInfoList;

                QDomElement pamElem = plotWinElem.firstChildElement();

                while(!pamElem.isNull())
                {
                    QString modName = pamElem.attribute("ModName");
                    QString pamName = pamElem.attribute("PamName");
                    bool autoScale = pamElem.attribute("AutoScale").toUInt() > 1;
                    double showMin = pamElem.attribute("ShowMin").toDouble();
                    double showMax = pamElem.attribute("ShowMax").toDouble();

                    PamInfo pamInfo;
                    pamInfo.modName = modName;
                    pamInfo.pamName = pamName;
                    pamInfo.autoScale = autoScale;
                    pamInfo.showMin = showMin;
                    pamInfo.showMax = showMax;

                    pamInfoList.append(pamInfo);

                    pamElem = pamElem.nextSiblingElement();
                }

                plot->setPamInfoList(pamInfoList);
                plot->Slt_RunActive(true);

                plotWinElem = plotWinElem.nextSiblingElement();
            }

        }

        elem = elem.nextSiblingElement();
    }

    return true;
}


TableWin *DockWindow::addReadTable(QString winName)
{
    if(winName.isEmpty()) return NULL;

    TableWin *table_ = (TableWin*)(winHash.value(winName));
    if(table_) return table_;

    TableWin *table = new TableWin(nullptr, winName, 0);

    //connect(this, SIGNAL(runActive(bool)), table, SLOT(Slt_RunActive(bool)));
    //connect(table, SIGNAL(winDeleted(QWidget*)), this, SLOT(Slt_TableWinDeleted(QWidget*)));
    //connect(table, SIGNAL(winUpdated()), this, SLOT(Slt_WinUpdated()));

    readTableList.append(table);
    winHash.insert(table->getWinName(), table);

    table->hideMenu();
    table->setProperty("borderless", "true");
    table->style()->unpolish(table);
    table->style()->polish(table);

    ads::CDockWidget* DockWidget = new ads::CDockWidget(winName);
    DockWidget->setWidget(table);
    DockWidget->setIcon(svgIcon(":/dock/images/readPamTable.svg"));
    d->ui.menuView->addAction(DockWidget->toggleViewAction());

    DockWidget->setFeature(ads::CDockWidget::DockWidgetDeleteOnClose, true);
    DockWidget->setFeature(ads::CDockWidget::DockWidgetForceCloseWithArea, true);
    DockWidget->setFeature(ads::CDockWidget::CustomCloseHandling, true);
    DockWidget->resize(QSize(420, 480));
    connect(DockWidget, &ads::CDockWidget::closeRequested, this,
        &DockWindow::onPamReadTableCloseRequested);

    d->DockManager->addDockWidget(ads::LeftDockWidgetArea, DockWidget);

    return table;
}

TableWin *DockWindow::addWriteTable(QString winName)
{

    if(winName.isEmpty()) return NULL;

    TableWin *table_ = (TableWin*)(winHash.value(winName));
    if(table_) return table_;

    TableWin *table = new TableWin(nullptr, winName, 1);

    //connect(this, SIGNAL(runActive(bool)), table, SLOT(Slt_RunActive(bool)));
    //connect(table, SIGNAL(winDeleted(QWidget*)), this, SLOT(Slt_TableWinDeleted(QWidget*)));
    //connect(table, SIGNAL(winUpdated()), this, SLOT(Slt_WinUpdated()));

    writeTableList.append(table);
    winHash.insert(table->getWinName(), table);

    table->hideMenu();
    table->setProperty("borderless", "true");
    table->style()->unpolish(table);
    table->style()->polish(table);

    ads::CDockWidget* DockWidget = new ads::CDockWidget(winName);
    DockWidget->setWidget(table);
    DockWidget->setIcon(svgIcon(":/dock/images/writePamTable.svg"));
    d->ui.menuView->addAction(DockWidget->toggleViewAction());

    DockWidget->setFeature(ads::CDockWidget::DockWidgetDeleteOnClose, true);
    DockWidget->setFeature(ads::CDockWidget::DockWidgetForceCloseWithArea, true);
    DockWidget->setFeature(ads::CDockWidget::CustomCloseHandling, true);
    DockWidget->resize(QSize(420, 480));
    connect(DockWidget, &ads::CDockWidget::closeRequested, this,
        &DockWindow::onPamWriteTableCloseRequested);

    d->DockManager->addDockWidget(ads::RightDockWidgetArea, DockWidget);

    return table;
}

PlotWin *DockWindow::addChartWin(QString winName)
{
    if(winName.isEmpty()) return NULL;

    PlotWin *chart_ = (PlotWin*)(winHash.value(winName));
    if(chart_) return chart_;

    PlotWin *chart = new PlotWin(nullptr);
    chart->setWinName(winName);

    //connect(this, SIGNAL(runActive(bool)), chart, SLOT(Slt_RunActive(bool)));
    //connect(chart, SIGNAL(winDeleted(QWidget*)), this, SLOT(Slt_TableWinDeleted(QWidget*)));
    //connect(chart, SIGNAL(winUpdated()), this, SLOT(Slt_WinUpdated()));

    chartList.append(chart);
    winHash.insert(chart->getWinName(), chart);

    chart->setProperty("borderless", "true");
    chart->style()->unpolish(chart);
    chart->style()->polish(chart);

    ads::CDockWidget* DockWidget = new ads::CDockWidget(winName);
    DockWidget->setWidget(chart);
    DockWidget->setIcon(svgIcon(":/dock/images/chart.svg"));
    d->ui.menuView->addAction(DockWidget->toggleViewAction());

    DockWidget->setFeature(ads::CDockWidget::DockWidgetDeleteOnClose, true);
    DockWidget->setFeature(ads::CDockWidget::DockWidgetForceCloseWithArea, true);
    DockWidget->setFeature(ads::CDockWidget::CustomCloseHandling, true);
    DockWidget->resize(QSize(640, 320));
    connect(DockWidget, &ads::CDockWidget::closeRequested, this,
        &DockWindow::onChartCloseRequested);

    d->DockManager->addDockWidget(ads::TopDockWidgetArea, DockWidget);

    return chart;
}

QString DockWindow::genDefaultReadTableWinName()
{
    for(int i = 0; i < 100; i++)
    {
        QString winName = QString("R_Table %1").arg(i);
        if(winHash.uniqueKeys().indexOf(winName) == -1)
            return winName;
    }

    return "";
}

QString DockWindow::genDefaultWriteTableWinName()
{
    for(int i = 0; i < 100; i++)
    {
        QString winName = QString("W_Table %1").arg(i);
        if(winHash.uniqueKeys().indexOf(winName) == -1)
            return winName;
    }

    return "";
}

QString DockWindow::genDefaultChartWinName()
{
    for(int i = 0; i < 100; i++)
    {
        QString winName = QString("Chart %1").arg(i);
        if(winHash.uniqueKeys().indexOf(winName) == -1)
            return winName;
    }

    return "";
}

