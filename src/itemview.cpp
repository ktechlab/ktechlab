/***************************************************************************
 *   Copyright (C) 2005-2006 David Saxton                                  *
 *   david@bluehaze.org                                                    *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#include "itemview.h"
#include "canvasitemparts.h"
#include "canvasmanipulator.h"
#include "cnitem.h"
#include "component.h"
#include "connector.h"
#include "docmanager.h"
#include "drawpart.h"
#include "ecnode.h"
#include "itemdocument.h"
#include "itemlibrary.h"
#include "ktechlab.h"
#include "utils.h"

//#include <kaccel.h>
#include <KLocalizedString>
#include <KStandardAction>
// #include <k3popupmenu.h>
#include <KActionCollection>
#include <KToolBarPopupAction>

#include <QApplication>
#include <QCursor>
#include <QMatrix>
#include <QMenu>
#include <QMimeData>
#include <QMouseEvent>
#include <QTimer>
#include <QVBoxLayout>

#include <cmath>

#include <ktlconfig.h>
#include <ktechlab_debug.h>

// BEGIN class ItemView
ItemView::ItemView(ItemDocument *itemDocument, ViewContainer *viewContainer, uint viewAreaId)
    : View(itemDocument, viewContainer, viewAreaId)
{
    KActionCollection *ac = actionCollection();

    KStandardAction::selectAll(itemDocument, SLOT(selectAll()), ac);
    KStandardAction::zoomIn(this, SLOT(zoomIn()), ac);
    KStandardAction::zoomOut(this, SLOT(zoomOut()), ac);
    KStandardAction::actualSize(this, SLOT(actualSize()), ac)->setEnabled(false);

    // KAccel *pAccel = new KAccel(this);
    // pAccel->insert( "Cancel", i18n("Cancel"), i18n("Cancel the current operation"), Qt::Key_Escape, itemDocument, SLOT(cancelCurrentOperation()) );
    // pAccel->readSettings(); // TODO what does this do?
    QAction *pAccel = new QAction(QIcon::fromTheme("process-stop"), i18n("Cancel"), ac);
    pAccel->setObjectName("cancelCurrentOperation");
    pAccel->setShortcut(Qt::Key_Escape);
    connect(pAccel, SIGNAL(triggered(bool)), itemDocument, SLOT(cancelCurrentOperation()));
    ac->addAction("cancelCurrentOperation", pAccel);

    {
        // new KAction( i18n("Delete"), "edit-delete", Qt::Key_Delete, itemDocument, SLOT(deleteSelection()), ac, "edit_delete" );
        QAction *action = new QAction(QIcon::fromTheme("edit-delete"), i18n("Delete"), ac);
        action->setObjectName("edit_delete");
        action->setShortcut(Qt::Key_Delete);
        connect(action, SIGNAL(triggered(bool)), itemDocument, SLOT(deleteSelection()));
        ac->addAction("edit_delete", action);
    }
    {
        // new KAction( i18n("Export as Image..."), 0, 0, itemDocument, SLOT(exportToImage()), ac, "file_export_image");
        QAction *action = new QAction(QIcon::fromTheme("document-export"), i18n("Export as Image..."), ac);
        action->setObjectName("file_export_image");
        connect(action, SIGNAL(triggered(bool)), itemDocument, SLOT(exportToImage()));
        ac->addAction("file_export_image", action);
    }

    // BEGIN Item Alignment actions
    {
        // new KAction( i18n("Align Horizontally"), 0, 0, itemDocument, SLOT(alignHorizontally()), ac, "align_horizontally" );
        QAction *action = new QAction(QIcon::fromTheme("align-horizontal-center"), i18n("Align Horizontally"), ac);
        action->setObjectName("align_horizontally");
        connect(action, SIGNAL(triggered(bool)), itemDocument, SLOT(alignHorizontally()));
        ac->addAction("align_horizontally", action);
    }
    {
        // new KAction( i18n("Align Vertically"), 0, 0, itemDocument, SLOT(alignVertically()), ac, "align_vertically" );
        QAction *action = new QAction(QIcon::fromTheme("align-vertical-center"), i18n("Align Vertically"), ac);
        action->setObjectName("align_vertically");
        connect(action, SIGNAL(triggered(bool)), itemDocument, SLOT(alignVertically()));
        ac->addAction("align_vertically", action);
    }
    {
        // new KAction( i18n("Distribute Horizontally"), 0, 0, itemDocument, SLOT(distributeHorizontally()), ac, "distribute_horizontally" );
        QAction *action = new QAction(QIcon::fromTheme("distribute-horizontal-x"), i18n("Distribute Horizontally"), ac);
        action->setObjectName("distribute_horizontally");
        connect(action, SIGNAL(triggered(bool)), itemDocument, SLOT(distributeHorizontally()));
        ac->addAction("distribute_horizontally", action);
    }
    {
        // new KAction( i18n("Distribute Vertically"), 0, 0, itemDocument, SLOT(distributeVertically()), ac, "distribute_vertically" );
        QAction *action = new QAction(QIcon::fromTheme("distribute-vertical-y"), i18n("Distribute Vertically"), ac);
        action->setObjectName("distribute_vertically");
        connect(action, SIGNAL(triggered(bool)), itemDocument, SLOT(distributeVertically()));
        ac->addAction("distribute_vertically", action);
    }
    // END Item Alignment actions

    // BEGIN Draw actions
    // KToolBarPopupAction * pa = new KToolBarPopupAction( i18n("Draw"), "paintbrush", 0, 0, 0, ac, "edit_draw" );
    KToolBarPopupAction *pa = new KToolBarPopupAction(QIcon::fromTheme("draw-brush"), i18n("Draw"), ac);
    pa->setObjectName("edit_draw");
    pa->setDelayed(false);
    ac->addAction("edit_draw", pa);

    QMenu *m = pa->menu();
    m->setTitle(i18n("Draw"));

    m->addAction(QIcon::fromTheme("draw-text"), i18n("Text"))->setData(DrawPart::da_text);
    m->addAction(QIcon::fromTheme("draw-line"), i18n("Line"))->setData(DrawPart::da_line);
    m->addAction(QIcon::fromTheme("draw-arrow"), i18n("Arrow"))->setData(DrawPart::da_arrow);
    m->addAction(QIcon::fromTheme("draw-ellipse"), i18n("Ellipse"))->setData(DrawPart::da_ellipse);
    m->addAction(QIcon::fromTheme("draw-rectangle"), i18n("Rectangle"))->setData(DrawPart::da_rectangle);
    m->addAction(QIcon::fromTheme("insert-image"), i18n("Image"))->setData(DrawPart::da_image);
    connect(m, SIGNAL(triggered(QAction *)), itemDocument, SLOT(slotSetDrawAction(QAction *)));
    // END Draw actions

    // BEGIN Item Control actions
    {
        // new KAction( i18n("Raise Selection"), "object-order-raise", Qt::Key_PageUp,   itemDocument, SLOT(raiseZ()), ac, "edit_raise" );
        QAction *action = new QAction(QIcon::fromTheme("object-order-raise"), i18n("Raise Selection"), ac);
        action->setObjectName("edit_raise");
        action->setShortcut(Qt::Key_PageUp);
        connect(action, SIGNAL(triggered(bool)), itemDocument, SLOT(raiseZ()));
        ac->addAction("edit_raise", action);
    }
    {
        // new KAction( i18n("Lower Selection"), "object-order-lower", Qt::Key_PageDown, itemDocument, SLOT(lowerZ()), ac, "edit_lower" );
        QAction *action = new QAction(QIcon::fromTheme("object-order-lower"), i18n("Lower Selection"), ac);
        action->setObjectName("edit_lower");
        action->setShortcut(Qt::Key_PageDown);
        connect(action, SIGNAL(triggered(bool)), itemDocument, SLOT(lowerZ()));
        ac->addAction("edit_lower", action);
    }
    // END Item Control actions

    {
        // KAction * na = new KAction( "", 0, 0, 0, 0, ac, "null_action" );
        QAction *na = new QAction("", ac);
        na->setObjectName("null_action");
        na->setEnabled(false);
    }

    setXMLFile("ktechlabitemviewui.rc");

    m_pUpdateStatusTmr = new QTimer(this);
    connect(m_pUpdateStatusTmr, SIGNAL(timeout()), this, SLOT(updateStatus()));
    connect(this, SIGNAL(unfocused()), this, SLOT(stopUpdatingStatus()));

    m_pDragItem = nullptr;
    p_itemDocument = itemDocument;
    m_zoomLevel = 1.;
    m_CVBEditor = new CVBEditor(p_itemDocument->canvas(), this);
    m_CVBEditor->setObjectName("cvbEditor");
    m_CVBEditor->setLineWidth(1);

    connect(m_CVBEditor, SIGNAL(horizontalSliderReleased()), itemDocument, SLOT(requestCanvasResize()));
    connect(m_CVBEditor, SIGNAL(verticalSliderReleased()), itemDocument, SLOT(requestCanvasResize()));

    m_layout->insertWidget(0, m_CVBEditor);

    setAcceptDrops(true);

    setFocusWidget(m_CVBEditor->viewport());
}

ItemView::~ItemView()
{
}

bool ItemView::canZoomIn() const
{
    return true;
}
bool ItemView::canZoomOut() const
{
    return int(std::floor((100 * m_zoomLevel) + 0.5)) > 25;
}

QPoint ItemView::mousePosToCanvasPos(const QPoint &contentsClick) const
{
    QPoint offsetPos = contentsClick + QPoint(cvbEditor()->contentsX(), cvbEditor()->contentsY());
    return (offsetPos / zoomLevel()) + p_itemDocument->canvas()->rect().topLeft();
}

void ItemView::zoomIn(const QPoint &center)
{
    // NOTE The code in this function is nearly the same as that in zoomOut.
    // Any updates to this code should also be done to zoomOut

    // Previous position of center in widget coordinates
    QPoint previous = center * zoomLevel() - QPoint(cvbEditor()->contentsX(), cvbEditor()->contentsY());

    // Don't repaint the view until we've also shifted it
    cvbEditor()->viewport()->setUpdatesEnabled(false);

    zoomIn();

    // Adjust the contents' position to ensure that "previous" remains fixed
    QPoint offset = center * zoomLevel() - previous;
    cvbEditor()->setContentsPos(offset.x(), offset.y());

    cvbEditor()->viewport()->setUpdatesEnabled(true);
    cvbEditor()->viewport()->update();
}

void ItemView::zoomOut(const QPoint &center)
{
    // NOTE The code in this function is nearly the same as that in zoomIn.
    // Any updates to this code should also be done to zoomIn

    // Previous position of center in widget coordinates
    QPoint previous = center * zoomLevel() - QPoint(cvbEditor()->contentsX(), cvbEditor()->contentsY());

    // Don't repaint the view until we've also shifted it
    cvbEditor()->viewport()->setUpdatesEnabled(false);

    zoomOut();

    // Adjust the contents' position to ensure that "previous" remains fixed
    QPoint offset = center * zoomLevel() - previous;
    cvbEditor()->setContentsPos(offset.x(), offset.y());

    cvbEditor()->viewport()->setUpdatesEnabled(true);
    cvbEditor()->viewport()->update();
}

void ItemView::zoomIn()
{
    int currentZoomPercent = int(std::floor((100 * m_zoomLevel) + 0.5));
    int newZoom = currentZoomPercent;

    if (currentZoomPercent < 100)
        newZoom += 25;
    else if (currentZoomPercent < 200)
        newZoom += 50;
    else
        newZoom += 100;

    m_zoomLevel = newZoom / 100.0;
    m_CVBEditor->updateWorldMatrix();

    p_itemDocument->requestEvent(ItemDocument::ItemDocumentEvent::ResizeCanvasToItems);
    updateZoomActions();
}

void ItemView::zoomOut()
{
    int currentZoomPercent = int(std::floor((100 * m_zoomLevel) + 0.5));
    int newZoom = currentZoomPercent;

    if (currentZoomPercent <= 25)
        return;
    if (currentZoomPercent <= 100)
        newZoom -= 25;
    else if (currentZoomPercent <= 200)
        newZoom -= 50;
    else
        newZoom -= 100;

    m_zoomLevel = newZoom / 100.0;
    m_CVBEditor->updateWorldMatrix();

    p_itemDocument->requestEvent(ItemDocument::ItemDocumentEvent::ResizeCanvasToItems);
    updateZoomActions();
}

void ItemView::actualSize()
{
    m_zoomLevel = 1.0;
    QMatrix m(m_zoomLevel, 0.0, 0.0, m_zoomLevel, 1.0, 1.0);
    m_CVBEditor->setWorldMatrix(m);

    p_itemDocument->requestEvent(ItemDocument::ItemDocumentEvent::ResizeCanvasToItems);
    updateZoomActions();
}

void ItemView::updateZoomActions()
{
    actionByName("view_zoom_in")->setEnabled(canZoomIn());
    actionByName("view_zoom_out")->setEnabled(canZoomOut());
    actionByName("view_actual_size")->setEnabled(m_zoomLevel != 1.0);
}

void ItemView::dropEvent(QDropEvent *event)
{
    removeDragItem();
    const QMimeData *mimeData = event->mimeData();

    if (mimeData->hasUrls()) {
        // Then it is URLs that we can decode :)
        const QList<QUrl> urls = mimeData->urls();
        for (const QUrl &u : urls) {
            DocManager::self()->openURL(u);
        }
        return;
    }

    QString matchingFormat;
    for (QString format : mimeData->formats()) {
        if (format.startsWith("ktechlab/")) {
            matchingFormat = format;
            break;
        }
    }
    if (matchingFormat.isEmpty()) {
        return;
    }
    event->acceptProposedAction();

    QString text;
    // QDataStream stream( event->encodedData(event->format()), QIODevice::ReadOnly );
    QByteArray byteArray(mimeData->data(matchingFormat));

    QDataStream stream(&byteArray, QIODevice::ReadOnly);
    stream >> text;

    // Get a new component item
    p_itemDocument->addItem(text, mousePosToCanvasPos(event->pos()), true);

    setFocus();
}

void ItemView::scrollToMouse(const QPoint &pos)
{
    QPoint viewPos = pos - p_itemDocument->canvas()->rect().topLeft();
    viewPos *= m_zoomLevel;
    int x = viewPos.x();
    int y = viewPos.y();

    int left = m_CVBEditor->contentsX();
    int top = m_CVBEditor->contentsY();
    int width = m_CVBEditor->contentsWidth();
    int height = m_CVBEditor->contentsHeight();
    int right = left + m_CVBEditor->visibleWidth();
    int bottom = top + m_CVBEditor->visibleHeight();

    // A magic "snap" region whereby if the mouse is near the edge of the canvas,
    // then assume that we want to scroll right up to it
    int snapMargin = 32;

    if (x < snapMargin)
        x = 0;
    else if (x > width - snapMargin)
        x = width;

    if (y < snapMargin)
        y = 0;
    else if (y > height - snapMargin)
        y = height;

    if (x < left)
        m_CVBEditor->scrollBy(x - left, 0);
    else if (x > right)
        m_CVBEditor->scrollBy(x - right, 0);

    if (y < top)
        m_CVBEditor->scrollBy(0, y - top);
    else if (y > bottom)
        m_CVBEditor->scrollBy(0, y - bottom);
}

void ItemView::contentsMousePressEvent(QMouseEvent *e)
{
    if (!e)
        return;

    e->accept();

    if (!p_itemDocument)
        return;

    EventInfo eventInfo(this, e);

    if (eventInfo.isRightClick && m_pDragItem) {
        // We are dragging an item, and the user has right clicked.
        // Therefore, we want to rotate the item.
        /// @todo we should implement a virtual method in item for "rotating the item by one"
        /// - whatever that one may be (e.g. by 90 degrees, or changing the pin layout for
        /// flowparts, or nothing if the item isn't rotatable).
        if (Component *c = dynamic_cast<Component *>(m_pDragItem))
            c->setAngleDegrees(c->angleDegrees() + 90);

        return;
    }

    p_itemDocument->canvas()->setMessage(QString());
    p_itemDocument->m_cmManager->mousePressEvent(eventInfo);
}

void ItemView::contentsMouseDoubleClickEvent(QMouseEvent *e)
{
    if (!e)
        return;

    e->accept();

    // HACK: Pass this of as a single press event if widget underneath
    KtlQCanvasItem *atTop = p_itemDocument->itemAtTop(e->pos() / zoomLevel());
    if (dynamic_cast<Widget *>(atTop))
        contentsMousePressEvent(e);
    else
        p_itemDocument->m_cmManager->mouseDoubleClickEvent(EventInfo(this, e));
}

void ItemView::contentsMouseMoveEvent(QMouseEvent *e)
{
    // 	qCDebug(KTL_LOG) << "state = " << e->state() << endl;

    if (!e || !p_itemDocument)
        return;

    e->accept();

    EventInfo eventInfo(this, e);

    p_itemDocument->m_cmManager->mouseMoveEvent(eventInfo);
    if (!m_pUpdateStatusTmr->isActive())
        startUpdatingStatus();
}

void ItemView::contentsMouseReleaseEvent(QMouseEvent *e)
{
    if (!e)
        return;

    e->accept();

    p_itemDocument->m_cmManager->mouseReleaseEvent(EventInfo(this, e));
}

void ItemView::contentsWheelEvent(QWheelEvent *e)
{
    if (!e)
        return;

    e->accept();
    EventInfo eventInfo(this, e);
    if (eventInfo.ctrlPressed) {
        // Zooming in or out

        if (eventInfo.scrollDelta > 0)
            zoomIn(eventInfo.pos);
        else
            zoomOut(eventInfo.pos);

        return;
    }

    p_itemDocument->m_cmManager->wheelEvent(eventInfo);
}

void ItemView::dragEnterEvent(QDragEnterEvent *event)
{
    startUpdatingStatus();

    if (event->mimeData()->hasUrls()) {
        event->setAccepted(true);
        // Then it is URLs that we can decode later :)
        return;
    }
}

void ItemView::createDragItem(QDragEnterEvent *e)
{
    removeDragItem();

    const QMimeData *mimeData = e->mimeData();
    QString matchingFormat;
    for (QString format : mimeData->formats()) {
        if (format.startsWith("ktechlab/")) {
            matchingFormat = format;
            break;
        }
    }
    if (matchingFormat.isEmpty()) {
        qCWarning(KTL_LOG) << "Invalid mime data" << mimeData->formats();
        return;
    }

    e->accept();

    QString text;
    // QDataStream stream( e->encodedData(e->format()), QIODevice::ReadOnly );
    // QByteArray byteArray( e->encodedData(e->format()) );
    QByteArray byteArray(mimeData->data(matchingFormat));
    QDataStream stream(&byteArray, QIODevice::ReadOnly);
    stream >> text;

    QPoint p = mousePosToCanvasPos(e->pos());

    m_pDragItem = itemLibrary()->createItem(text, p_itemDocument, true);

    if (CNItem *cnItem = dynamic_cast<CNItem *>(m_pDragItem)) {
        cnItem->move(snapToCanvas(p.x()), snapToCanvas(p.y()));
    } else {
        m_pDragItem->move(p.x(), p.y());
    }

    m_pDragItem->show();
}

void ItemView::removeDragItem()
{
    if (!m_pDragItem)
        return;

    m_pDragItem->removeItem();
    p_itemDocument->flushDeleteList();
    m_pDragItem = nullptr;
}

void ItemView::dragMoveEvent(QDragMoveEvent *e)
{
    if (!m_pDragItem)
        return;

    QPoint p = mousePosToCanvasPos(e->pos());

    if (CNItem *cnItem = dynamic_cast<CNItem *>(m_pDragItem)) {
        cnItem->move(snapToCanvas(p.x()), snapToCanvas(p.y()));
    } else {
        m_pDragItem->move(p.x(), p.y());
    }
}

void ItemView::dragLeaveEvent(QDragLeaveEvent *)
{
    removeDragItem();
}

void ItemView::enterEvent(QEvent *e)
{
    Q_UNUSED(e);
    startUpdatingStatus();
}

void ItemView::leaveEvent(QEvent *e)
{
    Q_UNUSED(e);
    stopUpdatingStatus();

    // Cleanup
    setCursor(Qt::ArrowCursor);

    if (KTechlab::self())
        KTechlab::self()->slotChangeStatusbar(QString());

    if (p_itemDocument)
        p_itemDocument->m_canvasTip->setVisible(false);
}

void ItemView::requestDocumentResizeToCanvasItems()
{
    p_itemDocument->requestEvent(ItemDocument::ItemDocumentEvent::ResizeCanvasToItems);
}

void ItemView::slotUpdateConfiguration()
{
    // 	m_CVBEditor->setEraseColor( KTLConfig::bgColor() );
    // m_CVBEditor->setEraseColor( Qt::white ); // 2018.12.02
    QPalette pe;
    pe.setColor(m_CVBEditor->backgroundRole(), Qt::white);
    m_CVBEditor->setPalette(pe);

    if (m_pUpdateStatusTmr->isActive())
        startUpdatingStatus();
}

void ItemView::startUpdatingStatus()
{
    m_pUpdateStatusTmr->stop();
    m_pUpdateStatusTmr->start(int(1000. / KTLConfig::refreshRate()));
}

void ItemView::stopUpdatingStatus()
{
    m_pUpdateStatusTmr->stop();
}

void ItemView::updateStatus()
{
    QPoint pos = mousePosToCanvasPos(m_CVBEditor->mapFromGlobal(QCursor::pos()));

    ItemDocument *itemDocument = static_cast<ItemDocument *>(document());
    if (!itemDocument)
        return;

    CMManager *cmManager = itemDocument->m_cmManager;
    CanvasTip *canvasTip = itemDocument->m_canvasTip;

    bool displayTip = false;
    QCursor cursor = Qt::ArrowCursor;
    QString statusbar;

    if (cmManager->cmState() & CMManager::cms_repeated_add) {
        cursor = Qt::CrossCursor;
        statusbar = i18n("Left click to add. Right click to resume normal editing");
    } else if (cmManager->cmState() & CMManager::cms_draw) {
        cursor = Qt::CrossCursor;
        statusbar = i18n("Click and hold to start drawing.");
    } else if (cmManager->currentManipulator()) {
        switch (cmManager->currentManipulator()->type()) {
        case CanvasManipulator::RepeatedItemAdd:
            cursor = Qt::CrossCursor;
            statusbar = i18n("Left click to add. Right click to resume normal editing");
            break;
        case CanvasManipulator::ManualConnector:
            statusbar = i18n("Right click to cancel the connector");
            // no break
        case CanvasManipulator::AutoConnector:
            cursor = Qt::CrossCursor;
            break;
        case CanvasManipulator::ItemMove:
        case CanvasManipulator::MechItemMove:
            cursor = Qt::SizeAllCursor;
            break;
        case CanvasManipulator::Draw:
            cursor = Qt::CrossCursor;
            break;
        default:
            break;
        }

    } else if (KtlQCanvasItem *qcanvasItem = itemDocument->itemAtTop(pos)) {
        if (Connector *con = dynamic_cast<Connector *>(qcanvasItem)) {
            cursor = Qt::CrossCursor;
            if (itemDocument->type() == Document::dt_circuit) {
                canvasTip->displayVI(con, pos);
                displayTip = true;
            }
        } else if (Node *node = dynamic_cast<Node *>(qcanvasItem)) {
            cursor = Qt::CrossCursor;
            if (ECNode *ecnode = dynamic_cast<ECNode *>(node)) {
                canvasTip->displayVI(ecnode, pos);
                displayTip = true;
            }
        } else if (CNItem *item = dynamic_cast<CNItem *>(qcanvasItem)) {
            statusbar = item->name();
        }
    }
    setCursor(cursor);

    if (KTechlab::self())
        KTechlab::self()->slotChangeStatusbar(statusbar);

    canvasTip->setVisible(displayTip);
}

// END class ItemView

// BEGIN class CVBEditor
CVBEditor::CVBEditor(Canvas *canvas, ItemView *itemView)
    : KtlQCanvasView(canvas, itemView /*,Qt::WNoAutoErase | Qt::WA_StaticContents */)
{
    setAttribute(Qt::WA_StaticContents);
    m_pCanvas = canvas;
    b_ignoreEvents = false;
    b_passEventsToView = true;
    p_itemView = itemView;

    setMouseTracking(true);
    viewport()->setMouseTracking(true);
    setAcceptDrops(true);
    setFrameShape(NoFrame);
    // 	setEraseColor( KTLConfig::bgColor() );
    // setEraseColor( Qt::white );
    // setPaletteBackgroundColor( Qt::white );
    {
        QPalette p;
        p.setColor(backgroundRole(), Qt::white);
        setPalette(p);
    }
    // viewport()->setEraseColor( Qt::white );
    // viewport()->setPaletteBackgroundColor( Qt::white ); // 2018.12.02
    {
        QPalette pv;
        pv.setColor(viewport()->backgroundRole(), Qt::white);
        viewport()->setPalette(pv);
    }

    connect(canvas, SIGNAL(resized(const QRect &, const QRect &)), this, SLOT(canvasResized(const QRect &, const QRect &)));
}

