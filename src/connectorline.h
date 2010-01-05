//
// C++ Interface: connectorline
//
// Description: 
//
//
// Author: David Saxton, Alan Grimes <agrimes@speakeasy.net>, (C) 2009
//
// Copyright: See COPYING file that comes with this distribution
//
//

#ifndef CONNECTORLINE_H
#define CONNECTORLINE_H

#include <qobject.h>
#include <canvas.h>

class Connector;

class ConnectorLine : public QObject, public QCanvasLine {

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

#endif