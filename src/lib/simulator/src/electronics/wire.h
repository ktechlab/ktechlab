/***************************************************************************
 *   Copyright (C) 2005 by David Saxton                                    *
 *   david@bluehaze.org                                                    *
 *   Modified                                                              *
 *   2010, Zoltan Padrah, zoltan_padrah@users.sourceforge.net              *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#ifndef WIRE_H
#define WIRE_H

#include "simulatorexport.h"

#include <QObject>

class Pin;

/**
@author David Saxton

@brief Connection between two nodes.

The wire always connects two Pins.
A current flows through the wire.
*/
class SIMULATOR_EXPORT Wire : public QObject {
    Q_OBJECT
public:
    /**
     * Create a wire between two pins.
     */
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

    /**
     * Set the current flowing through the wire
     * \param current the value of the current, starting from the start Pin
     *      and flowing towards the endPin
     */
	void setCurrent(double current ) { m_current = current; m_bCurrentIsKnown = true;}

	/**
     * Set the current flowing from one end of the wire. Current flows
     * from the start to the end node of the wire.
     * The current quantity defines the current flowing from the wire into the pin.
     */
	void setCurrentFor(const Pin* pin, double current);

	/**
	 * Returns the current flowing through the connector.
	 * This only applies for electronic connectors
	 */
	double current() const {
				return m_current; }

    /**
     * \return the current flowing from the wire into the pin given as
     *   parameter
     *  \param aPin the pin for which the current is requested
     */
	double currentFor(const Pin *aPin) const;

	/**
	 * Returns the voltage at the connector. This is an average of the
	 * voltages at either end.
	 */
	double voltage() const;

    /**
     * \return the other end of the wire, relative to the pin given as
     *      parameter. If the pin given as parameter is not connected
     *      to the wire, NULL is returned.
     * \param aPin one end of the wire.
     */
	Pin *otherPin(const Pin *aPin) const;


private:
        /// current flowing from the start pin to the end pin of the wire
	double m_current;
        /// is the value of the current known
	bool m_bCurrentIsKnown;

	Pin *m_pStartPin;
	Pin *m_pEndPin;
};

#endif
