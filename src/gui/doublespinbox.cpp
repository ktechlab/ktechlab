/***************************************************************************
 *   Copyright (C) 2003-2006 by David Saxton                               *
 *   david@bluehaze.org                                                    *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#include "doublespinbox.h"
#include "cnitem.h"

#include <KLocalizedString>

#include <QDebug>
#include <QLineEdit>
#include <QLocale>
#include <QRegExp>
#include <QTimer>

#include <algorithm>
#include <cmath>

using namespace std;

static bool isDoubleSpinboxDebugEnabled()
{
    return false; // note: in the future, loggers should be used
}

static QString DoubleSpinBox_nullDebug;

static QDebug DoubleSpinbox_qDebug()
{
    if (isDoubleSpinboxDebugEnabled()) {
        return qDebug();
    } else {
        DoubleSpinBox_nullDebug.clear();
        return QDebug(&DoubleSpinBox_nullDebug);
    }
}

inline int roundDouble(double val)
{
    return (val > 0) ? int(val + 0.5) : int(val - 0.5);
}

DoubleSpinBox::DoubleSpinBox(double lower, double upper, double minAbs, double value, const QString &unit, QWidget *parent)
    : QDoubleSpinBox(parent)
{
    DoubleSpinbox_qDebug() << " lower=" << lower << " upper=" << upper << " minAbs=" << minAbs << " value=" << value << " unit=" << unit << " parent=" << parent;

    setDecimals(20); // should be enough

    // 	m_lastEmittedValue = value;
    m_unit = unit;
    // 	m_minValue = lower;
    setMinimum(lower);
    // 	m_maxValue = upper;
    setMaximum(upper);
    m_minAbsValue = minAbs;
    // 	m_queuedSuffix = QString::null;

    init();
    setValue(value);
}

DoubleSpinBox::DoubleSpinBox(QWidget *parent)
    : QDoubleSpinBox(parent)
{
    setDecimals(20); // should be enough

    m_lastEmittedValue = 0;
    // 	m_minValue = 0;
    setMinimum(0);
    // 	m_maxValue = 1e9;
    setMaximum(1e9);
    m_minAbsValue = 1e-9;
    // 	m_queuedSuffix = QString::null;

    init();
    setValue(0);
}

void DoubleSpinBox::init()
{
    lineEdit()->setAlignment(Qt::AlignRight);

    // 	connect( this, SIGNAL(valueChanged(double)), this, SLOT(checkIfChanged()) );
    // 	QSpinBox::setMinValue( -(1<<30) );
    // 	QSpinBox::setMaxValue( +(1<<30) );

    // setValidator( 0 ); // apparently in Qt4 there is no validator
}

DoubleSpinBox::~DoubleSpinBox()
{
}

QValidator::State DoubleSpinBox::validate(QString &text, int &pos) const
{
    DoubleSpinbox_qDebug() << Q_FUNC_INFO << "text = |" << text << "| pos= " << pos;
    return QValidator::Acceptable; // QValidator::Intermediate; // don't bother
}

// double DoubleSpinBox::value()
// {
//     const double mult = getMult();
//     const double displatedNumber = getDisplayedNumber( 0 );
//     const double toRet = displatedNumber * mult ;
//     DoubleSpinbox_qDebug() << "value() mult = " << mult;
//     DoubleSpinbox_qDebug() << "value() displatedNumber = " << displatedNumber;
//     DoubleSpinbox_qDebug() << "value() toRet = " << toRet;
// 	return toRet;
// }

// void DoubleSpinBox::setValue( double value )
// {
// 	if ( this->value() == value )
// 		return;
//
// 	if ( value > maxValue() )
// 		value = maxValue();
//
// 	else if ( value < minValue() )
// 		value = minValue();
//
// 	if ( std::abs(value) < m_minAbsValue*0.9999 )
// 		value = 0.0;
//
// 	updateSuffix( value );
//
//     const int toBeStoredValue = roundDouble( (value / Item::getMultiplier( value )) /* * 100 */ );
//
//     DoubleSpinbox_qDebug() << "value = " << value;
//     DoubleSpinbox_qDebug() << "value() = " << QSpinBox::value();
//     DoubleSpinbox_qDebug() << "to be stored = " << toBeStoredValue;
//
// 	QSpinBox::setValue( toBeStoredValue );
// }

// void DoubleSpinBox::setUnit( const QString & unit )
// {
// 	updateSuffix( value() );
// 	m_unit = unit;
// }

void DoubleSpinBox::updateSuffix(double value)
{
    QString nextSuffix = " " + CNItem::getNumberMag(value) + m_unit;
    setSuffix(nextSuffix);
    // 	m_queuedSuffix = " " + CNItem::getNumberMag( value ) + m_unit;
    //
    // 	Set suffix to be empty if it is nothing but white space
    // 	if ( m_queuedSuffix.trimmed().isEmpty() )
    // 		m_queuedSuffix = "";
    //
    // 	QTimer::singleShot( 0, this, SLOT(setQueuedSuffix()) );
}

