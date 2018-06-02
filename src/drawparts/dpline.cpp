/***************************************************************************
 *   Copyright (C) 2005,2006 by David Saxton                               *
 *   david@bluehaze.org                                                    *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#include "dpline.h"
#include "libraryitem.h"
#include "resizeoverlay.h"
#include "variant.h"

#include <cmath>
#include <cstdlib> 
#include <kiconloader.h>
#include <klocalizedstring.h>
#include <qpainter.h>


//BEGIN class DPLine
Item* DPLine::construct( ItemDocument *itemDocument, bool newItem, const char *id )
{
	return new DPLine( itemDocument, newItem, id );
}

LibraryItem* DPLine::libraryItem()
{
	return new LibraryItem(
		QStringList(QString("dp/line")),
		i18n("Line"),
		i18n("Other"),
		KIconLoader::global()->loadIcon( "text", KIconLoader::Small ),
		LibraryItem::lit_drawpart,
		DPLine::construct );
}

DPLine::DPLine( ItemDocument *itemDocument, bool newItem, const char *id )
	: DrawPart( itemDocument, newItem, id ? id : "line" )
{
	m_pLineOverlay = new LineOverlay(this);
	m_name = i18n("Line");
	
	createProperty( "line-color", Variant::Type::Color );
	property("line-color")->setCaption( i18n("Line Color") );
	property("line-color")->setValue(Qt::black);
	
	createProperty( "line-width", Variant::Type::Int );
	property("line-width")->setCaption( i18n("Line Width") );
	property("line-width")->setMinValue(1);
	property("line-width")->setMaxValue(1000);
	property("line-width")->setValue(1);
	
	createProperty( "line-style", Variant::Type::PenStyle );
	property("line-style")->setCaption( i18n("Line Style") );
	property("line-style")->setAdvanced(true);
	setDataPenStyle( "line-style", Qt::SolidLine );
	
	createProperty( "cap-style", Variant::Type::PenCapStyle );
	property("cap-style")->setCaption( i18n("Cap Style") );
	property("cap-style")->setAdvanced(true);
	setDataPenCapStyle( "cap-style", Qt::FlatCap );
}

DPLine::~DPLine()
{
}

void DPLine::setSelected( bool yes )
{
	if ( yes == isSelected() )
		return;
	
	DrawPart::setSelected(yes);
	m_pLineOverlay->showResizeHandles(yes);
}


void DPLine::dataChanged()
{
	setPen( QPen( dataColor("line-color"),
			unsigned( dataInt("line-width") ),
			getDataPenStyle("line-style"),
			getDataPenCapStyle("cap-style"),
			Qt::MiterJoin ) );
	
	postResize(); // in case the pen width has changed
	update();
}


void DPLine::postResize()
{
	int x1 = offsetX();
	int y1 = offsetY();
	int x2 = x1+width();
	int y2 = y1+height();
	
	QPolygon p(4);
	int pw = pen().width();
	int dx = abs(x1-x2);
	int dy = abs(y1-y2);
	pw = pw * 4 / 3 + 2; // approx pw*sqrt(2) 
	int px = x1<x2 ? -pw : pw ;
	int py = y1<y2 ? -pw : pw ;
	if ( dx && dy && (dx > dy ? (dx*2/dy <= 2) : (dy*2/dx <= 2)) ) {
	// steep
		if ( px == py ) {
			p[0] = QPoint(x1   ,y1+py);
			p[1] = QPoint(x2-px,y2   );
			p[2] = QPoint(x2   ,y2-py);
			p[3] = QPoint(x1+px,y1   );
		} else {
			p[0] = QPoint(x1+px,y1   );
			p[1] = QPoint(x2   ,y2-py);
			p[2] = QPoint(x2-px,y2   );
			p[3] = QPoint(x1   ,y1+py);
		}
	} else if ( dx > dy ) {
	// horizontal
		p[0] = QPoint(x1+px,y1+py);
		p[1] = QPoint(x2-px,y2+py);
		p[2] = QPoint(x2-px,y2-py);
		p[3] = QPoint(x1+px,y1-py);
	} else {
	// vertical
		p[0] = QPoint(x1+px,y1+py);
		p[1] = QPoint(x2+px,y2-py);
		p[2] = QPoint(x2-px,y2-py);
		p[3] = QPoint(x1-px,y1+py);
	}
	setItemPoints( p, false );
}


void DPLine::drawShape( QPainter & p )
{
	int x1 = int(x()+offsetX());
	int y1 = int(y()+offsetY());
	int x2 = x1+width();
	int y2 = y1+height();
	
	p.drawLine( x1, y1, x2, y2 );
}
//END class DPLine


//BEGIN class DPArrow
Item* DPArrow::construct( ItemDocument *itemDocument, bool newItem, const char *id )
{
	return new DPArrow( itemDocument, newItem, id );
}

LibraryItem* DPArrow::libraryItem()
{
	return new LibraryItem(
		QStringList(QString("dp/arrow")),
		i18n("Arrow"),
		i18n("Other"),
		KIconLoader::global()->loadIcon( "text", KIconLoader::Small ),
		LibraryItem::lit_drawpart,
		DPArrow::construct );
}

DPArrow::DPArrow( ItemDocument *itemDocument, bool newItem, const char *id )
	: DPLine( itemDocument, newItem, id ? id : "arrow" )
{
	m_name = i18n("Arrow");
	
	// We don't want to use the square cap style as it screws up drawing our arrow head
	QStringList allowed = property("cap-style")->allowed();
	allowed.remove( DrawPart::penCapStyleToName( Qt::SquareCap ) );
	property("cap-style")->setAllowed(allowed);
	
	m_headAngle = 20.0;
	Variant * v = createProperty( "HeadAngle", Variant::Type::Double );
	v->setAdvanced( true );
	v->setCaption( i18n("Head angle") );
	v->setMinValue( 10.0 );
	v->setMaxValue( 60.0 );
	v->setUnit( QChar(0xb0) );
	v->setValue( m_headAngle );
}


DPArrow::~DPArrow()
{
}


void DPArrow::dataChanged()
{
	DPLine::dataChanged();
	m_headAngle = dataDouble( "HeadAngle" );
	setChanged();
}


inline int round_x( double x ) { return int(x+((x > 0) ? 0.5 : -0.5)); }

void DPArrow::drawShape( QPainter & p )
{
	double x1 = x()+offsetX();
	double y1 = y()+offsetY();
	double x2 = x1+width();
	double y2 = y1+height();
	
	p.drawLine( int(x1), int(y1), int(x2), int(y2) );
	
	double dx = x2-x1;
	double dy = y2-y1;
	
	if ( dx == 0. && dy == 0. )
		return;
	
	double arrow_angle = ( dx == 0 ? (dy>0?(M_PI_2):(-M_PI_2)) : std::atan(dy/dx) );
	if ( dx < 0 )
		arrow_angle += M_PI;
	
	double head_angle = M_PI * m_headAngle / 180.0;
	double head_length = 10.0;
	
	// Position of arrowhead
	double x3 = x2 + head_length*std::cos( M_PI + arrow_angle - head_angle );
	double y3 = y2 + head_length*std::sin( M_PI + arrow_angle - head_angle );
	double x4 = x2 + head_length*std::cos( M_PI + arrow_angle + head_angle );
	double y4 = y2 + head_length*std::sin( M_PI + arrow_angle + head_angle );
	
	// Draw arrowhead
	QPen pen = p.pen();
	pen.setCapStyle( Qt::RoundCap );
	p.setPen(pen);
	p.setBrush(pen.color());
	QPolygon pa(3);
	pa[0] = QPoint( round_x(x2), round_x(y2) );
	pa[1] = QPoint( round_x(x3), round_x(y3) );
	pa[2] = QPoint( round_x(x4), round_x(y4) );
	p.drawPolygon(pa);
	p.drawPolyline(pa);
}
//END class DPLine

