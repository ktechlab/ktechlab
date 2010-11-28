/***************************************************************************
 *   Copyright (C) 2003-2005 by David Saxton                               *
 *   david@bluehaze.org                                                    *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#ifndef CONNECTOR_H
#define CONNECTOR_H

#include <QObject>

class Node;

/**
@short Represents a connection between two Nodes
@author David Saxton, Zoltan Padrah

A connector contains one or more Wires internally.
*/
class Connector : public QObject {
	Q_OBJECT

public:
	Connector();
	~Connector();

	/**
	 * Node at start of connector (which refers to this as the output connector)
	 */
	virtual Node *startNode() const = 0;
    /**
     * Node at the end of connector.
     */
	virtual Node *endNode() const = 0;

protected:

};

#endif

