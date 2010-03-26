/***************************************************************************
 *   Copyright (C) 2004 by David Saxton                                    *
 *   david@bluehaze.org                                                    *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#include "fulladder.h"

#include "logic.h"
#include "libraryitem.h"

#include <kiconloader.h>
#include <klocale.h>

Item* FullAdder::construct( ItemDocument *itemDocument, bool newItem, const char *id )
{
	return new FullAdder( (ICNDocument*)itemDocument, newItem, id );
}

LibraryItem* FullAdder::libraryItem()
{
	return new LibraryItem(
		"ec/adder",
		i18n("Adder"),
		i18n("Integrated Circuits"),
		"ic1.png",
		LibraryItem::lit_component,
		FullAdder::construct);
}

FullAdder::FullAdder(ICNDocument *icnDocument, bool newItem, const char *id)
	: DIPComponent(icnDocument, newItem, (id) ? id : "adder")
{
	m_name = i18n("Adder");

	QStringList pins = QStringList::split(',', "A,B,>,,S,C", true);
	initDIPSymbol(pins, 48);
	initDIP(pins);

	setup1pinElement(SLogic,  ecNodeWithID("S")->pin());
	setup1pinElement(outLogic, ecNodeWithID("C")->pin());
	setup1pinElement(ALogic,  ecNodeWithID("A")->pin());
	setup1pinElement(BLogic,  ecNodeWithID("B")->pin());
	setup1pinElement(inLogic, ecNodeWithID(">")->pin());

	ALogic.setCallback(this, (CallbackPtr)(&FullAdder::inStateChanged));
	BLogic.setCallback(this, (CallbackPtr)(&FullAdder::inStateChanged));
	inLogic.setCallback(this, (CallbackPtr)(&FullAdder::inStateChanged));
}

FullAdder::~FullAdder() {}

void FullAdder::inStateChanged( bool /*state*/ )
{
	unsigned char A = ALogic.isHigh() 
		+ BLogic.isHigh() 
		+ inLogic.isHigh();
	
	SLogic.setHigh(A & 1); // result
	outLogic.setHigh(A >> 1);  // carry
}


