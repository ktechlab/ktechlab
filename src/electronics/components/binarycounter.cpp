/***************************************************************************
 *   Copyright (C) 2003-2005 by David Saxton                               *
 *   david@bluehaze.org                                                    *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#include "binarycounter.h"

#include <cstdlib>
#include <kiconloader.h>
#include <klocale.h>

#include "logic.h"
#include "libraryitem.h"

Item* BinaryCounter::construct(ItemDocument *itemDocument, bool newItem, const char *id) {
	return new BinaryCounter((ICNDocument*)itemDocument, newItem, id);
}

LibraryItem* BinaryCounter::libraryItem() {
	QStringList ids;
	ids << "ec/binary_counter" << "ec/4_bit_binary_counter";
	return new LibraryItem(
	           ids,
	           i18n("Binary Counter"),
	           i18n("Integrated Circuits"),
	           "ic1.png",
	           LibraryItem::lit_component,
	           BinaryCounter::construct);
}

BinaryCounter::BinaryCounter(ICNDocument *icnDocument, bool newItem, const char *id)
		: DIPComponent(icnDocument, newItem, id ? id : "binary_counter") {
	m_name = i18n("Binary Counter");

	enLogic = inLogic = rLogic = udLogic = 0L;

	b_triggerHigh = true;
	b_oldIn = false;
	m_value = 0;
	m_numBits = 0;
	m_bDoneLogicIn = false;

	createProperty("trig", Variant::Type::Select);
	property("trig")->setCaption(i18n("Trigger Edge"));
	QStringMap allowed;
	allowed["Rising"] = i18n("Rising");
	allowed["Falling"] = i18n("Falling");
	property("trig")->setAllowed(allowed);
	property("trig")->setValue("Falling");

	createProperty("bitcount", Variant::Type::Int);
	property("bitcount")->setCaption(i18n("Bit Count"));
	property("bitcount")->setMinValue(1);
	property("bitcount")->setMaxValue(26);
	property("bitcount")->setValue(4);
}

BinaryCounter::~BinaryCounter() {
}

void BinaryCounter::dataChanged() {
	initPins(dataInt("bitcount"));

	b_triggerHigh = dataString("trig") == "Rising";
	setDisplayText(">", b_triggerHigh ? "^>" : "_>");
}

void BinaryCounter::initPins(unsigned numBits) {
	if (m_numBits == numBits)
		return;

	QStringList pins;

	pins << "en" << ">" << "u/d" << "r";

	{
		int np = int(numBits) - 4;

		if (np > 0) {
			for (int i = 0; i < np; i++)
				pins << " ";
		}
	}

	for (int i = numBits - 1; i >= 0; i--)
		pins << QChar('A' + i);

	initDIPSymbol(pins, 64);
	initDIP(pins);

	if (m_numBits < numBits) {
		for (unsigned i = m_numBits; i < numBits; i++) {
			m_pLogicOut[i] = new LogicOut(LogicIn::getConfig(), false);
			setup1pinElement(m_pLogicOut[i], ecNodeWithID(QChar('A' + i))->pin());
		}
	} else {
		for (unsigned i = numBits; i < m_numBits; i++) {
			QString id = QChar('A' + i);
			removeElement(m_pLogicOut[i], false);
			removeDisplayText(id);
			removeNode(id);
		}
	}

	m_numBits = numBits;

	if (!m_bDoneLogicIn) {
		enLogic = new LogicIn(LogicIn::getConfig());
		setup1pinElement(enLogic, ecNodeWithID("en")->pin());

		inLogic = new LogicIn(LogicIn::getConfig());
		setup1pinElement(inLogic, ecNodeWithID(">")->pin());
		inLogic->setCallback(this, (CallbackPtr)(&BinaryCounter::inStateChanged));

		rLogic = new LogicIn(LogicIn::getConfig());
		setup1pinElement(rLogic, ecNodeWithID("r")->pin());
		rLogic->setCallback(this, (CallbackPtr)(&BinaryCounter::rStateChanged));

		udLogic = new LogicIn(LogicIn::getConfig());
		setup1pinElement(udLogic, ecNodeWithID("u/d")->pin());

		m_bDoneLogicIn = true;
	}

	outputValue();
}

void BinaryCounter::inStateChanged(bool state) {
	if ((state != b_oldIn) && enLogic->isHigh() && !rLogic->isHigh() && state == b_triggerHigh) {
		if (udLogic->isHigh()) m_value++;
		else m_value--;

		m_value &= (1 << m_numBits) - 1;

		outputValue();
	}

	b_oldIn = state;
}

void BinaryCounter::rStateChanged(bool state) {

	if (state) {
		m_value = 0;
		outputValue();
	}
}

void BinaryCounter::outputValue() {
	for (unsigned i = 0; i < m_numBits; i++)
		m_pLogicOut[i]->setHigh(m_value & (1 << i));
}
