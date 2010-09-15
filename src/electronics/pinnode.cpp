//
// C++ Implementation: pinnode
//
// Description:
//
// Copyright: See COPYING file that comes with this distribution


#include "pinnode.h"
#include "pin.h"
#include "component.h"

#include <qpainter.h>
//Added by qt3to4:
#include <Q3PointArray>
#include <cmath>

#include "voltageappearance.h"

/// The maximum length of the voltage indiactor
const int vLength = 8;

/// The current at the middle of the current indicator
const double iMidPoint = 0.03;

/// The maximum thicnkess of the current indicator
const int iLength = 6;

inline double calcIProp(const double i) {
	return 1 - iMidPoint / (iMidPoint + std::abs(i));
}

inline int calcThickness(const double prop) {
	return (int)((iLength -2)*prop + 2);
}

inline int calcLength(double v) {
	double prop = voltageLength(v);

	if (v > 0)
		prop *= -1.0;

	return int(vLength * prop);
}

PinNode::PinNode(ICNDocument* icnDocument, int dir, const QPoint& pos, QString* id) :
	ECNode(icnDocument, Node::ec_pin, dir, pos, id) {

	m_pinPoint = new Q3CanvasRectangle(0, 0, 3, 3, canvas());
	m_pinPoint->setBrush(Qt::black);
	m_pinPoint->setPen(Qt::black);
}

PinNode::~PinNode() {
	m_pinPoint->setCanvas(0);
	delete m_pinPoint;
}

void PinNode::drawShape(QPainter & p) {
	initPainter(p);

	double v = pin().voltage();
	QColor color = voltageColor(v);
	QPen pen = p.pen();

	if (isSelected())
		pen = m_selectedColor;
	else if (m_bShowVoltageColor)
		pen = color;

	if (m_pinPoint) {
		bool drawDivPoint;
		QPoint divPoint = findConnectorDivergePoint(&drawDivPoint);
		m_pinPoint->setVisible(drawDivPoint);
		m_pinPoint->move(divPoint.x() - 1, divPoint.y() - 1);
		m_pinPoint->setBrush(pen.color());
		m_pinPoint->setPen(pen.color());
	}

	// Now to draw on our current/voltage bar indicators
	int length = calcLength(v);
	if ((numPins() == 1) && m_bShowVoltageBars && length != 0) {
		// we can assume that v != 0 as length != 0
		double i = pin().calculateCurrentFromWires();
		double iProp = calcIProp(i);
		int thickness = calcThickness(iProp);

		p.setPen(QPen(color, thickness));

		// The node line (drawn at the end of this function) will overdraw
		// some of the voltage bar, so we need to adapt the length

		if ((v > 0) && (((225 < m_dir) && (m_dir < 315)) || ((45 < m_dir) && (m_dir < 135))))
			length--;
		else if ((v < 0) && (((135 < m_dir) && (m_dir < 225)) || ((315 < m_dir) || (m_dir < 45))))
			length++;

		if ((m_dir > 270) || (m_dir <= 90))
			p.drawLine(3, 0, 3, length);
		else	p.drawLine(3, 0, 3, -length);
	}

	pen.setWidth((numPins() > 1) ? 2 : 1);
	p.setPen(pen);
	p.drawLine(0, 0, m_length, 0);
	deinitPainter(p);
}

void PinNode::initPoints() {
	int l = - m_length;

	// Bounding rectangle, facing right
	Q3PointArray pa(QRect(0, -8, l, 16));

	QMatrix m;
	m.rotate(m_dir);
	pa = m.map(pa);
	setPoints(pa);
}

