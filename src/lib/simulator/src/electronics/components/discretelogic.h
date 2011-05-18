/***************************************************************************
 *   Copyright (C) 2005 by David Saxton                                    *
 *   david@bluehaze.org                                                    *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#ifndef DISCRETELOGIC_H
#define DISCRETELOGIC_H

#include "component.h"
#include "logic.h"

class Circuit;

/**
@short Boolean NOT
@author David Saxton
*/
class Inverter : public CallbackClass, public Component {

public:
	Inverter(Circuit &ownerCircuit);
	~Inverter();

protected:
	void inStateChanged(bool newState);

	LogicIn m_pIn;
	LogicOut m_pOut;
};

/**
@short Buffer
@author David Saxton
*/
class Buffer : public CallbackClass, public Component {

public:
	Buffer(Circuit &ownerCircuit);
	~Buffer();

private:
	void inStateChanged(bool newState);

	LogicIn m_pIn;
	LogicOut m_pOut;
};

/**
@short Boolean logic input
@author David Saxton

Note: because this class isn't a GUI class, the state of the button is
simulated with a property "buttonPressed". This property should be set by
the controller class on an user interface event.
*/
class ECLogicInput : public Component {

public:
	ECLogicInput(Circuit &ownerCircuit);
	~ECLogicInput();

protected:
    virtual void propertyChanged(Property& theProperty, QVariant newValue,
                                 QVariant oldValue);
private:
    bool m_toggleOn;
    bool m_outState;
	LogicOut m_pOut;
};

/**
@short Boolean logic output
@author David Saxton
*/
class ECLogicOutput : public CallbackClass, public Component {

public:
	ECLogicOutput(Circuit &ownerCircuit);
	~ECLogicOutput();

protected:
	void inStateChanged(bool newState);

    /** normalized value of the logic value to the interval [0,1],
        in the future possibly taking into account the mean value of
        digital signal in one gui update period
        */
    Property *outValue;

    bool m_bLastState;

	LogicIn m_pIn;
};

#endif
