/***************************************************************************
 *   Copyright (C) 2003,2005 by David Saxton                               * 
 *   david@bluehaze.org                                                    *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#include "canvasitemparts.h"
#include "eckeypad.h"
#include "libraryitem.h"
#include "switch.h"

#include "ecnode.h"
#include <KLocalizedString>

Item* ECKeyPad::construct( ItemDocument *itemDocument, bool newItem, const char *id )
{
	return new ECKeyPad( (ICNDocument*)itemDocument, newItem, id );
}

LibraryItem* ECKeyPad::libraryItem()
{
	return new LibraryItem(
		QStringList(QString("ec/keypad")),
		i18n("Keypad"),
		i18n("Switches"),
		"keypad.png",
		LibraryItem::lit_component,
		ECKeyPad::construct );
}

const QString text[4][9] =
	  { { "1","2","3","A","E","I","M","Q","U" },
		{ "4","5","6","B","F","J","N","R","V" },
		{ "7","8","9","C","G","K","O","S","W" },
		{ "*","0","#","D","H","L","P","T","X" } };
             
ECKeyPad::ECKeyPad( ICNDocument *icnDocument, bool newItem, const char *id )
	: Component( icnDocument, newItem, id ? id : "keypad" )
{
	m_name = i18n("Keypad");
	
	createProperty( "useToggles", Variant::Type::Bool );
	property("useToggles")->setCaption( i18n("Use Toggles") );
	property("useToggles")->setValue(false);
	
	createProperty( "numCols", Variant::Type::Int );
	property("numCols")->setCaption( i18n("Columns") );
	property("numCols")->setMinValue(3);
	property("numCols")->setMaxValue(9);
	property("numCols")->setValue(3);
	
	Variant * v = createProperty( "bounce", Variant::Type::Bool );
	v->setCaption("Bounce");
	v->setAdvanced(true);
	v->setValue(false);
	
	v = createProperty( "bounce_period", Variant::Type::Double );
	v->setCaption("Bounce Period");
	v->setAdvanced(true);
	v->setUnit("s");
	v->setValue(5e-3);
	
	for ( int i = 0; i < 4; i++ )
		createPin( 0, -32+i*24, 0, QString("row_%1").arg(QString::number(i)) );
	
	m_numCols = 0;
}


ECKeyPad::~ECKeyPad()
{
}


QString ECKeyPad::buttonID( int row, int col ) const
{
	return QString("b_%1_%2").arg(QString::number(row)).arg(QString::number(col));
}


int ECKeyPad::sideLength( unsigned numButtons ) const
{
	return 8 + 24*numButtons;
}


void ECKeyPad::dataChanged()
{
	initPins( dataInt("numCols") );
	
	bool useToggle = dataBool("useToggles");
	bool bounce = dataBool("bounce");
	int bouncePeriod_ms = int(dataDouble("bounce_period")*1e3);
	
	for ( unsigned i = 0; i < 4; i++ )
	{
		for ( unsigned j = 0; j < m_numCols; j++ )
		{
			button( buttonID(i,j) )->setToggle(useToggle);
			m_switch[i][j]->setBounce( bounce, bouncePeriod_ms );
		}
	}
}


void ECKeyPad::initPins( unsigned numCols )
{
	if ( numCols < 3 )
		numCols = 3;
	else if ( numCols > 9 )
		numCols = 9;
	
	if ( numCols == m_numCols )
		return;
	
	int w = sideLength(numCols);
	int h = sideLength(4);
	setSize( -int(w/16)*8, -int(h/16)*8, w, h, true );
	
	if ( numCols > m_numCols )
	{
		// Adding columns
		
		for ( unsigned i = 0; i < 4; i++ )
		{
			for ( unsigned j = m_numCols; j < numCols; j++ )
				addButton( buttonID(i,j), QRect( 0, 0, 20, 20 ), text[i][j] );
		}
		
		ECNode * cols[9];
	
		for ( unsigned j = m_numCols; j < numCols; j++ )
			cols[j] = createPin( 0, 64, 270, "col_" + QString::number(j) );
	
		for ( unsigned i = 0; i < 4; i++ )
		{
			ECNode * row = ecNodeWithID("row_"+QString::number(i));
			for ( unsigned j = m_numCols; j < numCols; j++ )
				m_switch[i][j] = createSwitch( cols[j], row, true );
		}
	}
	else
	{
		// Remove columns
		
		for ( unsigned i = 0; i < 4; i++ )
		{
			for ( unsigned j = numCols; j < m_numCols; j++ )
				removeWidget( buttonID(i,j) );
		}
	
		for ( unsigned j = numCols; j < m_numCols; j++ )
			removeNode( "col_" + QString::number(j) );
	
		for ( unsigned i = 0; i < 4; i++ )
		{
			for ( unsigned j = m_numCols; j < numCols; j++ )
				removeSwitch( m_switch[i][j] );	
		}
	}
	
	//BEGIN Update Positions
	m_numCols = numCols;
	
	for ( int i = 0; i < 4; i++ )
	{
		for ( int j = 0; j < int(m_numCols); j++ )
		{
			widgetWithID( buttonID(i,j) )->setOriginalRect(
					QRect( offsetX() + 6 + 24*j, offsetY() + 6 + 24*i, 20, 20 ) );
		}
	}
	
	for ( int i = 0; i < 4; i++ )
		m_nodeMap["row_" + QString::number(i)].x = width()+offsetX();
	
	for ( int j = 0; j < int(m_numCols); j++ )
		m_nodeMap["col_" + QString::number(j)].x = 24*j+offsetX()+16;
	
	updateAttachedPositioning();
	//END Update Positions
}


void ECKeyPad::buttonStateChanged( const QString &id, bool state )
{
	if ( !id.startsWith("b_") )
		return;
	
	QStringList tags = id.split('_', QString::SkipEmptyParts); // QStringList::split( '_', id ); // 2018.12.01
	const int i = tags[1].toInt();
	const int j = tags[2].toInt();
	m_switch[i][j]->setState( state ? Switch::Closed : Switch::Open );
}
