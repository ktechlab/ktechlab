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

#include <list>
#include <set>
#include <qvaluelist.h>

#include "cnitem.h"
#include "ecnode.h"
#include "elementmap.h"

class ICNDocument;
class CircuitDocument;
class ECSubcircuit;
class Element;
class Pin;
class Switch;
class LogicIn;

typedef std::list<Pin *> PinList;
typedef std::set<Element*> ElementList;
typedef QValueList<Switch*> SwitchList;
typedef QValueList< PinSet > PinSetList;
typedef QValueList<ElementMap> ElementMapList;

/**
@short Base class for all electrical components
@author David Saxton
*/

class Component : public CNItem {
	Q_OBJECT

public:
	Component(ICNDocument *icnDocument, bool newItem, const QString &id);
	virtual ~Component();

	ECNode* createPin(double _x, double _y, int orientation, const QString &name);

//###

	/**
	 * Angle of orientation
	 */
	int angleDegrees() const { return m_angleDegrees; }

	/**
	 * Sets the angle (in degrees)
	 */
	void setAngleDegrees(int degrees);

	/**
	 * Whether or not the item is flipped
	 */
	bool flipped() const { return b_flipped; }

	/**
	 * Sets whether or not the item is flipped
	 */
	void setFlipped(bool flipped);

	/**
	 * After calculating nodal voltages, each component will be
	 * called to tell its nodes what the current flowing *into*
	 * the component is.
	 */
	void setNodalCurrents();

	/**
	 * @return pointer to the CircuitDocument that we're in.
	* mainly used to deal with switches and the complexities those introduce in the current code.
	 */
	CircuitDocument *circuitDocument() const {
		return m_pCircuitDocument;
	}

	void initElements(const uint stage);
	virtual void finishedCreation();
	/**
	 * If reinherit (and use) the stepNonLogic function, then you must also
	 * reinherit this function so that it returns true. Else your component
	 * will not get called.
	 */
	virtual bool doesStepNonLogic() const {	return false; }

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

	/// simplified element attachers
// TODO: look into feasibility for using variable argument arrays for these and other
// methods in this class. 
	void setup1pinElement(Element *ele, Pin *a);
	void setup2pinElement(Element *ele, Pin *a, Pin *b);
	void setup3pinElement(Element *ele, Pin *a, Pin *b, Pin *c);
	void setup4pinElement(Element *ele, Pin *a, Pin *b, Pin *c, Pin *d);
	void setupSpcl4pinElement(Element *ele, Pin *a, Pin *b, Pin *c, Pin *d);

	Switch *createSwitch(Pin *n0, Pin *n1, bool open);
	ECNode *ecNodeWithID(const QString &ecNodeId);

	/**
	 * Safely remove a switch.
	 */
	void removeSwitch(Switch *sw);

	/**
	 * @return the list of switches that this component uses.
	 */
	SwitchList switchList() const {
		return m_switchList;
	}

	/**
	 * Safely delete an element - in this case, calls element->componentDeleted,
	 * and removes it from the element list.
	 * @param setPinsInterIndependent whether to call
	 * setPinsInterIndependent. The call is time-consuming, and unnecessary
	 * if the pins from which the element was originally attached will be/
	 * were removed, or they will become interdependent again.
	 */
	void removeElement(Element *element, bool setPinsInterIndependent);

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
	 * Emitted when a switch. is created
	 */
	void switchCreated(Switch *sw);

	/**
	 * Emitted when a switch is destroyed.
	 */
	void switchDestroyed(Switch *sw);

public slots:
	virtual void slotUpdateConfiguration();
	virtual void removeItem();

protected:
	/**
	 * Removes all elements and switches.
	 * @param setPinsInterIndependent whether to bother calling
	 * setPinsInterIndependent. This is false when calling from the
	 * destructor, or when the dependency information is the same.
	 */
	void removeElements(bool setPinsInterIndependent = false);

	virtual void itemPointsChanged();
	virtual void updateAttachedPositioning();
	virtual void initPainter(QPainter &p);

	/**
	 * Untranforms the painter from the matrix. This *must* be called after doing
	 * initPainter( QPainter &p );
	 */
	virtual void deinitPainter(QPainter &p);

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

	// Pointers to commonly used nodes
// TODO: why do we have two sets of these?
	ECNode *m_pPNode[4];
	ECNode *m_pNNode[4];

// TODO: only Switch cares about this, so either demote it to a member of class switch or
// refactor it out alltogether.
	CircuitDocument *m_pCircuitDocument;
	int m_angleDegrees;
	bool b_flipped;

private:
	/**
	 * When we remove an element, we have to rebuild the list of inter-dependent
	 * nodes. (when adding elements, we just call setInterDependent).
	 */
	void rebuildPinInterDepedence();

	/**
	 * Convenience function for calling both setInterCircuitDependent and
	 * setInterGroundDependent.
	 * @param it Which pins are inter-dependent needs to be recorded in case
	 * this information is later needed in rebuildPinInterDependence.
	 */
	void setInterDependent(ElementMapList::iterator it, PinSet &pins);

	/**
	 * Sets all pins independent of each other.
	 */
	void setAllPinsInterIndependent();

	/**
	 * The given pins will affect the simulation of each other. Therefore, they
	 * will need to be simulated in the same circuit.
	 */
	void setInterCircuitDependent(ElementMapList::iterator it, PinSet &pins);

	/**
	 * If any of the given pins are ground, then that will affect whether
	 * any of the other pins can be ground.
	 */
	void setInterGroundDependent(ElementMapList::iterator it, PinSet &pins);

	/**
	 * List of ElementMaps; which contain information on the pins associated
	 * with the element as well as the dependence between the pins for that
	 * element.
	 * @see ElementMap
	 */
	ElementMapList m_elementMapList;

	/**
	 * The switches used by the component.
	TODO: ammend this comment with a more complete justification for the design decision to put this here.
	 */
	SwitchList m_switchList;

	/**
	 * @return an iterator to the element in m_elementMapList
	 */
	ElementMapList::iterator handleElement(Element *e, const PinList &pins);
};

#endif
