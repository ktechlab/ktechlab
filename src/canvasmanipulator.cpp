/***************************************************************************
 *   Copyright (C) 2004-2006 by David Saxton                               *
 *   david@bluehaze.org                                                    *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#include "cnitemgroup.h"
#include "canvasitemparts.h"
#include "dptext.h"
#include "canvasmanipulator.h"
#include "connector.h"
#include "flowcontainer.h"
#include "icndocument.h"
#include "itemview.h"
#include "mechanicsdocument.h"
#include "mechanicsgroup.h"
#include "mechanicsitem.h"
#include "node.h"
#include "nodegroup.h"
#include "picitem.h"
#include "resizeoverlay.h"

#include "utils.h"
#include <cmath>
#include <cstdlib>

#include <kconfig.h>
#include <kdebug.h>
#include <kglobal.h>
#include <kconfiggroup.h>
#include <ksharedconfig.h>

#include <Qt/qcursor.h>
#include <Qt/qpainter.h>
#include <Qt/qtimer.h>

// FIXME: This source file is HUUUGE!!!, contains numerous clases, should be broken down. 


//BEGIN class CMManager
CMManager::CMManager( ItemDocument *itemDocument )
	: QObject()
{
	b_allowItemScroll = true;
	p_lastMouseOverResizeHandle = 0l;
	m_canvasManipulator = 0l;
	p_itemDocument = itemDocument;
	m_cmState = 0;
	p_lastMouseOverItem = 0l;
	p_lastItemClicked = 0l;
	m_drawAction = -1;
	m_allowItemScrollTmr = new QTimer(this);
	connect( m_allowItemScrollTmr, SIGNAL(timeout()), this, SLOT(slotAllowItemScroll()) );
	
	KConfigGroup grGen = KGlobal::config()->group("General");
	slotSetManualRoute( grGen.readEntry( "ManualRouting", false ) );
}


CMManager::~CMManager()
{
	delete m_allowItemScrollTmr;
	delete m_canvasManipulator;

	const ManipulatorInfoList::iterator end = m_manipulatorInfoList.end();
	for ( ManipulatorInfoList::iterator it = m_manipulatorInfoList.begin(); it != end; ++it )
	{
		delete *it;
	}
	m_manipulatorInfoList.clear();
}


void CMManager::addManipulatorInfo( ManipulatorInfo *eventInfo )
{
	if ( eventInfo && !m_manipulatorInfoList.contains(eventInfo) ) {
		m_manipulatorInfoList.prepend(eventInfo);
	}
}


void CMManager::cancelCurrentManipulation()
{
	delete m_canvasManipulator;
	m_canvasManipulator = 0l;
	setRepeatedAddId();
}


void CMManager::mousePressEvent( EventInfo eventInfo )
{
	if (m_canvasManipulator)
	{
		if (m_canvasManipulator->mousePressedRepeat(eventInfo))
		{
			delete m_canvasManipulator;
			m_canvasManipulator = 0l;
		}
		return;
	}
	
	uint eventState=0;
	if (eventInfo.isRightClick)
		eventState |= CMManager::es_right_click;
	
	if (eventInfo.ctrlPressed)
		eventState |= CMManager::es_ctrl_pressed;
	
	uint itemType=0;
	uint cnItemType=0;
	
	QCanvasItem * qcanvasItem = eventInfo.qcanvasItemClickedOn;
	
	if ( ! qcanvasItem ) itemType = it_none;
	else if ( dynamic_cast<Node*>(qcanvasItem) ) itemType = it_node;
	else if ( dynamic_cast<ConnectorLine*>(qcanvasItem) || dynamic_cast<Connector*>(qcanvasItem) )
		itemType = it_connector;
	else if ( dynamic_cast<PinItem*>(qcanvasItem) ) itemType = it_pin;
	else if ( dynamic_cast<ResizeHandle*>(qcanvasItem) ) itemType = it_resize_handle;
	else if ( DrawPart *drawPartClickedOn = dynamic_cast<DrawPart*>(qcanvasItem) )
	{
		itemType = it_drawpart;
			
		if ( drawPartClickedOn->mousePressEvent(eventInfo) ) {
			p_lastItemClicked = drawPartClickedOn;
			return;
		}

		if ( drawPartClickedOn->isMovable() )
			cnItemType |= CMManager::isi_isMovable;
	} else if ( MechanicsItem *p_mechanicsItemClickedOn = dynamic_cast<MechanicsItem*>(qcanvasItem) )
	{
		itemType = it_mechanics_item;
			
		if ( p_mechanicsItemClickedOn->mousePressEvent(eventInfo) )
		{
			p_lastItemClicked = p_mechanicsItemClickedOn;
			return;
		}
	} else {
		if ( Widget *widget = dynamic_cast<Widget*>(qcanvasItem) )
			qcanvasItem = widget->parent();
		
		if ( CNItem *cnItemClickedOn = dynamic_cast<CNItem*>(qcanvasItem) )
		{
			itemType = it_canvas_item;
			
			if ( cnItemClickedOn->mousePressEvent(eventInfo) )
			{
				p_lastItemClicked = cnItemClickedOn;
				return;
			}
			
			if ( cnItemClickedOn->isMovable() )
				cnItemType |= CMManager::isi_isMovable;
		}
	}
	
// 	uint highestScore=0;
// 	ManipulatorInfo *best=0l;
	const ManipulatorInfoList::iterator end = m_manipulatorInfoList.end();
	for ( ManipulatorInfoList::iterator it = m_manipulatorInfoList.begin(); it != end && !m_canvasManipulator; ++it )
	{
		if ( (*it)->m_acceptManipulationPtr( eventState, m_cmState, itemType, cnItemType ) )
		{
			m_canvasManipulator = (*it)->m_createManipulatorPtr( p_itemDocument, this );
		}
	}

	if (m_canvasManipulator)
	{
		if (m_canvasManipulator->mousePressedInitial(eventInfo))
		{
			delete m_canvasManipulator;
			m_canvasManipulator = 0l;
		}
	}
}


void CMManager::mouseDoubleClickEvent( const EventInfo &eventInfo )
{
	if (m_canvasManipulator)
	{
		// Translate this into a repeat-click event
		if (m_canvasManipulator->mousePressedRepeat(eventInfo))
		{
			delete m_canvasManipulator;
			m_canvasManipulator = 0l;
		}
		return;
	}
	
	Item *item = dynamic_cast<Item*>(eventInfo.qcanvasItemClickedOn);
	if (item)
	{
		item->mouseDoubleClickEvent(eventInfo);
		return;
	}

	Widget *widget = dynamic_cast<Widget*>(eventInfo.qcanvasItemClickedOn);
	if (widget)
	{
		widget->parent()->mouseDoubleClickEvent(eventInfo);
		return;
	}
}


void CMManager::mouseMoveEvent( const EventInfo &eventInfo )
{
	if (m_canvasManipulator)
	{
		if (m_canvasManipulator->mouseMoved(eventInfo))
		{
			delete m_canvasManipulator;
			m_canvasManipulator = 0l;
		}
		ItemView *itemView = dynamic_cast<ItemView*>(p_itemDocument->activeView());
		if (itemView)
			itemView->scrollToMouse(eventInfo.pos);
		return;
	}
	
	//BEGIN
	QCanvasItem *qcnItem = p_itemDocument->itemAtTop(eventInfo.pos);
	Item *item;
	Widget *widget = dynamic_cast<Widget*>(qcnItem);
	if (widget) item = widget->parent();
	else item = dynamic_cast<Item*>(qcnItem);
	
	if ( p_lastMouseOverItem != (QPointer<Item>)item ) {
		QEvent event(QEvent::Leave);
		
		if (p_lastMouseOverItem)
			p_lastMouseOverItem->leaveEvent();
		
		if (item) item->enterEvent();
		
		p_lastMouseOverItem = item;
	}
	
	// If we clicked on an item, then continue to pass mouse events to that item until we release the mouse...
	if (p_lastItemClicked) {
		p_lastItemClicked->mouseMoveEvent(eventInfo);
	} else if (item) {
		item->mouseMoveEvent(eventInfo);
	}
	//END
	
	updateCurrentResizeHandle( dynamic_cast<ResizeHandle*>(qcnItem) );
}


void CMManager::updateCurrentResizeHandle( ResizeHandle * resizeHandle )
{
	if ( p_lastMouseOverResizeHandle != (QPointer<ResizeHandle>)resizeHandle )
	{
		if (p_lastMouseOverResizeHandle)
			p_lastMouseOverResizeHandle->setHover(false);
		p_lastMouseOverResizeHandle = resizeHandle;
		if (resizeHandle)
			resizeHandle->setHover(true);
	}
}


void CMManager::mouseReleaseEvent( const EventInfo &eventInfo )
{
	// If it returns true, then it has finished its editing operation
	if ( m_canvasManipulator && m_canvasManipulator->mouseReleased(eventInfo) )
	{
		delete m_canvasManipulator;
		m_canvasManipulator = 0l;
	}
	
	if (p_lastItemClicked)
	{
		p_lastItemClicked->mouseReleaseEvent(eventInfo);
		p_lastItemClicked=0l;
	}
	
	updateCurrentResizeHandle( dynamic_cast<ResizeHandle*>( p_itemDocument->itemAtTop(eventInfo.pos) ) );
}


void CMManager::wheelEvent( const EventInfo &eventInfo )
{
	bool accepted = false;
	if (b_allowItemScroll)
	{
		QCanvasItem *qcnItem = p_itemDocument->itemAtTop(eventInfo.pos);
		Item *item;
		Widget *widget = dynamic_cast<Widget*>(qcnItem);
		if (widget)
			item = widget->parent();
		else	item = dynamic_cast<Item*>(qcnItem);
		if (item) accepted = item->wheelEvent(eventInfo);
	}
	if (!accepted)
	{
		// Only allow scrolling of items if we have not just been scrolling the canvas
		b_allowItemScroll = false;
		m_allowItemScrollTmr->stop();
		m_allowItemScrollTmr->start(500,true);
		
		ItemView *itemView = dynamic_cast<ItemView*>(p_itemDocument->activeView());
		if (itemView)
		{
			itemView->cvbEditor()->setPassEventsToView(false);
			itemView->cvbEditor()->contentsWheelEvent( eventInfo.wheelEvent( 0, 0 ) );
			itemView->cvbEditor()->setPassEventsToView(true);
		}
	}
}


void CMManager::setDrawAction( int drawAction )
{
	if ( m_drawAction == drawAction )
		return;
	
	m_drawAction = drawAction;
	setCMState( cms_draw, (m_drawAction != -1) );
}


void CMManager::slotSetManualRoute( bool manualRoute )
{
	KConfigGroup grGen = KGlobal::config()->group("General");
	grGen.writeEntry( "ManualRouting", manualRoute );
	
	setCMState( cms_manual_route, manualRoute );
}


void CMManager::setCMState( CMState type, bool state )
{
	// Set or clear the correct bit
	state ? (m_cmState|=type) : (m_cmState&=(~type));
	
	if ( type == CMManager::cms_manual_route )
		emit manualRoutingChanged(state);
}


void CMManager::setRepeatedAddId( const QString & repeatedId )
{
	m_repeatedItemId = repeatedId;
}
//END class CMManager



//BEGIN class CanvasManipulator
CanvasManipulator::CanvasManipulator( ItemDocument *itemDocument, CMManager *cmManager )
{
	p_itemDocument = itemDocument;
	p_icnDocument = dynamic_cast<ICNDocument*>(itemDocument);
	p_mechanicsDocument = dynamic_cast<MechanicsDocument*>(itemDocument);
	p_canvas = p_itemDocument->canvas();
// 	b_connectorsAllowedRouting = true;
	p_selectList = p_itemDocument->selectList();
	p_cnItemSelectList = dynamic_cast<CNItemGroup*>(p_selectList);
	p_mechItemSelectList = dynamic_cast<MechanicsGroup*>(p_selectList);
	p_cnItemClickedOn = 0l;
	p_cmManager = cmManager;
	
	connect( itemDocument->canvas(), SIGNAL(resized( const QRect&, const QRect& )), this, SLOT(canvasResized( const QRect&, const QRect& )) );
}


CanvasManipulator::~CanvasManipulator()
{
}


QPoint CanvasManipulator::snapPoint( QPoint point )
{
	point /= 8;
	point *= 8;
	point += QPoint( 4, 4 );
	return point;
}
//END class CanvasManipulator


CMRepeatedItemAdd::CMRepeatedItemAdd( ItemDocument *itemDocument, CMManager *cmManager )
	: CanvasManipulator( itemDocument, cmManager )
{
}

CMRepeatedItemAdd::~CMRepeatedItemAdd()
{
}

CanvasManipulator* CMRepeatedItemAdd::construct( ItemDocument *itemDocument, CMManager *cmManager )
{
	return new CMRepeatedItemAdd(itemDocument,cmManager);
}

ManipulatorInfo *CMRepeatedItemAdd::manipulatorInfo()
{
	ManipulatorInfo *eventInfo = new ManipulatorInfo();
	eventInfo->m_acceptManipulationPtr = CMRepeatedItemAdd::acceptManipulation;
	eventInfo->m_createManipulatorPtr = CMRepeatedItemAdd::construct;
	return eventInfo;
}

bool CMRepeatedItemAdd::acceptManipulation( uint /*eventState*/, uint cmState, uint /*itemType*/, uint /*cnItemType*/ )
{
	return (cmState & CMManager::cms_repeated_add);
}

