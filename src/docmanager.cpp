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
#include "docmanageriface.h"
#include "electronics/circuitdocument.h"
#include "flowcodedocument.h"
#include "iteminterface.h"
#include "itemselector.h"
#include "ktechlab.h"
#include "mechanicsdocument.h"
#include "textdocument.h"
#include "textview.h"
#include "viewcontainer.h"

#include <KLocalizedString>
#include <KMessageBox>
#include <KXMLGUIFactory>

#include <QAction>
#include <QFile>
#include <QTabWidget>

#include <cassert>

#include <ktlconfig.h>

DocManager *DocManager::m_pSelf = nullptr;

DocManager *DocManager::self()
{
    if (!m_pSelf)
        m_pSelf = new DocManager();

    return m_pSelf;
}

DocManager::DocManager()
    : QObject(KTechlab::self())
{
    p_focusedView = nullptr;
    m_countCircuit = 0;
    m_countFlowCode = 0;
    m_countMechanics = 0;
    m_countOther = 0;
    p_connectedDocument = nullptr;
    m_nextDocumentID = 1;
    m_pIface = new DocManagerIface(this);
}

DocManager::~DocManager()
{
    delete m_pIface;
}

bool DocManager::closeAll()
{
    while (!m_documentList.isEmpty()) {
        Document *document = m_documentList.first();
        if (document->fileClose()) {
            m_documentList.removeAll(document);
            removeDocumentAssociations(document);
        } else
            return false;
    }
    return true;
}

void DocManager::gotoTextLine(const QUrl &url, int line)
{
    TextDocument *doc = dynamic_cast<TextDocument *>(openURL(url));
    TextView *tv = doc ? doc->textView() : nullptr;

    if (!tv)
        return;

    tv->gotoLine(line);
    tv->setFocus();
}

Document *DocManager::openURL(const QUrl &url, ViewArea *viewArea)
{
    if (url.isEmpty())
        return nullptr;

    if (url.isLocalFile()) {
        QFile file(url.toLocalFile());
        if (file.open(QIODevice::ReadOnly) == false) {
            KMessageBox::sorry(nullptr, i18n("Could not open '%1'", file.fileName()));
            return nullptr;
        }
        file.close();
    }

    // If the currently active view area is empty, and we were not given a view area
    // to open into, then use the empty view area
    if (!viewArea) {
        ViewContainer *currentVC = static_cast<ViewContainer *>(KTechlab::self()->tabWidget()->currentWidget());
        if (currentVC) {
            ViewArea *va = currentVC->viewArea(currentVC->activeViewArea());
            if (!va->view())
                viewArea = va;
        }
    }

    // If the document is already open, and a specific view area hasn't been
    // specified, then just return that document - otherwise, create a new
    // view in the viewarea
    Document *document = findDocument(url);
    if (document) {
        if (viewArea)
            createNewView(document, viewArea);
        else
            giveDocumentFocus(document, viewArea);
        return document;
    }

    QString fileName = url.fileName();
    QString extension = fileName.right(fileName.length() - fileName.lastIndexOf('.'));

    if (extension == ".circuit")
        return openCircuitFile(url, viewArea);
    else if (extension == ".flowcode")
        return openFlowCodeFile(url, viewArea);
    else if (extension == ".mechanics")
        return openMechanicsFile(url, viewArea);
    else
        return openTextFile(url, viewArea);
}

Document *DocManager::getFocusedDocument() const
{
    Document *doc = p_focusedView ? p_focusedView->document() : nullptr;
    return (doc && !doc->isDeleted()) ? doc : nullptr;
}

void DocManager::giveDocumentFocus(Document *toFocus, ViewArea *viewAreaForNew)
{
    if (!toFocus)
        return;

    if (View *activeView = toFocus->activeView()) {
        // KTechlab::self()->tabWidget()->showPage( activeView->viewContainer() ); // 2018.12.01
        KTechlab::self()->tabWidget()->setCurrentIndex(KTechlab::self()->tabWidget()->indexOf(activeView->viewContainer()));
    }

    else if (viewAreaForNew)
        createNewView(toFocus, viewAreaForNew);
}

QString DocManager::untitledName(int type)
{
    QString name;
    switch (type) {
    case Document::dt_circuit: {
        if (m_countCircuit > 1)
            name = i18n("Untitled (Circuit %1)", QString::number(m_countCircuit));
        else
            name = i18n("Untitled (Circuit)");
        m_countCircuit++;
        break;
    }
    case Document::dt_flowcode: {
        if (m_countFlowCode > 1)
            name = i18n("Untitled (FlowCode %1)", QString::number(m_countFlowCode));
        else
            name = i18n("Untitled (FlowCode)");
        m_countFlowCode++;
        break;
    }
    case Document::dt_mechanics: {
        if (m_countMechanics > 1)
            name = i18n("Untitled (Mechanics %1)", QString::number(m_countMechanics));
        else
            name = i18n("Untitled (Mechanics)");
        m_countMechanics++;
        break;
    }
    default: {
        if (m_countOther > 1)
            name = i18n("Untitled (%1)", QString::number(m_countOther));
        else
            name = i18n("Untitled");
        m_countOther++;
        break;
    }
    }
    return name;
}