// void DoubleSpinBox::setQueuedSuffix()
// {
// 	bool changed = false;
// 	if ( !m_queuedSuffix.isNull() && suffix().simplifyWhiteSpace() != m_queuedSuffix.simplifyWhiteSpace() )
// 	{
// 		setSuffix( m_queuedSuffix );
// 		changed = true;
// 	}
// 	m_queuedSuffix = QString::null;
//
// 	if ( changed )
// 		emit valueChanged( value() );
// }

DoubleSpinBox::StepEnabled DoubleSpinBox::stepEnabled() const
{
    return QDoubleSpinBox::StepDownEnabled | QDoubleSpinBox::StepUpEnabled;
}

void DoubleSpinBox::stepBy(int steps)
{
    double workVal = value();
    while (steps != 0) {
        if (steps > 0) {
            workVal = getNextUpStepValue(workVal);
            steps--;
        } else {
            workVal = getNextDownStepValue(workVal);
            steps++;
        }
    }
    setValue(workVal);
}

double DoubleSpinBox::getNextUpStepValue(double in)
{
    DoubleSpinbox_qDebug() << Q_FUNC_INFO << " in = " << in;

    double value = roundToOneSF(in);

    if (value == 0) {
        value = m_minAbsValue;
    } else if (value > 0) {
        value += std::pow(10., std::floor(std::log10(value)));
    } else {
        double sub = std::pow(10., std::floor(std::log10(std::abs(value)) - 1));
        value += std::pow(10., std::floor(std::log10(std::abs(value) - sub)));
    }

    value *= 1.00001;

    if (std::abs(value) < m_minAbsValue) {
        value = 0.;
    }

    DoubleSpinbox_qDebug() << Q_FUNC_INFO << " out = " << value;
    return value;
}

double DoubleSpinBox::getNextDownStepValue(double in)
{
    DoubleSpinbox_qDebug() << Q_FUNC_INFO << " in = " << in;

    double value = roundToOneSF(in);

    if (value == 0) {
        value = -m_minAbsValue;
    } else if (value > 0) {
        double sub = std::pow(10., std::floor(std::log10(value) - 1));
        value -= std::pow(10., std::floor(std::log10(value - sub)));
    } else {
        double add = std::pow(10., std::floor(std::log10(std::abs(value)) - 1));
        value -= std::pow(10., std::floor(std::log10(std::abs(value) + add)));
    }

    value *= 1.00001;

    if (std::abs(value) < m_minAbsValue) {
        value = 0.;
    }

    DoubleSpinbox_qDebug() << Q_FUNC_INFO << " out = " << value;

    return value;
}

// double DoubleSpinBox::getMult()
// {
// 	QString text = this->text().trimmed();
// 	if ( !m_queuedSuffix.isNull() )
// 	{
// 		QString nsSuffix = suffix().trimmed();
//
// 		if ( nsSuffix.isEmpty() )
// 			text.append( m_queuedSuffix );
// 		else
// 			text.replace( nsSuffix, m_queuedSuffix );
// 	}
//
// 	if ( text.length() == 0 )
// 		return 1.0;
//
// 	if ( text.endsWith( m_unit, false ) )
// 		text = text.remove( text.length() - m_unit.length(), m_unit.length() );
//
// 	text.trimmed();
//
// 	QChar siExp = text[ text.length()-1 ];
//
// 	if ( siExp.isLetter() || siExp.isSymbol() )
// 		return CNItem::getMultiplier((QString)siExp);
//
// 	else
// 		return 1;
// }

// double DoubleSpinBox::getDisplayedNumber( bool * ok )
// {
// 	KLocale * locale = KGlobal::locale();
//
// 	//Fetch the characters that we don't want to discard
// 	const QString exclude = locale->decimalSymbol()
// 			+ locale->thousandsSeparator()
// 			+ locale->positiveSign()
// 			+ locale->negativeSign();
//
// 	QString number = cleanText().remove( QRegExp("[^"+exclude+"\\d]") );
//
// 	return locale->readNumber( number, ok );
// }

// int DoubleSpinBox::mapTextToValue( bool * ok )
// {
// 	(void)ok;
//
// 	double value = this->value();
//
// 	if ( value > maxValue() )
// 		value = maxValue();
//
// 	else if ( value < minValue() )
// 		value = minValue();
//
// 	if ( std::abs(value) < m_minAbsValue*0.9999 )
// 		value = 0.0;
//
// 	updateSuffix( value );
//
// 	value /= Item::getMultiplier( value );
//
// 	Precision of 2 extra digits
// 	return int( value /* * 100 */ );
// }
//

