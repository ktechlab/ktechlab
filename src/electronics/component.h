/***************************************************************************
 *   Copyright (C) 2003 by David Saxton                                    *
 *   david@bluehaze.org                                                    *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#ifndef COMPONENT_H
#define COMPONENT_H

#include "cnitem.h"

#include <qvaluelist.h>

class ICNDocument;
class CircuitDocument;
class ECNode;
class ECSubcircuit;
class Element;
class Node;
class Pin;

class BJT;
class Capacitance;
class CCCS;
class CCVS;
class CurrentSignal;
class CurrentSource;
class Diode;
class Inductance;
class LogicIn;
class LogicOut;
class OpAmp;
class Resistance;
class Switch;
class VCCS;
class VCVS;
class VoltagePoint;
class VoltageSignal;
class VoltageSource;

typedef QValueList<ECNode*> ECNodeList;
typedef QValueList<Element*> ElementList;
typedef QValueList<Switch*> SwitchList;

typedef QValueList< QValueList<Pin*> > PinListList;

/**
Contains vital information about the elements in the component.
*/

// FIXME: should be in it's own header/source file regardless of how tiny it is.
// OR it should be moved to the same file as the other element classes. 
class ElementMap
{
	public:
		ElementMap();
		
		Element *e; // The element
		Pin *n[4]; // The Pins associated with the CNodes in the element
	
		/// @see Component::setInterCircuitDependent
		PinListList interCircuitDependent;
	
		/// @see Component::setInterGroundDependent
		PinListList interGroundDependent;
};

typedef QValueList<ElementMap> ElementMapList;

/**
@short Base class for all electrical components
@author David Saxton
*/
class Component : public CNItem
{
	Q_OBJECT
public:
	Component( ICNDocument *icnDocument, bool newItem, const QString &id);
	virtual ~Component();
	
	ECNode* createPin( double _x, double _y, int orientation, const QString &name);

	/**
	 * Reinherit this function to disallow rotation of the component.
	 */
// Positioning
	virtual bool canRotate() const { return true; }
	/**
	 * Angle of orientation
	 */
	int angleDegrees() const { return m_angleDegrees; }

	/**
	 * Sets the angle (in degrees)
	 */
	void setAngleDegrees( int degrees);

	/**
	 * Whether or not the item is flipped
	 */
	bool flipped() const { return b_flipped; }

	/**
	 * Sets whether or not the item is flipped
	 */
	void setFlipped( bool flipped);

	/**
	 * After calculating nodal voltages, each component will be
	 * called to tell its nodes what the current flowing *into*
	 * the component is.
	 */
	void setNodalCurrents();

	/**
	 * @return pointer to the CircuitDocument that we're in.
	 */
	CircuitDocument *circuitDocument() const { return m_pCircuitDocument; }

	void initElements( const uint stage);
	virtual void finishedCreation();

	/**
	 * If reinherit (and use) the stepNonLogic function, then you must also
	 * reinherit this function so that it returns true. Else your component
	 * will not get called.
	 */

// FIXME: probably should be renamed to "TimeDependant" and "UpdateFortime".
	virtual bool doesStepNonLogic() const { return false; }
	virtual void stepNonLogic() {};

	/**
	 * Returns the translation matrix used for painting et al
	 * @param orientation The orientation to use
	 * @param x x co-ordinate of the center of the object to be mapped
	 * @param y y co-ordinate of the center of the object to be mapped
	 * @param inverse If false, maps the unrotated item to a rotated one, else mapped->unmapped
	 */

	static QWMatrix transMatrix(int angleDegrees, bool flipped, int x, int y, bool inverse = false);

	/**
	 * @return Information about the component in an ItemData struct.
	 */
	virtual ItemData itemData() const;

	/**
	 * Restores the state of the component from the ItemData struct.
	 */
	virtual void restoreFromItemData(const ItemData &itemData);

// FIXME: I think a common design principal states that a parent class must not know
// anything about it's children. =( 
	BJT *createBJT(Pin *c, Pin *b, Pin *e, bool isNPN = true);
	BJT *createBJT(ECNode *c, ECNode *b, ECNode *e, bool isNPN = true);

	Capacitance *createCapacitance(Pin *n0, Pin *n1, double capacitance);
	Capacitance *createCapacitance(ECNode *n0, ECNode *n1, double capacitance);

	CCCS *createCCCS(Pin *n0, Pin *n1, Pin *n2, Pin *n3, double gain);
	CCCS *createCCCS(ECNode *n0, ECNode *n1, ECNode *n2, ECNode *n3, double gain);