bool CMRepeatedItemAdd::mousePressedRepeat( const EventInfo &eventInfo )
{
	return mousePressedInitial(eventInfo);
}

bool CMRepeatedItemAdd::mousePressedInitial( const EventInfo &eventInfo )
{
	m_eventInfo = eventInfo;
	if (eventInfo.isRightClick)
	{
		p_cmManager->setCMState( CMManager::cms_repeated_add, false );
		return true;
	}
	
	p_icnDocument->addItem( p_cmManager->repeatedItemId(), eventInfo.pos, true );
	p_itemDocument->requestStateSave();
	return false;
}


bool CMRepeatedItemAdd::mouseMoved( const EventInfo &/*eventInfo*/ )
{
	return false;
}


bool CMRepeatedItemAdd::mouseReleased( const EventInfo &/*eventInfo*/ )
{
	return false;
}


CMRightClick::CMRightClick( ItemDocument *itemDocument, CMManager *cmManager )
	: CanvasManipulator( itemDocument, cmManager )
{
}

CMRightClick::~CMRightClick()
{
}

CanvasManipulator* CMRightClick::construct( ItemDocument *itemDocument, CMManager *cmManager )
{
	return new CMRightClick(itemDocument,cmManager);
}

ManipulatorInfo *CMRightClick::manipulatorInfo()
{
	ManipulatorInfo *eventInfo = new ManipulatorInfo();
// 	eventInfo->m_eventState.m_activate = CMManager::es_right_click;
	eventInfo->m_acceptManipulationPtr = CMRightClick::acceptManipulation;
	eventInfo->m_createManipulatorPtr = CMRightClick::construct;
	return eventInfo;
}

bool CMRightClick::acceptManipulation( uint eventState, uint /*cmState*/, uint /*itemType*/, uint /*cnItemType*/ )
{
	return eventState & CMManager::es_right_click;
}


bool CMRightClick::mousePressedInitial( const EventInfo &eventInfo )
{
	m_eventInfo = eventInfo;
	p_itemDocument->canvasRightClick( eventInfo.globalPos, eventInfo.qcanvasItemClickedOn );
	return true;
}


bool CMRightClick::mouseMoved( const EventInfo &/*eventInfo*/ )
{
	return true;
}


bool CMRightClick::mouseReleased( const EventInfo &/*eventInfo*/ )
{
	return true;
}



