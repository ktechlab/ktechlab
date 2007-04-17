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
#include "newfilewidget.h"
#include "microlibrary.h"
#include "microselectwidget.h"
#include "projectmanager.h"
#include "textdocument.h"

#include <kcombobox.h>
#include <kdebug.h>
#include <klineedit.h>
#include <kiconview.h>
#include <klocale.h>
#include <kiconloader.h>

#include <canvas.h>
#include <qcheckbox.h>
#include <qdir.h>
#include <qfile.h>
#include <qlabel.h>
#include <qpaintdevicemetrics.h>


NewFileDlg::NewFileDlg( QWidget *parent )
    : KDialogBase( parent, "newfiledlg", true, "New File", KDialogBase::Ok|KDialogBase::Cancel, KDialogBase::Ok, true )
{
	m_pMainParent = parent;
    m_bAccepted = false;
    m_pNewFileWidget = new NewFileWidget(this);

	 m_pNewFileWidget->typeIconView->setSelectionMode(QIconView::Single);
	 m_pNewFileWidget->typeIconView->setMode(KIconView::Select);
    
	KIconLoader *loader = KGlobal::iconLoader();
	
	QValueList<QIconViewItem*> items;
	
	//BEGIN insert icons
	QString text = QString("%1 (.asm)").arg(i18n("Assembly Code"));
	items << new QIconViewItem(m_pNewFileWidget->typeIconView, text, loader->loadIcon( "source", KIcon::NoGroup, KIcon::SizeHuge ) );
	
	text = "C (.c)";
	items << new QIconViewItem(m_pNewFileWidget->typeIconView, text, loader->loadIcon( "source_c", KIcon::NoGroup, KIcon::SizeHuge ) );
	
	text = QString("%1 (.circuit)").arg(i18n("Circuit"));
	items << new QIconViewItem(m_pNewFileWidget->typeIconView,text, loader->loadIcon( "ktechlab_circuit", KIcon::NoGroup, KIcon::SizeHuge ) );
	
	items << new QIconViewItem(m_pNewFileWidget->typeIconView,"FlowCode (.flowcode)", loader->loadIcon( "ktechlab_flowcode", KIcon::NoGroup, KIcon::SizeHuge ) );
	
#ifdef MECHANICS
	items << new QIconViewItem(m_pNewFileWidget->typeIconView,"Mechanics (.mechanics)", loader->loadIcon( "exec", KIcon::NoGroup, KIcon::SizeHuge ) );
#endif
	
	items << new QIconViewItem(m_pNewFileWidget->typeIconView,"Microbe (.microbe)", loader->loadIcon( "ktechlab_microbe", KIcon::NoGroup, KIcon::SizeHuge ) );
	//END insert icons
	
	unsigned minWidth = 20 + m_pNewFileWidget->typeIconView->spacing() * items.size();
	int minHeight = 0;
	
	const QValueList<QIconViewItem*>::iterator end = items.end();
	for ( QValueList<QIconViewItem*>::iterator it = items.begin(); it != end; ++it )
	{
		(*it)->setDragEnabled(false);
		minWidth += (*it)->width();
		minHeight = QMAX( minHeight, (*it)->height()+20 );
	}
	
	m_pNewFileWidget->typeIconView->setMinimumSize( minWidth, minHeight );
	m_pNewFileWidget->typeIconView->setCurrentItem(items[3]);
	m_pNewFileWidget->addToProjectCheck->setChecked( ProjectManager::self()->currentProject() );
	m_pNewFileWidget->addToProjectCheck->setEnabled( ProjectManager::self()->currentProject() );
	microSelectWidget()->setAllowedFlowCodeSupport( MicroInfo::FullSupport | MicroInfo::PartialSupport );
    
	setMainWidget(m_pNewFileWidget);
    
	// Our behaviour is to have single click selects and double click accepts the dialog
	connect( m_pNewFileWidget->typeIconView, SIGNAL(selectionChanged(QIconViewItem*)), this, SLOT(fileTypeChanged(QIconViewItem*)) );
	connect( m_pNewFileWidget->typeIconView, SIGNAL(doubleClicked(QIconViewItem*)), this, SLOT(accept()));

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
    m_bAccepted = false;
}


void NewFileDlg::fileTypeChanged( QIconViewItem *item )
{
	m_pNewFileWidget->m_pMicroSelect->setEnabled(
			item->text().contains(".flowcode") );
}


MicroSelectWidget * NewFileDlg::microSelectWidget() const
{
	return m_pNewFileWidget->m_pMicroSelect;
}


#include "newfiledlg.moc"
