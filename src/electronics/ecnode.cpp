/***************************************************************************
 *   Copyright (C) 2003-2005 by David Saxton                               *
 *   david@bluehaze.org                                                    *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#include <cassert>

#include "circuitdocument.h"
#include "src/core/ktlconfig.h"
#include "component.h"
#include "connector.h"
#include "ecnode.h"
#include "electronicconnector.h"
#include "pin.h"

#include <kdebug.h>
#include <qpainter.h>

ECNode::ECNode(ICNDocument *icnDocument, Node::node_type _type, int dir, const QPoint &pos, QString *_id)
		: Node(icnDocument, _type, dir, pos, _id) {

	m_bShowVoltageBars = KTLConfig::showVoltageBars();
	m_bShowVoltageColor = KTLConfig::showVoltageColor();

	if (icnDocument)
		icnDocument->registerItem(this);

	m_pins.resize(1);
	m_pins[0] = new Pin();
}

ECNode::~ECNode() {
	// delete all our connectors. 
	ConnectorList::const_iterator end = m_connectorList.end();
	for(ConnectorList::const_iterator it = m_connectorList.begin(); it != end; ++it) {
		removeConnector(*it);	
	}

	for (unsigned i = 0; i < m_pins.size(); i++)
		delete m_pins[i];

	m_pins.resize(0);
}

void ECNode::setNumPins(unsigned num) {
	unsigned oldNum = m_pins.size();

	if (num == oldNum) return;

	if (num > oldNum) {
		m_pins.resize(num);

		for (unsigned i = oldNum; i < num; i++)
			m_pins[i] = new Pin();
	} else {
		for (unsigned i = num; i < oldNum; i++)
			delete m_pins[i];

		m_pins.resize(num);
	}

// FIXME: we segfault when we reduce the size of a bus component because this signal isn't being received. =( 
	emit numPinsChanged(num);
}

void ECNode::setNodeChanged() {
	if (!canvas() || numPins() != 1) return;

//	Pin *pin = m_pins[0];
//	double v = pin->voltage();
//	double i = pin->current();

// TODO: move to a notification based system rather than a remember and check system.

//	if ( v != m_prevV || i != m_prevI ) {
	QRect r = boundingRect();

// 	r.setCoords( r.left()+(r.width()/2)-1, r.top()+(r.height()/2)-1, r.right()-(r.width()/2)+1, r.bottom()-(r.height()/2)+1 );
	canvas()->setChanged(r);

//	m_prevV = v;
//	m_prevI = i;
//	}
}

void ECNode::setParentItem(CNItem *parentItem) {
	Node::setParentItem(parentItem);

	if (Component *component = dynamic_cast<Component*>(parentItem)) {
		connect(component, SIGNAL(elementDestroyed(Element*)), this, SLOT(removeElement(Element*)));
	}
}

void ECNode::removeNode() {
	if(b_deleted) return;

	b_deleted = true;
	emit removed(this);
	p_icnDocument->appendDeleteList(this);
}

/* 
TODO: Do we really need to know about elements and switches at all here? =(  
All we want to care about are components and electronic connectors.

We only want to know about Pins so we can display voltage information, we don't really want to deliver their mail. =\ 
*/
void ECNode::removeElement(Element *e) {
	for (unsigned i = 0; i < m_pins.size(); i++)
		m_pins[i]->removeElement(e);
}

bool ECNode::isConnected(Node *node, NodeList *checkedNodes) {
	if(this == node) return true;

	bool firstNode = !checkedNodes;

	if(firstNode)
		checkedNodes = new NodeList();
	else if(checkedNodes->contains(this))
		return false;

	checkedNodes->append(this);

	const ConnectorList::const_iterator inputEnd = m_connectorList.end();
	for (ConnectorList::const_iterator it = m_connectorList.begin(); it != inputEnd; ++it) {
		Connector *connector = *it;

assert(connector);
		Node *aNode = connector->startNode();
assert(aNode);
		if(aNode->isConnected(node, checkedNodes)) {
			if (firstNode)
				delete checkedNodes;
			return true;
		}

		aNode = connector->endNode();
assert(aNode);
		if(aNode->isConnected(node, checkedNodes)) {
			if (firstNode)
				delete checkedNodes;
			return true;
		}		
	}

	if(firstNode) delete checkedNodes;

	return false;
}

