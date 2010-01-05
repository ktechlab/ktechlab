//
// C++ Interface: pinnode
//
// Description: 
//
//
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef PINNODE_H
#define PINNODE_H

#include <ecnode.h>

/**
An ECNode class with type() == ec_pin

*/
class PinNode : public ECNode
{
public:
	PinNode(ICNDocument* icnDocument, int dir, const QPoint& pos, QString* id = 0L);

	~PinNode();

	/** Returns the node's type. This member will be removed one day */
	virtual node_type type() const { return Node::ec_pin; }

	/**
	 *  draws the PinNode
	 */
	virtual void drawShape( QPainter &p );
	/**
	 * Removes a specific connector
	 */
	virtual void removeConnector(Connector *connector) {
		m_connectorList.remove(connector);
	}
protected:
	virtual void initPoints();
};

#endif
