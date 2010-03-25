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

#include "flowconnector.h"
#include "flowicndocument.h"

#include "fpnode.h"

FlowConnector::FlowConnector(FPNode* startNode, FPNode* endNode, ICNDocument* _ICNDocument, const QString &id) : 
	Connector(_ICNDocument, id)
{
	m_startFpNode = startNode;
	m_endFpNode = endNode;

	p_icnDocument->registerItem(this);
	p_icnDocument->requestRerouteInvalidatedConnectors();
}


FlowConnector::~FlowConnector()
{
}
