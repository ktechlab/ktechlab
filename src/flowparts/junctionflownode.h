//
// C++ Interface: junctionflownode
//
// Description: 
//
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef JUNCTIONFLOWNODE_H
#define JUNCTIONFLOWNODE_H

#include <fpnode.h>

/**
	A FlowNode which serves as a junction between connectors
*/
class JunctionFlowNode : public FPNode
{
public:
	JunctionFlowNode(ICNDocument* _icnView, int dir, const QPoint& pos, QString* id = 0L);

    ~JunctionFlowNode();


protected:
	virtual void initPoints();

};

#endif
