/***************************************************************************
 *   Copyright (C) 2005-2006 David Saxton                                  *
 *   david@bluehaze.org                                                    *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

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
#include "itemview.h"
#include "ktechlab.h"
#include "core/ktlconfig.h"
#include "utils.h"

#include <kaccel.h>
#include <kdebug.h>
#include <kiconloader.h>
#include <klocale.h>
#include <kpopupmenu.h>
#include <kurldrag.h>

#include <qapplication.h>
#include <qcursor.h>
#include <qtimer.h>
#include <qwmatrix.h>

#include <cmath>

#include "canvastip.h"


//BEGIN class ItemView
ItemView::ItemView( ItemDocument * itemDocument, ViewContainer *viewContainer, uint viewAreaId, const char *name )
	: View( itemDocument, viewContainer, viewAreaId, name )
{
	KActionCollection * ac = actionCollection();
	
	KStdAction::selectAll(	itemDocument,	SLOT(selectAll()),	ac );
	KStdAction::zoomIn(	this,		SLOT(zoomIn()),		ac );
	KStdAction::zoomOut(	this,		SLOT(zoomOut()),	ac );
	KStdAction::actualSize(	this,		SLOT(actualSize()),	ac )->setEnabled(false);
	
	
	KAccel *pAccel = new KAccel(this);
	pAccel->insert( "Cancel", i18n("Cancel"), i18n("Cancel the current operation"), Qt::Key_Escape, itemDocument, SLOT(cancelCurrentOperation()) );
	pAccel->readSettings();
	
	new KAction( i18n("Delete"), "editdelete", Qt::Key_Delete, itemDocument, SLOT(deleteSelection()), ac, "edit_delete" );
	new KAction( i18n("Export as Image..."), 0, 0, itemDocument, SLOT(exportToImage()), ac, "file_export_image");
	
	//BEGIN Item Alignment actions
	new KAction( i18n("Align Horizontally"), 0, 0, itemDocument, SLOT(alignHorizontally()), ac, "align_horizontally" );
	new KAction( i18n("Align Vertically"), 0, 0, itemDocument, SLOT(alignVertically()), ac, "align_vertically" );
	new KAction( i18n("Distribute Horizontally"), 0, 0, itemDocument, SLOT(distributeHorizontally()), ac, "distribute_horizontally" );
	new KAction( i18n("Distribute Vertically"), 0, 0, itemDocument, SLOT(distributeVertically()), ac, "distribute_vertically" );
	//END Item Alignment actions
	
	
	//BEGIN Draw actions
	KToolBarPopupAction * pa = new KToolBarPopupAction( i18n("Draw"), "paintbrush", 0, 0, 0, ac, "edit_draw" );
	pa->setDelayed(false);
	
	KPopupMenu * m = pa->popupMenu();
	m->insertTitle( i18n("Draw") );
	
	m->insertItem( KGlobal::iconLoader()->loadIcon( "tool_text",	KIcon::Small ), i18n("Text"),		DrawPart::da_text );
	m->insertItem( KGlobal::iconLoader()->loadIcon( "tool_line",	KIcon::Small ), i18n("Line"),		DrawPart::da_line );
	m->insertItem( KGlobal::iconLoader()->loadIcon( "tool_arrow",	KIcon::Small ), i18n("Arrow"),		DrawPart::da_arrow );
	m->insertItem( KGlobal::iconLoader()->loadIcon( "tool_ellipse",	KIcon::Small ), i18n("Ellipse"),	DrawPart::da_ellipse );
	m->insertItem( KGlobal::iconLoader()->loadIcon("tool_rectangle", KIcon::Small ), i18n("Rectangle"),	DrawPart::da_rectangle );
	m->insertItem( KGlobal::iconLoader()->loadIcon( "imagegallery",	KIcon::Small ), i18n("Image"),		DrawPart::da_image );
	connect( m, SIGNAL(activated(int)), itemDocument, SLOT(slotSetDrawAction(int)) );
	//END Draw actions
	
	
	//BEGIN Item Control actions
	new KAction( i18n("Raise Selection"), "bring_forward", Qt::Key_PageUp,   itemDocument, SLOT(raiseZ()), ac, "edit_raise" );
	new KAction( i18n("Lower Selection"), "send_backward", Qt::Key_PageDown, itemDocument, SLOT(lowerZ()), ac, "edit_lower" );
	//END Item Control actions
	
	
	KAction * na = new KAction( "", 0, 0, 0, 0, ac, "null_action" );
	na->setEnabled(false);
	
	setXMLFile( "ktechlabitemviewui.rc" );
	
	m_pUpdateStatusTmr = new QTimer(this);
	connect( m_pUpdateStatusTmr, SIGNAL(timeout()), this, SLOT(updateStatus()) );
	connect( this, SIGNAL(unfocused()), this, SLOT(stopUpdatingStatus()) );
	
	m_pDragItem = 0l;
	p_itemDocument = itemDocument;
	m_zoomLevel = 1.;
	m_CVBEditor = new CVBEditor( p_itemDocument->canvas(), this, "cvbEditor" );
	m_CVBEditor->setLineWidth(1);
	
	connect( m_CVBEditor, SIGNAL(horizontalSliderReleased()), itemDocument, SLOT(requestCanvasResize()) );
	connect( m_CVBEditor, SIGNAL(verticalSliderReleased()), itemDocument, SLOT(requestCanvasResize()) );
	
	m_layout->insertWidget( 0, m_CVBEditor );
	
	setAcceptDrops(true);

	setFocusWidget( m_CVBEditor->viewport() );
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


QPoint ItemView::mousePosToCanvasPos( const QPoint & contentsClick ) const
{
	QPoint offsetPos = contentsClick + QPoint( cvbEditor()->contentsX(), cvbEditor()->contentsY() );
	return (offsetPos / zoomLevel()) + p_itemDocument->canvas()->rect().topLeft();
}


void ItemView::zoomIn( const QPoint & center )
{
	// NOTE The code in this function is nearly the same as that in zoomOut.
	// Any updates to this code should also be done to zoomOut
	
	// Previous position of center in widget coordinates
	QPoint previous = center * zoomLevel() - QPoint( cvbEditor()->contentsX(), cvbEditor()->contentsY() );
	
	// Don't repaint the view until we've also shifted it
	cvbEditor()->viewport()->setUpdatesEnabled( false );
	
	zoomIn();
	
	// Adjust the contents' position to ensure that "previous" remains fixed
	QPoint offset = center * zoomLevel() - previous;
	cvbEditor()->setContentsPos( offset.x(), offset.y() );
	
	cvbEditor()->viewport()->setUpdatesEnabled( true );
	cvbEditor()->viewport()->update();
	
}


void ItemView::zoomOut( const QPoint & center )
{
	// NOTE The code in this function is nearly the same as that in zoomIn.
	// Any updates to this code should also be done to zoomIn
	
	// Previous position of center in widget coordinates
	QPoint previous = center * zoomLevel() - QPoint( cvbEditor()->contentsX(), cvbEditor()->contentsY() );
	
	// Don't repaint the view until we've also shifted it
	cvbEditor()->viewport()->setUpdatesEnabled( false );
	
	zoomOut();
	
	// Adjust the contents' position to ensure that "previous" remains fixed
	QPoint offset = center * zoomLevel() - previous;
	cvbEditor()->setContentsPos( offset.x(), offset.y() );
	
	cvbEditor()->viewport()->setUpdatesEnabled( true );
	cvbEditor()->viewport()->update();
}


void ItemView::zoomIn()
{
	int currentZoomPercent = int(std::floor((100*m_zoomLevel)+0.5));
	int newZoom = currentZoomPercent;
	
	if ( currentZoomPercent < 100 )
		newZoom += 25;
	else if ( currentZoomPercent < 200 )
		newZoom += 50;
	else	newZoom += 100;
	
	m_zoomLevel = newZoom/100.0;
	m_CVBEditor->updateWorldMatrix();
	
	p_itemDocument->requestEvent( ItemDocument::ItemDocumentEvent::ResizeCanvasToItems );
	updateZoomActions();
}


void ItemView::zoomOut()
{
	int currentZoomPercent = int(std::floor((100*m_zoomLevel)+0.5));
	int newZoom = currentZoomPercent;
	
	if ( currentZoomPercent <= 25 )
		return;
	if ( currentZoomPercent <= 100 )
		newZoom -= 25;
	else if ( currentZoomPercent <= 200 )
		newZoom -= 50;
	else	newZoom -= 100;
	
	m_zoomLevel = newZoom/100.0;
	m_CVBEditor->updateWorldMatrix();
	
	p_itemDocument->requestEvent( ItemDocument::ItemDocumentEvent::ResizeCanvasToItems );
	updateZoomActions();
}


void ItemView::actualSize()
{
	m_zoomLevel = 1.0;
	QWMatrix m( m_zoomLevel, 0.0, 0.0, m_zoomLevel, 1.0, 1.0 );
	m_CVBEditor->setWorldMatrix(m);
	
	p_itemDocument->requestEvent( ItemDocument::ItemDocumentEvent::ResizeCanvasToItems );
	updateZoomActions();
}


void ItemView::updateZoomActions()
{
	action("view_zoom_in")->setEnabled( canZoomIn() );
	action("view_zoom_out")->setEnabled( canZoomOut() );
	action("view_actual_size")->setEnabled( m_zoomLevel != 1.0 );
}


void ItemView::dropEvent( QDropEvent *event )
{
	removeDragItem();
	
	KURL::List urls;
	if ( KURLDrag::decode( event, urls ) )
	{
		// Then it is URLs that we can decode :)
		const KURL::List::iterator end = urls.end();
		for ( KURL::List::iterator it = urls.begin(); it != end; ++it )
		{
			DocManager::self()->openURL(*it);
		}
		return;
	}
	
	if ( !QString(event->format()).startsWith("ktechlab/") )
		return;
	
	QString text;
	QDataStream stream( event->encodedData(event->format()), IO_ReadOnly );
	stream >> text;

	// Get a new component item
	p_itemDocument->addItem( text, mousePosToCanvasPos( event->pos() ), true );
	
	setFocus();
}


void ItemView::scrollToMouse( const QPoint & pos )
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
	
	if ( x < snapMargin ) x = 0;
	else if ( x > width - snapMargin )
		x = width;
	
	if ( y < snapMargin ) y = 0;
	else if ( y > height - snapMargin )
		y = height;
	
	if ( x < left )		m_CVBEditor->scrollBy( x - left, 0 );
	else if ( x > right )	m_CVBEditor->scrollBy( x - right, 0 );
	
	if ( y < top )		m_CVBEditor->scrollBy( 0, y - top  );
	else if ( y > bottom )	m_CVBEditor->scrollBy( 0, y - bottom);
}


void ItemView::contentsMousePressEvent( QMouseEvent *e )
{
	if (!e) return;
	
	e->accept();
	
	if(!p_itemDocument ) return;
	
	EventInfo eventInfo( this, e );
	
	if ( eventInfo.isRightClick && m_pDragItem )
	{
		// We are dragging an item, and the user has right clicked.
		// Therefore, we want to rotate the item.
		/// @todo we should implement a virtual method in item for "rotating the item by one"
		/// - whatever that one may be (e.g. by 90 degrees, or changing the pin layout for
		/// flowparts, or nothing if the item isn't rotatable).
		if ( Component * c = dynamic_cast<Component*>( m_pDragItem ) )
			c->setAngleDegrees( c->angleDegrees() + 90 );
		
		return;
	}
	
	p_itemDocument->canvas()->setMessage( QString::null );
	p_itemDocument->m_cmManager->mousePressEvent( eventInfo );
}


void ItemView::contentsMouseDoubleClickEvent( QMouseEvent *e )
{
	if (!e) return;
	
	e->accept();
	
	//HACK: Pass this of as a single press event if widget underneath
	QCanvasItem * atTop = p_itemDocument->itemAtTop( e->pos()/zoomLevel() );
	if ( dynamic_cast<Widget*>(atTop) )
		contentsMousePressEvent(e);
	else	p_itemDocument->m_cmManager->mouseDoubleClickEvent( EventInfo( this, e ) );
}


void ItemView::contentsMouseMoveEvent( QMouseEvent *e )
{
// 	kdDebug() << k_funcinfo << "state = " << e->state() << endl;
	
	if ( !e || !p_itemDocument )
		return;
	
	e->accept();
	
	EventInfo eventInfo( this, e );
	
	p_itemDocument->m_cmManager->mouseMoveEvent( eventInfo );
	if ( !m_pUpdateStatusTmr->isActive() )
		startUpdatingStatus();
}


void ItemView::contentsMouseReleaseEvent( QMouseEvent *e )
{
	if (!e) return;
	
	e->accept();
	
	p_itemDocument->m_cmManager->mouseReleaseEvent( EventInfo( this, e ) );
}


void ItemView::contentsWheelEvent( QWheelEvent *e )
{
	if (!e) return;
	
	e->accept();
	EventInfo eventInfo( this, e );
	if ( eventInfo.ctrlPressed )
	{
		// Zooming in or out
		
		if ( eventInfo.scrollDelta > 0 )
			zoomIn( eventInfo.pos );
		else	zoomOut( eventInfo.pos );
		
		return;
	}
	
	p_itemDocument->m_cmManager->wheelEvent( eventInfo );
}


void ItemView::dragEnterEvent( QDragEnterEvent *event )
{
	startUpdatingStatus();
	
	KURL::List urls;
	if ( KURLDrag::decode( event, urls ) ) {
		event->accept(true);
		// Then it is URLs that we can decode later :)
		return;
	}
}


void ItemView::createDragItem( QDragEnterEvent * e )
{
	removeDragItem();
	
	if ( !QString(e->format()).startsWith("ktechlab/") )
		return;
	
	e->accept();
	
	QString text;
	QDataStream stream( e->encodedData(e->format()), IO_ReadOnly );
	stream >> text;

	QPoint p = mousePosToCanvasPos( e->pos() );
	
	m_pDragItem = itemLibrary()->createItem( text, p_itemDocument, true );
	
	if ( CNItem * cnItem = dynamic_cast<CNItem*>(m_pDragItem) )
		cnItem->move( snapToCanvas(p.x()), snapToCanvas(p.y()) );
	else m_pDragItem->move( p.x(), p.y() );
	
	m_pDragItem->show();
}


void ItemView::removeDragItem()
{
	if ( !m_pDragItem ) return;
	
	m_pDragItem->removeItem();
	p_itemDocument->flushDeleteList();
	m_pDragItem = 0l;
}


void ItemView::dragMoveEvent( QDragMoveEvent * e )
{
	if ( !m_pDragItem ) return;

	QPoint p = mousePosToCanvasPos( e->pos() );
	
	if ( CNItem * cnItem = dynamic_cast<CNItem*>(m_pDragItem) )
		cnItem->move( snapToCanvas(p.x()), snapToCanvas(p.y()) );
	else	m_pDragItem->move( p.x(), p.y() );
}


void ItemView::dragLeaveEvent( QDragLeaveEvent * )
{
	removeDragItem();
}


void ItemView::enterEvent( QEvent * e )
{
	Q_UNUSED(e);
	startUpdatingStatus();
}


void ItemView::leaveEvent( QEvent * e )
{
	Q_UNUSED(e);
	stopUpdatingStatus();
	
	// Cleanup
	setCursor(Qt::ArrowCursor);
	
	if ( KTechlab::self() )
		KTechlab::self()->slotChangeStatusbar(QString::null);
	
	if ( p_itemDocument )
		p_itemDocument->m_canvasTip->setVisible(false);
}


void ItemView::slotUpdateConfiguration()
{
// 	m_CVBEditor->setEraseColor( KTLConfig::bgColor() );
	m_CVBEditor->setEraseColor( Qt::white );
	
	if ( m_pUpdateStatusTmr->isActive() )
		startUpdatingStatus();
}


void ItemView::startUpdatingStatus()
{
	m_pUpdateStatusTmr->stop();
	m_pUpdateStatusTmr->start( int(1000./KTLConfig::refreshRate()) );
}


void ItemView::stopUpdatingStatus()
{
	m_pUpdateStatusTmr->stop();
}


void ItemView::updateStatus()
{
	QPoint pos = mousePosToCanvasPos( m_CVBEditor->mapFromGlobal( QCursor::pos() ) );
	
	ItemDocument * itemDocument = static_cast<ItemDocument*>(document());
	if ( !itemDocument ) return;
	
	CMManager * cmManager = itemDocument->m_cmManager;
	CanvasTip * canvasTip = itemDocument->m_canvasTip;
	
	bool displayTip = false;
	QCursor cursor = Qt::ArrowCursor;
	QString statusbar;
	
	if ( cmManager->cmState() & CMManager::cms_repeated_add ) {
		cursor = Qt::CrossCursor;
		statusbar = i18n("Left click to add. Right click to resume normal editing");
	} else if ( cmManager->cmState() & CMManager::cms_draw ) {
		cursor = Qt::CrossCursor;
		statusbar = i18n("Click and hold to start drawing.");
	} else if ( cmManager->currentManipulator()) {
		switch ( cmManager->currentManipulator()->type() )
		{
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
				
	} else if ( QCanvasItem *qcanvasItem = itemDocument->itemAtTop(pos) ) {
		if ( Connector * con = dynamic_cast<Connector*>(qcanvasItem) )
		{
			cursor = Qt::CrossCursor;
			if ( itemDocument->type() == Document::dt_circuit )
			{
				canvasTip->displayVI( con, pos );
				displayTip = true;
			}
		} else if ( Node * node = dynamic_cast<Node*>(qcanvasItem) ) {
			cursor = Qt::CrossCursor;
			if ( ECNode * ecnode = dynamic_cast<ECNode*>(node) )
			{
				canvasTip->displayVI( ecnode, pos );
				displayTip = true;
			}
		} else if ( CNItem * item = dynamic_cast<CNItem*>(qcanvasItem) ) {
			statusbar =item->name();
		}
	}
	setCursor(cursor);
	
	if ( KTechlab::self() )
		KTechlab::self()->slotChangeStatusbar(statusbar);
	
	canvasTip->setVisible(displayTip);
}

//END class ItemView



//BEGIN class CVBEditor
CVBEditor::CVBEditor( Canvas *canvas, ItemView *itemView, const char *name )
	: QCanvasView( canvas, itemView, name, WNoAutoErase | WStaticContents )
{
	m_pCanvas = canvas;
	b_ignoreEvents = false;
	b_passEventsToView = true;
	p_itemView = itemView;
	
	setMouseTracking(true);
	viewport()->setMouseTracking(true);
	setAcceptDrops(true);
	setFrameShape(NoFrame);
// 	setEraseColor( KTLConfig::bgColor() );
	setEraseColor( Qt::white );
	setPaletteBackgroundColor( Qt::white );
	viewport()->setEraseColor( Qt::white );
	viewport()->setPaletteBackgroundColor( Qt::white );
	
	connect( canvas, SIGNAL(resized( const QRect&, const QRect& )), this, SLOT(canvasResized( const QRect&, const QRect& )) );
}


void CVBEditor::canvasResized( const QRect & oldSize, const QRect & newSize )
{
	updateWorldMatrix();
	
	return;
	
	kdDebug() << k_funcinfo << endl;
	
	QPoint delta = oldSize.topLeft() - newSize.topLeft();
	delta *= p_itemView->zoomLevel();
	scrollBy( delta.x(), delta.y() );
}


void CVBEditor::updateWorldMatrix()
{
	double z = p_itemView->zoomLevel();
	QRect r = m_pCanvas->rect();
// 	QWMatrix m( z, 0.0, 0.0, z, -r.left(), -r.top() );
// 	QWMatrix m( z, 0.0, 0.0, z, 0.0, 0.0 );
	QWMatrix m;
	m.scale( z, z );
	m.translate( -r.left(), -r.top() );
	setWorldMatrix( m );
}


void CVBEditor::contentsWheelEvent( QWheelEvent * e )
{
	QWheelEvent ce( viewport()->mapFromGlobal( e->globalPos() ),
			e->globalPos(), e->delta(), e->state());
	
	if ( e->orientation() == Horizontal && horizontalScrollBar() )
		QApplication::sendEvent( horizontalScrollBar(), e);
	else  if (e->orientation() == Vertical && verticalScrollBar() )
		QApplication::sendEvent( verticalScrollBar(), e);
	
#if 0
	if ( b_ignoreEvents )
		return;
	b_ignoreEvents = true;
	QCanvasView::wheelEvent( e );
	b_ignoreEvents = false;
#endif
}


bool CVBEditor::event( QEvent * e )
{
	if ( !b_passEventsToView )
	{
		bool isWheel = e->type() == QEvent::Wheel;
		if ( isWheel && b_ignoreEvents )
			return false;
		
		b_ignoreEvents = isWheel;
		bool accepted = QCanvasView::event( e );
		b_ignoreEvents = false;
		return accepted;
	}
	
	switch ( e->type() )
	{
		case QEvent::MouseButtonPress:
			p_itemView->contentsMousePressEvent( (QMouseEvent*)e );
			return ((QMouseEvent*)e)->isAccepted();
			
		case QEvent::MouseButtonRelease:
			p_itemView->contentsMouseReleaseEvent( (QMouseEvent*)e );
			return ((QMouseEvent*)e)->isAccepted();
			
		case QEvent::MouseButtonDblClick:
			p_itemView->contentsMouseDoubleClickEvent( (QMouseEvent*)e );
			return ((QMouseEvent*)e)->isAccepted();
			
		case QEvent::MouseMove:
			p_itemView->contentsMouseMoveEvent( (QMouseEvent*)e );
			return ((QMouseEvent*)e)->isAccepted();
			
		case QEvent::DragEnter:
			p_itemView->dragEnterEvent((QDragEnterEvent*)e );
			return true;
			
		case QEvent::DragMove:
			p_itemView->dragMoveEvent((QDragMoveEvent*)e );
			return true;
			
		case QEvent::DragLeave:
			p_itemView->dragLeaveEvent((QDragLeaveEvent*)e );
			return true;
			
		case QEvent::Drop:
			p_itemView->dropEvent( (QDropEvent*)e );
			return true;
			
		case QEvent::Enter:
			p_itemView->enterEvent( e );
			return true;
			
		case QEvent::Leave:
			p_itemView->leaveEvent(e);
			return true;
			
		case QEvent::Wheel:
			p_itemView->contentsWheelEvent( (QWheelEvent*)e );
			return ((QWheelEvent*)e)->isAccepted();
			
		default:
			return QCanvasView::event( e );
	}
}


void CVBEditor::viewportResizeEvent( QResizeEvent * e )
{
	QCanvasView::viewportResizeEvent(e);
	p_itemView->p_itemDocument->requestEvent( ItemDocument::ItemDocumentEvent::ResizeCanvasToItems );
}
//END class CVBEditor

#include "itemview.moc"