void ECNode::checkForRemoval(Connector *connector) {
	removeConnector(connector);
	setNodeSelected(false);

	if(!p_parentItem && m_connectorList.empty())
		removeNode();
}

void ECNode::setVisible(bool yes) {
	if (isVisible() == yes) return;

	QCanvasPolygon::setVisible(yes);

	const ConnectorList::iterator inputEnd = m_connectorList.end();
	for (ConnectorList::iterator it = m_connectorList.begin(); it != inputEnd; ++it) {
		Connector *connector = *it;

		if(isVisible())
			connector->setVisible(true);
		else	connector->setVisible(connector->startNode()->isVisible());
	}
}

// I think this function tries to fix the case where a connector runs on top of another connector that it is 
// attached to. I think this should be moved up in the class heirarchy. 
// FIXME: Figure out where this function should be used and make sure it's used there! =( 
QPoint ECNode::findConnectorDivergePoint(bool *found) {
	bool temp;

	if(!found)
		found = &temp;

	*found = false;

	if(numCon(false, false) != 2)
		return QPoint(0, 0);

	QPointList p1;
	QPointList p2;

	int inSize = m_connectorList.size();
	const ConnectorList connectors = m_connectorList;
	const ConnectorList::const_iterator end = connectors.end();
	bool gotP1 = false;
	bool gotP2 = false;
	int at = -1;

	for (ConnectorList::const_iterator it = connectors.begin(); it != end && !gotP2; ++it) {
		at++;

		if(gotP1) {
			p2 = (*it)->connectorPoints(at < inSize);
			gotP2 = true;
		} else {
			p1 = (*it)->connectorPoints(at < inSize);
			gotP1 = true;
		}
	}

	if (!gotP1 || !gotP2)
		return QPoint(0, 0);

	unsigned maxLength = p1.size() > p2.size() ? p1.size() : p2.size();

	for(unsigned i = 1; i < maxLength; ++i) {
		if (p1[i] != p2[i]) {
			*found = true;
			return p1[i - 1];
		}
	}

	return QPoint(0, 0);
}

void ECNode::addConnector(Connector *const connector) {
	if (!handleNewConnector(connector))
		return;

	m_connectorList.insert(connector);
}

bool ECNode::handleNewConnector(Connector *connector) {

assert(connector);

	if (m_connectorList.find(connector) != m_connectorList.end()) {
		kdWarning() << k_funcinfo << " Already have connector = " << connector << endl;
		return false;
	}

	connect(this, SIGNAL(removed(Node*)), connector, SLOT(removeConnector(Node*)));

	connect(connector, SIGNAL(removed(Connector*)), this, SLOT(checkForRemoval(Connector*)));
	connect(connector, SIGNAL(selected(bool)), this, SLOT(setNodeSelected(bool)));

	if (!isChildNode())
		p_icnDocument->slotRequestAssignNG();

	return true;
}

Connector* ECNode::createConnector(Node *node) {
	// FIXME dynamic_cast used
	Connector *connector = new ElectronicConnector(dynamic_cast<ECNode*>(node), dynamic_cast<ECNode*>(this), p_icnDocument);
	addConnector(connector);

	return connector;
}

int ECNode::numCon(bool includeParentItem, bool includeHiddenConnectors) const {
	unsigned count = 0;

	ConnectorList::const_iterator end = m_connectorList.end();
	for(ConnectorList::const_iterator it = m_connectorList.begin(); it != end; ++it) {
		if(includeHiddenConnectors || (*it)->canvas())
			count++;
	}

	if(isChildNode() && includeParentItem)
		count++;

	return count;
}

void ECNode::removeConnector(Connector *connector) {
	if(m_connectorList.find(connector) != m_connectorList.end()) {
		connector->removeConnector();
		m_connectorList.erase(connector);
	}
}

Connector *ECNode::getAConnector() const {
	if(!m_connectorList.empty())
		return *m_connectorList.begin();
	else	return 0; // this can only happen on pin nodes, or nodes associated with an item.
}

#include "ecnode.moc"