//BEGIN class ConnectorDraw
ConnectorDraw::ConnectorDraw( ItemDocument *itemDocument, CMManager *cmManager )
	: CanvasManipulator( itemDocument, cmManager )
{
	p_startNode = 0l;
	p_startConnector = 0l;
	p_endNode = 0l;
	p_endConnector = 0l;
}


ConnectorDraw::~ConnectorDraw()
{
}


QColor ConnectorDraw::validConnectionColor()
{
	return QColor( 255, 166, 0 );
}


QPoint ConnectorDraw::toValidPos( const QPoint & clickPos, Connector * clickedConnector ) const
{
	if ( !clickedConnector )
		return clickPos;
	
	const QPointList pointList = clickedConnector->connectorPoints();
	
	QPointList::const_iterator end = pointList.end();
	
	double dl[] = { 0.5, 8.5, 11.5, 18.0, 23.0 }; // various distances rounded up of (0,0) cells, (0,1), etc
	for ( unsigned i = 0; i < 5; ++i )
	{
		for ( QPointList::const_iterator it = pointList.begin(); it != end; ++it )
		{
			if ( qpoint_distance( *it, clickPos ) <= dl[i] )
				return *it;
		}
	}
	
	return clickPos;
}


Connector * ConnectorDraw::toConnector( Node * node )
{
	if ( !node || node->numCon( true, false ) < 3 )
		return 0l;
	
	return node->getAConnector();
}


void ConnectorDraw::grabEndStuff( QCanvasItem * endItem, const QPoint & pos, bool posIsExact )
{
	if (!endItem)
		return;
	
	CNItem * cnItem = dynamic_cast<CNItem*>(endItem);
	if ( cnItem && !posIsExact )
		p_endNode = cnItem->getClosestNode(pos);
	else
		p_endNode = dynamic_cast<Node*>(endItem);
	
	if ( p_endNode && p_endNode->numCon( true, false ) > 2 )
	{
		p_endConnector = toConnector(p_endNode);
		p_endNode = 0l;
	}
	
	// If the endItem is a node, we have to finish exactly on the end when posIsExact is true
	if ( posIsExact && p_endNode && (p_endNode->x() != pos.x() || p_endNode->y() != pos.y()) )
		p_endNode = 0l;
	
	if (!p_endConnector)
		p_endConnector = dynamic_cast<Connector*>(endItem);
}
//END class ConnectorDraw


//BEGIN class CMAutoConnector
CMAutoConnector::CMAutoConnector( ItemDocument *itemDocument, CMManager *cmManager )
	: ConnectorDraw( itemDocument, cmManager )
{
	m_connectorLine = 0l;
	p_startNode = 0l;
	p_startConnector = 0l;
}

CMAutoConnector::~CMAutoConnector()
{
	delete m_connectorLine;

}

CanvasManipulator* CMAutoConnector::construct( ItemDocument *itemDocument, CMManager *cmManager )
{
	return new CMAutoConnector(itemDocument,cmManager);
}

ManipulatorInfo *CMAutoConnector::manipulatorInfo()
{
	ManipulatorInfo *eventInfo = new ManipulatorInfo();
	eventInfo->m_acceptManipulationPtr = CMAutoConnector::acceptManipulation;
	eventInfo->m_createManipulatorPtr = CMAutoConnector::construct;
	return eventInfo;
}

bool CMAutoConnector::acceptManipulation( uint /*eventState*/, uint cmState, uint itemType, uint /*cnItemType*/ )
{
	return (itemType & (CMManager::it_node | CMManager::it_connector)) && !(cmState & CMManager::cms_manual_route);
}

bool CMAutoConnector::mousePressedInitial( const EventInfo &eventInfo )
{
	m_eventInfo = eventInfo;
	
	p_startNode = dynamic_cast<Node*>(eventInfo.qcanvasItemClickedOn);
	
	if (p_startNode)
	{
		m_eventInfo.pos = m_prevPos = p_icnDocument->gridSnap( QPoint( (int)p_startNode->x(), (int)p_startNode->y() ) );
		if (p_startNode->numCon( true, false ) > 2)
		{
			p_startConnector = toConnector(p_startNode);
			p_startNode = 0l;
		}
	} else if (p_startConnector = dynamic_cast<Connector*>(eventInfo.qcanvasItemClickedOn) )
	{
// 		startConnectorPoint = m_eventInfo.pos = m_prevPos = p_icnDocument->gridSnap(m_eventInfo.pos);
		startConnectorPoint = m_eventInfo.pos = m_prevPos = toValidPos( m_eventInfo.pos, p_startConnector );
	} else return true;
		
	p_icnDocument->unselectAll();
	
	delete m_connectorLine;
	m_connectorLine = new QCanvasLine(p_canvas);
	m_connectorLine->setPen( QColor(0,0,0) );
	m_connectorLine->setZ( ItemDocument::Z::ConnectorCreateLine );
	m_connectorLine->show();
	return false;
}


bool CMAutoConnector::mouseMoved( const EventInfo &eventInfo )
{
	const QPoint pos = eventInfo.pos;
	
	int newX = p_icnDocument->gridSnap( pos.x() );
	int newY = p_icnDocument->gridSnap( pos.y() );

	bool movedFlag = false;

	if ( newX != m_prevPos.x() )
	{
		m_prevPos.setX(newX);
		movedFlag = true;
	}

	if ( newY != m_prevPos.y() )
	{
		m_prevPos.setY(newY);
		movedFlag = true;
	}

	m_connectorLine->setPoints( m_eventInfo.pos.x(), m_eventInfo.pos.y(), newX, newY );
		
	if (movedFlag) {
		QCanvasItem *startItem = 0l;
		if (p_startNode)
			startItem = p_startNode;
		else if (p_startConnector)
			startItem = p_startConnector;
		
		QCanvasItem *endItem = p_icnDocument->itemAtTop( QPoint( newX, newY ) );
		if ( CNItem * cni = dynamic_cast<CNItem*>(endItem) )
			endItem = cni->getClosestNode( QPoint( newX, newY ) );
		
		bool validLine = p_icnDocument->canConnect( startItem, endItem );
		m_connectorLine->setPen( validLine ? validConnectionColor() : Qt::black );
	}
	return false;
}


bool CMAutoConnector::mouseReleased( const EventInfo &eventInfo )
{
	const QPoint pos = eventInfo.pos;
	
	QPoint end = m_connectorLine->endPoint();
	delete m_connectorLine;
	m_connectorLine = 0l;
	
	QCanvasItem *qcanvasItem = p_icnDocument->itemAtTop(end);
	if ( !qcanvasItem )
		return true;
	
	grabEndStuff( qcanvasItem, pos, false );
	
	if (p_startConnector)
	{
		if (p_endConnector) {
			if ( !p_icnDocument->createConnector( p_endConnector, p_startConnector, p_icnDocument->gridSnap(pos), startConnectorPoint ) )
				return true;
		} else if (p_endNode) {
			if ( !p_icnDocument->createConnector( p_endNode, p_startConnector, startConnectorPoint ) )
				return true;
		} else	return true;
	} else if (p_startNode) {
		if (p_endConnector)
		{
			if ( !p_icnDocument->createConnector( p_startNode, p_endConnector, p_icnDocument->gridSnap(pos) ) )
				return true;
		} else if (p_endNode) {
			if ( !p_icnDocument->createConnector( p_startNode, p_endNode ) )
				return true;
		} else	return true;
	} else return true;
	
	p_itemDocument->requestStateSave();
	return true;
}
//END class CMAutoConnector



//BEGIN class CMManualConnector
CMManualConnector::CMManualConnector( ItemDocument *itemDocument, CMManager *cmManager )
	: ConnectorDraw( itemDocument, cmManager )
{
	m_manualConnectorDraw = 0l;
}

CMManualConnector::~CMManualConnector()
{
	delete m_manualConnectorDraw;
}

CanvasManipulator* CMManualConnector::construct( ItemDocument *itemDocument, CMManager *cmManager )
{
	return new CMManualConnector(itemDocument,cmManager);
}

