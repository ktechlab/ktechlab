//
// C++ Interface: flowconnector
//
// Description:
//
//
// Author: David Saxton, Alan Grimes, Zoltan Padrah <zoltan.padrah@gmail.com>, (C) 2008
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef FLOWCONNECTOR_H
#define FLOWCONNECTOR_H

//#include "fpnode.h"
#include "connector.h"

class FPNode;

/**
	@author David Saxton, Alan Grimes, Zoltan Padrah <zoltan.padrah@gmail.com>

	A flow connector, connecting 2 FPNodes
*/
class FlowConnector : public Connector
{

public:
    FlowConnector(FPNode* startNode, FPNode* endNode, ICNDocument* _ICNDocument, QString* id = nullptr);

    ~FlowConnector() override;

	/**
	 * Node at start of connector (which refers to this as the output connector)
	 */
	Node *startNode() const override ; // { return m_startFpNode );

	/**
	 * Node at end of connector (which refers to this as the input connector)
	 */
	Node *endNode() const override ; // { return m_endFpNode; }

private:
	/// the endnodes of the connector
	QPointer<FPNode> m_startFpNode;
	QPointer<FPNode> m_endFpNode;

};

#endif
