/***************************************************************************
 *   Copyright (C) 2005 by David Saxton                                    *
 *   david@bluehaze.org                                                    *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#ifndef METER_H
#define METER_H

#include <component.h>

/**
@author David Saxton
*/
class Meter : public Component
{
public:
	Meter( ICNDocument *icnDocument, bool newItem, const char *id );
	~Meter();
	
	virtual void stepNonLogic();
	virtual bool doesStepNonLogic() const { return true; }
	virtual void drawShape( QPainter &p );
	virtual bool contentChanged() const;
	
protected:
	QString displayText();
	virtual void dataChanged();
	/**
	 * Return the value / current, or whatever the meter is measuring
	 */
	virtual double meterValue() = 0;
	
	double calcProp( double v ) const;
	
	bool b_firstRun; // If true, then update the text dispalyed
	bool b_timerStarted; // The timer to change the text is started on change
	double m_timeSinceUpdate;
	double m_avgValue;
	double m_old_value;
	double m_minValue;
	double m_maxValue;
	Text *p_displayText;
	QString m_unit;
	
	double m_prevProp; // Used in contentChanged()
};

/**
@short Measures the frequency at a point in the circuit
@author David Saxton
*/
class FrequencyMeter : public Meter
{
public:
	FrequencyMeter( ICNDocument *icnDocument, bool newItem, const char *id = 0L );
	~FrequencyMeter();
	
	static Item* construct( ItemDocument *itemDocument, bool newItem, const char *id );
	static LibraryItem *libraryItem();
	
protected:
	virtual double meterValue();
	ECNode *m_probeNode;
};

/**
@short Simple resistor
@author David Saxton
*/
class ECAmmeter : public Meter
{
public:
	ECAmmeter( ICNDocument *icnDocument, bool newItem, const char *id = 0L );
	~ECAmmeter();
	
	static Item* construct( ItemDocument *itemDocument, bool newItem, const char *id );
	static LibraryItem *libraryItem();
	
protected:
	virtual double meterValue();

private:
	VoltageSource *m_voltageSource;
};

/**
@short Displays voltage across terminals
@author David Saxton
*/
class ECVoltMeter : public Meter
{
public:
	ECVoltMeter( ICNDocument *icnDocument, bool newItem, const char *id = 0L );
	~ECVoltMeter();
	
	static Item* construct( ItemDocument *itemDocument, bool newItem, const char *id );
	static LibraryItem *libraryItem();
	
protected:
	virtual double meterValue();
};

#endif
