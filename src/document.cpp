/***************************************************************************
 *   Copyright (C) 2005 by David Saxton                                    *
 *   david@bluehaze.org                                                    *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#include "document.h"
#include "docmanager.h"
#include "documentiface.h"
#include "ktechlab.h"
#include "projectmanager.h"
#include "view.h"
#include "viewcontainer.h"

#include <KLocalizedString>
#include <KMessageBox>

#include <QFileDialog>
#include <QTabWidget>
#include <QDebug>

Document::Document(const QString &caption)
    : QObject(KTechlab::self())
    , b_modified(false)
    , m_pDocumentIface(nullptr)
    , m_bDeleted(false)
    , m_pActiveView(nullptr)
    , m_caption(caption)
    , m_bAddToProjectOnSave(false)
    , m_dcopID(0)
    , m_nextViewID(0)
{
    connect(KTechlab::self(), &KTechlab::configurationChanged, this, &Document::slotUpdateConfiguration);
}

Document::~Document()
{
    m_bDeleted = true;

    ViewList viewsToDelete = m_viewList;
    const ViewList::iterator end = viewsToDelete.end();
    for (ViewList::iterator it = viewsToDelete.begin(); it != end; ++it)
        (*it)->deleteLater();
}

void Document::handleNewView(View *view)
{
    if (!view || m_viewList.contains(view))
        return;

    m_viewList.append(view);
    view->setDCOPID(m_nextViewID++);
    view->setWindowTitle(m_caption);
    connect(view, &View::destroyed, this, &Document::slotViewDestroyed);
    connect(view, &View::focused, this, &Document::slotViewFocused);
    connect(view, &View::unfocused, this, &Document::viewUnfocused);

    view->show();

    if (!DocManager::self()->getFocusedView())
        view->setFocus();
}

void Document::slotViewDestroyed(QObject *obj)
{
    View *view = static_cast<View *>(obj);

    m_viewList.removeAll(view);

    if (m_pActiveView == static_cast<QPointer<View> >(view)) {
        m_pActiveView = nullptr;
        Q_EMIT viewUnfocused();
    }

    if (m_viewList.isEmpty())
        deleteLater();
}

void Document::slotViewFocused(View *view)
{
    if (!view)
        return;

    m_pActiveView = view;
    Q_EMIT viewFocused(view);
}

void Document::setCaption(const QString &caption)
{
    m_caption = caption;
    const ViewList::iterator end = m_viewList.end();
    for (ViewList::iterator it = m_viewList.begin(); it != end; ++it)
        (*it)->setWindowTitle(caption);
}

bool Document::getURL(const QString &types, const QString &fileExtToEnforce)
{
    QUrl url = QFileDialog::getSaveFileUrl(KTechlab::self(), i18n("Save Location"), QUrl(), types);

    if (url.isEmpty())
        return false;

    if (url.isLocalFile() && QFile::exists(url.toLocalFile())) {
        // warningTwoActionsCancel
        // https://api.kde.org/frameworks/kwidgetsaddons/html/namespaceKMessageBox.html#afc8623f694c53c0e4e33b0be47fe3db3
        //int query = KMessageBox::warningYesNo(KTechlab::self(), i18n("A file named \"%1\" already exists. Are you sure you want to overwrite it?", url.fileName()), i18n("Overwrite File?"));
        int query = KMessageBox::warningTwoActions(
            KTechlab::self(),
            i18n("A file named \"%1\" already exists. Are you sure you want to overwrite it?", url.fileName()),
            i18n("Overwrite File?"),
            KStandardGuiItem::overwrite(),
            KStandardGuiItem::cancel()) ;
        if (query == QMessageBox::No)
            return false;
    }

    if (!url.fileName().endsWith(fileExtToEnforce)) {
        QUrl newUrl = QUrl( url.url().append(fileExtToEnforce) );
        qInfo() << "Document::getURL: overriding URL without extension '" << url.toString() << "' with '" << newUrl << "'";
        url = newUrl;
    }
    qInfo() << "Document::getURL: in types='" << types << "', out url='" << url.toString() << "'";

    setURL(url);

    return true;
}

bool Document::fileClose()
{
    if (isModified()) {
        // If the filename is empty then it must  be an untitled file.
        QString name = m_url.fileName().isEmpty() ? caption() : m_url.fileName();

        if (ViewContainer *viewContainer = (activeView() ? activeView()->viewContainer() : nullptr))
            KTechlab::self()->tabWidget()->setCurrentIndex(KTechlab::self()->tabWidget()->indexOf(viewContainer));

        // warningTwoActionsCancel
        // https://api.kde.org/frameworks/kwidgetsaddons/html/namespaceKMessageBox.html#afc8623f694c53c0e4e33b0be47fe3db3
        int choice = KMessageBox::warningTwoActions(KTechlab::self(), i18n("The document \'%1\' has been modified.\nDo you want to save it?", name), i18n("Save Document?"), KStandardGuiItem::save(), KStandardGuiItem::discard());

        if (choice == QMessageBox::Cancel)
            return false;
        if (choice == QMessageBox::Yes)
            fileSave();
    }

    deleteLater();
    return true;
}

void Document::setModified(bool modified)
{
    if (b_modified == modified)
        return;

    b_modified = modified;

    if (!m_bDeleted)
        Q_EMIT modifiedStateChanged();
}

void Document::setURL(const QUrl &url)
{
    if (m_url == url)
        return;

    bool wasEmpty = m_url.isEmpty();
    m_url = url;

    if (wasEmpty && m_bAddToProjectOnSave && ProjectManager::self()->currentProject())
        ProjectManager::self()->currentProject()->addFile(m_url);

    Q_EMIT fileNameChanged(url);

    if (KTechlab::self()) {
        KTechlab::self()->addRecentFile(url);
        KTechlab::self()->requestUpdateCaptions();
    }
}

DCOPObject *Document::dcopObject() const
{
    return m_pDocumentIface;
}

void Document::setDCOPID(unsigned id)
{
    if (m_dcopID == id)
        return;

    m_dcopID = id;
    if (m_pDocumentIface) {
        QString docID;
        docID.setNum(dcopID());
        m_pDocumentIface->setObjId("Document#" + docID);
    }
}

#include "moc_document.cpp"
