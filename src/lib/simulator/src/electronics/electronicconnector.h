//
// C++ Interface: electronicconnector
//
// Description:
//
//
// Author: David Saxton, Alan Grimes, Zoltan Padrah <zoltan.padrah@gmail.com>, (C) 2008
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef ELECTRONICCONNECTOR_H
#define ELECTRONICCONNECTOR_H

#include "connector.h"

#include "typedefs.h"

class ECNode;
class Wire;

/**
	@author David Saxton, Alan Grimes, Zoltan Padrah <zoltan.padrah@gmail.com>

	An electronic connector, connecting 2 ECNodes.
*/
class ElectronicConnector : public Connector {
	Q_OBJECT

public:
    /**
     Create an electroni connector that connects two nodes
     */
	ElectronicConnector(ECNode* startNode, ECNode* endNode);

	virtual ~ElectronicConnector();

	/**
	  * Node at start of connector (which refers to this as the output connector)
	  */
	virtual Node* startNode() const ;

	/**
	 * Node at end of connector (which refers to this as the input connector)
	 */
	virtual Node* endNode() const ;

    /**
     Number of wires in the connector
     */
	virtual int numWires() const ;

    /**
     \return a wire of given number contained in the connector
     */
	Wire *wire(unsigned num = 0) const ;


private:
	/// pointers to the endnodes of the connectors
	ECNode *m_startEcNode;
	ECNode *m_endEcNode;

    /// wires on the connector
    WireList m_wires;
};

#endif

