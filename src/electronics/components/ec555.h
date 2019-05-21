/***************************************************************************
 *   Copyright (C) 2003 by David Saxton                                    *
 *   david@bluehaze.org                                                    *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#ifndef EC555_H
#define EC555_H

#include "component.h"

#include <qstringlist.h>

/**
@short 555 IC
@author David Saxton
*/
class EC555 : public Component
{
public:
	EC555( ICNDocument *icnDocument, bool newItem, const char *id = 0L );
	~EC555() override;
	
	static Item* construct( ItemDocument *itemDocument, bool newItem, const char *id );
	static LibraryItem *libraryItem();
	
	void stepNonLogic() override;
	bool doesStepNonLogic() const override { return true; }
	
private:
	Pin * ground;
	Pin * trigger;
	Pin * output;
	Pin * reset;
	Pin * control;
	Pin * threshold;
	Pin * discharge;
	Pin * vcc;

	Resistance *m_r1;
	Resistance *m_r23;
	Resistance *m_po_sink;
	Resistance *m_po_source;
	Resistance *m_r_discharge;
	
	bool old_com1;
	bool old_com2;
	bool old_q;
};

#endif
