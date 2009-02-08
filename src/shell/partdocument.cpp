/***************************************************************************
 *   Copyright 2007 Alexander Dymo  <adymo@kdevelop.org>            *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU Library General Public License as       *
 *   published by the Free Software Foundation; either version 2 of the    *
 *   License, or (at your option) any later version.                       *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU Library General Public     *
 *   License along with this program; if not, write to the                 *
 *   Free Software Foundation, Inc.,                                       *
 *   51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.         *
 ***************************************************************************/
#include "partdocument.h"

#include <KMessageBox>
#include <KLocale>

#include <sublime/area.h>
#include <sublime/view.h>
#include <sublime/mainwindow.h>

#include "core.h"
#include "uicontroller.h"
#include "partcontroller.h"
#include "documentcontroller.h"

namespace KDevelop {

struct PartDocumentPrivate {
    QMap<QWidget*, KParts::Part*> partForView;
};

PartDocument::PartDocument(const KUrl &url, ICore* core)
    : Sublime::UrlDocument(core->uiController()->controller(), url), KDevelop::IDocument(core), d(new PartDocumentPrivate)
{
}

PartDocument::~PartDocument()
{
    delete d;
}

QWidget *PartDocument::createViewWidget(QWidget */*parent*/)
{
    KParts::Part *part = Core::self()->partControllerInternal()->createPart(url());
    if( part )
    {
        Core::self()->partController()->addPart(part);
        QWidget *w = part->widget();
        d->partForView[w] = part;
        return w;
    }
    return 0;
}

KParts::Part *PartDocument::partForView(QWidget *view) const
{
    return d->partForView[view];
}



//KDevelop::IDocument implementation


KMimeType::Ptr PartDocument::mimeType() const
{
    return KMimeType::findByUrl(url());
}

KTextEditor::Document *PartDocument::textDocument() const
{
    return 0;
}

bool PartDocument::isActive() const
{
    return Core::self()->uiControllerInternal()->activeSublimeWindow()->activeView()->document() == this;
}

bool PartDocument::save(DocumentSaveMode /*mode*/)
{
    //part document is read-only so do nothing here
    return true;
}

bool PartDocument::close(DocumentSaveMode mode)
{
    if (!(mode & Discard)) {
        if (mode & Silent) {
            if (!save(mode))
                return false;

        } else {
            if (state() == IDocument::Modified) {
                int code = KMessageBox::warningYesNoCancel(
                    Core::self()->uiController()->activeMainWindow(),
                    i18n("The document \"%1\" has unsaved changes. Would you like to save them?", url().toLocalFile()),
                    i18n("Close Document"));

                if (code == KMessageBox::Yes) {
                    if (!save(mode))
                        return false;

                } else if (code == KMessageBox::Cancel) {
                    return false;
                }

            } else if (state() == IDocument::DirtyAndModified) {
                if (!save(mode))
                    return false;
            }
        }
    }


    //close all views and then delete ourself
    ///@todo test this
    foreach (Sublime::Area *area,
             Core::self()->uiControllerInternal()->allAreas())
    {
        QList<Sublime::View*> areaViews = area->views();
        foreach (Sublime::View *view, areaViews) {
            if (views().contains(view)) {
                area->removeView(view);
                delete view;
            }
        }
    }

    foreach (KParts::Part* part, d->partForView)
        part->deleteLater();

    Core::self()->documentControllerInternal()->notifyDocumentClosed(this);

    // Here we go...
    deleteLater();

    return true;
}

void PartDocument::closeDocument() {
    close();
}

void PartDocument::reload()
{
    //part document is read-only so do nothing here
}

IDocument::DocumentState PartDocument::state() const
{
    return Clean;
}

void PartDocument::activate(Sublime::View *activeView, KParts::MainWindow *mainWindow)
{
    Q_UNUSED(mainWindow);
    KParts::Part *part = partForView(activeView->widget());
    if (Core::self()->partController()->activePart() != part)
        Core::self()->partController()->setActivePart(part);
    notifyActivated();
}

KTextEditor::Cursor KDevelop::PartDocument::cursorPosition() const
{
    return KTextEditor::Cursor::invalid();
}

void PartDocument::setCursorPosition(const KTextEditor::Cursor &cursor)
{
    //do nothing here
    Q_UNUSED(cursor);
}

void PartDocument::setTextSelection(const KTextEditor::Range &range)
{
    Q_UNUSED(range);
}

KUrl PartDocument::url() const
{
    return Sublime::UrlDocument::url();
}

void PartDocument::setUrl(const KUrl& newUrl)
{
    Sublime::UrlDocument::setUrl(newUrl);
    notifyUrlChanged();
}

}

#include "partdocument.moc"
