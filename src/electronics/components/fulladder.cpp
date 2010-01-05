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
		FullAdder::construct
			);
}

FullAdder::FullAdder(ICNDocument *icnDocument, bool newItem, const char *id)
	: Component(icnDocument, newItem, (id) ? id : "adder")
{
	m_name = i18n("Adder");

	ALogic = BLogic = inLogic = 0;
	outLogic = SLogic = 0;

	QStringList pins = QStringList::split(',', "A,B,>,,S,C", true);
	initDIPSymbol(pins, 48);
	initDIP(pins);
	
	ECNode *node;
	
	node =  ecNodeWithID("S");
	SLogic = createLogicOut(node->pin(), false);
	
	node = ecNodeWithID("C");
	outLogic = createLogicOut(node->pin(), false);
	
	node = ecNodeWithID("A");
	ALogic = createLogicIn(node->pin());
	
	node = ecNodeWithID("B");
	BLogic = createLogicIn(node->pin());
	
	node = ecNodeWithID(">");
	inLogic = createLogicIn(node->pin());
	
	ALogic->setCallback(this, (CallbackPtr)(&FullAdder::inStateChanged));
	BLogic->setCallback(this, (CallbackPtr)(&FullAdder::inStateChanged));
	inLogic->setCallback(this, (CallbackPtr)(&FullAdder::inStateChanged));
}

FullAdder::~FullAdder()
{
}


void FullAdder::inStateChanged( bool /*state*/ )
{
	const bool A = ALogic->isHigh();
	const bool B = BLogic->isHigh();
	const bool in = inLogic->isHigh();
	
	const bool out = (!A && B && in) || (A && !B && in) || (A && B);
	const bool S = (!A && !B && in) || (!A && B && !in) || (A && !B && !in) || (A && B && in);
	
	SLogic->setHigh(S);
	outLogic->setHigh(out);
}


