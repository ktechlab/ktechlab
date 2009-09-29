/***************************************************************************
 *   Copyright (C) 2003-2005 by David Saxton                               *
 *   david@bluehaze.org                                                    *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#ifndef ECNODE_H
#define ECNODE_H

#include "node.h"

#include <qvaluevector.h>

class Element;
class Pin;
class Switch;
class Item;

typedef QValueVector<Pin*> PinVector;

/**
@short Electrical node with voltage / current / etc properties

@author David Saxton
*/
class ECNode : public Node {
	Q_OBJECT

public:
	ECNode(ICNDocument *icnDocument, Node::node_type type, int dir, const QPoint &pos, QString *id = 0);
	~ECNode();

	virtual void setParentItem(CNItem *parentItem);

	/**
	 *  draws the ECNode; still only a pure virtual function
	 */
	virtual void drawShape(QPainter &p) = 0;

	/**
	 * Set the number of pins "contained" in this node.
		 actually creates and deletes pins! =0
	 */
	void setNumPins(unsigned num);

	/**
	 * @return the number of pins in this node.
	 * @see setNumPins
		NOTE: only bus wires/junctions have multiple pins! 
	 */
	unsigned numPins() const {
		return m_pins.size(); }

	/**
	 * @return the pins in the node, as a vector
	 */
	PinVector pins() const { return m_pins; }

	/**
	 * @param num number of the
	 * @return pointer to a pin in this node, given by num
	 */
	Pin *pin(unsigned num = 0) const {
		return (num < m_pins.size()) ? m_pins[num] : 0;
	}

	bool showVoltageBars() const {
		return m_bShowVoltageBars; }

	void setShowVoltageBars(const bool show) {
		m_bShowVoltageBars = show; }

	bool showVoltageColor() const {
		return m_bShowVoltageColor; }

	void setShowVoltageColor(bool show) {
		m_bShowVoltageColor = show; }

	void setNodeChanged();

	/**
	 * Returns true if this node is connected (or is the same as) the node given
	 * by other connectors or nodes (although not through CNItems)
	 * checkedNodes is a list of nodes that have already been checked for
	 * being the connected nodes, and so can simply return if they are in there.
	 * If it is null, it will assume that it is the first ndoe & will create a list
	 */
	virtual bool isConnected(Node *node, NodeList *checkedNodes = 0);

	/**
	 * Sets the node's visibility, as well as updating the visibility of the
	 * attached connectors as appropriate
	 */
	virtual void setVisible(bool yes);

	/**
	 * Registers an input connector (i.e. this is the end node) as connected
	 * to this node.
	 */
	void addConnector(Connector *const connector);

	/**
	 * Creates a new connector, sets this as the end node to the connector
	 * and returns a pointer to the connector.
	 */
	Connector *createConnector(Node *node);

	/**
	 * @return the list of all the connectors attached to the node
	 */
	virtual ConnectorList getAllConnectors() const {
		return m_connectorList;
	}

	/**
	 * Returns the total number of connections to the node. This is the number
	 * of connectors and the parent
	 * item connector if it exists and is requested.
	 * @param includeParentItem Count the parent item as a connector if it exists
	 * @param includeHiddenConnectors hidden connectors are those as e.g. part of a subcircuit
	 */
	virtual int numCon(bool includeParentItem, bool includeHiddenConnectors) const;
	/**
	 * Removes a specific connector
	 */
	virtual void removeConnector(Connector *connector);

	/**
	 * For an electric node: returns the first connector
	 * If the node isn't connected to anyithing, returns null ( 0 )
	 * @return pointer to the desired connector
	 */
	virtual Connector *getAConnector() const ;

signals:
	void numPinsChanged(unsigned newNum);

public slots:
	// -- from node.h --
	void checkForRemoval(Connector *connector);
	void removeNode(Item*) {
// parameter is made available for subclasses. 
		removeNode();
	}
	void removeNode();
protected slots:
	void removeElement(Element *e);
	void removeSwitch(Switch *sw);

protected:
// caches, also allows for context-dependant setting. 
	bool m_bShowVoltageBars;
	bool m_bShowVoltageColor;

	QCanvasRectangle *m_pinPoint;
	PinVector m_pins;

	// -- functionality from node.h --
	/** If this node has precisely two connectors emerging from it, then this
	 * function will trace the two connectors until the point where they
	 * diverge; this point is returned.
	 */
	virtual QPoint findConnectorDivergePoint(bool *found);

	/** The attached connectors to this electronic node. No directionality here */
	ConnectorList m_connectorList;

	/** (please document this) registers some signals for the node and the new connector (?) */
	bool handleNewConnector(Connector *newConnector);
};

#endif

