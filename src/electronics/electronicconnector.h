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

#include <connector.h>

#include "ecnode.h"

/**
	@author David Saxton, Alan Grimes, Zoltan Padrah <zoltan.padrah@gmail.com>
	
	An electronic connector, connecting 2 ECNodes.
*/
class ElectronicConnector : public Connector
{
	Q_OBJECT

public:
    ElectronicConnector(ECNode* startNode, ECNode* endNode, ICNDocument* _ICNDocument, QString* id = 0);

    ~ElectronicConnector() override;

   /**
     * Node at start of connector (which refers to this as the output connector)
     */
    Node* startNode() const override { return m_startEcNode; }

    /**
     * Node at end of connector (which refers to this as the input connector)
     */
    Node* endNode() const override { return m_endEcNode; }

public slots:
	/**
	 * Takes the minimum pin count of the start and end nodes, and creates a
	 * connector for each pin up to that minimum.
	 */
	void syncWiresWithNodes();

private:
	/// pointers to the endnodes of the connectors
	QPointer<ECNode> m_startEcNode;
	QPointer<ECNode> m_endEcNode;
	
};

#endif

