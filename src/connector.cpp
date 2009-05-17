/***************************************************************************
 *   Copyright (C) 2003-2005 by David Saxton                               *
 *   david@bluehaze.org                                                    *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#include <cstdlib>
#include <cmath>
#include <kdebug.h>

#include "connector.h"
#include "conrouter.h"
#include "cnitem.h"
#include "itemdocumentdata.h"
#include "src/core/ktlconfig.h"
#include "utils.h"
#include "icndocument.h"
#include "node.h"

// ### to be refactored:
#include "wire.h"
// ###

#include "ktlcanvas.h"
#include "connectorline.h"

// including only this is fairly inoccuous.
#include "voltageappearance.h" 

//BEGIN class Connector
Connector::Connector(ICNDocument *icnDocument, const QString &id)
		: QObject(icnDocument),
		QCanvasPolygon(icnDocument->canvas()) {
	m_currentAnimationOffset = 0.0;
	p_nodeGroup    = 0;
	b_semiHidden   = false;
	b_deleted      = false;
	b_pointsAdded  = false;
	b_manualPoints = false;
	p_icnDocument  = icnDocument;
	m_conRouter    = new ConRouter(p_icnDocument);

	if (!id.isEmpty()) {
		m_id = id;

		if (!p_icnDocument->registerUID(id)) {
			kdDebug() << k_funcinfo << "Connector attempted to register given ID, but ID already in use: " << id << endl;
			m_id = p_icnDocument->generateUID(id);
			kdDebug() << "Creating a new one: " << m_id << endl;
		}
	} else m_id = p_icnDocument->generateUID("connector");

	p_icnDocument->registerItem(this);
	p_icnDocument->requestRerouteInvalidatedConnectors();

	setVisible(true);
}

Connector::~Connector() {
	p_icnDocument->unregisterUID(id());

	delete m_conRouter;
}

void Connector::removeConnector(Node*) {
	if (b_deleted) return;

	b_deleted = true;

	// Remove 'penalty' points for this connector from the ICNDocument
	updateConnectorPoints(false);

	emit selected(false);
	emit removed(this);

	if (startNode()) startNode()->removeConnector(this);
	if (endNode())	 endNode()->removeConnector(this);

	p_icnDocument->appendDeleteList(this);
}

int getSlope(float x1, float y1, float x2, float y2) {
	enum slope {
		s_n = 0,//	.
		s_v,	//	|
		s_h,	//	-
		s_s,	//	/
		s_d	//	\ (backwards slash)
	};

	if (x1 == x2) {
		if (y1 == y2) return s_n;

		return s_v;
	} else if (y1 == y2) {
		return s_h;
	} else if ((y2 - y1) / (x2 - x1) > 0) {
		return s_s;
	} else 	return s_d;
}

void Connector::updateDrawList() {
	if (!startNode() || !endNode() || !canvas()) return;

	QPointList drawLineList;

	int prevX = (*m_conRouter->cellPointList()->begin()).x();
	int prevY = (*m_conRouter->cellPointList()->begin()).y();
	int prevX_canvas = toCanvas(prevX);
	int prevY_canvas = toCanvas(prevY);
	Cells *cells = p_icnDocument->cells();
	bool bumpNow = false;

	const QPointList::const_iterator cplEnd = m_conRouter->cellPointList()->end();
	for (QPointList::const_iterator it = m_conRouter->cellPointList()->begin(); it != cplEnd; ++it) {
		const int x = (*it).x();
		const int y = (*it).y();
		const int numCon = cells->haveCell(x, y) ? cells->cell(x, y).getNumCon() : 0;

		const int y_canvas = toCanvas(y);
		const int x_canvas = toCanvas(x);

		const bool bumpNext = (prevX == x
		                       && numCon > 1
		                       && std::abs(y_canvas - startNode()->y()) > 8
		                       && std::abs(y_canvas - endNode()->y())   > 8);

		int x0 = prevX_canvas;
		int x2 = x_canvas;
		int x1 = (x0 + x2) / 2;

		int y0 = prevY_canvas;
		int y3 = y_canvas;
		int y1 = (y0 == y3) ? y0 : ((y0 < y3) ? y0 + 3 : y0 - 3);
		int y2 = (y0 == y3) ? y3 : ((y0 < y3) ? y3 - 3 : y3 + 3);

		if (bumpNow)  x0 += 3;
		if (bumpNext) x2 += 3;

		if (!bumpNow && !bumpNext) {
			drawLineList += QPoint(x0, y0);
			drawLineList += QPoint(x2, y3);
		} else if (bumpNow) {
			drawLineList += QPoint(x0, y0);
			drawLineList += QPoint(x1, y1);
			drawLineList += QPoint(x2, y3);
		} else if (bumpNext) {
			drawLineList += QPoint(x0, y0);
			drawLineList += QPoint(x1, y2);
			drawLineList += QPoint(x2, y3);
		} else {
			drawLineList += QPoint(x0, y0);
			drawLineList += QPoint(x1, y1);
			drawLineList += QPoint(x1, y2);
			drawLineList += QPoint(x2, y3);
		}

		prevX = x;
		prevY = y;

		prevY_canvas = y_canvas;
		prevX_canvas = x_canvas;
		bumpNow = bumpNext;
	}

	// Now, remove redundant points (i.e. those that are either repeated or are
	// in the same direction as the previous points)

	if (drawLineList.size() < 3) return;

	const QPointList::iterator dllEnd = drawLineList.end();
	QPointList::iterator previous = drawLineList.begin();

	QPointList::iterator current = previous;
	current++;

	QPointList::const_iterator next = current;
	next++;

	int invalid = -(1 << 30);

	while (previous != dllEnd && current != dllEnd && next != dllEnd) {
		const int slope1 = getSlope((*previous).x(), (*previous).y(), (*current).x(), (*current).y());
		const int slope2  = getSlope((*current).x(), (*current).y(), (*next).x(), (*next).y());

		if (slope1 == slope2 || slope1 == 0 || slope2 == 0) {
			*current = QPoint(invalid, invalid);
		} else 	previous = current;

		current++;
		next++;
	}

	drawLineList.remove(QPoint(invalid, invalid));

	// Find the bounding rect
	{
		int x1 = invalid, y1 = invalid, x2 = invalid, y2 = invalid;
		const QPointList::iterator end = drawLineList.end();

		for (QPointList::iterator it = drawLineList.begin(); it != end; ++it) {
			const QPoint p = *it;

			if (p.x() < x1 || x1 == invalid) x1 = p.x();
			if (p.x() > x2 || x2 == invalid) x2 = p.x();
			if (p.y() < y1 || y1 == invalid) y1 = p.y();
			if (p.y() > y2 || y2 == invalid) y2 = p.y();
		}

		QRect boundRect(x1, y1, x2 - x1, y2 - y1);

		if (boundRect != m_oldBoundRect) {
			canvas()->setChanged(boundRect | m_oldBoundRect);
			m_oldBoundRect = boundRect;
		}
	}

	//BEGIN build up ConnectorLine list
	const ConnectorLineList::iterator ConnectorLineEnd = m_connectorLineList.end();

	for (ConnectorLineList::iterator it = m_connectorLineList.begin(); it != ConnectorLineEnd; ++it)
		delete *it;

	m_connectorLineList.clear();

	if (drawLineList.size() > 1) {
		QPoint prev = drawLineList.first();
		int pixelOffset = 0;
		const QPointList::iterator end = drawLineList.end();

		for (QPointList::iterator it = ++drawLineList.begin(); it != end; ++it) {
			const QPoint next = *it;

			ConnectorLine *line = new ConnectorLine(this, pixelOffset);
			m_connectorLineList.append(line);

			line->setPoints(prev.x(), prev.y(), next.x(), next.y());

			// (note that only one of the following QABS will be non-zero)
			pixelOffset += abs(prev.x() - next.x()) + abs(prev.y() - next.y());

			prev = next;
		}
	}

	updateConnectorLines();
	//END build up ConnectorPoint list
}

void Connector::setSemiHidden(bool semiHidden) {
	if (!canvas() || semiHidden == b_semiHidden)
		return;

	b_semiHidden = semiHidden;

	updateConnectorLines();
}

void Connector::updateConnectorPoints(bool add) {
	if (!canvas()) return;

	if (b_deleted || !isVisible()) add = false;

	// Check we haven't already added/removed the points...
	if (b_pointsAdded == add) return;

	b_pointsAdded = add;

	// We don't include the end points in the mapping
	if (m_conRouter->cellPointList()->size() < 3) return;

	Cells *cells = p_icnDocument->cells();

	const int mult = (add) ? 1 : -1;

	const QPointList::iterator end = --m_conRouter->cellPointList()->end();
	for (QPointList::iterator it = ++m_conRouter->cellPointList()->begin(); it != end; ++it) {
		int x = (*it).x();
		int y = (*it).y();

		// Add the points of this connector to the cell array in the ICNDocument,
		// so that other connectors still to calculate their points know to try
		// and avoid this connector

		p_icnDocument->addCPenalty(x, y - 1, mult*ICNDocument::hs_connector / 2);
		p_icnDocument->addCPenalty(x - 1, y, mult*ICNDocument::hs_connector / 2);
		p_icnDocument->addCPenalty(x, y, mult * ICNDocument::hs_connector);
		p_icnDocument->addCPenalty(x + 1, y, mult*ICNDocument::hs_connector / 2);
		p_icnDocument->addCPenalty(x, y + 1, mult*ICNDocument::hs_connector / 2);

		if (cells->haveCell(x , y))
			cells->cell(x, y).addConnectors(mult);
	}
}

void Connector::setRoutePoints(QPointList pointList, bool setManual, bool checkEndPoints) {
	if (!canvas())	return;

	updateConnectorPoints(false);

	bool reversed = pointsAreReverse(pointList);

	// a little performance boost: don't call (start|end)Node 4 times
	Node* l_endNode = endNode();

	Node* l_startNode = startNode();

	if (checkEndPoints) {
		if (reversed) {
			pointList.prepend(QPoint(int(l_endNode->x()),  int(l_endNode->y())));
			pointList.append(QPoint(int(l_startNode->x()), int(l_startNode->y())));
		} else {
			pointList.prepend(QPoint(int(l_startNode->x()), int(l_startNode->y())));
			pointList.append(QPoint(int(l_endNode->x()),    int(l_endNode->y())));
		}
	}

	m_conRouter->setPoints(pointList, reversed);

	b_manualPoints = setManual;
	updateConnectorPoints(true);
}

bool Connector::pointsAreReverse(const QPointList &pointList) const {
	if (!startNode() || !endNode()) {
		kdWarning() << k_funcinfo << "Cannot determine orientation as no start and end nodes" << endl;
		return false;
	}

	if (pointList.isEmpty()) return false;

	int plsx = pointList.first().x();
	int plsy = pointList.first().y();
	int plex =  pointList.last().x();
	int pley =  pointList.last().y();

	double nsx = startNode()->x();
	double nsy = startNode()->y();
	double nex =   endNode()->x();
	double ney =   endNode()->y();

	double dist_normal = (nsx - plsx) * (nsx - plsx)
	                     + (nsy - plsy) * (nsy - plsy)
	                     + (nex - plex) * (nex - plex)
	                     + (ney - pley) * (ney - pley);

	double dist_reverse = (nsx - plex) * (nsx - plex)
	                      + (nsy - pley) * (nsy - pley)
	                      + (nex - plsx) * (nex - plsx)
	                      + (ney - plsy) * (ney - plsy);

	return dist_reverse < dist_normal;
}

void Connector::rerouteConnector() {
	if (!isVisible()) return;

	if (nodeGroup()) {
		kdWarning() << k_funcinfo << "Connector is controlled by a NodeGroup! Use that to reroute the connector" << endl;
		return;
	}

	if (!startNode() || !endNode()) return;

	updateConnectorPoints(false);
	m_conRouter->mapRoute(int(startNode()->x()),
	                      int(startNode()->y()),
	                      int(endNode()->x()),
	                      int(endNode()->y()));

	b_manualPoints = false;
	updateConnectorPoints(true);
}

void Connector::translateRoute(int dx, int dy) {
	updateConnectorPoints(false);
	m_conRouter->translateRoute(dx, dy);
	updateConnectorPoints(true);
	updateDrawList();
}

void Connector::restoreFromConnectorData(const ConnectorData &connectorData) {
	updateConnectorPoints(false);
	b_manualPoints = connectorData.manualRoute;
	m_conRouter->setRoutePoints(connectorData.route);
	updateConnectorPoints(true);
	updateDrawList();
}

ConnectorData Connector::connectorData() const {
	ConnectorData connectorData;

	if (!startNode() || !endNode()) {
		kdDebug() << k_funcinfo << " m_startNode=" << startNode() << " m_endNode=" << endNode() << endl;
		return connectorData;
	}

	connectorData.manualRoute = usesManualPoints();

	connectorData.route = *m_conRouter->cellPointList();

	if (startNode()->isChildNode()) {
		connectorData.startNodeIsChild = true;
		connectorData.startNodeCId = startNode()->childId();
		connectorData.startNodeParent = startNode()->parentItem()->id();
	} else {
		connectorData.startNodeIsChild = false;
		connectorData.startNodeId = startNode()->id();
	}

	if (endNode()->isChildNode()) {
		connectorData.endNodeIsChild = true;
		connectorData.endNodeCId = endNode()->childId();
		connectorData.endNodeParent = endNode()->parentItem()->id();
	} else {
		connectorData.endNodeIsChild = false;
		connectorData.endNodeId = endNode()->id();
	}

	return connectorData;
}

void Connector::setVisible(bool yes) {
	if (!canvas() || isVisible() == yes) return;

	QCanvasPolygon::setVisible(yes);
	updateConnectorLines();
}

void Connector::setSelected(bool yes) {
	if (!canvas() || isSelected() == yes) return;

	QCanvasPolygon::setSelected(yes);
	updateConnectorLines();
	emit selected(yes);
}

void Connector::updateConnectorLines(bool forceRedraw) {
	if(b_deleted) return;

	QColor color;

	if (b_semiHidden) color = Qt::gray;
	else if (isSelected()) color = QColor(101, 134, 192);
	else if (!KTLConfig::showVoltageColor()) color = Qt::black;
	else if(m_wires.size()) {
		if(m_wires[0]) {
			color = voltageColor(m_wires[0]->voltage());
		} else {
			m_wires.resize(0);
			color = Qt::black;
		}
	} else {
		color = Qt::black;
	}

	int z = ICNDocument::Z::Connector + (isSelected() ? 5 : 0);

	QPen pen(color, (m_wires.size() > 1) ? 2 : 1);

	bool animateWires = KTLConfig::animateWires();

	ConnectorLineList::iterator end = m_connectorLineList.end();
	for (ConnectorLineList::iterator it = m_connectorLineList.begin(); it != end; ++it) {
		(*it)->setAnimateCurrent(animateWires);

		QCanvasPolygonalItem *item = static_cast<QCanvasPolygonalItem*>(*it);

		bool changed = (item->z() != z)
		               || (item->pen() != pen)
		               || (item->isVisible() != isVisible());

		if (!changed) {
			if (forceRedraw)
				canvas()->setChanged(item->boundingRect());

			continue;
		}

		item->setZ(z);
		item->setPen(pen);
		item->setVisible(isVisible());
	}
}

QValueList<QPointList> Connector::splitConnectorPoints(const QPoint & pos) const {
	return m_conRouter->splitPoints(pos);
}

QPointList Connector::connectorPoints(bool reverse) const {
	bool doReverse = (reverse != pointsAreReverse(m_conRouter->pointList(false)));
	return m_conRouter->pointList(doReverse);
}

void Connector::incrementCurrentAnimation(double deltaTime) {
	// The values and equations used in this function have just been developed
	// empircally to be able to show a nice range of currents while still giving
	// a good indication of the amount of current flowing

	double I_min = 1e-4;
	double sf    = 3.0; // scaling factor

	for (unsigned i = 0; i < m_wires.size(); ++i) {
		if (!m_wires[i]) continue;

		double I = m_wires[i]->current();
		double sign  = (I > 0) ? 1 : -1;
		double I_abs = I * sign;
		double prop  = (I_abs > I_min) ? std::log(I_abs / I_min) : 0.0;

		m_currentAnimationOffset += deltaTime * sf * std::pow(prop, 1.3) * sign;
	}
}
//END class Connector

#include "connector.moc"

