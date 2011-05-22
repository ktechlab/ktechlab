/***************************************************************************
 *   Copyright (C) 2005 by David Saxton                                    *
 *   david@bluehaze.org                                                    *
 *   Modified by                                                           *
 *    2010, Zoltan Padrah, zoltan_padrah@users.sourceforge.net             *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#ifndef PIN_H
#define PIN_H

#include "wire.h"
#include "element.h"
#include "typedefs.h"
#include "simulatorexport.h"

#include <set>
#include <QList>

class Element;
class Pin;
class Wire;


/**
@author David Saxton

@brief A pin of an element or a junction point

A pin always has a voltage, and it can have a current flow from an element.
It also can have one or more wires, through which current can flow,
into other pins.

*/
class SIMULATOR_EXPORT Pin {

public:
	/**
	 * Priorities for ground pin. gt_always will (as expected) always assign
	 * the given pin as ground, gt_never will never do. If no gt_always pins
	 * exist, then the pin with the highest priority will be set as ground -
	 * if there is at least one pin that is not of ground type gt_never. These
	 * are only predefined recommended values, so if you choose not to use one
	 * of these, please respect the priorities with respect to the examples, and
	 * always specify a priority between 0 and 20.
	 * @see groundLevel
	 */
	enum GroundType {
		gt_always = 0, // ground
		gt_high = 5, // voltage points
		gt_medium = 10, // voltage sources
		gt_low = 15, // current sources
		gt_never = 20 // everything else
	};

    /** create a pin */
	Pin();

    /** Destructor */
	~Pin();

	/**
	 * After calculating the nodal voltages in the circuit, this function should
	 * be called to tell the pin what its voltage is.
     * @param v the new voltage on the pin
	 */
	void setVoltage(double v);

	/**
	 * @return the voltage as set by setVoltage.
	 */
	double voltage() const ;

    /**
     * Set of the current is known on this pin
     */
    void setCurrentKnown(bool known);

	/**
	 * This returns the value given by setCurrentKnown AND'd with whether
	 * we know the current from each switch attached to this pin.
	 * @see setCurrentKnown
	 */
	bool currentIsKnown() const;

    /**
     * @return the current flowing into the pin from the associated element.
     *      If the pin doesn't belong to any element, then 0 should be
     *          returned.
     *      If currentIsKnown() is false, then the behaviour of this method
     *          is undefined.
     */
    double sourceCurrent() const;

    /**
     * Set the current coming from the element into the pin.
     * Should be called from @ref ElementMap.
     */
    void setSourceCurrent(double current);


	/**
	 * Sets the "ground type" - i.e. the priority that this pin has for being
	 * ground over other pins in the circuit. Lower gt = higher priority. It's
	 * recommended to use Pin::GroundType.
	 */
	void setGroundType(GroundType gt) { m_groundType = gt; }

	/**
	 * Returns the priority for ground.
	 */
	int groundType() const { return m_groundType; }

// ###  strange pin list stuff.
	/**
	 * Adds a dependent pin - one whose voltages will (or might) affect the
	 * voltage of this pin. This is set by Component.
	 */
	void addCircuitDependentPin(Pin *pin);
	/**
	 * Adds a dependent pin - one whose voltages will (or might) affect the
	 * voltage of this pin. This is set by Component.
	 */
	void addGroundDependentPin(Pin *pin);
	/**
	 * Removes all Circuit and Ground dependent pins.
	 */
	void removeDependentPins();
	/**
	 * Returns the ids of the pins whose voltages will affect this pin.
	 * @see void setDependentPins( QStringList ids )
	 */
	PinSet &circuitDependentPins() { return m_circuitDependentPins; }
	/**
	 * Returns the ids of the pins whose voltages will affect this pin.
	 * @see void setDependentPins( QStringList ids )
	 */
	PinSet &groundDependentPins() { return m_groundDependentPins; }
	/**
	 * This function returns the pins that are directly connected to this pins:
	 * either at the ends of connected wires, or via switches.
	 */
	PinSet localConnectedPins();

// ###
	/**
	 * Use this function to set the pin identifier for equations,
	 * which should be done every time new pins are registered.
	 */
	void setEqId(int id);
	/**
	 * The equation identifier. This identifier is a position on the
     * circuit equations.
	 * @see setEqId
	 */
	int eqId() const ;

// #########################
	/**
	 * Returns a list of elements that will affect this pin (e.g. if this
	 * pin is part of a resistor, then that list will contain a pointer to a
	 * Resistance element)
     * \TODO convert the list of elements to a simple pointer to an Element
	 */
	ElementList &elements() { return m_elementList; }

	/**
	 * Adds an element to the list of those that will affect this pin.
	 */
	void addElement(Element *e);

	/**
	 * Removes an element from the list of those that will affect this pin.
	 */
	void removeElement(Element *e);

// #########################

    /**
     * Add a wire to the list of wires connected to this pin.
     * @param wire the wire to be added
     */
	void addWire(Wire *wire);

    /**
     * Remove a wire from the list of wires connected to this pin.
     */
	void removeWire(Wire *wire);

    /**
     * @return the list of wires connected to the Pin.
     */
    const WireList wires() const;

private:
        /// voltage of the Pin
	double m_voltage;
        /// current flowing from an element into the pin, if applicable
    double m_sourceCurrent;

        /// is the current flowing from the Element known?
    bool m_currentIsKnown;

        /// Equation ID
	int m_eqId;

	GroundType m_groundType;

	PinSet m_circuitDependentPins;
	PinSet m_groundDependentPins; // for circuit partitioning.

        /// list of associated elements
	ElementList m_elementList;
        /// list of connected wires
	WireList m_wireList;
};

#endif
