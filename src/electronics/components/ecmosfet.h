/***************************************************************************
 *   Copyright (C) 2005 by David Saxton                                    *
 *   david@bluehaze.org                                                    *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#ifndef ECMOSFET_H
#define ECMOSFET_H

#include "component.h"

class MOSFET;

/**
@short Simulates a MOSFET
@author David Saxton
 */
class ECMOSFET : public Component
{
	public:
		ECMOSFET( int MOSFET_type, ICNDocument *icnDocument, bool newItem, const char * id = 0L );
		~ECMOSFET();

		static Item * constructNEM( ItemDocument * itemDocument, bool newItem, const char * id );
		static Item * constructPEM( ItemDocument * itemDocument, bool newItem, const char * id );
// 		static Item * constructNDM( ItemDocument * itemDocument, bool newItem, const char * id );
// 		static Item * constructPDM( ItemDocument * itemDocument, bool newItem, const char * id );
		static LibraryItem * libraryItemNEM();
		static LibraryItem * libraryItemPEM();
// 		static LibraryItem * libraryItemNDM();
// 		static LibraryItem * libraryItemPDM();

	protected:
		virtual void dataChanged() override;
		virtual void drawShape( QPainter &p ) override;

		bool m_bHaveBodyPin;
		int m_MOSFET_type;
		MOSFET * m_pMOSFET;
};
#endif
