/***************************************************************************
 *   Copyright (C) 2005 by David Saxton                                    *
 *   david@bluehaze.org                                                    *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#include "dependentsource.h"
// #include "ecnode.h"
// #include "libraryitem.h"

// #include <klocale.h>
// #include <qpainter.h>
//Added by qt3to4:
//#include <Q3PointArray>

//BEGIN class DependentSource
DependentSource::DependentSource(Circuit& ownerCircuit)
		: Component(ownerCircuit)
{
    /*
	createProperty("gain", Variant::Type::Double);
	property("gain")->setCaption(i18n("Gain"));
	property("gain")->setValue(1.0);
    */
    Property *gain = new Property("gain", Variant::Type::Double);
    gain->setCaption(tr("Gain"));
    gain->setValue(1.0);
    addProperty(gain);
}

DependentSource::~DependentSource() {
}

//END class DependentSource

//BEGIN class ECCCCS

ECCCCS::ECCCCS(Circuit& ownerCircuit)
		: DependentSource(ownerCircuit),
		m_cccs(1) {
}

ECCCCS::~ECCCCS() {}

/*
void ECCCCS::dataChanged() {
	double gain = dataDouble("gain");

	QString display = QString::number(gain / getMultiplier(gain), 'g', 3) + getNumberMag(gain) + QChar(' ');
	setDisplayText("gain", display);

	m_cccs.setGain(gain);
}
*/
void ECCCCS::propertyChanged(Property& theProperty, QVariant newValue, QVariant oldValue)
{
    Q_UNUSED(oldValue);
    Q_ASSERT(theProperty.name() == "gain");
    m_cccs.setGain(newValue.toDouble());
}

//END class ECCCCS

//BEGIN class ECCCVS

ECCCVS::ECCCVS(Circuit& ownerCircuit)
		: DependentSource(ownerCircuit),
		m_ccvs(1) {
}

ECCCVS::~ECCCVS() {}

/*
void ECCCVS::dataChanged() {
	double gain = dataDouble("gain");

	QString display = QString::number(gain / getMultiplier(gain), 'g', 3) + getNumberMag(gain) + QChar(' ');
	setDisplayText("gain", display);

	m_ccvs.setGain(gain);
}
*/
void ECCCVS::propertyChanged(Property& theProperty, QVariant newValue, QVariant oldValue)
{
    Q_UNUSED(oldValue);
    Q_ASSERT(theProperty.name() == "gain");
    m_ccvs.setGain( newValue.toDouble() );
}


//END class ECCCVS

//BEGIN class ECVCCS
ECVCCS::ECVCCS(Circuit &ownerCircuit)
		: DependentSource(ownerCircuit),
		m_vccs(1) {
}

ECVCCS::~ECVCCS() {}

/*
void ECVCCS::dataChanged() {
	double gain = dataDouble("gain");

	QString display = QString::number(gain / getMultiplier(gain), 'g', 3) + getNumberMag(gain) + QChar(' ');
	setDisplayText("gain", display);

	m_vccs.setGain(gain);
}
*/
void ECVCCS::propertyChanged(Property& theProperty, QVariant newValue, QVariant oldValue)
{
    Q_UNUSED(oldValue);
    Q_ASSERT( theProperty.name() == "gain");
    m_vccs.setGain( newValue.toDouble() );
}

//END class ECVCCS

//BEGIN class ECVCVS

ECVCVS::ECVCVS(Circuit &ownerCircuit)
		: DependentSource(ownerCircuit),
		m_vcvs(1) {
}

ECVCVS::~ECVCVS() {}

/*
void ECVCVS::dataChanged() {
	double gain = dataDouble("gain");

	QString display = QString::number(gain / getMultiplier(gain), 'g', 3) + getNumberMag(gain) + QChar(' ');
	setDisplayText("gain", display);

	m_vcvs.setGain(gain);
}
*/
void ECVCVS::propertyChanged(Property& theProperty, QVariant newValue, QVariant oldValue)
{
    Q_UNUSED(oldValue);
    Q_ASSERT(theProperty.name() == "gain");
    m_vcvs.setGain( newValue.toDouble() );
}

//END class ECVCVS

