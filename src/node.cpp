/***************************************************************************
 *   Copyright (C) 2003-2005 by David Saxton                               *
 *   david@bluehaze.org                                                    *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#include "cnitem.h"
#include "icndocument.h"
#include "connector.h"
#include "itemdocumentdata.h"
#include "node.h"

#include <kdebug.h>

#include <qpainter.h>

QColor Node::m_selectedColor = QColor( 101, 134, 192 );

Node::Node( ICNDocument *icnDocument, Node::node_type type, int dir, const QPoint &pos, QString *id )
	: QObject(), QCanvasPolygon( icnDocument ? icnDocument->canvas() : 0 )
{
	m_length = 8;
	p_nodeGroup = 0l;
	p_parentItem = 0L;
	b_deleted = false;
	m_dir = dir;
	m_type = type;
	p_icnDocument = icnDocument;
	m_level = 0;
	
	if ( p_icnDocument ) {
		if (id) {
			m_id = *id;
			if ( !p_icnDocument->registerUID(*id) )
				kdError() << k_funcinfo << "Could not register id " << *id << endl;
		} else m_id = p_icnDocument->generateUID("node"+QString::number(type));
	}
	
	initPoints();
	move( pos.x(), pos.y() );
	setBrush( Qt::black );
	setPen( Qt::black );
	show();
	
	emit (moved(this));
}

Node::~Node()
{
	if ( p_icnDocument )
		p_icnDocument->unregisterUID( id() );
}

void Node::setLevel( const int level )
{
	m_level = level;
}

void Node::setLength( int length )
{
	if ( m_length == length )
		return;
	m_length = length;
	initPoints();
}

void Node::setOrientation( int dir )
{
	if ( m_dir == dir )
		return;
	m_dir = dir;
	initPoints();
}

void Node::initPoints()
{
	if ( type() == ec_junction )
	{
		setPoints( QPointArray( QRect( -4, -4, 8, 8 ) ) );
		return;
	}

	if ( type() == fp_junction )
	{
		setPoints( QPointArray( QRect( -4, -4, 9, 9 ) ) );
		return;
	}

	int l = m_length;
	if ( type() != ec_pin )
		l *= -1;

	// Bounding rectangle, facing right
	QPointArray pa( QRect( 0, -8, l, 16 ) );

	QWMatrix m;
	m.rotate( m_dir );
	pa = m.map(pa);
	setPoints(pa);
}

void Node::setVisible( bool yes )
{
	if ( isVisible() == yes ) return;
	
	QCanvasPolygon::setVisible(yes);
}

void Node::setParentItem( CNItem *parentItem )
{
	if (!parentItem) {
		kdError() << k_funcinfo << "no parent item" << endl;
		return;
	}

	p_parentItem = parentItem;
	
	setLevel(p_parentItem->level());
	
	connect( p_parentItem, SIGNAL(movedBy(double, double )), this, SLOT(moveBy(double, double)) );
	connect( p_parentItem, SIGNAL(removed(Item*)), this, SLOT(removeNode(Item*)) );
}

void Node::removeNode()
{
	if (b_deleted) return;
	b_deleted = true;

	emit removed(this);
	p_icnDocument->appendDeleteList(this);
}

void Node::moveBy( double dx, double dy )
{
	if ( dx == 0 && dy == 0 ) return;
	QCanvasPolygon::moveBy( dx, dy );
	emit moved(this);
}

NodeData Node::nodeData() const
{
	NodeData data;
	data.x = x();
	data.y = y();
	return data;
}

void Node::setNodeSelected( bool yes )
{
	if ( isSelected() == yes )
		return;
	
	QCanvasItem::setSelected(yes);
	
	setPen(   yes ? m_selectedColor : Qt::black );
	setBrush( yes ? m_selectedColor : Qt::black );
}

void Node::initPainter( QPainter & p )
{
	p.translate( int(x()), int(y()) );
	p.rotate( m_dir );
}


void Node::deinitPainter( QPainter & p )
{
	p.rotate( -m_dir );
	p.translate( -int(x()), -int(y()) );
}

#include "node.moc"
