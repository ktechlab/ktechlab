/***************************************************************************
 *   Copyright (C) 2005 by Fredy Yanardi                                   *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#ifndef MAGNITUDECOMPARATOR_H
#define MAGNITUDECOMPARATOR_H

#include "component.h"
#include "logic.h"

#include <qbitarray.h>
#include <qptrvector.h>

/**
@author Fredy Yanardi
 */
class MagnitudeComparator : public CallbackClass, public Component
{
	public:
		MagnitudeComparator( ICNDocument *icnDocument, bool newItem, const char *id = 0L );
		~MagnitudeComparator();
	
		static Item * construct( ItemDocument *itemDocument, bool newItem, const char *id );
		static LibraryItem * libraryItem();
	
	protected:
		void initPins();
		virtual void dataChanged();
		void inStateChanged();
		
		int m_oldABLogicCount;
		int cascadingInputs;
		int outputs;
		bool firstTime;
	
		QBitArray m_data;
		
		QPtrVector<LogicIn> m_aLogic;
		QPtrVector<LogicIn> m_bLogic;
		QPtrVector<LogicIn> m_cLogic;
		QPtrVector<LogicOut> m_output;
};

#endif
