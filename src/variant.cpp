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
#include <qdebug.h>
#include <klocalizedstring.h>
using namespace std;

Variant::Variant( const QString & id, Type::Value type )
	: QObject(), m_id( id )
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


void Variant::appendAllowed( const QString & id, const QString & i18nName )
{
	m_allowed[id] = i18nName;
}


void Variant::setAllowed( const QStringList & allowed )
{
	m_allowed.clear();
	QStringList::const_iterator end = allowed.end();
	for ( QStringList::const_iterator it = allowed.begin(); it != end; ++it )
		m_allowed[ *it ] = *it;
}


void Variant::appendAllowed( const QString & allowed )
{
	m_allowed[ allowed ] = allowed;
}


void Variant::setMinValue( double value )
{
	m_minValue = value;
	if ( std::abs(value) < m_minAbsValue && value != 0.  )
	{
		m_minAbsValue = std::abs(value);
	}
}

void Variant::setMaxValue( double value )
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
			
		case Variant::Type::Select:
			return m_allowed[ m_value.toString() ];
			
		default:
			return m_value.toString();
	}
}

void Variant::setValue( QVariant val )
{
    qDebug() << Q_FUNC_INFO << "val=" << val << " old=" << m_value;
	if ( type() == Variant::Type::Select && !m_allowed.contains( val.toString() ) )
	{
		// Our value is being set to an i18n name, not the actual string id.
		// So change val to the id (if it exists)
		
		QString i18nName = val.toString();
		
		QStringMap::iterator end = m_allowed.end();
		for ( QStringMap::iterator it = m_allowed.begin(); it != end; ++it )
		{
			if ( it.value() == i18nName )
			{
				val = it.key();
				break;
			}
		}
	}
	
	if ( !m_bSetDefault )
	{
		m_defaultValue = val;
		m_bSetDefault = true;
	}
	
	if ( m_value == val )
		return;
	
	const QVariant old = m_value;
	m_value = val;
	emit( valueChanged( val, old ) );
	
	switch ( type() )
	{
		case Variant::Type::String:
		case Variant::Type::FileName:
		case Variant::Type::PenCapStyle:
		case Variant::Type::PenStyle:
		case Variant::Type::Port:
		case Variant::Type::Pin:
		case Variant::Type::VarName:
		case Variant::Type::Combo:
		case Variant::Type::Select:
		case Variant::Type::SevenSegment:
		case Variant::Type::KeyPad:
		case Variant::Type::Multiline:
		case Variant::Type::RichText:
            {
                QString dispString = displayString();
                qDebug() << Q_FUNC_INFO << "dispString=" << dispString << " value=" << m_value;
                emit valueChanged( dispString );
                emit valueChangedStrAndTrue( dispString, true );
            }
			break;
			
		case Variant::Type::Int:
			emit valueChanged( value().toInt() );
			break;
			
		case Variant::Type::Double:
			emit valueChanged( value().toDouble() );
			break;
			
		case Variant::Type::Color:
			emit valueChanged( value().value<QColor>() );
			break;
			
		case Variant::Type::Bool:
			emit valueChanged( value().toBool() );
			break;
			
		case Variant::Type::Raw:
		case Variant::Type::None:
			break;
	}
	qDebug() << Q_FUNC_INFO << "result m_value=" << m_value;
}


void Variant::setMinAbsValue( double val )
{
	m_minAbsValue = val;
}


bool Variant::changed() const
{
	// Have to handle double slightly differently due inperfect storage of real
	// numbers
	if ( type() == Type::Double )
	{
		double cur = value().toDouble();
		double def = defaultValue().toDouble();
		
		double diff = abs( cur - def );
		if ( diff == 0 )
			return false;
		
		// denom cannot be zero
		double denom = max( abs( cur ), abs( def ) );
		
		// not changed if within 1e-4% of each other's value
		return ( (diff / denom) > 1e-6 );
	}
	return value() != defaultValue();
}