void CVBEditor::canvasResized(const QRect &oldSize, const QRect &newSize)
{
    updateWorldMatrix();

    return;

    qCDebug(KTL_LOG) << endl;

    QPoint delta = oldSize.topLeft() - newSize.topLeft();
    delta *= p_itemView->zoomLevel();
    scrollBy(delta.x(), delta.y());
}

void CVBEditor::updateWorldMatrix()
{
    double z = p_itemView->zoomLevel();
    QRect r = m_pCanvas->rect();
    // 	QMatrix m( z, 0.0, 0.0, z, -r.left(), -r.top() );
    // 	QMatrix m( z, 0.0, 0.0, z, 0.0, 0.0 );
    QMatrix m;
    m.scale(z, z);
    m.translate(-r.left(), -r.top());
    setWorldMatrix(m);
}

void CVBEditor::contentsWheelEvent(QWheelEvent *e)
{
    QWheelEvent ce(viewport()->mapFromGlobal(e->globalPos()),
                   e->globalPos(),
                   e->delta(),
                   // e->state()
                   e->buttons(),
                   e->modifiers());

    if (e->orientation() == Qt::Horizontal && horizontalScrollBar())
        QApplication::sendEvent(horizontalScrollBar(), e);
    else if (e->orientation() == Qt::Vertical && verticalScrollBar())
        QApplication::sendEvent(verticalScrollBar(), e);

#if 0
	if ( b_ignoreEvents )
		return;
	b_ignoreEvents = true;
	KtlQCanvasView::wheelEvent( e );
	b_ignoreEvents = false;
#endif
}

