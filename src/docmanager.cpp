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
#include "mainwindow.h"
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
    p_connectedDocument = 0l;
    m_nextDocumentID = 1;
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

    // If the document is already open then just return that document
    // otherwise, create a new view in the viewarea
    Document *document = findDocument(url);
    if ( document )
    {
       return true;
    }

    QString fileName = url.fileName();
    QString extension = fileName.right( fileName.length() - fileName.lastIndexOf('.') );
    kDebug() << "open: " << url << endl;

    //FIXME: register document within DocumentEngine

    return true;
}

QString DocManager::untitledName( const QString &type )
{
    QString name = i18n("Untitled");

    int count;
    //handle empty types..
    if ( type.isEmpty() ) {
        count = m_count["Other"];
        m_count["Other"]++;
    } else {
        count = m_count[type];
        m_count[type]++;
    }

    if ( count > 1 ) {
        name += QString("(%1 %2)").arg( type )
                                .arg( QString::number( count ) );
    } else {
        name += type.isEmpty() ? QString() : QString("(%1)").arg( type );
    }

    return name;
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

    KTechLab::MainWindow *w = dynamic_cast<KTechLab::MainWindow *>( KApplication::activeWindow() );
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
    KTechLab::MainWindow* ktl = dynamic_cast<KTechLab::MainWindow *>( KApplication::activeWindow() );
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
