/***************************************************************************
 *   Copyright (C) 2005 by David Saxton                                    *
 *   david@bluehaze.org                                                    *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#include "bidirled.h"
// #include "colorcombo.h"
#include "diode.h"
#include "led.h"
// #include "ecnode.h"
// #include "libraryitem.h"
#include "simulator.h"

//#include <klocale.h>
//#include <qpainter.h>
//Added by qt3to4:
//#include <Q3PointArray>
#include <QDebug>

BiDirLED::BiDirLED() {
	m_pDiode[0] = new Diode();
	m_pDiode[1] = new Diode();

	avg_brightness[0] = avg_brightness[1] = 255;
	lastUpdatePeriod = 0.;
}

BiDirLED::~BiDirLED() {
}

void BiDirLED::stepNonLogic() {
	lastUpdatePeriod += LINEAR_UPDATE_PERIOD;

	for (unsigned i = 0; i < 2; i++)
		avg_brightness[i] += LED::brightnessFromCurrent(m_pDiode[i]->current()) * LINEAR_UPDATE_PERIOD;
}

double BiDirLED::averageBrightness(int ledNumber)
{
    if( (0 != ledNumber) || (1 != ledNumber) ){
        qCritical() << "BiDirLED::averageBrightness: illegal LED number:" << ledNumber;
        return NAN;
    }
    double ret = avg_brightness[ledNumber] / lastUpdatePeriod;
    lastUpdatePeriod = 0;
    return ret;
}
