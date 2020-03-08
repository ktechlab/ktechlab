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

class Oscilloscope;
class Simulator;
class QMouseEvent;
class QPaintEvent;
class QPixmap;
class QTimer;

/**
@author David Saxton
*/
class OscilloscopeView : public QFrame
{
	Q_OBJECT
	public:
		OscilloscopeView( QWidget *parent, const char *name = nullptr);
		~OscilloscopeView() override;
		
	public slots:
		/**
		 * Sets the needRedraw flag to true, and then class repaint
		 */
		void updateView();
		void slotSetFrameRate( QAction * );

	protected slots:
		void updateViewTimeout();

	protected:
		void mousePressEvent( QMouseEvent *event) override;
		void mouseMoveEvent( QMouseEvent *event) override;
		void mouseReleaseEvent( QMouseEvent *event) override;
		void paintEvent( QPaintEvent *event) override;
		void resizeEvent( QResizeEvent *event) override;

		void drawLogicData( QPainter & p);
		void drawFloatingData( QPainter & p);
		void updateOutputHeight();
		void updateTimeLabel();

		bool b_needRedraw;
		QPixmap *m_pixmap;
		QTimer *m_updateViewTmr;
		int m_fps;
		int m_sliderValueAtClick;
		int m_clickOffsetPos;
		Simulator * m_pSimulator;
		double m_halfOutputHeight;
};

#endif
