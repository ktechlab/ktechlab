/***************************************************************************
 *   Copyright (C) 2005 by David Saxton                                    *
 *   david@bluehaze.org                                                    *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#ifndef BUSSPLITTER_H
#define BUSSPLITTER_H

#include "simplecomponent.h"
#include <vector>

class Wire;

/**
@author David Saxton
*/
class BusSplitter : public SimpleComponent
{
public:
	BusSplitter(ICNDocument *icnDocument, bool newItem, const char *id = 0);
	~BusSplitter();

	static Item* construct(ItemDocument *itemDocument, bool newItem, const char *id);
	static LibraryItem *libraryItem();

protected:
	QString outNodeID(unsigned node) const;
	virtual void dataChanged();
	virtual void drawShape(QPainter &p);
	
	unsigned m_busSize;	
	std::vector<Wire *> m_pWires; // internal wires! =P
	ECNode *m_pInNode; // Should be PinNode
};

#endif