	CCVS *createCCVS(Pin *n0, Pin *n1, Pin *n2, Pin *n3, double gain);
	CCVS *createCCVS(ECNode *n0, ECNode *n1, ECNode *n2, ECNode *n3, double gain);

	CurrentSignal *createCurrentSignal(Pin *n0, Pin *n1, double current);
	CurrentSignal *createCurrentSignal(ECNode *n0, ECNode *n1, double current);

	CurrentSource *createCurrentSource(Pin *n0, Pin *n1, double current);
	CurrentSource *createCurrentSource(ECNode *n0, ECNode *n1, double current);

	Diode *createDiode(Pin *n0, Pin *n1);
	Diode *createDiode(ECNode *n0, ECNode *n1);

	Inductance *createInductance(Pin *n0, Pin *n1, double inductance);
	Inductance *createInductance(ECNode *n0, ECNode *n1, double inductance);

	LogicIn *createLogicIn(Pin *node);
	LogicIn *createLogicIn(ECNode *node);

	LogicOut *createLogicOut(Pin *node, bool isHigh);
	LogicOut *createLogicOut(ECNode *node, bool isHigh);

	OpAmp *createOpAmp(Pin * nonInverting, Pin * out, Pin * inverting);
	OpAmp *createOpAmp(ECNode * nonInverting, ECNode * out, ECNode * inverting);

	Resistance *createResistance(Pin *n0, Pin *n1, double resistance);
	Resistance *createResistance(ECNode *n0, ECNode *n1, double resistance);

	Switch *createSwitch(Pin *n0, Pin *n1, bool open);
	Switch *createSwitch(ECNode *n0, ECNode *n1, bool open);

	VCCS *createVCCS(Pin *n0, Pin *n1, Pin *n2, Pin *n3, double gain);
	VCCS *createVCCS(ECNode *n0, ECNode *n1, ECNode *n2, ECNode *n3, double gain);

	VCVS *createVCVS(Pin *n0, Pin *n1, Pin *n2, Pin *n3, double gain);
	VCVS *createVCVS(ECNode *n0, ECNode *n1, ECNode *n2, ECNode *n3, double gain);

	VoltagePoint *createVoltagePoint(Pin *n0, double voltage);
	VoltagePoint *createVoltagePoint(ECNode *n0, double voltage);

	VoltageSignal *createVoltageSignal(Pin *n0, Pin *n1, double voltage);
	VoltageSignal *createVoltageSignal(ECNode *n0, ECNode *n1, double voltage);

	VoltageSource *createVoltageSource(Pin *n0, Pin *n1, double voltage);
	VoltageSource *createVoltageSource(ECNode *n0, ECNode *n1, double voltage);

	ECNode* ecNodeWithID(const QString &ecNodeId);

	/**
	 * Safely delete an element - in this case, calls element->componentDeleted,
	 * and removes it from the element list.
	 * @param setPinsInterIndependent whether to call
	 * setPinsInterIndependent. The call is time-consuming, and unnecessary
	 * if the pins from which the element was originally attached will be/
	 * were removed, or they will become interdependent again.
	 */
	void removeElement(Element *element, bool setPinsInterIndependent);

	/**
FIXME: Not really a special case, involves knowledge of subclass, etc etc... 
	 * Safely remove a switch.
	 */
	void removeSwitch(Switch *sw);

	/**
	 * Removes all elements and switches.
	 * @param setPinsInterIndependent whether to bother calling
	 * setPinsInterIndependent. This is false when calling from the
	 * destructor, or when the dependency information is the same.
	 */
	void removeElements( bool setPinsInterIndependent = false);

	/**
	 * @return the list of switches that this component uses.
	 */
	SwitchList switchList() const { return m_switchList; }

// Random idiot: what's this "Signals:" thing? is it a C++ feature I'm too stupid to
// understand? 
signals:
	/**
	 * Emitted when an element is created.
	 */
	void elementCreated(Element *element);

	/**
	 * Emitted when an element is destroyed.
	 */
	void elementDestroyed(Element *element);

	/**
same comment above, not really special case, implies knowledge of subclass etc..
	 * Emitted when a switch. is created
	 */
	void switchCreated(Switch *sw);

	/**
	 * Emitted when a switch is destroyed.
	 */
	void switchDestroyed(Switch *sw);

// What's a public "slots"? Another crazy C++ Feature? 
public slots:
	virtual void slotUpdateConfiguration();
	virtual void removeItem();

protected:
	/**
	 * Convenience functionality provided for components in a port shape
	 * (such as ParallelPortComponent and SerialPortComponent).
	 */
	void drawPortShape(QPainter & p);
	virtual void itemPointsChanged();
	virtual void updateAttachedPositioning();
	virtual void initPainter(QPainter &p);