double DoubleSpinBox::valueFromText(const QString &text) const
{
    DoubleSpinbox_qDebug() << Q_FUNC_INFO << "text = " << text;

    QLocale locale;

    // Fetch the characters that we don't want to discard
    const QString exclude = QString(locale.decimalPoint()) + locale.groupSeparator() + locale.positiveSign() + locale.negativeSign();

    QString textToStrip(text);
    QString numberToRead = textToStrip.remove(QRegExp("[^" + exclude + "\\d]"));

    bool ok;
    double value = locale.toDouble(numberToRead, &ok);
    if (!ok) {
        DoubleSpinbox_qDebug() << Q_FUNC_INFO << "numberToRead = |" << numberToRead << "| NOT OK";
        value = 0;
    }
    DoubleSpinbox_qDebug() << Q_FUNC_INFO << "numberToRead = " << numberToRead << ", value = " << value;

    if (value > maximum()) {
        value = maximum();
    } else if (value < minimum()) {
        value = minimum();
    }

    if (std::abs(value) < m_minAbsValue * 0.9999) {
        value = 0.0;
    }

    double multiplier = 1.0;
    // updateSuffix( value );
    QString textForSuffix(text);

    if (textForSuffix.length() != 0) {
        if (textForSuffix.endsWith(m_unit, Qt::CaseInsensitive)) {
            textForSuffix = textForSuffix.remove(textForSuffix.length() - m_unit.length(), m_unit.length());
        }

        textForSuffix = textForSuffix.trimmed();

        QChar siExp;
        if (textForSuffix.length() > 0) {
            siExp = textForSuffix[textForSuffix.length() - 1];
        } else {
            siExp = '1';
        }

        DoubleSpinbox_qDebug() << Q_FUNC_INFO << "SI exp = " << siExp;

        if (siExp.isLetter() || siExp.isSymbol()) {
            multiplier = CNItem::getMultiplier(QString(siExp));
        } else {
            multiplier = 1;
        }
    }
    DoubleSpinbox_qDebug() << Q_FUNC_INFO << "multiplier = " << multiplier;

    // value /= Item::getMultiplier( value );
    value *= multiplier;

    DoubleSpinbox_qDebug() << Q_FUNC_INFO << "value = " << value;

    return value;
}

//
// QString DoubleSpinBox::mapValueToText( int v )
// {
// 	double val = double(v) /* /100.0 */;
//
// 	int leftDigits = (int)floor( log10( abs(val) ) ) + 1;
// 	if ( leftDigits < 0 )
// 		leftDigits = 0;
// 	else if ( leftDigits > 3 )
// 		leftDigits = 3;
//
// 	KLocale * locale = KGlobal::locale();
// 	return locale->formatNumber( val, 3-leftDigits );
// }

QString DoubleSpinBox::textFromValue(double value) const
{
    DoubleSpinbox_qDebug() << Q_FUNC_INFO << " value = " << value;

    //     int leftDigits = (int)floor( log10( abs( value ) ) ) + 1;
    //     if ( leftDigits < 0 ) {
    //         leftDigits = 0;
    //     } else if ( leftDigits > 3 ) {
    //         leftDigits = 3;
    //     }
    double multiplier = Item::getMultiplier(value);

    double toDisplayNr = value / multiplier;

    DoubleSpinbox_qDebug() << Q_FUNC_INFO << "toDisplayNr = " << toDisplayNr;

    QString numberStr = QLocale().toString(toDisplayNr, 'f', 0 /* 3-leftDigits */);

    QString magStr = Item::getNumberMag(value);

    QString toRet = numberStr + " " + magStr + m_unit;

    DoubleSpinbox_qDebug() << Q_FUNC_INFO << " text = " << toRet;
    return toRet;
}

// void DoubleSpinBox::checkIfChanged()
// {
// 	double newValue = value();
//
// 	if ( m_lastEmittedValue == newValue )
// 		return;
//
// 	m_lastEmittedValue = newValue;
// 	emit valueChanged( m_lastEmittedValue );
// }

double DoubleSpinBox::roundToOneSF(double value)
{
    if (value == 0.0)
        return 0.0;

    value *= 1.000001;
    double tens = pow(10.0, floor(log10(abs(value))));

    return int(value / tens) * tens;
}

// void DoubleSpinBox::stepUp()
// {
// 	double value = roundToOneSF( this->value() );
//
// 	if ( value == 0 )
// 		value = m_minAbsValue;
//
// 	else if ( value > 0 )
// 		value += std::pow( 10., std::floor( std::log10(value) ) );
//
// 	else
// 	{
// 		double sub = std::pow(10., std::floor( std::log10(std::abs(value))-1) );
// 		value += std::pow( 10., std::floor( std::log10(std::abs(value)-sub) ) );
// 	}
//
// 	value *= 1.00001;
//
// 	if ( std::abs(value) < m_minAbsValue )
// 		value = 0.;
//
// 	setValue( value );
// }

// void DoubleSpinBox::stepDown()
// {
// 	double value = roundToOneSF( this->value() );
//
// 	if ( value == 0 )
// 		value = -m_minAbsValue;
//
// 	else if ( value > 0 )
// 	{
// 		double sub = std::pow(10., std::floor( std::log10(value)-1) );
// 		value -= std::pow( 10., std::floor( std::log10(value-sub) ) );
// 	}
// 	else
// 	{
// 		double add = std::pow(10., std::floor( std::log10(std::abs(value))-1) );
// 		value -= std::pow( 10., std::floor( std::log10(std::abs(value)+add) ) );
// 	}
//
// 	value *= 1.00001;
//
// 	if ( std::abs(value) < m_minAbsValue )
// 		value = 0.;
//
// 	setValue( value );
// }
