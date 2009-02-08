/******************************************************************************
 * Main Window Class                                                          *
 *                                                                            *
 * Copyright (C) 2005 by David Saxton <david@bluehaze.org>                    *
 * Copyright (C) 2008 Julian Bäume <julian@svg4all.de>                        *
 *                                                                            *
 * This program is free software; you can redistribute it and/or              *
 * modify it under the terms of the GNU General Public License as             *
 * published by the Free Software Foundation; either version 2 of             *
 * the License, or (at your option) any later version.                        *
 *                                                                            *
 * This program is distributed in the hope that it will be useful,            *
 * but WITHOUT ANY WARRANTY; without even the implied warranty of             *
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the              *
 * GNU General Public License for more details.                               *
 *                                                                            *
 * You should have received a copy of the GNU General Public License          *
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.      *
 ******************************************************************************/
#ifndef KTECHLAB_H
#define KTECHLAB_H

#include <sublime/mainwindow.h>
#include <Plasma/View>

class QMenu;
class QSplitter;

class KUrl;
class KTextEdit;
class KActionMenu;
class KTabWidget;

namespace Sublime {
    class Area;
    class Controller;
}

/**
 * This class serves as the main window for KTechlab.  It handles the
 * menus, toolbars, status bars, loading/saving files, config, etc.
 *
 * @short Main window class
 * @author Julian Bäume
 */
class KTechlab : public Sublime::MainWindow
{
    Q_OBJECT
    public:
        explicit KTechlab( Sublime::Controller *controller, Qt::WindowFlags flags = KDE_DEFAULT_WINDOWFLAGS );
        ~KTechlab();

        void init();

        KTabWidget *tabWidget();
        void load( KUrl url );

    public slots:
        void slotFileNewAssembly();
        void slotFileNewMicrobe();
        void slotFileNewC();
        void slotFileNewCircuit();
        void slotFileNewFlowCode();
        void slotFileNewMechanics();
        void slotFileNew();
        void slotFileOpen();
        void slotFileOpenRecent();
        void slotFileSave();
        void slotFileSaveAs();
        void slotFilePrint();
        void slotFileQuit();
        void slotViewClose();
        void slotUndo();
        void slotRedo();
        void slotCut();
        void slotCopy();
        void slotPaste();
        void slotComponentRotateCCW();
        void slotComponentRotateCW();

    private:
        void setupActions();
        void createMenus();
        void createActions();
        void createToolBars();
        void createStatusBar();

        void readSettings();

        QMenu *fileMenu;
        KActionMenu *m_newActionMenu;

        Sublime::Area *m_mainArea;
};

#endif
