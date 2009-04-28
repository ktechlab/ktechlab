/***************************************************************************
 *   Copyright (C) 2005 by David Saxton                                    *
 *   david@bluehaze.org                                                    *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#ifndef PARALLELPORTCOMPONENT_H
#define PARALLELPORTCOMPONENT_H

#include "logic.h"
#include "dsubcon.h"

class ParallelPort;

/**
@author David Saxton 
 */
class ParallelPortComponent : public CallbackClass, public DSubCon
{
	public:
		ParallelPortComponent( ICNDocument *icnDocument, bool newItem, const char *id = 0L );
		~ParallelPortComponent();
	
		static Item * construct( ItemDocument *itemDocument, bool newItem, const char *id );
		static LibraryItem * libraryItem();
		
		virtual void stepNonLogic();
		virtual bool doesStepNonLogic() const { return true; }
	
	protected:
		void initPort( const QString & port );
		virtual void dataChanged();
		virtual void drawShape( QPainter & p );
		
		void dataCallback( bool );
		void controlCallback( bool );
		
		/// Registers: { Data[0...7], Status[0...5], 0[6...7], Control[0...4], 0[5...7] }
		LogicOut * m_pLogic[24];
		
		ParallelPort * m_pParallelPort;
};

#endif
