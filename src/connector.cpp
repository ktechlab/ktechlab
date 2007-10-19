/***************************************************************************
 *   Copyright (C) 2003-2005 by David Saxton                               *
 *   david@bluehaze.org                                                    *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#include "circuitdocument.h"
#include "connector.h"
#include "conrouter.h"
#include "cnitem.h"
#include "ecnode.h"
#include "itemdocumentdata.h"
#include "wire.h"

#include <kdebug.h>
#include <cmath>
#include <cassert>

// To/from canvas utility funcs ##################

inline static int toCanvas( int pos)
{
	return (pos<<3) | 4;
}

inline static int fromCanvas( int pos)
{
	return (pos-4) >> 3;
}

inline static QPoint toCanvas(const QPoint *const pos)
{
	return QPoint(toCanvas(pos->x()), toCanvas(pos->y()));
}

inline static QPoint fromCanvas(const QPoint *const pos)
{
	return QPoint(fromCanvas(pos->x()), fromCanvas(pos->y()));
}

inline static QPoint toCanvas(const QPoint &pos)
{
	return QPoint(toCanvas(pos.x()), toCanvas(pos.y()));
}

inline static QPoint fromCanvas(const QPoint &pos)
{
	return QPoint(fromCanvas(pos.x()), fromCanvas(pos.y()));
}

// ###########################

//BEGIN class Connector
Connector::Connector(Node *startNode, Node *endNode, ICNDocument *icnDocument, QString *id)
	: QObject(icnDocument),
	  QCanvasPolygon(icnDocument->canvas()),
	  p_icnDocument(icnDocument),
	  m_startNode(startNode),
	  m_endNode(endNode),
	  p_nodeGroup(0),
	  b_semiHidden(false),
	  b_deleted(false),
	  b_pointsAdded(false),
	  b_manualPoints(false)
{
	m_conRouter = new ConRouter(p_icnDocument);

	if(id) {
		m_id = *id;
		if( !p_icnDocument->registerUID(*id)) {
// 			kdDebug() << k_funcinfo << "KTechlab: Connector attempted to register given ID, but ID already in use"<<endl;
		}
	} else m_id = p_icnDocument->generateUID("connector");

	p_icnDocument->registerItem(this);
	p_icnDocument->requestRerouteInvalidatedConnectors();
	setVisible(true);

	ECNode *startECNode = dynamic_cast<ECNode*>(startNode);
	ECNode *endECNode = dynamic_cast<ECNode*>(endNode);
	if( startECNode && endECNode) {
		connect( startECNode, SIGNAL(numPinsChanged(unsigned)), this, SLOT(syncWiresWithNodes()));
		connect( endECNode, SIGNAL(numPinsChanged(unsigned)), this, SLOT(syncWiresWithNodes()));
		syncWiresWithNodes();
	}
}

Connector::~Connector()
{
	p_icnDocument->unregisterUID( id());
	delete m_conRouter;

	for( unsigned i = 0; i < m_wires.size(); i++) delete m_wires[i];

	m_wires.resize(0);
}

int Connector::rtti() const
{
	return ItemDocument::RTTI::Connector;
}

void Connector::syncWiresWithNodes()
{
	ECNode *startECNode = dynamic_cast<ECNode*>((Node*)m_startNode);
	ECNode *endECNode = dynamic_cast<ECNode*>((Node*)m_endNode);

	if(!startECNode || !endECNode) return;

	unsigned newNumWires = 0;

	if(startECNode->type() == Node::ec_junction ||
			endECNode->type() == Node::ec_junction)
		newNumWires = QMAX(startECNode->numPins(), endECNode->numPins());
	else newNumWires = QMIN(startECNode->numPins(), endECNode->numPins());

	unsigned oldNumWires = m_wires.size();

	if(newNumWires == oldNumWires) return;

	if(startECNode->type() == Node::ec_junction)
		startECNode->setNumPins(newNumWires);
	if(endECNode->type() == Node::ec_junction)
		endECNode->setNumPins(newNumWires);

	if(newNumWires > oldNumWires) {
		m_wires.resize(newNumWires);
		for( unsigned i = oldNumWires; i < newNumWires; i++) {
			if( startECNode->pin(i) && endECNode->pin(i))
				m_wires[i] = new Wire( startECNode->pin(i), endECNode->pin(i));
		}
	} else {
		for( unsigned i = newNumWires; i < oldNumWires; i++)
			delete m_wires[i];
		m_wires.resize(newNumWires);
	}

	updateConnectorLines();
	emit numWiresChanged(newNumWires);
}

// are we supposed to do something with the parameter or is it just cruft?
void Connector::removeConnector(Node *foo)
{
	if(b_deleted) return;

	b_deleted = true;

	// Remove 'penalty' points for this connector from the ICNDocument
	updateConnectorPoints(false);

	emit selected(false);
	emit removed(this);
	if(m_startNode) m_startNode->removeConnector(this);
	if(m_endNode) m_endNode->removeConnector(this);
	p_icnDocument->appendDeleteList(this);
}

int getSlope(float x1, float y1, float x2, float y2)
{
	enum slope {
		s_n = 0,//	.
		s_v,	//	|
		s_h,	//	-
		s_s,	//	/
		s_d	//	\ (backwards slash)
	};

	if( x1 == x2) {
		if( y1 == y2) return s_n;
		return s_v;
	};

	if( y1 == y2) return s_h;
	if((y2-y1)/(x2-x1) > 0)return s_s;

	return s_d;
}

void Connector::updateDrawList()
{
	if(!m_startNode || !m_endNode || !canvas()) return;

	QPointList drawLineList;

	int prevX = (*m_conRouter->cellPointList()->begin()).x();
	int prevY = (*m_conRouter->cellPointList()->begin()).y();

	int prevX_canvas = toCanvas(prevX);
	int prevY_canvas = toCanvas(prevY);

	Cells *cells = p_icnDocument->cells();

	bool bumpNow = false;
	const QPointList::const_iterator cplEnd = m_conRouter->cellPointList()->end();
	for( QPointList::const_iterator it = m_conRouter->cellPointList()->begin(); it != cplEnd; ++it) {
		const int x = (*it).x();
		const int y = (*it).y();
		const int numCon = p_icnDocument->isValidCellReference(x,y) ? (*cells)[x][y].numCon : 0;

		const int y_canvas = toCanvas(y);
		const int x_canvas = toCanvas(x);

		const bool bumpNext = (prevX == x && numCon > 1 &&
			std::abs(y_canvas-m_startNode->y())>8 &&
			std::abs(y_canvas-m_endNode->y())>8);

		int x0 = prevX_canvas;
		int x2 = x_canvas;
		int x1 = (x0+x2)/2;

		int y0 = prevY_canvas;
		int y3 = y_canvas;
		int y1 = ( y0 == y3) ? y0 : ((y0<y3) ? y0+3 : y0-3);
		int y2 = ( y0 == y3) ? y3 : ((y0<y3) ? y3-3 : y3+3);

		if(bumpNow) x0 += 3;

		if(bumpNext) x2 += 3;

// FIXME: problems with this code but I don't know what it should be doing.
		else if(!bumpNow && !bumpNext) {
			drawLineList += QPoint( x0, y0);
			drawLineList += QPoint( x2, y3);
		} else if(bumpNow) {
			drawLineList += QPoint( x0, y0);
			drawLineList += QPoint( x1, y1);
			drawLineList += QPoint( x2, y3);
		} else if(bumpNext) {
			drawLineList += QPoint( x0, y0);
			drawLineList += QPoint( x1, y2);
			drawLineList += QPoint( x2, y3);
		} else {
assert(false); // I don't think this code is reachable. =P
			drawLineList += QPoint( x0, y0);
			drawLineList += QPoint( x1, y1);
			drawLineList += QPoint( x1, y2);
			drawLineList += QPoint( x2, y3);
		}

		prevX = x;
		prevY = y;
		prevY_canvas = y_canvas;
		prevX_canvas = x_canvas;
		bumpNow = bumpNext;
	}
	
	// Now, remove redundant points (i.e. those that are either repeated or are
	// in the same direction as the previous points)

	if(drawLineList.size() < 3) return;

	const QPointList::iterator dllEnd = drawLineList.end();
	QPointList::iterator previous = drawLineList.begin();
	QPointList::iterator current = previous;
	current++;
	QPointList::const_iterator next = current;
	next++;

	while(previous != dllEnd && current != dllEnd && next != dllEnd) {
		const int slope1 = getSlope((*previous).x(), (*previous).y(), (*current).x(), (*current).y());
		const int slope2  = getSlope((*current).x(), (*current).y(), (*next).x(), (*next).y());

		if(slope1 == slope2 || slope1 == 0 || slope2 == 0) {
			*current = QPoint(-1, -1);
		} else previous = current;

		current++;
		next++;
	}

	drawLineList.remove(QPoint(-1, -1));

	// Find the bounding rect
	{
		int x1=-1, y1=-1, x2=-1, y2=-1;
		const QPointList::iterator end = drawLineList.end();
		for( QPointList::iterator it = drawLineList.begin(); it != end; ++it) {
			const QPoint p = *it;
			if( p.x() < x1 || x1 == -1) {
				x1 = p.x();
			}

			if( p.x() > x2 || x2 == -1) {
				x2 = p.x();
			}

			if( p.y() < y1 || y1 == -1) {
				y1 = p.y();
			}

			if( p.y() > y2 || y2 == -1) {
				y2 = p.y();
			}
		}

		QRect boundRect( x1, y1, x2-x1, y2-y1);
		if(boundRect != m_oldBoundRect) {
			canvas()->setChanged( boundRect | m_oldBoundRect);
			m_oldBoundRect = boundRect;
		}
	}

	//BEGIN build up ConnectorLine list
	const ConnectorLineList::iterator ConnectorLineEnd = m_connectorLineList.end();
	for( ConnectorLineList::iterator it = m_connectorLineList.begin(); it != ConnectorLineEnd; ++it)
		delete *it;
	m_connectorLineList.clear();

	if( drawLineList.size() > 1) {
		QPoint prev = drawLineList.first();
		const QPointList::iterator end = drawLineList.end();
		for(QPointList::iterator it = ++drawLineList.begin(); it != end; ++it) {
			const QPoint next = *it;
			ConnectorLine *line = new ConnectorLine(this);
			line->setPoints( prev.x(), prev.y(), next.x(), next.y());
			m_connectorLineList.append(line);
			prev = next;
		}
	}
	updateConnectorLines();
	//END build up ConnectorPoint list
}

void Connector::setSemiHidden(bool semiHidden)
{
	if(!canvas() || semiHidden == b_semiHidden) return;

	b_semiHidden = semiHidden;
	updateConnectorLines();
}

void Connector::updateConnectorPoints( bool add)
{
	if(!canvas()) return;
	if(b_deleted || !isVisible()) add = false;

	// Check we haven't already added/removed the points...
	if( b_pointsAdded == add) return;

	b_pointsAdded = add;

	// We don't include the end points in the mapping
	if(m_conRouter->cellPointList()->size() < 3) return;

	const int mult = (add)?1:-1;
	const QPointList::iterator end = --m_conRouter->cellPointList()->end();
	for(QPointList::iterator it = ++m_conRouter->cellPointList()->begin(); it != end; ++it) {
		int x = (*it).x();
		int y = (*it).y();

		// Add the points of this connector to the cell array in the ICNDocument,
		// so that other connectors still to calculate their points know to try
		// and avoid this connector

		p_icnDocument->addCPenalty(x, y-1, mult*ICNDocument::hs_connector/2);
		p_icnDocument->addCPenalty(x-1, y, mult*ICNDocument::hs_connector/2);
		p_icnDocument->addCPenalty(x, y, mult*ICNDocument::hs_connector);
		p_icnDocument->addCPenalty(x+1, y, mult*ICNDocument::hs_connector/2);
		p_icnDocument->addCPenalty(x, y+1, mult*ICNDocument::hs_connector/2);

		if( p_icnDocument->isValidCellReference( x, y)) {
			(*p_icnDocument->cells())[x][y].numCon += mult;
		}
	}

// 	updateDrawList();
}


void Connector::setRoutePoints( QPointList pointList, bool setManual, bool checkEndPoints)
{
	if(!canvas()) return;

	updateConnectorPoints(false);

	bool reversed = pointsAreReverse(pointList);
	if(checkEndPoints) {
		if(reversed) {
			pointList.prepend( QPoint( int(m_endNode->x()), int(m_endNode->y())));
			pointList.append( QPoint( int(m_startNode->x()), int(m_startNode->y())));
		} else {
			pointList.prepend( QPoint( int(m_startNode->x()), int(m_startNode->y())));
			pointList.append( QPoint( int(m_endNode->x()), int(m_endNode->y())));
		}
	}

	m_conRouter->setPoints( pointList, reversed);
	b_manualPoints = setManual;
	updateConnectorPoints(true);
}

bool Connector::pointsAreReverse( const QPointList &pointList) const
{
	if(!m_startNode || !m_endNode) {
		kdWarning() << k_funcinfo << "Cannot determine orientation as no start and end nodes" << endl;
		return false;
	}

	if(pointList.isEmpty()) return false;

	int plsx = pointList.first().x();
	int plsy = pointList.first().y();
	int plex = pointList.last().x();
	int pley = pointList.last().y();

	double nsx = m_startNode->x();
	double nsy = m_startNode->y();
	double nex = m_endNode->x();
	double ney = m_endNode->y();

	double dist_normal =  (nsx-plsx)*(nsx-plsx) + (nsy-plsy)*(nsy-plsy) + (nex-plex)*(nex-plex) + (ney-pley)*(ney-pley);
	double dist_reverse = (nsx-plex)*(nsx-plex) + (nsy-pley)*(nsy-pley) + (nex-plsx)*(nex-plsx) + (ney-plsy)*(ney-plsy);

	return dist_reverse < dist_normal;
}

void Connector::rerouteConnector()
{
	if(!isVisible()) return;

	if(nodeGroup()) {
		kdWarning() << k_funcinfo << "Connector is controlled by a NodeGroup! Use that to reroute the connector" << endl;
		return;
	}

	if(!startNode() || !endNode()) return;

	updateConnectorPoints(false);
	m_conRouter->mapRoute( int(startNode()->x()), int(startNode()->y()), int(endNode()->x()), int(endNode()->y()));
	b_manualPoints = false;
	updateConnectorPoints(true);
}

void Connector::translateRoute( int dx, int dy)
{
	updateConnectorPoints(false);
	m_conRouter->translateRoute(dx, dy);
	updateConnectorPoints(true);
	updateDrawList();
}

void Connector::restoreFromConnectorData( const ConnectorData &connectorData)
{
	updateConnectorPoints(false);
	b_manualPoints = connectorData.manualRoute;
	m_conRouter->setRoutePoints(connectorData.route);
	updateConnectorPoints(true);
	updateDrawList();
}

ConnectorData Connector::connectorData() const
{
	ConnectorData connectorData;
	if(!m_startNode || !m_endNode) {
		kdDebug() << k_funcinfo << " m_startNode="<<m_startNode<<" m_endNode="<<m_endNode<<endl;
		return connectorData;
	}

	connectorData.manualRoute = usesManualPoints();
	connectorData.route = *m_conRouter->cellPointList();

	if(m_startNode->isChildNode()) {
		connectorData.startNodeIsChild = true;
		connectorData.startNodeCId = m_startNode->childId();
		connectorData.startNodeParent = m_startNode->parentItem()->id();
	} else {
		connectorData.startNodeIsChild = false;
		connectorData.startNodeId = m_startNode->id();
	}

	if(m_endNode->isChildNode()) {
		connectorData.endNodeIsChild = true;
		connectorData.endNodeCId = m_endNode->childId();
		connectorData.endNodeParent = m_endNode->parentItem()->id();
	} else {
		connectorData.endNodeIsChild = false;
		connectorData.endNodeId = m_endNode->id();
	}

	return connectorData;
}

void Connector::setVisible(bool yes)
{
	if(!canvas() || isVisible() == yes) return;

	QCanvasPolygon::setVisible(yes);
	updateConnectorLines();
}

void Connector::setSelected(bool yes)
{
	if(!canvas() || isSelected() == yes) return;

	QCanvasPolygon::setSelected(yes);
	updateConnectorLines();
	emit selected(yes);
}

void Connector::updateConnectorLines()
{
	const QColor color = b_semiHidden ? Qt::gray : (isSelected() ? QColor( 101, 134, 192) : Qt::black);
// 	const QColor color = b_semiHidden ? Qt::gray : (isSelected() ? QColor(0x7f, 0x7f, 0xff) : Qt::black);
	const int z = ICNDocument::Z::Connector + (isSelected() ? 5 : 0);

	QPen pen(color, (numWires() > 1) ? 2 : 1);

	const ConnectorLineList::iterator end = m_connectorLineList.end();
	for(ConnectorLineList::iterator it = m_connectorLineList.begin(); it != end; ++it) {
		QCanvasPolygonalItem *item = static_cast<QCanvasPolygonalItem*>(*it);
		item->setZ(z);
		item->setPen(pen);
		item->setBrush(color);
		item->setVisible( isVisible());
	}
}

QValueList<QPointList> Connector::splitConnectorPoints(const QPoint &pos) const
{
	return m_conRouter->splitPoints(pos);
}

QPointList Connector::connectorPoints( bool reverse) const
{
	bool doReverse = (reverse != pointsAreReverse( m_conRouter->pointList(false)));
	return m_conRouter->pointList(doReverse);
}

//END class Connector

//BEGIN class ConnectorLine
ConnectorLine::ConnectorLine( Connector *connector)
	: QObject(connector), QCanvasLine( connector->canvas())
{
	p_connector = connector;
}

int ConnectorLine::rtti() const
{
	return ICNDocument::RTTI::ConnectorLine;
}

//END class ConnectorLine

#include "connector.moc"
