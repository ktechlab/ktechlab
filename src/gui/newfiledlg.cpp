/***************************************************************************
 *   Copyright (C) 2003-2005 by David Saxton                               *
 *   david@bluehaze.org                                                    *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#include "config.h"
#include "document.h"
#include "microinfo.h"
#include "newfiledlg.h"
#include "microlibrary.h"
#include "microselectwidget.h"
#include "projectmanager.h"
#include "textdocument.h"

#include <kcombobox.h>
#include <kdebug.h>
#include <klineedit.h>
#include <k3iconview.h>
#include <klocalizedstring.h>
#include <kiconloader.h>

#include <canvas.h>
#include <qcheckbox.h>
#include <qdir.h>
#include <qfile.h>
#include <qlabel.h>
#include <q3paintdevicemetrics.h>

#include <ui_newfilewidget.h>


struct NewFileWidget : public QWidget, Ui::NewFileWidget {
    NewFileWidget(QWidget *parent) : QWidget(parent) {
        setupUi(this);
    }
};

NewFileDlg::NewFileDlg( QWidget *parent )
    :
    //KDialog( parent, "newfiledlg", true, "New File", KDialog::Ok|KDialog::Cancel, KDialog::Ok, true )
    KDialog( parent ) // , "newfiledlg", true, "New File", KDialog::Ok|KDialog::Cancel, KDialog::Ok, true )
{
    setName("newfiledlg");
    setModal(true);
    setCaption(i18n("New File"));
    setButtons(KDialog::Ok | KDialog::Cancel);
    setDefaultButton(KDialog::Ok);
    showButtonSeparator(true);

	m_pMainParent = parent;
    m_bAccepted = false;
    m_pNewFileWidget = new NewFileWidget(this);

	 m_pNewFileWidget->typeIconView->setSelectionMode(Q3IconView::Single);
	 m_pNewFileWidget->typeIconView->setMode(K3IconView::Select);
    
	KIconLoader *loader = KIconLoader::global();
	
	QList<Q3IconViewItem*> items;
	
	//BEGIN insert icons
	QString text = QString("%1 (.asm)").arg(i18n("Assembly Code"));
	items << new Q3IconViewItem(m_pNewFileWidget->typeIconView, text, loader->loadIcon( "source", KIconLoader::NoGroup, KIconLoader::SizeHuge ) );
	
	text = "C (.c)";
	items << new Q3IconViewItem(m_pNewFileWidget->typeIconView, text, loader->loadIcon( "text-x-csrc", KIconLoader::NoGroup, KIconLoader::SizeHuge ) );
	
	text = QString("%1 (.circuit)").arg(i18n("Circuit"));
	items << new Q3IconViewItem(m_pNewFileWidget->typeIconView,text, loader->loadIcon( "application-x-circuit", KIconLoader::NoGroup, KIconLoader::SizeHuge ) );
	
	items << new Q3IconViewItem(m_pNewFileWidget->typeIconView,"FlowCode (.flowcode)", loader->loadIcon( "application-x-flowcode", KIconLoader::NoGroup, KIconLoader::SizeHuge ) );
	
#ifdef MECHANICS
	items << new Q3IconViewItem(m_pNewFileWidget->typeIconView,"Mechanics (.mechanics)", loader->loadIcon( "exec", KIconLoader::NoGroup, KIconLoader::SizeHuge ) );
#endif
	
	items << new Q3IconViewItem(m_pNewFileWidget->typeIconView,"Microbe (.microbe)", loader->loadIcon( "application-x-microbe", KIconLoader::NoGroup, KIconLoader::SizeHuge ) );
	//END insert icons
	
	unsigned minWidth = 20 + m_pNewFileWidget->typeIconView->spacing() * items.size();
	int minHeight = 0;
	
	const QList<Q3IconViewItem*>::iterator end = items.end();
	for ( QList<Q3IconViewItem*>::iterator it = items.begin(); it != end; ++it )
	{
		(*it)->setDragEnabled(false);
		minWidth += (*it)->width();
		minHeight = qMax( minHeight, (*it)->height()+20 );
	}
	
	m_pNewFileWidget->typeIconView->setMinimumSize( minWidth, minHeight );
	m_pNewFileWidget->typeIconView->setCurrentItem(items[3]);
	m_pNewFileWidget->addToProjectCheck->setChecked( ProjectManager::self()->currentProject() );
	m_pNewFileWidget->addToProjectCheck->setEnabled( ProjectManager::self()->currentProject() );
	microSelectWidget()->setAllowedFlowCodeSupport( MicroInfo::FullSupport | MicroInfo::PartialSupport );
    
	setMainWidget(m_pNewFileWidget);
    
	// Our behaviour is to have single click selects and double click accepts the dialog
	connect( m_pNewFileWidget->typeIconView, SIGNAL(selectionChanged(Q3IconViewItem*)), this, SLOT(fileTypeChanged(Q3IconViewItem*)) );
	connect( m_pNewFileWidget->typeIconView, SIGNAL(doubleClicked(Q3IconViewItem*)), this, SLOT(accept()));

    setAcceptDrops(true);

	m_pNewFileWidget->typeIconView->adjustSize();
	m_pNewFileWidget->adjustSize();
	adjustSize();
}

void NewFileDlg::accept()
{
	hide();
	m_bAccepted = true;
	
	const QString fileText = m_pNewFileWidget->typeIconView->currentItem()->text();
	
	if		( fileText.contains(".flowcode") )
		m_fileType = Document::dt_flowcode;
	
	else if ( fileText.contains(".circuit") )
		m_fileType = Document::dt_circuit;
	
	else if ( fileText.contains(".mechanics") )
		m_fileType = Document::dt_mechanics;
	
	else if ( fileText.contains(".asm") )
	{
		m_fileType = Document::dt_text;
		m_codeType = TextDocument::ct_asm;
	}
	
	else if ( fileText.contains(".basic") || fileText.contains(".microbe") )
	{
		m_fileType = Document::dt_text;
		m_codeType = TextDocument::ct_microbe;
	}
	
	else if (fileText.contains(".c") )
	{
		m_fileType = Document::dt_text;
		m_codeType = TextDocument::ct_c;
	}
	
	else
		m_fileType = Document::dt_text;

	m_bAddToProject = m_pNewFileWidget->addToProjectCheck->isChecked();
	
	m_microID = m_pNewFileWidget->m_pMicroSelect->micro();
}


void NewFileDlg::reject()
{
    hide();
    m_bAccepted = false;
}


void NewFileDlg::fileTypeChanged( Q3IconViewItem *item )
{
	m_pNewFileWidget->m_pMicroSelect->setEnabled(
			item->text().contains(".flowcode") );
}


MicroSelectWidget * NewFileDlg::microSelectWidget() const
{
	return m_pNewFileWidget->m_pMicroSelect;
}


#include "newfiledlg.moc"
