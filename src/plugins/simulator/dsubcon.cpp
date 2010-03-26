//
// C++ Implementation: dsubcon
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

#include "dsubcon.h"

void DSubCon::drawPortShape(QPainter &p) {
	int h = height();
	int w = width() - 1;
	int _x = int(x() + offsetX());
	int _y = int(y() + offsetY());

	double roundSize = 8;
	double slantIndent = 8;

	const double DPR = 180.0 / M_PI;
	double inner = std::atan(h / slantIndent);	// Angle for slight corner
	double outer = M_PI - inner;		// Angle for sharp corner

	int inner16 = int(16 * inner * DPR);
	int outer16 = int(16 * outer * DPR);

	p.save();
	p.setPen(Qt::NoPen);
	p.drawPolygon(areaPoints());
	p.restore();

	initPainter(p);

	// Left line
	p.drawLine(int(_x), int(_y + roundSize / 2), int(_x), int(_y + h - roundSize / 2));
	// Right line
	p.drawLine(int(_x + w),	int(_y - slantIndent + h - roundSize / 2), int(_x + w),	int(_y + slantIndent + roundSize / 2));

	// Bottom line
	p.drawLine(int(_x + (1 - std::cos(outer)) * (roundSize / 2)), int(_y + h + (std::sin(outer) - 1) * (roundSize / 2)),
	           int(_x + w + (std::cos(inner) - 1) * (roundSize / 2)), int(_y + h - slantIndent + (std::sin(inner) - 1) * (roundSize / 2)));
	// Top line
	p.drawLine(int(_x + w + (std::cos(outer) - 1) * (roundSize / 2)), int(_y + slantIndent + (1 - std::sin(inner)) * (roundSize / 2)), int(_x + (1 - std::cos(inner)) * (roundSize / 2)), int(_y + (1 - std::sin(outer)) * (roundSize / 2)));

	// Top left
	p.drawArc(int(_x), int(_y), int(roundSize), int(roundSize), 90 << 4,	outer16);

	// Bottom left
	p.drawArc(int(_x), int(_y + h - roundSize), int(roundSize), int(roundSize), 180 << 4,	outer16);

	// Top right
	p.drawArc(int(_x + w - roundSize), int(_y + slantIndent), int(roundSize), int(roundSize), 0, inner16);

	// Bottom right
	p.drawArc(int(_x + w - roundSize), int(_y - slantIndent + h - roundSize), int(roundSize), int(roundSize), 270 << 4, inner16);

	deinitPainter(p);
}