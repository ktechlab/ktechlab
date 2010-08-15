/***************************************************************************
 *   Copyright (C) 2003 by David Saxton                                    *
 *   david@bluehaze.org                                                    *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#include "end.h"

#include "libraryitem.h"
#include "flowcode.h"

#include <klocale.h>

Item* End::construct( ItemDocument *itemDocument, bool newItem, const char *id )
{
	return new End( (ICNDocument*)itemDocument, newItem, id );
}

LibraryItem* End::libraryItem()
{
	return new LibraryItem(
		"flow/end",
		i18n("End"),
		i18n("Common"),
		"end.png",
		LibraryItem::lit_flowpart,
		End::construct );
}

End::End( ICNDocument *icnDocument, bool newItem, const char *id )
	: FlowPart( icnDocument, newItem, id ? id : "END" )
{
	m_name = i18n("End");
	initRoundedRectSymbol();
	createStdInput();
	setCaption( i18n("End") );
}

End::~End()
{
}

void End::generateMicrobe( FlowCode */*code*/ )
{
}

