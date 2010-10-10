//
// C++ Interface: elementmap
//
// Description: 
//
//
// Author: Alan Grimes <agrimes@speakeasy.net>, (C) 2009
//
// Copyright: See COPYING file that comes with this distribution
//
//

#ifndef ELEMENTMAP_H
#define ELEMENTMAP_H

#include <list>

#include "simulation/element.h"
#include "pin.h"

class LogicIn;

typedef std::list< PinSet > PinSetList;

/**
Contains vital information about the elements in the component.
*/
class ElementMap {

public:
    /**
     Create an ElementMap associated with an existing element.
     The pins for the element are automatically created and they can be
     accessed by the pin() method.
     \param elemnt the element for which the ElementMap is created
     */
    ElementMap(Element *element);
            // TODO add 4 optional arguments for pointer to pins?
	ElementMap();

	void mergeCurrents();
	bool compareElement(const Element *anElement) const;
	void setElement(Element *anElement);
	LogicIn *getLogicInOrNull();
	void putPin(unsigned int slot, Pin *aPin);
	void setupCNodes();

	/// @see Component::setInterCircuitDependent
	PinSetList interCircuitDependent;

	/// @see Component::setInterGroundDependent
	PinSetList interGroundDependent;

private:
	Element *e; // The element
	Pin *n[4]; // The Pins associated with the CNodes in the element
};

#endif
