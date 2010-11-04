/***************************************************************************
 *   Copyright (C) 2004-2005 by David Saxton                               *
 *   david@bluehaze.org                                                    *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#include <cmath>
#include <QDebug>

// #include <kiconloader.h>
// #include <klocale.h>

#include "demultiplexer.h"
#include "logic.h"
// #include "libraryitem.h"

Demultiplexer::Demultiplexer()
		: Component(), 
		m_input(LogicConfig())
{
	m_input.setCallback(this, (CallbackPtr)(&Demultiplexer::inStateChanged));

    Property *size = new Property("addressSize", Variant::Type::Int);
    size->setCaption(tr("Address Size"));
    size->setMinValue(1);
    size->setMaxValue(8);
    size->setValue(1);
    addProperty(size);

    /*
	createProperty("addressSize", Variant::Type::Int);
	property("addressSize")->setCaption(i18n("Address Size"));
	property("addressSize")->setMinValue(1);
	property("addressSize")->setMaxValue(8);
	property("addressSize")->setValue(1);

	// For backwards compatibility
	createProperty("numInput", Variant::Type::Int);
	property("numInput")->setMinValue(-1);
	property("numInput")->setValue(-1);
	property("numInput")->setHidden(true);
    */
}

Demultiplexer::~Demultiplexer() {
/*	for (unsigned i = 0; i < m_aLogic.size(); ++i) {
		delete m_aLogic[i];
	}

	for (unsigned i = 0; i < m_xLogic.size(); ++i)
		delete m_xLogic[i];
*/
}

/*
void Demultiplexer::dataChanged() {
    
	if (hasProperty("numInput") && dataInt("numInput") != -1) {
		int addressSize = int(std::ceil(std::log((double)dataInt("numInput")) / std::log(2.0)));
		property("numInput")->setValue(-1);

		if (addressSize < 1)
			addressSize = 1;
		else if (addressSize > 8)
			addressSize = 8;

		// This function will get called again when we set the value of numInput
		property("addressSize")->setValue(addressSize);

		return;
	}

	if (hasProperty("numInput")) {
		m_variantData["numInput"]->deleteLater();
		m_variantData.remove("numInput");
	}

	initPins(unsigned(dataInt("addressSize")));
}
*/
void Demultiplexer::propertyChanged(Property& theProperty, QVariant newValue, QVariant oldValue)
{
    if( theProperty.name() != "addressSize" ){
        qCritical() << "unknown property: " << theProperty.name();
        return;
    }
    initPins(newValue.asUInt());
}


void Demultiplexer::inStateChanged(bool /*state*/) {
	if(m_input.isHigh()) {
		unsigned long long pos = 0;
		for (unsigned i = 0; i < m_aLogic.size(); ++i) {
			if (m_aLogic[i].isHigh())
				pos |= 1 << i;
		}

		for (unsigned i = 0; i < m_xLogic.size(); ++i)
			m_xLogic[i].setHigh((pos == i));
	} else {
		for (unsigned i = 0; i < m_xLogic.size(); ++i)
			m_xLogic[i].setHigh(false);
	}
}

void Demultiplexer::initPins(unsigned newAddressSize) {
	unsigned oldAddressSize = m_aLogic.size();
	unsigned long long oldXLogicCount = m_xLogic.size();
	unsigned long long newXLogicCount = 1 << newAddressSize;

	if (newXLogicCount == oldXLogicCount)
		return;

	QStringList pins;

	for (unsigned i = 0; i < newAddressSize; ++i)
		pins += "A" + QString::number(i);

	for (unsigned i = newAddressSize; i < (newXLogicCount + (newXLogicCount % 2)) / 2; ++i)
		pins += "";

	pins += "X";

	for (unsigned i = (newXLogicCount + (newXLogicCount % 2)) / 2 + 1; i < newXLogicCount; ++i)
		pins += "";

	for (int i = newXLogicCount - 1; i >= 0; --i)
		pins += "X" + QString::number(i);

	// initDIPSymbol(pins, 64);
	// initDIP(pins);

	// setup1pinElement(m_input, ecNodeWithID("X")->pin());

	if (newXLogicCount > oldXLogicCount) {
		m_xLogic.resize(newXLogicCount);
		for (unsigned i = oldXLogicCount; i < newXLogicCount; ++i) {
			// setup1pinElement(m_xLogic[i], ecNodeWithID("X" + QString::number(i))->pin());
		}

		m_aLogic.resize(newAddressSize);
		for (unsigned i = oldAddressSize; i < newAddressSize; ++i) {
			// setup1pinElement(m_aLogic[i], ecNodeWithID("A" + QString::number(i))->pin());
			m_aLogic[i].setCallback(this, (CallbackPtr)(&Demultiplexer::inStateChanged));
		}
	} else {
		for (unsigned i = newXLogicCount; i < oldXLogicCount; ++i) {
			QString id = "X" + QString::number(i);
            /* FIXME remove element
			removeDisplayText(id);
			removeElement(&m_xLogic[i], false);
			removeNode(id);
            */
		}

		m_xLogic.resize(newXLogicCount);

		for (unsigned i = newAddressSize; i < oldAddressSize; ++i) {
			QString id = "A" + QString::number(i);
            /* FIXME remove element
			removeDisplayText(id);
			removeElement(&m_aLogic[i], false);
			removeNode(id);
            */
		}

		m_aLogic.resize(newAddressSize);
	}
}

