//
// C++ Implementation: outputflownode
//
// Description:
//
//
// Author: David Saxton <david@bluehaze.org>, (C) 2008
//
// Copyright: See COPYING file that comes with this distribution
//
//
#include "outputflownode.h"
#include "connector.h"

#include <QPainter>

#include <ktechlab_debug.h>

OutputFlowNode::OutputFlowNode(ICNDocument *_icnView, int dir, const QPoint &pos, QString *id)
    : FPNode(_icnView, Node::fp_out, dir, pos, id)
{
}

OutputFlowNode::~OutputFlowNode()
{
}

bool OutputFlowNode::acceptInput() const
{
    return false;
}

bool OutputFlowNode::acceptOutput() const
{
    return true;
}

void OutputFlowNode::addInputConnector(Connector *const /*connector*/)
{
    qCDebug(KTL_LOG) << "BUG: trying to add input connector to an output node";
}

inline QPolygon arrowPoints(int dir)
{
    QPolygon pa(3);
    switch (dir) {
    case 0:
        pa[0] = QPoint(3, 0);
        pa[1] = QPoint(0, 2);
        pa[2] = QPoint(0, -2);
        break;
    case 180:
        pa[0] = QPoint(-3, 0);
        pa[1] = QPoint(0, 2);
        pa[2] = QPoint(0, -2);
        break;
    case 90:
        pa[0] = QPoint(2, 0);
        pa[1] = QPoint(-2, 0);
        pa[2] = QPoint(0, 3);
        break;
    case 270:
        pa[0] = QPoint(2, 0);
        pa[1] = QPoint(-2, 0);
        pa[2] = QPoint(0, -3);
        break;
    };
    return pa;
}

void OutputFlowNode::drawShape(QPainter &p)
{
    const int _x = int(x());
    const int _y = int(y());

    if (m_dir == 0)
        p.drawLine(_x, _y, _x - 8, _y);
    else if (m_dir == 90)
        p.drawLine(_x, _y, _x, _y - 8);
    else if (m_dir == 180)
        p.drawLine(_x, _y, _x + 8, _y);
    else if (m_dir == 270)
        p.drawLine(_x, _y, _x, _y + 8);

    QPolygon pa(3);

    switch (m_dir) {
    case 0: // right
        pa = arrowPoints(0);
        break;
    case 180: // left
        pa = arrowPoints(180);
        break;
    case 90: // down
        pa = arrowPoints(90);
        break;
    case 270: // up
        pa = arrowPoints(270);
        break;
    default:
        qCCritical(KTL_LOG) << "BUG: m_dir = " << m_dir;
    }

    // Note: I have not tested the positioning of the arrows for all combinations.
    // In fact, most almost definitely do not work. So feel free to change the code
    // as you see fit if necessary.

    if (m_dir == 0)
        pa.translate(-5, 0);
    else if (m_dir == 90)
        pa.translate(0, -5);
    else if (m_dir == 180)
        pa.translate(5, 0);
    else if (m_dir == 270)
        pa.translate(0, 5);

    pa.translate(_x, _y);
    p.drawPolygon(pa);
}
