/***************************************************************************
 *   Copyright (C) 2005 by David Saxton                                    *
 *   david@bluehaze.org                                                    *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#ifndef DOCMANAGER_H
#define DOCMANAGER_H

#include "view.h"

#include <QPointer>
#include <QUrl>

class CircuitDocument;
class DocManager;
class DocManagerIface;
class Document;
class FlowCodeDocument;
class KTechlab;
class MechanicsDocument;
class TextDocument;
class View;
class ViewArea;

typedef QList<Document *> DocumentList;
typedef QMap<QUrl, Document *> URLDocumentMap;

/**
@author David Saxton
*/
class DocManager : public QObject
{
    Q_OBJECT

    friend class KtlTestsAppFixture;

public:
    static DocManager *self();
    ~DocManager() override;

    /**
     * Attempts to close all open documents, returning true if successful
     */
    bool closeAll();
    /**
     * Goes to the given line in the given text file (if the file exists)
     */
    void gotoTextLine(const QUrl &url, int line);
    /**
     * Attempts to open the document at the given url.
     * @param viewArea if non-null, will open the new view into the ViewArea
     */
    Document *openURL(const QUrl &url, ViewArea *viewArea = nullptr);
    /**
     * Returns the focused View
     */
    View *getFocusedView() const
    {
        return p_focusedView;
    }
    /**
     * Returns the focused Document (the document of the focused view)
     */
    Document *getFocusedDocument() const;
    /**
     * Get a unique name, e.g. Untitled (circuit) - n" depending on the types
     * of Document and whether it is the first one or not
     * @param type Document::DocumentType - type of Document
     */
    QString untitledName(int type);
    /**
     * Checks to see if a document with the given URL is already open, and
     * returns a pointer to that Document if so - otherwises returns null
     * @see associateDocument
     */
    Document *findDocument(const QUrl &url) const;
    /**
     * Associates a url with a pointer to a document. When findFile is called
     * with the given url, it will return a pointer to this document if it still
     * exists.
     * @see findDocument
     */
    void associateDocument(const QUrl &url, Document *document);
    /**
     * Gives the given document focus. If it has no open views, one will be
     * created for it if viewAreaForNew is non-null
     */
    void giveDocumentFocus(Document *toFocus, ViewArea *viewAreaForNew = nullptr);
    void removeDocumentAssociations(Document *document);
    void disableContextActions();

public Q_SLOTS:
    /**
     * Creates an empty text document (with an open view)
     */
    TextDocument *createTextDocument();
    /**
     * Creates an empty circuit document (with an open view), and shows the
     * component selector.
     */
    CircuitDocument *createCircuitDocument();
    /**
     * Creates an empty flowcode document (with an open view), and shows the
     * flowpart selector.
     */
    FlowCodeDocument *createFlowCodeDocument();
    /**
     * Creates an empty mechanics document (with an open view), and shows the
     * mechanics selector.
     */
    MechanicsDocument *createMechanicsDocument();

Q_SIGNALS:
    /**
     * Emitted when a file is successfully opened
     */
    void fileOpened(const QUrl &url);

protected Q_SLOTS:
    /**
     * Does the appropriate enabling / disabling of actions, connections, etc
     */
    void slotViewFocused(View *view);
    /**
     * Does the appropriate enabling / disabling of actions, connections, etc
     */
    void slotViewUnfocused();
    void documentDestroyed(QObject *obj);

protected:
    /**
     * This function should be called after creating a new document to add it
     * to the appropriate lists and connect it up as appropriate
     */
    void handleNewDocument(Document *document, ViewArea *viewArea = nullptr);
    /**
     * Takes the document, creates a new view and shoves it in a new
     * ViewContainer
     */
    View *createNewView(Document *document, ViewArea *viewArea = nullptr);
    CircuitDocument *openCircuitFile(const QUrl &url, ViewArea *viewArea = nullptr);
    FlowCodeDocument *openFlowCodeFile(const QUrl &url, ViewArea *viewArea = nullptr);
    MechanicsDocument *openMechanicsFile(const QUrl &url, ViewArea *viewArea = nullptr);
    TextDocument *openTextFile(const QUrl &url, ViewArea *viewArea = nullptr);

    DocumentList m_documentList;
    URLDocumentMap m_associatedDocuments;

    // Keeps track of how many
    // new files have been made
    // for the purpose of making
    // titles of the form Untitled (n)
    int m_countCircuit;
    int m_countFlowCode;
    int m_countMechanics;
    int m_countOther;

    QPointer<View> p_focusedView;
    QPointer<Document> p_connectedDocument;
    DocManagerIface *m_pIface;
    unsigned m_nextDocumentID;

private:
    DocManager();
    static DocManager *m_pSelf;
};

#endif
