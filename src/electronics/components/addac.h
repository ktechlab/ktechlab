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

const int max_ADDAC_bits = 32;

/**
@author David Saxton
*/
class ADDAC : public Component
{
public:
	public:
		ADDAC( ICNDocument *icnDocument, bool newItem, const char *id = 0 );
		~ADDAC() override;
		
	protected:
		void dataChanged() override;
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
		~ADC() override;
	
		static Item* construct( ItemDocument *itemDocument, bool newItem, const char *id );
		static LibraryItem *libraryItem();
	
		void stepNonLogic() override;
		bool doesStepNonLogic() const override { return true; }
	
	protected:
		/**
		 * Add / remove pins according to the number of outputs the user has requested
		 */
		void initPins() override;
	
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
		~DAC() override;
	
		static Item* construct( ItemDocument *itemDocument, bool newItem, const char *id );
		static LibraryItem *libraryItem();
	
		void stepNonLogic() override;
		bool doesStepNonLogic() const override { return true; }
	
	protected:
		/**
		 * Add / remove pins according to the number of outputs the user has requested
		 */
		void initPins() override;
	
		LogicIn *m_logic[max_ADDAC_bits];
		VoltagePoint *m_voltagePoint;
};


#endif
