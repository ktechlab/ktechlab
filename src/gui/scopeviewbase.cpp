/***************************************************************************
 *   Copyright (C) 2005-2006 by Jonathan Myers and David Saxton            *
 *   electronerd@electronerdia.net david@bluehaze.org                      *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#include "scopeviewbase.h"

#include "oscilloscope.h"
#include "oscilloscopedata.h"
#include "probepositioner.h"

#include <qpainter.h>
#include <qevent.h>
#include <qdebug.h>

//for testing 
//#include <valgrind/callgrind.h>

ScopeViewBase::ScopeViewBase(QWidget *parent, const char *name)
: QFrame(parent, /* name, */ Qt::WNoAutoErase),
b_needRedraw(true),
m_pixmap(0L),
m_halfOutputHeight(0.0)
{
    setObjectName( name );
}


ScopeViewBase::~ScopeViewBase()
{
	delete m_pixmap;
}

void ScopeViewBase::paintEvent( QPaintEvent * event )
{
	QRect r = event->rect();
	
	if (b_needRedraw)
	{
		//CALLGRIND_TOGGLE_COLLECT();
		
		updateOutputHeight();

        if (!m_pixmap) {
            qWarning() << Q_FUNC_INFO << "unexpected null pixmap in " << this;
            return;
        }

		QPainter p;
		//m_pixmap->fill( paletteBackgroundColor() ); // 2018.12.07
        m_pixmap->fill( palette().color(backgroundRole()) );
		const bool startSuccess = p.begin(m_pixmap);
        if ((!startSuccess) || (!p.isActive())) {
            qWarning() << Q_FUNC_INFO << " painter is not active";
        }
		p.setClipRegion(event->region());
		
		//let the subclass draw the background (grids, etc.)
		drawBackground(p);
		
//		drawProbeMap(p, Oscilloscope::self()->m_logicProbeDataMap);	
//		drawProbeMap(p, Oscilloscope::self()->m_floatingProbeDataMap);	
		
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
		T * probe = it.value();
		
		if ( probe->isEmpty() )
			return;
		
		drawMidLine( p, probe );
		
		// Set the pen colour according to the colour the user has selected for the probe
		p.setPen( probe->color() );
		
		drawProbe( p, probe );
	}
}
#include "scopeviewbase.moc"
