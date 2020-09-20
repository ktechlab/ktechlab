//
// C++ Implementation: junctionflownode
//
// Description:
//
//
// Copyright: See COPYING file that comes with this distribution
//
//
#include "junctionflownode.h"
#include "connector.h"
#include "flowconnector.h"

#include <QPainter>

JunctionFlowNode::JunctionFlowNode(ICNDocument *_icnView, int dir, const QPoint &pos, QString *id)
    : FPNode(_icnView, Node::fp_junction, dir, pos, id)
{
}

JunctionFlowNode::~JunctionFlowNode()
{
}

void JunctionFlowNode::initPoints()
{
    setPoints(QPolygon(QRect(-4, -4, 9, 9)));
}

bool JunctionFlowNode::acceptInput() const
{
    return true;
}

bool JunctionFlowNode::acceptOutput() const
{
    return true;
}

void JunctionFlowNode::checkForRemoval(Connector *connector)
{
    FPNode::checkForRemoval(connector);

    if (!m_outputConnector)
        removeNode();
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

void JunctionFlowNode::drawShape(QPainter &p)
{
    const int _x = (int)x();
    const int _y = (int)y();

    if (!m_inFlowConnList.isEmpty()) {
        const FlowConnectorList::iterator end = m_inFlowConnList.end();
        for (FlowConnectorList::iterator it = m_inFlowConnList.begin(); it != end; ++it) {
            Connector *connector = *it;
            if (!connector)
                continue;

            // Work out the direction of the connector
            const QPointList points = connector->connectorPoints(false);

            const int count = points.size();
            if (count < 2)
                continue;

            QPoint end_0 = points[count - 1];
            QPoint end_1 = points[count - 2];

            QPolygon pa;
            if (end_0.x() < end_1.x()) {
                pa = arrowPoints(180);
                pa.translate(4, 0);
            } else if (end_0.x() > end_1.x()) {
                pa = arrowPoints(0);
                pa.translate(-4, 0);
            } else if (end_0.y() < end_1.y()) {
                pa = arrowPoints(270);
                pa.translate(0, 4);
            } else if (end_0.y() > end_1.y()) {
                pa = arrowPoints(90);
                pa.translate(0, -4);
            } else
                continue;

            pa.translate(_x, _y);
            p.setPen(connector->isSelected() ? m_selectedColor : Qt::black);
            p.drawPolygon(pa);
        }
        return;
    }

    if (m_dir == 0)
        p.drawLine(_x, _y, _x - 8, _y);
    else if (m_dir == 90)
        p.drawLine(_x, _y, _x, _y - 8);
    else if (m_dir == 180)
        p.drawLine(_x, _y, _x + 8, _y);
    else if (m_dir == 270)
        p.drawLine(_x, _y, _x, _y + 8);
}
