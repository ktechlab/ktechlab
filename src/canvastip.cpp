//
// C++ Implementation: canvastip
//
// Description:
//
// Author: David Saxton, Alan Grimes <agrimes@speakeasy.net>, (C) 2009
//
// Copyright: See COPYING file that comes with this distribution
//


#include <cmath>

#include <qpainter.h>
#include <qapplication.h>

#include "canvastip.h"
#include "icndocument.h"
#include "ecnode.h"
#include "electronicconnector.h"
#include "circuitdocument.h"
#include "simulator.h"
#include "cnitem.h"

//BEGIN class CanvasTip
CanvasTip::CanvasTip(ItemDocument *itemDocument, Q3Canvas *qcanvas)
		: Q3CanvasRectangle(qcanvas) {
	p_itemDocument = itemDocument;

	setZ(ICNDocument::Z::Tip);
}

CanvasTip::~CanvasTip() {
}

void CanvasTip::displayVI(ECNode *node, const QPoint &pos) {
	if (!node || !updateVI())
		return;

	unsigned num = node->numPins();

	info.resize(num);

	for (unsigned i = 0; i < num; i++) {
		if (Pin *pin = &node->pin(i)) {
			info[i].V = pin->voltage();
			info[i].I = std::abs(pin->calculateCurrentFromWires());
			info[i].id = pin->eqId();
		}
	}

	display(pos);
}

void CanvasTip::displayVI(Connector *connector, const QPoint &pos) {
	if(!updateVI()) return;

	ElectronicConnector *econnector = dynamic_cast<ElectronicConnector*>(connector);

	if(!econnector) return; 

	unsigned num = econnector->numWires();

	info.resize(num);

	for (unsigned i = 0; i < num; i++) {
		if (Wire *wire = econnector->wire(i)) {
			info[i].V = wire->voltage();
			info[i].I = std::abs(wire->current());
			info[i].id = -2;
		}
	}

	display(pos);
}

bool CanvasTip::updateVI() {
	CircuitDocument *circuitDocument = dynamic_cast<CircuitDocument*>(p_itemDocument);

	if (!circuitDocument || !Simulator::self()->isSimulating())
		return false;

	circuitDocument->calculateConnectorCurrents();

	return true;
}

void CanvasTip::display(const QPoint &pos) {
	unsigned num = info.size();

#ifdef BULLSHIT
	for (unsigned i = 0; i < num; i++) {
		if (!std::isfinite(m_v[i]) || std::abs(m_v[i]) < 1e-9)
			m_v[i] = 0.;

		if (!std::isfinite(m_i[i]) || std::abs(m_i[i]) < 1e-9)
			m_i[i] = 0.;
	}
#endif

	move(pos.x() + 20, pos.y() + 4);

	if (num == 0) return;
	if (num == 1)
		setText(displayText(0));
	else {
		QString text;

		for (unsigned i = 0; i < num; i++)
			text += QString("%1: %2\n").arg(QString::number(i)).arg(displayText(i));

		setText(text);
	}
}

QString CanvasTip::displayText(unsigned num) const {
	if(info.size() <= num)
		return QString::null;

	if(info[0].id != -2) {
		 
	return QString("%1%2V  %3%4A #%5")
	       .arg(QString::number(info[num].V / CNItem::getMultiplier(info[num].V), 'g', 3))
	       .arg(CNItem::getNumberMag(info[num].V))
	       .arg(QString::number(info[num].I / CNItem::getMultiplier(info[num].I), 'g', 3))
	       .arg(CNItem::getNumberMag(info[num].I))
		.arg(QString::number(info[num].id));
	}

	return QString("%1%2V  %3%4A")
	       .arg(QString::number(info[num].V / CNItem::getMultiplier(info[num].V), 'g', 3))
	       .arg(CNItem::getNumberMag(info[num].V))
	       .arg(QString::number(info[num].I / CNItem::getMultiplier(info[num].I), 'g', 3))
	       .arg(CNItem::getNumberMag(info[num].I));
}

void CanvasTip::draw(QPainter &p) {
	CircuitDocument *circuitDocument = dynamic_cast<CircuitDocument*>(p_itemDocument);

	if (!circuitDocument || !Simulator::self()->isSimulating())
		return;

	p.setBrush(QColor(0xff, 0xff, 0xdc));
	p.setPen(Qt::black);
	p.drawRect(boundingRect());

	QRect textRect = boundingRect();

	textRect.setLeft(textRect.left() + 3);
	textRect.setTop(textRect.top() + 1);

	p.drawText(textRect, 0, m_text);
}

void CanvasTip::setText(const QString &text) {
	m_text = text;
	canvas()->setChanged(boundingRect());

	QRect r = QFontMetrics(qApp->font()).boundingRect(0, 0, 0, 0, 0, m_text);
	setSize(r.width() + 4, r.height() - 1);
}
//END class CanvasTip


