//
// C++ Interface: pinnode
//
// Description:
//
//
// Copyright: See COPYING file that comes with this distribution
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
	PinNode(ICNDocument* icnDocument, int dir, const QPoint& pos, QString* id = 0);

	~PinNode();

	/** Returns the node's type. This member will be removed one day */
	virtual node_type type() const { return Node::ec_pin; }

	/**
	 *  draws the PinNode
	 */
	virtual void drawShape(QPainter &p);
	
protected:
	QCanvasRectangle *m_pinPoint;
	virtual void initPoints();
};

inline double calcIProp(const double i);
inline int calcThickness(const double prop);
inline int calcLength(double v);

#endif
