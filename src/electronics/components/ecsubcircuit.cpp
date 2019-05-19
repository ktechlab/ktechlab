/***************************************************************************
 *   Copyright (C) 2005 by David Saxton                                    *
 *   david@bluehaze.org                                                    *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#include "canvasitemparts.h"
#include "circuitdocument.h"
#include "ecsubcircuit.h"
#include "node.h"
#include "libraryitem.h"
#include "subcircuits.h"

#include <qdebug.h>
#include <klocalizedstring.h>
#include <qfile.h>

Item* ECSubcircuit::construct( ItemDocument *itemDocument, bool newItem, const char *id )
{
	return new ECSubcircuit( (ICNDocument*)itemDocument, newItem, id );
}

LibraryItem* ECSubcircuit::libraryItem()
{
	return new LibraryItem(
		QStringList(QString("ec/subcircuit")),
		QString::null,
		QString::null,
		QString::null,
		LibraryItem::lit_subcircuit,
		ECSubcircuit::construct );
}

ECSubcircuit::ECSubcircuit( ICNDocument *icnDocument, bool newItem, const char *id )
	: Component( icnDocument, newItem, (id) ? id : "subcircuit" )
{
	m_name = i18n("Subcircuit");
	
	createProperty( "id", Variant::Type::Int );
	property("id")->setMinValue(1);
	property("id")->setMaxValue(1<<15);
	property("id")->setValue(1);
	property("id")->setHidden(true);
}


ECSubcircuit::~ECSubcircuit()
{
}


void ECSubcircuit::removeItem()
{
	emit subcircuitDeleted();
	Component::removeItem();
}


void ECSubcircuit::setNumExtCon( unsigned numExtCon )
{
	m_conNames.resize(numExtCon);

	// Remove old pins
	const NodeInfoMap::iterator nodeMapEnd = m_nodeMap.end();
	for ( NodeInfoMap::iterator it = m_nodeMap.begin(); it != nodeMapEnd; ++it )
	{
		p_icnDocument->appendDeleteList( p_icnDocument->nodeWithID(it.value().id) );
	}
	p_icnDocument->flushDeleteList();
	m_nodeMap.clear();
	
	QStringList pins;
	for ( unsigned i=0; i<numExtCon; ++i )
	{
		pins += QString::number(i);
	}
	
	initDIPSymbol( pins, 80 );
	
	// We don't want the text that the dip symbol gave us as we initialize it later...
	for ( unsigned i = 0; i < numExtCon; ++i )
		removeDisplayText( QString::number(i) );
	
	initDIP(pins);
}


void ECSubcircuit::dataChanged()
{
	int subcircuitId = dataInt("id");
	if ( subcircuitId == -1 ) {
		return;
	}
	emit subcircuitDeleted();
	Subcircuits::initECSubcircuit( subcircuitId, this );
}


void ECSubcircuit::setExtConName( unsigned numId, const QString & name )
{
	if ( numId > m_conNames.size() )
		return;
	
	m_conNames[numId] = name;
}


void ECSubcircuit::doneSCInit()
{
	QStringList pins;
	for ( unsigned i = 0; i < m_conNames.size(); ++i )
		pins << m_conNames[i];
	initDIPSymbol( pins, 80 );
}


void ECSubcircuit::drawShape( QPainter &p )
{
	Component::drawShape(p);
}


#include "ecsubcircuit.moc"



