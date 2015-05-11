/***************************************************************************
 *   Copyright (C) 2003-2006 by David Saxton                               *
 *   david@bluehaze.org                                                    *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#include "cnitem.h"
#include "doublespinbox.h"

#include <kdebug.h>
#include <kglobal.h>
#include <klocale.h>

#include <Qt/qlineedit.h>
#include <Qt/qregexp.h>
#include <Qt/qtimer.h>

#include <algorithm>
#include <cmath>
using namespace std;


inline int roundDouble( double val )
{
	return (val > 0) ? int(val+0.5) : int(val-0.5);
}


DoubleSpinBox::DoubleSpinBox( double lower, double upper, double minAbs, double value, const QString &unit, QWidget * parent )
	: QSpinBox( parent )
{
	m_lastEmittedValue = value;
	m_unit = unit;
	m_minValue = lower;
	m_maxValue = upper;
	m_minAbsValue = minAbs;
	m_queuedSuffix = QString::null;
	
	init();
	setValue( value );
}


DoubleSpinBox::DoubleSpinBox( QWidget * parent )
	: QSpinBox( parent )
{
	m_lastEmittedValue = 0;
	m_minValue = 0;
	m_maxValue = 1e9;
	m_minAbsValue = 1e-9;
	m_queuedSuffix = QString::null;
	
	init();
	setValue( 0 );
}


void DoubleSpinBox::init()
{
	lineEdit()->setAlignment( Qt::AlignRight );
	
	connect( this, SIGNAL(valueChanged(int)), this, SLOT(checkIfChanged()) );
	QSpinBox::setMinValue( -(1<<30) );
	QSpinBox::setMaxValue( +(1<<30) );	
	
	// setValidator( 0 ); // apparently in Qt4 there is no validator
}


DoubleSpinBox::~DoubleSpinBox()
{
}


double DoubleSpinBox::value()
{
	return getDisplayedNumber( 0 ) * getMult();
}


void DoubleSpinBox::setValue( double value )
{
	if ( this->value() == value )
		return;
	
	if ( value > maxValue() )
		value = maxValue();
	
	else if ( value < minValue() )
		value = minValue();
	
	if ( std::abs(value) < m_minAbsValue*0.9999 )
		value = 0.0;
	
	updateSuffix( value );
	
	QSpinBox::setValue( roundDouble( (value / Item::getMultiplier( value )) * 100 ) );
}


void DoubleSpinBox::setUnit( const QString & unit )
{
	updateSuffix( value() );
	m_unit = unit;
}


void DoubleSpinBox::updateSuffix( double value )
{
	m_queuedSuffix = " " + CNItem::getNumberMag( value ) + m_unit;
	
	// Set suffix to be empty if it is nothing but white space
	if ( m_queuedSuffix.stripWhiteSpace().isEmpty() )
		m_queuedSuffix = "";
	
	QTimer::singleShot( 0, this, SLOT(setQueuedSuffix()) );
}


void DoubleSpinBox::setQueuedSuffix()
{
	bool changed = false;
	if ( !m_queuedSuffix.isNull() && suffix().simplifyWhiteSpace() != m_queuedSuffix.simplifyWhiteSpace() )
	{
		setSuffix( m_queuedSuffix );
		changed = true;
	}
	m_queuedSuffix = QString::null;
	
	if ( changed )
		emit valueChanged( value() );
}


double DoubleSpinBox::getMult()
{
	QString text = this->text().stripWhiteSpace();
	if ( !m_queuedSuffix.isNull() )
	{
		QString nsSuffix = suffix().stripWhiteSpace();
		
		if ( nsSuffix.isEmpty() )
			text.append( m_queuedSuffix );
		else
			text.replace( nsSuffix, m_queuedSuffix );
	}
	
	if ( text.length() == 0 )
		return 1.0;
	
	if ( text.endsWith( m_unit, false ) )
		text = text.remove( text.length() - m_unit.length(), m_unit.length() );
	
	text.stripWhiteSpace();
	
	QChar siExp = text[ text.length()-1 ];
	
	if ( siExp.isLetter() || siExp.isSymbol() ) 
		return CNItem::getMultiplier((QString)siExp);
	
	else
		return 1;
}


double DoubleSpinBox::getDisplayedNumber( bool * ok )
{
	KLocale * locale = KGlobal::locale();
	
	// Fetch the characters that we don't want to discard
	const QString exclude = locale->decimalSymbol()
			+ locale->thousandsSeparator()
			+ locale->positiveSign()
			+ locale->negativeSign();
	
	QString number = cleanText().remove( QRegExp("[^"+exclude+"\\d]") );
	
	return locale->readNumber( number, ok );
}


int DoubleSpinBox::mapTextToValue( bool * ok )
{
	(void)ok;
	
	double value = this->value();
	
	if ( value > maxValue() )
		value = maxValue();
	
	else if ( value < minValue() )
		value = minValue();
	
	if ( std::abs(value) < m_minAbsValue*0.9999 )
		value = 0.0;
	
	updateSuffix( value );
	
	value /= Item::getMultiplier( value );
	
	// Precision of 2 extra digits
	return int( value * 100 );
}


QString DoubleSpinBox::mapValueToText( int v )
{
	double val = double(v)/100.0;
	
	int leftDigits = (int)floor( log10( abs(val) ) ) + 1;
	if ( leftDigits < 0 )
		leftDigits = 0;
	else if ( leftDigits > 3 )
		leftDigits = 3;
	
	KLocale * locale = KGlobal::locale();
	return locale->formatNumber( val, 3-leftDigits );
}


void DoubleSpinBox::checkIfChanged()
{
	double newValue = value();
	
	if ( m_lastEmittedValue == newValue )
		return;
	
	m_lastEmittedValue = newValue;
	emit valueChanged( m_lastEmittedValue );
}


double DoubleSpinBox::roundToOneSF( double value )
{
	if ( value == 0.0 )
		return 0.0;
	
	value *= 1.000001;
	double tens = pow( 10.0, floor(log10( abs(value) )) );
	
	return int ( value / tens ) * tens;
}


void DoubleSpinBox::stepUp()
{
	double value = roundToOneSF( this->value() );
	
	if ( value == 0 )
		value = m_minAbsValue;
	
	else if ( value > 0 )
		value += std::pow( 10., std::floor( std::log10(value) ) );
	
	else
	{
		double sub = std::pow(10., std::floor( std::log10(std::abs(value))-1) );
		value += std::pow( 10., std::floor( std::log10(std::abs(value)-sub) ) );
	}
	
	value *= 1.00001;
	
	if ( std::abs(value) < m_minAbsValue )
		value = 0.;
	
	setValue( value );
}


void DoubleSpinBox::stepDown()
{
	double value = roundToOneSF( this->value() );
	
	if ( value == 0 )
		value = -m_minAbsValue;
	
	else if ( value > 0 )
	{
		double sub = std::pow(10., std::floor( std::log10(value)-1) );
		value -= std::pow( 10., std::floor( std::log10(value-sub) ) );
	}
	else
	{
		double add = std::pow(10., std::floor( std::log10(std::abs(value))-1) );
		value -= std::pow( 10., std::floor( std::log10(std::abs(value)+add) ) );
	}
	
	value *= 1.00001;
	
	if ( std::abs(value) < m_minAbsValue )
		value = 0.;
	
	setValue( value );
}

#include "doublespinbox.moc"