Document *DocManager::findDocument(const QUrl &url) const
{
    // First, look in the associated documents
    if (m_associatedDocuments.contains(url))
        return m_associatedDocuments[url];

    // Not found, so look in the known documents
    const DocumentList::const_iterator end = m_documentList.end();
    for (DocumentList::const_iterator it = m_documentList.begin(); it != end; ++it) {
        if ((*it)->url() == url)
            return *it;
    }

    return nullptr;
}

void DocManager::associateDocument(const QUrl &url, Document *document)
{
    if (!document)
        return;

    m_associatedDocuments[url] = document;
}

void DocManager::removeDocumentAssociations(Document *document)
{
    bool doneErase;
    do {
        doneErase = false;
        const URLDocumentMap::iterator end = m_associatedDocuments.end();
        for (URLDocumentMap::iterator it = m_associatedDocuments.begin(); it != end; ++it) {
            if (it.value() == document) {
                doneErase = true;
                m_associatedDocuments.erase(it);
                break;
            }
        }
    } while (doneErase);
}

void DocManager::handleNewDocument(Document *document, ViewArea *viewArea)
{
    if (!document || m_documentList.contains(document))
        return;

    m_documentList.append(document);
    document->setDCOPID(m_nextDocumentID++);

    connect(document, &Document::modifiedStateChanged, KTechlab::self(), &KTechlab::slotDocModifiedChanged);
    connect(document, &Document::fileNameChanged, KTechlab::self(), &KTechlab::slotDocModifiedChanged);
    connect(document, &Document::fileNameChanged, KTechlab::self(), &KTechlab::addRecentFile);
    connect(document, &Document::destroyed, this, &DocManager::documentDestroyed);
    connect(document, &Document::viewFocused, this, &DocManager::slotViewFocused);
    connect(document, &Document::viewUnfocused, this, &DocManager::slotViewUnfocused);

    createNewView(document, viewArea);
}

View *DocManager::createNewView(Document *document, ViewArea *viewArea)
{
    if (!document)
        return nullptr;

    View *view = nullptr;

    if (viewArea)
        view = document->createView(viewArea->viewContainer(), viewArea->id());

    else {
        ViewContainer *viewContainer = new ViewContainer(document->caption());
        view = document->createView(viewContainer, 0);
        KTechlab::self()->addWindow(viewContainer);
    }

    return view;
}

void DocManager::documentDestroyed(QObject *obj)
{
    Document *doc = static_cast<Document *>(obj);
    m_documentList.removeAll(doc);
    removeDocumentAssociations(doc);
    disableContextActions();
}

void DocManager::slotViewFocused(View *view)
{
    ViewContainer *vc = static_cast<ViewContainer *>(KTechlab::self()->tabWidget()->currentWidget());
    if (!vc)
        view = nullptr;

    if (!view)
        return;

    // This function can get called with a view that is not in the current view
    // container (such as when the user right clicks and then the popup is
    // destroyed - not too sure why, but this is the easiest way to fix it).
    if (view->viewContainer() != vc)
        view = vc->activeView();

    if (!view || (View *)p_focusedView == view)
        return;

    if (p_focusedView)
        slotViewUnfocused();

    p_focusedView = view;

    if (TextView *textView = dynamic_cast<TextView *>((View *)p_focusedView))
        KTechlab::self()->factory()->addClient(textView->kateView());
    else
        KTechlab::self()->factory()->addClient(p_focusedView);

    Document *document = view->document();

    connect(document, &Document::undoRedoStateChanged, KTechlab::self(), &KTechlab::slotDocUndoRedoChanged);
    p_connectedDocument = document;

    if (document->type() == Document::dt_circuit || document->type() == Document::dt_flowcode || document->type() == Document::dt_mechanics) {
        ItemDocument *cvb = static_cast<ItemDocument *>(view->document());
        ItemInterface::self()->slotItemDocumentChanged(cvb);
    }

    KTechlab::self()->slotDocUndoRedoChanged();
    KTechlab::self()->slotDocModifiedChanged();
    KTechlab::self()->requestUpdateCaptions();
}

