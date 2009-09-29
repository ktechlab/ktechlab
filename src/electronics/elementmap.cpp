//
// C++ Implementation: elementmap
//
// Description: 
//
//
// Author: David Saxton, Alan Grimes <agrimes@speakeasy.net>, (C) 2009
//
// Copyright: See COPYING file that comes with this distribution
//
//

#include <cassert>

#include "elementmap.h"
#include "simulation/logic.h"

//BEGIN class ElementMap
ElementMap::ElementMap() {
	e = 0;

	for(int i = 0; i < 4; ++i)
		n[i] = 0;
}

/*!
    \fn ElementMap::mergeCurrents()
 */
void ElementMap::mergeCurrents() {

	for (int i = 0; i < 4; i++) {
		double current = 0; 
		if(n[i]) {
			WireList wires = n[i]->wireList(); 
			if(wires.size() == 1) {

// FIXME: should probably set currents unknown if we can't compute it from our ecnode. 
			(*(wires.begin()))->setCurrent(e->cnodeCurrent(i));
			}
		}
	}
}

/*!
    \fn ElementMap::compareElement(Element *anElement);
 */
bool ElementMap::compareElement(const Element *anElement) const {
	return e == anElement;
}

/*!
    \fn ElementMap::getLogicInOrNull()
 */
LogicIn *ElementMap::getLogicInOrNull() {
	return dynamic_cast<LogicIn*>(e);
}

/*!
    \fn ElementMap::setElement(Element anElement)
 */
void ElementMap::setElement(Element *anElement) {
	e = anElement;
}

/*!
    \fn ElementMap::putPin(unsigned int slot, Pin *aPin)
 */
void ElementMap::putPin(unsigned int slot, Pin *aPin) {
	assert(slot < 4);
	n[slot] = aPin;
}

/*!
    \fn ElementMap::setupCNodes()
 */
void ElementMap::setupCNodes() {
	assert(e);

	if (n[3]) {
		e->setCNodes(n[0]->eqId(), n[1]->eqId(), n[2]->eqId(), n[3]->eqId());
	} else if (n[2]) {
		e->setCNodes(n[0]->eqId(), n[1]->eqId(), n[2]->eqId());
	} else if (n[1]) {
		e->setCNodes(n[0]->eqId(), n[1]->eqId());
	} else if (n[0]) {
		e->setCNodes(n[0]->eqId());
	}

	e->add_initial_dc();
}

//END class ElementMap

