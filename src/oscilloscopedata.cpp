/***************************************************************************
 *   Copyright (C) 2005 by David Saxton                                    *
 *   david@bluehaze.org                                                    *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#include "oscilloscopedata.h"
#include "oscilloscope.h"
 
//BEGIN class ProbeData
ProbeData::ProbeData( int id )
	: m_id(id)
{
	m_resetTime = Simulator::self()->time();
// 	b_isPaused = false;
	m_color = Qt::black;
	m_drawPosition = 0.5;
	m_insertPos = 0;
}


ProbeData::~ProbeData()
{
	unregisterProbe(m_id);
}


void ProbeData::setColor( QColor color )
{
	m_color = color;
	emit displayAttributeChanged();
}
//END class ProbeData


//BEGIN class LogicProbeData
LogicProbeData::LogicProbeData( int id )
	: ProbeData(id)
{
}


void LogicProbeData::eraseData()
{
	bool lastValue = false;
	bool hasLastValue = m_insertPos > 0;
	if (hasLastValue)
		lastValue = m_data[m_insertPos-1].value;
	
	m_data.reset();
	m_insertPos = 0;
	m_resetTime = Simulator::self()->time();
	
	if (hasLastValue)
		addDataPoint( LogicDataPoint( lastValue, m_resetTime ) );
}


ullong LogicProbeData::findPos( llong time ) const
{
	if ( time <= 0 )
		return 0;
	
	for ( int a = m_data.allocatedUpTo()-1; a >= 0; a-- )
	{
		DCArray<LogicDataPoint> * dcArray = m_data.dcArray(a);
		
		// We're only interested in this if the earliest recorded data point in this dcArray is <= time
		if ( m_data.toPos( a, 0, 0 ) >= m_insertPos || (dcArray->chunk(0)->data[0].time > ullong(time)) )
			continue;
		
		// Cool, somewhere in this dcArray....
		for ( int b = dcArray->allocatedUpTo()-1; b >= 0; b-- )
		{
			// Done check if the data we'd be accessing is beyond that set
			if ( m_data.toPos( a, b, 0 ) >= m_insertPos || dcArray->chunk(b)->data[0].time > ullong(time) )
				continue;
			
			// Soon...
			for ( int c = DATA_CHUNK_SIZE-1; c >= 0; c-- )
			{
				ullong pos = m_data.toPos( a, b, c );
				
				if ( pos >= m_insertPos || dcArray->chunk(b)->data[c].time > ullong(time) )
					continue;
				
				// Wee!
				return pos;
			}
		}
	}
	
	// Either we have no data points, or the one closest to the given time will be the one at the start
	return 0;
}
//END class LogicProbeData


//BEGIN class FloatingProbeData
FloatingProbeData::FloatingProbeData( int id )
	: ProbeData(id)
{
	m_scaling = Linear;
	m_upperAbsValue = 10.0;
	m_lowerAbsValue = 0.1;
}


void FloatingProbeData::eraseData()
{
	m_data.reset();
	m_insertPos = 0;
	m_resetTime = Simulator::self()->time();
}


ullong FloatingProbeData::findPos( llong time ) const
{
	if ( time <= 0 || ullong(time) <= m_resetTime || m_insertPos == 0 )
		return 0;
	
	ullong at = ullong((time-m_resetTime)*double(LINEAR_UPDATE_RATE)/double(LOGIC_UPDATE_RATE));
	
	if ( at >= m_insertPos )
		at = m_insertPos-1;
	
	return at;
}


ullong FloatingProbeData::toTime( ullong at ) const
{
	return ullong(m_resetTime + (at*LOGIC_UPDATE_RATE/LINEAR_UPDATE_RATE));
}


void FloatingProbeData::setScaling( Scaling scaling )
{
	if ( m_scaling == scaling )
		return;
	
	m_scaling = scaling;
	emit displayAttributeChanged();
}


void FloatingProbeData::setUpperAbsValue( double upperAbsValue )
{
	if ( m_upperAbsValue == upperAbsValue )
		return;
	
	m_upperAbsValue = upperAbsValue;
	emit displayAttributeChanged();
}


void FloatingProbeData::setLowerAbsValue( double lowerAbsValue )
{
	if ( m_lowerAbsValue == lowerAbsValue )
		return;
	
	m_lowerAbsValue = lowerAbsValue;
	emit displayAttributeChanged();
}
//END class FloatingProbeData


