//
// C++ Implementation: flowconnector
//
// Description: 
//
//
// Author: David Saxton, Alan Grimes, Zoltan Padrah <zoltan.padrah@gmail.com>, (C) 2008
//
// Copyright: See COPYING file that comes with this distribution
//
//
#include "flowconnector.h"

#include "fpnode.h"

FlowConnector::FlowConnector(FPNode* startNode, FPNode* endNode, ICNDocument* _ICNDocument, QString* id): Connector(startNode, endNode, _ICNDocument, id)
{
	m_startFpNode = startNode;
	m_endFpNode = endNode;
}


FlowConnector::~FlowConnector()
{
}

Node *FlowConnector::startNode() const {
    FPNode *fpNode = m_startFpNode;
    Node *node = dynamic_cast<Node*> (fpNode);
    return node;
    // return static_cast<Node*> ( static_cast<FPNode*> (m_startFpNode) );

}

Node *FlowConnector::endNode() const {
    return m_endFpNode;

}


