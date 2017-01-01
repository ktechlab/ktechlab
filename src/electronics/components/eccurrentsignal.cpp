/***************************************************************************
 *   Copyright (C) 2003-2004 by David Saxton                               *
 *   david@bluehaze.org                                                    *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#include "currentsignal.h"
#include "eccurrentsignal.h"
#include "ecnode.h"
#include "libraryitem.h"
#include "pin.h"
#include "simulator.h"

#include <klocalizedstring.h>
#include <Qt/qpainter.h>

Item* ECCurrentSignal::construct( ItemDocument *itemDocument, bool newItem, const char *id )
{
	return new ECCurrentSignal( (ICNDocument*)itemDocument, newItem, id );
}

LibraryItem* ECCurrentSignal::libraryItem()
{
	return new LibraryItem(
		QStringList(QString("ec/ac_current")),
		i18n("Current Signal"),
		i18n("Sources"),
		"currentsignal.png",
		LibraryItem::lit_component,
		ECCurrentSignal::construct );
}

ECCurrentSignal::ECCurrentSignal( ICNDocument *icnDocument, bool newItem, const char *id )
	: Component( icnDocument, newItem, id ? id : "current_signal" )
{
	m_name = i18n("Current Signal");
	setSize( -8, -8, 16, 16 );
	
	init1PinLeft();
	init1PinRight();
	
	m_pNNode[0]->pin()->setGroundType( Pin::gt_low );
	m_currentSignal = createCurrentSignal( m_pNNode[0], m_pPNode[0], 0. );
	m_currentSignal->setStep(ElementSignal::st_sinusoidal, 50. );
	
	createProperty( "1-frequency", Variant::Type::Double );
	property("1-frequency")->setCaption( i18n("Frequency") );
	property("1-frequency")->setUnit("Hz");
	property("1-frequency")->setMinValue(1e-9);
	property("1-frequency")->setMaxValue(1e3);
	property("1-frequency")->setValue(50.0);
	
	createProperty( "1-current", Variant::Type::Double );
	property("1-current")->setCaption( i18n("Current Range") );
	property("1-current")->setUnit("A");
	property("1-current")->setMinValue(-1e12);
	property("1-current")->setMaxValue(1e12);
	property("1-current")->setValue(0.02);
	
	addDisplayText( "~", QRect( -8, -8, 16, 16 ), "~" );
	addDisplayText( "current", QRect( -16, -24, 32, 16 ), "" );
}


ECCurrentSignal::~ECCurrentSignal()
{
}

void ECCurrentSignal::dataChanged()
{
	const double current = dataDouble("1-current");
	const double frequency = dataDouble("1-frequency");
	
	QString display = QString::number( current / getMultiplier(current), 'g', 3 ) + getNumberMag(current) + "A";
	setDisplayText( "current", display );
	
	m_currentSignal->setStep(ElementSignal::st_sinusoidal, frequency );
	m_currentSignal->setCurrent(current);
}

void ECCurrentSignal::drawShape( QPainter &p )
{
	initPainter(p);
	p.drawEllipse( (int)x()-8, (int)y()-8, width(), height() );
	deinitPainter(p);
}