ManipulatorInfo *CMManualConnector::manipulatorInfo()
{
	ManipulatorInfo *eventInfo = new ManipulatorInfo();
	eventInfo->m_acceptManipulationPtr = CMManualConnector::acceptManipulation;
	eventInfo->m_createManipulatorPtr = CMManualConnector::construct;
	return eventInfo;
}

bool CMManualConnector::acceptManipulation( uint /*eventState*/, uint cmState, uint itemType, uint /*cnItemType*/ )
{
	return (itemType & (CMManager::it_node | CMManager::it_connector)) && (cmState & CMManager::cms_manual_route);
}


bool CMManualConnector::mousePressedInitial( const EventInfo &eventInfo )
{
	if ( eventInfo.isRightClick ) return true;

	m_eventInfo = eventInfo;
	
	p_icnDocument->unselectAll();
	
	QPoint sp;
	
	if ( p_startNode = dynamic_cast<Node*>(eventInfo.qcanvasItemClickedOn) )
	{
		sp.setX( (int)p_startNode->x() );
		sp.setY( (int)p_startNode->y() );
		if ( p_startNode->numCon( true, false ) > 2 )
		{
			p_startConnector = toConnector(p_startNode);
			p_startNode = 0l;
		}
	}
	else
	{
		p_startConnector = dynamic_cast<Connector*>(eventInfo.qcanvasItemClickedOn);
		sp = toValidPos( eventInfo.pos, p_startConnector );
	}
	startConnectorPoint = sp;
	
	if (m_manualConnectorDraw)
		delete m_manualConnectorDraw;
	m_manualConnectorDraw = new ManualConnectorDraw( p_icnDocument, sp );
	return false;
}


bool CMManualConnector::mousePressedRepeat( const EventInfo &eventInfo )
{
	m_eventInfo = eventInfo;
	if ( eventInfo.isRightClick ) {
		return true;
	}
	m_manualConnectorDraw->mouseClicked( p_icnDocument->gridSnap(m_eventInfo.pos) );
	return false;
}


bool CMManualConnector::mouseMoved( const EventInfo &eventInfo )
{
	if ( !m_manualConnectorDraw )
		return true;
	
	const QPoint pos = eventInfo.pos;
	
    int newX = p_icnDocument->gridSnap( pos.x() );
	int newY = p_icnDocument->gridSnap( pos.y() );

	bool movedFlag = false;

	if ( newX != m_prevPos.x() )
	{
		m_prevPos.setX(newX);
		movedFlag = true;
	}

	if ( newY != m_prevPos.y() )
	{
		m_prevPos.setY(newY);
		movedFlag = true;
	}
	
	if ( movedFlag )
	{
		QCanvasItem *startItem = 0l;
		if (p_startNode)
			startItem = p_startNode;
		else if (p_startConnector)
			startItem = p_startConnector;
		
		QCanvasItem * endItem = p_icnDocument->itemAtTop( QPoint( newX, newY ) );
		
		// If the endItem is a node, we have to finish exactly on the end.
		if ( Node * node = dynamic_cast<Node*>(endItem) )
		{
			if ( node->x() != newX || node->y() != newY )
				endItem = 0l;
		}
		
		bool validLine = p_icnDocument->canConnect( startItem, endItem );
		
		m_manualConnectorDraw->setColor( validLine ? validConnectionColor() : Qt::black );
		m_manualConnectorDraw->mouseMoved( QPoint( newX, newY ) );
	}
	
	return false;
}


bool CMManualConnector::mouseReleased( const EventInfo &eventInfo )
{
	if (!m_manualConnectorDraw) return true;

	QPoint pos = p_icnDocument->gridSnap(eventInfo.pos);
	
	grabEndStuff( m_manualConnectorDraw->mouseClicked(pos), pos, true );
	
	if ( !p_endNode && !p_endConnector )
		return false;
	
	// Create the points that define the manual route
	QPointList list = m_manualConnectorDraw->pointList();
	delete m_manualConnectorDraw;
	m_manualConnectorDraw = 0l;
	
	if (p_startConnector)
	{
		if (p_endConnector)
		{
			if ( !p_icnDocument->createConnector( p_endConnector, p_startConnector, p_icnDocument->gridSnap(pos), startConnectorPoint, &list ) )
				return true;
		} else // if (p_endNode)
		{
			if ( !p_icnDocument->createConnector( p_endNode, p_startConnector, startConnectorPoint, &list ) )
				return true;
		}
	} else if (p_startNode) {
		if (p_endConnector)
		{
			if ( !p_icnDocument->createConnector( p_startNode, p_endConnector, p_icnDocument->gridSnap(pos), &list ) )
				return true;
		} else // if (p_endNode)
		{
			if ( !p_icnDocument->createConnector( p_startNode, p_endNode, &list ) )
				return true;
		}
	} else return true;
	
	p_itemDocument->requestStateSave();
	return true;
}
//END class CMManualConnector


//BEGIN class CMItemMove
CMItemMove::CMItemMove( ItemDocument *itemDocument, CMManager *cmManager )
	: CanvasManipulator( itemDocument, cmManager )
{
	p_flowContainerCandidate = 0l;
	m_bItemsSnapToGrid = false;
}

CMItemMove::~CMItemMove()
{
}

CanvasManipulator* CMItemMove::construct( ItemDocument *itemDocument, CMManager *cmManager )
{
	return new CMItemMove(itemDocument,cmManager);
}

ManipulatorInfo *CMItemMove::manipulatorInfo()
{
	ManipulatorInfo *eventInfo = new ManipulatorInfo();
	eventInfo->m_acceptManipulationPtr = CMItemMove::acceptManipulation;
	eventInfo->m_createManipulatorPtr = CMItemMove::construct;
	return eventInfo;
}

bool CMItemMove::acceptManipulation( uint eventState, uint /*cmState*/, uint itemType, uint cnItemType )
{
	return ((itemType & CMManager::it_canvas_item) || (itemType & CMManager::it_drawpart)) && (cnItemType & CMManager::isi_isMovable) && !(eventState & CMManager::es_right_click);
}


bool CMItemMove::mousePressedInitial( const EventInfo &eventInfo )
{
	m_eventInfo = eventInfo;
	m_prevPos = eventInfo.pos;
	
	Item *item = dynamic_cast<Item*>(eventInfo.qcanvasItemClickedOn);

	if (!item) return true;
	
	if ( !p_selectList->contains(item) )
	{
		if (!eventInfo.ctrlPressed)
			p_itemDocument->unselectAll();
		
		p_itemDocument->select(item);
	} else if (m_eventInfo.ctrlPressed)
		p_itemDocument->unselect(item);
	
	if ( p_selectList->isEmpty() )
		return true;
	
	// We want to allow dragging into FlowContainers if this is a FlowView
	p_flowContainerCandidate = 0l;
	{
		const ItemList &itemList = p_icnDocument->itemList();
		const ItemList::const_iterator ciEnd = itemList.end();
		for ( ItemList::const_iterator it = itemList.begin(); it != ciEnd; ++it )
		{
			if ( FlowContainer *flowContainer = dynamic_cast<FlowContainer*>((Item*)*it) )
				flowContainer->setFullBounds(true);
		}
	}
	
	ItemList itemList = p_cnItemSelectList->items(false);
	itemList.remove((Item*)0l);
	
	m_bItemsSnapToGrid = false;
	const ItemList::iterator itemListEnd = itemList.end();
	for ( ItemList::iterator it = itemList.begin(); it != itemListEnd; ++it )
	{
		CNItem *cnItem = dynamic_cast<CNItem*>((Item*)*it);
		if ( !cnItem || !cnItem->canvas() )
			continue;
			
		m_bItemsSnapToGrid = true;
	}
	
	if ( m_bItemsSnapToGrid )
		m_prevSnapPoint = this->snapPoint( m_prevPos );
	else	m_prevSnapPoint = m_prevPos;
	
	ConnectorList fixedConnectors;
	p_icnDocument->getTranslatable( itemList, &fixedConnectors, &m_translatableConnectors, &m_translatableNodeGroups );
	
	const ConnectorList::iterator fixedConnectorsEnd = fixedConnectors.end();
	for ( ConnectorList::iterator it = fixedConnectors.begin(); it != fixedConnectorsEnd; ++it )
		(*it)->setSemiHidden(true);
	
	p_flowContainerCandidate = p_icnDocument->flowContainer(eventInfo.pos);

	return false;
}


