/***************************************************************************
 *   Copyright (C) 2005 by David Saxton                                    *
 *   david@bluehaze.org                                                    *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#ifndef PIN_H
#define PIN_H

#include <set>
#include <qobject.h>
#include "wire.h"

class Element;
class Pin;
class Switch;
class Wire;

typedef std::set<Element*> ElementList;
typedef std::set<Pin *> PinSet;
typedef std::set<Switch*> SwitchSet;
typedef std::set<Wire *> WireList;

/**
@author David Saxton
*/
class Pin : public QObject {

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

	Pin();
	~Pin();

	/**
	 * After calculating the nodal voltages in the circuit, this function should
	 * be called to tell the pin what its voltage is.
	 */
	void setVoltage(double v) { m_voltage = v; }

	/**
	 * Returns the voltage as set by setVoltage.
	 */
	double voltage() const { return m_voltage; }

	/**
	 * Tell thie Pin that none of the currents from the switches have yet
	 * been merged.
	 */
	void setSwitchCurrentsUnknown() {
		m_unknownSwitchCurrents = m_switchList; }

	/**
	 * This returns the value given by setCurrentKnown AND'd with whether
	 * we know the current from each switch attached to this pin.
	 * @see setCurrentKnown
	 */
	bool currentIsKnown() const {
		return (m_wireList.size() < 2) && m_unknownSwitchCurrents.empty();
	}

	/**
	 * Tells the Pin that the current from the given switch has been merged.
	 */
	void setSwitchCurrentKnown(Switch *sw) {
		m_unknownSwitchCurrents.erase(sw);
	}

	/**
	 * Tries to calculate the Pin current from the input / output wires.
	 * @return whether was successful.
	 */
	double calculateCurrentFromWires(Wire *aWire = (Wire *)0) const;

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
	PinSet circuitDependentPins() const { return m_circuitDependentPins; }
	/**
	 * Returns the ids of the pins whose voltages will affect this pin.
	 * @see void setDependentPins( QStringList ids )
	 */
	PinSet groundDependentPins() const { return m_groundDependentPins; }
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
	void setEqId(int id) { m_eqId = id; }
	/**
	 * The equation identifier.
	 * @see setEqId
	 */
	int eqId() const { return m_eqId; }

// #########################
	/**
	 * Returns a list of elements that will affect this pin (e.g. if this
	 * pin is part of a resistor, then that list will contain a pointer to a
	 * Resistance element)
	 */
	ElementList elements() const {
		return m_elementList;
	}
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
	 * Adds an switch to the list of those that will affect this pin.
	 */
	void addSwitch(Switch *e);
	/**
	 * Removes an switch from the list of those that will affect this pin.
	 */
	void removeSwitch(Switch *e);

	void addWire(Wire *wire);
	void removeWire(Wire *wire);

	WireList wireList() const { return m_wireList; }
	int numWires() const { return m_wireList.size(); }

protected:
	double m_voltage;

	int m_eqId;
	GroundType m_groundType;

	PinSet m_circuitDependentPins;
	PinSet m_groundDependentPins; // for circuit partitioning. 

	SwitchSet m_switchList;
	SwitchSet m_unknownSwitchCurrents;

private:
	ElementList m_elementList;
	WireList m_wireList;
};

#endif
