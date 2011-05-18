/***************************************************************************
 *   Copyright (C) 2005 by David Saxton                                    *
 *   david@bluehaze.org                                                    *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#include "ecnode.h"
#include "flipflop.h"
#include "logic.h"
#include "libraryitem.h"
#include "simulator.h"

#include <kiconloader.h>
#include <klocale.h>
#include <qpainter.h>

//BEGIN class ClockedLogic
ClockedLogic::ClockedLogic(ICNDocument *icnDocument, bool newItem, const char *id)
		: SimpleComponent(icnDocument, newItem, id), m_bPrevClock(false) {
	createProperty("trig", Variant::Type::Select);
	property("trig")->setCaption(i18n("Trigger Edge"));
	QStringMap allowed;
	allowed["Rising"] = i18n("Rising");
	allowed["Falling"] = i18n("Falling");
	property("trig")->setAllowed(allowed);
	property("trig")->setValue("Rising");
	m_edgeTrigger = Rising;
}

void ClockedLogic::dataChanged() {
	EdgeTrigger t = (dataString("trig") == "Rising") ? Rising : Falling;

	if (t == m_edgeTrigger)
		return;

	m_edgeTrigger = t;

	initSymbolFromTrigger();
}
//END class ClockedLogic

//BEGIN class ECDFlipFlop
Item* ECDFlipFlop::construct(ItemDocument *itemDocument, bool newItem, const char *id) {
	return new ECDFlipFlop((ICNDocument*)itemDocument, newItem, id);
}

LibraryItem* ECDFlipFlop::libraryItem() {
	return new LibraryItem(
	           "ec/d_flipflop",
	           i18n("D Flip-Flop"),
	           i18n("Integrated Circuits"),
	           "ic3.png",
	           LibraryItem::lit_component,
	           ECDFlipFlop::construct);
}

ECDFlipFlop::ECDFlipFlop(ICNDocument *icnDocument, bool newItem, const char *id)
		: ClockedLogic(icnDocument, newItem, id ? id : "d_flipflop"), m_prevDChangeSimTime(0) {
	m_name = i18n("D-Type Flip-Flop");

	setSize(-32, -24, 64, 48);
	init2PinLeft(-8, 8);
	init2PinRight(-8, 8);
	initSymbolFromTrigger();

	m_prevD = false;
	m_pSimulator = Simulator::self();

	setup1pinElement(m_pD, m_pNNode[0]->pin());
	setup1pinElement(m_pQ, m_pPNode[0]->pin());
	m_pQ.setHigh(true);

	setup1pinElement(m_pClock, m_pNNode[1]->pin());
	setup1pinElement(m_pQBar, m_pPNode[1]->pin());
	setup1pinElement(setp, createPin(0, -32, 90, "set")->pin());
	setup1pinElement(rstp, createPin(0, 32, 270, "rst")->pin());

	// (The display text for D, >, Set, Rst is set in initSymbolFromTrigger
	addDisplayText("Q",  QRect(12, -16, 20, 16), "Q");
	addDisplayText("Q'", QRect(12,   0, 20, 16), "Q'");

	m_pD.setCallback(this, (CallbackPtr)(&ECDFlipFlop::inputChanged));
	m_pClock.setCallback(this, (CallbackPtr)(&ECDFlipFlop::clockChanged));
	setp.setCallback(this, (CallbackPtr)(&ECDFlipFlop::asyncChanged));
	rstp.setCallback(this, (CallbackPtr)(&ECDFlipFlop::asyncChanged));
}

ECDFlipFlop::~ECDFlipFlop() {
}

void ECDFlipFlop::initSymbolFromTrigger() {
	int offset = (m_edgeTrigger == Rising) ? 0 : 6;

	int w = 64 - offset;
	setSize(offset - 32, -24, w, 48, true);
	m_pNNode[0]->setLength(8 + offset);
	addDisplayText("D",   QRect(offset - 28, -16,	 20, 16), "D",   true, Qt::AlignLeft);
	addDisplayText(">",   QRect(offset - 28,   0,	 20, 16), ">",   true, Qt::AlignLeft);
	addDisplayText("Set", QRect(offset - 28, -20, w - 8, 16), "Set", true, Qt::AlignHCenter);
	addDisplayText("Rst", QRect(offset - 28,   4, w - 8, 16), "Rst", true, Qt::AlignHCenter);

	updateAttachedPositioning();
}

void ECDFlipFlop::drawShape(QPainter &p) {
	Component::drawShape(p);

	if (m_edgeTrigger == Falling) {
		initPainter(p);
		p.drawEllipse(int(x() - 32), int(y() + 5), 6, 6);
		deinitPainter(p);
	}
}

void ECDFlipFlop::asyncChanged(bool) {
	bool set = setp.isHigh();
	bool rst = rstp.isHigh();

	if (set || rst) {
		m_pQ.setHigh(set);
		m_pQBar.setHigh(rst);
	}
}

void ECDFlipFlop::inputChanged(bool newState) {
	if (newState == m_prevD) {
		// Only record the time that the input state changes
		return;
	}

	m_prevD = newState;
	m_prevDChangeSimTime = m_pSimulator->time();
}

void ECDFlipFlop::clockChanged(bool newState) {

	bool edge = (m_edgeTrigger == Falling) == (m_bPrevClock && !newState);

	m_bPrevClock = newState;

	if(setp.isHigh() || rstp.isHigh()) return;

	if (edge) {
		// The D Flip-Flop takes the input before the edge fall/rise - not after
		// the edge. So see if the input state last changed before now or at
		// now to work out if we should take the current value of m_prevD, or
		// its inverse.

		unsigned long long simTime = m_pSimulator->time();
		bool d = (simTime == m_prevDChangeSimTime) ? !m_prevD : m_prevD;

		m_pQ.setHigh(d);
		m_pQBar.setHigh(!d);
	}
}
//END class ECDFlipFlop

//BEGIN class ECJKFlipFlop
Item *ECJKFlipFlop::construct(ItemDocument *itemDocument, bool newItem, const char *id) {
	return new ECJKFlipFlop((ICNDocument*)itemDocument, newItem, id);
}

LibraryItem* ECJKFlipFlop::libraryItem() {
	return new LibraryItem(
	           "ec/jk_flipflop",
	           i18n("JK Flip-Flop"),
	           i18n("Integrated Circuits"),
	           "ic3.png",
	           LibraryItem::lit_component,
	           ECJKFlipFlop::construct);
}

ECJKFlipFlop::ECJKFlipFlop(ICNDocument *icnDocument, bool newItem, const char *id)
		: ClockedLogic(icnDocument, newItem, id ? id : "jk_flipflop") {
	m_name = i18n("JK-Type Flip-Flop");

	setSize(-32, -32, 64, 64);
	init3PinLeft(-16, 0, 16);
	init2PinRight(-16, 16);
	initSymbolFromTrigger();

	m_bPrevClock = false;

	initSymbolFromTrigger();

	setup1pinElement(m_pJ, m_pNNode[0]->pin());
	setup1pinElement(m_pQ, m_pPNode[0]->pin());
	m_pQ.setState(true);

	setup1pinElement(m_pClock, m_pNNode[1]->pin());
	setup1pinElement(m_pQBar, m_pPNode[1]->pin());
	setup1pinElement(m_pK, m_pNNode[2]->pin());
	setup1pinElement(setp, createPin(0, -40, 90, "set")->pin());
	setup1pinElement(rstp, createPin(0, 40, 270, "rst")->pin());

	addDisplayText("Q",  QRect(12, -24, 20, 16), "Q");
	addDisplayText("Q'", QRect(12,   8, 20, 16), "Q'");

	m_pClock.setCallback(this, (CallbackPtr)(&ECJKFlipFlop::clockChanged));
	setp.setCallback(this, (CallbackPtr)(&ECJKFlipFlop::asyncChanged));
	rstp.setCallback(this, (CallbackPtr)(&ECJKFlipFlop::asyncChanged));
}

ECJKFlipFlop::~ECJKFlipFlop() {
}

void ECJKFlipFlop::initSymbolFromTrigger() {
	int offset = (m_edgeTrigger == Rising) ? 0 : 6;

	int w = 64 - offset;
	setSize(offset - 32, -32, w, 64, true);
	m_pNNode[0]->setLength(8 + offset);
	m_pNNode[2]->setLength(8 + offset);
	addDisplayText("J",   QRect(offset - 28, -24,    20, 16), "J",   true, Qt::AlignLeft);
	addDisplayText(">",   QRect(offset - 28,  -8,    20, 16), ">",   true, Qt::AlignLeft);
	addDisplayText("K",   QRect(offset - 28,   8,    20, 16), "K",   true, Qt::AlignLeft);
	addDisplayText("Set", QRect(offset - 28, -28, w - 8, 16), "Set", true, Qt::AlignHCenter);
	addDisplayText("Rst", QRect(offset - 28,  12, w - 8, 16), "Rst", true, Qt::AlignHCenter);

	updateAttachedPositioning();
}

void ECJKFlipFlop::drawShape(QPainter &p) {
	Component::drawShape(p);

	if (m_edgeTrigger == Falling) {
		initPainter(p);
		p.drawEllipse(int(x() - 32), int(y() - 3), 6, 6);
		deinitPainter(p);
	}
}

void ECJKFlipFlop::clockChanged(bool newvalue) {
	bool edge = (m_edgeTrigger == Falling) == (m_bPrevClock && !newvalue);
	m_bPrevClock = newvalue;

	if(setp.isHigh() || rstp.isHigh()) return;

	if(edge) {
		bool j = m_pJ.isHigh();
		bool k = m_pK.isHigh();

		m_pQ.setHigh((j && m_pQBar.isHigh()) || (!k && m_pQ.isHigh()));
		m_pQBar.setHigh(!m_pQ.isHigh());
	}
}

void ECJKFlipFlop::asyncChanged(bool) {
	bool set = setp.isHigh();
	bool rst = rstp.isHigh();

	if(set || rst) {
		m_pQ.setHigh(set);
		m_pQBar.setHigh(rst);
	}
}
//END class ECJKFlipFlop

//BEGIN class ECSRFlipFlop
Item *ECSRFlipFlop::construct(ItemDocument *itemDocument, bool newItem, const char *id) {
	return new ECSRFlipFlop((ICNDocument*)itemDocument, newItem, id);
}

LibraryItem *ECSRFlipFlop::libraryItem() {
	return new LibraryItem(
	           "ec/sr_flipflop",
	           i18n("SR Flip-Flop"),
	           i18n("Integrated Circuits"),
	           "ic3.png",
	           LibraryItem::lit_component,
	           ECSRFlipFlop::construct);
}

ECSRFlipFlop::ECSRFlipFlop(ICNDocument *icnDocument, bool newItem, const char *id)
		: SimpleComponent(icnDocument, newItem, id ? id : "sr_flipflop") {

	m_pol = false;

	m_name = i18n("SR Flip-Flop");

	createProperty("polarity", Variant::Type::Bool);
	property("polarity")->setCaption(i18n("NAND type"));
	property("polarity")->setValue(false);

	setSize(-24, -24, 48, 48);

	init2PinLeft(-8, 8);
	init2PinRight(-8, 8);

	setup1pinElement(m_pS, m_pNNode[0]->pin());
	setup1pinElement(m_pR, m_pNNode[1]->pin());
	setup1pinElement(m_pQ, m_pPNode[0]->pin());
	setup1pinElement(m_pQBar, m_pPNode[1]->pin());

	m_pQ.setHigh(true);
	m_pQBar.setHigh(false);

	addDisplayText("S",  QRect(-24, -16, 20, 16), "S");
	addDisplayText("R",  QRect(-24,   0, 20, 16), "R");
	addDisplayText("Q",  QRect(4,   -16, 20, 16), "Q");
	addDisplayText("Q'", QRect(4,     0, 20, 16), "Q'");

	m_pS.setCallback(this, (CallbackPtr)(&ECSRFlipFlop::inStateChanged));
	m_pR.setCallback(this, (CallbackPtr)(&ECSRFlipFlop::inStateChanged));

// this type of flip-flop is sensitive to whether the load on the output overcomes its output drive.
// we also sometimes need two iterations to settle out our state...
	m_pQ.setCallback(this, (CallbackPtr)(&ECSRFlipFlop::inStateChanged));
	m_pQBar.setCallback(this, (CallbackPtr)(&ECSRFlipFlop::inStateChanged));
}

ECSRFlipFlop::~ECSRFlipFlop() {}

void ECSRFlipFlop::dataChanged() {
	m_pol = dataBool("polarity");
	inStateChanged(false);
}

void ECSRFlipFlop::inStateChanged(bool) {

	bool s = m_pS.isHigh() ^ m_pol;
	bool r = m_pR.isHigh() ^ m_pol;

	m_pQ.setHigh(!(m_pQBar.isHigh() || r));
	m_pQBar.setHigh(!(m_pQ.isHigh() || s));
	m_pQ.setHigh(!(m_pQBar.isHigh() || r));

//I think that if we change our state, we trigger our own callback
// so either we'll settle out or go nuts... we might need to set up a
// de-loop counter that counts the number of times we were called for a given
// logic update period and gives up after three or so. This case will only occour
// if the part is abused, -- and it will be. =P
}
//END class ECSRFlipFlop

