/***************************************************************************
 *   Copyright (C) 2005 by David Saxton                                    *
 *   david@bluehaze.org                                                    *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#include "view.h"
#include "document.h"
#include "iteminterface.h"
#include "ktechlab.h"
#include "viewcontainer.h"
#include "viewiface.h"

#include <KActionCollection>
#include <KLocalizedString>
#include <KSqueezedTextLabel>
#include <KXMLGUIFactory>

#include <QApplication>
#include <QPaintEvent>
#include <QStyle>
#include <QVBoxLayout>

#include <cassert>

#include <ktechlab_debug.h>

// BEGIN class View
View::View(Document *document, ViewContainer *viewContainer, uint viewAreaId)
    : QWidget(viewContainer->viewArea(viewAreaId))
    , KXMLGUIClient()
{
    setObjectName("view_" + QString::number(viewAreaId));
    m_pFocusWidget = nullptr;
    m_dcopID = 0;
    m_viewAreaId = viewAreaId;
    m_pDocument = document;
    p_viewContainer = viewContainer;
    m_pViewIface = nullptr;

    setFocusPolicy(Qt::ClickFocus);

    if (ViewArea *viewArea = viewContainer->viewArea(viewAreaId))
        viewArea->setView(this);

    else
        qCDebug(KTL_LOG) << " viewArea = " << viewArea;

    m_layout = new QVBoxLayout(this);
    //m_layout->setContentsMargins(0,0,0,0); // if we remove this some magic visibility calculation goes wrong

    // Don't bother creating statusbar if no ktechlab as we are not a main ktechlab tab
    if (KTechlab::self()) {
        m_statusBar = new ViewStatusBar(this);

        m_layout->addWidget(new KVSSBSep(this));
        m_layout->addWidget(m_statusBar);

        // connect(KTechlab::self(), SIGNAL(configurationChanged()), this, SLOT(slotUpdateConfiguration()));
        connect(KTechlab::self(), &KTechlab::configurationChanged, this, &View::slotUpdateConfiguration);
    }
}

View::~View()
{
    // if ( KTechlab::self() )   // 2015.09.13 - remove the XMLGUIClient from the factory, even at program close
    //	KTechlab::self()->factory()->removeClient(this);
    // 2017.01.09: do not crash on document close. factory has its clients removed in TextDocument::~TextDocument()
    // if ( factory() ) {
    //    factory()->removeClient( this );
    //} else {
    //    qCWarning(KTL_LOG) << "Null factory";
    //}
}

QAction *View::actionByName(const QString &name) const
{
    QAction *action = actionCollection()->action(name);
    if (!action)
        qCCritical(KTL_LOG) << "No such action: " << name;
    return action;
}

Document *View::document() const
{
    return m_pDocument;
}

DCOPObject *View::dcopObject() const
{
    return m_pViewIface;
}

bool View::closeView()
{
    return p_viewContainer->closeViewArea(viewAreaId());
}

void View::setFocusWidget(QWidget *focusWidget)
{
    assert(focusWidget);
    assert(!m_pFocusWidget);

    if (hasFocus())
        focusWidget->setFocus();

    m_pFocusWidget = focusWidget;
    setFocusProxy(m_pFocusWidget);
    m_pFocusWidget->installEventFilter(this);
    m_pFocusWidget->setFocusPolicy(Qt::ClickFocus);
}

bool View::eventFilter(QObject *watched, QEvent *e)
{
    // 	qCDebug(KTL_LOG) << e->type();

    if (watched != m_pFocusWidget)
        return false;

    switch (e->type()) {
    case QEvent::FocusIn: {
        p_viewContainer->setActiveViewArea(viewAreaId());

        if (KTechlab *ktl = KTechlab::self()) {
            ktl->actionByName("file_save")->setEnabled(true);
            ktl->actionByName("file_save_as")->setEnabled(true);
            ktl->actionByName("file_close")->setEnabled(true);
            ktl->actionByName("file_print")->setEnabled(true);
            ktl->actionByName("edit_paste")->setEnabled(true);
            ktl->actionByName("view_split_leftright")->setEnabled(true);
            ktl->actionByName("view_split_topbottom")->setEnabled(true);

            ItemInterface::self()->updateItemActions();
        }

        // 			qCDebug(KTL_LOG) << "Focused In";
        /* emit */ focused(this);
        break;
    }

    case QEvent::FocusOut: {
        // 			qCDebug(KTL_LOG) << "Focused Out.";
        QFocusEvent *fe = static_cast<QFocusEvent *>(e);

        if (QWidget *fw = qApp->focusWidget()) {
            QString fwClassName(fw->metaObject()->className());
            // 				qCDebug(KTL_LOG) << "New focus widget is \""<<fw->name()<<"\" of type " << fwClassName;

            if ((fwClassName != "KateViewInternal") && (fwClassName != "QViewportWidget")) {
                // 					qCDebug(KTL_LOG) << "Returning as a non-view widget has focus.";
                break;
            }
        } else {
            // 				qCDebug(KTL_LOG) << "No widget currently has focus.";
        }

        if (fe->reason() == Qt::PopupFocusReason) {
            // 				qCDebug(KTL_LOG) << "Ignoring focus-out event as was a popup.";
            break;
        }

        if (fe->reason() == Qt::ActiveWindowFocusReason) {
            // 				qCDebug(KTL_LOG) << "Ignoring focus-out event as main window lost focus.";
            break;
        }

        /* emit */ unfocused();
        break;
    }

    default:
        break;
    }

    return false;
}

