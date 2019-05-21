/***************************************************************************
 *   Copyright (C) 2005 by John Myers                                      *
 *   electronerd@electronerdia.net                                         *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#ifndef SCOPESCREENVIEW_H
#define SCOPESCREENVIEW_H
#include "scopeviewbase.h"
#include <qframe.h>
class QTimer;

///Screen-type Oscilloscope data view
/**
An Oscilloscope screen, as opposed to a paper tape (the current Oscilloscope)
 
	@author John Myers 
*/
class ScopeScreenView : public ScopeViewBase
{
    Q_OBJECT
public:
    ScopeScreenView(QWidget *parent = 0, const char *name = 0);

    ~ScopeScreenView() override;
	//virtual void drawContents(QPainter * p);
	void drawBackground(QPainter & p) override;
	
		///Draw the horizontal line indicating the midpoint of our output for \c probe
	void drawMidLine(QPainter & p, ProbeData * probe) override;
	
	///\TODO: remove virtual; draw one logic probe
	void drawProbe(QPainter& p, LogicProbeData * probe) override;
	///\TODO: remove virtual; draw one floating-point probe
	void drawProbe(QPainter& p, FloatingProbeData * probe) override;
	
		/// gives the first Simulator tick visible in the view
	llong visibleStartTime() const override {return 0;}
	/// gives the last Simulator tick visible in the view
	llong visibleEndTime() const override {return 0;}
	
	double ticksPerPixel() const override {return 0;}
	llong pixelsPerTick() const override {return 0;}
	
public slots:
	void setIntervalsX(int value);
	void setTicksPerIntervalX(int value);
	void setOffsetX(int value);
	
	void updateViewTimeout();
	
private:
	int m_intervalsX;
	int m_ticksPerIntervalX;
	int m_offsetX;
	
	QTimer* m_updateViewTmr;

};

#endif
