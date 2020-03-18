/***************************************************************************
 *   Copyright (C) 2005 by David Saxton                                    *
 *   david@bluehaze.org                                                    *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#include "docmanager.h"
#include "document.h"
#include "documentiface.h"
#include "ktechlab.h"
#include "projectmanager.h"
#include "view.h"
#include "viewcontainer.h"

#include <kfiledialog.h>
#include <klocalizedstring.h>
#include <kmessagebox.h>

#include <QTabWidget>

Document::Document( const QString &caption, const char *name )
	: QObject( KTechlab::self() /* , name */ ),
	b_modified(false),
	m_pDocumentIface(nullptr),
	m_bDeleted(false),
	m_pActiveView(nullptr),
	m_caption(caption),
	m_bAddToProjectOnSave(false),
	m_dcopID(0),
	m_nextViewID(0)
{
    setObjectName(name);
	connect( KTechlab::self(), SIGNAL(configurationChanged()), this, SLOT(slotUpdateConfiguration()) );
}


Document::~Document()
{
	m_bDeleted = true;
	
	ViewList viewsToDelete = m_viewList;
	const ViewList::iterator end = viewsToDelete.end();
	for ( ViewList::iterator it = viewsToDelete.begin(); it != end; ++it )
		(*it)->deleteLater();
}


void Document::handleNewView( View *view )
{
	if ( !view || m_viewList.contains(view) )
		return;
	
	m_viewList.append(view);
	view->setDCOPID(m_nextViewID++);
	view->setWindowTitle(m_caption);
	connect( view, SIGNAL(destroyed(QObject* )), this, SLOT(slotViewDestroyed(QObject* )) );
	connect( view, SIGNAL(focused(View* )), this, SLOT(slotViewFocused(View* )) );
	connect( view, SIGNAL(unfocused()), this, SIGNAL(viewUnfocused()) );
	
	view->show();
	
	if ( !DocManager::self()->getFocusedView() )
		view->setFocus();
}


void Document::slotViewDestroyed( QObject *obj )
{
	View *view = static_cast<View*>(obj);
	
	m_viewList.removeAll(view);
	
	if ( m_pActiveView == (QPointer<View>)view )
	{
		m_pActiveView = nullptr;
		emit viewUnfocused();
	}
	
	if ( m_viewList.isEmpty() )
		deleteLater();
}


void Document::slotViewFocused(View *view)
{
	if (!view) return;
	
	m_pActiveView = view;
	emit viewFocused(view);
}


void Document::setCaption( const QString &caption )
{
	m_caption = caption;
	const ViewList::iterator end = m_viewList.end();
	for ( ViewList::iterator it = m_viewList.begin(); it != end; ++it )
		(*it)->setWindowTitle(caption);
}


bool Document::getURL( const QString &types )
{
	KUrl url = KFileDialog::getSaveUrl( KUrl(), types, KTechlab::self(), i18n("Save Location"));
	
	if ( url.isEmpty() ) return false;
	
	if ( QFile::exists( url.path() ) )
	{
		int query = KMessageBox::warningYesNo( KTechlab::self(),
			   i18n( "A file named \"%1\" already exists. Are you sure you want to overwrite it?", url.fileName() ),
			   i18n( "Overwrite File?" ));
		if ( query == KMessageBox::No )
			return false;
	}
	
	setURL(url);
	
	return true;
}


bool Document::fileClose()
{
	if ( isModified() )
	{
		// If the filename is empty then it must  be an untitled file.
		QString name = m_url.fileName().isEmpty() ? caption() : m_url.fileName();
		
		if ( ViewContainer * viewContainer = (activeView() ? activeView()->viewContainer() : nullptr) )
			KTechlab::self()->tabWidget()->setCurrentIndex( KTechlab::self()->tabWidget()->indexOf(viewContainer) );
		
		int choice = KMessageBox::warningYesNoCancel( KTechlab::self(),
				i18n("The document \'%1\' has been modified.\nDo you want to save it?", name),
				i18n("Save Document?"),
				KStandardGuiItem::save(),
				KStandardGuiItem::discard() );
		
		if ( choice == KMessageBox::Cancel )
			return false;
		if ( choice == KMessageBox::Yes )
			fileSave();
	}
	
	deleteLater();
	return true;
}


void Document::setModified( bool modified )
{
	if ( b_modified == modified ) return;
	
	b_modified = modified;
	
	if (!m_bDeleted) emit modifiedStateChanged();
}


void Document::setURL( const KUrl &url )
{
	if ( m_url == url ) return;
	
	bool wasEmpty = m_url.isEmpty();
	m_url = url;
	
	if ( wasEmpty && m_bAddToProjectOnSave && ProjectManager::self()->currentProject() )
		ProjectManager::self()->currentProject()->addFile(m_url);
	
	emit fileNameChanged(url);
	
	if (KTechlab::self()) {
		KTechlab::self()->addRecentFile(url);
		KTechlab::self()->requestUpdateCaptions();
	}
}

DCOPObject * Document::dcopObject( ) const
{
	return m_pDocumentIface;
}

void Document::setDCOPID( unsigned id )
{
	if ( m_dcopID == id ) return;
	
	m_dcopID = id;
	if ( m_pDocumentIface ) {
		QString docID;
		docID.setNum( dcopID() );
		m_pDocumentIface->setObjId( "Document#" + docID );
	}
}
