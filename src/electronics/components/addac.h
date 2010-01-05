/***************************************************************************
 *   Copyright (C) 2005 by David Saxton                                    *
 *   david@bluehaze.org                                                    *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#ifndef ADDAC_H
#define ADDAC_H

#include "component.h"

class LogicIn;
class LogicOut;
class VoltagePoint;

const int max_ADDAC_bits = 32;

/**
@author David Saxton
*/
class ADDAC : public Component
{
public:
	ADDAC( ICNDocument *icnDocument, bool newItem, const char *id = 0 );
	~ADDAC();
protected:
	void dataChanged();
	/**
	 * Add / remove pins according to the number of outputs the user has requested
	 */
	virtual void initPins() = 0;

	int m_numBits;
	double m_range;
};

/**
@author David Saxton
 */
class ADC : public ADDAC
{
public:
	ADC( ICNDocument *icnDocument, bool newItem, const char *id = 0 );
	~ADC();
	
	static Item* construct( ItemDocument *itemDocument, bool newItem, const char *id );
	static LibraryItem *libraryItem();
	
	virtual void stepNonLogic();
	virtual bool doesStepNonLogic() const { return true; }

	protected:

	/**
	 * Add / remove pins according to the number of outputs the user has requested
	 */
	virtual void initPins();

	LogicOut *m_logic[max_ADDAC_bits];
	ECNode *m_realNode;
};

/**
@author David Saxton
 */
class DAC : public ADDAC
{
public:
	DAC( ICNDocument *icnDocument, bool newItem, const char *id = 0 );
	~DAC();

	static Item* construct( ItemDocument *itemDocument, bool newItem, const char *id );
	static LibraryItem *libraryItem();

	virtual void stepNonLogic();
	virtual bool doesStepNonLogic() const { return true; }

protected:
	/**
	 * Add / remove pins according to the number of outputs the user has requested
	 */
	virtual void initPins();

	LogicIn *m_logic[max_ADDAC_bits];
	VoltagePoint *m_voltagePoint;
};

#endif