void CMItemMove::canvasResized( const QRect & oldSize, const QRect & newSize )
{
	QPoint delta = oldSize.topLeft() - newSize.topLeft();
	
// 	scrollCanvasToSelection();
	
// 	QCursor::setPos( QCursor::pos() + delta );
// 	m_prevPos += delta;
// 	m_prevSnapPoint += delta;
}


void CMItemMove::scrollCanvasToSelection()
{
	QRect bound;
	ItemList itemList = p_cnItemSelectList->items(false);
	itemList.remove((Item*)0l);
	const ItemList::iterator itemListEnd = itemList.end();
	for ( ItemList::iterator it = itemList.begin(); it != itemListEnd; ++it )
		bound |= (*it)->boundingRect();
	
	QPoint scrollToPos = m_prevPos;
	if ( m_dx < 0 ) {
		// Scrolling left
		scrollToPos -= QPoint( bound.left(), 0 );
	} else {
		// Scrolling right
		scrollToPos += QPoint( bound.right(), 0 );
	}

	if ( m_dy < 0 ) {
		// Scrolling up
		scrollToPos -= QPoint( 0, bound.top() );
	} else {
		// Scrolling right
		scrollToPos += QPoint( 0, bound.bottom() );
	}

	ItemView *itemView = dynamic_cast<ItemView*>(p_itemDocument->activeView());
	if (itemView)
		itemView->scrollToMouse( scrollToPos );
}


bool CMItemMove::mouseMoved( const EventInfo &eventInfo )
{
	QPoint pos = eventInfo.pos;
	
	QPoint snapPoint = pos;
	if ( m_bItemsSnapToGrid )
		snapPoint = this->snapPoint( snapPoint );
	
	int dx = snapPoint.x() - m_prevSnapPoint.x();
	int dy = snapPoint.y() - m_prevSnapPoint.y();
	
	m_dx = dx;
	m_dy = dy;
	
   	const ItemList itemList = p_cnItemSelectList->items();
	const ItemList::const_iterator end = itemList.end();
	
	for ( ItemList::const_iterator it = itemList.begin(); it != end; ++it )
	{
		if ( !*it || !(*it)->isMovable() )
			continue;
		
		QRect oldRect = (*it)->boundingRect();
		(*it)->moveBy( dx, dy );
		QRect newRect = (*it)->boundingRect();
		QRect merged = oldRect | newRect;
	}
	
	if ( (dx != 0) || (dy != 0) )
	{
		const ConnectorList::iterator frEnd = m_translatableConnectors.end();
		for ( ConnectorList::iterator it = m_translatableConnectors.begin(); it != frEnd; ++it )
			(*it)->translateRoute( dx, dy );
		
		const NodeGroupList::iterator end = m_translatableNodeGroups.end();
		for ( NodeGroupList::iterator it = m_translatableNodeGroups.begin(); it != end; ++it )
			(*it)->translate( dx, dy );
	}
	
	FlowContainer *fc = p_icnDocument->flowContainer(pos);
	if ( fc != p_flowContainerCandidate )
	{
		if ( p_flowContainerCandidate )
		{
			p_flowContainerCandidate->setSelected(false);
			p_flowContainerCandidate = 0l;
		}
	}

	if (fc)
	{
		p_flowContainerCandidate = fc;
		p_flowContainerCandidate->setSelected(true);
	}
	
	p_itemDocument->requestEvent( ItemDocument::ItemDocumentEvent::ResizeCanvasToItems );
	p_canvas->update();
	m_prevPos = pos;
	m_prevSnapPoint = snapPoint;
	
// 	scrollCanvasToSelection();
	
	return false;
}


bool CMItemMove::mouseReleased( const EventInfo &eventInfo )
{
	// Is the release event from a right click (which rotates items)?
	if ( eventInfo.isRightClick || eventInfo.isMiddleClick )
		return false;
	
	QStringList itemIDs;
	
	const ItemList itemList = p_cnItemSelectList->items();
	const ItemList::const_iterator ilEnd = itemList.end();
	for ( ItemList::const_iterator it = itemList.begin(); it != ilEnd; ++it )
	{
		if (*it) itemIDs.append( (*it)->id() );
	}
	
	const QPoint pos = eventInfo.pos;
	
	// And make sure all connectors are properly shown
	const ConnectorList &connectorList = p_icnDocument->connectorList();
	const ConnectorList::const_iterator conEnd = connectorList.end();
	for ( ConnectorList::const_iterator it = connectorList.begin(); it != conEnd; ++it )
	{
		(*it)->setSemiHidden(false);
	}
	
	if (p_flowContainerCandidate)
	{
		for ( ItemList::const_iterator it = itemList.begin(); it != ilEnd; ++it )
			p_flowContainerCandidate->addChild(*it);
		
		p_flowContainerCandidate->setSelected(false);
		p_flowContainerCandidate = 0l;
	} else {
		for ( ItemList::const_iterator it = itemList.begin(); it != ilEnd; ++it )
			(*it)->setParentItem(0l);
	}
	
	// And disable the FlowContainers again...
	const ItemList &cnItemList = p_icnDocument->itemList();
	const ItemList::const_iterator end = cnItemList.end();
	for ( ItemList::const_iterator it = cnItemList.begin(); it != end; ++it )
	{
		if ( FlowContainer *flowContainer = dynamic_cast<FlowContainer*>((Item*)*it) )
			flowContainer->setFullBounds(false);
	}
	
	if (p_icnDocument) p_icnDocument->requestRerouteInvalidatedConnectors();
	
	if ( m_eventInfo.pos != eventInfo.pos ) p_itemDocument->requestStateSave();
	
	p_itemDocument->requestEvent( ItemDocument::ItemDocumentEvent::ResizeCanvasToItems );
	
	return true;
}


bool CMItemMove::mousePressedRepeat( const EventInfo & info )
{
	if ( info.isRightClick )
		p_cnItemSelectList->slotRotateCW();
	else if ( info.isMiddleClick )
		p_cnItemSelectList->flipHorizontally();
	
	return false;
}
//END class CMItemMove


CMItemResize::CMItemResize( ItemDocument *itemDocument, CMManager *cmManager )
	: CanvasManipulator( itemDocument, cmManager )
{
}

CMItemResize::~CMItemResize()
{
}

CanvasManipulator* CMItemResize::construct( ItemDocument *itemDocument, CMManager *cmManager )
{
	return new CMItemResize(itemDocument,cmManager);
}

ManipulatorInfo *CMItemResize::manipulatorInfo()
{
	ManipulatorInfo *eventInfo = new ManipulatorInfo();
// 	eventInfo->m_itemType.m_activate = CMManager::it_canvas_item;
	eventInfo->m_acceptManipulationPtr = CMItemResize::acceptManipulation;
	eventInfo->m_createManipulatorPtr = CMItemResize::construct;
	return eventInfo;
}

bool CMItemResize::acceptManipulation( uint eventState, uint /*cmState*/, uint itemType, uint /*cnItemType*/ )
{
	return (itemType & CMManager::it_resize_handle) && !(eventState & CMManager::es_right_click);
}


bool CMItemResize::mousePressedInitial( const EventInfo &eventInfo )
{
	m_eventInfo = eventInfo;
	p_resizeHandle = dynamic_cast<ResizeHandle*>(eventInfo.qcanvasItemClickedOn);
	m_rh_dx = p_resizeHandle->x()-eventInfo.pos.x();
	m_rh_dy = p_resizeHandle->y()-eventInfo.pos.y();
	return false;
}


