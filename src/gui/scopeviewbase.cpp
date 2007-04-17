/***************************************************************************
 *   Copyright (C) 2005-2006 by Jonathan Myers and David Saxton            *
 *   electronerd@electronerdia.net david@bluehaze.org                      *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/


#include <qpainter.h>

#include "oscilloscope.h"
#include "oscilloscopedata.h"
#include "probepositioner.h"


#include "scopeviewbase.h"

//for testing 
//#include <valgrind/callgrind.h>

ScopeViewBase::ScopeViewBase(QWidget *parent, const char *name)
: QFrame(parent, name, WNoAutoErase),
b_needRedraw(true),
m_pixmap(0L),
m_halfOutputHeight(0.0)
{}


ScopeViewBase::~ScopeViewBase()
{
	delete m_pixmap;
	m_pixmap = 0l;
}

void ScopeViewBase::paintEvent( QPaintEvent * event )
{
	QRect r = event->rect();
	
	if (b_needRedraw)
	{
		//CALLGRIND_TOGGLE_COLLECT();
		
		updateOutputHeight();
		
		QPainter p;
		m_pixmap->fill( paletteBackgroundColor() );
		p.begin(m_pixmap);
		p.setClipRegion(event->region());
		
		//let the subclass draw the background (grids, etc.)
		drawBackground(p);
		
		drawProbeMap(p, Oscilloscope::self()->m_logicProbeDataMap);	
		drawProbeMap(p, Oscilloscope::self()->m_floatingProbeDataMap);	
		
		p.setPen(Qt::black);
		p.drawRect( frameRect() );
		
		b_needRedraw = false;
		
		//CALLGRIND_TOGGLE_COLLECT();
	}
	
	bitBlt( this, r.x(), r.y(), m_pixmap, r.x(), r.y(), r.width(), r.height() );
}
void ScopeViewBase::updateOutputHeight()
{
	m_halfOutputHeight = int((Oscilloscope::self()->probePositioner->probeOutputHeight() - (probeArrowWidth/Oscilloscope::self()->numberOfProbes()))/2)-1;
}

void ScopeViewBase::resizeEvent( QResizeEvent * event )
{
	delete m_pixmap;
	m_pixmap = new QPixmap( event->size() );
	b_needRedraw = true;
	QFrame::resizeEvent(event);
}
/**
 * This is the main drawing loop function.
 */
template <typename T>
void ScopeViewBase::drawProbeMap( QPainter & p, QMap< int, T * > & map )
{
	typedef typename QMap<int, T*>::iterator TheIterator;
	const TheIterator end = map.end();
	for ( TheIterator it = map.begin(); it != end; ++it )
	{
		T * probe = it.data();
		
		if ( probe->m_data.allocatedUpTo() == 0 )
			return;
		
		drawMidLine( p, probe );
		
		// Set the pen colour according to the colour the user has selected for the probe
		p.setPen( probe->color() );
		
		drawProbe( p, probe );
	}
}
#include "scopeviewbase.moc"
