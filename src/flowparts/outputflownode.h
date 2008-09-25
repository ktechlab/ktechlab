//
// C++ Interface: outputflownode
//
// Description: 
//
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef OUTPUTFLOWNODE_H
#define OUTPUTFLOWNODE_H

#include <fpnode.h>

/**
	a FlowNode which serves as Output for flowparts
*/
class OutputFlowNode : public FPNode
{
public:
    OutputFlowNode(ICNDocument* _icnView, int dir, const QPoint& pos, QString* id = 0L);

    ~OutputFlowNode();


};

#endif
