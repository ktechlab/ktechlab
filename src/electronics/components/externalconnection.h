/***************************************************************************
 *   Copyright (C) 2004 by David Saxton                                    *
 *   david@bluehaze.org                                                    *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#ifndef EXTERNALCONNECTION_H
#define EXTERNALCONNECTION_H

#include <component.h>

/**
For connecting to something "outside" - e.g. a mechanical component, or as part
of a circuit part
@author David Saxton
*/
class ExternalConnection : public Component
{
public:
	ExternalConnection( ICNDocument *icnDocument, bool newItem, const char *id = nullptr );
	~ExternalConnection() override;
	
	static Item* construct( ItemDocument *itemDocument, bool newItem, const char *id );
	static LibraryItem *libraryItem();

private:
	void dataChanged() override;
	void drawShape( QPainter &p ) override;
};

#endif
