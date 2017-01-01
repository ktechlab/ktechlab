/***************************************************************************
 *   Copyright (C) 2003 by David Saxton                                    *
 *   david@bluehaze.org                                                    *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#include "start.h"

#include "libraryitem.h"
#include "flowcode.h"

#include <klocalizedstring.h>

Item* Start::construct( ItemDocument *itemDocument, bool newItem, const char *id )
{
	return new Start( (ICNDocument*)itemDocument, newItem, id );
}

LibraryItem* Start::libraryItem()
{
	return new LibraryItem(
		QStringList(QString("flow/start")),
		i18n("Start"),
		i18n("Common"),
		"start.png",
		LibraryItem::lit_flowpart,
		Start::construct );
}

Start::Start( ICNDocument *icnDocument, bool newItem, const char *id )
	: FlowPart( icnDocument, newItem, id ? id : "START" )
{
	m_name = i18n("Start");
	initRoundedRectSymbol();
	createStdOutput();
	setCaption( i18n("Start") );
}

Start::~Start()
{
}

void Start::generateMicrobe( FlowCode *code )
{
	code->addCodeBranch( outputPart("stdoutput") );
}

