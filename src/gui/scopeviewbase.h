/***************************************************************************
 *   Copyright (C) 2005-2006 by Jonathan Myers and David Saxton            *
 *   electronerd@electronerdia.net david@bluehaze.org                      *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#ifndef SCOPEVIEWBASE_H
#define SCOPEVIEWBASE_H

#include <qframe.h>

class Oscilloscope;
class Simulator;
class ProbeData;
class LogicProbeData;
class FloatingProbeData;

class QMouseEvent;
class QPaintEvent;
class QPixmap;
class QTimer;

typedef long long llong;

///base class of oscilloscope views
/**
 *
 * This is a refactoring of OscilloscopeView and my ScopeScreenView to promote
 * code reuse, both between the classes and within them. This is an abstract
 * class.
 * @author John Myers
 * @author David Saxton
 *
 */
class ScopeViewBase : public QFrame
{
    Q_OBJECT
public:
    ScopeViewBase(QWidget *parent = 0, const char *name = 0);
	virtual void drawBackground(QPainter & p) = 0;
	void resizeEvent( QResizeEvent *event ) override;
	void updateOutputHeight();


	
    ~ScopeViewBase() override;
protected:
	///Draw the horizontal line indicating the midpoint of our output for \c probe
 	virtual void drawMidLine(QPainter & p, ProbeData * probe) = 0;

	///\TODO: remove virtual; draw one logic probe
	virtual void drawProbe(QPainter& p, LogicProbeData * probe) = 0;
	///\TODO: remove virtual; draw one floating-point probe
	virtual void drawProbe(QPainter& p, FloatingProbeData * probe) = 0;

	/// gives the first Simulator tick visible in the view
	virtual llong visibleStartTime() const = 0;
	/// gives the last Simulator tick visible in the view
	virtual llong visibleEndTime() const = 0;

	virtual double ticksPerPixel() const = 0;
	virtual llong pixelsPerTick() const = 0;

	bool b_needRedraw;
	QPixmap *m_pixmap;
	double m_halfOutputHeight;

private:
	///draws a mapping of probes
	template<typename T>
		void drawProbeMap(QPainter& p, QMap<int, T*>& map);
	void paintEvent( QPaintEvent *event ) override;
};

#endif
