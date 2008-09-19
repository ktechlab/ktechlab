/***************************************************************************
 *   Copyright (C) 2005 by David Saxton <david@bluehaze.org>               *
 *   Copyright (C) 2008 Julian Bäume <julian@svg4all.de>                   *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#include "docmanager.h"
//#include "docmanageriface.h"
//#include "iteminterface.h"
//#include "itemselector.h"
#include "ktechlab.h"
//#include "core/ktlconfig.h"
#include "document.h"
//#include "circuitdocument.h"
//#include "flowcodedocument.h"
//#include "mechanicsdocument.h"
//#include "textdocument.h"
//#include "textview.h"
#include "view.h"
#include "viewcontainer.h"

#include <KApplication>
#include <KLocale>
#include <KMessageBox>
#include <KTabWidget>

#include <QFile>

#include <cassert>

DocManager * DocManager::m_pSelf = 0l;

DocManager * DocManager::self()
{
    if ( !m_pSelf )
        m_pSelf = new DocManager();

    return m_pSelf;
}


DocManager::DocManager()
    : QObject( /*KTechlab::self()*/ )
{
    p_focusedView = 0l;
    m_countCircuit = 0;
    m_countFlowCode = 0;
    m_countMechanics = 0;
    m_countOther = 0;
    p_connectedDocument = 0l;
    m_nextDocumentID = 1;
//	m_pIface = new DocManagerIface(this);
}


DocManager::~DocManager()
{
}


bool DocManager::closeAll()
{
    foreach( Document *document, m_documentList )
    {
        if ( !document->fileClose() )
            return false;

        removeDocumentAssociations(document);
    }
    return true;
}


void DocManager::gotoTextLine( const KUrl &url, int line )
{
/*    TextDocument * doc = dynamic_cast<TextDocument*>( openURL(url) );
    TextView * tv = doc ? doc->textView() : 0l;

    if ( !tv )
        return;

    tv->gotoLine(line);
    tv->setFocus();*/
}


Document* DocManager::openURL( const KUrl &url, ViewArea *viewArea )
{
    if ( url.isEmpty() )
        return 0l;

    if ( url.isLocalFile() )
    {
        QFile file(url.path());
        if ( !file.open(QIODevice::ReadOnly) )
        {
            KMessageBox::sorry( 0l, i18n("Could not open '%1'").arg( url.prettyUrl() ) );
            return 0l;
        }
        file.close();
    }

    // If the currently active view area is empty, and we were not given a view area
    // to open into, then use the empty view area
    if ( !viewArea )
    {
        KTechlab *w = dynamic_cast<KTechlab *>( KApplication::activeWindow() );
        ViewContainer * currentVC = w ? static_cast<ViewContainer*>( w->tabWidget()->currentWidget() ) : 0;
        if ( currentVC )
        {
            ViewArea * va = currentVC->viewArea( currentVC->activeViewArea() );
            if ( !va->view() )
                viewArea = va;
        }
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
    QString extension = fileName.right( fileName.length() - fileName.lastIndexOf('.') );
/*
    if ( extension == ".circuit" )
        return openCircuitFile( url, viewArea );

    else if ( extension == ".flowcode" )
        return openFlowCodeFile( url, viewArea );

    else if ( extension == ".mechanics" )
        return openMechanicsFile( url, viewArea );

    else
        return openTextFile( url, viewArea );
*/
    return document;
}


Document *DocManager::getFocusedDocument() const
{
    Document * doc = p_focusedView ? p_focusedView->document() : 0l;
    return (doc && !doc->isDeleted()) ? doc : 0l;
}


void DocManager::giveDocumentFocus( Document * toFocus, ViewArea * viewAreaForNew )
{
    if ( !toFocus )
        return;

    if ( View * activeView = toFocus->activeView() )
        KTechlab::self()->tabWidget()->showPage( activeView->viewContainer() );

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


Document *DocManager::findDocument( const KUrl &url ) const
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

    return 0l;
}


void DocManager::associateDocument( const KUrl &url, Document *document )
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

    connect( document, SIGNAL(modifiedStateChanged()), KTechlab::self(), SLOT(slotDocModifiedChanged()) );
    connect( document, SIGNAL(fileNameChanged(const KUrl&)), KTechlab::self(), SLOT(slotDocModifiedChanged()) );
    connect( document, SIGNAL(fileNameChanged(const KUrl&)), KTechlab::self(), SLOT(addRecentFile(const KUrl&)) );
    connect( document, SIGNAL(destroyed(QObject* )), this, SLOT(documentDestroyed(QObject* )) );
    connect( document, SIGNAL(viewFocused(View* )), this, SLOT(slotViewFocused(View* )) );
    connect( document, SIGNAL(viewUnfocused()), this, SLOT(slotViewUnfocused()) );

    createNewView( document, viewArea );
}


View *DocManager::createNewView( Document *document, ViewArea *viewArea )
{
    if (!document)
        return 0l;

    View *view = 0l;

    if (viewArea)
        view = document->createView( viewArea->viewContainer(), viewArea->id() );

    else
    {
        ViewContainer *viewContainer = new ViewContainer( document->caption() );
        view = document->createView( viewContainer, 0 );
        KTechlab::self()->addWindow(viewContainer);
    }

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
    ViewContainer * vc = static_cast<ViewContainer*>(KTechlab::self()->tabWidget()->currentPage());
    if (!vc)
        view = 0l;

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
        KTechlab::self()->factory()->addClient( textView->kateView() );
    else
        KTechlab::self()->factory()->addClient( p_focusedView );

    Document *document = view->document();

    connect( document, SIGNAL(undoRedoStateChanged()), KTechlab::self(), SLOT(slotDocUndoRedoChanged()) );
    p_connectedDocument = document;

    if ( document->type() == Document::dt_circuit ||
            document->type() == Document::dt_flowcode ||
            document->type() == Document::dt_mechanics )
    {
        ItemDocument *cvb = static_cast<ItemDocument*>(view->document());
        ItemInterface::self()->slotItemDocumentChanged(cvb);
    }

    KTechlab::self()->slotDocUndoRedoChanged();
    KTechlab::self()->slotDocModifiedChanged();
    KTechlab::self()->requestUpdateCaptions();
}


