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

#ifndef MAINWINDOWQT_H
#define MAINWINDOWQT_H

#include <QMainWindow>

class QApplication;
class QMenu;
class QToolBar;

namespace KTechLab {

class CircuitDocument;
class CircuitView;

class MainWindowQt : public QMainWindow
{
	Q_OBJECT
public:
    MainWindowQt(QApplication & app);

private slots:
	void about();

private:
	void createActions();
	void createToolBars();
	void createMenus();

	QApplication & qtApp;

	CircuitDocument * circDoc;
	CircuitView * circView;

	QAction *exitAct;
	QAction *aboutAct;
    QAction *aboutQtAct;

    QMenu *fileMenu;
    QMenu *editMenu;
    QMenu *helpMenu;

	QToolBar *fileToolBar;
    QToolBar *editToolBar;


};

}

#endif // MAINWINDOWQT_H
