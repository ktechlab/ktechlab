/***************************************************************************
 *   Copyright (C) 2004-2005 by David Saxton                               *
 *   david@bluehaze.org                                                    *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#include "demultiplexer.h"

#include "logic.h"
#include "libraryitem.h"

#include <kiconloader.h>
#include <klocalizedstring.h>

#include <cmath>

Item* Demultiplexer::construct( ItemDocument *itemDocument, bool newItem, const char *id )
{
	return new Demultiplexer( (ICNDocument*)itemDocument, newItem, id );
}

LibraryItem* Demultiplexer::libraryItem()
{
	return new LibraryItem(
		QStringList(QString("ec/demultiplexer")),
		i18n("Demultiplexer"),
		i18n("Integrated Circuits"),
		"ic1.png",
		LibraryItem::lit_component,
		Demultiplexer::construct
			);
}

Demultiplexer::Demultiplexer( ICNDocument *icnDocument, bool newItem, const char *id )
	: Component( icnDocument, newItem, id ? id : "demultiplexer" )
{
	m_name = i18n("Demultiplexer");
	
	m_input = nullptr;
	
	createProperty( "addressSize", Variant::Type::Int );
	property("addressSize")->setCaption( i18n("Address Size") );
	property("addressSize")->setMinValue(1);
	property("addressSize")->setMaxValue(8);
	property("addressSize")->setValue(1);
	
	// For backwards compatibility
	createProperty( "numInput", Variant::Type::Int );
	property("numInput")->setMinValue(-1);
	property("numInput")->setValue(-1);
	property("numInput")->setHidden(true);
}

Demultiplexer::~Demultiplexer()
{
}


void Demultiplexer::dataChanged()
{
	if ( hasProperty("numInput") && dataInt("numInput") != -1 )
	{
		int addressSize = int( std::ceil( std::log( (double)dataInt("numInput") ) / std::log(2.0) ) );
		property("numInput")->setValue(-1);
		
		if ( addressSize < 1 )
			addressSize = 1;
		else if ( addressSize > 8 )
			addressSize = 8;
		
		// This function will get called again when we set the value of numInput
		property("addressSize")->setValue(addressSize);
		return;
	}
	
	if ( hasProperty("numInput") )
	{
		m_variantData["numInput"]->deleteLater();
		m_variantData.remove("numInput");
	}
	
	initPins( unsigned(dataInt("addressSize")) );
}


void Demultiplexer::inStateChanged( bool /*state*/ )
{
	unsigned long long pos = 0;
	for ( unsigned i = 0; i < m_aLogic.size(); ++i )
	{
		if ( m_aLogic[i]->isHigh() )
			pos += 1 << i;
	}
	for ( unsigned i = 0; i < m_xLogic.size(); ++i )
		m_xLogic[i]->setHigh( (pos == i) && m_input->isHigh() );
}


void Demultiplexer::initPins( unsigned newAddressSize )
{
	unsigned oldAddressSize = m_aLogic.size();
	unsigned long long oldXLogicCount = m_xLogic.size();
	unsigned long long newXLogicCount = 1 << newAddressSize;
	
	if ( newXLogicCount == oldXLogicCount )
		return;
	
	QStringList pins;
	
	for ( unsigned i=0; i<newAddressSize; ++i )
		pins += "A"+QString::number(i);
	for ( unsigned i=newAddressSize; i<(newXLogicCount+(newXLogicCount%2))/2; ++i )
		pins += "";
	pins += "X";
	for ( unsigned i=(newXLogicCount+(newXLogicCount%2))/2+1; i<newXLogicCount; ++i )
		pins += "";
	for ( int i=newXLogicCount-1; i>=0; --i )
		pins += "X"+QString::number(i);
	
	initDIPSymbol( pins, 64 );
	initDIP(pins);
	
	ECNode *node;
	
	if (!m_input)
	{
		node =  ecNodeWithID("X");
		m_input = createLogicIn(node);
		m_input->setCallback( this, (CallbackPtr)(&Demultiplexer::inStateChanged) );
	}
	
	if ( newXLogicCount > oldXLogicCount )
	{
		m_xLogic.resize(newXLogicCount);
		for ( unsigned i = oldXLogicCount; i < newXLogicCount; ++i )
		{
			node = ecNodeWithID("X"+QString::number(i));
			m_xLogic.insert( i, createLogicOut(node,false) );
		}
		
		m_aLogic.resize(newAddressSize);
		for ( unsigned i = oldAddressSize; i < newAddressSize; ++i )
		{
			node = ecNodeWithID("A"+QString::number(i));
			m_aLogic.insert( i, createLogicIn(node) );
			m_aLogic[i]->setCallback( this, (CallbackPtr)(&Demultiplexer::inStateChanged) );
		}
	}
	else
	{
		for ( unsigned i = newXLogicCount; i < oldXLogicCount; ++i )
		{
			QString id = "X"+QString::number(i);
			removeDisplayText(id);
			removeElement( m_xLogic[i], false );
			removeNode(id);
		}
		m_xLogic.resize(newXLogicCount);
		
		for ( unsigned i = newAddressSize; i < oldAddressSize; ++i )
		{
			QString id = "A"+QString::number(i);
			removeDisplayText(id);
			removeElement( m_aLogic[i], false );
			removeNode(id);
		}
		m_aLogic.resize(newAddressSize);
	}
}

