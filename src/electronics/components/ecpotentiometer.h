/***************************************************************************
 *   Copyright (C) 2003-2005 by David Saxton                               *
 *   david@bluehaze.org                                                    *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#ifndef ECPOTENTIOMETER_H
#define ECPOTENTIOMETER_H

#include "component.h"

class QSlider;

/**
@short Potentiometer
@author David Saxton
*/
class ECPotentiometer : public Component
{
public:
	ECPotentiometer( ICNDocument *icnDocument, bool newItem, const char *id = 0L );
	~ECPotentiometer() override;
	
	static Item* construct( ItemDocument *itemDocument, bool newItem, const char *id );
	static LibraryItem *libraryItem();
	
	void sliderValueChanged( const QString &id, int newValue ) override;
	
private:
	void dataChanged() override;
	void drawShape( QPainter &p ) override;
	
	ECNode * m_p1;
	Resistance *m_r1, *m_r2;
	double m_resistance;
	double m_sliderProp;
	QSlider * m_pSlider;
};
#endif
