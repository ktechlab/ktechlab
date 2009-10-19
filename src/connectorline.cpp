//
// C++ Implementation: connectorline
//
// Description: 
//
//
// Author: David Saxton, Alan Grimes <agrimes@speakeasy.net>, (C) 2009
//
// Copyright: See COPYING file that comes with this distribution
//
//

#include <qpainter.h>

#include "connectorline.h"
#include "electronicconnector.h"

//BEGIN class ConnectorLine
ConnectorLine::ConnectorLine(Connector *connector, int pixelOffset)
		: QObject(connector), QCanvasLine(connector->canvas()) {
	m_pConnector = connector;
	m_pixelOffset = pixelOffset;
}

/**
 * @returns x, possibly moving it to the closest bound if it is out of bounds.
 */
int boundify(int x, int bound1, int bound2) {
	if (bound2 < bound1) {
		// swap bounds
		int temp = bound2;
		bound2   = bound1;
		bound1   = temp;
	}

	// now, have bound1 <= bound2
	if (x < bound1)	     return bound1;
	else if (x > bound2) return bound2;
	else	return x;
}

void ConnectorLine::drawShape(QPainter &p) {
	if (!m_bAnimateCurrent) {
		QCanvasLine::drawShape(p);
		return;
	}

	const int ss = 3; // segment spacing
	const int sl = 13; // segment length (includes segment spacing)

	int offset = int(dynamic_cast<ElectronicConnector *>(m_pConnector)->currentAnimationOffset() - m_pixelOffset);
	offset = ((offset % sl) - sl) % sl;

	int x1 = startPoint().x();
	int y1 = startPoint().y();
	int x2 = endPoint().x();
	int y2 = endPoint().y();

	QPen pen = p.pen();
// 	pen.setStyle( Qt::DashDotLine );
	p.setPen(pen);

	if (x1 == x2) {
		int _x = int(x() + x1);
		int y_end = int(y() + y2);

		if (y1 > y2) {
			// up connector line
			for (int _y = int(y() + y1 - offset); _y >= y_end; _y -= sl) {
				int y_1 = boundify(_y, int(y() + y1), y_end);
				int y_2 = boundify(_y - (sl - ss), int(y() + y1), y_end);
				p.drawLine(_x, y_1, _x, y_2);
			}
		} else {
			// down connector line
			for (int _y = int(y() + y1 + offset); _y <= y_end; _y += sl) {
				int y_1 = boundify(_y, int(y() + y1), y_end);
				int y_2 = boundify(_y + (sl - ss), int(y() + y1), y_end);
				p.drawLine(_x, y_1, _x, y_2);
			}
		}
	} else {
		// y1 == y2
		int _y    = int(y() + y1);
		int x_end = int(x() + x2);

		if (x1 > x2) {
			// left connector line
			int x_start = int(x() + x1 - offset);

			for (int _x = x_start; _x >= x_end; _x -= sl) {
				int x_1 = boundify(_x, int(x() + x1), x_end);
				int x_2 = boundify(_x - (sl - ss), int(x() + x1), x_end);
				p.drawLine(x_1, _y, x_2, _y);
			}
		} else {
			// right connector line
			for (int _x = int(x() + x1 + offset); _x <= x_end; _x += sl) {
				int x_1 = boundify(_x, int(x() + x1), x_end);
				int x_2 = boundify(_x + (sl - ss), int(x() + x1), x_end);
				p.drawLine(x_1, _y, x_2, _y);
			}
		}
	}
}

//END class ConnectorLine

