/***************************************************************************
 *   Copyright (C) 2005 by David Saxton                                    *
 *   david@bluehaze.org                                                    *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#include "itemdocument.h"
#include "itemdocumentdata.h"
#include "drawpart.h"
#include "variant.h"

#include <klocale.h>
#include <Qt/qbitarray.h>

DrawPart::DrawPart( ItemDocument *itemDocument, bool newItem, const char *id )
	: Item( itemDocument, newItem, id )
{
	if ( itemDocument )
		itemDocument->registerItem(this);
}


DrawPart::~DrawPart()
{
}


Variant * DrawPart::createProperty( const QString & id, Variant::Type::Value type )
{
	if ( type == Variant::Type::PenStyle )
	{
		QStringList penStyles;
		penStyles << DrawPart::penStyleToName(Qt::SolidLine) << DrawPart::penStyleToName(Qt::DashLine)
				<< DrawPart::penStyleToName(Qt::DotLine) << DrawPart::penStyleToName(Qt::DashDotLine)
				<< DrawPart::penStyleToName(Qt::DashDotDotLine);
	
		Variant * v = createProperty( id, Variant::Type::String );
		v->setType( Variant::Type::PenStyle );
		v->setAllowed(penStyles);
		return v;
	}
	
	if ( type == Variant::Type::PenCapStyle )
	{
		QStringList penCapStyles;
		penCapStyles << DrawPart::penCapStyleToName(Qt::FlatCap) << DrawPart::penCapStyleToName(Qt::SquareCap)
				<< DrawPart::penCapStyleToName(Qt::RoundCap);
		
		Variant * v = createProperty( id, Variant::Type::String );
		v->setType( Variant::Type::PenCapStyle );
		v->setAllowed(penCapStyles);
		return v;
	}
	
	return Item::createProperty( id, type );
}


Qt::PenStyle DrawPart::getDataPenStyle( const QString & id )
{
	return nameToPenStyle( dataString(id) );
}
Qt::PenCapStyle DrawPart::getDataPenCapStyle( const QString & id )
{
	return nameToPenCapStyle( dataString(id) );
}
void DrawPart::setDataPenStyle( const QString & id, Qt::PenStyle value )
{
	property(id)->setValue( penStyleToName(value) );
}
void DrawPart::setDataPenCapStyle( const QString & id, Qt::PenCapStyle value )
{
	property(id)->setValue( penCapStyleToName(value) );
}


ItemData DrawPart::itemData() const
{
	ItemData itemData = Item::itemData();
	
	const VariantDataMap::const_iterator end = m_variantData.end();
	for ( VariantDataMap::const_iterator it = m_variantData.begin(); it != end; ++it )
	{
		switch( it.data()->type() )
		{
			case Variant::Type::PenStyle:
				itemData.dataString[it.key()] = penStyleToID( nameToPenStyle( it.data()->value().toString() ) );
				break;
			case Variant::Type::PenCapStyle:
				itemData.dataString[it.key()] = penCapStyleToID( nameToPenCapStyle( it.data()->value().toString() ) );
				break;
			case Variant::Type::String:
			case Variant::Type::FileName:
			case Variant::Type::Port:
			case Variant::Type::Pin:
			case Variant::Type::VarName:
			case Variant::Type::Combo:
			case Variant::Type::Select:
			case Variant::Type::Multiline:
			case Variant::Type::RichText:
			case Variant::Type::Int:
			case Variant::Type::Double:
			case Variant::Type::Color:
			case Variant::Type::Bool:
			case Variant::Type::Raw:
			case Variant::Type::SevenSegment:
			case Variant::Type::KeyPad:
			case Variant::Type::None:
				// All of these are handled by Item
				break;
		}
	}
	
	return itemData;
}


void DrawPart::restoreFromItemData( const ItemData &itemData )
{
	Item::restoreFromItemData(itemData);
	
	const QStringMap::const_iterator stringEnd = itemData.dataString.end();
	for ( QStringMap::const_iterator it = itemData.dataString.begin(); it != stringEnd; ++it )
	{
		VariantDataMap::iterator vit = m_variantData.find(it.key());
		if ( vit == m_variantData.end() )
			continue;
		
		if ( vit.data()->type() == Variant::Type::PenStyle )
			setDataPenStyle( it.key(), idToPenStyle( it.data() ) );
		
		else if ( vit.data()->type() == Variant::Type::PenCapStyle )
			setDataPenCapStyle( it.key(), idToPenCapStyle( it.data() ) );
	}
}



QString DrawPart::penStyleToID( Qt::PenStyle style )
{
	switch (style)
	{
		case Qt::SolidLine:
			return "SolidLine";
		case Qt::NoPen:
			return "NoPen";
		case Qt::DashLine:
			return "DashLine";
		case Qt::DotLine:
			return "DotLine";
		case Qt::DashDotLine:
			return "DashDotLine";
		case Qt::DashDotDotLine:
			return "DashDotDotLine";
		case Qt::MPenStyle:
		default:
			return ""; // ?!
	}
}
Qt::PenStyle DrawPart::idToPenStyle( const QString & id )
{
	if ( id == "NoPen" )
		return Qt::NoPen;
	if ( id == "DashLine" )
		return Qt::DashLine;
	if ( id == "DotLine" )
		return Qt::DotLine;
	if ( id == "DashDotLine" )
		return Qt::DashDotLine;
	if ( id == "DashDotDotLine" )
		return Qt::DashDotDotLine;
	return Qt::SolidLine;
}
QString DrawPart::penCapStyleToID( Qt::PenCapStyle style )
{
	switch (style)
	{
		case Qt::FlatCap:
			return "FlatCap";
		case Qt::SquareCap:
			return "SquareCap";
		case Qt::RoundCap:
			return "RoundCap";
		case Qt::MPenCapStyle:
		default:
			return ""; // ?!
	}
}
Qt::PenCapStyle DrawPart::idToPenCapStyle( const QString & id )
{
	if ( id == "SquareCap" )
		return Qt::SquareCap;
	if ( id == "RoundCap" )
		return Qt::RoundCap;
	return Qt::FlatCap;
}

QString DrawPart::penStyleToName( Qt::PenStyle style )
{
	switch (style)
	{
		case Qt::SolidLine:
			return i18n("Solid");
		case Qt::NoPen:
			return i18n("None");
		case Qt::DashLine:
			return i18n("Dash");
		case Qt::DotLine:
			return i18n("Dot");
		case Qt::DashDotLine:
			return i18n("Dash Dot");
		case Qt::DashDotDotLine:
			return i18n("Dash Dot Dot");
		case Qt::MPenStyle:
		default:
			return ""; // ?!
	}
}
Qt::PenStyle DrawPart::nameToPenStyle( const QString & name )
{
	if ( name == i18n("None") )
		return Qt::NoPen;
	if ( name == i18n("Dash") )
		return Qt::DashLine;
	if ( name == i18n("Dot") )
		return Qt::DotLine;
	if ( name == i18n("Dash Dot") )
		return Qt::DashDotLine;
	if ( name == i18n("Dash Dot Dot") )
		return Qt::DashDotDotLine;
	return Qt::SolidLine;
}
QString DrawPart::penCapStyleToName( Qt::PenCapStyle style )
{
	switch (style)
	{
		case Qt::FlatCap:
			return i18n("Flat");
		case Qt::SquareCap:
			return i18n("Square");
		case Qt::RoundCap:
			return i18n("Round");
		case Qt::MPenCapStyle:
		default:
			return ""; // ?!
	}
}
Qt::PenCapStyle DrawPart::nameToPenCapStyle( const QString & name )
{
	if ( name == i18n("Square") )
		return Qt::SquareCap;
	if ( name == i18n("Round") )
		return Qt::RoundCap;
	return Qt::FlatCap;
}

