//
// C++ Implementation: voltageappearance
//
// Description: 
//
//
// Author: David Saxton, Alan Grimes <agrimes@speakeasy.net>, (C) 2009
//
// Copyright: See COPYING file that comes with this distribution
//
//

#include "voltageappearance.h"

#include <qcolor.h>

#include <cmath>

double voltageLength(double v) {
	double v_max = 1e1;
	double v_min = 1e-1;

	v = std::abs(v);

	if(v >= v_max) return 1.0;
	else if(v <= v_min) return 0.0;
	else return std::log(v / v_min) / std::log(v_max / v_min);
}

QColor voltageColor(double v) {
	double prop = voltageLength(v);

	if(v >= 0)
		return QColor(int(255 * prop), int(166 * prop), 0);
	else return QColor(0, int(136 * prop), int(255 * prop));
}

