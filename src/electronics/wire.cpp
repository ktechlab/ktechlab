/***************************************************************************
 *   Copyright (C) 2005 by David Saxton                                    *
 *   david@bluehaze.org                                                    *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#include "pin.h"
#include "wire.h"
#include <cassert>
#include <kdebug.h>

Wire::Wire( Pin * startPin, Pin * endPin )
{
	assert(startPin);
	assert(endPin);

	m_pStartPin = startPin;
	m_pEndPin = endPin;
	m_current = 0.;
	m_bCurrentIsKnown = false;

	m_pStartPin->addWire(this);
	m_pEndPin->addWire(this);
}

Wire::~Wire()
{
}

bool Wire::calculateCurrent()
{
	if ( m_pStartPin->currentIsKnown() && m_pStartPin->numWires() < 2 )
	{
		m_current = m_pStartPin->current();
		m_bCurrentIsKnown = true;
		return true;
	}
	
	if ( m_pEndPin->currentIsKnown() && m_pEndPin->numWires() < 2 )
	{
		m_current = -m_pEndPin->current();
		m_bCurrentIsKnown = true;
		return true;
	}
	
	if(m_pStartPin->currentIsKnown()) {

		double i = m_pStartPin->current();
		bool ok = true;

		const WireList list = m_pStartPin->wireList();
		WireList::const_iterator end = list.end();
		for(WireList::const_iterator it = list.begin(); it != end && ok; ++it)
		{
			if(*it && (Wire*)*it != this)
			{
				if((*it)->currentIsKnown())
					i -= (*it)->current();
				
				else
					ok = false;
			}
		}

		if (ok) {
			m_current = i;
			m_bCurrentIsKnown = true;
			return true;
		}
	}
	
	if(m_pEndPin->currentIsKnown()) {

		double i = -m_pEndPin->current();
		bool ok = true;


		const WireList list = m_pEndPin->wireList();
		WireList::const_iterator end = list.end();
		for(WireList::const_iterator it = list.begin(); it != end && ok; ++it)
		{
			if(*it && (Wire*)*it != this)
			{
				if((*it)->currentIsKnown())
					i += (*it)->current();
				else ok = false;
			}	
		}
		
		if (ok)
		{	
			m_current = i;
			m_bCurrentIsKnown = true;
			return true;
		}
	}
	
	m_bCurrentIsKnown = false;
	return false;
}

double Wire::voltage() const
{
	double temp;
	if( (temp=m_pStartPin->voltage() - m_pEndPin->voltage()) ) {
		kdError() << "Wire voltage error: " << temp << endl;
	}
	
	return m_pStartPin->voltage();
}

void Wire::setCurrentKnown( bool known )
{
	m_bCurrentIsKnown = known;
	if (!known)
		m_current = 0.;
}