bool CMItemResize::mouseMoved( const EventInfo &eventInfo )
{
	int _x = int(m_rh_dx + eventInfo.pos.x());
	int _y = int(m_rh_dy + eventInfo.pos.y());
	
	// Shift pressed == snap to grid
	if ( eventInfo.shiftPressed )
	{
		_x = snapToCanvas(_x);
		_y = snapToCanvas(_y);
	}
	
	p_resizeHandle->moveRH( _x, _y );
	return false;
}


bool CMItemResize::mouseReleased( const EventInfo &/*eventInfo*/ )
{
	if (p_icnDocument)
		p_icnDocument->requestRerouteInvalidatedConnectors();
	p_itemDocument->requestStateSave();
	return true;
}


CMMechItemMove::CMMechItemMove( ItemDocument *itemDocument, CMManager *cmManager )
	: CanvasManipulator( itemDocument, cmManager )
{
}

CMMechItemMove::~CMMechItemMove()
{
}

CanvasManipulator* CMMechItemMove::construct( ItemDocument *itemDocument, CMManager *cmManager )
{
	return new CMMechItemMove(itemDocument,cmManager);
}

ManipulatorInfo *CMMechItemMove::manipulatorInfo()
{
	ManipulatorInfo *eventInfo = new ManipulatorInfo();
// 	eventInfo->m_itemType.m_activate = CMManager::it_canvas_item;
	eventInfo->m_acceptManipulationPtr = CMMechItemMove::acceptManipulation;
	eventInfo->m_createManipulatorPtr = CMMechItemMove::construct;
	return eventInfo;
}

bool CMMechItemMove::acceptManipulation( uint eventState, uint /*cmState*/, uint itemType, uint /*cnItemType*/ )
{
	return ((itemType & CMManager::it_mechanics_item) || (itemType & CMManager::it_drawpart)) && !(eventState & CMManager::es_right_click);
}


bool CMMechItemMove::mousePressedInitial( const EventInfo &eventInfo )
{
	m_eventInfo = eventInfo;
	m_prevPos = eventInfo.pos;
	
	Item *item = dynamic_cast<Item*>(eventInfo.qcanvasItemClickedOn);
	if (!item)
		return true;
	
	MechanicsItem *mechItem = dynamic_cast<MechanicsItem*>(eventInfo.qcanvasItemClickedOn);
	
	if (mechItem) m_prevClickedOnSM = mechItem->selectionMode();
	
	if (eventInfo.shiftPressed)
	{
		p_mechanicsDocument->unselectAll();
		p_mechanicsDocument->select(item);
		if (mechItem)
		{
			mechItem->setSelectionMode(MechanicsItem::sm_move);
			mechItem->setParentItem(0l);
		}
	} else if ( !p_selectList->contains(mechItem) )
	{
		if (!eventInfo.ctrlPressed)
			p_mechanicsDocument->unselectAll();
		
		p_mechanicsDocument->select(item);
		
		if (mechItem)
			mechItem->setSelectionMode(MechanicsItem::sm_move);
	} else {
		if (mechItem)
			mechItem->setSelectionMode(MechanicsItem::sm_move);
		
		if (m_eventInfo.ctrlPressed)
			p_mechanicsDocument->unselect(item);
	}
	
	if ( p_selectList->isEmpty() )
		return true;
	
	p_mechItemSelectList->setSelectionMode( MechanicsItem::sm_move );
	p_mechItemSelectList->setRaised(true);
	return false;
}


bool CMMechItemMove::mouseMoved( const EventInfo &eventInfo )
{
	const QPoint pos = eventInfo.pos;
	
	int x = pos.x();
	int y = pos.y();
	
	const MechItemList itemList = p_mechItemSelectList->toplevelMechItemList();
	const MechItemList::const_iterator ilEnd = itemList.end();
	for ( MechItemList::const_iterator it = itemList.begin(); it != ilEnd; ++it )
	{
		if (*it)
			(*it)->moveBy( x - m_prevPos.x(), y - m_prevPos.y() );
	}
	
	m_prevPos = QPoint( x, y );
	
	p_canvas->update();
	p_itemDocument->requestEvent( ItemDocument::ItemDocumentEvent::ResizeCanvasToItems );
	return false;
}


bool CMMechItemMove::mouseReleased( const EventInfo &eventInfo )
{
	const QPoint pos = eventInfo.pos;
			
	int dx = pos.x() - m_eventInfo.pos.x();
	int dy = pos.y() - m_eventInfo.pos.y();
	
	p_mechItemSelectList->setRaised(false);
	
	MechanicsItem *mechItem = dynamic_cast<MechanicsItem*>(m_eventInfo.qcanvasItemClickedOn);
	if ( dx == 0 && dy == 0 )
	{
		if ( mechItem && mechItem->isSelected() )
		{
			if ( m_prevClickedOnSM == MechanicsItem::sm_resize )
				mechItem->setSelectionMode( MechanicsItem::sm_rotate );
			else
				mechItem->setSelectionMode( MechanicsItem::sm_resize );
		}
		p_itemDocument->requestStateSave();
		return true;
	}
	
	if ( mechItem && mechItem->isSelected() )
	{
		if ( m_prevClickedOnSM == MechanicsItem::sm_rotate )
			mechItem->setSelectionMode(MechanicsItem::sm_rotate);
		else
			mechItem->setSelectionMode(MechanicsItem::sm_resize);
	}
	
	QStringList itemIDs;
	
	ItemList itemList = p_mechItemSelectList->items();
	const ItemList::iterator ilEnd = itemList.end();
	for ( ItemList::iterator it = itemList.begin(); it != ilEnd; ++it )
	{
		if (*it) {
			itemIDs.append( (*it)->id() );
		}
	}
	
	p_mechItemSelectList->setSelectionMode( MechanicsItem::sm_resize );
	p_itemDocument->requestStateSave();
	p_itemDocument->requestEvent( ItemDocument::ItemDocumentEvent::ResizeCanvasToItems );
	return true;
}



//BEGIN class SelectRectangle
SelectRectangle::SelectRectangle( int x, int y, int w, int h, QCanvas *qcanvas )
	: m_x(x), m_y(y)
{
	m_topLine = new QCanvasLine(qcanvas);
	m_rightLine = new QCanvasLine(qcanvas);
	m_bottomLine = new QCanvasLine(qcanvas);
	m_leftLine = new QCanvasLine(qcanvas);
	setSize( w, h );
	
	QCanvasLine* lines[] = { m_topLine, m_rightLine, m_bottomLine, m_leftLine };
	for ( int i=0; i<4; ++ i)
	{
		lines[i]->setPen( QPen( QColor(190,190,190), 1, Qt::DotLine ) );
		lines[i]->setZ( ICNDocument::Z::Select );
		lines[i]->show();
	}
}


SelectRectangle::~SelectRectangle()
{
	delete m_topLine;
	delete m_rightLine;
	delete m_bottomLine;
	delete m_leftLine;
}


void SelectRectangle::setSize( int w, int h )
{
	m_topLine->setPoints( m_x, m_y, m_x+w, m_y );
	m_rightLine->setPoints( m_x+w, m_y, m_x+w, m_y+h );
	m_bottomLine->setPoints( m_x+w, m_y+h, m_x, m_y+h );
	m_leftLine->setPoints( m_x, m_y+h, m_x, m_y );
	m_w = w;
	m_h = h;
}


QCanvasItemList SelectRectangle::collisions()
{
	QCanvas *canvas = m_topLine->canvas();
	
	return canvas->collisions( QRect( m_x, m_y, m_w, m_h ) );
}
//END class SelectRectangle


//BEGIN class CMSelect
CMSelect::CMSelect( ItemDocument *itemDocument, CMManager *cmManager )
	: CanvasManipulator( itemDocument, cmManager )
{
	m_selectRectangle = 0l;
}

CMSelect::~CMSelect()
{
	delete m_selectRectangle;
}

CanvasManipulator* CMSelect::construct( ItemDocument *itemDocument, CMManager *cmManager )
{
	return new CMSelect(itemDocument,cmManager);
}

