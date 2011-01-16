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
// #include <kiconloader.h>
// #include <klocale.h>

#include "logic.h"
// #include "libraryitem.h"
#include "variant.h"

#include <QDebug>
#include <QStringList>

BinaryCounter::BinaryCounter(Circuit& ownerCircuit) : Component(ownerCircuit)
{
	m_bTriggerHigh = true;
	b_oldIn = false;
	m_value = 0;
	m_numBits = 0;
	m_bDoneLogicIn = false;

    Property *trig = new Property("trig", Variant::Type::Select);
    trig->setCaption(tr("Trigger Edge"));
    QStringMap allowed;
    allowed["Rising"] = tr("Rising");
    allowed["Falling"] = tr("Falling");
    trig->setAllowed(allowed);
    trig->setValue("Falling");
    addProperty(trig);

    Property *bitcnt = new Property("bitcount", Variant::Type::Int);
    bitcnt->setCaption(tr("Bit Count"));
    bitcnt->setMinValue(1);
    bitcnt->setMaxValue(26);
    bitcnt->setValue(4);
    addProperty(bitcnt);

    /*
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
    */
}

BinaryCounter::~BinaryCounter() {
	for (unsigned i = 0; i < m_numBits; i++) {
		delete m_pLogicOut[i];
	}
}

/*
void BinaryCounter::dataChanged() {
	initPins(dataInt("bitcount"));

	b_triggerHigh = dataString("trig") == "Rising";
	setDisplayText(">", b_triggerHigh ? "^>" : "_>");
}
*/
void BinaryCounter::propertyChanged(Property& theProperty, QVariant newValue, QVariant oldValue)
{
    if( theProperty.name() == "bitcount"){
        initPins( newValue.asInt());
    }
    if(theProperty.name() == "trig"){
        m_bTriggerHigh = newValue.asString() == "Rising";
        // setDisplayText(">", m_bTriggerHigh ? "^>" : "_>");
    }
    Q_UNUSED(oldValue);
}
/*
int BinaryCounter::bitNumber() const
{
    return m_numBits;
}

void BinaryCounter::setBitNumber(int numBits)
{
    if( (numBits < 0) || (numBits >= 26)) // FIXME magic constant
    {
        qWarning() << "BinaryCounter::setBitNumber: invalid number of bits"
            << numBits << "Truncating to 26";
        numBits = 26;
    }
    initPins(numBits);
}

bool BinaryCounter::triggherIsOnRisingEdge() const
{
    return m_bTriggerHigh;
}

void BinaryCounter::setTriggerOnRisingEdge(bool isOnRisingEdge)
{
    m_bTriggerHigh = isOnRisingEdge;
}
*/

void BinaryCounter::initPins(unsigned numBits) {
	if (m_numBits == numBits)
		return;

	QStringList pins;

	pins << "en" << ">" << "u/d" << "r";

	{
		int np = int(numBits) - 4;

		if(np > 0) {
			for(int i = 0; i < np; i++)
				pins << "";
		}
	}

	for (int i = numBits - 1; i >= 0; i--)
		pins << QChar('A' + i);

	if (m_numBits < numBits) {
		for (unsigned i = m_numBits; i < numBits; i++) {
			m_pLogicOut[i] = new LogicOut(LogicConfig(), false);
		}
	} else {
		for (unsigned i = numBits; i < m_numBits; i++) {
			QString id = QChar('A' + i);
            /* FIXME implement element removal
			removeElement(m_pLogicOut[i], false);
			removeDisplayText(id);
			removeNode(id);
            */
            m_pLogicOut[i] = 0;
		}
	}

	m_numBits = numBits;

	if (!m_bDoneLogicIn) {
        /*
		setup1pinElement(enLogic, ecNodeWithID("en")->pin());
		setup1pinElement(udLogic, ecNodeWithID("u/d")->pin());
		setup1pinElement(inLogic, ecNodeWithID(">")->pin());
		setup1pinElement(rLogic, ecNodeWithID("r")->pin());
        */

		inLogic.setCallback(this, (CallbackPtr)(&BinaryCounter::inStateChanged));
		rLogic.setCallback(this, (CallbackPtr)(&BinaryCounter::rStateChanged));

		m_bDoneLogicIn = true;
	}

	outputValue();
}

void BinaryCounter::inStateChanged(bool state) {
	if ((state != b_oldIn) && enLogic.isHigh() && !rLogic.isHigh() && state == m_bTriggerHigh) {
		if (udLogic.isHigh()) m_value++;
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
