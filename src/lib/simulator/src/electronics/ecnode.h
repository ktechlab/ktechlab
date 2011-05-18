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

class Circuit;
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
     Create an ECNode with 1 new pin inside
     */
	ECNode(Circuit &c);
    /**
     Create an ECNode based on a given pin
     \param pin an existing Pin
     */
    ECNode(Circuit &c, Pin* pin);

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
    /// the circuit in which the ECNode exists
    Circuit &m_circuit;

    /// pins in this Node
	PinList m_pins;

	/** The attached connectors to this electronic node. No directionality here */
	ConnectorList m_connectorList;

    /** flag to check of the node should delete the pins associated with it,
        or not
        */
    bool m_ownsPins;
};

#endif

