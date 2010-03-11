/***************************************************************************
 *   Copyright (C) 2005 by David Saxton                                    *
 *   david@bluehaze.org                                                    *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#ifndef PROBEPOSITIONER_H
#define PROBEPOSITIONER_H

#include <qwidget.h>

class ProbeData;
typedef QMap< int, ProbeData* > ProbeDataMap;

const float probeArrowWidth = 9;
const float probeArrowHeight = 12;

/**
Widget for positioning the output of Probes in the OscilloscopeView
@author David Saxton
*/
class ProbePositioner : public QWidget
{
	Q_OBJECT
	public:
		ProbePositioner(QWidget *parent = 0, const char *name = 0);
		~ProbePositioner();
		/**
		 * Returns the amount of space (height in pixels) that a probe output
		 * takes up
		 */
		int probeOutputHeight() const;
		/**
		 * Returns the probe position (from the top) in pixels that the probe
		 * with the given id should be displayed at, or -1 if probe with the
		 * given id couldn't be found
		 */
		int probePosition( ProbeData *probeData ) const;
		/**
		 * Sets the probe position relative to the top of this widget (and hence
		 * relative to the top of the oscilloscope view) in pixels
		 */
		void setProbePosition( ProbeData *probeData, int position );
		/**
		 * Returns the probe at the given position (plus or minus an an arrow),
		 * or NULL if none. Records the offset of the position from the mouse
		 * in m_probePosOffset.
		 */
		ProbeData* probeAtPosition( const QPoint &pos );
		
	public slots:
		void forceRepaint();
		
	protected slots:
		void slotProbeDataRegistered( int id, ProbeData *probe );
		void slotProbeDataUnregistered( int id );
		
	protected:
		virtual void mousePressEvent( QMouseEvent * e );
		virtual void mouseReleaseEvent( QMouseEvent * e );
		virtual void mouseMoveEvent( QMouseEvent * e );
		virtual void paintEvent( QPaintEvent *e );
		virtual void resizeEvent( QResizeEvent *event );
		
		ProbeDataMap m_probeDataMap;
		ProbeData *p_draggedProbe;
		int m_probePosOffset;
		
		bool b_needRedraw;
		QPixmap *m_pixmap;
};

#endif
