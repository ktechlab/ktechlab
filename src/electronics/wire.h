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

#include <pin.h>

#include <qpointer.h>
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
		 * Attempts to calculate the current that is flowing through
		 * the connector. Returns true if successfuly, otherwise returns false
		 */
		bool calculateCurrent();
		/**
		 * Returns true if the current flowing through the connector is known
		 */
		bool currentIsKnown() const { return m_bCurrentIsKnown; }
		/**
		 * Set whether the actual current flowing into this node is known (in some
		 * cases - such as this node being ground - it is not known, and so the
		 * value returned by current() cannot be relied on.
		 */
		void setCurrentKnown( bool known );
		/**
		 * Returns the current flowing through the connector.
		 * This only applies for electronic connectors
		 */
		double current() const { return m_current; } 
		/**
		 * Returns the voltage at the connector. This is an average of the
		 * voltages at either end.
		 */
		double voltage() const;
		
		Pin *startPin() const { return m_pStartPin; }
		Pin *endPin() const { return m_pEndPin; }
		
// protected:

private:
	double m_current;
	bool m_bCurrentIsKnown;
	QPointer<Pin> m_pStartPin;
	QPointer<Pin> m_pEndPin;
};

#endif
