/***************************************************************************
 *   Copyright (C) 2003-2004 by David Saxton                               *
 *   david@bluehaze.org                                                    *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#include <cassert>

#include "ecbcdto7segment.h"

#include "logic.h"
#include "libraryitem.h"

#include <kiconloader.h>
#include <klocale.h>

// Values for a,b,c,d,e,f,g of common-anode 7 segment display
static bool numbers[16][7] = { 
	{ 1, 1, 1, 1, 1, 1, 0 }, // 0
	{ 0, 1, 1, 0, 0, 0, 0 }, // 1
	{ 1, 1, 0, 1, 1, 0, 1 }, // 2
	{ 1, 1, 1, 1, 0, 0, 1 }, // 3
	{ 0, 1, 1, 0, 0, 1, 1 }, // 4
	{ 1, 0, 1, 1, 0, 1, 1 }, // 5
	{ 1, 0, 1, 1, 1, 1, 1 }, // 6
	{ 1, 1, 1, 0, 0, 0, 0 }, // 7
	{ 1, 1, 1, 1, 1, 1, 1 }, // 8
	{ 1, 1, 1, 0, 0, 1, 1 }, // 9
	{ 1, 1, 1, 0, 1, 1, 1 }, // A
	{ 0, 0, 1, 1, 1, 1, 1 }, // b
	{ 1, 0, 0, 1, 1, 1, 0 }, // C
	{ 0, 1, 1, 1, 1, 0, 1 }, // d
	{ 1, 0, 0, 1, 1, 1, 1 }, // E
	{ 1, 0, 0, 0, 1, 1, 1 }}; // F

Item* ECBCDTo7Segment::construct(ItemDocument *itemDocument, bool newItem, const char *id) {
	return new ECBCDTo7Segment((ICNDocument*)itemDocument, newItem, id);
}

LibraryItem* ECBCDTo7Segment::libraryItem() {
	return new LibraryItem(
	           "ec/bcd_to_seven_segment",
	           i18n("BCD to 7 Segment"),
	           i18n("Integrated Circuits"),
	           "ic2.png",
	           LibraryItem::lit_component,
	           ECBCDTo7Segment::construct);
}

ECBCDTo7Segment::ECBCDTo7Segment(ICNDocument *icnDocument, bool newItem, const char *id)
		: DIPComponent(icnDocument, newItem, id ? id : "bcd_to_seven_segment") {
	m_name = i18n("BCD to Seven Segment");

	QStringList pins = QStringList::split(',', "A,B,C,D,,lt,rb,en,d,e,f,g,,a,b,c", true);

	initDIPSymbol(pins, 48);
	initDIP(pins);

	setup1pinElement(ALogic, ecNodeWithID("A")->pin());
	setup1pinElement(BLogic, ecNodeWithID("B")->pin());
	setup1pinElement(CLogic, ecNodeWithID("C")->pin());
	setup1pinElement(DLogic, ecNodeWithID("D")->pin());
	setup1pinElement(ltLogic, ecNodeWithID("lt")->pin());
	setup1pinElement(rbLogic, ecNodeWithID("rb")->pin());
	setup1pinElement(enLogic, ecNodeWithID("en")->pin());

	ALogic.setCallback(this, (CallbackPtr)(&ECBCDTo7Segment::inStateChanged));
	BLogic.setCallback(this, (CallbackPtr)(&ECBCDTo7Segment::inStateChanged));
	CLogic.setCallback(this, (CallbackPtr)(&ECBCDTo7Segment::inStateChanged));
	DLogic.setCallback(this, (CallbackPtr)(&ECBCDTo7Segment::inStateChanged));
	ltLogic.setCallback(this, (CallbackPtr)(&ECBCDTo7Segment::inStateChanged));
	rbLogic.setCallback(this, (CallbackPtr)(&ECBCDTo7Segment::inStateChanged));
	enLogic.setCallback(this, (CallbackPtr)(&ECBCDTo7Segment::inStateChanged));

	for (uint i = 0; i < 7; ++i) {
		outLogic[i] = new LogicOut(LogicConfig(), false);
		setup1pinElement(*(outLogic[i]), ecNodeWithID(QChar('a' + i))->pin());
	}

	inStateChanged(false);
}

ECBCDTo7Segment::~ECBCDTo7Segment() {}

void ECBCDTo7Segment::inStateChanged(bool) {

	unsigned char n = ALogic.isHigh() | 
			 (BLogic.isHigh() << 1) |
			 (CLogic.isHigh() << 2) |
			 (DLogic.isHigh() << 3);

	if(!ltLogic.isHigh()) { 
		if(!rbLogic.isHigh()) {
			if(enLogic.isHigh()) { // Enable (store)
				for (int i = 0; i < 7; i++) {
					outLogic[i]->setHigh(numbers[n][i]);
				}
			}
		} else { // Ripple Blank
			for (int i = 0; i < 7; i++) 
				outLogic[i]->setHigh(false);
		}
	} else { // Lamp test
		for (int i = 0; i < 7; i++)
			outLogic[i]->setHigh(true);
	}
}
