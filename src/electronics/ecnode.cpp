/***************************************************************************
 *   Copyright (C) 2003-2005 by David Saxton                               *
 *   david@bluehaze.org                                                    *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#include "circuitdocument.h"
#include "component.h"
#include "connector.h"
#include "ecnode.h"
#include "electronicconnector.h"
#include "pin.h"

#include <qdebug.h>
#include <qpainter.h>

#include <ktlconfig.h>

ECNode::ECNode( ICNDocument *icnDocument, Node::node_type _type, int dir, const QPoint &pos, QString *_id )
	: Node( icnDocument, _type, dir, pos, _id )
{
	m_prevV = 0;
	m_prevI = 0;
	m_pinPoint = nullptr;
	m_bShowVoltageBars = KTLConfig::showVoltageBars();
	m_bShowVoltageColor = KTLConfig::showVoltageColor();

	if ( icnDocument )
		icnDocument->registerItem(this);


	m_pins.resize(1);
	m_pins[0] = new Pin(this);
}

ECNode::~ECNode()
{
	if (m_pinPoint) {
		m_pinPoint->setCanvas(nullptr);
		delete m_pinPoint;
	}

	for ( unsigned i = 0; i < m_pins.size(); i++ )
		delete m_pins[i];
	m_pins.resize(0);
}


void ECNode::setNumPins( unsigned num )
{
	unsigned oldNum = m_pins.size();

	if ( num == oldNum ) return;

	if ( num > oldNum ) {
		m_pins.resize(num);
		for ( unsigned i = oldNum; i < num; i++ )
			m_pins[i] = new Pin(this);
	} else {
		for ( unsigned i = num; i < oldNum; i++ )
			delete m_pins[i];
		m_pins.resize(num);
	}

	emit numPinsChanged(num);
}

Pin *ECNode::pin( unsigned num ) const
    { return (num < m_pins.size()) ? m_pins[num] : nullptr; }

void ECNode::setNodeChanged()
{
	if ( !canvas() || numPins() != 1 ) return;

	Pin * pin = m_pins[0];

	double v = pin->voltage();
	double i = pin->current();

	if ( v != m_prevV || i != m_prevI ) {
		QRect r = boundingRect();
// 		r.setCoords( r.left()+(r.width()/2)-1, r.top()+(r.height()/2)-1, r.right()-(r.width()/2)+1, r.bottom()-(r.height()/2)+1 );
		canvas()->setChanged(r);
		m_prevV = v;
		m_prevI = i;
	}
}


void ECNode::setParentItem( CNItem * parentItem )
{
	Node::setParentItem(parentItem);

	if ( Component * component = dynamic_cast<Component*>(parentItem) ) {
		connect( component, SIGNAL(elementDestroyed(Element* )), this, SLOT(removeElement(Element* )) );
		connect( component, SIGNAL(switchDestroyed( Switch* )), this, SLOT(removeSwitch( Switch* )) );
	}
}


void ECNode::removeElement( Element * e )
{
	for ( unsigned i = 0; i < m_pins.size(); i++ )
		m_pins[i]->removeElement(e);
}


void ECNode::removeSwitch( Switch * sw )
{
	for ( unsigned i = 0; i < m_pins.size(); i++ )
		m_pins[i]->removeSwitch( sw );
}


// -- functionality from node.cpp --

bool ECNode::isConnected( Node *node, NodeList *checkedNodes )
{
	if ( this == node )
		return true;

	bool firstNode = !checkedNodes;
	if (firstNode)
		checkedNodes = new NodeList();

	else if ( checkedNodes->contains(this) )
		return false;

	checkedNodes->append(this);

	const ConnectorList::const_iterator inputEnd = m_connectorList.end();
	for ( ConnectorList::const_iterator it = m_connectorList.begin(); it != inputEnd; ++it )
	{
		Connector *connector = *it;
		if (connector) {
			Node *startNode = connector->startNode();
			if ( startNode && startNode->isConnected( node, checkedNodes ) ) {
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

void ECNode::checkForRemoval( Connector *connector )
{
	removeConnector(connector);
	setNodeSelected(false);

	removeNullConnectors();

	if (!p_parentItem) {
		int conCount = m_connectorList.count();
		if ( conCount < 1 )
			removeNode();
	}
}

void ECNode::setVisible( bool yes )
{
	if ( isVisible() == yes ) return;

	KtlQCanvasPolygon::setVisible(yes);

	const ConnectorList::iterator inputEnd = m_connectorList.end();
	for ( ConnectorList::iterator it = m_connectorList.begin(); it != inputEnd; ++it ) {
		Connector *connector = *it;
		if (connector) {
			if ( isVisible() )
				connector->setVisible(true);
			else {
				Node *node = connector->startNode();
				connector->setVisible( node && node->isVisible() );
			}
		}
	}
}

QPoint ECNode::findConnectorDivergePoint( bool * found )
{
	// FIXME someone should check that this function is OK ... I just don't understand what it does
	bool temp;
	if (!found)
		found = &temp;
	*found = false;

	if ( numCon( false, false ) != 2 )
		return QPoint(0,0);

	QPointList p1;
	QPointList p2;

	int inSize = m_connectorList.count();

	const ConnectorList connectors = m_connectorList;
	const ConnectorList::const_iterator end = connectors.end();

	bool gotP1 = false;
	bool gotP2 = false;

	int at = -1;
	for ( ConnectorList::const_iterator it = connectors.begin(); it != end && !gotP2; ++it )
	{
		at++;
		if ( !(*it) || !(*it)->canvas() )
			continue;

		if (gotP1) {
			p2 = (*it)->connectorPoints( at < inSize );
			gotP2 = true;
		} else {
			p1 = (*it)->connectorPoints( at < inSize );
			gotP1 = true;
		}
	}

	if ( !gotP1 || !gotP2 )
		return QPoint(0,0);

	unsigned maxLength = p1.size() > p2.size() ? p1.size() : p2.size();

	for ( unsigned i = 1; i < maxLength; ++i )
	{
		if ( p1[i] != p2[i] ) {
			*found = true;
			return p1[i-1];
		}
	}
	return QPoint(0, 0);
}

void ECNode::addConnector( Connector * const connector )
{
	if ( !handleNewConnector(connector) )
		return;

	m_connectorList.append(connector);
}

bool ECNode::handleNewConnector( Connector * connector )
{
	if (!connector)
		return false;

	if ( m_connectorList.contains(connector) )
	{
		qWarning() << Q_FUNC_INFO << " Already have connector = " << connector << endl;
		return false;
	}

	connect( this, SIGNAL(removed(Node*)), connector, SLOT(removeConnector(Node*)) );
	connect( connector, SIGNAL(removed(Connector*)), this, SLOT(checkForRemoval(Connector*)) );
	connect( connector, SIGNAL(selected(bool)), this, SLOT(setNodeSelected(bool)) );

	if ( !isChildNode() )
		p_icnDocument->slotRequestAssignNG();

	return true;
}

Connector* ECNode::createConnector( Node * node)
{
	// FIXME dynamic_cast used
	Connector *connector = new ElectronicConnector( dynamic_cast<ECNode*>(node), dynamic_cast<ECNode*>(this), p_icnDocument );
	addConnector(connector);

	return connector;
}

void ECNode::removeNullConnectors()
{
	m_connectorList.removeAll((Connector*)nullptr);
}

int ECNode::numCon( bool includeParentItem, bool includeHiddenConnectors ) const
{
	unsigned count = 0;

	const ConnectorList connectors = m_connectorList;

	ConnectorList::const_iterator end = connectors.end();
	for ( ConnectorList::const_iterator it = connectors.begin(); it != end; ++it )
	{
		if ( *it && (includeHiddenConnectors || (*it)->canvas()) )
			count++;
	}


	if ( isChildNode() && includeParentItem )
		count++;

	return count;
}

void ECNode::removeConnector( Connector *connector )
{
	if (!connector) return;

	ConnectorList::iterator it;

	//it = m_connectorList.find(connector); // 2018.12.02
    int i = m_connectorList.indexOf(connector);
    it = (i == -1 ? m_connectorList.end() : (m_connectorList.begin()+i));
	if ( it != m_connectorList.end() )
	{
		(*it)->removeConnector();
		(*it) = nullptr;
	}
}

Connector* ECNode::getAConnector() const
{
	if( ! m_connectorList.isEmpty() )
		return *m_connectorList.begin();
	else	return nullptr;
}
