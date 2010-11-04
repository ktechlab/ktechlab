/***************************************************************************
 *   Copyright (C) 2004-2005 by David Saxton                               *
 *   david@bluehaze.org                                                    *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#ifndef DEMULTIPLEXER_H
#define DEMULTIPLEXER_H

// #include "dipcomponent.h"
#include "logic.h"
#include "component.h"

#include <vector>

/**
@author David Saxton
*/
class Demultiplexer : public CallbackClass,  public Component
{
public:
	Demultiplexer();
	~Demultiplexer();
	
protected:
    virtual void propertyChanged(Property& theProperty,
                                 QVariant newValue, QVariant oldValue );
	// void dataChanged();
	/**
	 * Add / remove pins according to the number of inputs the user has requested
	 */
	void initPins( unsigned addressSize );
	
	void inStateChanged( bool newState );
	
	std::vector<LogicIn> m_aLogic;
	std::vector<LogicOut> m_xLogic;
	LogicIn m_input;
};

#endif
