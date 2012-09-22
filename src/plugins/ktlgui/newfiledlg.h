/***************************************************************************
 *   Copyright (C) 2005 by David Saxton <david@bluehaze.org>               *
 *   Copyright (C) 2008 Julian Bäume <julian@svg4all.de>                   *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#ifndef NEWFILEDLG_H
#define NEWFILEDLG_H

#include <KDialog>

namespace Ui
{
    class NewFileWidget;
}

class MicroSelectWidget;
class QIconViewItem;
class QListWidgetItem;

namespace KTechLab
{

/**
A standard dialog for getting file details from the user for a new project
@short Dialog for new file details
@author David Saxton
*/
class NewFileDlg : public KDialog
{
    Q_OBJECT
    public:
        NewFileDlg( QWidget *parent );

        /**
         Method called when the user cancels the dialog box, for example by
         pressing the cancel button
         */
        void reject();
        void accept();

        bool accepted() const { return m_bAccepted; }
        bool addToProject() const { return m_bAddToProject; }
        QString microID() const { return m_microID; }

        MicroSelectWidget * microSelectWidget() const;

		void addFiletypeToNewFileDialog(
							QListWidgetItem *item,
							const QObject *receiver,
							const char *slot
							);

		int removeFiletypeFromNewFileDialog(
							const QObject *receiver,
							const char *slot
							);

    public slots:
        void fileTypeChanged();

    signals:
        void signalFileNewAssembly();
        void signalFileNewC();
        void signalFileNewCircuit();
        void signalFileNewFlowCode();
        void signalFileNewMechanics();
        void signalFileNewMicrobe();

    protected:
        bool m_bAccepted;
        bool m_bAddToProject;
        QString m_microID;

        Ui::NewFileWidget * m_pNewFileWidget;
        QWidget * m_pMainParent;

		QList<const QObject*> m_receiverList;
		QList<const char *> m_slotList;


};

}

#endif
