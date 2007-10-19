/***************************************************************************
 *   Copyright (C) 2003-2005 by David Saxton                               *
 *   david@bluehaze.org                                                    *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#include "dptext.h"
#include "itemdocument.h"
#include "libraryitem.h"
#include "resizeoverlay.h"

#include <kiconloader.h>
#include <klocale.h>
#include <qpainter.h>

Item* DPText::construct( ItemDocument *itemDocument, bool newItem, const char *id)
{
	return new DPText( itemDocument, newItem, id);
}

LibraryItem* DPText::libraryItem()
{
	QStringList idList;
	idList << "dp/text" << "dp/canvas_text" << "canvas_text";
	
	return new LibraryItem(
		idList,
		i18n("Canvas Text"),
		i18n("Other"),
		KGlobal::iconLoader()->loadIcon( "text", KIcon::Small),
		LibraryItem::lit_drawpart,
		DPText::construct);
}

DPText::DPText( ItemDocument *itemDocument, bool newItem, const char *id)
	: DrawPart( itemDocument, newItem, (id) ? id : "canvas_text")
{
	m_rectangularOverlay = new RectangularOverlay(this);
	m_name = i18n("Text");
	m_desc = i18n("Doubleclick the Text Item to set the text");
	
	createProperty( "text", Variant::Type::Multiline);
	property("text")->setValue( i18n("Text"));
	
	createProperty( "background", Variant::Type::Bool);
	property("background")->setValue(false);
	property("background")->setCaption( i18n("Display Background"));
	property("background")->setAdvanced(true);
	
	createProperty( "background-color", Variant::Type::Color);
	property("background-color")->setValue(Qt::white);
	property("background-color")->setCaption( i18n("Background Color"));
	property("background-color")->setAdvanced(true);
	
	createProperty( "frame-color", Variant::Type::Color);
	property("frame-color")->setValue(Qt::black);
	property("frame-color")->setCaption( i18n("Frame Color"));
	property("frame-color")->setAdvanced(true);
	
	createProperty( "text-color", Variant::Type::Color);
	property("text-color")->setValue(Qt::black);
	property("text-color")->setCaption( i18n("Text Color"));
}

DPText::~DPText()
{
}

void DPText::setSelected( bool yes)
{
	if( yes == isSelected())
		return;
	
	DrawPart::setSelected(yes);
	m_rectangularOverlay->showResizeHandles(yes);
}


void DPText::dataChanged()
{
	m_caption = dataString("text");
	b_displayBackground = dataBool("background");
	m_backgroundColor = dataColor("background-color");
	m_textColor = dataColor("text-color");
	m_frameColor = dataColor("frame-color");
	update();
}


void DPText::postResize()
{
	setItemPoints( QPointArray(m_sizeRect), false);
}


QSize DPText::minimumSize() const
{
	return QSize( 48, 24);
}


void DPText::drawShape( QPainter &p)
{
	QRect bound = m_sizeRect;
	bound.setWidth( bound.width()-2);
	bound.setHeight( bound.height()-2);
	bound.moveBy( int(x()+1), int(y()+1));
	
	if(b_displayBackground)
	{
		p.save();
		p.setPen( QPen( m_frameColor, 1, Qt::DotLine));
		p.setBrush(m_backgroundColor);
		p.drawRect(bound);
		p.restore();
	}
	
	const int pad = 6;
	
	bound.setLeft( bound.left()+pad);
	bound.setTop( bound.top()+pad);
	bound.setRight( bound.right()-pad);
	bound.setBottom( bound.bottom()-pad);
	
	p.setPen(m_textColor);
	p.setFont( font());
	p.drawText( bound, (Qt::WordBreak | Qt::AlignHCenter | Qt::AlignVCenter), m_caption);
}

