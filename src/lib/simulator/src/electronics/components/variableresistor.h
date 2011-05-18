/***************************************************************************
 *   Copyright (C) 2006 by William Hillerby - william.hillerby@ntlworld.com*
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#ifndef VARIABLERESISTOR_H
#define VARIABLERESISTOR_H

#include "simplecomponent.h"
#include "resistance.h"

class Resistance;
class QSlider;

/**
@author William Hillerby
*/

class VariableResistor : public SimpleComponent
{
	public:
		VariableResistor( ICNDocument* icnDocument, bool newItem, const QString& id = 0L );
		~VariableResistor();
	
		static Item* construct( ItemDocument *itemDocument, bool newItem, const char *id );
		static LibraryItem *libraryItem();
		
		virtual void sliderValueChanged( const QString &id, int newValue );
		
	private:
		void dataChanged();
		virtual void drawShape( QPainter &p );
		
		Resistance m_pResistance;
		QSlider *m_pSlider;
		double m_minResistance;
		double m_maxResistance;
		double m_currResistance;
		double m_tickValue;
};

#endif
