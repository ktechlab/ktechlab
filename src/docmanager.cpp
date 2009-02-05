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

#include <Plasma/DataEngineManager>
#include <KApplication>
#include <KDebug>
#include <KAction>
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
    m_countCircuit = 0;
    m_countFlowCode = 0;
    m_countMechanics = 0;
    m_countOther = 0;
    p_connectedDocument = 0l;
    m_nextDocumentID = 1;
    m_docEngine = Plasma::DataEngineManager::self()->loadEngine( "DocumentEngine" );
//  m_pIface = new DocManagerIface(this);
}


DocManager::~DocManager()
{
}


bool DocManager::closeAll()
{
    return true;
}

bool DocManager::openUrl( const KUrl &url )
{
    if ( url.isEmpty() )
        return false;

    if ( url.isLocalFile() )
    {
        QFile file(url.path());
        if ( !file.open(QIODevice::ReadOnly) )
        {
            KMessageBox::sorry( 0l, i18n("Could not open '%1'").arg( url.prettyUrl() ) );
            return false;
        }
        file.close();
    }

    KTechlab *w = dynamic_cast<KTechlab *>( KApplication::activeWindow() );
    //ViewContainer * currentVC = w ? static_cast<ViewContainer*>( w->tabWidget()->currentWidget() ) : 0;
    //if ( currentVC )
    {
        //ViewArea * va = currentVC->viewArea( currentVC->activeViewArea() );
        //if ( !va->view() )
        //    viewArea = va;
    }

    // If the document is already open then just return that document
    // otherwise, create a new view in the viewarea
    Document *document = findDocument(url);
    if ( document )
    {
       return true;
    }

    QString fileName = url.fileName();
    QString extension = fileName.right( fileName.length() - fileName.lastIndexOf('.') );
    kDebug() << "open: " << url;

    //FIXME: register document within DocumentEngine

    return true;
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
    Plasma::DataEngine::Data result = m_docEngine->query( url.url() );

    return 0;
}

Document* DocManager::getFocusedDocument() const
{
    return 0;
}


void DocManager::associateDocument( const KUrl &url, Document *document )
{
    if (!document)
        return;

}


void DocManager::removeDocumentAssociations( Document *document )
{
}


void DocManager::handleNewDocument( Document *document )
{
//     document->setDCOPID(m_nextDocumentID++);

    KTechlab *w = dynamic_cast<KTechlab *>( KApplication::activeWindow() );
    if ( w )
    {
        connect( document, SIGNAL(modifiedStateChanged()), w, SLOT(slotDocModifiedChanged()) );
        connect( document, SIGNAL(fileNameChanged(const KUrl&)), w, SLOT(slotDocModifiedChanged()) );
        connect( document, SIGNAL(fileNameChanged(const KUrl&)), w, SLOT(addRecentFile(const KUrl&)) );
    }
    connect( document, SIGNAL(destroyed(QObject* )), this, SLOT(documentDestroyed(QObject* )) );
    connect( document, SIGNAL(viewFocused(View* )), this, SLOT(slotViewFocused(View* )) );
    connect( document, SIGNAL(viewUnfocused()), this, SLOT(slotViewUnfocused()) );

}

void DocManager::documentDestroyed( QObject *obj )
{
    Document *doc = static_cast<Document*>(obj);
    removeDocumentAssociations(doc);
    disableContextActions();
}


void DocManager::disableContextActions()
{
    KTechlab * ktl = dynamic_cast<KTechlab *>( KApplication::activeWindow() );
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

#include "docmanager.moc"
