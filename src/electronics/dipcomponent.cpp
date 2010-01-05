//
// C++ Implementation: dipcomponent
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

#include "dipcomponent.h"

void DIPComponent::initDIP(const QStringList &pins) {
	const int numPins = pins.size();
	const int numSide = (numPins + 1) >> 1;

	// Pins along left
	for(int i = 0; i < numSide; i++) {
		if (!pins[i].isEmpty()) {
			const int nodeX = -8 + offsetX();
			const int nodeY = ((i + 1) << 4) + offsetY();
			ECNode *node = ecNodeWithID(pins[i]);

			if (node) {
				m_nodeMap[pins[i]].setXY(nodeX, nodeY);
				m_nodeMap[pins[i]].setOrientationNorth();
			} else	createPin(nodeX, nodeY, 0, pins[i]);
		}
	}

	// Pins along right
	for(int i = numSide; i < numPins; i++) {
		if(!pins[i].isEmpty()) {
			const int nodeX = width() + 8 + offsetX();
			const int nodeY = ((2 * numSide - i) << 4) + offsetY();
			ECNode *node = ecNodeWithID(pins[i]);

			if(node) {
				m_nodeMap[pins[i]].setXY(nodeX, nodeY);
				m_nodeMap[pins[i]].setOrientationSouth();
			} else	createPin(nodeX, nodeY, 180, pins[i]);
		}
	}

	updateAttachedPositioning();
}

void DIPComponent::initDIPSymbol(const QStringList &pins, int _width) {
	const int numPins = pins.size();
	const int numSide = (numPins + 1) >> 1;

	setSize(-(_width - (_width % 16)) / 2, -(numSide + 1) * 8, _width, (numSide + 1) * 16, true);

	QWidget tmpWidget;
	QPainter p(&tmpWidget);

	p.setFont(font());

	// Pins along left
	for(int i = 0; i < numSide; i++) {
		if (!pins[i].isEmpty()) {
			const QString text = *pins.at(i);

			const int _top = (i + 1) * 16 - 8 + offsetY();
			const int _width = width() / 2 - 6;
			const int _left = 6 + offsetX();
			const int _height = 16;

			QRect br = p.boundingRect(QRect(_left, _top, _width, _height), Qt::AlignLeft, text);
			addDisplayText(text, br, text);
		}
	}

	// Pins along right
	for(int i = numSide; i < numPins; i++) {
		if(!pins[i].isEmpty()) {
			const QString text = *pins.at(i);

			const int _top = (2 * numSide - i) * 16 - 8 + offsetY();
			const int _width = width() / 2 - 6;
			const int _left = (width() / 2) + offsetX();
			const int _height = 16;

			QRect br = p.boundingRect(QRect(_left, _top, _width, _height), Qt::AlignRight, text);
			addDisplayText(text, br, text);
		}
	}

	updateAttachedPositioning();
}

