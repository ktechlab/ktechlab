/***************************************************************************
 *   Copyright (C) 2003-2005 by David Saxton                               *
 *   david@bluehaze.org                                                    *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#include "ecnode.h"
#include "ecsignallamp.h"
#include "element.h"
#include "libraryitem.h"
#include "pin.h"

#include <KLocalizedString>
#include <qpainter.h>
#include <cmath>

// TODO: resistance and power rating should be user definable properties.
#define RESISTANCE 100
#define WATTAGE    0.5
// minimal power to create glow. (looks low...)
#define LIGHTUP   (WATTAGE / 20)

Item* ECSignalLamp::construct( ItemDocument *itemDocument, bool newItem, const char *id )
{
	return new ECSignalLamp( (ICNDocument*)itemDocument, newItem, id );
}

LibraryItem* ECSignalLamp::libraryItem()
{
	return new LibraryItem(
		QStringList(QString("ec/signal_lamp")),
		i18n("Signal Lamp"),
		i18n("Outputs"),
		"signal_lamp.png",
		LibraryItem::lit_component,
		ECSignalLamp::construct );
}

ECSignalLamp::ECSignalLamp( ICNDocument *icnDocument, bool newItem, const char *id )
	: Component( icnDocument, newItem, id ? id : "signal_lamp" )
{
	m_name = i18n("Signal Lamp");
	setSize( -8, -8, 16, 16 );
	
	init1PinLeft();
	init1PinRight();
	
	createResistance( m_pPNode[0], m_pNNode[0], RESISTANCE );
	
	advanceSinceUpdate = 0;
	avgPower = 0.;
	m_bDynamicContent = true;
}

ECSignalLamp::~ECSignalLamp()
{
}

void ECSignalLamp::stepNonLogic()
{
	const double voltage = m_pPNode[0]->pin()->voltage()-m_pNNode[0]->pin()->voltage();
    if (advanceSinceUpdate == 0) {
        advanceSinceUpdate = 1; // do not try to divide by 0
    }
	avgPower = fabs(avgPower * advanceSinceUpdate +
			(voltage * voltage / RESISTANCE)) /
			advanceSinceUpdate;
    ++advanceSinceUpdate;
}

void ECSignalLamp::drawShape( QPainter &p )
{
	initPainter(p);

	int _x = int(x());
	int _y = int(y());

	// Calculate the brightness as a linear function of power, bounded below by
	// 25 milliWatts and above by 500 milliWatts.
	int brightness = (avgPower < LIGHTUP) ? 255 :
			((avgPower > WATTAGE) ? 0 : (int)(255 * (1 - ((avgPower - LIGHTUP) / (WATTAGE - LIGHTUP)))));
	advanceSinceUpdate = 0;
	
	p.setBrush( QColor( 255, 255, brightness ) );
	p.drawEllipse( _x-8, _y-8, 16, 16 );
	
	int pos = 8 - int(8 * M_SQRT1_2);
	
	p.drawLine( _x-8+pos, _y-8+pos, _x+8-pos, _y+8-pos );
	p.drawLine( _x+8-pos, _y-8+pos, _x-8+pos, _y+8-pos );
	
	deinitPainter(p);
}
