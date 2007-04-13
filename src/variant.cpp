/***************************************************************************
 *   Copyright (C) 2003-2005 by David Saxton                               *
 *   david@bluehaze.org                                                    *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#include "colorcombo.h"
#include "cnitem.h"

#include <cmath>
#include <klocale.h>

Variant::Variant( Type::Value type )
	: QObject()
{
	m_type = type;
	m_bSetDefault = false;
	m_bHidden = false;
	m_bAdvanced = false; 
	m_minValue = 1e-6;
	m_maxValue = 1e9;
	m_minAbsValue = 1e-6;
	m_colorScheme = ColorCombo::QtStandard;
	if ( type == Type::Color )
	{
		// this value is taken from ColorCombo and should ideally be put somewhere...
		m_defaultValue = "#f62a2a";
		m_value = "#f62a2a";
	}
}


Variant::~Variant()
{
}


void Variant::setType( Type::Value type )
{
	m_type = type;
}


void Variant::appendAllowed(QString string)
{
	if ( !m_allowed.contains(string) ) {
		m_allowed.append(string);
	}
}


void Variant::setAllowed(QStringList stringList)
{
	// Ideally, we should check for duplicates in whatever is setting the
	// allowed strings, but it is a lot easier and permanent to do it here
	m_allowed.clear();
	const QStringList::iterator end = stringList.end();
	for ( QStringList::iterator it = stringList.begin(); it != end; ++it )
	{
		if ( !m_allowed.contains(*it) ) {
			m_allowed.append(*it);
		}
	}
}


void Variant::setMinValue( const double value )
{
	m_minValue = value;
	if ( std::abs(value) < m_minAbsValue && value != 0.  )
	{
		m_minAbsValue = std::abs(value);
	}
}

void Variant::setMaxValue( const double value )
{
	m_maxValue = value;
	if ( std::abs(value) < m_minAbsValue && value != 0. )
	{
		m_minAbsValue = std::abs(value);
	}
}

QString Variant::displayString() const
{
	switch(type())
	{
		case Variant::Type::Double:
		{
			double numValue = m_value.toDouble();
			return QString::number( numValue / CNItem::getMultiplier(numValue) ) + " " + CNItem::getNumberMag(numValue) + m_unit;
		}
		
		case Variant::Type::Int:
			return m_value.toString()+" "+m_unit;
			
		case Variant::Type::Bool:
			return m_value.toBool() ? i18n("True") : i18n("False");
			
		default:
			return m_value.toString();
	}
}

void Variant::setValue( const QVariant& val )
{
	if (!m_bSetDefault)
		setDefaultValue(val);
	
	if ( m_value == val )
		return;
	
	const QVariant old = m_value;
	m_value = val;
	emit( valueChanged( val, old ) );
}

void Variant::setDefaultValue( QVariant val )
{
	m_defaultValue = val;
	m_bSetDefault = true;
}

void Variant::resetToDefault()
{
	setValue( defaultValue() );
}

void Variant::setMinAbsValue( double val )
{
	m_minAbsValue = val;
}

#include "variant.moc"