void View::setDCOPID(unsigned id)
{
    if (m_dcopID == id)
        return;

    m_dcopID = id;
    if (m_pViewIface) {
        QString docID;
        docID.setNum(document()->dcopID());

        QString viewID;
        viewID.setNum(dcopID());

        m_pViewIface->setObjId("View#" + docID + "." + viewID);
    }
}
// END class View

// BEGIN class ViewStatusBar
ViewStatusBar::ViewStatusBar(View *view)
    : QStatusBar(view)
{
    p_view = view;

    m_modifiedLabel = new QLabel(this);
    addWidget(m_modifiedLabel, 0 /*, false */);
    m_fileNameLabel = new KSqueezedTextLabel(this);
    addWidget(m_fileNameLabel, 1 /*, false */);
    m_statusLabel = new QLabel(this);
    m_statusLabel->setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
    addPermanentWidget(m_statusLabel, 0);

    const int iconSize = style()->pixelMetric(QStyle::PM_SmallIconSize);
    m_modifiedPixmap = QIcon::fromTheme("document-save").pixmap(iconSize);
    m_unmodifiedPixmap = QIcon::fromTheme("null").pixmap(iconSize);

    connect(view->document(), &Document::modifiedStateChanged, this, &ViewStatusBar::slotModifiedStateChanged);
    connect(view->document(), &Document::fileNameChanged, this, &ViewStatusBar::slotFileNameChanged);

    connect(view, &View::focused, this, &ViewStatusBar::slotViewFocused);
    connect(view, &View::unfocused, this, &ViewStatusBar::slotViewUnfocused);

    slotModifiedStateChanged();
    slotFileNameChanged(view->document()->url());

    slotViewUnfocused();
}

void ViewStatusBar::setStatusText(const QString &statusText)
{
    m_statusLabel->setText(statusText);
}

void ViewStatusBar::slotModifiedStateChanged()
{
    m_modifiedLabel->setPixmap(p_view->document()->isModified() ? m_modifiedPixmap : m_unmodifiedPixmap);
}

void ViewStatusBar::slotFileNameChanged(const QUrl &url)
{
    m_fileNameLabel->setText(url.isEmpty() ? i18n("Untitled") : url.adjusted(QUrl::StripTrailingSlash).fileName());
}

void ViewStatusBar::slotViewFocused(View *)
{
    setPalette(p_view->palette());
}

void ViewStatusBar::slotViewUnfocused()
{
    QPalette pal(p_view->palette());
    pal.setColor(QPalette::Window /*QColorGroup::Background */, pal.mid().color());
    pal.setColor(QPalette::Light, pal.midlight().color());
    setPalette(pal);
}
// END class ViewStatusBar

// BEGIN class KVSSBSep
void KVSSBSep::paintEvent(QPaintEvent *e)
{
    // QPainter p( this );
    QPainter p;
    const bool beginSuccess = p.begin(this);
    if (!beginSuccess) {
        qCWarning(KTL_LOG) << " painter is not active";
    }
    // p.setPen( colorGroup().shadow() );
    // QColorGroup colorGroup(palette()); // 2018.12.02
    p.setPen(palette().shadow().color() /* colorGroup.shadow() */);
    p.drawLine(e->rect().left(), 0, e->rect().right(), 0);
    // p.setPen( ((View*)parentWidget())->hasFocus() ? colorGroup.light() : colorGroup.midlight() );
    View *parentView = dynamic_cast<View *>(parentWidget());
    if (!parentView) {
        qCWarning(KTL_LOG) << "parent not View for this=" << this << ", parent=" << parentWidget();
        return;
    }
    p.setPen(parentView->hasFocus() ? palette().light().color() : palette().midlight().color());
    p.drawLine(e->rect().left(), 1, e->rect().right(), 1);
}
// END  class KVSSBSep

#include "moc_view.cpp"
