/***************************************************************************
*   Copyright (C) 2011 Zoltan Padrah <zoltan_padrah@users.sourceforge.net>*
*                                                                         *
*   This program is free software; you can redistribute it and/or modify  *
*   it under the terms of the GNU General Public License as published by  *
*   the Free Software Foundation; either version 2 of the License, or     *
*   (at your option) any later version.                                   *
***************************************************************************/

#ifndef KTL_GUI_PLUGIN_H
#define KTL_GUI_PLUGIN_H

#include "interfaces/iguiplugin.h"

#include <kdevplatform/interfaces/iplugin.h>
#include <QVariantList>

namespace KTechLab
{

class NewFileDlg;

    class KTLGuiPlugin : public KDevelop::IPlugin, IGuiPlugin
    {
        Q_OBJECT
        /* IGuiPlugin interface */
	public:
		virtual void addFiletypeToNewFileDialog(
							const QListWidgetItem &item,
							const QObject *receiver,
							const char *slot
							);

		virtual int removeFiletypeFromNewFileDialog(
							const QObject *receiver,
							const char *slot
							);

		/* rest of the plugin */
        public:
            explicit KTLGuiPlugin( QObject *parent = 0,
                                        const QVariantList &args = QVariantList() );
            virtual ~KTLGuiPlugin();

            virtual void createActionsForMainWindow(
                                                    Sublime::MainWindow* window,
                                                    QString& xmlFile,
                                                    KActionCollection& actions);

        public slots:
            void slotFileOpen();
            void slotFileNew();
            void slotFileNewAssembly();
            void slotFileNewC();
            void slotFileNewCircuit();
            void slotFileNewFlowCode();
            void slotFileNewMechanics();
            void slotFileNewMicrobe();

        private:
            NewFileDlg *m_newFileDlg;
            QWidget *m_mainWindow;

    };

}

#endif
