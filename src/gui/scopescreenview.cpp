/***************************************************************************
 *   Copyright (C) 2005 by John Myers                                      *
 *   electronerd@electronerdia.net                                         *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#include "oscilloscope.h"
#include "oscilloscopedata.h"
#include "probe.h"
#include "scopescreenview.h"
#include "simulator.h"
#include "probepositioner.h"

#include <qpainter.h>
#include <qtimer.h>
#include <kdebug.h>

#include <cmath>
#define FADESPEED 1

ScopeScreenView::ScopeScreenView(QWidget *parent, const char *name)
: ScopeViewBase(parent, name),
m_intervalsX(10),
m_ticksPerIntervalX(10000),
m_offsetX(0)
{
	m_updateViewTmr = new QTimer(this);
	connect( m_updateViewTmr, SIGNAL(timeout()), this, SLOT(updateViewTimeout()) );
	m_updateViewTmr->start(50);
}


ScopeScreenView::~ScopeScreenView()
{}

#if 0
void ScopeScreenView::drawContents(QPainter * p)
{
	QRect cr = contentsRect();

	for(int i =1; i < m_intervalsX; i++)
	{
		int x = cr.left() + cr.width()*i/m_intervalsX;
		p->drawLine(x, cr.top(), x, cr.bottom());
	}
	const int ticksPerScreen = m_intervalsX * m_ticksPerIntervalX;
	const double pixelsPerTick = cr.width()/double(ticksPerScreen);
	const double ticksPerPixel = m_intervalsX * m_ticksPerIntervalX / cr.width();	
	//draw the current time
	int curTimeX = ((Simulator::self()->time() + m_offsetX) % (ticksPerScreen)) * pixelsPerTick;
	//kDebug() << curTimeX <<endl;
	p->drawLine(curTimeX, cr.top(), curTimeX, cr.bottom());
	
	//the following is liberally borrowed from OscilloscopeView::drawFloatingData
	const FloatingProbeDataMap::iterator end = Oscilloscope::self()->m_floatingProbeDataMap.end();
	for ( FloatingProbeDataMap::iterator it = Oscilloscope::self()->m_floatingProbeDataMap.begin(); it != end; ++it )
	{
		FloatingProbeData * probe = it.data();
		StoredData<float> * data = &(probe->m_data);
		
		if ( data->allocatedUpTo() == 0 )
			continue;
		
		bool logarithmic = probe->scaling() == FloatingProbeData::Logarithmic;
		double lowerAbsValue = probe->lowerAbsValue();
		double sf = ((cr.height()/Oscilloscope::self()->numberOfProbes())/2) / (logarithmic ? log(probe->upperAbsValue()/lowerAbsValue) : probe->upperAbsValue());
		
		const int midHeight = Oscilloscope::self()->probePositioner->probePosition(probe);
		//const int midHeight = cr.top() + cr.height()/2;		
		//const llong timeOffset = Oscilloscope::self()->scrollTime();
		const llong timeOffset = Simulator::self()->time() - (FADESPEED * m_intervalsX * m_ticksPerIntervalX);
		
		// Draw the horizontal line indicating the midpoint of our output
		p->setPen( QColor( 228, 228, 228 ) );
		p->drawLine( 0, midHeight, width(), midHeight );
		
		// Set the pen colour according to the colour the user has selected for the probe
		p->setPen( probe->color() );
		
		llong at = probe->findPos(timeOffset); 
		const llong maxAt = probe->insertPos();
		llong prevTime = probe->toTime(at);
		
		double v = data->dataAt((at>0)?at:0);
		int prevY = int(midHeight - (logarithmic ? ( (v>0) ? log(v/lowerAbsValue) : -log(-v/lowerAbsValue) ) : v) * sf);
		int prevX = (int((prevTime - timeOffset)*pixelsPerTick) + curTimeX) % cr.width();
		
		while ( at < maxAt-1 )
		{
			at++;
			
			ullong nextTime = probe->toTime(at);
			
			double v = data->dataAt((at>0)?at:0);
			int nextY = int(midHeight - (logarithmic ? ( (v>0) ? log(v/lowerAbsValue) : -log(-v/lowerAbsValue) ) : v) * sf);
			int nextX = (int((nextTime - timeOffset)*pixelsPerTick) + curTimeX) % cr.width();
			if(nextX < prevX)
			{
				prevX = 0;
			}
			//kDebug() <<at<<" "<<nextX<<" "<<nextY<<" "<<nextTime<<endl;
			p->drawLine( prevX, prevY, nextX, nextY );
			
			prevTime = nextTime;
			prevX = nextX;
			prevY = nextY;
			
			//if ( nextX > width() )
			//break;
		};
		
		// If we could not draw right to the end; it is because we exceeded
		// maxAt
		//if ( prevX < curTimeX )
		//	p->drawLine( prevX, prevY, curTimeX, prevY );
	}
	
	//and this was liberally borrowed from OscilloscopeView::DrawLogicData
	{
	const LogicProbeDataMap::iterator end = Oscilloscope::self()->m_logicProbeDataMap.end();
	for ( LogicProbeDataMap::iterator it = Oscilloscope::self()->m_logicProbeDataMap.begin(); it != end; ++it )
	{
		// When searching for the next logic value to display, we look along
		// until there is a recorded point which is at least one pixel along
		// If we are zoomed out far, there might be thousands of data points
		// between each pixel. It is time consuming searching for the next point
		// to display one at a time, so we record the average number of data points
		// between pixels ( = deltaAt / totalDeltaAt )
		llong deltaAt = 1;
		int totalDeltaAt = 1;
		
		LogicProbeData * probe = it.data();
		StoredData<LogicDataPoint> * data = &(probe->m_data);
		
		if ( data->allocatedUpTo() == 0 )
			continue;
		
		const int midHeight = Oscilloscope::self()->probePositioner->probePosition(probe);
		const llong timeOffset = Simulator::self()->time() - (FADESPEED * m_intervalsX * m_ticksPerIntervalX);//Oscilloscope::self()->scrollTime();
		
		const int halfOutputHeight = ((cr.height()/Oscilloscope::self()->numberOfProbes())/2);
		
		// Draw the horizontal line indicating the midpoint of our output
		p->setPen( QColor( 228, 228, 228 ) );
		p->drawLine( 0, midHeight, width(), midHeight );
		
		// Set the pen colour according to the colour the user has selected for the probe
		p->setPen( probe->color() );
		
		// The smallest time step that will display in our oscilloscope
		const int minTimeStep = ticksPerPixel;//int(LOGIC_UPDATE_RATE/pixelsPerSecond);
		
		llong at = probe->findPos(timeOffset);
		const llong maxAt = probe->insertPos();
		llong prevTime = data->dataAt(at).time;
		int prevX = (int((prevTime - timeOffset)*pixelsPerTick) + curTimeX) % cr.width();
		bool prevHigh = data->dataAt(at).value;
		int prevY = midHeight + int(prevHigh ? -halfOutputHeight : +halfOutputHeight);
		while ( at < maxAt )
		{
			// Search for the next pos which will show up at our zoom level
			llong previousAt = at;
			llong dAt = deltaAt / totalDeltaAt;
			
			while ( (dAt > 1) && (at < maxAt) && ( (llong(data->dataAt(at).time) - prevTime) != minTimeStep ) )
			{
				// Search forwards until we overshoot
				while ( at < maxAt && ( llong(data->dataAt(at).time) - prevTime ) < minTimeStep )
					at += dAt;
				dAt /= 2;
				
				// Search backwards until we undershoot
				while ( (at < maxAt) && ( llong(data->dataAt(at).time) - prevTime ) > minTimeStep )
				{
					at -= dAt;
					if ( at < 0 )
						at = 0;
				}
				dAt /= 2;
			}
			
			// Possibly increment the value of at found by one (or more if this is the first go)
			while ( (previousAt == at) || ((at < maxAt) && ( llong(data->dataAt(at).time) - prevTime ) < minTimeStep) )
				at++;
			
			if ( at >= maxAt )
				break;
			
			// Update the average values
			deltaAt += at - previousAt;
			totalDeltaAt++;
			
			bool nextHigh = data->dataAt(at).value;
			if ( nextHigh == prevHigh )
				continue;
			llong nextTime = data->dataAt(at).time;
			int nextX = (int((nextTime - timeOffset)*pixelsPerTick) + curTimeX) % cr.width();
			int nextY = midHeight + int(nextHigh ? -halfOutputHeight : +halfOutputHeight);
			
			p->drawLine( prevX, prevY, nextX, prevY );
			p->drawLine( nextX, prevY, nextX, nextY );
			
			prevHigh = nextHigh;
			prevTime = nextTime;
			prevX = nextX;
			prevY = nextY;
			
			if ( nextX > width() )
				break;
		};
		
		// If we could not draw right to the end; it is because we exceeded
		// maxAt
		//if ( prevX < width() )
		//	p->drawLine( prevX, prevY, width(), prevY );
	}
	}
}
#endif
void ScopeScreenView::setIntervalsX( int value )
{
	m_intervalsX = value;
}

void ScopeScreenView::setTicksPerIntervalX( int value )
{
	m_ticksPerIntervalX = value;
}

void ScopeScreenView::setOffsetX( int value )
{
	m_offsetX = value;
}

void ScopeScreenView::updateViewTimeout( )
{
	repaint();
}

void ScopeScreenView::drawBackground( QPainter & p )
{
	QRect cr = contentsRect();
	
	for(int i =1; i < m_intervalsX; i++)
	{
		int x = cr.left() + cr.width()*i/m_intervalsX;
		p.drawLine(x, cr.top(), x, cr.bottom());
	}
	
	///\todo REMOVE THIS NOTICE
 		
	p.drawText(cr.left(), cr.top(), "NOT YET IMPLEMENTED");
	
}

void ScopeScreenView::drawMidLine( QPainter & p, ProbeData * probe )
{
	const int midHeight = Oscilloscope::self()->probePositioner->probePosition(probe);
	
	// Draw the horizontal line indicating the midpoint of our output
	p.setPen( QColor( 228, 228, 228 ) );
	p.drawLine( 0, midHeight, width(), midHeight );
}


void ScopeScreenView::drawProbe( QPainter & p, LogicProbeData * probe )
{
}

void ScopeScreenView::drawProbe( QPainter & p, FloatingProbeData * probe )
{
}

#include "scopescreenview.moc"

