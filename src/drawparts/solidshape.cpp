/***************************************************************************
 *   Copyright (C) 2005 by David Saxton                                    *
 *   david@bluehaze.org                                                    *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#include "solidshape.h"
#include "libraryitem.h"
#include "resizeoverlay.h"

#include <cmath>
#include <KIconLoader>
#include <KLocalizedString>
#include <qpainter.h>


//BEGIN class DPRectangle
Item * DPRectangle::construct( ItemDocument *itemDocument, bool newItem, const char *id )
{
	return new DPRectangle( itemDocument, newItem, id );
}

LibraryItem* DPRectangle::libraryItem()
{
	return new LibraryItem(
			QStringList(QString("dp/rectangle")),
	i18n("Rectangle"),
	i18n("Other"),
	KIconLoader::global()->loadIcon( "text", KIconLoader::Small ),
	LibraryItem::lit_drawpart,
	DPRectangle::construct );
}

DPRectangle::DPRectangle( ItemDocument *itemDocument, bool newItem, const char *id )
	: DrawPart( itemDocument, newItem, id ? id : "rectangle" )
{
	m_pRectangularOverlay = new RectangularOverlay(this);
	m_name = i18n("Rectangle");
	
	createProperty( "background", Variant::Type::Bool );
	property("background")->setValue(false);
	property("background")->setCaption( i18n("Display Background") );
	property("background")->setAdvanced(true);
	
	createProperty( "background-color", Variant::Type::Color );
	property("background-color")->setValue(QColor(Qt::white));
	property("background-color")->setCaption( i18n("Background Color") );
	property("background-color")->setAdvanced(true);
	
	createProperty( "line-color", Variant::Type::Color );
	property("line-color")->setValue(QColor(Qt::black));
	property("line-color")->setCaption( i18n("Line Color") );
	property("line-color")->setAdvanced(true);
	
	createProperty( "line-width", Variant::Type::Int );
	property("line-width")->setCaption( i18n("Line Width") );
	property("line-width")->setMinValue(1);
	property("line-width")->setMaxValue(1000);
	property("line-width")->setValue(1);
	property("line-width")->setAdvanced(true);
	
	createProperty( "line-style", Variant::Type::PenStyle );
	property("line-style")->setAdvanced(true);
	property("line-style")->setCaption( i18n("Line Style") );
	setDataPenStyle( "line-style", Qt::SolidLine );
}

DPRectangle::~DPRectangle()
{
}

void DPRectangle::setSelected( bool yes )
{
	if ( yes == isSelected() )
		return;
	
	DrawPart::setSelected(yes);
	m_pRectangularOverlay->showResizeHandles(yes);
}


void DPRectangle::dataChanged()
{
	bool displayBackground = dataBool("background");
	QColor line_color = dataColor("line-color");
	unsigned width = unsigned( dataInt("line-width") );
	Qt::PenStyle style = getDataPenStyle("line-style");
	
	setPen( QPen( line_color, width, style ) );
	
	if (displayBackground)
		setBrush( dataColor("background-color") );
	else
		setBrush( Qt::NoBrush );
	
	postResize();
	update();
}


QSize DPRectangle::minimumSize() const
{
	int side = qMax(16, pen().width()+2);
	return QSize( side, side );
}


void DPRectangle::postResize()
{
	setItemPoints( m_sizeRect, false );
}


QRect DPRectangle::drawRect() const
{
	int lw = pen().width();
	
	if ( lw > m_sizeRect.width() )
		lw = m_sizeRect.width();
	
	if ( lw > m_sizeRect.height() )
		lw = m_sizeRect.height();
	
	return QRect( int(x() + m_sizeRect.x()+lw/2), int(y() + m_sizeRect.y()+lw/2),
				  m_sizeRect.width()-lw, m_sizeRect.height()-lw );
}


void DPRectangle::drawShape( QPainter & p )
{
	p.drawRect(drawRect());
}
//END class DPRectangle


//BEGIN class DPEllipse
Item * DPEllipse::construct( ItemDocument *itemDocument, bool newItem, const char *id )
{
	return new DPEllipse( itemDocument, newItem, id );
}

LibraryItem* DPEllipse::libraryItem()
{
	return new LibraryItem(
			QStringList(QString("dp/ellipse")),
	i18n("Ellipse"),
	i18n("Other"),
	KIconLoader::global()->loadIcon( "text", KIconLoader::Small ),
	LibraryItem::lit_drawpart,
	DPEllipse::construct );
}

DPEllipse::DPEllipse( ItemDocument *itemDocument, bool newItem, const char *id )
	: DPRectangle( itemDocument, newItem, id ? id : "ellipse" )
{
	m_name = i18n("Ellipse");
}

DPEllipse::~DPEllipse()
{
}


void DPEllipse::postResize()
{
	QRect br = m_sizeRect;
	
	// Make octagon that roughly covers ellipse
	QPolygon pa(8);
	pa[0] = QPoint( br.x() + br.width()/4,		br.y() );
	pa[1] = QPoint( br.x() + 3*br.width()/4,	br.y() );
	pa[2] = QPoint( br.x() + br.width(),		br.y() + br.height()/4 );
	pa[3] = QPoint( br.x() + br.width(),		br.y() + 3*br.height()/4 );
	pa[4] = QPoint( br.x() + 3*br.width()/4,	br.y() + br.height() );
	pa[5] = QPoint( br.x() + br.width()/4,		br.y() + br.height() );
	pa[6] = QPoint( br.x(),						br.y() + 3*br.height()/4 );
	pa[7] = QPoint( br.x(),						br.y() + br.height()/4 );
	
	setItemPoints( pa, false );
}


void DPEllipse::drawShape( QPainter & p )
{
	p.drawEllipse(drawRect());
}
//END class SolidShape


