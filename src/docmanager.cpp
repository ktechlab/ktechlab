/***************************************************************************
 *   Copyright (C) 2005 by David Saxton                                    *
 *   david@bluehaze.org                                                    *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#include "circuitdocument.h"
#include "docmanager.h"
#include "docmanageriface.h"
#include "flowcodedocument.h"
#include "iteminterface.h"
#include "itemselector.h"
#include "ktechlab.h"
#include "core/ktlconfig.h"
#include "mechanicsdocument.h"
#include "textdocument.h"
#include "textview.h"
#include "viewcontainer.h"

#include <klocale.h>
#include <kmessagebox.h>
#include <ktabwidget.h>
#include <qfile.h>

#include <cassert>

DocManager * DocManager::m_pSelf = 0;

DocManager * DocManager::self( KTechlab * ktechlab )
{
	if ( !m_pSelf )
	{
		assert(ktechlab);
		m_pSelf = new DocManager(ktechlab);
	}
	return m_pSelf;
}


DocManager::DocManager( KTechlab * ktechlab )
	: QObject( ktechlab ),
	p_ktechlab(ktechlab)
{
	p_focusedView = 0;
	m_countCircuit = 0;
	m_countFlowCode = 0;
	m_countMechanics = 0;
	m_countOther = 0;
	p_connectedDocument = 0;
	m_nextDocumentID = 1;
	m_pIface = new DocManagerIface(this);
}


DocManager::~DocManager()
{
}


bool DocManager::closeAll()
{
	const DocumentList::iterator end = m_documentList.end();
	while ( !m_documentList.isEmpty() )
	{
		Document *document = m_documentList.first();
		if ( document->fileClose() )
		{
			m_documentList.remove(document);
			removeDocumentAssociations(document);
		}
		else
			return false;
	}
	return true;
}


void DocManager::gotoTextLine( const KURL &url, int line )
{
	TextDocument * doc = dynamic_cast<TextDocument*>( openURL(url) );
	if (!doc)
		return;
	
	doc->textView()->gotoLine(line);
}


Document* DocManager::openURL( const KURL &url, ViewArea *viewArea )
{
	if ( url.isEmpty() )
		return 0;
	
	if ( url.isLocalFile() )
	{
		QFile file(url.path());
		if ( file.open(IO_ReadOnly) == false )
		{
			KMessageBox::sorry( 0, i18n("Could not open '%1'").arg( url.prettyURL() ) );
			return 0;
		}
		file.close();
	}
	
	// If the document is already open, and a specific view area hasn't been
	// specified, then just return that document - otherwise, create a new
	// view in the viewarea
	Document *document = findDocument(url);
	if ( document )
	{
		if ( viewArea )
			createNewView( document, viewArea );
		else
			giveDocumentFocus( document, viewArea );
		return document;
	}
	
	QString fileName = url.fileName();
	QString extension = fileName.right( fileName.length() - fileName.findRev('.') );
	
	if ( extension == ".circuit" )
		return openCircuitFile( url, viewArea );
	
	else if ( extension == ".flowcode" )
		return openFlowCodeFile( url, viewArea );
	
	else if ( extension == ".mechanics" )
		return openMechanicsFile( url, viewArea );
	
	else
		return openTextFile( url, viewArea );
}


Document *DocManager::getFocusedDocument() const
{
	Document * doc = p_focusedView ? p_focusedView->document() : 0;
	return (doc && !doc->isDeleted()) ? doc : 0;
}


void DocManager::giveDocumentFocus( Document * toFocus, ViewArea * viewAreaForNew )
{
	if ( !toFocus )
		return;
	
	if ( View * activeView = toFocus->activeView() )
	{
		p_ktechlab->tabWidget()->showPage( activeView->viewContainer() );
		activeView->setFocused();
		activeView->viewContainer()->setFocused();
	}
	else if ( viewAreaForNew )
		createNewView( toFocus, viewAreaForNew );
}


QString DocManager::untitledName( int type )
{
	QString name;
	switch(type)
	{
		case Document::dt_circuit:
		{
			if ( m_countCircuit>1 )
				name = i18n("Untitled (Circuit %1)").arg(QString::number(m_countCircuit));
			else
				name = i18n("Untitled (Circuit)");
			m_countCircuit++;
			break;
		}
		case Document::dt_flowcode:
		{
			if ( m_countFlowCode>1 )
				name = i18n("Untitled (FlowCode %1)").arg(QString::number(m_countFlowCode));
			else
				name = i18n("Untitled (FlowCode)");
			m_countFlowCode++;
			break;
		}
		case Document::dt_mechanics:
		{
			if ( m_countMechanics>1 )
				name = i18n("Untitled (Mechanics %1)").arg(QString::number(m_countMechanics));
			else
				name = i18n("Untitled (Mechanics)");
			m_countMechanics++;
			break;
		}
		default:
		{
			if ( m_countOther>1 )
				name = i18n("Untitled (%1)").arg(QString::number(m_countOther));
			else
				name = i18n("Untitled");
			m_countOther++;
			break;
		}
	}
	return name;
}


Document *DocManager::findDocument( const KURL &url ) const
{
	// First, look in the associated documents
	if ( m_associatedDocuments.contains(url) )
		return m_associatedDocuments[url];
	
	// Not found, so look in the known documents
	const DocumentList::const_iterator end = m_documentList.end();
	for ( DocumentList::const_iterator it = m_documentList.begin(); it != end; ++it )
	{
		if ( (*it)->url() == url )
			return *it;
	}
	
	return 0;
}


void DocManager::associateDocument( const KURL &url, Document *document )
{
	if (!document)
		return;
	
	m_associatedDocuments[url] = document;
}


void DocManager::removeDocumentAssociations( Document *document )
{
	bool doneErase;
	do
	{
		doneErase = false;
		const URLDocumentMap::iterator end = m_associatedDocuments.end();
		for ( URLDocumentMap::iterator it = m_associatedDocuments.begin(); it != end; ++it )
		{
			if ( it.data() == document )
			{
				doneErase = true;
				m_associatedDocuments.erase(it);
				break;
			}
		}
	}
	while (doneErase);
}


void DocManager::handleNewDocument( Document *document, ViewArea *viewArea )
{
	if ( !document || m_documentList.contains(document) )
		return;
	
	m_documentList.append(document);
	document->setDCOPID(m_nextDocumentID++);
	
	connect( document, SIGNAL(modifiedStateChanged()), p_ktechlab, SLOT(slotDocModifiedChanged()) );
	connect( document, SIGNAL(fileNameChanged(const KURL&)), p_ktechlab, SLOT(slotDocModifiedChanged()) );
	connect( document, SIGNAL(fileNameChanged(const KURL&)), p_ktechlab, SLOT(addRecentFile(const KURL&)) );
	connect( document, SIGNAL(destroyed(QObject* )), this, SLOT(documentDestroyed(QObject* )) );
	connect( document, SIGNAL(viewFocused(View* )), this, SLOT(slotViewFocused(View* )) );
	connect( document, SIGNAL(viewUnfocused()), this, SLOT(slotViewUnfocused()) );
	
	createNewView( document, viewArea );
}


View *DocManager::createNewView( Document *document, ViewArea *viewArea )
{
	if (!document)
		return 0;
	
	View *view = 0;
	
	if (viewArea)
		view = document->createView( viewArea->viewContainer(), viewArea->id() );
	
	else
	{
		ViewContainer *viewContainer = new ViewContainer( document->caption(), p_ktechlab );
		view = document->createView( viewContainer, 0 );
		p_ktechlab->addWindow(viewContainer);
	}
	
	view->setFocused();
	return view;
}


void DocManager::documentDestroyed( QObject *obj )
{
	Document *doc = static_cast<Document*>(obj);
	m_documentList.remove(doc);
	removeDocumentAssociations(doc);
	disableContextActions();
}


void DocManager::slotViewFocused( View *view )
{
	ViewContainer * vc = static_cast<ViewContainer*>(p_ktechlab->tabWidget()->currentPage());
	if (!vc)
		view = 0;
	
	if (!view)
		return;
	
	// This function can get called with a view that is not in the current view
	// container (such as when the user right clicks and then the popup is
	// destroyed - not too sure why, but this is the easiest way to fix it).
	if ( view->viewContainer() != vc )
		view = vc->activeView();
	
	if ( !view || (View*)p_focusedView == view )
		return;
	
	if (p_focusedView)
		slotViewUnfocused();
	
	p_focusedView = view;
	
	if ( TextView * textView = dynamic_cast<TextView*>((View*)p_focusedView) )
		p_ktechlab->factory()->addClient( textView->kateView() );
	else
		p_ktechlab->factory()->addClient( p_focusedView );
	
	Document *document = view->document();
	
	connect( document, SIGNAL(undoRedoStateChanged()), p_ktechlab, SLOT(slotDocUndoRedoChanged()) );
	p_connectedDocument = document;
		
	if ( document->type() == Document::dt_circuit ||
			   document->type() == Document::dt_flowcode ||
			   document->type() == Document::dt_mechanics )
	{
		ItemDocument *cvb = static_cast<ItemDocument*>(view->document());
		ItemInterface::self()->slotItemDocumentChanged(cvb);
	}
	
	p_ktechlab->slotDocUndoRedoChanged();
	p_ktechlab->slotDocModifiedChanged();
	p_ktechlab->requestUpdateCaptions();
}


void DocManager::slotViewUnfocused()
{
	p_ktechlab->removeGUIClients();
	disableContextActions();
	
	if (!p_focusedView)
		return;
	
	if (p_connectedDocument)
	{
		disconnect( p_connectedDocument, SIGNAL(undoRedoStateChanged()), p_ktechlab, SLOT(slotDocUndoRedoChanged()) );
		p_connectedDocument = 0;
	}
	
	ItemInterface::self()->slotItemDocumentChanged(0);
	p_focusedView = 0;
	
// 	p_ktechlab->setCaption( 0 );
	p_ktechlab->requestUpdateCaptions();
}


void DocManager::disableContextActions()
{
	p_ktechlab->action("file_save")->setEnabled(false);
	p_ktechlab->action("file_save_as")->setEnabled(false);
	p_ktechlab->action("file_close")->setEnabled(false);
	p_ktechlab->action("file_print")->setEnabled(false);
	p_ktechlab->action("edit_undo")->setEnabled(false);
	p_ktechlab->action("edit_redo")->setEnabled(false);
	p_ktechlab->action("edit_cut")->setEnabled(false);
	p_ktechlab->action("edit_copy")->setEnabled(false);
	p_ktechlab->action("edit_paste")->setEnabled(false);
	p_ktechlab->action("view_split_leftright")->setEnabled(false);
	p_ktechlab->action("view_split_topbottom")->setEnabled(false);
}


TextDocument *DocManager::createTextDocument()
{
	TextDocument *document = TextDocument::constructTextDocument( untitledName(Document::dt_text), p_ktechlab );
	handleNewDocument(document);
	return document;
}


CircuitDocument *DocManager::createCircuitDocument()
{
	CircuitDocument *document = new CircuitDocument( untitledName(Document::dt_circuit), p_ktechlab );
	handleNewDocument(document);
	if ( KTLConfig::raiseItemSelectors() )
		p_ktechlab->showToolView( p_ktechlab->toolView( ComponentSelector::toolViewIdentifier() ) );
	return document;
}


FlowCodeDocument *DocManager::createFlowCodeDocument()
{
	FlowCodeDocument *document = new FlowCodeDocument( untitledName(Document::dt_flowcode), p_ktechlab );
	handleNewDocument(document);
	if ( KTLConfig::raiseItemSelectors() )
		p_ktechlab->showToolView( p_ktechlab->toolView( FlowPartSelector::toolViewIdentifier() ) );
	return document;
}


MechanicsDocument *DocManager::createMechanicsDocument()
{
	MechanicsDocument *document = new MechanicsDocument( untitledName(Document::dt_mechanics), p_ktechlab );
	handleNewDocument(document);
	if ( KTLConfig::raiseItemSelectors() )
		p_ktechlab->showToolView( p_ktechlab->toolView( MechanicsSelector::toolViewIdentifier() ) );
	return document;
}


CircuitDocument *DocManager::openCircuitFile( const KURL &url, ViewArea *viewArea )
{
	CircuitDocument *document = new CircuitDocument( url.fileName().remove(url.directory()), p_ktechlab );
	
	if ( !document->openURL(url) )
	{
		KMessageBox::sorry( 0, i18n("Could not open Circuit file \"%1\"").arg(url.prettyURL()) );
		document->deleteLater();
		return 0;
	}
	
	handleNewDocument( document, viewArea );
	emit fileOpened(url);
	return document;
}


FlowCodeDocument *DocManager::openFlowCodeFile( const KURL &url, ViewArea *viewArea )
{
	FlowCodeDocument *document = new FlowCodeDocument( url.fileName().remove(url.directory()), p_ktechlab );
	
	if ( !document->openURL(url) )
	{
		KMessageBox::sorry( 0, i18n("Could not open FlowCode file \"%1\"").arg(url.prettyURL()) );
		document->deleteLater();
		return 0;
	}
	
	handleNewDocument( document, viewArea );
	emit fileOpened(url);
	return document;
}


MechanicsDocument *DocManager::openMechanicsFile( const KURL &url, ViewArea *viewArea )
{
	MechanicsDocument *document = new MechanicsDocument( url.fileName().remove(url.directory()), p_ktechlab );
	
	if ( !document->openURL(url) )
	{
		KMessageBox::sorry( 0, i18n("Could not open Mechanics file \"%1\"").arg(url.prettyURL()) );
		document->deleteLater();
		return 0;
	}
	
	handleNewDocument( document, viewArea );
	emit fileOpened(url);
	return document;
	
}


TextDocument *DocManager::openTextFile( const KURL &url, ViewArea *viewArea )
{
	TextDocument *document = TextDocument::constructTextDocument( url.fileName().remove(url.directory()), p_ktechlab );
	
	if (!document)
		return 0;
	
	if ( !document->openURL(url) )
	{
		KMessageBox::sorry( 0, i18n("Could not open text file \"%1\"").arg(url.prettyURL()) );
		document->deleteLater();
		return 0;
	}
	
	handleNewDocument( document, viewArea );
	emit fileOpened(url);
	return document;
}


#include "docmanager.moc"