	/**
	 * Untranforms the painter from the matrix. This *must* be called after doing
	 * initPainter( QPainter &p);
	 */
	virtual void deinitPainter(QPainter &p);

	/**
	 * This creates a set of nodes with their internal IDs set to those in QStringList pins.
	 * The pins are in a DIP arrangement, and are spaced width() apart.
	 */
// FIXME:  Implies special knowledge of subclass (or what should be subclass.) 
	void initDIP(const QStringList & pins);

	/**
	 * Creates the DIP symbol:
	 * @li constructs rectangular shape
	 * @li puts on text labels in appropriate positions from QStringList pins
	 */
// FIXME: SHOULD BE SUBCLASS
	void initDIPSymbol(const QStringList & pins, int width);

	/**
	 * Create 1 pin on the left of the component, placed half way down if h1 is
	 * -1 - else at the position of h1.
	 */
	void init1PinLeft(int h1 = -1);

	/**
	 * Create 2 pins on the left of the component, either spread out, or at the
	 * given heights.
	 */
	void init2PinLeft(int h1 = -1, int h2 = -1);

	/**
	 * Create 3 pins on the left of the component, either spread out, or at the
	 * given heights.
	 */
	void init3PinLeft(int h1 = -1, int h2 = -1, int h3 = -1);

	/**
	 * Create 4 pins on the left of the component, either spread out, or at the
	 * given heights.
	 */
	void init4PinLeft(int h1 = -1, int h2 = -1, int h3 = -1, int h4 = -1);

	/**
	 * Create 1 pin on the right of the component, placed half way down if h1 is
	 * -1 - else at the position of h1.
	 */
	void init1PinRight(int h1 = -1);

	/**
	 * Create 2 pins on the right of the component, either spread out, or at the
	 * given heights.
	 */
	void init2PinRight(int h1 = -1, int h2 = -1);

	/**
	 * Create 3 pins on the right of the component, either spread out, or at the
	 * given heights.
	 */
	void init3PinRight(int h1 = -1, int h2 = -1, int h3 = -1);

	/**
	 * Create 4 pins on the right of the component, either spread out, or at the
	 * given heights.
	 */
	void init4PinRight(int h1 = -1, int h2 = -1, int h3 = -1, int h4 = -1);

	/**
	 * When we remove an element, we have to rebuild the list of inter-dependent
	 * nodes. (when adding elements, we just call setInterDependent).
	 */
	void rebuildPinInterDepedence();

	// Pointers to commonly used nodes
// design issue: In a very complex class, performance tweeks of this type tend to
// promote bugs. Perhaps a "fast component" subclass could add caching and other
// performance enhancements while building on a stable, reliable API. 
	ECNode *m_pPNode[4];
	ECNode *m_pNNode[4];

	QGuardedPtr<CircuitDocument> m_pCircuitDocument;
	int m_angleDegrees;
	bool b_flipped;

private:
	/**
	 * Convenience function for calling both setInterCircuitDependent and
	 * setInterGroundDependent.
	 * @param it Which pins are inter-dependent needs to be recorded in case
	 * this information is later needed in rebuildPinInterDependence.
	 */
	void setInterDependent(ElementMapList::iterator it, const QValueList<Pin*> & pins);

	/**
	 * Sets all pins independent of each other.
	 */
	void setAllPinsInterIndependent();

	/**
	 * The given pins will affect the simulation of each other. Therefore, they
	 * will need to be simulated in the same circuit.
	 */
	void setInterCircuitDependent( ElementMapList::iterator it, const QValueList<Pin*> & pins);

	/**
	 * If any of the given pins are ground, then that will affect whether
	 * any of the other pins can be ground.
	 */
	void setInterGroundDependent( ElementMapList::iterator it, const QValueList<Pin*> & pins);

	/**
	 * List of ElementMaps; which contain information on the pins associated
	 * with the element as well as the dependence between the pins for that
	 * element.
	 * @see ElementMap
	 */
	ElementMapList m_elementMapList;

	/**
	 * The switches used by the component.
	 */
// FIXME: again with the switches! bleh! 
	SwitchList m_switchList;

	/**
	 * @return an iterator to the element in m_elementMapList
	 */
	ElementMapList::iterator handleElement(Element *e, const QValueList<Pin*> &pins);
};

#endif

