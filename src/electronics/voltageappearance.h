//
// C++ Interface: voltageappearance
//
// Description:
//
// Author: Alan Grimes <agrimes@speakeasy.net>, (C) 2009

// Copyright: See COPYING file that comes with this distribution
//

#ifndef VOLTAGEAPPEARANCE_H
#define VOLTAGEAPPEARANCE_H

class QColor;

/**
 * Converts the voltage level to a colour - this is used in drawing
 * wires and pins.
 */
QColor voltageColor(double v);

/**
 * @return a value between 0.0 and 1.0, representing a scaled version of
 * the absolute value of the voltage.
 * @see voltageColor
 */
double voltageLength(double v);

#endif

