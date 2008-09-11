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
	A FPNode with type() == fp_junction
*/
class JunctionFlowNode : public FPNode
{
public:
	JunctionFlowNode(ICNDocument* _icnView, int dir, const QPoint& pos, QString* id = 0L);

    ~JunctionFlowNode();

    /** Returns the node's type. This member will be removed one day */
    virtual node_type type() const { return Node::fp_junction; }

};

#endif
