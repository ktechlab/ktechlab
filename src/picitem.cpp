/***************************************************************************
 *   Copyright (C) 2003-2005 by David Saxton                               *
 *   david@bluehaze.org                                                    *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#include "canvasitemparts.h"
#include "flowcodedocument.h"
#include "microinfo.h"
#include "microsettings.h"
#include "microsettingsdlg.h"
#include "micropackage.h"
#include "picitem.h"
#include "eventinfo.h"

#include <qdebug.h>
#include <klocalizedstring.h>

#include <qicon.h>
#include <qpainter.h>

static const int InnerWidth = 88;
static const int SidePadding = 24;
static const int TopPadding = 36;
static const int BottomPadding = 42;
static const int PinWidth = 12;
static const int PinSeparation = 16 - PinWidth;
static const int PinLength = 8;
static const int ArcWidth = 22;
static const int PinDirArrow = 3;


//BEGIN class PinItem
PinItem::PinItem( FlowCodeDocument* _view, QPoint position, bool _onLeft, PinSettings * pinSettings )
	: KtlQCanvasRectangle(nullptr)
{
	m_pinSettings = pinSettings;
	view = _view;
	onLeft = _onLeft;

	connect( m_pinSettings, SIGNAL(settingsChanged()), this, SLOT(updateDrawing()) );

	if ( QFontInfo(m_font).pixelSize() > 11 ) // It has to be > 11, not > 12, as (I think) pixelSize() rounds off the actual size
		m_font.setPixelSize(12);

	setCanvas( view->canvas() );

	move ( position.x(), position.y() );
	initItem();
	setZ( (ICNDocument::Z::RaisedItem + ICNDocument::Z::ResizeHandle)/2 + 1 ); // Hackish, but whatever
}


void PinItem::updateDrawing()
{
	update();
}


void PinItem::initItem()
{
	setSize( PinLength, PinWidth );
	setSelected(false);
	setPen( QPen( Qt::black ) );
	calcTextRect();
	show();
}


void PinItem::drawShape( QPainter& p )
{
	if (!m_pinSettings)
		return;

	if ( m_pinSettings->state() == PinSettings::ps_on )
	{
		if ( m_pinSettings->type() == PinSettings::pt_output )
			setBrush( QColor( 255, 127, 127 ) );
		else
			setBrush( QColor( 255, 191, 191 ) );
	}
	else
		setBrush( Qt::white );

	p.drawRect(rect());

	p.setFont(m_font);
	p.setBrush( Qt::NoBrush );
	QRect r = m_textRect;
	if ( onLeft )
		p.drawText( r, Qt::AlignLeft, m_pinSettings->id() );
	else
		p.drawText( r, Qt::AlignRight, m_pinSettings->id() );
	//QRect br = p.boundingRect( r, Qt::AlignLeft, m_pinSettings->id() ); // 2017.10.01 - comment out unused variable

	int left;
	int right;
	if ( onLeft )
	{
		right = (int)x();
		left = right - 8;
	}
	else
	{
		left = (int)x() + PinLength;
		right = left + 8;
	}

	int midY = (int)y() + PinWidth/2;
	QPolygon pa(3);
	int midLeft = left + (8-PinDirArrow)/2;
	int midRight = left + (8+PinDirArrow)/2;

	if ( onLeft )
	{
		midLeft--;
		midRight--;
	}
	else
	{
		midLeft++;
		midRight++;
	}

	p.setBrush( Qt::black );

	// Right facing arrow
	if ( (m_pinSettings->type() == PinSettings::pt_input && onLeft) ||
			 (m_pinSettings->type() == PinSettings::pt_output && !onLeft) )
	{
		pa[0] = QPoint( midRight, midY );
		pa[1] = QPoint( midLeft, midY - PinDirArrow );
		pa[2] = QPoint( midLeft, midY + PinDirArrow );
		p.drawPolygon(pa);
		p.drawLine ( left, midY, right, midY );
	}
	else // Left facing arrow
	{
		pa[0] = QPoint( midLeft, midY );
		pa[1] = QPoint( midRight, midY - PinDirArrow );
		pa[2] = QPoint( midRight, midY + PinDirArrow );
		p.drawPolygon(pa);
		p.drawLine ( left, midY, right, midY );
	}
}


QRect PinItem::boundingRect () const
{
	QRect r = m_textRect;
	if ( onLeft )
		r.setLeft( (int)x() - 10 );
	else
		r.setRight( (int)x() + PinLength + 10 );

	return r;
}


QString PinItem::id()
{
	return m_pinSettings->id();
}


void PinItem::switchState()
{
	if ( m_pinSettings->state() == PinSettings::ps_on )
		m_pinSettings->setState(PinSettings::ps_off);
	else
		m_pinSettings->setState(PinSettings::ps_on);

	update();
}


void PinItem::dragged( int dx )
{
	if ( (onLeft && dx > 0) ||
	     (!onLeft && dx < 0) )
	{
		m_pinSettings->setType(PinSettings::pt_input);
	}
	else
		m_pinSettings->setType(PinSettings::pt_output);

	update();
}


void PinItem::moveBy ( double dx, double dy )
{
	KtlQCanvasRectangle::moveBy( dx, dy );
	calcTextRect();
}


void PinItem::calcTextRect()
{
	m_textRect = rect();
	m_textRect.moveTop( m_textRect.top()-2 );
	QRect br;

// 	QWidget tmpWidget;
//     //tmpWidget.setAttribute(Qt::WA_PaintOutsidePaintEvent, true); //note: add this if needed
// 	//QPainter p(&tmpWidget); // 2016.05.03 - initialize painter explicitly
//     QPainter p;
//     const bool isBeginSuccess = p.begin(&tmpWidget);
//     {
//         qWarning() << Q_FUNC_INFO << " painter not active";
//     }
//
// 	p.setFont(m_font);

    QFontMetrics fontMetrics( m_font );

	if (!m_pinSettings)
	{
		qDebug() << "PinItem::textRect: No pinSettings!"<<endl;
		return;
	}
	// note: br is assigned but not used; here might be some bug...
	if ( onLeft )
	{
		m_textRect.setLeft( (int)x() + PinLength + 2 );
		m_textRect.setRight( (int)x() + InnerWidth/2 );
		//br = p.boundingRect( m_textRect, Qt::AlignLeft, m_pinSettings->id() ); // 2016.05.03 - do not create dummy widget
        br = fontMetrics.boundingRect( m_textRect, Qt::AlignLeft, m_pinSettings->id() );
	}
	else
	{
		m_textRect.setLeft( m_textRect.right() - InnerWidth/2 );
		m_textRect.setRight( (int)x() - 2 );
		//br = p.boundingRect( m_textRect, Qt::AlignRight, m_pinSettings->id() ); // 2016.05.03 - do not create dummy widget
        br = fontMetrics.boundingRect( m_textRect, Qt::AlignRight, m_pinSettings->id() );
	}
}
//END class PinItem



//BEGIN class PicItem
PicItem::PicItem( ICNDocument *icnDocument, bool newItem, const char *id, MicroSettings *_microSettings )
	: CNItem( icnDocument, newItem, id ? id : "picitem" )
{
	m_name = "PIC";
	m_type = typeString();
	p_icnDocument = icnDocument;
	icnDocument->registerItem(this);

	microSettings = _microSettings;
	const int numPins = microSettings->microInfo()->package()->pinCount( PicPin::type_bidir | PicPin::type_input | PicPin::type_open );
	const int numSide = (numPins/2) + (numPins%2);

	m_bExpanded = true;
	m_innerHeight = (numSide+2)*PinWidth + (numSide-1)*PinSeparation;
	updateVisibility();

	addButton( "settings", QRect( SidePadding-8, m_innerHeight+TopPadding+(BottomPadding-24)/2-1, InnerWidth+16, 24 ), i18n("Advanced...") );
	addButton( "expandBtn", QRect( (TopPadding-22)/2, (TopPadding-22)/2, 22, 22 ), QIcon::fromTheme( "go-down" ), true );
	button("expandBtn")->setState(true);

	move( 12, 12 );

	QStringList pinIDs = microSettings->microInfo()->package()->pinIDs( PicPin::type_bidir | PicPin::type_input | PicPin::type_open );
	QStringList::iterator it = pinIDs.begin();

	for ( int i=0; i < numSide; ++i, ++it )
	{
		QPoint position( int(this->x()) + SidePadding - PinLength+1, int(y()) + TopPadding + (i+1)*PinWidth + i*PinSeparation );
		const QString id = *it;
		PinSettings *settings = microSettings->pinWithID(id);
		m_pinItemList.append( new PinItem( dynamic_cast<FlowCodeDocument*>(icnDocument), position, true, settings ) );
	}

	for ( int i=0; i < numPins/2; ++i, ++it )
	{
		QPoint position( int(this->x()) + SidePadding + InnerWidth-1, int(y()) + TopPadding + m_innerHeight - ( (i+2)*PinWidth + i*PinSeparation ) );
		const QString id = *it;
		PinSettings *settings = microSettings->pinWithID(id);
		m_pinItemList.append( new PinItem( dynamic_cast<FlowCodeDocument*>(icnDocument), position, false, settings ) );
	}

	setSelected(false);
	setPen( QPen( Qt::black ) );
	updateZ(-1);
	update();
	show();
}


PicItem::~PicItem()
{
	const PinItemList::iterator end = m_pinItemList.end();
	for ( PinItemList::iterator it = m_pinItemList.begin(); it != end; ++it )
		delete *it;

	m_pinItemList.clear();
}


void PicItem::updateZ( int baseZ )
{
	(void)baseZ;
	setZ( (ICNDocument::Z::RaisedItem + ICNDocument::Z::ResizeHandle)/2 ); // Hackish, but whatever
	button("settings")->setZ( z()+1 );
	button("expandBtn")->setZ( z()+1 );
}


void PicItem::drawShape( QPainter & p )
{
	int _x = int(x());
	int _y = int(y());

	p.setBrush( QColor( 0xef, 0xff, 0xef ) );
	p.setFont( font() );

	p.drawRoundRect( _x, _y, width(), height(), 2000/width(), 2000/height() );

	p.drawText( _x+TopPadding-2, _y, width()-TopPadding+2, TopPadding, Qt::AlignVCenter, i18n("PIC Settings") );

	if ( !m_bExpanded )
		return;

	// Draw rectangle to cut off pins
	p.setBrush( QColor( 239, 255, 255 ) );
	QRect r( _x+SidePadding, _y+TopPadding, InnerWidth, m_innerHeight );
	p.drawRect(r);

	// Draw dimple thingy at end of pic
	p.drawArc( r.x()+(r.width()-ArcWidth)/2, r.y()+1-ArcWidth/2, ArcWidth, ArcWidth, 180*16, 180*16 );

	// Draw vertical text centered in PIC
	p.translate( r.width()/2 + r.x(), r.height()/2 + r.y() );
	p.rotate(90);
	QRect textRect( r.width()/-2, r.height()/-2, r.width(), r.height() );
	p.drawText( textRect, Qt::AlignCenter, microSettings->microInfo()->id() );

	p.rotate(-90);
	p.translate( r.width()/-2 - r.x(), r.height()/-2 - r.y() );
}


void PicItem::buttonStateChanged( const QString &id, bool state )
{
	if ( id == "expandBtn" )
	{
		m_bExpanded = state;
		updateVisibility();
	}

	else if ( id == "settings" )
	{
		if (!state)
			return;

		// Redraw button
		button("settings")->setState(false);
		update();

		MicroSettingsDlg *dlg = new MicroSettingsDlg( microSettings, nullptr, "microSettingsDlg" );
		connect( dlg, &MicroSettingsDlg::accepted, this, &PicItem::slotMicroSettingsDlgAccepted);
		connect( dlg, &MicroSettingsDlg::applyClicked, this, &PicItem::slotMicroSettingsDlgAccepted);
		dlg->show();
		// At this point the PIC is selected but this does not appear to the
		// user so we must deselect it when done.
		p_icnDocument->unselectAll();
	}
}


void PicItem::updateVisibility()
{
	if (m_bExpanded)
		setSize( 0, 0, InnerWidth+(2*SidePadding), m_innerHeight+TopPadding+BottomPadding, true );

	else
		setSize( 0, 0, InnerWidth+(2*SidePadding), TopPadding, true );

	const PinItemList::iterator end = m_pinItemList.end();
	for ( PinItemList::iterator it = m_pinItemList.begin(); it != end; ++it )
		(*it)->setVisible(m_bExpanded);

	if ( Button * btn = button("settings") )
		btn->setVisible(m_bExpanded);
}


void PicItem::slotMicroSettingsDlgAccepted()
{
	const PinItemList::iterator end = m_pinItemList.end();
	for ( PinItemList::iterator it = m_pinItemList.begin(); it != end; ++it )
		canvas()->setChanged( (*it)->boundingRect() );

	p_icnDocument->requestStateSave();
}

bool PicItem::mousePressEvent( const EventInfo &info )
{
	QMouseEvent *e = info.mousePressEvent( 0, 0 );
	const PinItemList::iterator end = m_pinItemList.end();
	for ( PinItemList::iterator it = m_pinItemList.begin(); it != end; ++it )
		if ( e->isAccepted() && (*it)->boundingRect().contains( info.pos ) ) {
			//reset mouse-gesture state
			m_pressed = true;
			m_pos = info.pos;
			m_dragged = false;
			m_dx = 0;
			delete e;
			return true;
		}

	m_pressed = false;
	delete e;
	return CNItem::mousePressEvent( info );
}

bool PicItem::mouseReleaseEvent( const EventInfo &info )
{
	QMouseEvent *e = info.mouseReleaseEvent( 0, 0 );
	if ( !m_pressed ) {
		delete e;
		return CNItem::mouseReleaseEvent( info );
	}

	const PinItemList::iterator end = m_pinItemList.end();
	for ( PinItemList::iterator it = m_pinItemList.begin(); it != end; ++it )
		if ( !e->isAccepted() ) {
			continue;
		} else if ( (*it)->boundingRect().contains(m_pos) && (*it)->boundingRect().contains( info.pos ) ) {
			if ( m_dragged ) {
				(*it)->dragged( m_dx );
			} else {
				(*it)->switchState();
			}
			m_pressed = false;
			m_dragged = false;
			m_pos = QPoint();
			m_dx = 0;
			delete e;
			return true;
		}
	delete e;
	return CNItem::mouseReleaseEvent( info );
}

bool PicItem::mouseMoveEvent( const EventInfo &info )
{
	QMouseEvent *e = info.mouseMoveEvent( 0, 0 );
	const PinItemList::iterator end = m_pinItemList.end();
	for ( PinItemList::iterator it = m_pinItemList.begin(); it != end; ++it )
		if ( e->isAccepted() && (*it)->boundingRect().contains( info.pos ) ) {
			QPoint vec = info.pos - m_pos;
			if ( m_pressed && vec.manhattanLength() > 4 ) {
				m_dragged = true;
				m_dx = vec.x();
        		}
			delete e;
			return true;
		}
	delete e;
	return CNItem::mouseMoveEvent( info );
}

//END class PicItem
