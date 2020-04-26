/***************************************************************************
 *   Copyright (C) 2005 by David Saxton                                    *
 *   david@bluehaze.org                                                    *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#include "port.h"
#include "parallelportcomponent.h"

#include "ecnode.h"
#include "itemdocument.h"
#include "libraryitem.h"
#include "pin.h"
#include "resistance.h"

#include <qdebug.h>
#include <KLocalizedString>
#include <qpainter.h>

#include <cmath>
#include <termios.h>

Item* ParallelPortComponent::construct( ItemDocument *itemDocument, bool newItem, const char *id )
{
	return new ParallelPortComponent( (ICNDocument*)itemDocument, newItem, id );
}

LibraryItem* ParallelPortComponent::libraryItem()
{
	return new LibraryItem(
			QStringList(QString("ec/parallel_port")),
	i18n("Parallel Port"),
	i18n("Connections"),
	"ic1.png",
	LibraryItem::lit_component,
	ParallelPortComponent::construct
						  );
}

ParallelPortComponent::ParallelPortComponent( ICNDocument *icnDocument, bool newItem, const char *id )
	: Component( icnDocument, newItem, id ? id : "parallel_port" )
{
	m_name = i18n("Parallel Port");
	
	QPolygon pa( 4 );
	pa[0] = QPoint( -32, -112 );
	pa[1] = QPoint( 32, -104 );
	pa[2] = QPoint( 32, 104 );
	pa[3] = QPoint( -32, 112 );
	setItemPoints( pa );
	
	m_pParallelPort = new ParallelPort();
	
	for ( unsigned i = 0; i < 24; ++i )
		m_pLogic[i] = nullptr;
	
	ECNode * pin = nullptr;
	
	//BEGIN Data register
	for ( int i = 0; i < 8; ++i )
	{
		QString id = QString("D%1").arg(i);
		QString name = id;
		
		pin = createPin( -40, -80 + 16*i, 0, id );
		addDisplayText( id, QRect( -28, -88 + 16*i, 28, 16 ), name, true, Qt::AlignLeft | Qt::AlignVCenter );
		
		m_pLogic[i] = createLogicOut( pin, false );
		m_pLogic[i]->setCallback( this, (CallbackPtr)(&ParallelPortComponent::dataCallback) );
	}
	//END Data register
	
	
	//BEGIN Status register
	QString statusNames[] = { "ERR", "ON", "PE", "ACK", "BUSY" };
	
	// The statusIDs are referenced in the save file and must not change
	QString statusIDs[] = { "ERROR", "ONLINE", "PE", "ACK", "BUSY" };
	
	// Bits 0...2 in the Status register are not used
	for ( int i = 3; i < 8; ++i )
	{
		QString id = statusIDs[i-3];
		QString name = statusNames[i-3];
		
		// Bit 3 (pin 15) doesn't not follow the same positioning pattern as
		// the other pins in the Status register.
		if ( i == 3 )
		{
			pin = createPin( 40, -72, 180, id );
			addDisplayText( id, QRect( 0, -80, 28, 16 ), name, true, Qt::AlignRight | Qt::AlignVCenter );
		}
		else
		{
			pin = createPin( -40, -16 + 16*i, 0, id );
			addDisplayText( id, QRect( -28, -24 + 16*i, 28, 16 ), name, true, Qt::AlignLeft | Qt::AlignVCenter );
		}
		
		m_pLogic[i+8] = createLogicOut( pin, false );
	}
	//END Status register
	
	
	//BEGIN Control register
	QString controlNames[] = { "STR", "AUT", "INIT", "SEL" };
	
	// The controlIDs are referenced in the save file and must not change
	QString controlIDs[] = { "STROBE", "AUTO", "INIT", "SELECT" };
	
	// Bits 4..7 are not used (well; bit 5 is, but not as a pin)
	for ( int i = 0; i < 4; ++i )
	{
		QString id = controlIDs[i];
		QString name = controlNames[i];
		
		if ( i == 0 )
		{
			pin = createPin( -40, -96, 0, id );
			addDisplayText( id, QRect( -28, -104, 28, 16 ), name, true, Qt::AlignLeft | Qt::AlignVCenter );
		}
		else if ( i == 1 )
		{
			pin = createPin( 40, -88, 180, id );
			addDisplayText( id, QRect( 0, -96, 28, 16 ), name, true, Qt::AlignRight | Qt::AlignVCenter );
		}
		else
		{
			pin = createPin( 40, -88 + i*16, 180, id );
			addDisplayText( id, QRect( 0, -96 + i*16, 28, 16 ), name, true, Qt::AlignRight | Qt::AlignVCenter );
		}
		
		m_pLogic[i+16] = createLogicOut( pin, false );
		m_pLogic[i+16]->setCallback( this, (CallbackPtr)(&ParallelPortComponent::controlCallback) );
	}
	//END Control register
	
	
#if 0
	// And make the rest of the pins ground
	for ( int i = 0; i < 8; ++i )
	{
		pin = createPin( 40, -24 + i*16, 180, QString("GND%1").arg( i ) );
		pin->pin()->setGroundType( Pin::gt_always );
	}
#endif
	
	Variant * v = createProperty( "port", Variant::Type::Combo );
	v->setAllowed( ParallelPort::ports( Port::ExistsAndRW ) );
	v->setCaption( i18n("Port") );
}


ParallelPortComponent::~ParallelPortComponent()
{
    for (int i = 0; i < 24; i++) {
        if (m_pLogic[i]) {
            m_pLogic[i]->setCallback(nullptr, nullptr);
        }
    }
	delete m_pParallelPort;
}


void ParallelPortComponent::dataChanged()
{
	initPort( dataString("port") );
}


void ParallelPortComponent::initPort( const QString & port )
{
	if ( port.isEmpty() )
	{
		m_pParallelPort->closePort();
		return;
	}
	
	if ( ! m_pParallelPort->openPort( port ) )
	{
		p_itemDocument->canvas()->setMessage( i18n("Could not open port %1", port ) );
		return;
	}
}


void ParallelPortComponent::dataCallback( bool )
{
	uchar value = 0;
	for ( unsigned i = 0; i < 8; ++ i )
		value |= m_pLogic[ i + 0 ]->isHigh() ? 0 : (1 << i);
	
	m_pParallelPort->writeToData( value );
}


void ParallelPortComponent::controlCallback( bool )
{
	uchar value = 0;
	for ( unsigned i = 0; i < 4; ++ i )
		value |= m_pLogic[ i + 16 ]->isHigh() ? 0 : (1 << i);
	
	m_pParallelPort->writeToControl( value );
}


void ParallelPortComponent::stepNonLogic()
{
	uchar status = m_pParallelPort->readFromRegister( ParallelPort::Status );
	// Bits 0...2 in the Status register are not used
	for ( int i = 3; i < 8; ++i )
		m_pLogic[i + 8]->setHigh( status | (1 << i) );
}


void ParallelPortComponent::drawShape( QPainter & p )
{
	drawPortShape( p );
}
