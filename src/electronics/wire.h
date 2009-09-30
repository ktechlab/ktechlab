/***************************************************************************
 *   Copyright (C) 2005 by David Saxton                                    *
 *   david@bluehaze.org                                                    *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#ifndef WIRE_H
#define WIRE_H

#include <qobject.h>

class Pin;

/**
@author David Saxton
*/
class Wire : public QObject
{
public:
	Wire( Pin *startPin, Pin *endPin );
	~Wire();

	/**
	 * Returns true if the current flowing through the connector is known
	 */
	bool currentIsKnown() const { return m_bCurrentIsKnown; }

	/**
	 * Set whether the actual current flowing into this node is known (in some
	 * cases - such as this node being ground - it is not known, and so the
	 * value returned by current() cannot be relied on.
	 */
	void setCurrentKnown(bool known);

	void setCurrent(double current ) { m_current = current; m_bCurrentIsKnown = true;}

	/**
	 * Returns the current flowing through the connector.
	 * This only applies for electronic connectors
	 */
	double current() { if(!m_bCurrentIsKnown) calculateCurrent();
				return m_current; }

	double currentFor(const Pin *aPin) const;

	/**
	 * Returns the voltage at the connector. This is an average of the
	 * voltages at either end.
	 */
	double voltage() const;

	Pin *otherPin(const Pin *aPin) const;

private:
	/**
	 * Attempts to calculate the current that is flowing through
	 * the connector. Returns true if successfuly, otherwise returns false
	 */
	bool calculateCurrent();

	double m_current;
	bool m_bCurrentIsKnown;
	Pin *m_pStartPin;
	Pin *m_pEndPin;
};

#endif
