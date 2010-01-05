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

#include <qguardedptr.h>
#include <qobject.h>
#include <qvaluelist.h>

#include "wire.h"

class ECNode;
class Element;
class Pin;
class Switch;
class Wire;

typedef std::set<Element*> ElementList;
typedef std::set<QGuardedPtr<Pin> > PinList;
typedef QValueList<Switch*> SwitchList;
typedef QValueList<QGuardedPtr<Wire> > WireList;

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

	Pin(ECNode *parent);
	~Pin();

	ECNode *parentECNode() const { return m_pECNode; }

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
	 * After calculating nodal voltages, each component will be called to tell
	 * its pins what the current flowing *into* the component is. This sets it
	 * to zero in preparation to merging the current.
	 */
	void resetCurrent() { m_current = 0.0; }

	/**
	 * Adds the given current to that already flowing into the pin.
	 * @see setCurrent
	 */
	void mergeCurrent(double i) { m_current += i; }

	/**
	 * Returns the current as set by mergeCurrent.
	 */
	double current() const { return m_current; }

	/**
	 * In many cases (such as if this pin is a ground pin), the current
	 * flowing into the pin has not been calculated, and so the value
	 * returned by current() cannot be trusted.
	 */
	void setCurrentKnown(bool isKnown) { m_bCurrentIsKnown = isKnown; }

	/**
	 * Tell thie Pin that none of the currents from the switches have yet
	 * been merged.
	 */
	void setSwitchCurrentsUnknown() {
		m_switchList.remove(0);
		m_unknownSwitchCurrents = m_switchList;
	}

	/**
	 * This returns the value given by setCurrentKnown AND'd with whether
	 * we know the current from each switch attached to this pin.
	 * @see setCurrentKnown
	 */
	bool currentIsKnown() const {
		return m_bCurrentIsKnown && m_unknownSwitchCurrents.isEmpty();
	}

	/**
	 * Tells the Pin that the current from the given switch has been merged.
	 */
	void setSwitchCurrentKnown(Switch *sw) {
		m_unknownSwitchCurrents.remove(sw);
	}

	/**
	 * Tries to calculate the Pin current from the input / output wires.
	 * @return whether was successful.
	 */
	bool calculateCurrentFromWires();

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
	PinList circuitDependentPins() const { return m_circuitDependentPins; }
	/**
	 * Returns the ids of the pins whose voltages will affect this pin.
	 * @see void setDependentPins( QStringList ids )
	 */
	PinList groundDependentPins() const { return m_groundDependentPins; }
	/**
	 * This function returns the pins that are directly connected to this pins:
	 * either at the ends of connected wires, or via switches.
	 */
	PinList localConnectedPins() const;
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

// ###
	double m_current;
	bool m_bCurrentIsKnown;
// ###

	int m_eqId;
	GroundType m_groundType;

	PinList m_circuitDependentPins;
	PinList m_groundDependentPins; // for circuit partitioning. 

	ElementList m_elementList;

	WireList m_wireList;
	ECNode *m_pECNode;  /// we *ALWAYS* have an ECNode, and it never changes.

	SwitchList m_switchList;
	SwitchList m_unknownSwitchCurrents;
};

#endif
