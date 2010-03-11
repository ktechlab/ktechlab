/***************************************************************************
 *   Copyright (C) 2005 by Fredy Yanardi                                   *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#include "libraryitem.h"
#include "logic.h"
#include "magnitudecomparator.h"
#include "variant.h"

#include <cmath>
#include <klocale.h>

Item *MagnitudeComparator::construct(ItemDocument *itemDocument, bool newItem, const char *id) {
	return new MagnitudeComparator((ICNDocument*)itemDocument, newItem, id);
}

LibraryItem *MagnitudeComparator::libraryItem() {
	return new LibraryItem(
	           "ec/magnitudecomparator",
	           i18n("Magnitude Comparator"),
	           i18n("Integrated Circuits"),
	           "ic1.png",
	           LibraryItem::lit_component,
	           MagnitudeComparator::construct);
}

MagnitudeComparator::MagnitudeComparator(ICNDocument *icnDocument, bool newItem, const char *id)
		: DIPComponent(icnDocument, newItem, id ? id : "magnitudecomparator") {
	m_name = i18n("Magnitude Comparator");

	createProperty("numInput", Variant::Type::Int);
	property("numInput")->setCaption(i18n("Number Inputs"));
	property("numInput")->setMinValue(1);
	property("numInput")->setMaxValue(8);
	property("numInput")->setValue(4);

	m_oldABLogicCount = 0;

	firstTime = true;
}

MagnitudeComparator::~MagnitudeComparator() {
}

void MagnitudeComparator::dataChanged() {
	initPins();
}

void MagnitudeComparator::inStateChanged() {
	int i;

	for (i = 0; i < 3; i++)
		m_output[i]->setHigh(false);

	for (i = m_oldABLogicCount - 1; i >= 0; i--) {
		if (m_aLogic[i].isHigh() && !m_bLogic[i].isHigh()) {
			m_output[0]->setHigh(true);
			return;
		} else if (!m_aLogic[i].isHigh() && m_bLogic[i].isHigh()) {
			m_output[1]->setHigh(true);
			return;
		}
	}

	if (m_cLogic[2]->isHigh())
		m_output[2]->setHigh(true);
	else if (m_cLogic[0]->isHigh()) {
		if (!m_cLogic[1]->isHigh())
			m_output[0]->setHigh(true);
	} else if (m_cLogic[1]->isHigh())
		m_output[1]->setHigh(true);
	else {
		m_output[0]->setHigh(true);
		m_output[1]->setHigh(true);
	}
}

void MagnitudeComparator::initPins() {
	const double numInputs = dataInt("numInput");
	int newABLogicCount = (int)numInputs;

	if (newABLogicCount == m_oldABLogicCount)
		return;

	QStringList leftPins;

	int space = 3 - newABLogicCount;

	for (int i = 0; i < space; i++)
		leftPins << "";

	for (int i = 0; i < newABLogicCount; i++)
		leftPins << QString("A%1").arg(QString::number(i));

	for (int i = 0; i < newABLogicCount; i++)
		leftPins << QString("B%1").arg(QString::number(i));

	for (int i = 0; i < space; i++)
		leftPins << "";

	QStringList rightPins;
	space = -space;

	for (int i = 0; i < space; i++)
		rightPins << "";

	QString inNames[] = { "I: A>B", "I: A<B", "I: A=B" };
	rightPins << inNames[2] << inNames[1] << inNames[0];

	QString outNames[] = { "O: A>B", "O: A<B", "O: A=B" };
	rightPins << outNames[2] << outNames[1] << outNames[0];

	for (int i = 0; i < space; i++)
		rightPins << "";

	QStringList pins = leftPins + rightPins;

	initDIPSymbol(pins, 88);
	initDIP(pins);

	if (firstTime) {
		for (int i = 0; i < 3; i++) {
			LogicIn *inLogic = new LogicIn(LogicConfig());
			setup1pinElement(*inLogic, ecNodeWithID(inNames[i])->pin());
			m_cLogic[i] = inLogic;

			m_cLogic[i]->setCallback(this, (CallbackPtr)(&MagnitudeComparator::inStateChanged));
		}

		for (int i = 0; i < 3; i++) {
			LogicOut *outLogic = new LogicOut(LogicConfig(), false);
			setup1pinElement(*outLogic, ecNodeWithID(outNames[i])->pin());
			m_output[i] = outLogic;
		}

		firstTime = false;
	}

	if (newABLogicCount > m_oldABLogicCount) {
		m_aLogic.resize(newABLogicCount);
		for (int i = m_oldABLogicCount; i < newABLogicCount; ++i) {
			setup1pinElement(m_aLogic[i], ecNodeWithID("A" + QString::number(i))->pin());
			m_aLogic[i].setCallback(this, (CallbackPtr)(&MagnitudeComparator::inStateChanged));
		}

		m_bLogic.resize(newABLogicCount);
		for (int i = m_oldABLogicCount; i < newABLogicCount; ++i) {
			setup1pinElement(m_bLogic[i], ecNodeWithID("B" + QString::number(i))->pin());
			m_bLogic[i].setCallback(this, (CallbackPtr)(&MagnitudeComparator::inStateChanged));
		}
	} else {
		for (int i = newABLogicCount; i < m_oldABLogicCount; ++i) {
			QString id = "A" + QString::number(i);
			removeDisplayText(id);
			removeElement(&m_aLogic[i], false);
			removeNode(id);
		}

		m_aLogic.resize(newABLogicCount);

		for (int i = newABLogicCount; i < m_oldABLogicCount; ++i) {
			QString id = "B" + QString::number(i);
			removeDisplayText(id);
			removeElement(&m_bLogic[i], false);
			removeNode(id);
		}

		m_bLogic.resize(newABLogicCount);
	}

	m_oldABLogicCount = newABLogicCount;
	inStateChanged();
}

