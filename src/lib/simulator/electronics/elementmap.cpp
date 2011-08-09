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

#include <QDebug>

#include <cassert>

#include "elementmap.h"
#include "simulation/logic.h"

//BEGIN class ElementMap
ElementMap::ElementMap(Element* element)
{
    e = element;

    // create pins
    int nodeCnt = element->numCNodes();
    for(int i = 0; i < nodeCnt; ++i){
        n[i] = new Pin;
        n[i]->addElement(element);
    }
    // fill the rest with null-s
    for(int i = nodeCnt; i < 4; ++i)
        n[i] = 0;

    setupCNodes();
}

ElementMap::~ElementMap()
{
    // the element
    delete e;
    // the pins
    for(int i=0; i<4; i++)
        if(n[i]){
            delete n[i];
            n[i] = 0;
        }
}

Pin* ElementMap::pin(int number)
{
    if((0 <= number) || (number < e->numCNodes()))
        return n[number];
    // not good
    qWarning() << "Trying to get an invalid pin from the ElementMap";
    return NULL;
}

/*!
    \fn ElementMap::mergeCurrents()
 */
void ElementMap::mergeCurrents() {

// TODO: refactor out so that we don't re-do pins that have multiple elements!

	for (int i = 0; i < 4; i++) {
		if(n[i]) {
            n[i]->setSourceCurrent( e->nodeCurrent(i) );
		}
	}
}

#if 0
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

#endif

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


Element* ElementMap::element() const
{
    return e;
}

//END class ElementMap

