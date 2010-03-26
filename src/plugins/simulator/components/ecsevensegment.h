/***************************************************************************
 *   Copyright (C) 2003-2005 by David Saxton                               *
 *   david@bluehaze.org                                                    *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#ifndef ECSEVENSEGMENT_H
#define ECSEVENSEGMENT_H

#include "dipcomponent.h"

class Diode;

/**
@short Seven segment display component
@author David Saxton
*/
class ECSevenSegment : public DIPComponent {

public:
	ECSevenSegment(ICNDocument *icnDocument, bool newItem, const char *id = 0);
	~ECSevenSegment();

	static Item* construct(ItemDocument *itemDocument, bool newItem, const char *id);
	static LibraryItem *libraryItem();

	virtual void stepNonLogic();
	virtual bool doesStepNonLogic() const {
		return true;
	}

	virtual void dataChanged();

private:
	virtual void drawShape(QPainter &p);

	bool m_bCommonCathode;
	uint lastUpdatePeriod;
	uint avg_brightness[8];
	uint last_brightness[8];
	Diode *m_diodes[8];
	ECNode *m_nodes[8];
	ECNode *m_nNode;
	float r, g, b;
};

#endif
