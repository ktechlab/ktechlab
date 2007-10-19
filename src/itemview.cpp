/***************************************************************************
 *   Copyright (C) 2005 by David Saxton                                    *
 *   david@bluehaze.org                                                    *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#include "canvasmanipulator.h"
#include "cnitem.h"
#include "connector.h"
#include "docmanager.h"
#include "drawpart.h"
#include "ecnode.h"
#include "itemdocument.h"
#include "itemview.h"
#include "ktechlab.h"
#include "core/ktlconfig.h"

#include <kaccel.h>
#include <kiconloader.h>
#include <klocale.h>
#include <kpopupmenu.h>
#include <kurldrag.h>

#include <cmath>
#include <qcursor.h>
#include <qtimer.h>
#include <qwmatrix.h>


//BEGIN class ItemView
ItemView::ItemView( ItemDocument * itemDocument, ViewContainer *viewContainer, uint viewAreaId, const char *name)
	: View( itemDocument, viewContainer, viewAreaId, name)
{
	KActionCollection * ac = actionCollection();
	
	KStdAction::selectAll(	itemDocument,	SLOT(selectAll()),		ac);
	KStdAction::zoomIn(		this,			SLOT(zoomIn()),			ac);
	KStdAction::zoomOut(	this,			SLOT(zoomOut()),		ac);
	KStdAction::actualSize(	this,			SLOT(actualSize()),		ac)->setEnabled(false);
	
	
	KAccel *pAccel = new KAccel(this);
	pAccel->insert( "Cancel", i18n("Cancel"), i18n("Cancel the current operation"), Qt::Key_Escape, itemDocument, SLOT(cancelCurrentOperation()));
	pAccel->readSettings();
	
	new KAction( i18n("Delete"), "editdelete", Qt::Key_Delete, itemDocument, SLOT(deleteSelection()), ac, "edit_delete");
	new KAction( i18n("Export as Image..."), 0, 0, itemDocument, SLOT(exportToImage()), ac, "file_export_image");
	
	//BEGIN Item Alignment actions
	new KAction( i18n("Align Horizontally"), 0, 0, itemDocument, SLOT(alignHorizontally()), ac, "align_horizontally");
	new KAction( i18n("Align Vertically"), 0, 0, itemDocument, SLOT(alignVertically()), ac, "align_vertically");
	new KAction( i18n("Distribute Horizontally"), 0, 0, itemDocument, SLOT(distributeHorizontally()), ac, "distribute_horizontally");
	new KAction( i18n("Distribute Vertically"), 0, 0, itemDocument, SLOT(distributeVertically()), ac, "distribute_vertically");
	//END Item Alignment actions
	
	
	//BEGIN Draw actions
	KToolBarPopupAction * pa = new KToolBarPopupAction( i18n("Draw"), "paintbrush", 0, 0, 0, ac, "edit_draw");
	pa->setDelayed(false);
	
	KPopupMenu * m = pa->popupMenu();
	m->insertTitle( i18n("Draw"));
	
	m->insertItem( KGlobal::iconLoader()->loadIcon( "tool_text",		KIcon::Small), i18n("Text"),		DrawPart::da_text);
	m->insertItem( KGlobal::iconLoader()->loadIcon( "tool_line",		KIcon::Small), i18n("Line"),		DrawPart::da_line);
	m->insertItem( KGlobal::iconLoader()->loadIcon( "tool_arrow",		KIcon::Small), i18n("Arrow"),		DrawPart::da_arrow);
	m->insertItem( KGlobal::iconLoader()->loadIcon( "tool_ellipse",		KIcon::Small), i18n("Ellipse"),	DrawPart::da_ellipse);
	m->insertItem( KGlobal::iconLoader()->loadIcon( "tool_rectangle",	KIcon::Small), i18n("Rectangle"),	DrawPart::da_rectangle);
	connect( m, SIGNAL(activated(int)), itemDocument, SLOT(slotSetDrawAction(int)));
	//END Draw actions
	
	
	//BEGIN Item Control actions
	new KAction( i18n("Raise Selection"), "1uparrow", Qt::Key_PageUp, itemDocument, SLOT(raiseZ()), ac, "edit_raise");
	new KAction( i18n("Lower Selection"), "1downarrow", Qt::Key_PageDown, itemDocument, SLOT(lowerZ()), ac, "edit_lower");
	//END Item Control actions
	
	
	KAction * na = new KAction( "", 0, 0, 0, 0, ac, "null_action");
	na->setEnabled(false);
	
	setXMLFile( "ktechlabitemviewui.rc");
	
	m_pUpdateStatusTmr = new QTimer(this);
	connect( m_pUpdateStatusTmr, SIGNAL(timeout()), this, SLOT(updateStatus()));
	connect( this, SIGNAL(viewUnfocused()), this, SLOT(stopUpdatingStatus()));
	
	p_itemDocument = itemDocument;
	m_zoomLevel = 1.;
	m_CVBEditor = new CVBEditor( p_itemDocument->canvas(), this, "cvbEditor");
	m_CVBEditor->setLineWidth(1);
	
	m_layout->insertWidget( 0, m_CVBEditor);
	
	setAcceptDrops(true);
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
	return int(std::floor((100*m_zoomLevel)+0.5)) > 25;
}


void ItemView::zoomIn()
{
	int currentZoomPercent = int(std::floor((100*m_zoomLevel)+0.5));
	int newZoom = currentZoomPercent;
	
	if( currentZoomPercent < 100)
		newZoom += 25;
	else if( currentZoomPercent < 200)
		newZoom += 50;
	else
		newZoom += 100;
	
	m_zoomLevel = newZoom/100.;
	
	QWMatrix m( m_zoomLevel, 0.0, 0.0, m_zoomLevel, 1.0, 1.0);
	m_CVBEditor->setWorldMatrix(m);
	
	p_itemDocument->requestEvent( ItemDocument::ItemDocumentEvent::ResizeCanvasToItems);
	updateZoomActions();
}


void ItemView::zoomOut()
{
	int currentZoomPercent = int(std::floor((100*m_zoomLevel)+0.5));
	int newZoom = currentZoomPercent;
	
	if( currentZoomPercent <= 25)
		return;
	if( currentZoomPercent <= 100)
		newZoom -= 25;
	else if( currentZoomPercent <= 200)
		newZoom -= 50;
	else
		newZoom -= 100;
	
	m_zoomLevel = newZoom/100.;
	
	QWMatrix m( m_zoomLevel, 0.0, 0.0, m_zoomLevel, 1.0, 1.0);
	m_CVBEditor->setWorldMatrix(m);
	
	p_itemDocument->requestEvent( ItemDocument::ItemDocumentEvent::ResizeCanvasToItems);
	updateZoomActions();
}


void ItemView::actualSize()
{
	m_zoomLevel = 1.0;
	QWMatrix m( m_zoomLevel, 0.0, 0.0, m_zoomLevel, 1.0, 1.0);
	m_CVBEditor->setWorldMatrix(m);
	
	p_itemDocument->requestEvent( ItemDocument::ItemDocumentEvent::ResizeCanvasToItems);
	updateZoomActions();
}


void ItemView::updateZoomActions()
{
	action("view_zoom_in")->setEnabled( canZoomIn());
	action("view_zoom_out")->setEnabled( canZoomOut());
	action("view_actual_size")->setEnabled( m_zoomLevel != 1.0);
}


void ItemView::dropEvent( QDropEvent *event)
{
	KURL::List urls;
	if( KURLDrag::decode( event, urls))
	{
		// Then it is URLs that we can decode :)
		const KURL::List::iterator end = urls.end();
		for( KURL::List::iterator it = urls.begin(); it != end; ++it)
		{
			DocManager::self()->openURL(*it);
		}
		return;
	}
	
	if( !QString(event->format()).startsWith("ktechlab/"))
		return;
	
	QString text;
	QDataStream stream( event->encodedData(event->format()), IO_ReadOnly);
	stream >> text;

	QPoint position = event->pos();
	position.setX( int((position.x() + m_CVBEditor->contentsX())/m_zoomLevel));
	position.setY( int((position.y() + m_CVBEditor->contentsY())/m_zoomLevel));

	// Get a new component item
	p_itemDocument->addItem( text, position, true);
	
	setFocus();
}


void ItemView::scrollToMouse( const QPoint &pos)
{
	QPoint position = pos * m_zoomLevel;
	
	int left = m_CVBEditor->contentsX();
	int top = m_CVBEditor->contentsY();
	int right = left + m_CVBEditor->visibleWidth();
	int bottom = top + m_CVBEditor->visibleHeight();
	
	if( position.x() < left) m_CVBEditor->scrollBy( position.x() - left, 0);
	else if( position.x() > right) m_CVBEditor->scrollBy( position.x() - right, 0);
	
	if( position.y() < top) m_CVBEditor->scrollBy( 0, position.y() - top);
	else if( position.y() > bottom) m_CVBEditor->scrollBy( 0, position.y() - bottom);
}


void ItemView::contentsMousePressEvent( QMouseEvent *e)
{
	if(!e)
		return;
	
	e->accept();
	
	// For some reason, when we are initially unfocused, we only receive the
	// release event if the user drags the mouse - not very often. So see if we
	// were initially unfocused, and if so, do unclick as well.
	bool wasFocused = isFocused();
	setFocused();
	
	if( !p_itemDocument)
		return;
	
	p_itemDocument->canvas()->setMessage( QString::null);
	p_itemDocument->m_cmManager->mousePressEvent( EventInfo( this, e));
	
	if( !wasFocused)
		p_itemDocument->m_cmManager->mouseReleaseEvent( EventInfo( this, e));
}


void ItemView::contentsMouseDoubleClickEvent( QMouseEvent *e)
{
	if(!e)
		return;
	
	e->accept();
	
	//HACK: Pass this of as a single press event if widget underneath
	QCanvasItem * atTop = p_itemDocument->itemAtTop( e->pos()/zoomLevel());
	if( atTop && atTop->rtti() == ItemDocument::RTTI::Widget)
		contentsMousePressEvent(e);
	else
		p_itemDocument->m_cmManager->mouseDoubleClickEvent( EventInfo( this, e));
}


void ItemView::contentsMouseMoveEvent( QMouseEvent *e)
{
	if( !e || !p_itemDocument)
		return;
	
	e->accept();
	
	p_itemDocument->m_cmManager->mouseMoveEvent( EventInfo( this, e));
	if( !m_pUpdateStatusTmr->isActive())
		startUpdatingStatus();
}


void ItemView::contentsMouseReleaseEvent( QMouseEvent *e)
{
	if(!e)
		return;
	
	e->accept();
	
	p_itemDocument->m_cmManager->mouseReleaseEvent( EventInfo( this, e));
}


void ItemView::contentsWheelEvent( QWheelEvent *e)
{
	if(!e)
		return;
	
	e->accept();
	
	p_itemDocument->m_cmManager->wheelEvent( EventInfo( this, e));
}


void ItemView::dragEnterEvent( QDragEnterEvent *event)
{
	startUpdatingStatus();
	
	KURL::List urls;
	if( KURLDrag::decode( event, urls))
	{
		event->accept(true);
		// Then it is URLs that we can decode later :)
		return;
	}
}


void ItemView::enterEvent( QEvent * e)
{
	Q_UNUSED(e);
	startUpdatingStatus();
}


void ItemView::leaveEvent( QEvent * e)
{
	Q_UNUSED(e);
	stopUpdatingStatus();
	
	// Cleanup
	setCursor(Qt::ArrowCursor);
	
	if(p_ktechlab)
		p_ktechlab->slotChangeStatusbar(QString::null);
	
	if( p_itemDocument)
		p_itemDocument->m_canvasTip->setVisible(false);
}


void ItemView::slotUpdateConfiguration()
{
// 	m_CVBEditor->setEraseColor( KTLConfig::bgColor());
	m_CVBEditor->setEraseColor( Qt::white);
	
	if( m_pUpdateStatusTmr->isActive())
		startUpdatingStatus();
}


void ItemView::startUpdatingStatus()
{
	m_pUpdateStatusTmr->stop();
	m_pUpdateStatusTmr->start( int(1000./KTLConfig::refreshRate()));
}


void ItemView::stopUpdatingStatus()
{
	m_pUpdateStatusTmr->stop();
}


void ItemView::updateStatus()
{
	QPoint pos = (m_CVBEditor->mapFromGlobal( QCursor::pos()) + QPoint( m_CVBEditor->contentsX(), m_CVBEditor->contentsY())) / zoomLevel();

	ItemDocument * itemDocument = static_cast<ItemDocument*>(document());
	if(!itemDocument) return;

	CMManager * cmManager = itemDocument->m_cmManager;
	CanvasTip * canvasTip = itemDocument->m_canvasTip;

	bool displayTip = false;
	QCursor cursor = Qt::ArrowCursor;
	QString statusbar;

	if( cmManager->cmState() & CMManager::cms_repeated_add) {
		cursor = Qt::CrossCursor;
		statusbar = i18n("Left click to add. Right click to resume normal editing");
	} else if( cmManager->cmState() & CMManager::cms_draw) {
		cursor = Qt::CrossCursor;
		statusbar = i18n("Click and hold to start drawing.");
	} else if( cmManager->currentManipulator()) {
		switch(cmManager->currentManipulator()->type()) {
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
	} else if(QCanvasItem *qcanvasItem = itemDocument->itemAtTop(pos)) {
		switch(qcanvasItem->rtti())
		{
			case ItemDocument::RTTI::Connector:
			{
				cursor = Qt::CrossCursor;
				if( itemDocument->type() != Document::dt_circuit)
					break;
				
				canvasTip->displayVI( static_cast<Connector*>(qcanvasItem), pos);
				displayTip = true;
				break;
			}
			case ItemDocument::RTTI::Node:
			{
				cursor = Qt::CrossCursor;
				ECNode * ecnode = dynamic_cast<ECNode*>(qcanvasItem);
				if( !ecnode)
					break;
				
				canvasTip->displayVI( ecnode, pos);
				displayTip = true;
				break;
			}
			case ItemDocument::RTTI::CNItem:
			{
				statusbar = (static_cast<CNItem*>(qcanvasItem))->name();
				break;
			}
			default:
			{
				break;
			}
		}
	}
	setCursor(cursor);

	if(p_ktechlab) p_ktechlab->slotChangeStatusbar(statusbar);

	canvasTip->setVisible(displayTip);
}

//END class ItemView



//BEGIN class CVBEditor
CVBEditor::CVBEditor( QCanvas *canvas, ItemView *itemView, const char *name)
	: QCanvasView( canvas, itemView, name, WNoAutoErase | WStaticContents)
{
	b_ignoreEvents = false;
	b_passEventsToView = true;
	p_itemView = itemView;
	viewport()->setMouseTracking(true);
	setAcceptDrops(true);
	setFrameShape(NoFrame);
// 	setEraseColor( KTLConfig::bgColor());
	setEraseColor( Qt::white);
	setPaletteBackgroundColor( Qt::white);
	viewport()->setEraseColor( Qt::white);
	viewport()->setPaletteBackgroundColor( Qt::white);
}


void CVBEditor::contentsMousePressEvent( QMouseEvent* e)
{
	if(b_passEventsToView)
		p_itemView->contentsMousePressEvent(e);
	else
		QCanvasView::contentsMousePressEvent(e);
}


void CVBEditor::contentsMouseReleaseEvent( QMouseEvent* e)
{
	if(b_passEventsToView)
		p_itemView->contentsMouseReleaseEvent(e);
	else
		QCanvasView::contentsMouseReleaseEvent(e);
}


void CVBEditor::contentsMouseDoubleClickEvent( QMouseEvent* e)
{
	if(b_passEventsToView)
		p_itemView->contentsMouseDoubleClickEvent(e);
	else
		QCanvasView::contentsMouseDoubleClickEvent(e);
}


void CVBEditor::contentsMouseMoveEvent( QMouseEvent* e)
{
	if(b_passEventsToView)
		p_itemView->contentsMouseMoveEvent(e);
	else
		QCanvasView::contentsMouseMoveEvent(e);
}


void CVBEditor::dragEnterEvent( QDragEnterEvent* e)
{
	if(b_passEventsToView)
		p_itemView->dragEnterEvent(e);
	else
		QCanvasView::dragEnterEvent(e);
}


void CVBEditor::dropEvent( QDropEvent* e)
{
	if(b_passEventsToView)
		p_itemView->dropEvent(e);
	else
		QCanvasView::dropEvent(e);
}


void CVBEditor::enterEvent( QEvent * e)
{
	if(b_passEventsToView)
		p_itemView->enterEvent(e);
	else
		QCanvasView::enterEvent(e);
}


void CVBEditor::leaveEvent( QEvent* e)
{
	if(b_passEventsToView)
		p_itemView->leaveEvent(e);
	else
		QCanvasView::leaveEvent(e);
}


void CVBEditor::contentsWheelEvent( QWheelEvent *e)
{
	if(b_ignoreEvents)
	{
		e->ignore();
		return;
	}
	
	if(b_passEventsToView)
		p_itemView->contentsWheelEvent(e);
	else
	{
		b_ignoreEvents = true;
		QCanvasView::wheelEvent(e);
		b_ignoreEvents = false;
	}
}

void CVBEditor::viewportResizeEvent( QResizeEvent * e)
{
	QCanvasView::viewportResizeEvent(e);
	p_itemView->p_itemDocument->requestEvent( ItemDocument::ItemDocumentEvent::ResizeCanvasToItems);
}
//END class CVBEditor

#include "itemview.moc"