void DocManager::slotViewUnfocused()
{
    if (!KTechlab::self())
        return;

    KTechlab::self()->removeGUIClients();
    disableContextActions();

    if (!p_focusedView)
        return;

    if (p_connectedDocument) {
        disconnect(p_connectedDocument, &Document::undoRedoStateChanged, KTechlab::self(), &KTechlab::slotDocUndoRedoChanged);
        p_connectedDocument = nullptr;
    }

    ItemInterface::self()->slotItemDocumentChanged(nullptr);
    p_focusedView = nullptr;

    // 	KTechlab::self()->setCaption( 0 );
    KTechlab::self()->requestUpdateCaptions();
}

void DocManager::disableContextActions()
{
    KTechlab *ktl = KTechlab::self();
    if (!ktl)
        return;

    ktl->actionByName("file_save")->setEnabled(false);
    ktl->actionByName("file_save_as")->setEnabled(false);
    ktl->actionByName("file_close")->setEnabled(false);
    ktl->actionByName("file_print")->setEnabled(false);
    ktl->actionByName("edit_undo")->setEnabled(false);
    ktl->actionByName("edit_redo")->setEnabled(false);
    ktl->actionByName("edit_cut")->setEnabled(false);
    ktl->actionByName("edit_copy")->setEnabled(false);
    ktl->actionByName("edit_paste")->setEnabled(false);
    ktl->actionByName("view_split_leftright")->setEnabled(false);
    ktl->actionByName("view_split_topbottom")->setEnabled(false);
}

TextDocument *DocManager::createTextDocument()
{
    TextDocument *document = TextDocument::constructTextDocument(untitledName(Document::dt_text));
    handleNewDocument(document);
    return document;
}

CircuitDocument *DocManager::createCircuitDocument()
{
    CircuitDocument *document = new CircuitDocument(untitledName(Document::dt_circuit));
    handleNewDocument(document);
    if (KTLConfig::raiseItemSelectors())
        KTechlab::self()->showToolView(KTechlab::self()->toolView(ComponentSelector::toolViewIdentifier()));
    return document;
}

FlowCodeDocument *DocManager::createFlowCodeDocument()
{
    FlowCodeDocument *document = new FlowCodeDocument(untitledName(Document::dt_flowcode));
    handleNewDocument(document);
    if (KTLConfig::raiseItemSelectors())
        KTechlab::self()->showToolView(KTechlab::self()->toolView(FlowPartSelector::toolViewIdentifier()));
    return document;
}

MechanicsDocument *DocManager::createMechanicsDocument()
{
    MechanicsDocument *document = new MechanicsDocument(untitledName(Document::dt_mechanics));
    handleNewDocument(document);
    if (KTLConfig::raiseItemSelectors())
        KTechlab::self()->showToolView(KTechlab::self()->toolView(MechanicsSelector::toolViewIdentifier()));
    return document;
}

CircuitDocument *DocManager::openCircuitFile(const QUrl &url, ViewArea *viewArea)
{
    CircuitDocument *document = new CircuitDocument(url.fileName());

    if (!document->openURL(url)) {
        KMessageBox::sorry(nullptr, i18n("Could not open Circuit file \"%1\"", url.toDisplayString(QUrl::PreferLocalFile)));
        document->deleteLater();
        return nullptr;
    }

    handleNewDocument(document, viewArea);
    emit fileOpened(url);
    return document;
}

FlowCodeDocument *DocManager::openFlowCodeFile(const QUrl &url, ViewArea *viewArea)
{
    FlowCodeDocument *document = new FlowCodeDocument(url.fileName());

    if (!document->openURL(url)) {
        KMessageBox::sorry(nullptr, i18n("Could not open FlowCode file \"%1\"", url.toDisplayString(QUrl::PreferLocalFile)));
        document->deleteLater();
        return nullptr;
    }

    handleNewDocument(document, viewArea);
    emit fileOpened(url);
    return document;
}

MechanicsDocument *DocManager::openMechanicsFile(const QUrl &url, ViewArea *viewArea)
{
    MechanicsDocument *document = new MechanicsDocument(url.fileName());

    if (!document->openURL(url)) {
        KMessageBox::sorry(nullptr, i18n("Could not open Mechanics file \"%1\"", url.toDisplayString(QUrl::PreferLocalFile)));
        document->deleteLater();
        return nullptr;
    }

    handleNewDocument(document, viewArea);
    emit fileOpened(url);
    return document;
}

TextDocument *DocManager::openTextFile(const QUrl &url, ViewArea *viewArea)
{
    TextDocument *document = TextDocument::constructTextDocument(url.fileName());

    if (!document)
        return nullptr;

    if (!document->openURL(url)) {
        KMessageBox::sorry(nullptr, i18n("Could not open text file \"%1\"", url.toDisplayString(QUrl::PreferLocalFile)));
        document->deleteLater();
        return nullptr;
    }

    handleNewDocument(document, viewArea);
    emit fileOpened(url);
    return document;
}
