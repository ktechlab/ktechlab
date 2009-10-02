/***************************************************************************
 *   Copyright (C) 2003-2005 by David Saxton                               *
 *   david@bluehaze.org                                                    *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#ifndef CONNECTOR_H
#define CONNECTOR_H

#include <canvas.h>
#include <qvaluevector.h>

class Cell;
class ConnectorData;
class ConnectorLine;
class ConRouter;
class CNItem;
class ICNDocument;
class Node;
class NodeGroup;

typedef QValueList<ConnectorLine*> ConnectorLineList;
typedef QValueList<QPoint> QPointList;

// TODO: refactor these: 
class Wire;
typedef QValueVector<Wire *> WireVector;
// ###

/**
@short Represents a connection between two Nodes on a ICNDocument
@author David Saxton
*/
class Connector : public QObject, public QCanvasPolygon {
	Q_OBJECT

public:
	Connector(ICNDocument *_ICNDocument, const QString &id = QString() );
	~Connector();

	/**
	 * Node at start of connector (which refers to this as the output connector)
	 */
	virtual Node *startNode() const = 0;
	virtual Node *endNode() const = 0;

	/**
	 * @returns connector data describing this connector
	 */
	ConnectorData connectorData() const;

	/**
	 * Restore the state of the connector (route, etc) from the saved data
	 */
	void restoreFromConnectorData(const ConnectorData &connectorData);

	/**
	 * If selected, will be drawn in a different colour
	 */
	virtual void setSelected(bool yes);

	/**
	 * Connected id
	 */
	QString id() const { return m_id; }

	/**
	 * Update the list of lines and connetion-points that the connector uses for
	 * drawing.
	 */
	void updateDrawList();

	/**
	 * Tells the connector that it is under the control of a NodeGroup. When
	 * the connector is under the control of a NodeGroup, all requests for
	 * connection rerouting will be passed onto that NodeGroup
	 */
	void setNodeGroup(NodeGroup *nodeGroup) { p_nodeGroup = nodeGroup; }

	/**
	 * Returns the NodeGroup that the connector is under the control of (if any)
	 */
	NodeGroup *nodeGroup() const { return p_nodeGroup; }

	/**
	 * ICNDocument needs to know what 'cells' a connector is present in,
	 * so that connection mapping can be done to avoid connectors.
	 * This function will add the hit penalty to the cells pointed to
	 * by ICNDocument::cells()
	 */
	void updateConnectorPoints(bool add);

	/**
	 * Sets the canvas points that the connector should route itself along.
	 * This is used for manual routing. The cells points are absolute positions
	 * (unlike the points stored internally in this class, which are the cell poisition
	 * @param setManual if true then the connector will change to a manual route one
	 * @param checkEndPoints if true then will  check to see if the end points are at the nodes, and adds them if not
	 */
	void setRoutePoints(QPointList pointList, bool setManual, bool checkEndPoints = false);

	/**
	 * Call this function (e.g. when moving a CNItem connected to the connector)
	 * to make the connector partially hidden - probably grayed out - if semiHidden
	 * is true.
	 */
	void setSemiHidden(bool semiHidden);

	/**
	 * @returns whether the points have been set by the user manually defining them
	 */
	bool usesManualPoints() const { return b_manualPoints; }

	/**
	 * Returns two sets of points (in canvas-reference) that define the connector
	 * from start to finish, when it is split at the given point (in canvas-reference)
	 */
	QValueList<QPointList> splitConnectorPoints(const QPoint &pos) const;

	/**
	 * @returns pointer to ICNDocument that this connector is a member of
	 */
	ICNDocument *icnDocument() const { return p_icnDocument; }

	/**
	 * Looks at the set of canvas points and tries to determine whether they are
	 * in the reverse order from start to end node
	 */
	bool pointsAreReverse(const QPointList &pointList) const;

	/**
	 * Returns the points, given in canvas-reference, in order of start node to
	 * end node if reverse is false
	 * @param reverse whether or not to reverse the points from start node to end node
	 */
	QPointList connectorPoints(bool reverse = false) const;

	/**
	 * Reroute the connector. Note that if this connector is controlled by a
	 * NodeGroup, it will do nothing (other than print out a warning)
	 */
	void rerouteConnector();

	/**
	 * Translates the route by the given amoumt. No checking is done to see if
	 * the translation is useful, etc.
	 */
	void translateRoute(int dx, int dy);
	virtual void setVisible(bool yes);

	void updateConnectorLines(bool forceRedraw = false);

signals:
	void removed(Connector *connector);
	void selected(bool yes);

public slots:
	void removeConnector(Node* = 0);

protected:

// TODO: refactor this!!
	WireVector        m_wires;
// ###

	ICNDocument *p_icnDocument;

private:
	bool b_semiHidden;
	bool b_deleted;
	bool b_manualPoints;
	bool b_pointsAdded;

// DEAD CODE ??
	NodeGroup   *p_nodeGroup;
	ConRouter   *m_conRouter;

	QString     m_id;
	QRect       m_oldBoundRect;

	ConnectorLineList m_connectorLineList;
};

#endif

