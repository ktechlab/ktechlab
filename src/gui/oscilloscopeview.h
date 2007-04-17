/***************************************************************************
 *   Copyright (C) 2005 by David Saxton                                    *
 *   david@bluehaze.org                                                    *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#ifndef OSCILLOSCOPEVIEW_H
#define OSCILLOSCOPEVIEW_H

#include <qframe.h>
#include "scopeviewbase.h"

class Oscilloscope;
class Simulator;
class QMouseEvent;
class QPaintEvent;
class QPixmap;
class QTimer;

///A printing tape-style view of oscilloscope data
/**
@author David Saxton
*/
class OscilloscopeView : public ScopeViewBase
{
	Q_OBJECT
	public:
		OscilloscopeView( QWidget *parent, const char *name = 0 );
		virtual ~OscilloscopeView();
		
	public slots:
		/**
		 * Sets the needRedraw flag to true, and then class repaint
		 */
		void updateView();
		void slotSetFrameRate( int fps );
		
	protected slots:
		void updateViewTimeout();
		
	protected:
		virtual void mousePressEvent( QMouseEvent *event );
		virtual void mouseMoveEvent( QMouseEvent *event );
		virtual void mouseReleaseEvent( QMouseEvent *event );
	
		virtual void drawBackground( QPainter& p );
		virtual void drawMidLine(QPainter & p, ProbeData * probe);
	
	
		virtual llong visibleStartTime() const;
		virtual llong visibleEndTime() const;
	
		virtual double ticksPerPixel() const;
		virtual llong pixelsPerTick() const;
	
		void drawProbe( QPainter & p, LogicProbeData * probe);
		void drawProbe( QPainter & p, FloatingProbeData * probe );
		void updateTimeLabel();

		QTimer *m_updateViewTmr;
		int m_fps;
		int m_sliderValueAtClick;
		int m_clickOffsetPos;
		Simulator * m_pSimulator;
		
};

#endif
