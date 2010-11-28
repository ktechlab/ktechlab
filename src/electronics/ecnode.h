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

#include "connector.h"
#include "node.h"
#include "pin.h"
#include "typedefs.h"

class Element;
class Item;

/**
@short Electrical node with voltage / current / etc properties

@author David Saxton
*/
class ECNode : public Node {
	Q_OBJECT

public:
    /**
     Create an ECNode with 1 pin inside
     */
	ECNode();
	~ECNode();

    // --- internal pin related functions ---
	/**
     Set the number of pins "contained" in this node.
		 actually creates and deletes pins! =0
	 */
	void setNumPins(unsigned num);

	/**
	 * @return the number of pins in this node.
	 * @see setNumPins
	 */
	int numPins() const ;

	/**
	 * @return the pins in the node, as a vector
	 */
	PinList pins() const ;

	/**
	 * @param num number of the
	 * @return pointer to a pin in this node, given by num,
     *   or null, if the gien pin doesn't exist
	 */
	Pin *pin(unsigned num = 0) const ;

    // --- connector related --
	/**
	 * Registers an input connector (i.e. this is the end node) as connected
	 * to this node.
	 */
	virtual void addConnector(Connector *connector);

	/**
	 * @return the list of all the connectors attached to the node
	 */
	virtual ConnectorList allConnectors() const ;

    /**
     * @return the number of connectors connected to this node
     */
	virtual int numCon() const;

    /**
	 * Removes a specific connector
	 */
	virtual void removeConnector(Connector *connector);

protected:
    /// pins in this Node
	PinList m_pins;

	/** The attached connectors to this electronic node. No directionality here */
	ConnectorList m_connectorList;
};

#endif

