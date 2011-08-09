/***************************************************************************
 *   Copyright (C) 2003-2005 by David Saxton                               *
 *   david@bluehaze.org                                                    *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#ifndef NODE_H
#define NODE_H

#include "connector.h"
#include "typedefs.h"
#include "simulatorexport.h"

#include <QList>
#include <QObject>

#include <set>

class Node;


/**
@short A standard node that can be associated with a Connector
@author David Saxton, Zoltan Padrah

A node is associated to one or more pins in the simulation.
Two nodes are connected by a \ref Connector.

Note that a node might change the number of Pins it contains,
during runtime.
*/
class SIMULATOR_EXPORT Node : public QObject {
	Q_OBJECT

public:
    /**
     Create a Node with 1 associated pin and 0 connector.
     */
	Node();
	virtual ~Node();


    /**
     * Registers an input connector (i.e. this is the end node) as connected
     * to this node.
     */
    virtual void addConnector(Connector *connector) = 0;

    /**
     * @return the list of all the connectors attached to the node
     */
    virtual ConnectorList allConnectors() const = 0;

    /**
     * @return the number of connectors connected to this node
     */
    virtual int numCon() const = 0;

    /**
     * Removes a specific connector
     */
    virtual void removeConnector(Connector *connector) = 0;

};

#endif
