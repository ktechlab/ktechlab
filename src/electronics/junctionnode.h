//
// C++ Interface: junctionnode
//
// Description: 
//
//
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef JUNCTIONNODE_H
#define JUNCTIONNODE_H

#include <ecnode.h>

/**
 An ECNode class with type() == ec_junction
*/
class JunctionNode : public ECNode
{
public:
	JunctionNode(ICNDocument* icnDocument, int dir, const QPoint& pos, QString* id = 0L);

	~JunctionNode() override;

	/** Returns the node's type. This member will be removed one day */
	virtual node_type type() const { return Node::ec_junction; }

	/**
	 *  draws the JunctionNode
	 */
    	void drawShape( QPainter &p ) override;

protected:
	void initPoints() override;
	
};

#endif