ManipulatorInfo *CMSelect::manipulatorInfo()
{
	ManipulatorInfo *eventInfo = new ManipulatorInfo();
// 	eventInfo->m_itemType.m_activate = CMManager::it_none;
	eventInfo->m_acceptManipulationPtr = CMSelect::acceptManipulation;
	eventInfo->m_createManipulatorPtr = CMSelect::construct;
	return eventInfo;
}

bool CMSelect::acceptManipulation( uint /*eventState*/, uint /*cmState*/, uint itemType, uint /*cnItemType*/ )
{
	return (itemType & CMManager::it_none);
}


bool CMSelect::mousePressedInitial( const EventInfo &eventInfo )
{
	m_eventInfo = eventInfo;
	
    if (!eventInfo.ctrlPressed) {
		p_itemDocument->unselectAll();
	}
	
	m_selectRectangle = new SelectRectangle( eventInfo.pos.x(), eventInfo.pos.y(), 0, 0, p_canvas );
	return false;
}


bool CMSelect::mouseMoved( const EventInfo &eventInfo )
{
	QPoint pos = eventInfo.pos;
	
    m_selectRectangle->setSize( pos.x()-m_eventInfo.pos.x(), pos.y()-m_eventInfo.pos.y() );
	
	if (m_eventInfo.ctrlPressed) {
		p_itemDocument->select( m_selectRectangle->collisions() );
	} else if (p_selectList) {
		p_selectList->setItems( m_selectRectangle->collisions() );
	}

	if (p_selectList && !p_mechanicsDocument) {
		p_selectList->setSelected(true);
	}
	return false;
}


bool CMSelect::mouseReleased( const EventInfo &/*eventInfo*/ )
{
	delete m_selectRectangle;
	m_selectRectangle = 0l;
	
	return true;
}
//END class CMSelect


CMItemDrag::CMItemDrag( ItemDocument *itemDocument, CMManager *cmManager )
	: CanvasManipulator( itemDocument, cmManager )
{
	b_dragged = false;
}

CMItemDrag::~CMItemDrag()
{
}

CanvasManipulator* CMItemDrag::construct( ItemDocument *itemDocument, CMManager *cmManager )
{
	return new CMItemDrag(itemDocument,cmManager);
}

ManipulatorInfo *CMItemDrag::manipulatorInfo()
{
	ManipulatorInfo *eventInfo = new ManipulatorInfo();
// 	eventInfo->m_itemType.m_activate = CMManager::it_canvas_item;
	eventInfo->m_acceptManipulationPtr = CMItemDrag::acceptManipulation;
	eventInfo->m_createManipulatorPtr = CMItemDrag::construct;
	return eventInfo;
}

bool CMItemDrag::acceptManipulation( uint /*eventState*/, uint /*cmState*/, uint itemType, uint cnItemType )
{
	return (itemType & (CMManager::it_canvas_item|CMManager::it_pin)) && !(cnItemType & CMManager::isi_isMovable);
}


bool CMItemDrag::mousePressedInitial( const EventInfo &eventInfo )
{
	m_eventInfo = eventInfo;
	b_dragged = false;
	return false;
}


bool CMItemDrag::mouseMoved( const EventInfo &eventInfo )
{
	const QPoint pos = eventInfo.pos;
			
	if ( b_dragged ||
		 pos.x() > (m_eventInfo.pos.x()+4 ) ||
		 pos.x() < (m_eventInfo.pos.x()-4) ||
		 pos.y() > (m_eventInfo.pos.y()+4) ||
		 pos.y() < (m_eventInfo.pos.y()-4) )
	{
	
		b_dragged = true;
		
		if ( PinItem * pi = dynamic_cast<PinItem*>(m_eventInfo.qcanvasItemClickedOn) )
			pi->dragged( pos.x() - m_eventInfo.pos.x() );
	}
	return false;
}


bool CMItemDrag::mouseReleased( const EventInfo &/*eventInfo*/ )
{
	if ( !b_dragged )
	{
		if ( PinItem * pi = dynamic_cast<PinItem*>(m_eventInfo.qcanvasItemClickedOn) )
			pi->switchState();
	}
	
	p_itemDocument->requestStateSave();
	return true;
}


//BEGIN class CanvasEllipseDraw
CanvasEllipseDraw::CanvasEllipseDraw( int x, int y, QCanvas * canvas )
	: QCanvasEllipse( 0, 0, canvas )
{
	move( x, y );
}

void CanvasEllipseDraw::drawShape( QPainter & p )
{
	p.drawEllipse( int(x()-width()/2), int(y()-height()/2), width(), height() );
}
//END class CanvasEllipseDraw


//BEGIN class CMDraw
CMDraw::CMDraw( ItemDocument *itemDocument, CMManager *cmManager )
	: CanvasManipulator( itemDocument, cmManager )
{
	m_pDrawLine = 0l;
	m_pDrawRectangle = 0l;
	m_pDrawEllipse = 0l;
}

CMDraw::~CMDraw()
{
	p_cmManager->setDrawAction(-1);
}

CanvasManipulator* CMDraw::construct( ItemDocument *itemDocument, CMManager *cmManager )
{
	return new CMDraw(itemDocument,cmManager);
}

ManipulatorInfo *CMDraw::manipulatorInfo()
{
	ManipulatorInfo *eventInfo = new ManipulatorInfo();
	eventInfo->m_acceptManipulationPtr = CMDraw::acceptManipulation;
	eventInfo->m_createManipulatorPtr = CMDraw::construct;
	return eventInfo;
}

bool CMDraw::acceptManipulation( uint /*eventState*/, uint cmState, uint /*itemType*/, uint /*cnItemType*/ )
{
	return (cmState & CMManager::cms_draw);
}

bool CMDraw::mousePressedInitial( const EventInfo &eventInfo )
{
	m_eventInfo = eventInfo;
	
	switch ( (DrawPart::DrawAction) p_cmManager->drawAction() )
	{
		case DrawPart::da_text:
		case DrawPart::da_rectangle:
		case DrawPart::da_image:
		{
			m_pDrawRectangle = new QCanvasRectangle( eventInfo.pos.x(), eventInfo.pos.y(), 0, 0, p_canvas );
			m_pDrawRectangle->setPen( QPen( QColor(0,0,0), 1 ) );
			m_pDrawRectangle->setZ( ICNDocument::Z::ConnectorCreateLine );
			m_pDrawRectangle->show();
			break;
		}
		case DrawPart::da_ellipse:
		{
			m_pDrawEllipse = new CanvasEllipseDraw( eventInfo.pos.x(), eventInfo.pos.y(), p_canvas );
			m_pDrawEllipse->setPen( QPen( QColor(0,0,0), 1 ) );
			m_pDrawEllipse->setZ( ICNDocument::Z::ConnectorCreateLine );
			m_pDrawEllipse->show();
			break;
		}
		case DrawPart::da_line:
		case DrawPart::da_arrow:
		{
			m_pDrawLine = new QCanvasLine(p_canvas);
			m_pDrawLine->setPoints( eventInfo.pos.x(), eventInfo.pos.y(), eventInfo.pos.x(), eventInfo.pos.y() );
			m_pDrawLine->setPen( QPen( QColor(0,0,0), 1 ) );
			m_pDrawLine->setZ( ICNDocument::Z::ConnectorCreateLine );
			m_pDrawLine->show();
			break;
		}
		default:
			return true;
	}
	
	return false;
}


bool CMDraw::mouseMoved( const EventInfo &eventInfo )
{
	const QPoint pos = eventInfo.pos;
	
	if (m_pDrawRectangle)
		m_pDrawRectangle->setSize( pos.x()-m_eventInfo.pos.x(), pos.y()-m_eventInfo.pos.y() );
	
	else if (m_pDrawEllipse) {
// 		QRect r( m_eventInfo.pos.x(), m_eventInfo.pos.y(), pos.x()-m_eventInfo.pos.x(), pos.y()-m_eventInfo.pos.y() );
// 		r = r.normalize();
// 		
// 		m_pDrawEllipse->setSize( r.width(), r.height() );
// 		m_pDrawEllipse->move( r.left()+(r.width()/2), r.top()+(r.height()/2) );
		
		m_pDrawEllipse->setSize( 2 * abs(pos.x() - m_eventInfo.pos.x()), 2 * abs(pos.y() - m_eventInfo.pos.y()) );
	}
	
	else if (m_pDrawLine)
		m_pDrawLine->setPoints( eventInfo.pos.x(), eventInfo.pos.y(), m_pDrawLine->endPoint().x(), m_pDrawLine->endPoint().y() );
	else return true;
	
	return false;
}


