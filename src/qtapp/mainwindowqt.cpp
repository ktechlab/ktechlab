/*
KTechLab, an IDE for electronics
Copyright (C) 2010 Zoltan Padrah
    zoltan_padrah@users.sourceforge.net

    This program is free software;
you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation;
either version 2 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
     but WITHOUT ANY WARRANTY;
without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License along
with this program;
if not, write to the Free Software Foundation, Inc.,
   51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
*/

#include "mainwindowqt.h"

#include <QApplication>
#include <QMenu>
#include <QMenuBar>
#include <QMessageBox>
#include <QUrl>

#include "plugins/circuit/circuitview.h"
#include <plugins/circuit/circuitdocument.h>

using namespace KTechLab;

MainWindowQt::MainWindowQt(QApplication & app): QMainWindow(), qtApp(app)
{
	createActions();
	createMenus();
	createToolBars();

	circDoc = new CircuitDocument(QUrl(""));
	circView = new CircuitView(circDoc);

	setCentralWidget(circView);
}

void MainWindowQt::createActions() {

	exitAct = new QAction(tr("E&xit"), this);
	exitAct->setShortcuts(QKeySequence::Quit);
	exitAct->setStatusTip(tr("Exit the application"));
	connect(exitAct, SIGNAL(triggered()), this, SLOT(close()));


	aboutAct = new QAction(tr("&About"), this);
	aboutAct->setStatusTip(tr("Show the application's About box"));
	connect(aboutAct, SIGNAL(triggered()), this, SLOT(about()));

	aboutQtAct = new QAction(tr("About &Qt"), this);
	aboutQtAct->setStatusTip(tr("Show the Qt library's About box"));
	connect(aboutQtAct, SIGNAL(triggered()), qApp, SLOT(aboutQt()));
}

void MainWindowQt::createMenus()
{
	fileMenu = menuBar()->addMenu(tr("&File"));
	fileMenu->addAction(exitAct);

	editMenu = menuBar()->addMenu(tr("&Edit"));

	menuBar()->addSeparator();

	helpMenu = menuBar()->addMenu(tr("&Help"));
	helpMenu->addAction(aboutAct);
	helpMenu->addAction(aboutQtAct);
}

void MainWindowQt::createToolBars()
{
	fileToolBar = addToolBar(tr("File"));
	editToolBar = addToolBar(tr("Edit"));
}

void MainWindowQt::about()
{
   QMessageBox::about(this, tr("About KTechLab"),
             tr("An IDE for microcontrollers and electronics. "
             "This is the Qt version, without KDE dependencies"));

}
