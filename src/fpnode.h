/***************************************************************************
 *   Copyright (C) 2003-2005 by David Saxton                               *
 *   david@bluehaze.org                                                    *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#ifndef FPNODE_H
#define FPNODE_H

#include "node.h"


class FlowPart;
typedef QValueList<FlowPart*> FlowPartList;

/**
You should use this node for all FlowParts. It ensures that connections between FlowParts are
always valid (eg not more than two outputs from one node, which makes no sense)
@short FlowPart node
@author David Saxton
*/
class FPNode : public Node
{
Q_OBJECT
public:
    FPNode( ICNDocument *_icnView, Node::node_type type, node_dir dir, const QPoint &pos, QString *id = 0 );
    ~FPNode();
	
	/**
	 * Returns a pointer to the FlowPart attached to this node if this node isInput, or
	 * to the other end of the connector (if one exists) if it isOutput()
	 */
	FlowPart *outputFlowPart() const;
	/** 
	 * Returns a list of FlowParts attached to the node - either a single-item list containing
	 * the FlowPart attached to this node if isOutput, or a list of FlowParts connected to the
	 * input (?) connectors
	 */
	FlowPartList inputFlowParts() const;

protected:
	virtual void drawShape( QPainter & p );
	
private:
	bool m_isInput;
};

#endif

