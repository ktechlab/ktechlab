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
	
	if ( p_icnDocument )
	{
		if (id)
		{
			m_id = *id;
			if ( !p_icnDocument->registerUID(*id) )
				kdError() << k_funcinfo << "Could not register id " << *id << endl;
		}
		else
			m_id = p_icnDocument->generateUID("node"+QString::number(type));
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


bool Node::acceptInput() const
{
	return type() != fp_out;
}


bool Node::acceptOutput() const
{
	return type() != fp_in;
}


void Node::setVisible( bool yes )
{
	if ( isVisible() == yes )
		return;
	
	QCanvasPolygon::setVisible(yes);
	
	const ConnectorList::iterator inputEnd = m_inputConnectorList.end();
	for ( ConnectorList::iterator it = m_inputConnectorList.begin(); it != inputEnd; ++it )
	{
		Connector *connector = *it;
		if (connector)
		{
			if ( isVisible() )
				connector->setVisible(true);
			
			else
			{
				Node *node = connector->startNode();
				connector->setVisible( node && node->isVisible() );
			}
		}
	}
	
	const ConnectorList::iterator outputEnd = m_outputConnectorList.end();
	for ( ConnectorList::iterator it = m_outputConnectorList.begin(); it != outputEnd; ++it )
	{
		Connector *connector = *it;
		if (connector)
		{
			if ( isVisible() )
				connector->setVisible(true);
			
			else
			{
				Node *node = connector->endNode();
				connector->setVisible( node && node->isVisible() );
			}
		}
	}
}


bool Node::isConnected( Node *node, NodeList *checkedNodes )
{
	if ( this == node )
		return true;
		
	bool firstNode = !checkedNodes;
	if (firstNode)
		checkedNodes = new NodeList();
		
	else if ( checkedNodes->contains(this) )
		return false;
	
	
	checkedNodes->append(this);
	
	const ConnectorList::const_iterator inputEnd = m_inputConnectorList.end();
	for ( ConnectorList::const_iterator it = m_inputConnectorList.begin(); it != inputEnd; ++it )
	{
		Connector *connector = *it;
		if (connector)
		{
			Node *startNode = connector->startNode();
			if ( startNode && startNode->isConnected( node, checkedNodes ) ) {
				if (firstNode) {
					delete checkedNodes;
				}
				return true;
			}
		}
	}
	
	const ConnectorList::const_iterator outputEnd = m_outputConnectorList.end();
	for ( ConnectorList::const_iterator it = m_outputConnectorList.begin(); it != outputEnd; ++it )
	{
		Connector *connector = *it;
		if (connector)
		{
			Node *endNode = connector->endNode();
			if ( endNode && endNode->isConnected( node, checkedNodes ) ) {
				if (firstNode) {
					delete checkedNodes;
				}
				return true;
			}
		}
	}
	
	if (firstNode) {
		delete checkedNodes;
	}
	
	return false;
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


QPoint Node::findConnectorDivergePoint( bool * found )
{
	bool temp;
	if (!found)
		found = &temp;
	*found = false;
	
	if ( numCon( false, false ) != 2 )
		return QPoint(0,0);
	
	QPointList p1;
	QPointList p2;
	
	int inSize = m_inputConnectorList.count();
	
	const ConnectorList connectors = m_inputConnectorList + m_outputConnectorList;
	const ConnectorList::const_iterator end = connectors.end();
	bool gotP1 = false;
	bool gotP2 = false;
	int at = -1;
	for ( ConnectorList::const_iterator it = connectors.begin(); it != end && !gotP2; ++it )
	{
		at++;
		if ( !(*it) || !(*it)->canvas() )
			continue;
		
		if (gotP1)
		{
			p2 = (*it)->connectorPoints( at < inSize );
			gotP2 = true;
		}
		else
		{
			p1 = (*it)->connectorPoints( at < inSize );
			gotP1 = true;
		}
	}
	if ( !gotP1 || !gotP2 )
		return QPoint(0,0);
	
	unsigned maxLength = p1.size() > p2.size() ? p1.size() : p2.size();
	
	for ( unsigned i = 1; i < maxLength; ++i )
	{
		if ( p1[i] != p2[i] )
		{
			*found = true;
			return p1[i-1];
		}
	}
	return QPoint(0, 0);
}


void Node::setParentItem( CNItem *parentItem )
{
	if (!parentItem)
	{
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
	if (b_deleted)
		return;
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


int Node::numCon( bool includeParentItem, bool includeHiddenConnectors ) const
{
	unsigned count = 0;
	
	const ConnectorList connectors[2] = { m_inputConnectorList, m_outputConnectorList };
	
	for ( unsigned i = 0; i < 2; i++ )
	{
		ConnectorList::const_iterator end = connectors[i].end();
		for ( ConnectorList::const_iterator it = connectors[i].begin(); it != end; ++it )
		{
			if ( *it && (includeHiddenConnectors || (*it)->canvas()) )
				count++;
		}
	}
	
	if ( isChildNode() && includeParentItem )
		count++;
	
	return count;
}


void Node::addOutputConnector( Connector * const connector )
{
	if ( type() == fp_in || !handleNewConnector(connector) )
		return;
	
	m_outputConnectorList.append(connector);
	
	if ( type() == fp_out || type() == fp_junction )
	{
		// We can only have one output connector, so remove the others. Note
		// that this code has to come *after* adding the new output connector,
		// as this node will delete itself if it's an fp_junction and there are
		// no output connectors.
		
		const ConnectorList connectors = m_outputConnectorList;
		const ConnectorList::const_iterator end = connectors.end();
		for ( ConnectorList::const_iterator it = connectors.begin(); it != end; ++it )
		{
			Connector * con = *it;
			if ( con && con != connector )
				con->removeConnector();
		}
	}
	
	m_outputConnectorList.remove((Connector*)0l);
}


void Node::addInputConnector( Connector * const connector )
{
	if ( type() == fp_out || !handleNewConnector(connector) )
		return;
	
	m_inputConnectorList.append(connector);
}


bool Node::handleNewConnector( Connector * connector )
{
	if (!connector)
		return false;
	
	if ( m_inputConnectorList.contains(connector) || m_outputConnectorList.contains(connector) )
	{
		kdWarning() << k_funcinfo << " Already have connector = " << connector << endl;
		return false;
	}
	
	connect( this, SIGNAL(removed(Node*)), connector, SLOT(removeConnector(Node*)) );
	connect( connector, SIGNAL(removed(Connector*)), this, SLOT(checkForRemoval(Connector*)) );
	connect( connector, SIGNAL(selected(bool)), this, SLOT(setNodeSelected(bool)) );
	
	if ( !isChildNode() )
		p_icnDocument->slotRequestAssignNG();
	
	return true;
}


Connector* Node::createInputConnector( Node * startNode )
{
	if ( type() == fp_out || !startNode )
		return 0l;
	
	Connector *connector = new Connector( startNode, this, p_icnDocument );
	addInputConnector(connector);
	
	return connector;
}


void Node::removeConnector( Connector *connector )
{
	if (!connector)
		return;
	
	ConnectorList::iterator it;
	
	it = m_inputConnectorList.find(connector);
	if ( it != m_inputConnectorList.end() )
	{
		(*it)->removeConnector();
		(*it) = 0L;
	}
	
	it = m_outputConnectorList.find(connector);
	if ( it != m_outputConnectorList.end() )
	{
		(*it)->removeConnector();
		(*it) = 0L;
	}
}

void Node::checkForRemoval( Connector *connector )
{
	removeConnector(connector);
	setNodeSelected(false);
	
	removeNullConnectors();
	
	if (!p_parentItem)
	{
		int conCount = m_inputConnectorList.count() + m_outputConnectorList.count();
		if ( conCount < 2 )
			removeNode();
	}
	
	if ( type() == Node::fp_junction && m_outputConnectorList.isEmpty() )
		removeNode();
}

void Node::removeNullConnectors()
{
	m_inputConnectorList.remove((Connector*)0L);
	m_outputConnectorList.remove((Connector*)0L);
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
