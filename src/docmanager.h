/***************************************************************************
 *   Copyright (C) 2005 by David Saxton <david@bluehaze.org>               *
 *   Copyright (C) 2008 Julian Bäume <julian@svg4all.de>                   *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#ifndef DOCMANAGER_H
#define DOCMANAGER_H

#include <KUrl>

#include <QPointer>
#include <QSet>
#include <QLinkedList>

//class DocManagerIface;
class Document;

/**
@author David Saxton
*/
class DocManager : public QObject
{
Q_OBJECT
public:
    static DocManager * self();
    ~DocManager();

    /**
    * Attempts to close all open documents, returning true if successful
    */
    bool closeAll();
   /**
    * Attempts to open the document at the given url.
    * Registers the document within the DocManager
    */
    bool openUrl( const KUrl &url );
    /**
    * Returns the focused Document (the document of the focused view)
    */
    Document* getFocusedDocument() const;
    /**
    * Get a unique name, e.g. Untitled (circuit) - n" depending on the type
    * of the Document and whether it is the first one or not.
    * The type is used in the name "as is", so it needs to be localized
    * to show a translated name.
    * @param type QString - type of Document (localized!)
    */
    QString untitledName( const QString &type );
    /**
    * Checks to see if a document with the given URL is already open, and
    * returns a pointer to that Document if so - otherwises returns null
    * @see associateDocument
    */
    Document* findDocument( const KUrl &url ) const;
    /**
    * Associates a url with a pointer to a document. When findFile is called
    * with the given url, it will return a pointer to this document if it still
    * exists.
    * @see findDocument
    */
    void associateDocument( const KUrl &url, Document *document );
    /**
    * Remove all of the documents assosiations with anything.
    */
    void removeDocumentAssociations( Document *document );
    void disableContextActions();

public slots:

signals:
    /**
    * Emitted when a file is successfully opened
    */
    void fileOpened( const KUrl &url );

protected slots:
    void documentDestroyed( QObject *obj );

protected:
    /**
    * This function should be called after creating a new document to add it
    * to the appropriate lists and connect it up as appropriate
    */
    void handleNewDocument( Document *document );

    // Keeps track of how many
    // new files have been made
    // for the purpose of making
    // titles of the form Untitled (n)
    QHash<QString, int> m_count;

    QPointer<Document> p_connectedDocument;
//  DocManagerIface *m_pIface;
    unsigned m_nextDocumentID;

private:
    DocManager();
    static DocManager * m_pSelf;
};

#endif
