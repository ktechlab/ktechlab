/***************************************************************************
 *   Copyright (C) 2003-2005 by David Saxton                               *
 *   david@bluehaze.org                                                    *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#include "canvasitemparts.h"
#include "icndocument.h"
#include "cells.h"
#include "component.h"
#include "pinnode.h"
#include "junctionnode.h"
#include "inputflownode.h"
#include "outputflownode.h"
#include "junctionflownode.h"
#include "itemdocumentdata.h"
#include <qdebug.h>

#include <qbitarray.h>
#include <qpainter.h>

#include <cstdlib>
#include <cmath>

CNItem::CNItem( ICNDocument *icnDocument, bool newItem, const QString &id )
	: Item( icnDocument, newItem, id ),
	  CIWidgetMgr( icnDocument ? icnDocument->canvas() : 0, this ),
	  p_icnDocument(icnDocument),
	  b_pointsAdded(false)
{
    qDebug() << Q_FUNC_INFO << " this=" << this;

	setZ( ICNDocument::Z::Item );
	setSelected(false);
	
	m_brushCol = QColor( 0xf7, 0xf7, 0xff );
	m_selectedCol = QColor( 101, 134, 192 );
	
	setBrush(m_brushCol);
	setPen( QPen( Qt::black ) );
}

CNItem::~CNItem()
{
	const TextMap::iterator textMapEnd = m_textMap.end();
	for ( TextMap::iterator it = m_textMap.begin(); it != textMapEnd; ++it )
	{
		if (it.value()) it.value()->setCanvas(0l);

		delete (Text*)it.value();
	}
	m_textMap.clear();
	
	updateConnectorPoints(false);
}


bool CNItem::preResize( QRect sizeRect )
{
	if ( (std::abs(sizeRect.width()) < minimumSize().width()) ||
		 (std::abs(sizeRect.height()) < minimumSize().height()) )
		return false;
	
	updateConnectorPoints(false);
	return true;
}


void CNItem::postResize()
{
	updateAttachedPositioning();
}


void CNItem::setVisible( bool yes )
{
	if (b_deleted)
	{
		Item::setVisible(false);
		return;
	}
	
	Item::setVisible(yes);
	
	const TextMap::iterator textMapEnd = m_textMap.end();
	for ( TextMap::iterator it = m_textMap.begin(); it != textMapEnd; ++it )
	{
		it.value()->setVisible(yes);
	}

	const NodeInfoMap::iterator nodeMapEnd = m_nodeMap.end();
	for ( NodeInfoMap::iterator it = m_nodeMap.begin(); it != nodeMapEnd; ++it )
	{
		it.value().node->setVisible(yes);
	}
	
	CNItem::setDrawWidgets(yes);
	
	if (!yes)
		updateConnectorPoints(false);
}


void CNItem::reparented( Item *oldParent, Item *newParent )
{
	Item::reparented( oldParent, newParent );
	updateNodeLevels();
}


void CNItem::updateNodeLevels()
{
	int l = level();
	
	// Tell our nodes about our level
	const NodeInfoMap::iterator nodeMapEnd = m_nodeMap.end();
	for ( NodeInfoMap::iterator it = m_nodeMap.begin(); it != nodeMapEnd; ++it )
	{
		it.value().node->setLevel(l);
	}
	
	const ItemList::iterator end = m_children.end();
	for ( ItemList::iterator it = m_children.begin(); it != end; ++it )
	{
		if ( CNItem *cnItem = dynamic_cast<CNItem*>((Item*)*it) )
			cnItem->updateNodeLevels();
	}
}


ConnectorList CNItem::connectorList()
{
	ConnectorList list;

	const NodeInfoMap::iterator nodeMapEnd = m_nodeMap.end();
	for ( NodeInfoMap::iterator it = m_nodeMap.begin(); it != nodeMapEnd; ++it )
	{
		Node *node = p_icnDocument->nodeWithID(it.value().id);
		if (node)
		{
			ConnectorList nodeList = node->getAllConnectors();
			ConnectorList::iterator end = nodeList.end();
			for ( ConnectorList::iterator it = nodeList.begin(); it != end; ++it )
			{
				if ( *it && !list.contains(*it) )
				{
					list.append(*it);
				}
			}

		}
	}
	
	return list;
}


void CNItem::removeItem()
{
	if (b_deleted)
		return;
	
	const TextMap::iterator textMapEnd = m_textMap.end();
	for ( TextMap::iterator it = m_textMap.begin(); it != textMapEnd; ++it )
		it.value()->setCanvas(0l);
	
	Item::removeItem();
	updateConnectorPoints(false);
}


void CNItem::restoreFromItemData( const ItemData &itemData )
{
	Item::restoreFromItemData(itemData);
	
	updateConnectorPoints(false);
	
	{
		const BoolMap::const_iterator end = itemData.buttonMap.end();
		for ( BoolMap::const_iterator it = itemData.buttonMap.begin(); it != end; ++it )
		{
			Button *b = button(it.key());
			if (b)
				b->setState(it.value());
		}
	}
	{
		const IntMap::const_iterator end = itemData.sliderMap.end();
		for ( IntMap::const_iterator it = itemData.sliderMap.begin(); it != end; ++it )
		{
			Slider *s = slider(it.key());
			if (s)
				s->setValue(it.value());
		}
	}
}


ItemData CNItem::itemData() const
{
	ItemData itemData = Item::itemData();
	
	const WidgetMap::const_iterator end = m_widgetMap.end();
	for ( WidgetMap::const_iterator it = m_widgetMap.begin(); it != end; ++it )
	{
		if ( Slider *slider = dynamic_cast<Slider*>(*it) )
			itemData.sliderMap[slider->id()] = slider->value();
		
		else if ( Button *button = dynamic_cast<Button*>(*it) )
			itemData.buttonMap[button->id()] = button->state();
		
	}
	
	return itemData;
}


Node* CNItem::createNode( double _x, double _y, int orientation, const QString &name, uint type )
{
	orientation %= 360;
	if ( orientation < 0 )
		orientation += 360;
	
	Node *node = NULL;

	// TODO get rid of this switch statement... 
	switch(type) {
		case Node::ec_pin:
			node = new PinNode(p_icnDocument, orientation, QPoint( 0, 0) );
			break;
		case Node::ec_junction:
			node = new JunctionNode( p_icnDocument, orientation, QPoint( 0, 0) );
			break;
		case Node::fp_junction:
			node = new JunctionFlowNode(p_icnDocument, orientation, QPoint( 0, 0) );
			break;
		case Node::fp_in:
			node = new InputFlowNode( p_icnDocument, orientation, QPoint( 0, 0) );
			break;
		case Node::fp_out:
			node = new OutputFlowNode( p_icnDocument, orientation, QPoint( 0, 0) );
			break;
	}
			
	node->setLevel( level() );
	
	node->setParentItem(this);
	node->setChildId(name);
	
	NodeInfo info;
	info.id = node->id();
	info.node = node;
	info.x = _x;
	info.y = _y;
	info.orientation = orientation;
	
	m_nodeMap[name] = info;
	
	updateAttachedPositioning();
	
	return node;
}


bool CNItem::removeNode( const QString &name )
{
	NodeInfoMap::iterator it = m_nodeMap.find(name);
	if ( it == m_nodeMap.end() ) {
		return false;
	}
	it.value().node->removeNode();
	p_icnDocument->flushDeleteList();
	m_nodeMap.erase(it);
	return true;
}

Node *CNItem::getClosestNode( const QPoint &pos )
{
	// Work through the nodes, finding the one closest to the (x, y) position
	Node *shortestNode = 0L;
	double shortestDistance = 1e10; // Nice large distance :-)
	
	const NodeInfoMap::iterator end = m_nodeMap.end();
	for ( NodeInfoMap::iterator it = m_nodeMap.begin(); it != end; ++it )
	{
		Node *node = p_icnDocument->nodeWithID(it.value().id);
		if (node)
		{
			// Calculate the distance
			// Yeah, it's actually the distance squared; but it's all relative, so doesn't matter
			double distance = std::pow(node->x()-pos.x(),2) + std::pow(node->y()-pos.y(),2);
			
			if ( distance < shortestDistance )
			{
				shortestDistance = distance;
				shortestNode = node;
			}
		}
	}
	
	return shortestNode;
}


void CNItem::updateAttachedPositioning()
{
	if (b_deleted)
		return;
	
	// Actually, we don't do anything anymore...
}


void CNItem::updateZ( int baseZ )
{
	Item::updateZ(baseZ);
	
	double _z = z();
	
	const NodeInfoMap::iterator nodeMapEnd = m_nodeMap.end();
	for ( NodeInfoMap::iterator it = m_nodeMap.begin(); it != nodeMapEnd; ++it )
		it.value().node->setZ( _z + 0.5 );
	
	const WidgetMap::iterator widgetMapEnd = m_widgetMap.end();
	for ( WidgetMap::iterator it = m_widgetMap.begin(); it != widgetMapEnd; ++it )
		it.value()->setZ( _z + 0.5 );
	
	const TextMap::iterator textMapEnd = m_textMap.end();
	for ( TextMap::iterator it = m_textMap.begin(); it != textMapEnd; ++it )
		it.value()->setZ( _z + 0.5 );
}


void CNItem::moveBy( double dx, double dy )
{
	if ( dx == 0 && dy == 0 ) return;
	updateConnectorPoints(false);
	Item::moveBy( dx, dy );
	
	setWidgetsPos( QPoint( int(x()), int(y()) ) );
}


bool CNItem::mousePressEvent( const EventInfo &info )
{
	bool accepted = Item::mousePressEvent(info);
	if (!accepted)
		accepted = CIWidgetMgr::mousePressEvent(info);
	if (accepted)
		setChanged();
	return accepted;
}


bool CNItem::mouseReleaseEvent( const EventInfo &info )
{
	bool accepted = Item::mouseReleaseEvent(info);
	if (!accepted)
		accepted = CIWidgetMgr::mouseReleaseEvent(info);
	if (accepted)
		setChanged();
	return accepted;
}


bool CNItem::mouseDoubleClickEvent( const EventInfo &info )
{
	bool accepted = Item::mouseDoubleClickEvent(info);
	if (!accepted)
		accepted = CIWidgetMgr::mouseDoubleClickEvent(info);
	if (accepted)
		setChanged();
	return accepted;
}


bool CNItem::mouseMoveEvent( const EventInfo &info )
{
	bool accepted = Item::mouseMoveEvent(info);
	if (!accepted)
		accepted = CIWidgetMgr::mouseMoveEvent(info);
	if (accepted)
		setChanged();
	return accepted;
}


bool CNItem::wheelEvent( const EventInfo &info )
{
	bool accepted = Item::wheelEvent(info);
	if (!accepted)
		accepted = CIWidgetMgr::wheelEvent(info);
	if (accepted)
		setChanged();
	return accepted;
}


void CNItem::enterEvent(QEvent *)
{
	Item::enterEvent(0);
	CIWidgetMgr::enterEvent(0);
	setChanged();
}


void CNItem::leaveEvent(QEvent *)
{
	Item::leaveEvent(0);
	CIWidgetMgr::leaveEvent(0);
	setChanged();
}


void CNItem::drawShape( QPainter &p )
{
	if (!isVisible())
		return;
	
// 	initPainter(p);
	if ( isSelected() )
		p.setPen(m_selectedCol);
	
	p.drawPolygon(areaPoints());
	p.drawPolyline(areaPoints());
// 	deinitPainter(p);
}


void CNItem::initPainter( QPainter &p )
{
	if ( isSelected() )
		p.setPen(m_selectedCol);
}


void CNItem::updateConnectorPoints( bool add )
{
	if ( b_deleted || !isVisible() )
		add = false;

	if ( b_pointsAdded == add )
		return;

	b_pointsAdded = add;

	Cells *cells = p_icnDocument->cells();
	if (!cells)
		return;
	
	// Get translation matrix
	// Hackish...
	QMatrix m;
	if ( Component *c = dynamic_cast<Component*>(this) )
		m = c->transMatrix( c->angleDegrees(), c->flipped(), int(x()), int(y()), false );
	
	// Convention used here: _UM = unmapped by both matrix and cell reference, _M = mapped

	const QPoint start_UM = QPoint( int(x()+offsetX())-8, int(y()+offsetY())-8 );
	const QPoint end_UM = start_UM + QPoint( width()+2*8, height()+2*8 );
	
	const QPoint start_M = roundDown( m.map(start_UM), 8 );
	const QPoint end_M = roundDown( m.map(end_UM), 8 );
	
	
	int sx_M = start_M.x();
	int ex_M = end_M.x();
	
	int sy_M = start_M.y();
	int ey_M = end_M.y();
	
	
	// Normalise start and end points
	if ( sx_M > ex_M )
	{
		const int temp = sx_M;
		sx_M = ex_M;
		ex_M = temp;
	}
	if ( sy_M > ey_M )
	{
		const int temp = sy_M;
		sy_M = ey_M;
		ey_M = temp;
	}
	
	ex_M++;
	ey_M++;
	
	const int mult = add ? 1 : -1;
	
	for ( int x = sx_M; x < ex_M; x++ )
	{
		for ( int y = sy_M; y < ey_M; y++ )
		{
			if ( cells->haveCell( x, y ) )
			{
				if ( x != sx_M && y != sy_M && x != (ex_M-1) && y != (ey_M-1) )
				{
					cells->cell( x, y ).CIpenalty += mult*ICNDocument::hs_item;
				}
				else 
				{
//					(*cells)[x][y].CIpenalty += mult*ICNDocument::hs_item/2;
					cells->cell( x, y ).CIpenalty += mult*ICNDocument::hs_connector*5;
				}
			}
		}
	}
	
#if 0
	// And subtract the positions of the node on the border
	NodeInfoMap::iterator end = m_nodeMap.end();
	for ( NodeInfoMap::iterator it = m_nodeMap.begin(); it != end; ++it )
	{
		const int x = (int)((it->second.node->x()-4)/cellSize);
		const int y = (int)((it->second.node->y()-4)/cellSize);
		if ( p_icnDocument->isValidCellReference(x,y) ) {
			(*cells)[x][y].CIpenalty -= mult*ICNDocument::hs_connector*5;
		}
	}
#endif
	
	const TextMap::iterator textMapEnd = m_textMap.end();
	for ( TextMap::iterator it = m_textMap.begin(); it != textMapEnd; ++it )
	{
		it.value()->updateConnectorPoints(add);
	}
	const WidgetMap::iterator widgetMapEnd = m_widgetMap.end();
	for ( WidgetMap::iterator it = m_widgetMap.begin(); it != widgetMapEnd; ++it )
	{
		it.value()->updateConnectorPoints(add);
	}
}


Text* CNItem::addDisplayText( const QString &id, const QRect & pos, const QString &display, bool internal, int flags )
{
	Text *text = 0l;
	TextMap::iterator it = m_textMap.find(id);
	if ( it != m_textMap.end() )
	{
// 		qWarning() << "CNItem::addDisplayText: removing old text"<<endl;
		delete it.value();
		m_textMap.erase(it);
	}
	
	text = new Text( "", this, pos, canvas(), flags );
	text->setZ( z()+(internal?0.1:-0.1) );
	
	m_textMap[id] = text;
	
	// Calculate the correct size
	setDisplayText( id, display );
	text->show();
	return text;
}


void CNItem::setDisplayText( const QString &id, const QString &display )
{
	TextMap::iterator it = m_textMap.find(id);
	if ( it == m_textMap.end() )
	{
		qCritical() << "CNItem::setDisplayText: Could not find text with id \""<<id<<"\""<<endl;
		return;
	}
	it.value()->setText(display);
	updateAttachedPositioning();
}


void CNItem::removeDisplayText( const QString &id )
{
	TextMap::iterator it = m_textMap.find(id);
	if ( it == m_textMap.end() )
	{
// 		qCritical() << "CNItem::removeDisplayText: Could not find text with id \""<<id<<"\""<<endl;
		return;
	}
	it.value()->updateConnectorPoints(false);
	delete it.value();
	m_textMap.erase(it);
}


QString CNItem::nodeId( const QString &internalNodeId )
{
	NodeInfoMap::iterator it = m_nodeMap.find(internalNodeId);
	if ( it == m_nodeMap.end() ) return "";
	else return it.value().id;
}


Node *CNItem::childNode( const QString &childId )
{
	return p_icnDocument->nodeWithID( nodeId(childId) );
}


NodeInfo::NodeInfo()
{
	node = 0l;
	x = 0.;
	y = 0.;
	orientation = 0;
}


#include "cnitem.moc"
