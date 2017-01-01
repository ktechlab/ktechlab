/***************************************************************************
 *   Copyright (C) 2003 by David Saxton                                    *
 *   david@bluehaze.org                                                    *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#include "sevenseg.h"

#include "libraryitem.h"
#include "flowcode.h"

#include <klocale.h>

Item* SevenSeg::construct( ItemDocument *itemDocument, bool newItem, const char *id )
{
	return new SevenSeg( (ICNDocument*)itemDocument, newItem, id );
}


LibraryItem* SevenSeg::libraryItem()
{
	return new LibraryItem(
		QStringList(QString("flow/sevenseg")),
		i18n("Seven Segment"),
		i18n("Functions"),
		"seven_segment.png",
		LibraryItem::lit_flowpart,
		SevenSeg::construct
			);
}


SevenSeg::SevenSeg( ICNDocument *icnDocument, bool newItem, const char *id )
	: FlowPart( icnDocument, newItem, id ? id : "sevenseg" )
{
	m_name = i18n("SevenSeg");
	initProcessSymbol();
	createStdInput();
	createStdOutput();
	
	createProperty( "expression", Variant::Type::Combo );
	property("expression")->setValue("x");
	property("expression")->setCaption( i18n("Variable") );
	
	createProperty( "sevenseg", Variant::Type::SevenSegment );
	property("sevenseg")->setCaption( i18n("Pin map") );
}


SevenSeg::~SevenSeg()
{
}


void SevenSeg::dataChanged()
{
	setCaption( i18n("Display %1 on %2", dataString("expression"), dataString("sevenseg") ) );
}


void SevenSeg::generateMicrobe( FlowCode *code )
{
	code->addCode( QString("%1 = %2").arg( dataString("sevenseg") ).arg( dataString("expression") ) );
	code->addCodeBranch( outputPart("stdoutput") );
}