bool CVBEditor::event(QEvent *e)
{
    if (!b_passEventsToView) {
        bool isWheel = e->type() == QEvent::Wheel;
        if (isWheel && b_ignoreEvents)
            return false;

        b_ignoreEvents = isWheel;
        bool accepted = KtlQCanvasView::event(e);
        b_ignoreEvents = false;
        return accepted;
    }

    switch (e->type()) {
    case QEvent::MouseButtonPress:
        p_itemView->contentsMousePressEvent((QMouseEvent *)e);
        return ((QMouseEvent *)e)->isAccepted();

    case QEvent::MouseButtonRelease:
        p_itemView->contentsMouseReleaseEvent((QMouseEvent *)e);
        return ((QMouseEvent *)e)->isAccepted();

    case QEvent::MouseButtonDblClick:
        p_itemView->contentsMouseDoubleClickEvent((QMouseEvent *)e);
        return ((QMouseEvent *)e)->isAccepted();

    case QEvent::MouseMove:
        p_itemView->contentsMouseMoveEvent((QMouseEvent *)e);
        return ((QMouseEvent *)e)->isAccepted();

    case QEvent::DragEnter:
        p_itemView->dragEnterEvent((QDragEnterEvent *)e);
        return true;

    case QEvent::DragMove:
        p_itemView->dragMoveEvent((QDragMoveEvent *)e);
        return true;

    case QEvent::DragLeave:
        p_itemView->dragLeaveEvent((QDragLeaveEvent *)e);
        return true;

    case QEvent::Drop:
        p_itemView->dropEvent((QDropEvent *)e);
        return true;

    case QEvent::Enter:
        p_itemView->enterEvent(e);
        return true;

    case QEvent::Leave:
        p_itemView->leaveEvent(e);
        return true;

    case QEvent::Wheel:
        p_itemView->contentsWheelEvent((QWheelEvent *)e);
        return ((QWheelEvent *)e)->isAccepted();

    default:
        return KtlQCanvasView::event(e);
    }
}

void CVBEditor::viewportResizeEvent(QResizeEvent *e)
{
    KtlQCanvasView::viewportResizeEvent(e);
    // 2018.09.26 - move to explicit method
    // p_itemView->p_itemDocument->requestEvent( ItemDocument::ItemDocumentEvent::ResizeCanvasToItems );
    p_itemView->requestDocumentResizeToCanvasItems();
}
// END class CVBEditor