void DocManager::slotViewUnfocused()
{
    if ( !KTechlab::self() )
        return;

    KTechlab::self()->removeGUIClients();
    disableContextActions();

    if (!p_focusedView)
        return;

    if (p_connectedDocument)
    {
        disconnect( p_connectedDocument, SIGNAL(undoRedoStateChanged()), KTechlab::self(), SLOT(slotDocUndoRedoChanged()) );
        p_connectedDocument = 0l;
    }

    ItemInterface::self()->slotItemDocumentChanged(0l);
    p_focusedView = 0l;

// 	KTechlab::self()->setCaption( 0 );
    KTechlab::self()->requestUpdateCaptions();
}


void DocManager::disableContextActions()
{
    KTechlab * ktl = KTechlab::self();
    if ( !ktl )
        return;

    ktl->action("file_save")->setEnabled(false);
    ktl->action("file_save_as")->setEnabled(false);
    ktl->action("file_close")->setEnabled(false);
    ktl->action("file_print")->setEnabled(false);
    ktl->action("edit_undo")->setEnabled(false);
    ktl->action("edit_redo")->setEnabled(false);
    ktl->action("edit_cut")->setEnabled(false);
    ktl->action("edit_copy")->setEnabled(false);
    ktl->action("edit_paste")->setEnabled(false);
    ktl->action("view_split_leftright")->setEnabled(false);
    ktl->action("view_split_topbottom")->setEnabled(false);
}


TextDocument *DocManager::createTextDocument()
{
    TextDocument *document = TextDocument::constructTextDocument( untitledName(Document::dt_text) );
    handleNewDocument(document);
    return document;
}


CircuitDocument *DocManager::createCircuitDocument()
{
    CircuitDocument *document = new CircuitDocument( untitledName(Document::dt_circuit) );
    handleNewDocument(document);
    if ( KTLConfig::raiseItemSelectors() )
        KTechlab::self()->showToolView( KTechlab::self()->toolView( ComponentSelector::toolViewIdentifier() ) );
    return document;
}


FlowCodeDocument *DocManager::createFlowCodeDocument()
{
    FlowCodeDocument *document = new FlowCodeDocument( untitledName(Document::dt_flowcode) );
    handleNewDocument(document);
    if ( KTLConfig::raiseItemSelectors() )
        KTechlab::self()->showToolView( KTechlab::self()->toolView( FlowPartSelector::toolViewIdentifier() ) );
    return document;
}


MechanicsDocument *DocManager::createMechanicsDocument()
{
    MechanicsDocument *document = new MechanicsDocument( untitledName(Document::dt_mechanics) );
    handleNewDocument(document);
    if ( KTLConfig::raiseItemSelectors() )
        KTechlab::self()->showToolView( KTechlab::self()->toolView( MechanicsSelector::toolViewIdentifier() ) );
    return document;
}


CircuitDocument *DocManager::openCircuitFile( const KUrl &url, ViewArea *viewArea )
{
    CircuitDocument *document = new CircuitDocument( url.fileName().remove(url.directory()) );

    if ( !document->openURL(url) )
    {
        KMessageBox::sorry( 0l, i18n("Could not open Circuit file \"%1\"").arg(url.prettyURL()) );
        document->deleteLater();
        return 0l;
    }

    handleNewDocument( document, viewArea );
    emit fileOpened(url);
    return document;
}


FlowCodeDocument *DocManager::openFlowCodeFile( const KUrl &url, ViewArea *viewArea )
{
    FlowCodeDocument *document = new FlowCodeDocument( url.fileName().remove(url.directory()) );

    if ( !document->openURL(url) )
    {
        KMessageBox::sorry( 0l, i18n("Could not open FlowCode file \"%1\"").arg(url.prettyURL()) );
        document->deleteLater();
        return 0l;
    }

    handleNewDocument( document, viewArea );
    emit fileOpened(url);
    return document;
}


MechanicsDocument *DocManager::openMechanicsFile( const KUrl &url, ViewArea *viewArea )
{
    MechanicsDocument *document = new MechanicsDocument( url.fileName().remove(url.directory()) );

    if ( !document->openURL(url) )
    {
        KMessageBox::sorry( 0l, i18n("Could not open Mechanics file \"%1\"").arg(url.prettyURL()) );
        document->deleteLater();
        return 0l;
    }

    handleNewDocument( document, viewArea );
    emit fileOpened(url);
    return document;

}


TextDocument *DocManager::openTextFile( const KUrl &url, ViewArea *viewArea )
{
    TextDocument *document = TextDocument::constructTextDocument( url.fileName().remove(url.directory()) );

    if (!document)
        return 0l;

    if ( !document->openURL(url) )
    {
        KMessageBox::sorry( 0l, i18n("Could not open text file \"%1\"").arg(url.prettyURL()) );
        document->deleteLater();
        return 0l;
    }

    handleNewDocument( document, viewArea );
    emit fileOpened(url);
    return document;
}


#include "docmanager.moc"
