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
#include <qpointer.h>
#include <q3valuevector.h>

class Cell;
class ConnectorData;
class ConnectorLine;
class ConRouter;
class CNItem;
class ICNDocument;
class Node;
class NodeGroup;
class Wire;

typedef QList<ConnectorLine*> ConnectorLineList;
typedef QList<QPoint> QPointList;
typedef Q3ValueVector<QPointer<Wire> > WireVector;


/**
@short Represents a connection between two Nodes on a ICNDocument
@author David Saxton
*/


class Connector : /* public QObject, */ public KtlQCanvasPolygon {
	Q_OBJECT

public:
	Connector(Node *startNode, Node *endNode, ICNDocument *_ICNDocument, QString *id = 0L);
	~Connector();

	/**
	 * Node at start of connector (which refers to this as the output connector)
	 */
	virtual Node *startNode() const = 0;

	/**
	 * Node at end of connector (which refers to this as the input connector)
	 */
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
	 * Sets the container parent (i.e. the container of the parent item)
	 */
	void setParentContainer(const QString &cnItemId);

	/**
	 * Returns a pointer to the parent item container
	 */
	CNItem *parentContainer() const { return p_parentContainer; }

	/**
	 * @returns whether the points have been set by the user manually defining them
	 */
	bool usesManualPoints() const { return b_manualPoints; }

	/**
	 * Returns two sets of points (in canvas-reference) that define the connector
	 * from start to finish, when it is split at the given point (in canvas-reference)
	 */
	QList<QPointList> splitConnectorPoints(const QPoint &pos) const;

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

	/**
	Methods relating to wire lists
	*/
	WireVector wires() const { return m_wires; }
	unsigned numWires() const { return m_wires.size(); }
	Wire *wire(unsigned num = 0) const ; /* {
		return (num < m_wires.size()) ? m_wires[num] : 0;
	} */

	void updateConnectorLines(bool forceRedraw = false);

	/**
	 * Modular offset of moving dots in connector, indicating current (in
	 * pixels).
	 */
	double currentAnimationOffset() const { return m_currentAnimationOffset; }

	/**
	 * Increases the currentAnimationOffset according to the current flowing in
	 * the connector and deltaTime.
	 */
	void incrementCurrentAnimation(double deltaTime);

signals:
	void removed(Connector *connector);
	void selected(bool yes);
	void numWiresChanged(unsigned newNum);

public slots:
	void removeConnector(Node* = 0);

//protected:
//	bool m_bIsSyncingWires;

protected:
	WireVector        m_wires;
		
private:

	bool b_semiHidden;
	bool b_deleted;
	bool b_manualPoints;
	bool b_pointsAdded;

	double m_currentAnimationOffset;

	NodeGroup   *p_nodeGroup;
	CNItem      *p_parentContainer;
	ICNDocument *p_icnDocument;
	ConRouter   *m_conRouter;

	QString           m_id;
	QRect             m_oldBoundRect;

	ConnectorLineList m_connectorLineList;
};

typedef QList<QPointer<Connector> > ConnectorList;

//BEGIN ConnectorLine things

class ConnectorLine : /* public QObject, */ public KtlQCanvasLine {

public:
	/**
	 * @param pixelOffset the number of pixels between the start of the
	 * parent connector and the start of this wire. Used in current
	 * animation.
	 */
	ConnectorLine(Connector *connector, int pixelOffset);
	Connector *parent() const { return m_pConnector; }

	void setAnimateCurrent(bool animateCurrent) { m_bAnimateCurrent = animateCurrent; }

protected:
	virtual void drawShape(QPainter &p);

	Connector *m_pConnector;
	int  m_pixelOffset;
	bool m_bAnimateCurrent;
};

//END ConnectorLine things

#endif