bool CMDraw::mouseReleased( const EventInfo &eventInfo )
{
	const QPoint pos = eventInfo.pos;
	
	QRect sizeRect;
	
	if ( m_pDrawRectangle || m_pDrawEllipse )
	{
		if (m_pDrawRectangle)
		{
			sizeRect = m_pDrawRectangle->rect();
			
			// We have to manually adjust the size rect so that it matches up with what the user has drawn
			
			sizeRect.setWidth( sizeRect.width()+1 );
			sizeRect.setHeight( sizeRect.height()+1 );
			
			sizeRect = sizeRect.normalize();
			
			if ( m_pDrawRectangle->rect().width() < 0 )
				sizeRect.moveLeft( sizeRect.left() + 1);
			
			if ( m_pDrawRectangle->rect().height() < 0 )
				sizeRect.moveTop( sizeRect.top() + 1);
		} else {
			int w = m_pDrawEllipse->width()+1;
			int h = m_pDrawEllipse->height()+1;
			int x = int(m_pDrawEllipse->x()-w/2);
			int y = int(m_pDrawEllipse->y()-h/2);
			sizeRect = QRect( x, y, w, h ).normalize();
		}
	
		delete m_pDrawRectangle;
		delete m_pDrawEllipse;
		m_pDrawRectangle = 0l;
		m_pDrawEllipse = 0l;
	} else if (m_pDrawLine) {
		int sx = m_pDrawLine->startPoint().x();
		int sy = m_pDrawLine->startPoint().y();
		int ex = m_pDrawLine->endPoint().x();
		int ey = m_pDrawLine->endPoint().y();
		
		sizeRect = QRect( ex, ey, sx-ex, sy-ey );
		
		delete m_pDrawLine;
		m_pDrawLine = 0l;
	} else return true;
	
	QString id;
	switch ( (DrawPart::DrawAction) p_cmManager->drawAction() )
	{
		case DrawPart::da_rectangle:
			id = "dp/rectangle";
			break;
			
		case DrawPart::da_image:
			id = "dp/image";
			break;
			
		case DrawPart::da_ellipse:
			id = "dp/ellipse";
			break;
			
		case DrawPart::da_text:
			id = "dp/canvas_text";
			
			if ( sizeRect.width() < 56 )
				sizeRect.setWidth( 56 );
			
			if ( sizeRect.height() < 24 )
				sizeRect.setHeight( 24 );
			
			break;
			
		case DrawPart::da_line:
			id = "dp/line";
			break;
			
		case DrawPart::da_arrow:
			id = "dp/arrow";
			break;
	}

	if ( id.isEmpty() ) return true;

	Item *item = p_itemDocument->addItem( id, sizeRect.topLeft(), true );

	if (!item) return true;

	item->move( sizeRect.x(), sizeRect.y() ); // We call this again as p_itemDocument->addItem will move the item if it is slightly off the canvas.
	
	item->setSize( 0, 0, sizeRect.width(), sizeRect.height() );
	
	p_itemDocument->requestStateSave();
	return true;
}
//END class CMDraw


//BEGIN class ManualConnectorDraw
ManualConnectorDraw::ManualConnectorDraw( ICNDocument *_icnDocument, const QPoint &initialPos )
{
	m_color = Qt::black;
	
	icnDocument = _icnDocument;
	m_currentPos = m_previousPos = m_initialPos = initialPos;
	p_initialItem = icnDocument->itemAtTop(initialPos);
	
	b_currentVertical = false;
	b_orientationDefined = false;
	
	m_connectorLines.append( m_previousCon = new QCanvasLine( icnDocument->canvas() ) );
	m_connectorLines.append( m_currentCon = new QCanvasLine( icnDocument->canvas() ) );
	
	m_currentCon->setPoints( initialPos.x(), initialPos.y(), initialPos.x(), initialPos.y() );
	m_previousCon->setPoints( initialPos.x(), initialPos.y(), initialPos.x(), initialPos.y() );
	
	m_currentCon->setPen( m_color );
	m_previousCon->setPen( m_color );
	
	updateConnectorEnds();
	
	m_currentCon->show();
	m_previousCon->show();
}


ManualConnectorDraw::~ManualConnectorDraw()
{
	const QList<QCanvasLine*>::iterator end = m_connectorLines.end();
	for ( QList<QCanvasLine*>::iterator it = m_connectorLines.begin(); it != end; ++it )
		delete *it;
	
	m_connectorLines.clear();
}

void ManualConnectorDraw::setColor( const QColor & color )
{
	m_color = color;
	
	const QList<QCanvasLine*>::iterator end = m_connectorLines.end();
	for ( QList<QCanvasLine*>::iterator it = m_connectorLines.begin(); it != end; ++it )
		(*it)->setPen( m_color );
}

void ManualConnectorDraw::mouseMoved( const QPoint &pos )
{
	if ( m_currentPos == pos ) return;
	
	if (!b_orientationDefined)
	{
		QPoint previousStart = m_previousCon->startPoint();
		
		double distance = std::sqrt( std::pow( (double)(m_currentPos.x()-previousStart.x()), 2. ) +
									 std::pow( (double)(m_currentPos.y()-previousStart.y()), 2. ) );
	
		if ( distance < 24 )
		{
			b_currentVertical = ( std::abs( double(m_currentPos.x()-previousStart.x()) ) >= std::abs( double(m_currentPos.y()-previousStart.y()) ) );
		}
		
	}
	
	m_previousPos = m_currentPos;
	m_currentPos = pos;
	updateConnectorEnds();
}


QCanvasItem* ManualConnectorDraw::mouseClicked( const QPoint &pos )
{
	if (b_orientationDefined)
		b_currentVertical = !b_currentVertical;
	else	mouseMoved(pos);
	
	b_orientationDefined = true;

	m_currentPos = pos;
	
	QCanvasItem * qcanvasItem = icnDocument->itemAtTop(pos);
	
	if ( qcanvasItem && pos != m_initialPos && qcanvasItem != p_initialItem )
		return qcanvasItem;
	
	m_previousCon = m_currentCon;
	
	m_connectorLines.append( m_currentCon = new QCanvasLine( icnDocument->canvas() ) );
	m_currentCon->setPoints( pos.x(), pos.y(), pos.x(), pos.y() );
	m_currentCon->setPen( m_color );
	updateConnectorEnds();
	m_currentCon->show();
	
	return 0L;
}


void ManualConnectorDraw::updateConnectorEnds()
{
	QPoint pivot = m_currentPos;
	QPoint previousStart = m_previousCon->startPoint();
	
	if (b_currentVertical)
	{
		pivot.setY( previousStart.y() );
		m_currentCon->setPoints( pivot.x(), pivot.y(), pivot.x(), m_currentPos.y() );
	} else {
		pivot.setX( previousStart.x() );
		m_currentCon->setPoints( pivot.x(), pivot.y(), m_currentPos.x(), pivot.y() );
	}
	
	m_previousCon->setPoints( previousStart.x(), previousStart.y(), pivot.x(), pivot.y() );
}


QPointList ManualConnectorDraw::pointList()
{
	QPointList list;
	list.append(m_initialPos);
	
	const QList<QCanvasLine*>::iterator end = m_connectorLines.end();
	for ( QList<QCanvasLine*>::iterator it = m_connectorLines.begin(); it != end; ++it )
	{
		list.append( (*it)->endPoint() );
	}
	
	return list;
}
//END class ManualConnectorDraw


//BEGIN class ManipulatorInfo
ManipulatorInfo::ManipulatorInfo()
{
}
//END class ManipulatorInfo


#include "canvasmanipulator.moc"
