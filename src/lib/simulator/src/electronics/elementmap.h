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
     \param element the element for which the ElementMap is created
     */
    ElementMap(Element *element);
            // TODO add 4 optional arguments for pointer to pins?

    /**
     Destructor. Deletes the elementmap, the element,
        and the pins associated to it.
     */
    ~ElementMap();

    /**
     \return a pin of the Element associated with this PinMap.
        If the pin number is invalid, NULL is returned.
     \param number the number used for identication for the pin.
     */
    Pin *pin(int number);

    /**
     * Get the currents from the associated Element and place
     * the values in the Pins
     */
	void mergeCurrents();

	// bool compareElement(const Element *anElement) const;
    // TODO get rid of the method below
	//void setElement(Element *anElement);

	// LogicIn *getLogicInOrNull();
	// void putPin(unsigned int slot, Pin *aPin);

    /**
     * Set the CNode IDs of the Element, based on the
     * CNode IDs of the pins
     */
	void setupCNodes();

	/// @see Component::setInterCircuitDependent
	//PinSetList interCircuitDependent;

	/// @see Component::setInterGroundDependent
	// PinSetList interGroundDependent;

    /**
     \return the Element to which this set belongs
     */
    Element *element() const;

private:
	Element *e; // The element
	Pin *n[4]; // The Pins associated with the CNodes in the element
};

#endif
